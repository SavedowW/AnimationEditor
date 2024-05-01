#include "DBManager.h"
#include <algorithm>

DBManager::DBManager(std::shared_ptr<DBAccessor> db_) :
    m_db(db_),
    m_filemanager(db_)
{
    auto *stmt = m_db->prepareStmt(
    "CREATE TABLE IF NOT EXISTS used_files (  "
	"file_id INTEGER PRIMARY KEY, "
	"file_path TEXT NOT NULL UNIQUE, "
	"file_name TEXT NOT NULL, "
	"use_datetime TEXT NOT NULL )"
    );

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

void DBManager::pushFile(const std::string &filepath_, const std::string &filename_)
{
    auto path = filepath_;
    std::replace(path.begin(), path.end(), '\\', '/');

    auto id = m_filemanager.find(path);
    if (id == -1)
        m_filemanager.create(path, filename_);
    else
        m_filemanager.updateDate(id);
}

UsedFilesManager &DBManager::getFileManager()
{
    return m_filemanager;
}
