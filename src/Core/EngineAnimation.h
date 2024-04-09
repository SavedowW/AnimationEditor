#ifndef ENGINE_ANIMATION_H_
#define ENGINE_ANIMATION_H_

#include "Renderer.h"
#include "TimelineProperty.h"

class EngineAnimation
{
public:
    EngineAnimation();

    void saveAnimation(const std::string &path_, int blurRange_, float blurScaler_);

    void loadAnimation(const std::string &path_, Renderer &ren_);

    void addFrame(const std::string &path_, Renderer &ren_);

    void setFrame(uint32_t frameId_, int spriteId_);
    void setDuration(uint32_t duration_);
    void scaleToHeight(int height_);

    SDL_Texture *operator[](uint32_t frame_);

    ~EngineAnimation();

//private:
    std::vector<SDL_Surface *> m_surfaces;
    std::vector<SDL_Surface *> m_whiteSurfaces;
    std::vector<SDL_Texture *> m_textures;
    int m_width = 0;
    int m_height = 0;
    int m_realWidth = 0;
    int m_realHeight = 0;
    int m_frameCount = 0;
    SDL_RWops *m_rw = nullptr;

    TimelineProperty<int> m_framesData;
    uint32_t m_duration = 0;
    Vector2<float> m_origin;

    void saveSurface(SDL_Surface *sur_);

    SDL_Surface *loadSurface();

    // Create pure white version of surface
    SDL_Surface *toPureWhite(SDL_Surface *sur_, int blurRange_, float blurScaler_);
};

#endif