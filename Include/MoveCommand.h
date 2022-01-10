#pragma once
#include "Command.h"
#include "ComponentBase.h"
#include "TransformComponent.h"


class MoveCommand : public Command
{
public:
    MoveCommand(float x, float y, TransformComponent* transform);


	void Execute() override;

private:

    float mXOffset;
    float mYOffset;
    TransformComponent* mTransform;
};