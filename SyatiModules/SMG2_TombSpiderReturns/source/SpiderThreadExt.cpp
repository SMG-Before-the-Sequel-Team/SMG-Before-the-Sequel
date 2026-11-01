#include "SpiderThreadExt.h"

#include "Game/Scene/SceneObjHolder.h"
#include "Game/MapObj/SpiderThread.h"
#include "Game/MapObj/SpiderThreadHangInfo.h"
#include "Game/MapObj/SpiderThreadMainPoint.h"
#include "Game/MapObj/SpiderThreadPart.h"
#include "Game/MapObj/SpiderThreadPoint.h"
#include "Game/MapObj/SpiderThreadRadialLine.h"
#include "Game/MapObj/SpiderThreadWindCtrl.h"

#include "Game/NameObj/NameObjCategories.h"
#include "Game/Util.h"

#include "Util.h"

// TODO: Split into several files?

class SpiderThreadSMG1 : public SpiderThread {
public:

    void initThread(const TVec3f&);
    void startActorBind(LiveActor*, const TVec3f**, const TVec3f**, const TVec3f**, const TVec3f&, const TVec3f&, s32);
    bool touchActor(const TVec3f&, const TVec3f&);
    void tryPush(const TVec3f&, f32);
    void stopAllPartsPoint();
    void cutSpiderThread();
    void initMainThreads();
    void connectMainPoint(SpiderThreadPoint*, SpiderThreadPoint*, bool);
    void findNearestPointPos(const TVec3f**, const TVec3f**, s32*, s32*, const TVec3f&) const;

    inline SpiderThreadRadialLine* getMainRadialLine(int idx) const {
        return mRadialLines[idx];
    }
    inline SpiderThreadPart* getRadialPart(int idx) const {
        return mRadialParts[idx];
    }
    inline SpiderThreadPart* getCirclePart(int idx) const {
        return mCircleParts[idx];
    }
    inline SpiderThreadHangInfo* getActorHangInfo(int idx) const {
        return mHangInfos[idx];
    }
};
class SpiderThreadHangInfoSMG1 : public SpiderThreadHangInfo {
public:
    SpiderThreadHangInfoSMG1();

    void startBind(LiveActor*, const TVec3f*, SpiderThreadPart*, s32, const TVec3f&, s32);
};
class SpiderThreadPointNearInfoSMG1 : public SpiderThreadPointNearInfo {
public:
    SpiderThreadPointNearInfoSMG1();
};
class SpiderThreadMainPointSMG1 : public SpiderThreadMainPoint {
public:
    SpiderThreadMainPointSMG1(const TVec3f&, int);

    void addNearMainPoint(SpiderThreadMainPoint*, SpiderThreadPart*);
    void cutNearPoints(s32*, SpiderThreadMainPoint**);
};
class SpiderThreadPartSMG1 : public SpiderThreadPart {
public:
    SpiderThreadPartSMG1(SpiderThreadMainPoint*, SpiderThreadMainPoint*, f32);

    void cut(SpiderThreadMainPoint*, SpiderThreadMainPoint*);
    void stopAllPoints();
    f32 calcNearestPointInfo(const TVec3f**, const TVec3f**, s32*, const TVec3f&);
    bool touchActor(s32, const TVec3f&);
    f32 findNearestPointNo(s32*, const TVec3f&) const;

    inline void padBox(f32 padAmount) {
        TVec3f padding(padAmount);
        mBoundBox.mPointMin.sub(padding);
        mBoundBox.mPointMax.add(padding);
    }

    inline SpiderThreadPoint* getPoint(int idx) const {
        return mPoints[idx];
    }
};
class SpiderThreadPointSMG1 : public SpiderThreadPoint {
public:
    SpiderThreadPointSMG1(const TVec3f&, f32);

    bool tryPush(const TVec3f&, f32);


    inline void setPos(const TVec3f& rPos) {
        mPosition.set(rPos);
        mVelocity.zero();
    }

    inline f32 getPosZDiff(const TVec3f& rPos) const {
        return rPos.z - mPosition.z;
    }

