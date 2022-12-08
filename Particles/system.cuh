/* 
 * reference: CUDA samples (particles)
 */

#pragma once

#include "param.h"

extern "C" {
    void cudaInit(int argc, char** argv);

    void allocateArray(void** devPtr, size_t size);
    void initializeArray(void* devPtr, size_t size);
    void freeArray(void* devPtr);

    void copyArrayFromDevice(void* host, const void* device, size_t size);
    void copyArrayToDevice(void* device, const void* host, int offset, int size);

    void setParameters(SimParams* hostParams);
    void setProtos(SimProtos* hostProtos);

    void calcHash(uint* gridParticleHash, uint* gridParticleIndex, float* pos, uint numParticles);
    void sortParticles(uint* dGridParticleHash, uint* dGridParticleIndex, uint numParticles);
    void findCellStart(uint* cellStart, uint* cellEnd, uint* gridParticleHash, uint numParticles, uint numCells);

    void collide(float* pos, float* vel, float* accel, uint* types,
        uint* gridParticleIndex, 
        uint* cellStart, uint* cellEnd,
        uint numParticles,
        float deltaT
    );
}