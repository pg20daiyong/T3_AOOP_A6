#include "RespawnCommand.h"
#include <TransformComponent.h>

RespawnCommand::RespawnCommand(bool active, exVector2 spawnPosition, Actor* actor)
    : mActive(active)
    , mSpawnPosition(spawnPosition)
    , mActor(actor)
{}

void RespawnCommand::Execute()
{
    mActor->GetComponent<TransformComponent>(ComponentType::Transform)->SetPosition(mSpawnPosition);
    mActor->SetActive(mActive);
}