#pragma once

class LiveActor;
class PlanetMap;
class ModelObj;
class JMapInfoIter;

class TombSpiderEnvironment {
public:
    TombSpiderEnvironment(LiveActor*);

    void initPlanet(const JMapInfoIter& rIter);
    void initCocoon(const JMapInfoIter& rIter);

    /* 0x00 */ LiveActor* mActor;
    /* 0x04 */ PlanetMap* mPlanet;
    /* 0x08 */ ModelObj* mCocoon;
};
