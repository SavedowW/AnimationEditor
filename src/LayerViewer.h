#ifndef LAYER_VIEWER_H_
#define LAYER_VIEWER_H_

#include "Renderer.h"
#include "EngineAnimation.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Vector2.h"

class LayerViewer {
public:
    LayerViewer(const Vector2<float> lvlSize_, Renderer *ren_, EngineAnimation *anim_ = nullptr);
    void setAnim(EngineAnimation *anim_);

    void proceed();

    void draw(Renderer &renderer_, Camera &cam_, int currentFrame_, float levelOfGround_);

private:
    struct LayerData
    {
        size_t m_layerId = 0;
        float m_alpha = 0;
    };

    std::vector<LayerData> m_layers;

    Vector2<float> m_lvlSize;
    EngineAnimation *m_anim;
    Renderer *m_ren;

    std::array<float[3], 3> m_layerColorMods = {
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f
        };
};

#endif