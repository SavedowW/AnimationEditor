#include "ParticleManager.h"

Particle::Particle(Animation &&anim_) :
    m_anim(std::forward<Animation>(anim_))
{

}

void Particle::update()
{
    if (m_lifetime >= 0)
    m_lifetime--;

    m_pos += m_velocity;
    m_anim.update();

    m_velocity += m_accel;
}

ParticleManager::ParticleManager(Renderer *renderer_, AnimationManager *animManager_) :
    m_renderer(renderer_),
    m_animationManager(animManager_)
{

}

void ParticleManager::update()
{
    int i = 0;
    while (i < m_particles.size())
    {
        m_particles[i].update();
        if (m_particles[i].m_lifetime < 0)
            m_particles.erase(m_particles.begin() + i);
        else
            i++;
    }
}

void ParticleManager::draw(Camera &camera_)
{
    for (auto &particle : m_particles)
    {
        auto tex = particle.m_anim.getSprite();
        auto size = particle.m_anim.getSize() * particle.m_scale;
        auto pos = particle.m_pos - (size / 2.0f);
        m_renderer->renderTexture(tex, pos.x, pos.y, size.x, size.y, camera_, particle.m_angle, particle.m_flip);
    }
}

void ParticleManager::spawnParticle(const ParticleSpawnData &partData_)
{
    Animation anim(*m_animationManager, partData_.m_animation, partData_.m_loopMethod, partData_.m_beginFrame, partData_.m_beginDirection);
    Particle part{std::move(anim)};
    part.m_pos = partData_.m_pos;
    part.m_velocity = partData_.m_velocity;
    part.m_lifetime = partData_.m_lifeTime;
    part.m_flip = partData_.m_flip;
    part.m_scale = partData_.m_scale;
    part.m_angle = partData_.m_angle;
    part.m_accel = partData_.m_accel;
    if (part.m_flip != SDL_FLIP_NONE)
        part.m_angle *= -1;
    m_particles.push_back(std::move(part));
}

void ParticleManager::spawnParticles(const ParticlesSpawnData &partData_, const Vector2<float> &basePos_, ORIENTATION orientation_)
{
    ParticleSpawnData psd;
    psd.m_angle = partData_.m_angle;
    psd.m_pos = basePos_;
    psd.m_scale = partData_.m_scale;
    psd.m_flip = (orientation_ == ORIENTATION::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    psd.m_animation = partData_.m_animation;
    psd.m_loopMethod = partData_.m_loopMethod;
    psd.m_beginFrame = partData_.m_beginFrame;
    psd.m_beginDirection = partData_.m_beginDirection;
    for (int i = 0; i < partData_.m_count; ++i)
    {
        psd.m_velocity = partData_.m_baseVelocity;
        if (partData_.m_randVelocity)
        {
            psd.m_velocity.x += partData_.m_baseVelocity.x + ((rand() % (partData_.m_velocityRange.x * 100)) - partData_.m_velocityRange.x * 100 / 2.0f) / 100.0f;
            psd.m_velocity.y += partData_.m_baseVelocity.y + ((rand() % (partData_.m_velocityRange.y * 100)) - partData_.m_velocityRange.y * 100 / 2.0f) / 100.0f;
        }

        psd.m_accel = partData_.m_additionalAccel + (psd.m_velocity * (-partData_.m_reverseAccel));

        if (partData_.m_minLifeTime == partData_.m_maxLifeTime)
        {
            psd.m_lifeTime = partData_.m_minLifeTime;
        }
        else
        {
            psd.m_lifeTime = (rand() % (partData_.m_maxLifeTime - partData_.m_minLifeTime + 1)) + partData_.m_minLifeTime;
        }

        spawnParticle(psd);
    }
}
