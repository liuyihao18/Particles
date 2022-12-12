#pragma once
#include "constant.h"

// 粒子的数目
constexpr int PARTICLE_NUM = 5 * 5 * 5;

// 容器大小
constexpr float CUBE_SIZE = 2.0f;

// 时间戳比率
constexpr float TIMESTAMP_RATIO_FPS = 2.5;

/* 物理量配置 */
constexpr float DECAY = 0.9999f;  // 衰减
constexpr float GRAVITY = 0.2f;	  // 重力
constexpr float SPRING = 222.0f;  // 弹力
constexpr float DAMPING = 8.0f;	  // 阻尼
constexpr float FRICTION = 0.05f; // 摩擦

// 弹性
constexpr float ELASTICITY[PROTO_NUM][PROTO_NUM] = {
    0.9f, 0.9f, 0.7f, 0.8f, 0.6f,
    0.9f, 0.9f, 0.7f, 0.8f, 0.5f,
    0.7f, 0.7f, 0.4f, 0.5f, 0.8f,
    0.8f, 0.8f, 0.5f, 0.6f, 0.9f,
    0.5f, 0.9f, 0.6f, 0.7f, 0.8f
};
