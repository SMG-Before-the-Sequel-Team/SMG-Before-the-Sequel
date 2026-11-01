#pragma once

#include "Game/Boss/SkeletalFishBossInfo.h"
#include "Game/Camera/CameraTargetArg.h"
#include "Game/LiveActor/ActorCameraInfo.h"
#include "Game/LiveActor/LiveActor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/LiveActor/PartsModel.h"
#include "Game/Util/JointController.h"

class SkeletalFishGuard;
class SkeletalFishGuardHolder;
class SkeletalFishBossScarFlash;
class SkeletalFishBossHead;
class SkeletalFishBossInfo;
class SkeletalFishRailControl;
class SkeletalFishBossBattleDirector;
class SkeletalFishBossRail;

class SkeletalFishBoss : public LiveActor {
public:

    struct SensorToCollider {
        const char* mJointName;
        const char* mColliderName;
    };

    struct JointToShadow {
        const char* mName;
        const char* mJointName;
        const char* mShadowName;
    };

    struct HitPos {
        const char* mName;
        const char* mSensorName;
        Vec mOffset;
    };

    typedef void (SkeletalFishBoss::* SceneFunc)(void);

    SkeletalFishBoss(const char*);

    virtual ~SkeletalFishBoss();
    virtual void init(const JMapInfoIter&);
    virtual void initAfterPlacement();
    virtual void calcAnim();
    virtual void appear();
    virtual void makeActorAppeared();
    virtual void kill();
    virtual void control();
    virtual void calcAndSetBaseMtx();

    bool calcJoint(TPos3f*, const JointControllerInfo&);
    void damage(const HitSensor*, const TVec3f&);
    void exeSwim();
    void exeOpen();
    void exeOpenWait();
    void exeClose();
    void exeBite();
    void exeDamage();
    void exeDown();
    void exeDeadDamage();
    void exeDead();
    void exeAppearWait();
    void exeAppearDemo();
    void exePowerUpDemo();
    void exeDeadDemo();
    void exeBreakDemo();
    void exeDemoWait();
    void notifyAttack(SkeletalFishGuard*);
    LiveActor* getCurrentRail();
    f32 getBodyThickness() const;
    SkeletalFishBossHead* getHeadActor() const;
    void initLevelStatus();
    void initJoint();
    void initCollision();
    void initHead();
    void initScarFlash();
    void initBreakModel();
    void initSwitch(const JMapInfoIter&);
    void createGuards();
    void initShadow();
    void initCamera(const JMapInfoIter& rIter);
    void validateCollision();
    void invalidateCollision();
    void getMouthSensorCenterPos(TVec3f&, f32) const;
    void updateCollision();
    void powerUp();
    void startDamageAnim();
    void calcPlanetCenterPosition();
    bool calcGravityCenter(TVec3f*, const TVec3f&, const TVec3f&);
    void resetRail();
    void updateBgm();
    LiveActor* getCurrentBossRail();
    void stopScene(const char*, const Nerve*, SceneFunc);
    void startCamera(const char*);
    void resetCamera();
    void playDamageBrk();
    void startAppearDemo();
    void endAppearDemo();
    void startPowerUpDemo();
    void endPowerUpDemo();
    void startDeadDemo();
    void endBreakDemo();
    bool isEnableToBeDamaged() const;

    SkeletalFishBossInfo::LevelStatus* getLevelStatus() const {
        return mBossInfo->getLevelStatus(_110);
    }

    JointController* mControllers[0xD];
    s32* mJointIndicies;                            // _C0
    CollisionParts** mPartsArray;                   // _C4
    SkeletalFishBossHead* mBossHead;                // _C8
    SkeletalFishBossScarFlash* mScarFlash;              // _CC
    ModelObj* mBreakModel;                          // _D0
    const HitSensor* _D4;
    TPos3f _D8;
    SkeletalFishRailControl* mRailControl;          // _108
    SkeletalFishBossBattleDirector* mBossDirector;  // _10C
    s32 _110;
    s32 _114;
    u32 _118;
    CameraTargetMtx* mCameraTargetMtx;              // _11C
    TPos3f _120;
    TPos3f _150;
    TVec3f _180;
    SceneFunc mSceneFunc;                           // _18C
    const char* mCurScene;                          // _198
    const Nerve* mSceneNerve;                             // _19C
    s32 _1A0;
    s32 _1A4;
    SkeletalFishBossInfo* mBossInfo;                // _1A8
    SkeletalFishGuardHolder* mGuardHolder;          // _1AC
    u32 _1B0;
    s32 _1B4;

    ActorCameraInfo* mActorCameraInfo; // NEW TO SMG2
};

class SkeletalFishBossScarFlash : public PartsModel {
public:
    SkeletalFishBossScarFlash(LiveActor*);

    virtual ~SkeletalFishBossScarFlash();
    virtual void init(const JMapInfoIter&);
    virtual void appear();
    virtual void control();
};

class SkeletalFishBossHead : public PartsModel {
public:
    SkeletalFishBossHead(LiveActor*);

    virtual ~SkeletalFishBossHead();
    virtual void movement();
    virtual void calcAnim();
    virtual void attackSensor(HitSensor*, HitSensor*);
    virtual bool receiveMsgPlayerAttack(u32, HitSensor*, HitSensor*);

    void updateCollisionMtx();
    void createSubModel();

    TMtx34f _9C;
    CollisionParts* _CC;
    TMtx34f _D0;
    CollisionParts* _100;
    ModelObj* mLightModels[2];              // _104
    ModelObj* mBloomModels[2];              // _10C
    u8 _114;
};

namespace {
    NERVE(SkeletalFishBossNrvSwim);
    NERVE(SkeletalFishBossNrvOpen);
    NERVE(SkeletalFishBossNrvOpenWait);
    NERVE(SkeletalFishBossNrvClose);
    NERVE(SkeletalFishBossNrvBite);
    NERVE(SkeletalFishBossNrvDamage);
    NERVE(SkeletalFishBossNrvDown);
    NERVE(SkeletalFishBossNrvDeadDamage);
    NERVE(SkeletalFishBossNrvDead);
    NERVE(SkeletalFishBossNrvAppearWait);
    NERVE(SkeletalFishBossNrvAppearDemo);
    NERVE(SkeletalFishBossNrvPowerUpDemo);
    NERVE(SkeletalFishBossNrvDeadDemo);
    NERVE(SkeletalFishBossNrvBreakDemo);
    NERVE(SkeletalFishBossNrvDemoWait);
};