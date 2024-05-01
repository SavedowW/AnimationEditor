#ifndef USED_FILES_MANAGER_H_
#define USED_FILES_MANAGER_H_
#include "DBAccessor.h"
#include <vector>
#include <memory>

struct filedata
{
    int id;
    std::string filepath;
    std::string filename;
};

class UsedFilesManager
{
public:
    UsedFilesManager(std::shared_ptr<DBAccessor> db_);
    void pushFile(const std::string &filepath_, const std::string &filename_);

    void create(const std::string &filepath_, const std::string &filename_);
    int find(const std::string &filepath_);
    void updateDate(int id_);
    void deleteFile(int id_);
    std::vector<filedata> getLastFiles(int count_);

private:
    std::shared_ptr<DBAccessor> m_db;

};

#endif