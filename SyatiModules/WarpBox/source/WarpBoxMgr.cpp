#include "WarpBoxMgr.h"
#include "syati.h"

WarpBoxMgr::WarpBoxMgr (const char *pName) : NameObj(pName) {
    mNumEntries = 0;
    mEntries = new Entry[WARP_CUBE_LIMIT];
    for (s32 i = 0; i < WARP_CUBE_LIMIT; i++) {
        mEntries[i].mEntranceCube = 0;
        mEntries[i].mExitCube = 0;
        mEntries[i].mConnectionID = -1;
    }
}

WarpBoxMgr::~WarpBoxMgr () {

}

void WarpBoxMgr::entry (WarpBox *pWarpBox, const JMapInfoIter &rIter) {
    s32 connectionID = -1;
    MR::getJMapInfoArg3NoInit(rIter, &connectionID);
    for (s32 i = 0; i < mNumEntries; i++) {
        WarpBoxMgr::Entry *pEntry = &mEntries[i];
        if (connectionID == pEntry->mConnectionID) {
            OSReport("Found box, adding box at %d\n", i);
            pEntry->mExitCube = pWarpBox;
            return;
        }
    }
    WarpBoxMgr::Entry *pEntry = &mEntries[mNumEntries++];
    pEntry->mEntranceCube = pWarpBox;
    pEntry->mConnectionID = connectionID;
    OSReport("Added box at %d\n", mNumEntries - 1);
}

void WarpBoxMgr::entry (WarpBoxExitPoint *pExitPoint, const JMapInfoIter &rIter) {
    s32 connectionID = -1;
    MR::getJMapInfoArg0NoInit(rIter, &connectionID);
    for (s32 i = 0; i < mNumEntries; i++) {
        WarpBoxMgr::Entry *pEntry = &mEntries[i];
        if (connectionID == pEntry->mConnectionID) {
            OSReport("Found box, adding exit point at %d\n", i);
            pEntry->mExitPoint = pExitPoint;
            return;
        }
    }
    WarpBoxMgr::Entry *pEntry = &mEntries[mNumEntries++];
    pEntry->mExitPoint = pExitPoint;
    pEntry->mConnectionID = connectionID;
    OSReport("Added exit point at %d\n", mNumEntries - 1);
}

WarpBox *WarpBoxMgr::getOppositeCube (WarpBox *pWarpBox) {
    for (s32 i = 0; i < mNumEntries; i++) {
        WarpBoxMgr::Entry *pEntry = &mEntries[i];
        if (pEntry->mEntranceCube == pWarpBox && pEntry->mExitCube && !MR::isDead((LiveActor *)pEntry->mExitCube)) {
            OSReport("WarpBoxMgr: Exit cube\n");
            return pEntry->mExitCube;
        }
        else if (pEntry->mExitCube == pWarpBox && pEntry->mEntranceCube && !MR::isDead((LiveActor *)pEntry->mEntranceCube)) {
            OSReport("WarpBoxMgr: Entrance cube\n");
            return pEntry->mEntranceCube;
        }
    }
    OSReport("WarpBoxMgr: Opposite cube not found!\n");
    return 0;
}

WarpBoxExitPoint *WarpBoxMgr::getExitPoint (WarpBox *pWarpBox) {
    for (s32 i = 0; i < mNumEntries; i++) {
        WarpBoxMgr::Entry *pEntry = &mEntries[i];
        if (pEntry->mEntranceCube == pWarpBox || pEntry->mExitCube == pWarpBox) 
            return pEntry->mExitPoint;
    }
    OSReport("WarpBoxMgr: Exit point not found!\n");
    return 0;
}

NameObj *WarpBoxMgr::createWarpBoxMgr () {
    return new WarpBoxMgr("WarpBoxMgr");
}

WarpBoxMgr *WarpBoxMgr::getManager () {
    return (WarpBoxMgr *)MR::getSceneObjHolder()->getObj(EXT_SCENE_OBJ_WARP_CUBE_MANAGER);
}