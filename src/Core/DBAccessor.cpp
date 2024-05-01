#include "DBAccessor.h"

DBAccessor::DBAccessor(const std::string &filename_)
{
    int exit = sqlite3_open("example.db", &m_db); 
  
    if (exit) { 
        std::cerr << "Error open DB " << sqlite3_errmsg(m_db) << std::endl; 
        return;
    } 
    else
        std::cout << "Opened Database Successfully!" << std::endl; 
}

sqlite3_stmt *DBAccessor::prepareStmt(const std::string &line)
{
    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(m_db, line.c_str(), line.size(), &stmt, nullptr);
    return stmt;
}

const char *DBAccessor::getErr()
{
    return sqlite3_errmsg(m_db);
}

DBAccessor::~DBAccessor()
{
    if (m_db)
        sqlite3_close(m_db); 
}

bool DBAccessor::isValid() const
{
    return m_db != nullptr;
}