    // TODO: this should be removed when the appropriate inline in JAudio is made/found
    inline s32 systemLevelParam(f32 a, f32 b) {
        return static_cast<s16>(a) << 16 | static_cast<s16>(b) & 0xFFFF;
    }
};
class SpiderThreadRadialLineSMG1 : public SpiderThreadRadialLine {
public:
    SpiderThreadRadialLineSMG1(int);

    void addPoint(SpiderThreadMainPoint*);
};
class SpiderThreadWindCtrlSMG1 : public SpiderThreadWindCtrl {
public:
    SpiderThreadWindCtrlSMG1();

    void startWindBattle();
    void startWindChance();
    void startWindBattleEnd();
};

// ==============================================================================================

#pragma region SpiderThread
namespace {
    const CutPointEntry sCutPointNoTable[] = { {0, 3}, {1, 1}, {2, 0}, {4, 3}, {6, 1}, {6, 4} };

    static const s32 sMainRadialLinePointNum = 8;
    static const f32 sMainRadialLineInterval = 175.0f;
    static const f32 sMainRadialLineLength = 1800.0f;
    static const f32 sMainPointRadius = 100.0f;
    static const f32 sPartPointRadius = 100.0f;
    static const f32 sTouchPointerSpeedMin = 5.0f;
    static const f32 sTouchPointerSpeedAccelRate = 2.0f;
    static const f32 sTouchSpeedMax = 50.0f;
    static const s32 sHangActorNumMax = 8 * sMainRadialLinePointNum;
    static const f32 sIndirectScaleX = 0.035f;
    static const f32 sIndirectScaleY = 0.03f;

    // Horrifying
    SpiderThreadWindCtrlSMG1* getWindCtrl() {
        return (SpiderThreadWindCtrlSMG1*)(((SpiderThread*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_SPIDER_THREAD))->mWindCtrl);
    }
}

inline SpiderThreadPart* connectPoints(SpiderThreadPoint* pPointA, SpiderThreadPoint* pPointB) {
    SpiderThreadMainPointSMG1* pointB = reinterpret_cast<SpiderThreadMainPointSMG1*>(pPointB);
    SpiderThreadMainPointSMG1* pointA = reinterpret_cast<SpiderThreadMainPointSMG1*>(pPointA);

    SpiderThreadPart* part = new SpiderThreadPartSMG1(pointA, pointB, sMainPointRadius);

    pointA->addNearMainPoint(pointB, part);
    pointB->addNearMainPoint(pointA, part);

    return part;
}


void SpiderThreadSMG1::initThread(const TVec3f& rPos) {
    mTranslation.set(rPos);

    MR::connectToScene(this, MR::MovementType_Ride, MR::CategoryList_Null, MR::CategoryList_Null, MR::DrawType_SpiderThread);

    mWindCtrl = new SpiderThreadWindCtrlSMG1();
    initMainThreads();
    MR::invalidateClipping(this);

    mThreadTexture = new JUTTexture(MR::loadTexFromArc("SpiderThread.arc", "SpiderThread.bti", nullptr), 0);
    mIndirectTexture = new JUTTexture(MR::loadTexFromArc("SpiderThread.arc", "Indirect.bti", nullptr), 0);

    mHangInfos = new SpiderThreadHangInfo * [sHangActorNumMax];
    for (s32 idx = 0; idx < sHangActorNumMax; idx++) {
        mHangInfos[idx] = new SpiderThreadHangInfoSMG1();
    }

    MR::registerDemoSimpleCastAll(this);
    makeActorDead();
}

void SpiderThreadSMG1::startActorBind(LiveActor* pActor, const TVec3f** pPointPos, const TVec3f** pNeutralPos, const TVec3f** pUp, const TVec3f& rPos, const TVec3f& rVel, s32 a1) {
    s32 nearestIndex = 0;
    s32 pointNum = 0;
    findNearestPointPos(pPointPos, pNeutralPos, &nearestIndex, &pointNum, rPos);

    OSReport("NearestPoint: %d, %d, (%ff, %ff%ff), (%ff, %ff, %ff)\n", nearestIndex, pointNum, (*pPointPos)->x, (*pPointPos)->y, (*pPointPos)->z, (*pNeutralPos)->x, (*pNeutralPos)->y, (*pNeutralPos)->z);

    SpiderThreadHangInfoSMG1* hangInfo = (SpiderThreadHangInfoSMG1*)getActorHangInfo(mNumHangInfos);
    hangInfo->startBind(pActor, &pActor->mTranslation, mCircleParts[nearestIndex], pointNum, rVel, a1);
    *pUp = &hangInfo->mThreadPart->mUp;
    mNumHangInfos++;
}

