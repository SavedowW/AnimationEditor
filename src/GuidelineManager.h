#ifndef GUIDELINE_MANAGER_H_
#define GUIDELINE_MANAGER_H_

#include "Renderer.h"

struct Guideline {
    enum class LineAxis {
        HORIZONTAL,
        VERTICAL
    } m_axis;

    float m_coord;
};

class GuidelineManager {
public:
    GuidelineManager(const Vector2<float> lvlSize_);

    void addGuideline(const Guideline &guideline_);

    void updateMousePos(const Vector2<float> mouseWorldPos_);
    void draw(Renderer &renderer_, Camera &cam_);

private:
    Vector2<float> m_lvlSize;
    std::vector<Guideline> m_guidelines;
    int m_closestLineIndex = -1;

    const float m_maxRange = 30.0f;

};

#endif