#include "GuidelinesManager.h"

GuidelinesManager::GuidelinesManager(std::shared_ptr<DBAccessor> db_) :
    m_db(db_)
{
}

std::vector<Guideline> GuidelinesManager::getGuidelines()
{
    std::vector<Guideline> resvec;

    auto *stmt = m_db->prepareStmt("SELECT guideline_id, is_vertical, coord from guidelines");

    int res = SQLITE_ROW;
    while (res == SQLITE_ROW)
    {
        res = sqlite3_step(stmt);
        if (res == SQLITE_ROW)
        {
            Guideline gl;
            gl.m_id = sqlite3_column_int(stmt, 0);
            gl.m_axis = (Guideline::LineAxis)sqlite3_column_int(stmt, 1);
            gl.m_coord = sqlite3_column_double(stmt, 2);
            gl.m_dirtyflag = false;
            resvec.push_back(gl);
        }
    }

    sqlite3_finalize(stmt);

    return resvec;
}

void GuidelinesManager::update(const Guideline &guideline)
{
    auto *stmt = m_db->prepareStmt("UPDATE guidelines SET is_vertical = ?, coord = ? WHERE guideline_id = ?");
    sqlite3_bind_int(stmt, 1, (int)guideline.m_axis);
    sqlite3_bind_double(stmt, 2, guideline.m_coord);
    sqlite3_bind_int(stmt, 3, guideline.m_id);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

void GuidelinesManager::deleteGuideline(int id_)
{
    auto *stmt = m_db->prepareStmt("DELETE FROM guidelines WHERE guideline_id = ?");
    sqlite3_bind_int(stmt, 1, id_);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
}

int GuidelinesManager::createGuideline()
{
    auto *stmt = m_db->prepareStmt("INSERT INTO guidelines ( is_vertical, coord ) VALUES (0, 0)");

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    auto res = m_db->getLastInsertedID();
    std::cout << res << std::endl;
    return res;
}
