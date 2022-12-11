#pragma once

#include "param.h"
#include "config.h"

class System
{
public:
    System(
        uint numParticles,
        float3 origin,
        float size
    );
    ~System();

    // 返回粒子的位置
    float *getPos();

    // 更新粒子的位置
    void update(float deltaT);

    // 返回粒子的种类
    uint *getType();

protected:
    // 初始化参数
    void initParams();

    // 初始化粒子
    void initParticles();

    // 初始化
    void initialize();

    // 后处理
    void finalize();

private:
    uint numParticles;

    /* CPU */
    float *hPos; // 位置
    float *hVel; // 速度
    uint *hType; // 类型

    /* GPU */
    float *dPos;              // 位置
    float *dVel;              // 速度
    float *dAccel;            // 加速度
    uint *dType;              // 类型
    uint *dGridParticleHash;  // 哈希
    uint *dGridParticleIndex; // 索引
    uint *dCellStart;         // 哈希反向索引起始
    uint *dCellEnd;           // 哈希反向索引结束

    /* 参数 */
    SimParams params; // 环境
    SimProtos protos; // 原型
    float cellSize;   // 单元大小
    uint numCells;    // 单元数

    /* 容器 */
    float3 origin; // 原点
    float size;    // 大小
};
