#include "GuidelineManager.h"

GuidelineManager::GuidelineManager(const Vector2<float> lvlSize_) :
    m_lvlSize(lvlSize_)
{
}

void GuidelineManager::addGuideline(const Guideline &guideline_)
{
    m_guidelines.push_back(guideline_);
}

void GuidelineManager::updateMousePos(const Vector2<float> mouseWorldPos_)
{
    float minrange = -1;
    int id = 0;
    for (int i = 0; i < m_guidelines.size(); ++i)
    {
        float range = 0;
        if (m_guidelines[i].m_axis == Guideline::LineAxis::HORIZONTAL)
            range = abs(m_guidelines[i].m_coord - mouseWorldPos_.y);
        else
            range = abs(m_guidelines[i].m_coord - mouseWorldPos_.x);

        if (minrange == -1 || range < minrange)
        {
            minrange = range;
            id = i;
        }
    }

    if (minrange != -1 && minrange <= m_maxRange)
        m_closestLineIndex = id;
    else
        m_closestLineIndex = -1;
}

void GuidelineManager::draw(Renderer &renderer_, Camera &cam_)
{
    for (int i = 0; i < m_guidelines.size(); ++i)
    {
        auto &line = m_guidelines[i];
        SDL_Color col = (i == m_closestLineIndex ? SDL_Color{100, 100, 255, 255} : SDL_Color{0, 0, 200, 255});
        if (line.m_axis == Guideline::LineAxis::HORIZONTAL)
        {
            renderer_.drawLine({0, line.m_coord}, {m_lvlSize.x, line.m_coord}, col, cam_);
        }
        else
        {
            renderer_.drawLine({line.m_coord, 0.0f}, {line.m_coord, m_lvlSize.y}, col, cam_);
        }
    }

}
