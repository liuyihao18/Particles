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
__constant__ SimProtos protos;

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
	float3* pos,              // input: positions
	uint numParticles
) {
	uint index = GET_INDEX;

	if (index >= numParticles) return;

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
	uint* gridParticleHash,   // input: sorted grid hashes
	uint numParticles
) {
	uint index = GET_INDEX;
	
	if (index >= numParticles) return;
	
	uint hash = gridParticleHash[index];

	if (index == 0) {
		cellStart[hash] = index;
	}
	else if (hash != gridParticleHash[index - 1]) {
		cellStart[hash] = index;
		cellEnd[gridParticleHash[index - 1]] = index;
	}
	if (index == numParticles - 1) {
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

	float3 force = make_float3(0.0f);
	
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

__global__ void collideD(
	float3* accel,            // output
	float3* pos,              // input
	float3* vel,              // input
	uint* type,               // input
	uint* gridParticleIndex,  // input: sorted particle indices
	uint* cellStart,          // input
	uint* cellEnd,            // input
	uint numParticles
) {
	uint index = GET_INDEX;

	if (index >= numParticles) return;

	uint originalIndexA = gridParticleIndex[index];

	float3 posA = pos[originalIndexA];
	float3 velA = vel[originalIndexA];
	uint typeA = type[originalIndexA];	

	int3 gridPosA = calcGridPos(posA);
	
	float3 force = make_float3(0.0f);
	
	for (int z = -1; z <= 1; z++) {
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				int3 neighborPos = gridPosA + make_int3(x, y, z);
				uint neighborHash = calcGridHash(neighborPos);
				uint startIndex = cellStart[neighborHash];

				if (startIndex != 0xffffffff) {
					uint endIndex = cellEnd[neighborHash];
					for (uint j = startIndex; j < endIndex; j++) {
						if (j != index) {
							uint originalIndexB = gridParticleIndex[j];
							float3 posB = pos[originalIndexB];
							float3 velB = vel[originalIndexB];
							uint typeB = type[originalIndexB];
							force += collideSpheres(posA, posB, velA, velB, typeA, typeB);
						}
					}
				}
			}
		}
	}

	accel[originalIndexA] = force / protos.mass[typeA];
}

__global__ void updateD(
	float3 *pos,
	float3 *vel,
	float3 *accel,
	uint* type,
	uint* gridParticleIndex,  // input: sorted particle indices
	uint numParticles,
	float deltaT
) {
	uint index = GET_INDEX;

	if (index >= numParticles) return;

	index = gridParticleIndex[index];

	float3 posA = pos[index];
	float3 velA = vel[index];
	float3 accelA = accel[index];
	uint typeA = type[index];

	float radiusA = protos.radius[typeA];
	float restitution = protos.restitution[typeA][typeA];

	float3 minCorner = params.minCorner;
	float3 maxCorner = params.maxCorner;

	velA += accelA * deltaT;
	velA += params.gravity * deltaT;
	velA *= params.decay;

	posA += velA * deltaT;

	if (posA.x > maxCorner.x - radiusA) {
		posA.x = maxCorner.x - radiusA;
		velA.x *= restitution;
	}

	if (posA.x < minCorner.x + radiusA) {
		posA.x = minCorner.x + radiusA;
		velA.x *= restitution;
	}

	if (posA.y > maxCorner.y - radiusA) {
		posA.y = maxCorner.y - radiusA;
		velA.y *= restitution;
	}

	if (posA.y < minCorner.y + radiusA) {
		posA.y = minCorner.y + radiusA;
		velA.y *= restitution;
	}

	if (posA.z > maxCorner.z - radiusA) {
		posA.z = maxCorner.z - radiusA;
		velA.z *= restitution;
	}

	if (posA.z < minCorner.z + radiusA) {
		posA.z = minCorner.z + radiusA;
		velA.z *= restitution;
	}
	
	pos[index] = posA;
	vel[index] = velA;
}
