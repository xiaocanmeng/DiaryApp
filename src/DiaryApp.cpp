// DiaryApp.cpp
#include "DiaryApp.h"
#include <iostream>
#include <sqlite3.h>
#include <chrono>
#include <ctime>
#include "DBoperation.h"
#include <SQLiteCpp/SQLiteCpp.h>

DiaryApp::DiaryApp(std::string dbFile) : DB_FILE(dbFile)
{
    initializeDatabase(DB_FILE);
    std::string suffixBACKUP{"backup.db"};
    DB_BACKUPFILE = DB_FILE + suffixBACKUP;
}


void DiaryApp::displayMenu()
{
    std::cout << "Choose an option:" << std::endl;
    std::cout << "1. Add Diary Entry" << std::endl;
    std::cout << "2. View Diary Entries" << std::endl;
    std::cout << "3. Delete Diary Entry" << std::endl;
    std::cout << "4. Clear" << std::endl;
    std::cout << "5. DeleteALL" << std::endl;
    std::cout << "6. Restore" << std::endl;
    std::cout << "7. Exit" << std::endl;
}

int DiaryApp::startup()
{
    displayMenu();
    while (true)
    {
        int choice;
        

        // 检查输入流状态
        if (!(std::cin >> choice))
        {
            // 输入不是整数，清除错误状态并清空输入缓冲区
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue; // 跳过本次循环，重新获取输入
        }

        switch (choice)
        {
        case 1:
            addEntry();
            // clearConsole();
            displayMenu();
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
            backupDiary();
            clearAllEntries(DB_FILE);
            break;
        case 6:
            restoreDatabase(DB_BACKUPFILE, DB_FILE);
            break;
        case 7:
            {
                backupDiary();
                std::cout << "Exiting the diary application." << std::endl;
                return 0;
            }
        default:
            std::cout << "Invalid choice. Try again." << std::endl;
            break;
        }
    }

    return 0;
}

void DiaryApp::backupDiary()
{
    // 调用备份函数
    initializeDatabase(DB_BACKUPFILE);
    if (backupSQLiteDatabase(DB_FILE, DB_BACKUPFILE))
    {
        std::cout << "Database backup successful\n";
    }
    else
    {
        std::cerr << "Database backup failed\n";
    }
}

std::string DiaryApp::getTime()
{
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    std::tm *timeinfo = std::localtime(&currentTime_t);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return std::string(buffer);
}


void DiaryApp::addEntry() {
    try {
        // Open the SQLite database
        SQLite::Database db(DB_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        std::string entry;
        std::cin.ignore(); // Ignore newline from previous input
        std::cout << "Enter your diary entry:" << std::endl;
        std::getline(std::cin, entry);

        std::cout << "Enter your diary priority:" << std::endl;
        std::cout << "1. Low" << std::endl;
        std::cout << "2. Medium" << std::endl;
        std::cout << "3. High" << std::endl;
        std::string priority[3]{"Low", "Medium", "High"};
        int32_t index{};
        std::cin >> index;

        // Prepare the SQL statement
        SQLite::Statement query(db, "INSERT INTO diary_entries (entry, priority, time) VALUES (?, ?, ?);");
        query.bind(1, entry);
        query.bind(2, priority[index - 1]);
        query.bind(3, getTime());

        // Execute the SQL statement
        query.exec();

        std::cout << "Add Success" << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error inserting entry: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void DiaryApp::viewEntries()
{
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE.c_str(), &db);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    const char *selectSQL = "SELECT * FROM diary_entries ORDER BY CASE priority WHEN 'High' THEN 0 WHEN 'Medium' THEN 1 WHEN 'Low' THEN 2 ELSE 3 END, time ASC;";

    sqlite3_stmt *statement;

    result = sqlite3_prepare_v2(db, selectSQL, -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    std::cout << "Diary Entries:" << std::endl;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(statement, 0);
        const char *entryText = reinterpret_cast<const char *>(sqlite3_column_text(statement, 1));
        const char *priorityText = reinterpret_cast<const char *>(sqlite3_column_text(statement, 2));
        const char *timeText = reinterpret_cast<const char *>(sqlite3_column_text(statement, 3));

        std::cout << "ID: " << id << "- Thing: " << entryText
                  << " - Priority: " << priorityText << " - Time: " << timeText << std::endl;
    }

    sqlite3_finalize(statement);
    sqlite3_close(db);
}

void DiaryApp::deleteEntry()
{
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE.c_str(), &db);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }

    int entryId;
    std::cout << "Enter the ID of the entry you want to delete:" << std::endl;
    std::cin >> entryId;

    std::string deleteSQL = "DELETE FROM diary_entries WHERE id = " + std::to_string(entryId) + ";";
    result = sqlite3_exec(db, deleteSQL.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error deleting entry: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    std::cout << "Entry with ID " << entryId << " deleted successfully." << std::endl;

    sqlite3_close(db);
}

void DiaryApp::clearConsole()
{
    // ANSI escape code to clear the screen
    std::cout << "\033[2J\033[H";
}