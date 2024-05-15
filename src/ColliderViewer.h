#ifndef COLLIDER_VIEWER_H_
#define COLLIDER_VIEWER_H_

#include "Renderer.h"
#include "DBManager.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Vector2.h"

class ColliderViewer {
public:
    ColliderViewer(Vector2<float> animPos_, DBManager *dbManager_);

    void setCurrentFrame(int currentFrame_);
    void setDuration(int duration_);

    void proceed();

    void updateMousePos(const Vector2<float> mouseWorldPos_);
    bool attachPoint();
    void detachPoint();

    void draw(Renderer &renderer_, Camera &cam_);

    void setFile(const std::string &filepath_);
    void resetFile();

private:
    std::shared_ptr<DBManager> m_db;
    Vector2<float> m_animPos;

    Vector2<float> m_lvlSize;
    std::vector<collidergroupdata> m_colliderGroups;

    colliderdata *m_closestCollider = nullptr;

    /*
    00  10
    01  11
    */
    Vector2<int> m_attachedPoint;

    const float m_maxRange = 100.0f;

    bool m_shown = false;
    bool m_attached = false;

    std::string m_filepath;

    int m_currentFrame = 1;
    int m_duration = 1;

};

#endif