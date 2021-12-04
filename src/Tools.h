/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#pragma once

/**
 * Static class for simple tasks
 */
class Tools {

/* Interface */
public:

    /**
     * This class should be static. We don't want any object of this class
     */
    Tools() = delete;

    /**
     * Checks if the a file with the given path exists
     * @param path The path to the file
     * @return True if the file exists, otherwise false
     */
    static bool file_exists(const char* path) noexcept;
};
