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

// 返回粒子的位置
float *System::getPos()
{
    copyArrayFromDevice(hPos, dPos, numParticles * sizeof(float) * 3);
    return hPos;
}

// 更新粒子的位置
void System::update(float deltaT)
{
    // 1. 计算粒子哈希
    calcHash(dGridParticleHash, dGridParticleIndex, dPos, numParticles);
    // 2. 哈希排序
    sortParticles(dGridParticleHash, dGridParticleIndex, numParticles);
    // 3. 哈希反向索引
    findCellStart(dCellStart, dCellEnd, dGridParticleHash, numParticles, numCells);
    // 4. 计算碰撞
    collide(dPos, dVel, dAccel, dType, dGridParticleIndex, dCellStart, dCellEnd, numParticles, deltaT);
}

// 返回粒子的种类
uint *System::getType()
{
    return hType;
}

// 初始化参数
void System::initParams()
{
    // 原型参数
    float maxRadius = 0.0f;
    for (int i = 0; i < PROTO_NUM; i++)
    {
        Sphere *proto = Sphere::GetProto(i);
        maxRadius = fmaxf(maxRadius, proto->radius);
        protos.mass[i] = proto->mass;
        protos.radius[i] = proto->radius;
        for (int j = 0; j < PROTO_NUM; j++)
        {
            protos.elasticity[i][j] = ELASTICITY[i][j];
            float temp = logf(ELASTICITY[i][j]);
            protos.damping[i][j] = -2 * temp / sqrtf(temp * temp + PI * PI);
        }
    }

    // 环境参数
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

// 初始化粒子
void System::initParticles()
{
    // 粒子立方体
    int cubeSize = (int)ceilf(powf((float)numParticles, 1.0 / 3.0f));

    // 增加随机扰动
    float cellCenter = cellSize * 0.5;
    float posRandomMax = cellSize * 0.5 * 0.01;
    float velRandomMax = cellSize * 0.5 * 0.1;

    // 生成每个粒子的位置和初始速度
    for (int x = 0; x < cubeSize; x++)
    {
        for (int y = 0; y < cubeSize; y++)
        {
            for (int z = 0; z < cubeSize; z++)
            {
                int index = (x * cubeSize + y) * cubeSize + z;
                if (index < numParticles)
                {
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

// 初始化
void System::initialize()
{
    // CPU初始化
    hPos = new float[numParticles * 3]();
    hVel = new float[numParticles * 3]();
    hType = new uint[numParticles]();

    initParams();
    initParticles();

    // GPU初始化
    allocateArray((void **)&dPos, numParticles * sizeof(float) * 3);
    allocateArray((void **)&dVel, numParticles * sizeof(float) * 3);
    allocateArray((void **)&dAccel, numParticles * sizeof(float) * 3);
    initializeArray(dAccel, numParticles * sizeof(float) * 3);
    allocateArray((void **)&dType, numParticles * sizeof(uint));
    allocateArray((void **)&dGridParticleHash, numParticles * sizeof(uint));
    allocateArray((void **)&dGridParticleIndex, numParticles * sizeof(uint));
    allocateArray((void **)&dCellStart, numCells * sizeof(uint));
    allocateArray((void **)&dCellEnd, numCells * sizeof(uint));

    // 把CPU上的初始化数据送到GPU中
    copyArrayToDevice(dPos, hPos, 0, numParticles * sizeof(float) * 3);
    copyArrayToDevice(dVel, hVel, 0, numParticles * sizeof(float) * 3);
    copyArrayToDevice(dType, hType, 0, numParticles * sizeof(uint));

    setParameters(&params);
    setProtos(&protos);
}

// 后处理
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
