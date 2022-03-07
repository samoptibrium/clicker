#include "controlswindow.h"
#include "ui_controlswindows.h"

#include <cassert>
#include <memory>
#include <optional>

#include <QTime>
#include <QTimer>

#include "bindings.h"
#include "capturearea.h"
#include "captureareawindow.h"
#include "scene.h"
#include "qhotkey.h"
#include "settings.h"

namespace clickeroo::ui
{

class ControlsWindow::ControlsWindowImpl
{
public:
    ControlsWindowImpl(ControlsWindow* pQ) : q{pQ}, ui(std::make_unique<Ui::ControlsWindow>())
    {
        q->setObjectName("controlswindow");
        q->setWindowFlags(Qt::WindowStaysOnTopHint);
        q->setAttribute(Qt::WA_QuitOnClose, true);

        ui->setupUi(q);

        ui->startStopButton->setObjectName("startStopButton");
        ui->clearLogButton->setObjectName("clearLogButton");

        ui->sceneView->setScene(scene);

        q->connect(ui->startStopButton, &QPushButton::pressed, [this]() {
            setRunning(!running);
        });

        screengrabTimer = new QTimer(q);
        connect(screengrabTimer, &QTimer::timeout, [this]() {
            onScreengrabRequested();
        });

        scriptSaverTimer = new QTimer(q);
        connect(scriptSaverTimer, &QTimer::timeout, [this]() {
            for(const auto& editor : scriptEditors) {
                clickeroo::writeFile(editor.first, editor.second->toPlainText());
            }
        });

        scriptRunnerTimer = new QTimer(q);
        connect(scriptRunnerTimer, &QTimer::timeout, [this]() {
            if(!running) {
                return;
            }

            for(const auto& editor : scriptEditors) {
                try {
                    clickeroo::run(editor.second->toPlainText().toStdString());
                } catch(const std::exception& e) {
                    log(std::string("Error: ") + e.what());
                }
            }
        });

        connect(q, &clickeroo::ui::ControlsWindow::signal_captureAreaGrabbed, [this](QPixmap pixmap) {
            setCaptureAreaPreviewPixmap(pixmap);
        });

        // Simulate a stop button keypress if the shortcut is activated
        assert(stopHotKey.isRegistered());
        QObject::connect(&stopHotKey, &QHotkey::activated, q, [this]() {
            if(running) {
                emit ui->startStopButton->pressed();
            }
        });

        ui->addPresetButton->connect(ui->addPresetButton, &QPushButton::clicked, [this]() {
            const auto text = ui->presetNameEdit->text().toStdString();
            if(!text.empty()) {
                addPreset(clickeroo::getPresetPath(text));
            }
        });
        ui->removePresetButton->connect(ui->removePresetButton, &QPushButton::clicked, [this]() {
            const auto text = ui->presetComboBox->currentText().toStdString();
            if(!text.empty()) {
                removePreset(clickeroo::getPresetPath(text));
            }
        });

        ui->refreshPresetsListButton->connect(ui->refreshPresetsListButton, &QPushButton::clicked, [this]() {
            clearScriptEditors();
            populateScriptSelectionComboBox();
        });

        ui->presetComboBox->connect(ui->presetComboBox, &QComboBox::currentIndexChanged, [this]() {
            const auto text = ui->presetComboBox->itemText(ui->presetComboBox->currentIndex()).toStdString();
            loadPreset(clickeroo::getPresetPath(text));
        });

        ui->clearGlobalsButton->connect(ui->clearGlobalsButton, &QPushButton::clicked, [this]() {
            ui->namedGlobalsList->clear();
        });

        ui->clearLogButton->connect(ui->clearLogButton, &QPushButton::clicked, [this]() {
            ui->logTextEdit->clear();
        });

        populateScriptSelectionComboBox();

        screengrabTimer->start(100);
        scriptSaverTimer->start(10000);
        scriptRunnerTimer->start(100);
    }

    ~ControlsWindowImpl()
    {
        screengrabTimer->stop();
        scriptSaverTimer->stop();
        scriptRunnerTimer->stop();
    }

    void setCaptureAreaWindow(clickeroo::ui::CaptureAreaWindow* window)
    {
        captureAreaWindow = window;
    }

    void setCaptureAreaPreviewPixmap(QPixmap pixmap)
    {
        scene->setCaptureGraphic(pixmap);
        ui->sceneView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }

    bool isRunning() const
    {
        return running;
    }

    void setRunning(bool running)
    {
        this->running = running;
        if(running) {
            ui->startStopButton->setText("Stop (" + stopKeySequence + ")");
            emit q->startPressed();
        } else {
            ui->startStopButton->setText("Start");
            emit q->stopPressed();
        }
    }