bool SpiderThreadSMG1::touchActor(const TVec3f& rPos, const TVec3f& rVel) {
    const TVec3f* pointPos = nullptr;
    const TVec3f* neutralPos = nullptr;
    s32 nearestIndex = 0;
    s32 pointNum = 0;
    findNearestPointPos(&pointPos, &neutralPos, &nearestIndex, &pointNum, rPos);

    return ((SpiderThreadPartSMG1*)mCircleParts[nearestIndex])->touchActor(pointNum, rVel);
}

void SpiderThreadSMG1::tryPush(const TVec3f& rPos, f32 radius) {
    ((SpiderThreadPointSMG1*)mMainPoint)->tryPush(rPos, radius);
    for (s32 radialIdx = 0; radialIdx < mNumRadialLines; radialIdx++) {
        for (s32 idx = 0; idx < getMainRadialLine(radialIdx)->mNumPoints; idx++) {
            ((SpiderThreadPointSMG1*)getMainRadialLine(radialIdx)->getPoint(idx))->tryPush(rPos, radius);
        }
    }
}

void SpiderThreadSMG1::stopAllPartsPoint() {
    for (s32 idx = 0; idx < mNumCircleParts; idx++) {
        ((SpiderThreadPartSMG1*)getCirclePart(idx))->stopAllPoints();
    }
}

void SpiderThreadSMG1::cutSpiderThread() {
    ((SpiderThreadMainPointSMG1*)mMainPoint)->cutNearPoints(&mNumCutPoints, mCutPoints);

    for (s32 idx = 0; idx < (sizeof(sCutPointNoTable) / sizeof(sCutPointNoTable[0])); idx++) {
        ((SpiderThreadMainPointSMG1*)getMainRadialLine(sCutPointNoTable[idx].mRadialIdx)->getPoint(sCutPointNoTable[idx].mPointIdx))->cutNearPoints(&mNumCutPoints, mCutPoints);
    }
}

void SpiderThreadSMG1::initMainThreads() {
    TRot3f rotMtx;
    MR::makeMtxRotate(rotMtx.toMtxPtr(), 0.0f, 0.0f, 360.0f / mNumRadialLines);
    TVec3f down(0.0f, -1.0f, 0.0f);

    mMainPoint = new SpiderThreadMainPointSMG1(mTranslation, mNumRadialLines + 1);

    mAnchorPoints = new SpiderThreadMainPoint * [mNumRadialLines];
    for (s32 idx = 0; idx < mNumRadialLines; idx++) {
        mAnchorPoints[idx] = nullptr;
    }

    mCutPointsBufferSize = mNumRadialLines + (sizeof(sCutPointNoTable) / sizeof(sCutPointNoTable[0])) * 4;
    mCutPoints = new SpiderThreadMainPoint * [mCutPointsBufferSize];
    for (s32 idx = 0; idx < mCutPointsBufferSize; idx++) {
        mCutPoints[idx] = new SpiderThreadMainPointSMG1(mTranslation, 5);
    }

    mRadialLines = new SpiderThreadRadialLine * [mNumRadialLines];
    for (s32 radialIdx = 0; radialIdx < mNumRadialLines; radialIdx++) {
        mRadialLines[radialIdx] = new SpiderThreadRadialLineSMG1(sMainRadialLinePointNum);

        // main axial points
        for (s32 idx = 0; idx < sMainRadialLinePointNum; idx++) {
            TVec3f pos(down);
            pos.scale(sMainRadialLineInterval * (idx + 1));
            pos.add(mMainPoint->mPosition);

            SpiderThreadMainPoint* point = new SpiderThreadMainPointSMG1(pos, 5);
            ((SpiderThreadRadialLineSMG1*)getMainRadialLine(radialIdx))->addPoint(point);
            point->mRadialLine = getMainRadialLine(radialIdx);
        }

        // anchor point
        TVec3f pos(down);
        pos.scale(sMainRadialLineLength);
        pos.add(mMainPoint->mPosition);
        mAnchorPoints[radialIdx] = new SpiderThreadMainPointSMG1(pos, 5);
        mAnchorPoints[radialIdx]->mRadialLine = getMainRadialLine(radialIdx);

        // connect points
        SpiderThreadMainPoint* point = getMainRadialLine(radialIdx)->getPoint(0);
        SpiderThreadMainPoint* nextPoint;
        connectMainPoint(point, mMainPoint, true);
        for (s32 idx = 0; idx < sMainRadialLinePointNum - 1; idx++) {
            nextPoint = getMainRadialLine(radialIdx)->getPoint(idx + 1);
            connectMainPoint(point, nextPoint, true);
            point = nextPoint;
        }
        connectMainPoint(nextPoint, mAnchorPoints[radialIdx], true);

        rotMtx.mult(down, down);
    }

    for (s32 radialIdx = 0; radialIdx < mNumRadialLines; radialIdx++) {
        for (s32 idx = 0; idx < sMainRadialLinePointNum; idx++) {
            connectMainPoint(getMainRadialLine(radialIdx)->getPoint(idx), getMainRadialLine((radialIdx + mNumRadialLines + 1) % mNumRadialLines)->getPoint(idx), false);
        }
    }
}

