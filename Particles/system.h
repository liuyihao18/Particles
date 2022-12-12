#pragma once

#include "param.h"
#include "config.h"

class System
{
public:
    System() = default;
    System(
        uint numParticles,
        float3 origin,
        float size
    );
    ~System();

    // 初始化
    void init(
        uint numParticles,
        float3 origin,
        float size
    );

    // 重新开始
    void restart();

    // 返回粒子的位置
    float *getPos();

    // 更新粒子的位置
    void updateParticles(float deltaT);

    // 返回粒子的种类
    uint *getType();

    // 改变重力方向
    void setUp(float3 up);

protected:
    // 初始化参数
    void initParams();

    // 初始化粒子
    void initParticles();

    // 初始化
    void initialize(
        uint numParticles,
        float3 origin,
        float size
    );

    // 后处理
    void finalize();

private:
    uint numParticles{ 0 }; // 粒子数量

    /* CPU */
    float *hPos{ nullptr }; // 位置
    float *hVel{ nullptr }; // 速度
    uint *hType{ nullptr }; // 类型

    /* GPU */
    float *dPos{ nullptr };              // 位置
    float *dVel{ nullptr };              // 速度
    float *dAccel{ nullptr };            // 加速度
    uint *dType{ nullptr };              // 类型
    uint *dGridParticleHash{ nullptr };  // 哈希
    uint *dGridParticleIndex{ nullptr }; // 索引
    uint *dCellStart{ nullptr };         // 哈希反向索引起始
    uint *dCellEnd{ nullptr };           // 哈希反向索引结束

    /* 参数 */
    SimParams params{ 0 }; // 环境
    SimProtos protos{ 0 }; // 原型
    float cellSize{ 0 };   // 单元大小
    uint numCells{ 0 };    // 单元数

    /* 容器 */
    float3 origin{ 0 }; // 原点
    float size{ 0 };    // 大小
};
