#include "MoveCommand.h"

MoveCommand::MoveCommand(float x, float y, TransformComponent* transform)
    : mXOffset(x)
    , mYOffset(y)
    , mTransform(transform)
{}

void MoveCommand::Execute()
{
    exVector2 pos = mTransform->GetPosition();
    mTransform->SetPosition(exVector2(pos.x + mXOffset, pos.y + mYOffset));
}

