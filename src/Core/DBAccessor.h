#ifndef DB_ACCESSOR_H_
#define DB_ACCESSOR_H_
#include <string>
#include <iostream>
#include <sqlite3.h>

class DBAccessor
{
public:
    DBAccessor(const std::string &filename_);
    sqlite3_stmt *prepareStmt(const std::string &line);
    const char *getErr();
    sqlite3_int64 getLastInsertedID();

    ~DBAccessor();

    bool isValid() const;

private:
    sqlite3* m_db = nullptr; 

};

#endif