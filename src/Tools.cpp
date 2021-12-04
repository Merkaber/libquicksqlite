/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#include "Tools.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
bool Tools::file_exists(const char* const path) noexcept
{
    GetFileAttributes(path);
    if (0xffffffff == GetFileAttributes(path) && GetLastError() == ERROR_FILE_NOT_FOUND) {
        return false;
    } else {
        return true;
    }
}
#endif