void SpiderThreadSMG1::connectMainPoint(SpiderThreadPoint* pPointA, SpiderThreadPoint* pPointB, bool isMainPart) {
    SpiderThreadPart* part = connectPoints(pPointA, pPointB);

    if (isMainPart) {
        mRadialParts[mNumRadialParts] = part;
        mNumRadialParts++;
    }
    else {
        mCircleParts[mNumCircleParts] = part;
        mNumCircleParts++;
    }
}

void SpiderThreadSMG1::findNearestPointPos(const TVec3f** pPointPos, const TVec3f** pNeutralPos, s32* nearestIndex, s32* pointNum, const TVec3f& rPos) const {
    TVec3f pos(rPos);
    f32 minDistance = sMainRadialLineLength;
    *pPointPos = nullptr;
    *pNeutralPos = nullptr;

    for (s32 idx = 0; idx < mNumCircleParts; idx++) {
        const TVec3f* pointPos = nullptr;
        const TVec3f* neutralPos = nullptr;
        s32 pointNo = -1;
        f32 dist = ((SpiderThreadPartSMG1*)getCirclePart(idx))->calcNearestPointInfo(&pointPos, &neutralPos, &pointNo, pos);

        if (dist < minDistance) {
            *pPointPos = pointPos;
            *pNeutralPos = neutralPos;
            *nearestIndex = idx;
            *pointNum = pointNo;
            minDistance = dist;
        }
    }
}
#pragma endregion

#pragma region SpiderThreadHangInfo
SpiderThreadHangInfoSMG1::SpiderThreadHangInfoSMG1() {
    mBindedActor = nullptr;
    mPosition = nullptr;
    mThreadPart = nullptr;
    mHangIndex = -1;
    mIsHanging = false;
    _14 = 0;
}

void SpiderThreadHangInfoSMG1::startBind(LiveActor* pActor, const TVec3f* pPos, SpiderThreadPart* pThreadPart, s32 hangIndex, const TVec3f& rVel, s32 a2) {
    mBindedActor = pActor;
    mPosition = pPos;
    mThreadPart = pThreadPart;
    mHangIndex = hangIndex;
    mIsHanging = false;
    _14 = a2;

    if (!MR::isNearZero(rVel, 0.001f)) {
        ((SpiderThreadPartSMG1*)mThreadPart)->touchActor(mHangIndex, rVel);
    }
}
#pragma endregion

