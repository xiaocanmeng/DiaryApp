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
    std::string DB_BACKUPFILE;
    std::string getTime();
    void addEntry();
    void viewEntries();
    void deleteEntry();
    void clearConsole();
    void backupDiary();
};

#endif  // DIARYAPP_H
