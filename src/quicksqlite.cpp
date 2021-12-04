/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#include "quicksqlite.h"

#include "Tools.h"

quicksqlite::Exception::Exception(const char* const error_msg, const int error) : error_msg(error_msg), sqlite_err_code(error)
{

}

const char* quicksqlite::Exception::what() const noexcept
{
    return error_msg;
}

const int& quicksqlite::Exception::get_error() const noexcept
{
    return sqlite_err_code;
}

quicksqlite::Database& quicksqlite::Database::instance() noexcept
{
    static quicksqlite::Database database;
    return database;
}

bool quicksqlite::Database::open(const char* filepath) noexcept(false)
{
    if (is_open) {
        return false;
    }

    /* Create new database with the given file path if the database did not exist yet */
    if (!Tools::file_exists(filepath)) {
        //TODO: Create database
        return true;
    }



    is_open = true;
    return true;
}
