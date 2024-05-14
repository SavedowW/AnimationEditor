#ifndef COLLIDER_MANAGER_H_
#define COLLIDER_MANAGER_H_
#include "DBAccessor.h"
#include "Vector2.h"
#include <vector>
#include <memory>

struct colliderdata
{
    int m_id;
    Vector2<float> m_pos;
    Vector2<float> m_size;
    bool m_dirtyflag = false;
};

struct collidergroupdata
{
    int m_id;
    std::string m_name;
    std::vector<colliderdata> m_colliders;
    bool m_dirtyflag = false;
};

class ColliderManager
{
public:
    ColliderManager(std::shared_ptr<DBAccessor> db_);    
    void getColliders(collidergroupdata &collidergroup_);
    std::vector<collidergroupdata> getRawGroups(const std::string &filepath_);

private:
    std::shared_ptr<DBAccessor> m_db;

};

#endif