#pragma once

#include "Constants.h"
#include "EngineTypes.h"
#include "ActorFactory.h"
#include "ActorPool.h"
#include "Actor.h"
#include "ActorHandle.h"
#include "Bullet.h"
#include "AsteroidManager.h"
#include "Command.h"
#include "ReplaySlice.h"

#include <vector>
class Command;
struct ReplaySlice;

#define VERBOSE 1

#define WORLD World::GetInstance()

typedef std::vector<ActorHandle*> ActorList;

class World
{
    FACTORIZE

public:
    static World* GetInstance();
    ~World();

    void Initialize();
    void Update(float deltaTime);
    void PostUpdate();

    float GetTotalPlayedTime() const { return mPlayedTime;  }

    int GetPLayerLives() const { return mPlayerLives; }
    exVector2 GetPlayerPosition() const { return mPlayerPosition; }
    void HurtPlayer(int damage);

    int GetPlayerScore() const { return mPlayerScore; }
    void AddPlayerScore(int score) { mPlayerScore += score; }

    exVector2 GetWorldSize() const { return exVector2{ kViewportWidth, kViewportHeight }; }

    ActorHandle* GetActor(int id) { return mActors[id]; }
    ActorList GetActors() const { return mActors; }

    ActorPool<Bullet>* GetBulletPool() const { return mBulletPool; }

    GameState GetGameState() const { return mGameState;  }
    void        ExecuteCommands();
    void        StartReplay();
    std::vector<Command*> mCommands;
    std::vector<ReplaySlice*> mHistory;

    int mFrame;
    int mLastCommandReplayed;
private:
    static World* sWorld;

    World() = default;
    void AddActor(Actor* actor);

    void SetupShip();
    void SetupBulletPool();
    void SetupAsteroidManager();
    void SetupCallbacks();

    GameState mGameState = GameState::Playing;

    ActorList mActors;

    int mBulletPoolSize = 0;



    ActorPool<Bullet>* mBulletPool;
    AsteroidManager mAsteroidMgr;

    exVector2 mPlayerPosition;
    int mShipID = -1;

    float mPlayedTime;

    int mPlayerLives;
    int mPlayerScore;

    bool mIsReplaying;
};