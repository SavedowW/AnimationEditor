#include "DBManager.h"
#include <algorithm>

DBManager::DBManager(std::shared_ptr<DBAccessor> db_) :
    m_db(db_),
    m_filemanager(db_),
    m_guidelineManager(db_),
    m_colliderManager(db_)
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

    stmt = m_db->prepareStmt(
    "CREATE TABLE IF NOT EXISTS guidelines ( "
	"guideline_id INTEGER PRIMARY KEY, "
	"is_vertical INTEGER NOT NULL, "
	"coord REAL NOT NULL )"
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    stmt = m_db->prepareStmt(
    "CREATE TABLE IF NOT EXISTS collider_groups ( "
    "group_id INTEGER PRIMARY KEY, "
    "group_name TEXT, "
    "color_r REAL, "
    "color_g REAL, "
    "color_b REAL, "
    "file_id INTEGER, "
    "FOREIGN KEY (file_id) REFERENCES  used_files (file_id) ) "
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    stmt = m_db->prepareStmt(
    "CREATE TABLE IF NOT EXISTS colliders ( "
    "collider_id INTEGER PRIMARY KEY, "
    "x REAL, "
    "y REAL, "
    "w REAl, "
    "h REAL, "
    "first_frame int, "
    "last_frame int, "
    "group_id INTEGER, "
    "FOREIGN KEY (group_id) REFERENCES  collider_groups (group_id) ) "
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    stmt = m_db->prepareStmt(
    "CREATE TRIGGER IF NOT EXISTS auto_delete_collider "
    "BEFORE DELETE ON collider_groups "
    "BEGIN "
    "DELETE FROM colliders WHERE colliders.group_id = old.group_id; "
    "END "
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    stmt = m_db->prepareStmt(
    "CREATE TRIGGER IF NOT EXISTS auto_delete_collider_group "
    "BEFORE DELETE ON used_files "
    "BEGIN "
    "DELETE FROM collider_groups WHERE collider_groups.file_id = old.file_id; "
    "END "
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    m_guidelineManager.getGuidelines();
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

std::vector<collidergroupdata> DBManager::getColliders(const std::string &filepath_)
{
    auto path = filepath_;
    std::replace(path.begin(), path.end(), '\\', '/');
    
    auto resvec = m_colliderManager.getRawGroups(filepath_);
    for (auto &el : resvec)
    {
        m_colliderManager.getColliders(el);
    }

    return resvec;
}

int DBManager::createColliderGroup(const std::string &groupname_, const std::string &filepath_)
{
    auto path = filepath_;
    std::replace(path.begin(), path.end(), '\\', '/');

    return m_colliderManager.createColliderGroup(groupname_, path);
}

UsedFilesManager &DBManager::getFileManager()
{
    return m_filemanager;
}

GuidelinesManager *DBManager::getGuidelineManager()
{
    return &m_guidelineManager;
}

ColliderManager &DBManager::getColliderManager()
{
    return m_colliderManager;
}
