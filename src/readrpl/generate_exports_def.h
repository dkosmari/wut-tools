#pragma once
#include "readrpl.h"
#include <filesystem>
#include <string>

bool
generateExportsDef(const Rpl &rpl,
                   const std::string &rplName,
                   const std::filesystem::path &outFileName);
