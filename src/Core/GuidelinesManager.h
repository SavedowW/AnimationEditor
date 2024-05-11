#ifndef GUIDELINE_MANAGER_H_
#define GUIDELINE_MANAGER_H_
#include "DBAccessor.h"
#include <vector>
#include <memory>

struct Guideline {
    int m_id;

    enum class LineAxis {
        HORIZONTAL,
        VERTICAL
    } m_axis;

    float m_coord;

    bool m_dirtyflag = false;
};

class GuidelinesManager
{
public:
    GuidelinesManager(std::shared_ptr<DBAccessor> db_);
    std::vector<Guideline> getGuidelines();

    void update(const Guideline &guideline);
    void deleteGuideline(int id_);
    int createGuideline();
    

private:
    std::shared_ptr<DBAccessor> m_db;

};

#endif