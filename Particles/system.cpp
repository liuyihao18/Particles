#include "system.h"
#include "sphere.h"
#include "system.cuh"

#include <helper_math.h>

#include <random>

System::System(uint numParticles, const QVector3D& origin, float size)
    : numParticles(numParticles), origin(origin), size(size), 
    numCells(GRID_SIZE * GRID_SIZE * GRID_SIZE)
{
    initialize();
}

System::~System()
{
    finalize();
}

void System::initParams()
{
    // compute protos
    float maxRadius = 0.0f;
    for (int i = 0; i < PROTO_NUM; i++) {
        Sphere proto = Sphere::GetProto(i);
        maxRadius = fmaxf(maxRadius, proto.radius);
        protos.mass[i] = proto.mass;
        protos.radius[i] = proto.radius;
        for (int j = 0; j < PROTO_NUM; j++) {
            protos.restitution[i][j] = RESTITUION[i][j];
            float temp = logf(RESTITUION[i][j]);
            protos.damping[i][j] = -2 * temp / sqrtf(temp * temp + PI * PI);
        }
    }
    
    // compute params
    params.cellSize = cellSize = maxRadius * 2;
    params.minCorner = make_float3(
        origin.x() - 0.5 * size,
        origin.y() - 0.5 * size,
        origin.z() - 0.5 * size
    );
    params.maxCorner = make_float3(
        origin.x() + 0.5 * size,
        origin.y() + 0.5 * size,
        origin.z() + 0.5 * size
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

inline float random() {
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    static std::mt19937 gen{ std::random_device{}() };
    return dis(gen);
}

inline float random(float min, float max) {
    return min + (max - min) * random();
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
                int index = (x * 2 * cubeSize + y) * cubeSize + z;
                if (index < numParticles) {
                    hPos[index * 3] = origin.x() + (x - cubeSize / 2.0) * cellSize + cellCenter + random(0, posRandomMax);
                    hPos[index * 3 + 1] = origin.y() + (y - cubeSize / 2.0) * cellSize + cellCenter + random(0, posRandomMax);
                    hPos[index * 3 + 2] = origin.z() + (z - cubeSize / 2.0) * cellSize + cellCenter + random(0, posRandomMax);
                    hVel[index * 3] = random(0, velRandomMax);
                    hVel[index * 3 + 1] = random(0, velRandomMax);
                    hVel[index * 3 + 2] = random(0, velRandomMax);
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
    hAccel = new float[numParticles * 3]();

    initParams();
    initParticles();

    // GPU Data
    allocateArray((void**)&dPos, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dVel, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dAccel, numParticles * sizeof(float) * 3);
    initializeArray((void**)&dAccel, numParticles * sizeof(float) * 3);
    allocateArray((void**)&dType, numParticles * sizeof(uint));
    allocateArray((void**)&dParticleHash, numParticles * sizeof(uint));
    allocateArray((void**)&dParticleIndex, numParticles * sizeof(uint));
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
}