    void log(const std::string& s)
    {
        const QString now = QTime::currentTime().toString("hh:mm:ss");
        QString oldText = ui->logTextEdit->toPlainText();
        ui->logTextEdit->setPlainText(now + " - " + QString::fromStdString(s) + "\n" + oldText);
    }

    void setNamedGlobal(const std::string& s, const std::string& value)
    {
        // TODO
    }

private:
    void loadPreset(const std::filesystem::path& path)
    {
        clearScriptEditors();

        const auto scriptPaths = clickeroo::getScriptPaths(path);
        for(const auto& scriptPath : scriptPaths) {
            const QString script = clickeroo::readFile(scriptPath);

            auto* editor = new QTextEdit(ui->scriptsScrollerContents);
            editor->setPlainText(script);
            scriptEditors[scriptPath] = editor;
            ui->scriptEditorScrollerLayout->addWidget(editor);
        }
    }

    void populateScriptSelectionComboBox()
    {
        ui->presetComboBox->clear();

        const auto folders = clickeroo::getScriptFolders();
        for(const auto& folder : folders) {
            addPreset(folder);
        }
    }

    void addPreset(const std::filesystem::path& folder)
    {
        for(int i = 0; i < ui->presetComboBox->count(); i++) {
            if(ui->presetComboBox->itemText(i) == QString::fromWCharArray(folder.filename().c_str())) {
                return;
            }
        }

        try {
            if(!std::filesystem::exists(folder)) {
                std::filesystem::create_directories(folder);
            }
        } catch(...) {
           assert(0 && "Failed to create preset folder");
        }
        ui->presetComboBox->addItem(QString::fromWCharArray(folder.filename().c_str()));
    }

    void removePreset(const std::filesystem::path& folder)
    {
        try {
            if(std::filesystem::exists(folder)) {
                std::filesystem::remove_all(folder);
            }
        } catch(...) {
            assert(0 && "Failed to remove preset folder");
        }
        for(int i = 0; i < ui->presetComboBox->count(); i++) {
            if(ui->presetComboBox->itemText(i) == QString::fromWCharArray(folder.filename().c_str())) {
                ui->presetComboBox->removeItem(i);
                break;
            }
        }
    }

    void onScreengrabRequested()
    {
        const auto screen = captureAreaWindow->screen(); // NOTE this ties the capture area screen to the area rect
        const auto rect = clickeroo::getCaptureArea();
        const auto dpiScale = captureAreaWindow->devicePixelRatioF();
        const auto border = captureAreaWindow->getBorderSizePixels();
        const auto pixmap = screen->grabWindow(0, ((double)rect.x() / dpiScale) + border, ((double)rect.y() / dpiScale) + border,
                                               ((double)rect.width() / dpiScale) - border * 2, ((double)rect.height() / dpiScale) - border * 2);
        emit q->signal_captureAreaGrabbed(pixmap);
    }

    void clearScriptEditors()
    {
        QLayoutItem* child;
        while ((child = ui->scriptEditorScrollerLayout->takeAt(0)) != 0) {
            delete child->widget();
            delete child;
        }
        scriptEditors.clear();
    }

    std::optional<QString> getNamedScriptEditorContent(const QString& editorName)
    {
        for(const auto& editor : scriptEditors) {
            if(editor.second->objectName() == editorName) {
                return std::make_optional(editor.second->toPlainText());
            }
        }
        return std::nullopt;
    }

    ControlsWindow* q;
    std::unique_ptr<Ui::ControlsWindow> ui;

    bool running{ false };
    QString stopKeySequence{ "Ctrl+T" };
    QHotkey stopHotKey{ QKeySequence(stopKeySequence), true, q };
    std::map<std::filesystem::path, QTextEdit*> scriptEditors;

    QTimer* screengrabTimer;
    clickeroo::ui::CaptureAreaWindow* captureAreaWindow{ nullptr };
    clickeroo::Scene* scene{ new Scene(q) };

    QTimer* scriptSaverTimer;

    QTimer* scriptRunnerTimer;
};

ControlsWindow::ControlsWindow(QWidget* parent, clickeroo::ui::CaptureAreaWindow* window) : QMainWindow(parent), d{std::make_unique<ControlsWindow::ControlsWindowImpl>(this)}
{
    d->setCaptureAreaWindow(window);
}

ControlsWindow::~ControlsWindow()
{

}

void ControlsWindow::setCaptureAreaPreviewPixmap(QPixmap pixmap)
{
    d->setCaptureAreaPreviewPixmap(pixmap);
}

void ControlsWindow::closeEvent(QCloseEvent* event)
{
    QApplication::closeAllWindows();
    QMainWindow::closeEvent(event);
}

bool ControlsWindow::isRunning() const
{
    return d->isRunning();
}

void ControlsWindow::setRunning(bool running)
{
    d->setRunning(running);
}

void ControlsWindow::log(const std::string& s)
{
    d->log(s);
}

}
