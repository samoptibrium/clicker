#pragma once

#include <memory>

#include <QMainWindow>

#include "captureareawindow.h"

namespace Ui {
class ControlsWindow;
}

namespace clickeroo::ui
{

/// The controls window UI
class ControlsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlsWindow(QWidget* parent, clickeroo::ui::CaptureAreaWindow* window);
    ~ControlsWindow();

    void setCaptureAreaPreviewPixmap(QPixmap pixmap);

    void closeEvent(QCloseEvent *event);

    bool isRunning() const;
    void setRunning(bool running);

    void log(const std::string& s);

signals:
    void signal_captureAreaGrabbed(QPixmap pixmap);
    void startPressed();
    void stopPressed();

private:
    class ControlsWindowImpl;
    std::unique_ptr<ControlsWindowImpl> d;
};

}