#pragma region SpiderThreadMainPoint and SpiderThreadPointNearInfo
namespace {
    static const f32 sFrictionRate = 0.9f;
    // static const _32 sSpeedMin =
    static const f32 sWindFrictionRate_1 = 0.9f;
    static const f32 sBasePosAccelRate = 0.3f;
    static const f32 sBasePosDiffMin = 1.0f;
    static const f32 sNearMainPointAccelRate = 0.1f;
    static const f32 sNearHangPointAccelRate = 0.02f;
    static const f32 sNearPointDiffMin = 1.0f;
}  // namespace

SpiderThreadPointNearInfoSMG1::SpiderThreadPointNearInfoSMG1() {
    mMainPoint = nullptr;
    mPartPoint = nullptr;
    mDistToPoint = 0.0f;
    mSpringFactor = 0.0f;
    mThreadPart = nullptr;
}


SpiderThreadMainPointSMG1::SpiderThreadMainPointSMG1(const TVec3f& rPos, int nearInfoBufferSize) {
    //SpiderThreadPoint
    mBasePos = rPos;
    mPosition = rPos;
    mVelocity.zero();
    mFriction = sFrictionRate;
    mUp.set(0.0f, 1.0f, 0.0f);
    mFront.set(0.0f, 0.0f, 1.0f);

    mWindStartTime = getWindCtrl()->getTimeToStartWind();
    mWindTime = getWindCtrl()->getWindTime();
    mPrevStretchDist = 0.0f;

    // SpiderThreadMainPoint
    mRadialLine = nullptr;
    mNearInfoBufferSize = nearInfoBufferSize;
    mNumNearInfos = 0;
    mNearInfos = new SpiderThreadPointNearInfoSMG1[nearInfoBufferSize];
}

void SpiderThreadMainPointSMG1::addNearMainPoint(SpiderThreadMainPoint* pMainPoint, SpiderThreadPart* pThreadPart) {
    getNearPointInfo(mNumNearInfos)->mMainPoint = pMainPoint;
    getNearPointInfo(mNumNearInfos)->setInfo(sNearMainPointAccelRate, pThreadPart, mPosition);
    mNumNearInfos++;
}

void SpiderThreadMainPointSMG1::cutNearPoints(s32* numCutPoints, SpiderThreadMainPoint** pMainPoints) {
    SpiderThreadPartSMG1* threadPart;
    for (s32 idx = 0; idx < mNumNearInfos; idx++) {
        SpiderThreadMainPointSMG1* mainPoint;
        if (getNearPointInfo(idx)->mMainPoint != nullptr) {
            getNearPointInfo(idx)->mMainPoint->removeNearPoint(this);

            mainPoint = (SpiderThreadMainPointSMG1*)pMainPoints[*numCutPoints];
            threadPart = (SpiderThreadPartSMG1*)getNearPointInfo(idx)->mThreadPart;

            ((SpiderThreadMainPointSMG1*)getNearPointInfo(idx)->mMainPoint)->addNearMainPoint(mainPoint, threadPart);
            mainPoint->mPosition.set(mPosition);
            mainPoint->addNearMainPoint(getNearPointInfo(idx)->mMainPoint, threadPart);
            threadPart->cut(this, mainPoint);

            *numCutPoints = *numCutPoints + 1;
        }
    }
}
#pragma endregion

#pragma region SpiderThreadPart
namespace {
    static Color8 sColorPlusX(0x64, 0x64, 0x64, 0xFF);
    static Color8 sColorMinusX(0x96, 0x96, 0x96, 0xFF);
    static Color8 sColorMinusY(0xFF, 0xFF, 0xFF, 0xFF);

    static const f32 sPointInterval = 50.0f;
    static const f32 sFricitonLengthMin = 200.0f;
    static const f32 sFricitonLengthMax = 700.0f;
    // static const _32 sFrictionLengthRateMin =
    // static const _32 sFrictionLengthRateMax =
    // static const _32 sFrictionTipMin =
    static const s32 sTouchTimerMax = 5;
    static const s32 sActorToucnTimerMax = 60;
    static const s32 sForceAttachTime = 20;
    static const f32 sWindFrictionRate_0 = 0.93f;
    // static const f32 sScaleMin =
    // static const f32 sDrawWidthX =
    // static const f32 sDrawWidthY =
    // static const f32 sDrawWidthLongOffsetX =
    // static const f32 sDrawWidthShortOffsetX =
    // static const f32 sDrawWidthShortOffsetY =
    static const f32 sTexRateV = 0.002f;
    // static const f32 sIndirectTexEndU =
    static const f32 sIndirectTexRateV = 0.001f;
    static const f32 sIndirectTexSpeed = -0.002f;
}  // namespace

