#ifndef DATABASE_MANAGER_H_
#define DATABASE_MANAGER_H_
#include "UsedFilesManager.h"
#include "GuidelinesManager.h"

class DBManager
{
public:
    DBManager(std::shared_ptr<DBAccessor> db_);
    void pushFile(const std::string &filepath_, const std::string &filename_);
    UsedFilesManager &getFileManager();
    GuidelinesManager *getGuidelineManager();

private:
    UsedFilesManager m_filemanager;
    GuidelinesManager m_guidelineManager;
    std::shared_ptr<DBAccessor> m_db;
};

#endif