#ifndef DATABASE_MANAGER_H_
#define DATABASE_MANAGER_H_
#include "UsedFilesManager.h"

class DBManager
{
public:
    DBManager(std::shared_ptr<DBAccessor> db_);
    void pushFile(const std::string &filepath_, const std::string &filename_);
    UsedFilesManager &getFileManager();

private:
    UsedFilesManager m_filemanager;
    std::shared_ptr<DBAccessor> m_db;
};

#endif