/*
 * reference: CUDA samples (particles)
 */

#pragma once

#include "constant.h"

#include <vector_types.h>

using uint = unsigned int;

// 环境参数
struct SimParams
{
    /* Grid */
    uint gridSize;  // 网格大小
    float cellSize; // 单元大小（一个网格的大小）

    /* Container */
    float3 minCorner; // 正方体的最小点
    float3 maxCorner; // 正方体的最大点

    /* Physical */
    float3 gravity; // 重力
    float spring;   // 弹力
    float damping;  // 阻尼
    float friction; // 摩擦
    float decay;    // 衰减
};

// 原型参数
struct SimProtos
{
    float radius[PROTO_NUM];                // 半径
    float mass[PROTO_NUM];                  // 质量
    float damping[PROTO_NUM][PROTO_NUM];    // 阻尼
    float elasticity[PROTO_NUM][PROTO_NUM]; // 弹性
};
