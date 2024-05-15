#ifndef DATABASE_MANAGER_H_
#define DATABASE_MANAGER_H_
#include "UsedFilesManager.h"
#include "GuidelinesManager.h"
#include "ColliderManager.h"

class DBManager
{
public:
    DBManager(std::shared_ptr<DBAccessor> db_);

    void pushFile(const std::string &filepath_, const std::string &filename_);
    std::vector<collidergroupdata> getColliders(const std::string &filepath_);
    int createColliderGroup(const std::string &groupname_, const std::string &filepath_);
    
    UsedFilesManager &getFileManager();
    GuidelinesManager *getGuidelineManager();
    ColliderManager &getColliderManager();

private:
    UsedFilesManager m_filemanager;
    GuidelinesManager m_guidelineManager;
    ColliderManager m_colliderManager;
    std::shared_ptr<DBAccessor> m_db;
};

#endif