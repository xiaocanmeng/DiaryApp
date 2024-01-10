// DiaryApp.h
#ifndef DIARYAPP_H
#define DIARYAPP_H

#include <string>

class DiaryApp {
public:
    DiaryApp(const char *dbFile);
    void displayMenu();
    int startup();

private:
    const char *DB_FILE;
    std::string getTime();
    void initializeDatabase();
    void addEntry();
    void viewEntries();
    void deleteEntry();
    void clearConsole();
};

#endif  // DIARYAPP_H
