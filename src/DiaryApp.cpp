// DiaryApp.cpp
#include "DiaryApp.h"
#include <iostream>
#include <sqlite3.h>
#include <chrono>
#include <ctime>

DiaryApp::DiaryApp(const char *dbFile) : DB_FILE(dbFile) {
    initializeDatabase();
}

void DiaryApp::displayMenu() {
    std::cout << "Choose an option:" << std::endl;
    std::cout << "1. Add Diary Entry" << std::endl;
    std::cout << "2. View Diary Entries" << std::endl;
    std::cout << "3. Delete Diary Entry" << std::endl;
    std::cout << "4. Clear" << std::endl;
    std::cout << "5. Exit" << std::endl;
}

int DiaryApp::startup() {
    displayMenu();
    while (true) {
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                addEntry();
                break;
            case 2:
                viewEntries();
                break;
            case 3:
                deleteEntry();
                break;
            case 4:
                clearConsole();
                displayMenu();
                break;
            case 5:
                std::cout << "Exiting the diary application." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Try again." << std::endl;
                break;
        }
    }

    return 0;
}

std::string DiaryApp::getTime() {
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    std::tm* timeinfo = std::localtime(&currentTime_t);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

void DiaryApp::initializeDatabase() {
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS diary_entries (id INTEGER PRIMARY KEY AUTOINCREMENT, entry TEXT);";

    result = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error creating table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_close(db);
}

void DiaryApp::addEntry() {
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string entry;
    std::cin.ignore(); // Ignore newline from previous input
    std::cout << "Enter your diary entry:" << std::endl;
    std::getline(std::cin, entry);
    entry = entry + " RECORDTIME:" + getTime();

    std::string insertSQL = "INSERT INTO diary_entries (entry) VALUES ('" + entry + "');";
    result = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Error inserting entry: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_close(db);
}

void DiaryApp::viewEntries() {
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    const char *selectSQL = "SELECT * FROM diary_entries;";
    sqlite3_stmt *statement;

    result = sqlite3_prepare_v2(db, selectSQL, -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    std::cout << "Diary Entries:" << std::endl;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        std::cout << "ID: " << sqlite3_column_int(statement, 0) << " - Entry: " << sqlite3_column_text(statement, 1) << std::endl;
    }

    sqlite3_finalize(statement);
    sqlite3_close(db);
}

void DiaryApp::deleteEntry() {
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    int entryId;
    std::cout << "Enter the ID of the entry you want to delete:" << std::endl;
    std::cin >> entryId;

    std::string deleteSQL = "DELETE FROM diary_entries WHERE id = " + std::to_string(entryId) + ";";
    result = sqlite3_exec(db, deleteSQL.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting entry: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    std::cout << "Entry with ID " << entryId << " deleted successfully." << std::endl;

    sqlite3_close(db);
}

void DiaryApp::clearConsole() {
    // ANSI escape code to clear the screen
    std::cout << "\033[2J\033[H";
}
