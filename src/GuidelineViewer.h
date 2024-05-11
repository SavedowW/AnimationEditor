#ifndef GUIDELINE_VIEWER_H_
#define GUIDELINE_VIEWER_H_

#include "Renderer.h"
#include "GuidelinesManager.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Vector2.h"

class GuidelineViewer {
public:
    GuidelineViewer(const Vector2<float> lvlSize_, GuidelinesManager *guidelineManager_);

    void proceed();

    void addGuideline(const Guideline &guideline_);

    void updateMousePos(const Vector2<float> mouseWorldPos_);
    void attachLine();
    void detachLine();

    void draw(Renderer &renderer_, Camera &cam_);

private:
    GuidelinesManager *m_guidelineManager;

    Vector2<float> m_lvlSize;
    std::vector<Guideline> m_guidelines;
    int m_closestLineIndex = -1;

    const float m_maxRange = 30.0f;

    bool m_editing = false;
    bool m_attached = false;

};

#endif