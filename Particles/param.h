/*
 * reference: CUDA samples (particles)
 */

#pragma once

#include "constant.h"

#include <vector_types.h>

using uint = unsigned int;

// simulation parameters
struct SimParams {
    // Grid
    uint gridSize;
    float cellSize;

    // Container
    float3 minCorner;
    float3 maxCorner;

    // Phisical
    float3 gravity;
    float spring;
    float decay;
    float damping;
    float friction;
};

struct SimProtos {
    float radius[PROTO_NUM];
    float mass[PROTO_NUM];
    float damping[PROTO_NUM][PROTO_NUM];
    float elasticity[PROTO_NUM][PROTO_NUM];
};
