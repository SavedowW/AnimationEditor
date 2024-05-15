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

int ColliderManager::createColliderGroup(const std::string &groupname_, const std::string &filepath_)
{
    auto *stmt = m_db->prepareStmt("INSERT INTO collider_groups(group_name, file_id) VALUES (?, (SELECT file_id FROM used_files WHERE file_path=?))");
    sqlite3_bind_text(stmt, 1, groupname_.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, filepath_.c_str(), -1, SQLITE_STATIC);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    auto res = m_db->getLastInsertedID();
    std::cout << res << std::endl;
    return res;
}

int ColliderManager::createCollider(int cldgroup_, const colliderdata &cld_)
{
    auto *stmt = m_db->prepareStmt("INSERT INTO colliders(x, y, w, h, group_id) VALUES (?, ?, ?, ?, ?)");
    sqlite3_bind_double(stmt, 1, cld_.m_pos.x);
    sqlite3_bind_double(stmt, 2, cld_.m_pos.y);
    sqlite3_bind_double(stmt, 3, cld_.m_size.x);
    sqlite3_bind_double(stmt, 4, cld_.m_pos.y);
    sqlite3_bind_int(stmt, 5, cldgroup_);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    auto res = m_db->getLastInsertedID();
    std::cout << res << std::endl;
    return res;
}

void ColliderManager::updateCollider(const colliderdata &cld_)
{
    auto *stmt = m_db->prepareStmt("UPDATE colliders SET x = ?, y = ?, w = ?, h = ? WHERE collider_id = ?");
    sqlite3_bind_double(stmt, 1, cld_.m_pos.x);
    sqlite3_bind_double(stmt, 2, cld_.m_pos.y);
    sqlite3_bind_double(stmt, 3, cld_.m_size.x);
    sqlite3_bind_double(stmt, 4, cld_.m_size.y);
    sqlite3_bind_double(stmt, 5, cld_.m_id);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}
