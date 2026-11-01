#include "syati.h"

class SilentRealmEntrance : public MapObjActor {
  public:
  SilentRealmEntrance(const char *);

  virtual void init(const JMapInfoIter &);
  virtual void control();

  void changeScene();

  s32 mChangeSceneIndex;
  s32 mColorFrame;

  const JMapInfo *mTable;
};