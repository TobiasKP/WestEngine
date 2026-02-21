#pragma once

#include <filesystem>
#include <string>

namespace PathUtils
{

const std::string& getExecutableDir();

std::string resolve(const std::string& relativePath);

FILE* openFile(const std::string& path, bool readOnly);

}  // namespace PathUtils
