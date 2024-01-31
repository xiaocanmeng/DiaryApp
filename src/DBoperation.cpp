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


bool backupSQLiteDatabase(const std::string &sourcePath, const std::string &backupPath)
{
#if 1
    // 打开源数据库连接
    sqlite3 *sourceDb;
    if (sqlite3_open(sourcePath.c_str(), &sourceDb) != SQLITE_OK)
    {
        std::cerr << "Error opening source database\n";
        return false;
    }
    // 创建目标数据库连接（备份文件）
    sqlite3 *backupDb;
    if (sqlite3_open(backupPath.c_str(), &backupDb) != SQLITE_OK)
    {
        std::cerr << "Error opening backup database\n";
        sqlite3_close(sourceDb);
        return false;
    }
    // 创建备份对象
    sqlite3_backup *backup = sqlite3_backup_init(backupDb, "main", sourceDb, "main");
    if (!backup)
    {
        std::cerr << "Error creating backup object\n";
        sqlite3_close(sourceDb);
        sqlite3_close(backupDb);
        return false;
    }
    // 执行备份
    int rc;
    do
    {
        rc = sqlite3_backup_step(backup, -1);
        if (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
        {
            // 继续备份
        }
        else if (rc == SQLITE_DONE)
        {
            // 备份完成
        }
        else
        {
            std::cerr << "Error during backup: " << sqlite3_errmsg(backupDb) << "\n";
            break;
        }
    } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    // 关闭备份对象和数据库连接
    sqlite3_backup_finish(backup);
    sqlite3_close(sourceDb);
    sqlite3_close(backupDb);
    return rc == SQLITE_DONE;
#endif
#if 0
    bool ret{false};
    std::cout << "The source database list.\n";
    queryDatabaseContent(sourcePath,"diary_entries");
    try
    {
        // 打开源数据库连接
        SQLite::Database sourceDb(sourcePath, SQLite::OPEN_READONLY);

        // 查询源数据库中的数据
        std::string querySQL = "SELECT * FROM diary_entries;";
        SQLite::Statement query(sourceDb, querySQL);

        // 打开目标数据库连接（备份文件）
        SQLite::Database backupDb(backupPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // 开始事务
        SQLite::Transaction transaction(backupDb);

        // 使用 SQL 语句进行备份（请替换为你的表名）
        if (!backupDb.tableExists("diary_entries"))
        {
            // 表不存在，创建表
            std::string createTableSQL = "CREATE TABLE diary_entries AS SELECT * FROM main.diary_entries;";
            std::cout << "Executing SQL: " << createTableSQL << std::endl;
            backupDb.exec(createTableSQL);
        }
        else
        {
            // 表已存在，直接插入数据
            std::string insertDataSQL = "INSERT INTO diary_entries SELECT * FROM main.diary_entries;";
            std::cout << "Executing SQL: " << insertDataSQL << std::endl;
            backupDb.exec(insertDataSQL);
        }

        // 提交事务
        transaction.commit();
        // 引入延迟
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if(isDatabaseEmpty(backupPath, "diary_entries"))
        {
            std::cout << "Backup completed Failed.\n";
            ret = false;
        }
        else
        {
            std::cout << "Backup completed Success.\n";
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error during backup: " << e.what() << "\n";
    }
    return ret;
#endif 
}

void initializeDatabase(const std::string DB_FILE)
{
#if 0    //C接口
    sqlite3 *db;
    int result = sqlite3_open(DB_FILE, &db);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
        exit(EXIT_FAILURE);
    }
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS diary_entries (id INTEGER PRIMARY KEY AUTOINCREMENT, entry TEXT, priority TEXT, time TEXT);";
    result = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error creating table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_close(db);
#endif
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
bool clearAllEntries(const std::string& dbFilePath)
{
    // 打开数据库连接
    sqlite3* db;
    if (sqlite3_open(dbFilePath.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 执行 SQL 命令来删除所有记录
    const char* sql = "DELETE FROM diary_entries;"; // 请替换为你的表名
    if (sqlite3_exec(db, sql, nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error clearing entries: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // 关闭数据库连接
    sqlite3_close(db);

    std::cout << "All entries cleared successfully." << std::endl;
    return true;
}

// 数据恢复函数
bool restoreDatabase(const std::string& backupFilePath, const std::string& dbFilePath)
{
    // 打开备份数据库连接
    sqlite3* backupDB;
    if (sqlite3_open(backupFilePath.c_str(), &backupDB) != SQLITE_OK)
    {
        std::cerr << "Error opening backup database: " << sqlite3_errmsg(backupDB) << std::endl;
        return false;
    }

    // 打开主数据库连接
    sqlite3* mainDB;
    if (sqlite3_open(dbFilePath.c_str(), &mainDB) != SQLITE_OK)
    {
        std::cerr << "Error opening main database: " << sqlite3_errmsg(mainDB) << std::endl;
        sqlite3_close(backupDB);
        return false;
    }

    // 开始数据库事务
    if (sqlite3_exec(mainDB, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error starting transaction: " << sqlite3_errmsg(mainDB) << std::endl;
        sqlite3_close(backupDB);
        sqlite3_close(mainDB);
        return false;
    }

    // 使用 ATTACH 命令连接备份数据库
    std::string attachSQL = "ATTACH DATABASE '" + backupFilePath + "' AS backupDB;";
    if (sqlite3_exec(mainDB, attachSQL.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error attaching backup database: " << sqlite3_errmsg(mainDB) << std::endl;
        sqlite3_exec(mainDB, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_close(backupDB);
        sqlite3_close(mainDB);
        return false;
    }

    int result = sqlite3_table_column_metadata(backupDB, nullptr, "diary_entries", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error checking if table exists: " << sqlite3_errmsg(backupDB) << std::endl;
        // 可以选择返回 false 或者采取其他处理措施
        return false;
    }

    // 执行 SQL 命令来复制数据
    const char* copySQL = "INSERT INTO diary_entries SELECT * FROM backupDB.diary_entries;"; // 请替换为你的表名
    if (sqlite3_exec(mainDB, copySQL, nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error restoring data: " << sqlite3_errmsg(mainDB) << std::endl;
        sqlite3_exec(mainDB, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_close(backupDB);
        sqlite3_close(mainDB);
        return false;
    }

    // 提交数据库事务
    if (sqlite3_exec(mainDB, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error committing transaction: " << sqlite3_errmsg(mainDB) << std::endl;
        sqlite3_close(backupDB);
        sqlite3_close(mainDB);
        return false;
    }

    // 分离备份数据库
    if (sqlite3_exec(mainDB, "DETACH DATABASE backupDB;", nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "Error detaching backup database: " << sqlite3_errmsg(mainDB) << std::endl;
    }

    // 关闭数据库连接
    sqlite3_close(backupDB);
    sqlite3_close(mainDB);

    std::cout << "Data restored successfully." << std::endl;
    return true;
}

