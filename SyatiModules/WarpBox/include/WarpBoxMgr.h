#pragma once

#include "syati.h"

#define WARP_CUBE_LIMIT 32
#define EXT_SCENE_OBJ_WARP_CUBE_MANAGER 73

class WarpBox;
class WarpBoxExitPoint;

class WarpBoxMgr : public NameObj {
public:
    WarpBoxMgr(const char *pName);
    
    struct Entry {
        WarpBox *mEntranceCube;
        WarpBox *mExitCube;
        WarpBoxExitPoint *mExitPoint;
        s32 mConnectionID;
    };
    
    ~WarpBoxMgr();
    void entry(WarpBox *pWarpBox, const JMapInfoIter &rIter);
    void entry(WarpBoxExitPoint *pExitPoint, const JMapInfoIter &rIter);
    WarpBox *getOppositeCube(WarpBox *pWarpBox);
    WarpBoxExitPoint *getExitPoint(WarpBox *pWarpBox);
    
    static NameObj *createWarpBoxMgr();
    static WarpBoxMgr *getManager();

    s32 mNumEntries;
    Entry *mEntries;
};