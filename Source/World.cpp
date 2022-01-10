#include "World.h"
#include "EngineInterface.h"
#include "ActorFactoryImpl.h"
#include "ActorPoolImpl.h"
#include "ActorImpl.h"
#include "Ship.h"
#include "EventSystem.h"
#include "Callbacks.h"
#include "TransformComponent.h"

World* World::sWorld = nullptr;

void ShootCallback(const IEventArgs& shootEvt);
void CollisionCallback(const IEventArgs& collisionEvt);

World* World::GetInstance()
{
    if (sWorld == nullptr)
    {
        sWorld = new World();
    }
    return sWorld;
}

World::~World()
{

}

void World::Initialize()
{
    mPlayerLives = 3;
    mPlayerScore = 0;
    mPlayedTime = 0.0f;

    mFrame = 0;
    mIsReplaying = false;
    SetupShip();
    SetupBulletPool();
    SetupAsteroidManager();
    SetupCallbacks();
}

void World::Update(float deltaTime)
{
    ExecuteCommands();
    if (mGameState == GameState::Replaying)
    {
        ++mFrame;
        if (!mHistory.empty())
        {
            while (mLastCommandReplayed < mHistory.size() && mHistory[mLastCommandReplayed]->frame == mFrame)
            {
                mHistory[mLastCommandReplayed]->command->Execute();

                ++mLastCommandReplayed;
                if (mLastCommandReplayed >= mHistory.size())
                {
                    mGameState = GameState::GameOver;
                }
            }
        }
        else
        {
            mGameState = GameState::GameOver;
        }
    }

    if (mGameState != GameState::GameOver)
    {
        if (mGameState != GameState::Replaying)
        {
            ++mFrame;
            mPlayedTime += deltaTime;

            mAsteroidMgr.Update(deltaTime);

            for (ActorHandle* actorHandle : mActors)
            {
                if (actorHandle->IsValid())
                {
                    auto actor = actorHandle->GetActor();
                    if (actor->IsActive())
                    {
                        actor->Update(deltaTime);
                        if (actor->GetID() == mShipID)
                        {
                            mPlayerPosition = actor->GetComponent<TransformComponent>(ComponentType::Transform)->GetPosition();
                        }

                    }
                }
            }
        }

        for (ActorHandle* actorHandle : mActors)
        {

            if (actorHandle->IsValid())
            {
                auto actor = actorHandle->GetActor();
                auto* collider = actor->GetComponent<ColliderComponent>(ComponentType::Collider);
                if (actor->IsActive() && collider != nullptr)
                {
                    collider->Update(deltaTime);
                }
            }
        }

        for (ActorHandle* actorHandle : mActors)
        {
            if (actorHandle->IsValid())
            {
                auto actor = actorHandle->GetActor();
                if (actor->IsActive())
                {
                    actor->Render();
                    if (actor->GetID() == mShipID)
                    {
                        auto ship = static_cast<Ship*>(actor);
                        ship->CheckHurtRoutine(deltaTime);
                    }                    
                }
            }
        }
    }
    
}

void World::PostUpdate()
{
    if (mGameState != GameState::GameOver)
    {
        for (ActorHandle* actorHandle : mActors)
        {
            if (actorHandle->IsValid())
            {
                auto actor = actorHandle->GetActor();
                auto* collider = actor->GetComponent<ColliderComponent>(ComponentType::Collider);
                if (actor->IsActive() && collider != nullptr)
                {
                    collider->PostUpdate();
                }
            }
        }
    }
}

void World::HurtPlayer(int damage)
{
    mPlayerLives -= damage;

    if (mPlayerLives <= 0)
    {
        mGameState = GameState::GameOver;
    }
    else
    {
        if (mActors[mShipID]->IsValid())
        {        
            Ship* ship = static_cast<Ship*>(mActors[mShipID]->GetActor());
            ship->PlayHurt();
        }
    }
}


void World::ExecuteCommands()
{
    for (auto& command : mCommands)
    {
        command->Execute();

        ReplaySlice* replayEntry = new ReplaySlice();
        replayEntry->frame = mFrame;
        replayEntry->command = command;
        mHistory.push_back(replayEntry);
    }

    mCommands.clear();
}

void World::StartReplay()
{
    for(ActorHandle* actorHandle : mActors)
    {
        if (actorHandle->IsValid())
        {
            auto actor = actorHandle->GetActor();
            actor->SetActive(false);
        }
    }
    mGameState = GameState::Replaying;
    auto ship = static_cast<Ship*>(GetActor(mShipID)->GetActor());
    ship->Initialize();    
    
    
    mPlayerLives = 3;
    mPlayerScore = 0;
    mPlayedTime = 0.0f;
    mFrame = 0;
    mIsReplaying = false;   
    
    mLastCommandReplayed = 0;
}
void World::AddActor(Actor* actor)
{
    mActors.push_back(new ActorHandle(actor));
}

void World::SetupShip()
{
    auto ship = ACTORFAC->CreateActor<Ship>();
    mShipID = ship->GetID();
    ship->Initialize();
}

void World::SetupBulletPool()
{
    mBulletPoolSize = 10;
    mBulletPool = new ActorPool<Bullet>();
    mBulletPool->InitializePool(mBulletPoolSize);
}

void World::SetupAsteroidManager()
{
    // lambda 
    auto generator = [this]() -> exVector2
    {
        exVector2 target = exVector2((rand() % 150) - 75, 0);
        if (mActors[mShipID]->IsValid())
        {
            auto shipTransfom = mActors[mShipID]->GetActor()->GetComponent<TransformComponent>(ComponentType::Transform);

            target.x += shipTransfom->GetPosition().x;
            target.y += shipTransfom->GetPosition().y;
        }
        return target;
    };

    mAsteroidMgr.Initialize(20, 2.0f, generator);
}

void World::SetupCallbacks()
{
    EVTSYS->StartListening(EventTypes::Shoot, &ShootCallback);
    EVTSYS->StartListening(EventTypes::Collision, &CollisionCallback);
}