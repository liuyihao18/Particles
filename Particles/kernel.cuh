/*
 * reference: CUDA samples (particles)
 */

#pragma once

#include "param.h"

#include <cuda_runtime.h>
#include <helper_math.h>
#include <cstdio>

#define GET_INDEX __umul24(blockIdx.x, blockDim.x) + threadIdx.x

__constant__ SimParams params;
__constant__ SimProtos protos;

// 计算网格坐标
__device__ int3 calcGridPos(float3 p)
{
	int3 gridPos = {
		static_cast<int>(floorf(p.x / params.cellSize)),
		static_cast<int>(floorf(p.y / params.cellSize)),
		static_cast<int>(floorf(p.z / params.cellSize))};
	return gridPos;
}

// 计算网格坐标的哈希值
__device__ uint calcGridHash(int3 gridPos)
{
	gridPos.x = gridPos.x & (params.gridSize - 1);
	gridPos.y = gridPos.y & (params.gridSize - 1);
	gridPos.z = gridPos.z & (params.gridSize - 1);
	return __umul24(__umul24(gridPos.z, params.gridSize), params.gridSize) +
		   __umul24(gridPos.y, params.gridSize) + gridPos.x;
}

// 计算每个粒子的哈希值
__global__ void calcHashD(
	uint *gridParticleHash,	 // output
	uint *gridParticleIndex, // output
	float3 *pos,			 // input
	uint numParticles)
{
	uint index = GET_INDEX;

	if (index >= numParticles)
		return;

	// 计算网格坐标
	int3 gridPos = calcGridPos(pos[index]);
	uint hash = calcGridHash(gridPos);

	// 存储哈希值与对应的索引
	gridParticleHash[index] = hash;
	gridParticleIndex[index] = index;
}

// 计算每个粒子的哈希反向索引和邻接关系
__global__ void findCellStartD(
	uint *cellStart,		// output
	uint *cellEnd,			// output
	uint *gridParticleHash, // input
	uint numParticles)
{
	uint index = GET_INDEX;

	if (index >= numParticles)
		return;

	uint hash = gridParticleHash[index];

	// 在[cellStart[hash], cellEnd[hash])之间的索引粒子都处于同一个网格
	if (index == 0)
	{
		cellStart[hash] = index;
	}
	else if (hash != gridParticleHash[index - 1])
	{
		cellStart[hash] = index;
		cellEnd[gridParticleHash[index - 1]] = index;
	}
	if (index == numParticles - 1)
	{
		cellEnd[hash] = index + 1;
	}
}

// 计算两个粒子碰撞产生的力
__device__ float3 collideSpheres(
	float3 posA,
	float3 posB,
	float3 velA,
	float3 velB,
	uint typeA,
	uint typeB)
{
	float radiusA = protos.radius[typeA];
	float radiusB = protos.radius[typeB];
	float massA = protos.mass[typeA];
	float massB = protos.mass[typeB];

	// 计算相对位置
	float3 relPos = posB - posA;
	float dist = length(relPos);
	float collidDist = radiusA + radiusB;

	float3 force = make_float3(0.0f);

	// 如果碰撞
	if (dist < collidDist)
	{
		// 速度分解
		float3 normal = relPos / dist;
		float3 relVel = velB - velA;
		float3 normVel = dot(relVel, normal) * normal;
		float3 tanVel = relVel - normVel;

		// 弹力
		force -= params.spring * (collidDist - dist) * normal;
		// 阻尼
		float damping = protos.damping[typeA][typeB];
		float massSqrt = sqrtf((massA * massB) / (massA + massB));
		force += (params.damping * damping * massSqrt) * normVel;
		// 摩擦
		float normForce = dot(force, normal);
		force -= (params.friction * normForce) * tanVel;
	}

	return force;
}

// 计算每个粒子受到的力产生的加速度
__global__ void collideD(
	float3 *accel,			 // output
	float3 *pos,			 // input
	float3 *vel,			 // input
	uint *type,				 // input
	uint *gridParticleIndex, // input: sorted particle indices
	uint *cellStart,		 // input
	uint *cellEnd,			 // input
	uint numParticles)
{
	uint index = GET_INDEX;

	if (index >= numParticles)
		return;

	// 对粒子A
	uint originalIndexA = gridParticleIndex[index];

	float3 posA = pos[originalIndexA];
	float3 velA = vel[originalIndexA];
	uint typeA = type[originalIndexA];

	int3 gridPosA = calcGridPos(posA);

	float3 force = make_float3(0.0f);

	// 遍历其周围8个相邻网格
	for (int z = -1; z <= 1; z++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int3 neighborPos = gridPosA + make_int3(x, y, z);
				uint neighborHash = calcGridHash(neighborPos);
				uint startIndex = cellStart[neighborHash];
				// 如果网格中有其他粒子
				if (startIndex != 0xffffffff)
				{
					uint endIndex = cellEnd[neighborHash];
					for (uint j = startIndex; j < endIndex; j++)
					{
						uint originalIndexB = gridParticleIndex[j];
						if (originalIndexA != originalIndexB)
						{
							// 计算其对粒子A产生的力
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

	// 计算加速度
	accel[originalIndexA] = force / protos.mass[typeA] + params.gravity;
}

// 更新每个粒子的位置和速度
__global__ void updateD(
	float3 *pos,			 // output
	float3 *vel,			 // output
	float3 *accel,			 // input
	uint *type,				 // input
	uint *gridParticleIndex, // input
	uint numParticles,
	float deltaT)
{
	uint index = GET_INDEX;

	if (index >= numParticles)
		return;

	index = gridParticleIndex[index];

	float3 posA = pos[index];
	float3 velA = vel[index];
	float3 accelA = accel[index];
	uint typeA = type[index];

	float radiusA = protos.radius[typeA];
	float restitutionA = -protos.elasticity[typeA][typeA];

	float3 minCorner = params.minCorner;
	float3 maxCorner = params.maxCorner;

	// v = v0 + at
	velA += accelA * deltaT;
	velA *= params.decay;

	// p = p0 + vt
	posA += velA * deltaT;

	// 计算和容器的碰撞
	if (posA.x > maxCorner.x - radiusA)
	{
		posA.x = maxCorner.x - radiusA;
		velA.x *= restitutionA;
	}

	if (posA.x < minCorner.x + radiusA)
	{
		posA.x = minCorner.x + radiusA;
		velA.x *= restitutionA;
	}

	if (posA.y > maxCorner.y - radiusA)
	{
		posA.y = maxCorner.y - radiusA;
		velA.y *= restitutionA;
	}

	if (posA.y < minCorner.y + radiusA)
	{
		posA.y = minCorner.y + radiusA;
		velA.y *= restitutionA;
	}

	if (posA.z > maxCorner.z - radiusA)
	{
		posA.z = maxCorner.z - radiusA;
		velA.z *= restitutionA;
	}

	if (posA.z < minCorner.z + radiusA)
	{
		posA.z = minCorner.z + radiusA;
		velA.z *= restitutionA;
	}

	pos[index] = posA;
	vel[index] = velA;
}
