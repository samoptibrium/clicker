#pragma once

#include <filesystem>
#include <string>

namespace clickeroo
{

void initBindings();
void run(const std::string& code);
void runFile(const std::filesystem::path& path);

}
