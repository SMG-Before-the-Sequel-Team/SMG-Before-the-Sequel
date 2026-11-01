#pragma once

#include "syati.h"
#include "Game/LiveActor/LiveActor.h"		//needed?

namespace pt {


	class MarblePlanetElectron;
	class MarblePlanetElectronShadow;

	class MarblePlanet : public LiveActor {
	public:
		MarblePlanet(const char *pName);

		virtual void init(const JMapInfoIter &rIter);
		virtual void kill();
		virtual void startClipped();
		virtual void endClipped();
		virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);

		void initCoreAndElectron();
		void exeScaleUpCore();
		void exeBreakCore();

		ModelObj* mMarblePlanetCore;
		MarblePlanetElectron** mMarblePlanetElectrons;
		CollisionParts* mWaterMelonCollision;
		s32 mNumElectrons;
		s32 mRemainingElectrons;
	};

	class MarblePlanetElectron : public LiveActor {
	public:
		MarblePlanetElectron(LiveActor *pHost, const TVec3f &rTranslation, const TVec3f &rRotation, const char *pName);

		virtual void init(const JMapInfoIter &rIter);
		virtual void control();
		virtual void attackSensor(HitSensor *pSender, HitSensor *pReceiver);
		virtual bool receiveMsgPush(HitSensor *pSender, HitSensor *pReceiver);
		virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);

		void exeMove();
		void exeAttack();
		void crashElectron(HitSensor *pCollidedSensor);

		LiveActor* mHost;
		MarblePlanetElectronShadow* mShadow;
		TVec3f mFrontVec;
	};

	class MarblePlanetElectronShadow : public LiveActor {
	public:
		MarblePlanetElectronShadow(LiveActor *pHost, const TVec3f &rMarblePlanetPos, const char *pName);

		virtual void init(const JMapInfoIter &rIter);
		virtual void calcAndSetBaseMtx();

		LiveActor* mHost;
		const TVec3f* mMarblePlanetPos;
	};

	namespace NrvMarblePlanet {
		NERVE(NrvWait);
		NERVE(NrvScaleUpCore);
		NERVE(NrvBreakCore);
	};

	namespace NrvMarblePlanetElectron {
		NERVE(NrvMove);
		NERVE(NrvAttack);
	};
}