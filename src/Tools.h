/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#pragma once

/**
 * Namespace for static functions
 */
namespace quicksqlite::Tools {

    /**
     * Checks if the a file with the given path exists
     * @param path The path to the file
     * @return True if the file exists, otherwise false
     */
    bool file_exists(const char* path) noexcept;
}
