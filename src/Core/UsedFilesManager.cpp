#include "UsedFilesManager.h"

UsedFilesManager::UsedFilesManager(std::shared_ptr<DBAccessor> db_) :
    m_db(db_)
{
}

void UsedFilesManager::pushFile(const std::string &filepath_, const std::string &filename_)
{
    auto id = find(filepath_);
    if (id == -1)
        create(filepath_, filename_);
    else
        updateDate(id);
}

void UsedFilesManager::create(const std::string &filepath_, const std::string &filename_)
{
    auto *stmt = m_db->prepareStmt("INSERT INTO used_files(file_path, file_name, use_datetime) VALUES ( ?, ?, datetime() )");
    sqlite3_bind_text(stmt, 1, filepath_.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, filename_.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

int UsedFilesManager::find(const std::string &filepath_)
{
    int res = -1;
    auto *stmt = m_db->prepareStmt("SELECT file_id FROM used_files WHERE file_path = ?");
    sqlite3_bind_text(stmt, 1, filepath_.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        res = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    
    return res;
}

void UsedFilesManager::updateDate(int id_)
{
    auto *stmt = m_db->prepareStmt("UPDATE used_files SET use_datetime = datetime() WHERE file_id = ?");
    sqlite3_bind_int(stmt, 1, id_);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

void UsedFilesManager::deleteFile(int id_)
{
    auto *stmt = m_db->prepareStmt("DELETE FROM used_files WHERE file_id = ?");
    sqlite3_bind_int(stmt, 1, id_);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

std::vector<filedata> UsedFilesManager::getLastFiles(int count_)
{
    std::vector<filedata> resvec;
    auto *stmt = m_db->prepareStmt("SELECT file_id, file_path, file_name from used_files ORDER BY use_datetime DESC LIMIT ?");
    sqlite3_bind_int(stmt, 1, count_);

    int res = SQLITE_ROW;
    while (res == SQLITE_ROW)
    {
        res = sqlite3_step(stmt);
        if (res == SQLITE_ROW)
        {
            filedata fd;
            fd.id = sqlite3_column_int(stmt, 0);
            fd.filepath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            fd.filename = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            resvec.push_back(fd);
        }
    }

    sqlite3_finalize(stmt);

    return resvec;
}
