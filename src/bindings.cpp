#include "bindings.h"

#include <string>

#include <QApplication>
#include <QMessageBox>

#include "pybind11/embed.h"

#include "capturearea.h"
#include "captureareawindow.h"
#include "controlswindow.h"
#include "settings.h"

PYBIND11_EMBEDDED_MODULE(clickeroo, m) {
    // `m` is a `py::module_` which is used to bind functions and classes
    m.def("testArea", &clickeroo::testAreaDiffLessThan);
    m.def("percentToMouseX", &clickeroo::percentToMouseX);
    m.def("percentToMouseY", &clickeroo::percentToMouseY);
    m.def("areaX1", []() {
       return clickeroo::getCaptureArea().left();
    });
    m.def("areaY1", []() {
       return clickeroo::getCaptureArea().top();
    });
    m.def("areaX2", []() {
       return clickeroo::getCaptureArea().right();
    });
    m.def("areaY2", []() {
       return clickeroo::getCaptureArea().bottom();
    });
    m.def("areaWidth", []() {
       return clickeroo::getCaptureArea().width();
    });
    m.def("areaHeight", []() {
       return clickeroo::getCaptureArea().height();
    });

    m.def("isRunning", []() {
        auto widgets = QApplication::allWidgets();
        for(auto widget : widgets) {
            if(auto w = dynamic_cast<clickeroo::ui::ControlsWindow*>(widget)) {
                return w->isRunning();
            }
        }
        return false;
    });

    m.def("stop", []() {
        auto widgets = QApplication::allWidgets();
        for(auto widget : widgets) {
            if(auto w = dynamic_cast<clickeroo::ui::ControlsWindow*>(widget)) {
                if(w->isRunning()) {
                    w->setRunning(false);
                }
            }
        }
        QApplication::processEvents();
    });

    m.def("log", [](const std::string s) {
        auto widgets = QApplication::allWidgets();
        for(auto widget : widgets) {
            if(auto w = dynamic_cast<clickeroo::ui::ControlsWindow*>(widget)) {
                w->log(s);
            }
        }
        QApplication::processEvents();
    });

    m.def("processEvents", []() {
        QApplication::processEvents();
    });
}

namespace clickeroo
{

void initBindings()
{
    for(const auto& path : clickeroo::getHelperScriptsPaths()) {
        runFile(path);
    }
}

void run(const std::string& code)
{
    pybind11::exec(code);
}

void runFile(const std::filesystem::path& path)
{
    run(clickeroo::readFile(path).toStdString());
}

}
