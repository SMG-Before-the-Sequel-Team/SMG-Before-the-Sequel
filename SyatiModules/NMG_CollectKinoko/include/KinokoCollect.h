#pragma once

#include "Game/LiveActor/LiveActor.h"
#include "Game/MapObj/MapObjConnector.h"

#define KINOKO_COLLECT_APPEAR_FIX 0
#define KINOKO_COLLECT_APPEAR_THROW 1

#define KINOKO_COLLECT_POSE_ROTATE 1
#define KINOKO_COLLECT_POSE_GRAVITY 2
#define KINOKO_COLLECT_POSE_CONNECTOR 4

namespace neo {
    class KinokoCollect : public LiveActor {
    public:
        KinokoCollect(const char *pName);

        virtual void init(const JMapInfoIter &rIter);
        virtual void initAfterPlacement();
        virtual void appear();
        virtual void calcAndSetBaseMtx();
        virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
        virtual bool receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver);

        void doGetItem();
        void exeAppearFix();
        void exeAppearThrow();
        void exeWait();
        void exeThrow();

        bool isCollectable() const;

        s32 mRewardKind;
        MapObjConnector* mConnector;
        s32 mAppearType;
        s32 mPoseType;
        s32 mShadowType;
        f32 mRotateY;
        f32 mThrowSpeed;
        f32 mThrowAngle;
        TVec3f mFrontVec;
        TVec3f mThrowSrc;
    };

    namespace NrvKinokoCollect {
        NERVE(NrvAppearFix);
        ENDABLE_NERVE(NrvAppearThrow);
        NERVE(NrvWait);
        NERVE(NrvBound);
        NERVE(NrvThrow);
    };
};

#define NrvKinokoCollectAppearFix &neo::NrvKinokoCollect::NrvAppearFix::sInstance
#define NrvKinokoCollectAppearThrow &neo::NrvKinokoCollect::NrvAppearThrow::sInstance
#define NrvKinokoCollectWait &neo::NrvKinokoCollect::NrvWait::sInstance
#define NrvKinokoCollectBound &neo::NrvKinokoCollect::NrvBound::sInstance
#define NrvKinokoCollectThrow &neo::NrvKinokoCollect::NrvThrow::sInstance
