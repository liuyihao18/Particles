/*
 * reference: CUDA samples (particles)
 */

#pragma once

#include <vector_types.h>

using uint = unsigned int;

// simulation parameters
struct SimParams {
    int numSpheres;

    // Grid
    float3 cellSize;

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

struct SimProto {
    float radius[4];
    float mass[4];
    float damping[4][4];
    float restitution[4][4];
};
