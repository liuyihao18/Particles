#include "system.h"
#include "utils.h"
#include "sphere.h"
#include "system.cuh"

#include <helper_math.h>

System::System(uint numParticles, float3 origin, float size)
    : numParticles(numParticles), origin(origin), size(size), 
    numCells(GRID_SIZE * GRID_SIZE * GRID_SIZE)
{
    initialize();
}

System::~System()
{
    finalize();
}

float* System::getPos()
{
    copyArrayFromDevice(hPos, dPos, numParticles * sizeof(float) * 3);
    return hPos;
}

void System::update(float deltaT)
{
    calcHash(dGridParticleHash, dGridParticleIndex, dPos, numParticles);
    sortParticles(dGridParticleHash, dGridParticleIndex, numParticles);
    findCellStart(dCellStart, dCellEnd, dGridParticleHash, numParticles, numCells);
    collide(dPos, dVel, dAccel, dType, dGridParticleIndex, dCellStart, dCellEnd, numParticles, deltaT);
}

uint* System::getType()
{
    return hType;
}

void System::initParams()
{
    // compute protos
    float maxRadius = 0.0f;
    for (int i = 0; i < PROTO_NUM; i++) {
        Sphere* proto = Sphere::GetProto(i);
        maxRadius = fmaxf(maxRadius, proto->radius);
        protos.mass[i] = proto->mass;
        protos.radius[i] = proto->radius;
        for (int j = 0; j < PROTO_NUM; j++) {
            protos.restitution[i][j] = RESTITUION[i][j];
            float temp = logf(RESTITUION[i][j]);
            protos.damping[i][j] = -2 * temp / sqrtf(temp * temp + PI * PI);
        }
    }
    
    // compute params
    params.gridSize = GRID_SIZE;
    params.cellSize = cellSize = maxRadius * 2;
    params.minCorner = make_float3(
        origin.x - 0.5 * size,
        origin.y - 0.5 * size,
        origin.z - 0.5 * size
    );
    params.maxCorner = make_float3(
        origin.x + 0.5 * size,
        origin.y + 0.5 * size,
        origin.z + 0.5 * size
    );
    params.gravity = make_float3(
        0,
        -GRAVITY,
        0
    );
    params.spring = SPRING;
    params.decay = DECAY;
    params.damping = DAMPING;
    params.friction = FRICTION;
}

void System::initParticles()
{
    // Initialize as cubic
    float cellCenter = cellSize * 0.5;
    float posRandomMax = cellSize * 0.5 * 0.01;
    float velRandomMax = cellSize * 0.5 * 0.1;
    int cubeSize = (int)ceilf(powf((float)numParticles, 1.0 / 3.0f));
    for (int x = 0; x < cubeSize; x++) {
        for (int y = 0; y < cubeSize; y++) {
            for (int z = 0; z < cubeSize; z++) {
                int index = (x * cubeSize + y) * cubeSize + z;
                if (index < numParticles) {
                    hPos[index * 3] = origin.x + (x - cubeSize / 2.0) * cellSize + cellCenter + randomFloat(0, posRandomMax);
                    hPos[index * 3 + 1] = origin.y + (y - cubeSize / 2.0) * cellSize + cellCenter + randomFloat(0, posRandomMax);
                    hPos[index * 3 + 2] = origin.z + (z - cubeSize / 2.0) * cellSize + cellCenter + randomFloat(0, posRandomMax);
                    hVel[index * 3] = randomFloat(0, velRandomMax);
                    hVel[index * 3 + 1] = randomFloat(0, velRandomMax);
                    hVel[index * 3 + 2] = randomFloat(0, velRandomMax);
                    hType[index] = rand() % PROTO_NUM;
                }
            }
        }
    }
}

void System::initialize()
{
    // CPU Data
    hPos = new float[numParticles * 3]();
    hVel = new float[numParticles * 3]();
    hType = new uint[numParticles]();

    initParams();
    initParticles();

    // GPU Data
    allocateArray((void**)&dPos, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dVel, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dAccel, numParticles * sizeof(float) * 3);
    initializeArray(dAccel, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dType, numParticles * sizeof(uint));
    allocateArray((void**)&dGridParticleHash, numParticles * sizeof(uint));
    allocateArray((void**)&dGridParticleIndex, numParticles * sizeof(uint));
    allocateArray((void**)&dCellStart, numCells * sizeof(uint));
    allocateArray((void**)&dCellEnd, numCells * sizeof(uint));

    copyArrayToDevice(dPos, hPos, 0, numParticles * sizeof(float) * 3);
    copyArrayToDevice(dVel, hVel, 0, numParticles * sizeof(float) * 3);
    copyArrayToDevice(dType, hType, 0, numParticles * sizeof(uint));

    setParameters(&params);
    setProtos(&protos);
}

void System::finalize()
{
    freeArray(dPos);
    freeArray(dVel);
    freeArray(dAccel);
    freeArray(dType);
    freeArray(dGridParticleHash);
    freeArray(dGridParticleIndex);
    freeArray(dCellStart);
    freeArray(dCellEnd);

    delete[] hPos;
    delete[] hVel;
    delete[] hType;
}
