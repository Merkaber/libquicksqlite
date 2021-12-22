/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#include "quicksqlite.h"

#include <thread>
#include <chrono>
#include <stdexcept>

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
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_SELECT) + std::string(ERR_DB_NOT_OPEN);
        throw quicksqlite::Exception(msg.c_str(), ERRC_DB_NOT_OPEN);
    }

    std::vector<std::vector<std::string>> res_set;
    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_SELECT) + std::string(ERR_PREPARED_STMT_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res = sqlite3_step(stmt);
    while (step_res == SQLITE_BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    }

    if (step_res != SQLITE_DONE && step_res != SQLITE_OK && step_res != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_SELECT) + std::string(ERR_STEP_FAILED)
                + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
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

            std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_SELECT) + std::string(ERR_STEP_FAILED)
                              + sqlite3_errmsg(db);
            throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
        }

        int col_count = sqlite3_column_count(stmt);
        std::vector<std::string> row_tmp;
        row_tmp.reserve(col_count);
        for (int i = 0; i < col_count; ++i) {
            row_tmp.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
        }

        res_set.push_back(row_tmp);
        step_res = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    return res_set;
}

int quicksqlite::Database::get_entry_id(const char* query, const char* id_col_name) const noexcept(false)
{
    if (!db) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_GET_ENTRY_ID) + std::string(ERR_DB_NOT_OPEN);
        throw quicksqlite::Exception(msg.c_str(), ERRC_DB_NOT_OPEN);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_GET_ENTRY_ID) + std::string(ERR_PREPARED_STMT_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res = sqlite3_step(stmt);
    while (step_res == SQLITE_BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    }

    if (step_res != SQLITE_DONE && step_res != SQLITE_OK && step_res != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_GET_ENTRY_ID) + std::string(ERR_STEP_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
    }

    if (step_res == SQLITE_DONE) {
        return 0;
    }

    const char* id_col_name_tmp = sqlite3_column_name(stmt, 0);
    if (std::string(id_col_name) != std::string(id_col_name_tmp)) {

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_GET_ENTRY_ID) + std::string(ERR_COLUMN_NOT_ID)
                          + std::string(id_col_name);
        throw quicksqlite::Exception(msg.c_str(), ERRC_COLUMN_NOT_ID);
    }

    try {
        return std::stoi(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
    } catch (std::logic_error& e) {

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_GET_ENTRY_ID) + std::string(e.what());
        throw quicksqlite::Exception(msg.c_str(), 0);
    }
}

int quicksqlite::Database::delete_entry(const char* query) const noexcept(false)
{
    if (!db) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_DELETE) + std::string(ERR_DB_NOT_OPEN);
        throw quicksqlite::Exception(msg.c_str(), ERRC_DB_NOT_OPEN);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_DELETE) + std::string(ERR_PREPARED_STMT_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res = sqlite3_step(stmt);
    while (step_res == SQLITE_BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    }

    if (step_res != SQLITE_OK && step_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_DELETE) + std::string(ERR_STEP_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
    }

    sqlite3_finalize(stmt);
    return sqlite3_changes(db);
}

unsigned long long quicksqlite::Database::insert(const char* query) const noexcept(false)
{
    if (!db) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_INSERT) + std::string(ERR_DB_NOT_OPEN);
        throw quicksqlite::Exception(msg.c_str(), ERRC_DB_NOT_OPEN);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_INSERT) + std::string(ERR_PREPARED_STMT_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res = sqlite3_step(stmt);
    while (step_res == SQLITE_BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    }

    if (step_res != SQLITE_OK && step_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_INSERT) + std::string(ERR_STEP_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

int quicksqlite::Database::update(const char* query) const noexcept(false)
{
    if (!db) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_UPDATE) + std::string(ERR_DB_NOT_OPEN);
        throw quicksqlite::Exception(msg.c_str(), ERRC_DB_NOT_OPEN);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_UPDATE) + std::string(ERR_PREPARED_STMT_FAILED)
                + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res = sqlite3_step(stmt);
    while (step_res == SQLITE_BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    }

    if (step_res != SQLITE_OK && step_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_UPDATE) + std::string(ERR_STEP_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
    }

    sqlite3_finalize(stmt);
    return sqlite3_changes(db);
}

bool quicksqlite::Database::_open(const char* filepath) noexcept(false)
{
    /* If the database did not exist yet */
    if (!Tools::file_exists(filepath)) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_OPEN) + std::string(ERR_FILE_DOES_NOT_EXIST);
        throw quicksqlite::Exception(msg.c_str(), ERRC_FILE_DOES_NOT_EXIST);
    }

    int open_res = sqlite3_open(filepath, &db);
    if (open_res == SQLITE_OK) {
        is_open = true;
        return true;
    } else {

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_OPEN) + std::string(ERR_COULD_NOT_OPEN) +
                          sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_COULD_NOT_OPEN);
    }
}

bool quicksqlite::Database::open(const char* filepath, const char* query) noexcept(false)
{
    if (is_open) {
        return false;
    }

    if (std::string(query).empty()) {
        return _open(filepath);
    }

    /* If the database did already exist */
    if (Tools::file_exists(filepath)) {
        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_OPEN) + std::string(ERR_FILE_ALREADY_EXIST);
        throw quicksqlite::Exception(msg.c_str(), ERRC_FILE_ALREADY_EXIST);
    }

    sqlite3_stmt* stmt = nullptr;
    int prep_res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (prep_res != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_OPEN) + std::string(ERR_PREPARED_STMT_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_PREPARED_STMT_FAILED);
    }

    int step_res;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        step_res = sqlite3_step(stmt);
    } while (step_res == SQLITE_BUSY);

    if (step_res != SQLITE_OK && step_res != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_OPEN) + std::string(ERR_STEP_FAILED)
                          + sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_STEP_FAILED);
    }

    sqlite3_finalize(stmt);
    is_open = true;
    return true;
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

        std::string msg = std::string(Exception::ERR_QUICKSQLITE) + std::string(FN_CLOSE) + std::string(ERR_COULD_NOT_CLOSE) +
                sqlite3_errmsg(db);
        throw quicksqlite::Exception(msg.c_str(), ERRC_COULD_NOT_CLOSE);
    }
}
