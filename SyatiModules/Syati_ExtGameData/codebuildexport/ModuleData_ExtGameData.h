#pragma once

#include "syati.h"
#include "Game/System/BinaryDataContentAccessor.h"
#include "Game/System/BinaryDataContentHeaderSerializer.h"



class ExtGameDataHolder : public GameDataHolder {
public:
    
};

namespace {
    typedef BinaryDataChunkBase* (*ChunkCreationFunc)();

    template<typename T>
    BinaryDataChunkBase* createBinaryChunk() {
        return new T();
    }

    const ChunkCreationFunc cExtSaveChunkCreateTable[] = {
        NULL,
        
    };

    const s32 cExtSaveChunkCount = sizeof(cExtSaveChunkCreateTable) / sizeof(ChunkCreationFunc);
};