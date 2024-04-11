#ifndef ANIM_VIEW_LEVEL_H_
#define ANIM_VIEW_LEVEL_H_

#include "Level.h"
#include "EngineAnimation.h"
#include "ImTimelinePropertyInt.h"

class AnimViewLevel : public Level
{
public:
    AnimViewLevel(Application *application_, const Vector2<float> &size_, int lvlId_);
    virtual void enter() override;
    void receiveInput(EVENTS event, const float scale_) override;
    void receiveMouseMovement(const Vector2<float> &offset_) override;

protected:
    void update() override;
    void draw() override;

    ImTimelinePropertyInt m_animFrames;
    EngineAnimation *m_anim;
    int currentFrame = -1;
    Camera m_camera;
    bool m_holdingCamera = false;
    bool m_winOpen = true;
    bool m_loopAnimation = true;
    bool m_runAnimation = true;

    std::vector<int> vals;
};

#endif