#include "settings.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

namespace
{

constexpr const char* topLevelSettingsFolderPath = "clickeroo/scripts";
constexpr const char* topLevelSettingsIniName = "settings.ini";

std::filesystem::path getSettingsPath()
{
    return std::filesystem::path(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).toStdString()) / topLevelSettingsFolderPath;
}

std::filesystem::path getSettingsIniFile()
{
    return getSettingsPath() / topLevelSettingsIniName;
}

bool isScriptFile(const std::filesystem::path& path)
{
    return std::filesystem::is_regular_file(path) && path.extension().string() == ".py";
}

}

namespace clickeroo
{

std::filesystem::path getPresetPath(const std::string& folderName)
{
    return getSettingsPath() / folderName;
}

std::vector<std::filesystem::path> getScriptFolders()
{
    std::vector<std::filesystem::path> paths;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(getSettingsPath())) {
            if(entry.is_directory()) {
                paths.emplace_back(entry);
            }
        }
    } catch(...) {
        assert(0 && "Failed to get script folders");
    }
    return paths;
}

std::vector<std::filesystem::path> getHelperScriptsPaths()
{
    return { getSettingsPath() / "helper_scripts.py" };
}

std::vector<std::filesystem::path> getScriptPaths(const std::filesystem::path& scriptFolder)
{
    std::vector<std::filesystem::path> paths;
    for (const auto& entry : std::filesystem::directory_iterator(scriptFolder)) {
        if(isScriptFile(entry)) {
            paths.push_back(entry);
        }
    }
    return paths;
}

std::map<std::filesystem::path, std::vector<std::filesystem::path>> getPerFolderScriptPaths()
{
    const auto folders = getScriptFolders();

    std::map<std::filesystem::path, std::vector<std::filesystem::path>> m;

    for(const auto& folder : folders) {
        try {
            m[folder] = getScriptPaths(folder);
        } catch(...) {
            assert(0 && "Failed to get script folders");
        }
    }
    return m;
}

QString readFile(const std::filesystem::path& path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        assert(0 && "Failed to read file");
        return "";
    }
    QTextStream strm(&file);
    return strm.readAll();
}

void writeFile(const std::filesystem::path& path, const QString& text)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        assert(0 && "Failed to open file to write");
        QMessageBox msgBox;
        msgBox.setText(file.errorString());
        msgBox.exec();
        return;
    }
    QTextStream out(&file);
    out << text;
}

void setupSettings()
{
    const auto path = getSettingsPath();

    try {
        if(!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    } catch(...) {
        assert(0 && "Failed to create settings directory");
    }

    getSettings();
}

QSettings getSettings()
{
    return QSettings(QString::fromWCharArray(getSettingsIniFile().wstring().c_str()), QSettings::Format::IniFormat);
}

}
