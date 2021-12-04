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

    int open_res = sqlite3_open(filepath, &db);
    if (open_res == SQLITE_OK) {

        //TODO: Check if the given database is the correct one
        is_open = true;
        return true;
    } else {
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }
}