SpiderThreadPartSMG1::SpiderThreadPartSMG1(SpiderThreadMainPoint* pPointA, SpiderThreadMainPoint* pPointB, f32 boundBoxPadding) {
    mPointA = pPointA;
    mIsStill = true;
    mIsCut = false;
    mIsFree = false;
    _4 = 0;
    mNumHangingPoints = 0;
    mStopTime = 0;
    mTouchTimeout = 0;
    mPointerPosPad0.zero();
    mPointerPosPad1.zero();
    mPointB = pPointB;
    mNumPoints = 0;
    mPoints = nullptr;
    mSide.set(1.0f, 0.0f, 0.0f);
    mUp.set(0.0f, 1.0f, 0.0f);
    mFront.set(0.0f, 0.0f, 1.0f);
    mDistBetweenMainPoints = 0.0f;
    mDistBetweenPoints = 0.0f;
    mTextureOffset = 0.0f;

    TVec3f forward(mPointB->mPosition);
    forward.sub(mPointA->mPosition);
    mDistBetweenMainPoints = PSVECMag(forward.toVecPtr());
    mNumPoints = mDistBetweenMainPoints / sPointInterval;
    mDistBetweenPoints = mDistBetweenMainPoints / (mNumPoints + 1);
    MR::normalize(&forward);

    mPoints = new SpiderThreadPoint * [mNumPoints];

    for (s32 idx = 0; idx < mNumPoints; idx++) {
        TVec3f pos(forward);
        pos.scale(mDistBetweenPoints * (idx + 1));
        pos.add(mPointA->mPosition);

        s32 halfNumPoints = mNumPoints / 2;
        s32 index = __abs(halfNumPoints - idx);
        f32 indexNormalized = 1.0f - (static_cast<f32>(index) / static_cast<f32>(halfNumPoints));

        f32 length = mDistBetweenMainPoints - sFricitonLengthMin;
        if (length < 0.0f) {
            length = 0.0f;
        }
        else if (length > sFricitonLengthMax - sFricitonLengthMin) {
            length = sFricitonLengthMax - sFricitonLengthMin;
        }

        f32 lengthNormalized = length / (sFricitonLengthMax - sFricitonLengthMin);

        f32 indexSpring = ((0.9f * (1.0f - MR::getEaseOutValue(indexNormalized, 0.0f, 1.0f, 1.0f))) + MR::getEaseOutValue(indexNormalized, 0.0f, 1.0f, 1.0f));
        f32 lengthSpring = (0.9f * (1.0f - lengthNormalized) + (0.9f * lengthNormalized));
        f32 friction = indexSpring * lengthSpring;

        mPoints[idx] = new SpiderThreadPointSMG1(pos, friction);
    }

    mSide.x = 0.0f;
    mSide.y = 0.0f;
    mSide.z = -1.0f;

    mUp.set(forward.y, -forward.x, 0.0f);
    mFront.set(-forward.y, forward.x, 0.0f);

    mBoundBox.mPointMin.zero();
    mBoundBox.mPointMax.zero();
    mBoundBox.extend(mPointA->mPosition);
    mBoundBox.extend(mPointB->mPosition);

    padBox(boundBoxPadding);

    f32 minZ = mPointA->mPosition.z >= mPointB->mPosition.z ? mPointB->mPosition.z : mPointA->mPosition.z;
    TVec3f far(mBoundBox.mPointMin.x, mBoundBox.mPointMin.y, minZ - 1000.0f);

    f32 maxZ = mPointA->mPosition.z >= mPointB->mPosition.z ? mPointA->mPosition.z : mPointB->mPosition.z;
    TVec3f near(mBoundBox.mPointMax.x, mBoundBox.mPointMax.y, maxZ + 100.0f);
    mBoundBox.extend(far);
    mBoundBox.extend(near);
}

