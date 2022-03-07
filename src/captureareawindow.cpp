#include "captureareawindow.h"
#include "ui_captureareawindow.h"

#include <memory>
#include <optional>

#include <QObject>
#include <QMouseEvent>

#include "capturearea.h"
#include "qhotkey.h"
#include "windows.h"

namespace clickeroo::ui
{

class CaptureAreaWindow::CaptureAreaWindowImpl
{
public:
    CaptureAreaWindowImpl(CaptureAreaWindow* pQ) : q{pQ}, ui(std::make_unique<Ui::CaptureAreaWindow>())
    {
        q->setObjectName("captureareawindow");
        q->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        q->setWindowFlags(q->windowFlags() & (~Qt::WindowCloseButtonHint));

        ui->setupUi(q);

        setRunningVisualState(false);

        q->setAttribute(Qt::WA_TranslucentBackground);
        //q->setWindowOpacity(0.4);
        //q->setStyleSheet("QWidget{background: transparent}");

        // Simulate a stop button keypress if the shortcut is activated
        assert(toggleWindowHotKey.isRegistered());
        QObject::connect(&toggleWindowHotKey, &QHotkey::activated, q, [this](){
            q->isVisible() ? q->hide() : q->show();
        });

        qApp->installEventFilter(q);
    }

    void updateCaptureArea()
    {
        const auto rect = ui->centralWidget->rect();
        const auto rectPosition = ui->centralWidget->mapToGlobal(ui->centralWidget->pos());
        const auto dpiScale = q->devicePixelRatioF();
        clickeroo::setCaptureArea(QRect(std::round((double)rectPosition.x() * dpiScale),
                                        std::round((double)rectPosition.y() * dpiScale),
                                        std::round((double)rect.width() * dpiScale),
                                        std::round((double)rect.height() * dpiScale)));
    }

    ~CaptureAreaWindowImpl()
    {
        qApp->removeEventFilter(q);
    }

    void setRunningVisualState(bool running)
    {
        this->running = running;
        running ? q->setStyleSheet("border: " + QString::number(windowBorderSizePixels) + "px solid rgba(0, 255, 0, 1);") :
                  q->setStyleSheet("border: " + QString::number(windowBorderSizePixels) + "px solid rgba(255, 0, 0, 1);");
    }

    bool isRunning() const
    {
        return running;
    }

    bool eventFilter(QObject* obj, QEvent* event)
    {
        if(running) {
            return false;
        }

        if (obj == ui->centralWidget && event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *ev = static_cast<QMouseEvent *>(event);
            if (ev->buttons() & Qt::LeftButton)
            {
                lastDragPosition = ev->globalPosition() - q->geometry().topLeft();
            }
        }
        if (obj == ui->centralWidget && event->type() == QEvent::MouseMove)
        {
            QMouseEvent *ev = static_cast<QMouseEvent *>(event);
            if (ev->buttons() & Qt::LeftButton)
            {
                q->move(ev->globalPosition().x() - lastDragPosition.x(), ev->globalPosition().y() - lastDragPosition.y());
            }
        }
        return false;
    }

    std::size_t getBorderSizePixels() const
    {
        return windowBorderSizePixels;
    }

private:
    CaptureAreaWindow* q;
    std::unique_ptr<Ui::CaptureAreaWindow> ui;
    QString toggleCaptureWindowKeySequence{ "Ctrl+Y" };
    QHotkey toggleWindowHotKey{ QKeySequence(toggleCaptureWindowKeySequence), true, q };
    bool running{ false };
    QPointF lastDragPosition{0, 0};
    std::size_t windowBorderSizePixels{ 10 };
};

CaptureAreaWindow::CaptureAreaWindow(QWidget* parent) : QMainWindow(parent), d{std::make_unique<CaptureAreaWindow::CaptureAreaWindowImpl>(this)}
{

}

CaptureAreaWindow::~CaptureAreaWindow()
{
}

void CaptureAreaWindow::moveEvent(QMoveEvent* event)
{
    QMainWindow::moveEvent(event);
    d->updateCaptureArea();
}

void CaptureAreaWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    d->updateCaptureArea();
}

void CaptureAreaWindow::setRunningVisualState(bool running)
{
    d->setRunningVisualState(running);
}

bool CaptureAreaWindow::eventFilter(QObject* obj, QEvent* event)
{
    return d->eventFilter(obj, event);
}

bool CaptureAreaWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    if(d->isRunning()) {
        return false; // Prevent resizing if running
    }

    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_NCHITTEST)
    {
        if (isMaximized())
        {
            return false;
        }

        *result = 0;
        const LONG borderWidth = 8;
        RECT winrect;
        GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

        // must be short to correctly work with multiple monitors (negative coordinates)
        short x = msg->lParam & 0x0000FFFF;
        short y = (msg->lParam & 0xFFFF0000) >> 16;

        bool resizeWidth = minimumWidth() != maximumWidth();
        bool resizeHeight = minimumHeight() != maximumHeight();
        if (resizeWidth)
        {
            //left border
            if (x >= winrect.left && x < winrect.left + borderWidth)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - borderWidth)
            {
                *result = HTRIGHT;
            }
        }
        if (resizeHeight)
        {
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOP;
            }
        }
        if (resizeWidth && resizeHeight)
        {
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPRIGHT;
            }
        }

        if (*result != 0)
            return true;

        QWidget *action = QApplication::widgetAt(QCursor::pos());
        if (action == this){
            *result = HTCAPTION;
            return true;
        }
    }

    return false;
}

std::size_t CaptureAreaWindow::getBorderSizePixels() const
{
    return d->getBorderSizePixels();
}

}
