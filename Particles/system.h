#pragma once

#include "param.h"

#include <QVector3D>

class System
{
public:
    System(
        uint numParticles,
        const QVector3D& origin,
        float size
    );
    ~System();

    float* getPos();
    void update();
    uint* getSphereType();

protected:
    void initParams();
    void initialize();
    void finalize();

private:
    uint numParticles;

    // CPU data
    float* hPos;
    float* hVel;
    float* hAccel;
    uint* hType;
    uint* hParticleHash;
    uint* hCellStart;
    uint* hCelllEnd;

    // GPU data
    float* dPos;
    float* dVel;
    float* dAccel;
    uint* dType;
    uint* dParticleHash;
    uint* dParticleIndex;
    uint* dCellStart;
    uint* dCelllEnd;

    // params
    SimParams params;
    SimProtos protos;
    float3 cellSize;
    uint numCells;

    // container
    QVector3D origin;
    float size;
};