void SpiderThreadPartSMG1::cut(SpiderThreadMainPoint* pPointA, SpiderThreadMainPoint* pPointB) {
    if (pPointA == mPointA) {
        mPointA = mPointB;
        mPointB = pPointB;
    }
    else {
        mPointB = pPointB;
    }

    if (mIsCut) {
        mIsFree = true;
    }

    mIsCut = true;
    mIsStill = false;
}

void SpiderThreadPartSMG1::stopAllPoints() {
    if (mIsStill) {
        return;
    }

    mStopTime = sForceAttachTime;
}

f32 SpiderThreadPartSMG1::calcNearestPointInfo(const TVec3f** pPos, const TVec3f** pNeutralPos, s32* pPointNum, const TVec3f& rPos) {
    if (mIsStill) {
        updatePointPosToLine();
    }
    f32 dist = findNearestPointNo(pPointNum, rPos);
    *pPos = &getPoint(*pPointNum)->mPosition;
    *pNeutralPos = &getPoint(*pPointNum)->mBasePos;
    return dist;
}

bool SpiderThreadPartSMG1::touchActor(s32 index, const TVec3f& rVel) {
    if (mTouchTimeout > 0) {
        return false;
    }

    // TODO: this really screams vector arithmetic inline
    f32 projUp = mUp.dot(rVel);
    f32 projFront = mFront.dot(rVel);
    TVec3f scaleUp(mUp);
    TVec3f scaleFront(mFront);
    scaleUp.scale(projUp);
    scaleFront.scale(projFront);
    TVec3f vel(scaleUp);
    vel.add(scaleFront);

    getPoint(index)->mVelocity.add(vel);
    updatePointVelocityTouch(index, vel);
    mIsStill = false;
    mTouchTimeout = sActorToucnTimerMax;

    return true;
}

f32 SpiderThreadPartSMG1::findNearestPointNo(s32* pPointNum, const TVec3f& rPos) const {
    *pPointNum = 0;
    f32 minDistance = MR::calcDistanceXY(getPoint(0)->mPosition, rPos);

    f32 dist;
    for (s32 idx = 1; idx < mNumPoints; idx++) {
        dist = MR::calcDistanceXY(getPoint(idx)->mPosition, rPos);
        if (dist < minDistance) {
            *pPointNum = idx;
            minDistance = dist;
        }
    }

    return minDistance;
}
#pragma endregion

#pragma region SpiderThreadPoint
namespace {
    static const f32 sAccelRate = 0.02f;
    static const f32 sSpeedMinToStop = 0.1f;
    static const f32 sDistanceToStop = 1.0f;
    // static const _32 sPushAccelRate =
}  // namespace

SpiderThreadPointSMG1::SpiderThreadPointSMG1(const TVec3f& rPos, f32 friction) {
    mBasePos = rPos;
    mPosition = rPos;
    mVelocity.zero();
    mFriction = friction;
    mUp.set(0.0f, 1.0f, 0.0f);
    mFront.set(0.0f, 0.0f, 1.0f);

    mWindStartTime = getWindCtrl()->getTimeToStartWind();
    mWindTime = getWindCtrl()->getWindTime();
    mPrevStretchDist = 0.0f;
}

bool SpiderThreadPointSMG1::tryPush(const TVec3f& rPos, f32 radius) {
    f32 dist = MR::calcDistanceXY(mPosition, rPos);
    if (dist > radius || mPosition.z < rPos.z) {
        return false;
    }

    mVelocity.z += getPosZDiff(rPos);

    return true;
}
#pragma endregion

#pragma region SpiderThreadRadialLine
SpiderThreadRadialLineSMG1::SpiderThreadRadialLineSMG1(int bufferSize) {
    mBufferSize = bufferSize;
    mNumPoints = 0;

    mPoints = new SpiderThreadMainPoint * [mBufferSize];
    for (s32 idx = 0; idx < mBufferSize; idx++) {
        mPoints[idx] = nullptr;
    }
}

void SpiderThreadRadialLineSMG1::addPoint(SpiderThreadMainPoint* pPoint) {
    mPoints[mNumPoints] = pPoint;
    mNumPoints++;
}
#pragma endregion

