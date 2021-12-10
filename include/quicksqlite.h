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

    /* Error constants */
    public:
        static constexpr const char* const ERR_QUICKSQLITE = "quicksqlite: ";
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
         * Create a new database under the given filepath with the given create query if query string is not empty
         * If query string is empty, database with the given filepath will tried to be opened
         * @param filepath The filepath to the new database
         * @param query The create query for the database
         * @return True if creation and opening of the database was successful, otherwise false
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        bool open(const char* filepath, const char* query = "") const noexcept(false);

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

        /**
         * Deletes the entry with the given query
         * @param query The delete query
         * @return The number of rows affected
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        int delete_entry(const char* query) const noexcept(false);

        /**
         * Inserts the entries specified by the query
         * @param query The insert query
         * @return The last inserted row id
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        unsigned long long insert(const char* query) const noexcept(false);

        /**
         * Updates the entry specified by the query
         * @param query The update query
         * @return \ref quicksqlite::Exception if an error occurred
         */
        int update(const char* query) const noexcept(false);

    /* Member functions */
    private:

        /**
         * Scott Meyers' singleton
         */
        Database() = default;

        /**
         * Opens the database if not already done
         * @param filepath The path to the database file
         * @return True if a new database has been opened, false if a database is already _open
         * @throw \ref quicksqlite::Exception if an error occurred
         */
        bool _open(const char* filepath) noexcept(false);

    /* Member */
    private:

        /// Flag which indicates that a proper database has already been opened
        bool is_open = false;

        /// The pointer to the database object
        sqlite3* db = nullptr;

    /* Error Constants */
    private:

        /* Function names */
        static constexpr const char* const FN_SELECT = "select(): ";
        static constexpr const char* const FN_DELETE = "delete_entry(): ";
        static constexpr const char* const FN_UPDATE = "update(): ";
        static constexpr const char* const FN_INSERT = "insert(): ";
        static constexpr const char* const FN_CLOSE = "close(): ";
        static constexpr const char* const FN_OPEN = "_open(): ";

        /* Errors */
        /* Error code >100 */
        static constexpr const int ERRC_DB_NOT_OPEN = 100;
        static constexpr const char* const ERR_DB_NOT_OPEN = "Database is not open! ";

        static constexpr const int ERRC_COULD_NOT_OPEN = 101;
        static constexpr const char* const ERR_COULD_NOT_OPEN = "Could not open database! ";

        static constexpr const int ERRC_COULD_NOT_CLOSE = 102;
        static constexpr const char* const ERR_COULD_NOT_CLOSE = "Could not close database! ";

        static constexpr const int ERRC_FILE_DOES_NOT_EXIST = 103;
        static constexpr const char* const ERR_FILE_DOES_NOT_EXIST = "The file with the given filepath does not exists!";

        static constexpr const int ERRC_FILE_ALREADY_EXIST = 104;
        static constexpr const char* const ERR_FILE_ALREADY_EXIST = "The file with the given filepath already exists!";

        /* Error code >200 */
        static constexpr const int ERRC_PREPARED_STMT_FAILED = 200;
        static constexpr const char* const ERR_PREPARED_STMT_FAILED = "Prepared statement failed! ";

        static constexpr const int ERRC_STEP_FAILED = 201;
        static constexpr const char* const ERR_STEP_FAILED = "Step failed! ";
    };
}
