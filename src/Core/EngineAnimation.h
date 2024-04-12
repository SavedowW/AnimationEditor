#ifndef ENGINE_ANIMATION_H_
#define ENGINE_ANIMATION_H_

#include "Renderer.h"
#include "TimelineProperty.h"
#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"

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
    int m_width = 1;
    int m_height = 1;
    int m_realWidth = 1;
    int m_realHeight = 1;
    int m_frameCount = 0;
    SDL_RWops *m_rw = nullptr;

    TimelineProperty<int> m_framesData;
    int m_duration = 1;
    Vector2<float> m_origin;

    void saveSurfaceLZ4(SDL_Surface *sur_, Uint32 uncompressed_size_, int max_lz4_size_);
    SDL_Surface *loadSurfaceLZ4();


    char *m_compressionBuffer = nullptr;

    // Create pure white version of surface
    SDL_Surface *toPureWhite(SDL_Surface *sur_, int blurRange_, float blurScaler_);
};

#endif