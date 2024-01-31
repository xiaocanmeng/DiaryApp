#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Backup.h>
#include <thread> // 用于 std::this_thread::sleep_for
#include <chrono> // 用于 std::chrono::seconds
#include "DBoperation.h"

bool queryDatabaseContent(const SQLite::Database& db, const std::string& tableName) {
    try {
        std::string querySQL = "SELECT * FROM " + tableName + ";";
        SQLite::Statement query(db, querySQL);

        // 遍历查询结果并输出
        while (query.executeStep()) {
            int id = query.getColumn("id");
            std::string entry = query.getColumn("entry");
            std::string priority = query.getColumn("priority");
            std::string time = query.getColumn("time");

            std::cout << "ID: " << id << ", Entry: " << entry << ", Priority: " << priority << ", Time: " << time << std::endl;
        }

        return true;
    } catch (std::exception &e) {
        std::cerr << "Error querying database: " << e.what() << std::endl;
        return false;
    }
}

bool isDatabaseEmpty(const std::string &databasePath, const std::string &tableName)
{
    bool ret {false};
    try
    {
        // 打开数据库连接
        SQLite::Database db(databasePath);

        // 查询表的行数
        std::string countQuery = "SELECT COUNT(*) FROM " + tableName + ";";
        SQLite::Statement query(db, countQuery);

        // 执行查询
        if (query.executeStep())
        {
            int rowCount = query.getColumn(0);
            // 如果行数为0，则数据库为空
            if(rowCount == 0)
            {
                ret = true;
                std::cout << "The database is empty.\n";
            }
            else
            {
                std::cout << "The database is not empty.\n";
            }
            
        }
        else
        {
            // 查询执行失败
            std::cerr << "Error executing count query.\n";
        }
    }
    catch (std::exception &e)
    {
        // 异常处理
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return ret;
}



bool backupSQLiteDatabase(const std::string &sourcePath, const std::string &backupPath) {
    try {
        // Open source database connection
        SQLite::Database sourceDb(sourcePath, SQLite::OPEN_READONLY);

        // Open target database connection (backup file)
        SQLite::Database backupDb(backupPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // Create a backup object
        sqlite3 *pSrc = sourceDb.getHandle();
        sqlite3 *pDest = backupDb.getHandle();
        sqlite3_backup *pBackup = sqlite3_backup_init(pDest, "main", pSrc, "main");

        if (pBackup) {
            // Perform the backup
            while (sqlite3_backup_step(pBackup, -1) == SQLITE_OK) {}

            // Finish the backup
            sqlite3_backup_finish(pBackup);
        } else {
            std::cerr << "Error creating backup object\n";
            return false;
        }

        std::cout << "Database backup successful\n";
        return true;
    } catch (std::exception &e) {
        std::cerr << "Error during backup: " << e.what() << "\n";
        return false;
    }
}


void initializeDatabase(const std::string DB_FILE)
{
    try
    {
        // 打开数据库连接
        SQLite::Database db(DB_FILE, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
        // 创建表
        db.exec("CREATE TABLE IF NOT EXISTS diary_entries (id INTEGER PRIMARY KEY AUTOINCREMENT, entry TEXT, priority TEXT, time TEXT)");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error during database initialization: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}



// 清空数据库所有数据的函数
bool clearAllEntries(const std::string &dbFilePath)
{
    try
    {
        // 打开 SQLite 数据库
        SQLite::Database db(dbFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // 执行 SQL 命令来删除所有记录
        const std::string sql = "DELETE FROM diary_entries;"; // 请替换为你的表名
        db.exec(sql.c_str());

        std::cout << "All entries cleared successfully." << std::endl;
        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "清空所有日记时发生错误：" << e.what() << std::endl;
        return false;
    }
}

// 数据恢复函数
bool restoreDatabase(const std::string& backupFilePath, const std::string& dbFilePath)
{
    try
    {
        // 打开备份数据库连接
        SQLite::Database backupDB(backupFilePath, SQLite::OPEN_READONLY);

        // 打开主数据库连接
        SQLite::Database mainDB(dbFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // 开始数据库事务
        mainDB.exec("BEGIN;");

        // 使用 ATTACH 命令连接备份数据库
        std::string attachSQL = "ATTACH DATABASE '" + backupFilePath + "' AS backupDB;";
        mainDB.exec(attachSQL.c_str());

        // 检查表是否存在
        bool tableExists = mainDB.tableExists("diary_entries");

        // 如果表存在，则执行 SQL 命令来复制数据
        if (tableExists)
        {
            std::string copySQL = "INSERT INTO diary_entries SELECT * FROM backupDB.diary_entries;";
            mainDB.exec(copySQL.c_str());
        }

        // 提交数据库事务
        mainDB.exec("COMMIT;");

        // 分离备份数据库
        mainDB.exec("DETACH DATABASE backupDB;");

        std::cout << "Data restored successfully." << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during restore: " << e.what() << std::endl;
        return false;
    }
}

