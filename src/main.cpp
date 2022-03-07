#include <optional>

#include <QApplication>
#include <QMessageBox>
#include <QObject>
#include <QPixmap>

#include "bindings.h"
#include "capturearea.h"
#include "controlswindow.h"
#include "captureareawindow.h"
#include "settings.h"
#include "pybind11/embed.h"

int main(int argc, char* argv[])
{
    try {
        pybind11::initialize_interpreter();
    } catch(...) {
        QMessageBox::information(nullptr, "Error", "Failed to initialize Python interpreter");
        return -1;
    }
    auto cleanup = qScopeGuard([] { pybind11::finalize_interpreter(); });

    clickeroo::initBindings();

    QApplication a(argc, argv);

    clickeroo::setupSettings();

    clickeroo::ui::CaptureAreaWindow captureAreaWindow;

    clickeroo::ui::ControlsWindow controlsWindow(nullptr, &captureAreaWindow);
    controlsWindow.show();

    captureAreaWindow.move(controlsWindow.x() - captureAreaWindow.width(), controlsWindow.y());
    captureAreaWindow.show();

    controlsWindow.connect(&controlsWindow, &clickeroo::ui::ControlsWindow::signal_captureAreaGrabbed, [](QPixmap pixmap) {
        clickeroo::setImage(pixmap.toImage());
    });

    controlsWindow.connect(&controlsWindow, &clickeroo::ui::ControlsWindow::startPressed, [&captureAreaWindow]() {
        captureAreaWindow.setRunningVisualState(true);
    });
    controlsWindow.connect(&controlsWindow, &clickeroo::ui::ControlsWindow::stopPressed, [&captureAreaWindow]() {
        captureAreaWindow.setRunningVisualState(false);
    });

    return a.exec();
}
