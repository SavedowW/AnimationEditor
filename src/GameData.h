#ifndef GAMEDATA_H_
#define GAMEDATA_H_
#include <string_view>
#include <SDL.h>
#include "Vector2.h"
#include <array>

namespace gamedata
{
    namespace stages
    {
        inline constexpr int numberOfStages = 1;
        inline constexpr float stageWidth = 2500.0f;
        inline constexpr float stageHeight = 2500.0f;
        inline constexpr float levelOfGround = 2460.0f;
        inline constexpr int framesBeforeZoom = 60;
        inline constexpr float maxCharRange = 1500.0f;
        inline constexpr Vector2<float> startingCameraPos = {stageWidth / 2.0f, stageHeight - 450};
        inline constexpr float hitstunCameraPriorityReduction = 0.2f;
        inline constexpr float startingCameraScale = 1.0f;
        inline constexpr float charSpawnRange = 300.0f;
        inline constexpr std::array<Vector2<float>, 2> spawnPoints = {{
            Vector2{stageWidth / 2.0f - charSpawnRange, levelOfGround},
            Vector2{stageWidth / 2.0f + charSpawnRange, levelOfGround}
        }};
        inline constexpr float minCameraScale = 0.1f;
        inline constexpr float maxCameraScale = 1.5f;
        inline constexpr float heightMovePriority = 0.6f;
        inline constexpr float reservedScaleRange = 200.0f;
        inline constexpr float verticalCameraOffset = 270.0f;
        inline constexpr float maxPushRange = 35.0f;
        inline constexpr float maxPushRangeTreshold = 10.0f;


        namespace stage1
        {
            inline constexpr int stageID = 1;
        }
    }

    namespace global
    {
        inline constexpr float framerate = 60.0f;
        inline constexpr int numberOfLevels = 2;
        inline constexpr int initialLevelId = 1;
        inline constexpr float cameraWidth = 1600.0f;
        inline constexpr float cameraHeight = 900.0f;
        inline constexpr float maxCameraScale = std::min(gamedata::stages::stageWidth / cameraWidth, gamedata::stages::stageHeight / cameraHeight);
        inline constexpr int inputBufferLength = 4;
    }

    namespace characters
    {
        inline constexpr int numberOfCharacters = 1;
        inline constexpr SDL_Color pushboxColor = {255, 242, 0, 100};
        inline constexpr SDL_Color hitboxColor = {255, 0, 0, 100};
        inline constexpr SDL_Color hurtboxColor = {0, 255, 0, 100};
        inline constexpr int instantBlockWindow = 7;
        inline constexpr int instantBlockAttemptDelay = 10;
        inline constexpr float initialBlockstunScaling = 2.0f;
        inline constexpr float finalBlockstunScaling = 1.0f;
        inline constexpr float blockstunScalingDiff = initialBlockstunScaling - finalBlockstunScaling;
        inline constexpr float inertiaIntoCornerPushbackCarry = 0.1f;
        inline constexpr float airborneImpulseIntoPushbackCarry = 0.1f;
        inline constexpr float airbornePushbackMultiplier = 10.0f;
        inline constexpr float pushblockInertiaCarry = 0.2f;
        inline constexpr float takenHitInertiaCarry = 0.3f;
        inline constexpr int totalStateCount = 100;


        namespace char1
        {
            inline constexpr std::string_view charname = "Idk";
            inline constexpr Collider standingHurtbox = {-70, -375, 140, 375};
            inline constexpr Collider crouchingHurtbox = {-70, -200, 140, 200};
            inline constexpr Collider airHitstunHurtbox = {-350/2, -160, 350, 120};
            inline constexpr Collider floatingHurtbox = {-70, -350, 140, 300};
            inline constexpr float gravity = 1.5f;
            inline constexpr float inertiaDrag = 1.0f;
            inline constexpr float maxCornerPushback = 20.0f;
            inline constexpr Collider standingPushbox = {-50.0f, -300.0f, 100.0f, 300.0f};
            inline constexpr Collider crouchingPushbox = {-50.0f, -200.0f, 100.0f, 200.0f};
            inline constexpr Collider airbornePushbox = {-50.0f, -300.0f, 100.0f, 200.0f};
            inline constexpr Collider airborneHitstunPushbox = {-75.0f, -150.0f, 150.0f, 100.0f};
            inline constexpr float forwardWalkSpd = 6.0f;
            inline constexpr float backwardWalkSpd = 6.0f;
            inline constexpr float groundJumpVerticalImpulse = 30.0f;
            inline constexpr float groundJumpHorizontalImpulse = 10.0f;
            inline constexpr float groundJumpMaxHorizontalInertia = 10.0f;
            inline constexpr float airJumpVerticalImpulse = 30.0f;
            inline constexpr float airJumpHorizontalImpulse = 10.0f;
            inline constexpr int prejumpFrames = 4;
            inline constexpr float dashAccel = 3.0f;
            inline constexpr float dashMaxSpeed = 20.0f;
            inline constexpr float dashRecovery = 8;
            inline constexpr float softLandingRecovery = 6;
            inline constexpr float hardLandingRecovery = 10;
            inline constexpr float vulnerableLandingRecovery = 5;
            inline constexpr float jcLandingRecovery = 10;
            inline constexpr int backdashDuration = 25;
            inline constexpr int backdashInvulDuration = 5;
            inline constexpr float backdashSpd = 13.0f;
            inline constexpr float airdashDuration = 10.0f;
            inline constexpr float airdashExtentionDuration = 15.0f;
            inline constexpr float airdashSpeed = 25.0f;
            inline constexpr float airdashExtentionMaxSpeed = 25.0f;
            inline constexpr float airdashExtentionMinSpeed = 15.0f;
            inline constexpr float doubleJumpInertiaDivider = 3.0f;
            inline constexpr float airBackdashDuration = 10.0f;
            inline constexpr float airBackdashSpeed = 25.0f;
            inline constexpr int stepDuration = 15;
            inline constexpr float stepSpeed = 18.0f;
            inline constexpr int stepRecoveryDuration = 4;
        }
    }
}

#endif