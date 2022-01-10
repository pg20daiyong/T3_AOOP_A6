#pragma once
#include "Command.h"
#include "Actor.h"

class RespawnCommand :public Command
{
public:
    RespawnCommand(bool active, exVector2 spawnPosition, Actor* actor);

    void Execute() override;

private:

    bool mActive;
    exVector2 mSpawnPosition;
    Actor* mActor;
};
#pragma once
