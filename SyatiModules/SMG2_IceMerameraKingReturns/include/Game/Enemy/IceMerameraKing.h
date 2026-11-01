#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Game/LiveActor/ModelObj.h"
#include "Game/Util/Array.h"

class CameraTargetDemoActor;
template < typename T >
class JointControlDelegator;
class JointControllerInfo;
class Meramera;
class SpinPullParticleCallBack;
class ThrowingIce;
class FixedPosition;

class IceMerameraKing : public LiveActor {
public:
    IceMerameraKing(const char*);

    virtual void init(const JMapInfoIter&);
    virtual void initAfterPlacement();
    virtual void kill();
    virtual void control();
    virtual void calcAndSetBaseMtx();
    virtual void attackSensor(HitSensor*, HitSensor*);
    virtual bool receiveMsgPlayerAttack(u32, HitSensor*, HitSensor*);
    virtual bool receiveOtherMsg(u32, HitSensor*, HitSensor*);

    void exeSearch();
    void exeThrow();
    void exeOnEndThrow();
    void tearDownThrow();
    void exeExtinguish();
    void exeEscape();
    void exeOnEndEscape();
    void exeEscapeJump();
    void exeDamage();
    void exePreRecover();
    void exeRecover();
    void exePreAttack();
    void exeAttack();
    void exeAttackAfter();
    void exeAngryDemo();
    void exeDeathDemoWait();
    void exeDeathDemo();
    void addVelocityToInitPos();
    bool isDeadAllIce();
    bool isEnableThrow();
    ThrowingIce* getDeadWeaponAndAppear();
    bool calcJoint(TPos3f*, const JointControllerInfo&);

    FixedPosition* mFixedPos;  //_8C
    ThrowingIce* mThrowingIce; //_90
    Meramera* mMeramera; // _94
    SpinPullParticleCallBack* mSpinParticle;  //_98
    MR::Vector< MR::AssignableArray< ThrowingIce* > > mIce;  //_9C
    LiveActor* mShockWave;
    LiveActor* mBreakModel;
    TVec3f mFrontVec;
    TVec3f mUpVec;
    TVec3f mInitPosition;
    TVec3f mInitRotation;
    s32 mThrownInARow;
    u32 mTimeInEscape;
    s32 mHealth; // ObjArg2
    s32 mHealthChild; // ObjArg3 : Determines at which health value the children start spawning
    s32 mEscapeTime; // ObjArg4 : The max time allowed for escaping the player
    MR::Vector< MR::AssignableArray< Meramera* > > mChildArray;
    CameraTargetDemoActor* mCameraTarget;                                 //_F8
    JointControlDelegator< IceMerameraKing >* mJointController; //_FC
    TQuat4f mRollRotation;
    TVec3f mBinderOffsetVec;
    f32 _11C;
    s32 mStarId; // ObjArg0
    s32 mAttackMsg; // ObjArg1
    f32 mOriginalBinderRadius;
    f32 mOriginalBodyRadius;
    bool _120;
    bool _121;
};

class IceMerameraKingShockWave : public ModelObj {
public:
    IceMerameraKingShockWave(const char*);

    virtual void appear();
    virtual void control();
    virtual void attackSensor(HitSensor*, HitSensor*);
};



namespace MR {
    void initMarioResourceIce(const JMapInfoIter& rIter);
}