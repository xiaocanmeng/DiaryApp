#include <iostream>
#include <sqlite3.h>

// SQLite3 数据库文件
const char* DB_FILE = "diary.db";

// 初始化数据库
void initializeDatabase() {
    sqlite3* db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS diary_entries (id INTEGER PRIMARY KEY AUTOINCREMENT, entry TEXT);";

    result = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error creating table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_close(db);
}

// 添加日记条目
void addEntry(const std::string& entry) {
    sqlite3* db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string insertSQL = "INSERT INTO diary_entries (entry) VALUES ('" + entry + "');";
    result = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Error inserting entry: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_close(db);
}

// 查看所有日记条目
void viewEntries() {
    sqlite3* db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* selectSQL = "SELECT * FROM diary_entries;";
    sqlite3_stmt* statement;

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

// 删除日记条目
void deleteEntry(int entryId) {
    sqlite3* db;
    int result = sqlite3_open(DB_FILE, &db);

    if (result != SQLITE_OK) {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

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

int main() {
    initializeDatabase();

    while (true) {
        std::cout << "Choose an option:" << std::endl;
        std::cout << "1. Add Diary Entry" << std::endl;
        std::cout << "2. View Diary Entries" << std::endl;
        std::cout << "3. Delete Diary Entry" << std::endl;
        std::cout << "4. Exit" << std::endl;

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cin.ignore(); // Ignore newline from previous input
                std::string entry;
                std::cout << "Enter your diary entry:" << std::endl;
                std::getline(std::cin, entry);
                addEntry(entry);
                break;
            }
            case 2:
                viewEntries();
                break;
            case 3: {
                std::cout << "Enter the ID of the entry you want to delete:" << std::endl;
                int entryId;
                std::cin >> entryId;
                deleteEntry(entryId);
                break;
            }
            case 4:
                std::cout << "Exiting the diary application." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Try again." << std::endl;
                break;
        }
    }

    return 0;
}
