#include "AnimViewLevel.h"
#include "Application.h"

AnimViewLevel::AnimViewLevel(Application *application_, const Vector2<float> &size_, int lvlId_) :
    Level(application_, size_, lvlId_),
    m_camera(gamedata::stages::startingCameraPos, {gamedata::global::cameraWidth, gamedata::global::cameraHeight}, m_size)
{
    subscribe(EVENTS::RMB);
    subscribe(EVENTS::MOUSE_MOVEMENT);
    subscribe(EVENTS::WHEEL_SCROLL);
}

void AnimViewLevel::enter()
{
    Level::enter();

    m_camera.setPos(gamedata::stages::startingCameraPos);
    m_camera.setScale(gamedata::stages::startingCameraScale);

    auto path = m_application->getBasePath();

    Renderer *renderer = m_application->getRenderer();
    m_anim = new EngineAnimation();
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_1.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_2.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_3.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_4.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_5.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_6.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_7.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_8.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_9.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_10.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_11.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_12.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_13.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_14.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_15.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_16.png", *renderer);
    m_anim->addFrame(path + "/Resources/MoveProjectileCharAnim_17.png", *renderer);
    m_anim->scaleToHeight(495);
    m_anim->m_origin = {200, 495};
    m_anim->setDuration(60);

    m_anim->setFrame(6, 1);
    m_anim->setFrame(9, 3);
    m_anim->setFrame(14, 4);
    m_anim->setFrame(19, 5);
    m_anim->setFrame(23, 6);
    m_anim->setFrame(26, 7);
    m_anim->setFrame(30, 8);
    m_anim->setFrame(32, 9);
    m_anim->setFrame(34, 10);
    m_anim->setFrame(36, 11);
    m_anim->setFrame(38, 12);
    m_anim->setFrame(40, 13);
    m_anim->setFrame(44, 14);
    m_anim->setFrame(48, 15);
    m_anim->setFrame(50, 16);
}

void AnimViewLevel::receiveInput(EVENTS event, const float scale_)
{
    Level::receiveInput(event, scale_);
    
    if (event == EVENTS::RMB)
    {
        if (scale_ == 1.0f)
            m_holdingCamera = true;
        else
            m_holdingCamera = false;
    }
    else if (event == EVENTS::WHEEL_SCROLL)
    {
        m_camera.setScale(utils::clamp(m_camera.getScale() + scale_ * -0.1f, gamedata::stages::minCameraScale, gamedata::stages::maxCameraScale));
    }
}

void AnimViewLevel::receiveMouseMovement(const Vector2<float> &offset_)
{
    if (m_holdingCamera)
    {
        m_camera.setPos(m_camera.getPos() - offset_);
    }
}

void AnimViewLevel::update()
{
    m_camera.update();
    currentFrame++;

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    ImGui::ShowDemoWindow(&m_winOpen);
}

void AnimViewLevel::draw()
{
    ImGui::Render();

    auto &renderer = *m_application->getRenderer();
    renderer.fillRenderer(SDL_Color{ 25, 25, 25, 255 });

    renderer.drawRectangle({0, gamedata::stages::levelOfGround}, {m_size.x, 2}, {0, 0, 200, 255}, m_camera);
    renderer.drawRectangle({m_size.x / 2.0f, 0.0f}, {2.0f, m_size.y}, {0, 0, 200, 255}, m_camera);

    renderer.drawRectangle({m_size.x / 2.0f - gamedata::global::cameraWidth * gamedata::stages::minCameraScale / 2.0f, gamedata::stages::stageHeight - gamedata::global::cameraHeight * gamedata::stages::minCameraScale - 1},
    {gamedata::global::cameraWidth * gamedata::stages::minCameraScale, gamedata::global::cameraHeight * gamedata::stages::minCameraScale}, {255, 255, 0, 255}, m_camera);
    
    renderer.drawRectangle({m_size.x / 2.0f - gamedata::global::cameraWidth * gamedata::stages::maxCameraScale / 2.0f, gamedata::stages::stageHeight - gamedata::global::cameraHeight * gamedata::stages::maxCameraScale - 1},
    {gamedata::global::cameraWidth * gamedata::stages::maxCameraScale, gamedata::global::cameraHeight * gamedata::stages::maxCameraScale}, {255, 255, 0, 255}, m_camera);

    //renderer.renderTexture((*m_anim)[currentFrame], m_size.x / 2.0f - m_anim->m_origin.x, gamedata::stages::levelOfGround - m_anim->m_origin.y, m_anim->m_width, m_anim->m_height, m_camera, 0, SDL_FLIP_NONE);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    renderer.updateScreen();
}
