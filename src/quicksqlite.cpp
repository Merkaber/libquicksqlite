/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#include "quicksqlite.h"

#include <thread>
#include <chrono>

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

std::vector<std::vector<std::string>> quicksqlite::Database::select(const char* query) const noexcept(false)
{
    if (!db) {
        // TODO: Implement proper exception
        throw quicksqlite::Exception("", 0);
    }

    std::vector<std::vector<std::string>> res_set;
    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
        // TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }

    int step_res;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    } while (step_res == SQLITE_BUSY);

    if (step_res != SQLITE_DONE && step_res != SQLITE_OK && step_res != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        // TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }

    /* Save the names of the columns in the first row of the result set */
    if (step_res != SQLITE_DONE) {
        int counter = 0;
        std::vector<std::string> name_tmp;
        while (true) {
            const char* col_name = sqlite3_column_name(stmt, counter);

            if (col_name == nullptr) {
                break;
            }

            ++counter;
            name_tmp.emplace_back(col_name);
        }
        res_set.push_back(name_tmp);
    }

    /* Save data within res_set */
    while (step_res != SQLITE_DONE) {
        while (step_res == SQLITE_BUSY) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            step_res = sqlite3_step(stmt);
        }

        if (step_res != SQLITE_OK && step_res != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            // TODO: Implement proper exception
            throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
        }

        int col_count = sqlite3_column_count(stmt);
        std::vector<std::string> row_tmp(col_count);
        for (int i = 0; i < col_count; ++i) {
            row_tmp.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
        }

        res_set.push_back(row_tmp);
        step_res = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    return res_set;
}

int quicksqlite::Database::delete_entry(const char* query) const noexcept(false)
{
    if (!db) {
        // TODO: Implement proper exception
        throw quicksqlite::Exception("", 0);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);
        // TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }

    int step_res;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    } while (step_res == SQLITE_BUSY);

    if (step_res != SQLITE_OK && step_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        // TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }

    sqlite3_finalize(stmt);
    return sqlite3_changes(db);
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

        //TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }
}

bool quicksqlite::Database::close() noexcept(false)
{
    if (!is_open) {
        return false;
    }

    int close_res = sqlite3_close(db);
    if (close_res == SQLITE_OK) {
        is_open = false;
        return true;
    } else {

        //TODO: Implement proper exception
        throw quicksqlite::Exception(sqlite3_errmsg(db), 0);
    }
}
