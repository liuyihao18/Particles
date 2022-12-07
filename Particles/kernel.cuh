/*
 * reference: CUDA samples (particles)
 */

#pragma once

#include "morton.cuh"
#include "param.h"

#include <cuda_runtime.h>
#include <helper_math.h>

#define GET_INDEX __umul24(blockIdx.x, blockDim.x) + threadIdx.x

__constant__ SimParams params;
__constant__ SimProto protos;
__constant__ int3 neighborhood[27] = {
	-1, -1, -1,
	 0, -1, -1,
	 1, -1, -1,
	-1,  0, -1,
	 0,  0, -1,
	 1,  0, -1,
	-1,  1, -1,
	 0,  1, -1,
	 1,  1, -1,
	-1, -1,  0,
	 0, -1,  0,
	 1, -1,  0,
	-1,  0,  0,
	 0,  0,  0,
	 1,  0,  0,
	-1,  1,  0,
	 0,  1,  0,
	 1,  1,  0,
	-1, -1,  1,
	 0, -1,  1,
	 1, -1,  1,
	-1,  0,  1,
	 0,  0,  1,
	 1,  0,  1,
	-1,  1,  1,
	 0,  1,  1,
	 1,  1,  1,
};

__device__ int3 calcGridPos(float3 p) {
	int3 gridPos = {
		static_cast<int>(floorf(p.x / params.cellSize.x)),
		static_cast<int>(floorf(p.y / params.cellSize.y)),
		static_cast<int>(floorf(p.z / params.cellSize.z))
	};
	return gridPos;
}

__device__ uint calcGridHash(int3 gridPos) {
	return EncodeMorton3(gridPos);
}

__global__ void calcHashD(
	uint* gridParticleHash,   // output
	uint* gridParticleIndex,  // output
	float3* pos               // input: positions
) {
	uint index = GET_INDEX;

	if (index >= params.numSpheres) return;

	// get address in grid
	int3 gridPos = calcGridPos(pos[index]);
	uint hash = calcGridHash(gridPos);

	// store grid hash and particle index
	gridParticleHash[index] = hash;
	gridParticleIndex[index] = index;
}

__global__ void findCellStartD(
	uint* cellStart,          // output: cell start index
	uint* cellEnd,            // output: cell end index
	uint* gridParticleHash    // input: sorted grid hashes
) {
	uint index = GET_INDEX;
	
	if (index >= params.numSpheres) return;
	
	uint hash = gridParticleHash[index];

	if (index == 0) {
		cellStart[hash] = index;
	}
	else if (hash != gridParticleHash[index - 1]) {
		cellStart[hash] = index;
		cellEnd[gridParticleHash[index - 1]] = index;
	}
	if (index == params.numSpheres - 1) {
		cellEnd[hash] = index + 1;
	}
}

__device__ float3 collideSpheres(
	float3 posA,
	float3 posB,
	float3 velA,
	float3 velB,
	uint typeA,
	uint typeB
) {
	float radiusA = protos.radius[typeA];
	float radiusB = protos.radius[typeB];
	float massA = protos.mass[typeA];
	float massB = protos.mass[typeB];

	float3 relPos = posB - posA;
	float dist = length(relPos);
	float collidDist = radiusA + radiusB;

	float3 force = { 0 };
	
	if (dist < collidDist) {
		// vel decompose
		float3 normal = relPos / dist;
		float3 relVel = velB - velA;
		float3 normVel = dot(relVel, normal) * normal;
		float3 tanVel = relVel - normVel;
		
		// spring
		force -= params.spring * (collidDist - dist) * normal;
		// damping
		float damping = protos.damping[typeA][typeB];
		float massSqrt = sqrtf((massA * massB) / (massA + massB));
		force += (params.damping * damping * massSqrt) * normVel;
		// friction
		float normForce = dot(force, normal);
		force -= (params.friction * normForce) * tanVel;
	}

	return force;
}

