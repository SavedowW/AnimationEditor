#ifndef PARTICLE_MANAGER_H_
#define PARTICLE_MANAGER_H_

#include "Renderer.h"
#include "Vector2.h"
#include <vector>
#include "AnimationManager.h"

struct Particle
{
    Particle(Animation &&anim_);
    void update();

    Vector2<float> m_pos;
    Vector2<float> m_velocity;
    Vector2<float> m_accel;

    SDL_RendererFlip m_flip;
    float m_angle;
    float m_scale;

    Animation m_anim;
    int m_lifetime;
};

struct ParticleSpawnData
{
    Vector2<float> m_pos;
    Vector2<float> m_velocity;
    Vector2<float> m_accel;
    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
    float m_angle = 0;
    float m_scale = 1;
    int m_lifeTime = -1;

    int m_animation;
    LOOPMETHOD m_loopMethod = LOOPMETHOD::JUMP_LOOP;
    int m_beginFrame = -1;
    int m_beginDirection = 1;
};

struct ParticlesSpawnData
{
    int m_count = 1;
    float m_angle = 0.0f;
    float m_scale = 1.0f;

    Vector2<float> m_baseVelocity;
    bool m_randVelocity = false;
    Vector2<int> m_velocityRange;

    float m_reverseAccel = 0.0f;
    Vector2<float> m_additionalAccel;

    int m_minLifeTime = 1;
    int m_maxLifeTime = 1;

    int m_animation;
    LOOPMETHOD m_loopMethod = LOOPMETHOD::JUMP_LOOP;
    int m_beginFrame = -1;
    int m_beginDirection = 1;
};

class ParticleManager
{
public:
    ParticleManager(Renderer *renderer_, AnimationManager *animManager_);
    void update();
    void draw(Camera &camera_);
    void spawnParticle(const ParticleSpawnData &partData_);
    void spawnParticles(const ParticlesSpawnData &partData_, const Vector2<float> &basePos_, ORIENTATION orientation_);

private:
    std::vector<Particle> m_particles;
    Renderer* m_renderer;
    AnimationManager* m_animationManager;
};

#endif