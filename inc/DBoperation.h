#include <iostream>
#include <sqlite3.h>

void initializeDatabase(const char* DB_FILE);
bool backupSQLiteDatabase(const std::string &sourcePath, const std::string &backupPath);
bool clearAllEntries(const std::string& dbFilePath);
bool restoreDatabase(const std::string& backupFilePath, const std::string& dbFilePath);