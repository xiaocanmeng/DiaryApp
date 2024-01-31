// DiaryApp.h
#ifndef DIARYAPP_H
#define DIARYAPP_H

#include <string>

class DiaryApp {
public:
    DiaryApp(std::string dbFile);
    void displayMenu();
    int startup();

private:
    std::string DB_FILE;
    std::string DB_BACKUPFILE;
    std::string getTime();
    void addEntry();
    void viewEntries();
    void deleteEntry();
    void clearConsole();
    void backupDiary();
};

#endif  // DIARYAPP_H
