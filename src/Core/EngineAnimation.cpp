#include "EngineAnimation.h"

EngineAnimation::EngineAnimation()
{
    m_framesData.addPropertyValue(0, std::move(0));
}

void EngineAnimation::saveAnimation(const std::string &path_, int blurRange_, float blurScaler_)
{
    m_rw = SDL_RWFromFile(path_.c_str(), "w+b");

    int version = 1;

    Uint32 uncompressed_size = m_realWidth * m_realHeight * 4;
    int max_lz4_size = LZ4_compressBound (uncompressed_size);
    m_compressionBuffer = new char[max_lz4_size];

    SDL_RWwrite(m_rw, &version, sizeof(version), 1);
    SDL_RWwrite(m_rw, &m_width, sizeof(m_width), 1);
    SDL_RWwrite(m_rw, &m_height, sizeof(m_height), 1);
    SDL_RWwrite(m_rw, &m_realWidth, sizeof(m_realWidth), 1);
    SDL_RWwrite(m_rw, &m_realHeight, sizeof(m_realHeight), 1);
    SDL_RWwrite(m_rw, &m_frameCount, sizeof(m_frameCount), 1);

    SDL_RWwrite(m_rw, &m_origin.x, sizeof(m_origin.x), 1);
    SDL_RWwrite(m_rw, &m_origin.y, sizeof(m_origin.y), 1);

    auto frameDataLen = m_framesData.getValuesCount();
    SDL_RWwrite(m_rw, &frameDataLen, sizeof(frameDataLen), 1);
    for (int i = 0; i < frameDataLen; ++i)
    {
        auto &data = m_framesData.getValuePair(i);
        SDL_RWwrite(m_rw, &data.first, sizeof(data.first), 1);
        SDL_RWwrite(m_rw, &data.second, sizeof(data.second), 1);
    }
    SDL_RWwrite(m_rw, &m_duration, sizeof(m_duration), 1);

    std::cout << "Saving regular sprites\n";
    for (int i = 0; i < m_frameCount; ++i)
    {
        std::cout << "Sprite " << i + 1 << m_frameCount << std::endl;
        saveSurfaceLZ4(m_surfaces[i], uncompressed_size, max_lz4_size);
    }

    std::cout << "Saving white sprites\n";
    for (int i = 0; i < m_frameCount; ++i)
    {
        std::cout << "Sprite " << i + 1 << m_frameCount << std::endl;
        if (m_whiteSurfaces[i] == nullptr)
            m_whiteSurfaces[i] = toPureWhite(m_surfaces[i], blurRange_, blurScaler_);
        saveSurfaceLZ4(m_whiteSurfaces[i], uncompressed_size, max_lz4_size);
    }

    std::cout << "DONE\n";

    SDL_RWclose(m_rw);

    delete [] m_compressionBuffer;
}

void EngineAnimation::loadAnimation(const std::string &path_, Renderer &ren_)
{
    m_rw = SDL_RWFromFile(path_.c_str(), "r+b");

    int version = 1;

    SDL_RWread(m_rw, &version, sizeof(version), 1);
    SDL_RWread(m_rw, &m_width, sizeof(m_width), 1);
    SDL_RWread(m_rw, &m_height, sizeof(m_height), 1);
    SDL_RWread(m_rw, &m_realWidth, sizeof(m_realWidth), 1);
    SDL_RWread(m_rw, &m_realHeight, sizeof(m_realHeight), 1);
    SDL_RWread(m_rw, &m_frameCount, sizeof(m_frameCount), 1);

    SDL_RWread(m_rw, &m_origin.x, sizeof(m_origin.x), 1);
    SDL_RWread(m_rw, &m_origin.y, sizeof(m_origin.y), 1);

    int frameDataLen;
    SDL_RWread(m_rw, &frameDataLen, sizeof(frameDataLen), 1);
    for (int i = 0; i < frameDataLen; ++i)
    {
        uint32_t timeMark;
        int value;
        SDL_RWread(m_rw, &timeMark, sizeof(timeMark), 1);
        SDL_RWread(m_rw, &value, sizeof(value), 1);
        m_framesData.addPropertyValue(timeMark, std::move(value));
    }
    SDL_RWread(m_rw, &m_duration, sizeof(m_duration), 1);


    for (int i = 0; i < m_frameCount; ++i)
    {
        SDL_Surface *tmp = loadSurfaceLZ4();
        m_surfaces.push_back(tmp);
        m_textures.push_back(ren_.createTextureFromSurface(tmp));
    }

    for (int i = 0; i < m_frameCount; ++i)
    {
        SDL_Surface *tmp = loadSurfaceLZ4();
        m_whiteSurfaces.push_back(tmp);
    }

    SDL_RWclose(m_rw);
}

