#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <QSettings>
#include <QString>

namespace clickeroo
{

void setupSettings();

QSettings getSettings();

QString readFile(const std::filesystem::path& path);
void writeFile(const std::filesystem::path& path, const QString& text);

std::filesystem::path getPresetPath(const std::string& folderName);

std::map<std::filesystem::path, std::vector<std::filesystem::path>> getPerFolderScriptPaths();
std::vector<std::filesystem::path> getScriptPaths(const std::filesystem::path& scriptFolder);
std::vector<std::filesystem::path> getScriptFolders();

std::vector<std::filesystem::path> getHelperScriptsPaths();

}
