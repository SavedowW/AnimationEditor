#ifndef ANIM_VIEW_LEVEL_H_
#define ANIM_VIEW_LEVEL_H_

#include "Level.h"
#include "EngineAnimation.h"

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

    EngineAnimation *m_anim;
    uint32_t currentFrame = -1;
    Camera m_camera;
    bool m_holdingCamera = false;
    bool m_winOpen = true;
};

#endif