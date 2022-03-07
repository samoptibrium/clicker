#pragma once

#include <memory>

#include <QMainWindow>

namespace Ui {
class CaptureAreaWindow;
}

namespace clickeroo::ui
{

/// The capture area window UI
class CaptureAreaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CaptureAreaWindow(QWidget* parent = nullptr);
    ~CaptureAreaWindow();

    void moveEvent(QMoveEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void setRunningVisualState(bool running);

    bool eventFilter(QObject* obj, QEvent* event) override;

    // Gets the size of the frame border of the widget in pixels (useful for ignoring/trimming it away later)
    std::size_t getBorderSizePixels() const;

protected:
    virtual bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    class CaptureAreaWindowImpl;
    std::unique_ptr<CaptureAreaWindowImpl> d;
};

}