void EngineAnimation::addFrame(const std::string &path_, Renderer &ren_)
{
    SDL_Surface *frame = IMG_Load(path_.c_str());
    m_surfaces.push_back(frame);
    m_whiteSurfaces.push_back(nullptr);
    m_textures.push_back(ren_.createTextureFromSurface(frame));

    if (m_frameCount == 0)
    {
        m_width = frame->w;
        m_height = frame->h;
        m_realWidth = m_width;
        m_realHeight = m_height;
    }

    m_frameCount++;
}

void EngineAnimation::setFrame(uint32_t frameId_, int spriteId_)
{
    m_framesData.addPropertyValue(frameId_, std::move(spriteId_));
}

void EngineAnimation::setDuration(uint32_t duration_)
{
    m_duration = duration_;
}

void EngineAnimation::scaleToHeight(int height_)
{
    float scale = (float)m_realHeight / (float)height_;
    m_height = height_;
    m_width = m_realWidth / scale;
}

SDL_Texture *EngineAnimation::operator[](uint32_t frame_)
{
    return m_textures[m_framesData[frame_ % m_duration]];
}

EngineAnimation::~EngineAnimation()
{
    for (auto *el : m_surfaces)
        SDL_FreeSurface(el);

    for (auto *el : m_whiteSurfaces)
        SDL_FreeSurface(el);

    for (auto *el : m_textures)
        SDL_DestroyTexture(el);
}

void EngineAnimation::saveSurfaceLZ4(SDL_Surface *sur_, Uint32 uncompressed_size_, int max_lz4_size_)
{
    SDL_Surface *surcpy = SDL_ConvertSurfaceFormat(sur_, SDL_PIXELFORMAT_ARGB8888, 0);

    const char* uncompressed_buffer = (const char*)(surcpy->pixels);
    
    int true_size = LZ4_compress_HC(uncompressed_buffer, m_compressionBuffer,
                                uncompressed_size_, max_lz4_size_,
                                LZ4HC_CLEVEL_MAX);

    auto sz = surcpy->h * surcpy->w;
    SDL_RWwrite(m_rw, &(true_size), sizeof(true_size), 1);
    auto res = SDL_RWwrite(m_rw, m_compressionBuffer, 1, true_size);

    if (res < true_size)
    {
        std::cout << "Writing " << true_size << " bytes\n";
        std::cout << res << std::endl;
        std::cout << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(surcpy);
}

SDL_Surface *EngineAnimation::loadSurfaceLZ4()
{
    SDL_Surface *tar = SDL_CreateRGBSurface(0, m_realWidth, m_realHeight, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    Uint32 uncompressed_size = tar->w * tar->h * tar->format->BytesPerPixel;

    int compressed_size;
    SDL_RWread (m_rw, &compressed_size, sizeof(compressed_size), 1);

    char* compressed_buffer = new char[compressed_size];
    auto res = SDL_RWread(m_rw, compressed_buffer, 1, compressed_size);
    if (res == 0)
    {
        std::cout << res << std::endl;
        std::cout << SDL_GetError() << std::endl;
        SDL_FreeSurface(tar);
        delete [] compressed_buffer;
        return nullptr;
    }

    char* uncompressed_buffer = (char*)(tar->pixels);
    LZ4_decompress_safe(compressed_buffer, uncompressed_buffer, compressed_size, uncompressed_size);


    delete [] compressed_buffer;
    return tar;
}

SDL_Surface *EngineAnimation::toPureWhite(SDL_Surface *sur_, int blurRange_, float blurScaler_)
{
    SDL_Surface *nsur = SDL_CreateRGBSurface(0, sur_->w, sur_->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_FillRect(nsur, nullptr, SDL_MapRGBA(nsur->format, 255, 255, 255, 0));

    for (int y = 0; y < nsur->h; ++y)
    {
        for (int x = 0; x < nsur->w; ++x)
        {
            uint8_t *pixel1 = ((uint8_t*)sur_->pixels + sur_->pitch * y + x * 4);
            uint8_t *pixel2 = ((uint8_t*)nsur->pixels + nsur->pitch * y + x * 4);
            pixel2[3] = pixel1[3];
            if (pixel1[3] == 0)
                continue;

            int currentAlpha = pixel1[3];

            for (int i = 1; i < blurRange_; ++i)
            {
                currentAlpha /= blurScaler_;
                auto pts = utils::getAreaEdgePoints(sur_->w, sur_->h, {x, y}, i);
                for (const auto &pt : pts)
                {
                    uint8_t *pixel3 = ((uint8_t*)nsur->pixels + nsur->pitch * pt.y + pt.x * 4);
                    if (pixel3[3] < currentAlpha)
                        pixel3[3] = currentAlpha;
                }
            }
        }
    }

    return nsur;
}