#pragma region SpiderThreadWindCtrl
namespace {
    static const f32 sWindAccelBattle = 0.5f;
    static const f32 sWindAccelChance = 1.0f;
    // static const _32 sWindAccelDamage =
    static const f32 sWindAccelBattleEndTarget = 2.0f;
    static const f32 sWindAccelBattleEndCurrent = 100.0f;
    static const f32 sWindAccelSpeedBattleEnd = 0.4f;
    static const s32 sTimeToStartWindMin = 30;
    static const s32 sTimeToStartWindMax = 120;
    static const s32 sWindTimeMin = 60;
    static const s32 sWindTimeMax = 120;
}  // namespace

SpiderThreadWindCtrlSMG1::SpiderThreadWindCtrlSMG1() {
    mWindTargetSpeed = sWindAccelBattle;
    mWindSpeed = sWindAccelBattle;
    mWindAccel = 0.0f;
    mWindDirection.set(0.0f, 0.0f, -1.0f);
    mWind.set(mWindDirection);
}

void SpiderThreadWindCtrlSMG1::startWindBattle() {
    mWindSpeed = sWindAccelBattle;
    mWindTargetSpeed = sWindAccelBattle;
    mWindAccel = 0.0f;
}

void SpiderThreadWindCtrlSMG1::startWindChance() {
    mWindSpeed = sWindAccelChance;
    mWindTargetSpeed = sWindAccelChance;
    mWindAccel = 0.0f;
}

void SpiderThreadWindCtrlSMG1::startWindBattleEnd() {
    mWindTargetSpeed = sWindAccelBattleEndTarget;
    mWindSpeed = sWindAccelBattleEndCurrent;
    mWindAccel = sWindAccelSpeedBattleEnd;
}
#pragma endregion


namespace MR {
    inline SpiderThreadSMG1* getSpiderThread() {
        return (SpiderThreadSMG1*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_SPIDER_THREAD);
    }

    void initSpiderThread(const TVec3f& rPos) {
        getSpiderThread()->initThread(rPos);
    }

    void appearSpiderThread() {
        getSpiderThread()->appear();
    }

    void startSpiderThreadBattle() {
        getSpiderThread()->stopAllPartsPoint();
        ((SpiderThreadWindCtrlSMG1*)getSpiderThread()->mWindCtrl)->startWindBattle();
    }

    void startSpiderThreadChance() {
        getSpiderThread()->stopAllPartsPoint();
        ((SpiderThreadWindCtrlSMG1*)getSpiderThread()->mWindCtrl)->startWindChance();
    }

    void startSpiderThreadBattleEnd() {
        getSpiderThread()->stopAllPartsPoint();
        getSpiderThread()->cutSpiderThread();
        ((SpiderThreadWindCtrlSMG1*)getSpiderThread()->mWindCtrl)->startWindBattleEnd();
    }

    void pauseOffSpiderThread() {
        MR::requestMovementOn(getSpiderThread());
    }

    f32 getSpiderThreadPosZ() {
        if (!MR::isExistSceneObj(SCENE_OBJ_SPIDER_THREAD)) {
            return 0.0f;
        }

        return getSpiderThread()->mTranslation.z;
    }

    void startActorBindToSpiderThread(LiveActor* pActor, const TVec3f** pPointPos, const TVec3f** pNeutralPos, const TVec3f** pUp, const TVec3f& rPos, const TVec3f& rVel) {
        getSpiderThread()->startActorBind(pActor, pPointPos, pNeutralPos, pUp, rPos, rVel, 1);
    }

    bool sendMsgToSpiderThread(u32 msg, HitSensor* pSender) {
        return getSpiderThread()->receiveMessage(msg, pSender, MR::getMessageSensor());
    }

    bool touchActorToSpiderThread(const TVec3f& rPos, const TVec3f& rVel) {
        return getSpiderThread()->touchActor(rPos, rVel);
    }

    void tryPushSpiderThread(const TVec3f& rPos, f32 radius) {
        getSpiderThread()->tryPush(rPos, radius);
    }

    void onSpiderThreadBloom() {
        getSpiderThread()->mIsBloomOn = true;
    }
}