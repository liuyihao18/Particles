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
    void update(float deltaT);
    uint* getType();

protected:
    void initParams();
    void initParticles();
    void initialize();
    void finalize();

private:
    uint numParticles;

    // CPU data
    float* hPos;
    float* hVel;
    uint* hType;

    // GPU data
    float* dPos;
    float* dVel;
    float* dAccel;
    uint* dType;
    uint* dGridParticleHash;
    uint* dGridParticleIndex;
    uint* dCellStart;
    uint* dCellEnd;

    // params
    SimParams params;
    SimProtos protos;
    float cellSize;
    uint numCells;

    // container
    QVector3D origin;
    float size;
};

