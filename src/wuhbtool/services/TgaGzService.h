#pragma once
#include <cstdint>
#include <FreeImage.h>

#include "../entities/FileEntry.h"

FileEntry* createTgaGzFileEntry(const char* inputFile, unsigned width, unsigned height, unsigned bpp, const char* filename);
