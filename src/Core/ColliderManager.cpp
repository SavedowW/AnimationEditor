#include "ColliderManager.h"

ColliderManager::ColliderManager(std::shared_ptr<DBAccessor> db_) :
    m_db(db_)
{
}

void ColliderManager::getColliders(collidergroupdata &collidergroup_)
{
    auto *stmt = m_db->prepareStmt("SELECT collider_id, x, y, w, h FROM colliders where group_id = ?");
    sqlite3_bind_int(stmt, 1, collidergroup_.m_id);

    int res = SQLITE_ROW;
    while (res == SQLITE_ROW)
    {
        res = sqlite3_step(stmt);
        if (res == SQLITE_ROW)
        {
            colliderdata cld;
            cld.m_id = sqlite3_column_int(stmt, 0);
            cld.m_pos.x = sqlite3_column_double(stmt, 1);
            cld.m_pos.y = sqlite3_column_double(stmt, 2);
            cld.m_size.x = sqlite3_column_double(stmt, 3);
            cld.m_size.y = sqlite3_column_double(stmt, 4);
            cld.m_dirtyflag = false;
            collidergroup_.m_colliders.push_back(cld);
        }
    }

    sqlite3_finalize(stmt);

}

std::vector<collidergroupdata> ColliderManager::getRawGroups(const std::string &filepath_)
{
    std::vector<collidergroupdata> resvec;

    auto *stmt = m_db->prepareStmt("SELECT group_id, group_name	FROM collider_groups LEFT JOIN used_files on collider_groups.file_id = used_files.file_id WHERE file_path=?");
    sqlite3_bind_text(stmt, 1, filepath_.c_str(), -1, SQLITE_STATIC);

    int res = SQLITE_ROW;
    while (res == SQLITE_ROW)
    {
        res = sqlite3_step(stmt);
        if (res == SQLITE_ROW)
        {
            collidergroupdata cg;
            cg.m_id = sqlite3_column_int(stmt, 0);
            cg.m_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            cg.m_dirtyflag = false;
            resvec.push_back(cg);
        }
    }

    sqlite3_finalize(stmt);

    return resvec;
}
