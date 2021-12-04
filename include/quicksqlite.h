/*
 * quicksqlite
 *
 * Copyright (c) 2021 Simon Piorecki
 */

#pragma once

#include <sqlite3.h>
#include <exception>
#include <vector>
#include <string>

namespace quicksqlite {
    class Exception : std::exception {
    public:
        Exception(const char* error_msg, int error);
        [[nodiscard]] const char* what() const noexcept override;
        [[nodiscard]] const int& get_error() const noexcept;

    private:
        const char* const error_msg;
        const int sqlite_err_code;
    };

    class Database {

    /* Interface */
    public:

        /**
         * Scott Meyers' singleton
         */
        Database(const Database&) = delete;
        void operator=(const Database&) = delete;

        /**
         * @return The unique instance of this class
         */
        static Database& instance() noexcept;

        /**
         * Opens the database if not already done
         * @param filepath The path to the database file
         * @return True if a new database has been opened, false if a database is already open
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        bool open(const char* filepath) noexcept(false);

        /**
         * Closes the database if not already closed
         * @return True if the database was closed successfully, otherwise false
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        bool close() noexcept(false);

        /**
         * Executes the given select query
         * @param query The select query
         * @return A result set in which the first row are the names of the columns
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        std::vector<std::vector<std::string>> select(const char* query) const noexcept(false);

    /* Member functions */
    private:

        /**
         * Scott Meyers' singleton
         */
        Database() = default;

    /* Member */
    private:

        /// Flag which indicates that a proper database has already been opened
        bool is_open = false;

        /// The pointer to the database object
        sqlite3* db = nullptr;

    };
}
