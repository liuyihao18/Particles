/*
 * reference: CUDA samples (particles)
 */

#include "kernel.cuh"

#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

extern "C"
{

    void cudaInit(int argc, char **argv)
    {
        int devID;

        // use command-line specified CUDA device, otherwise use device with highest
        // Gflops/s
        devID = findCudaDevice(argc, (const char **)argv);

        if (devID < 0)
        {
            printf("No CUDA Capable devices found, exiting...\n");
            exit(EXIT_SUCCESS);
        }
    }

    void allocateArray(void **devPtr, uint size)
    {
        checkCudaErrors(cudaMalloc(devPtr, size));
    }

    void initializeArray(void *devPtr, uint size)
    {
        checkCudaErrors(cudaMemset(devPtr, 0, size));
    }

    void freeArray(void *devPtr)
    {
        checkCudaErrors(cudaFree(devPtr));
    }

    void copyArrayFromDevice(void *host, const void *device, uint size)
    {
        checkCudaErrors(cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost));
    }

    void copyArrayToDevice(void *device, const void *host, int offset, int size)
    {
        checkCudaErrors(cudaMemcpy((char *)device + offset, host, size, cudaMemcpyHostToDevice));
    }

    void setParameters(SimParams *hostParams)
    {
        checkCudaErrors(cudaMemcpyToSymbol(params, hostParams, sizeof(SimParams)));
    }

    void setProtos(SimProtos *hostProtos)
    {
        checkCudaErrors(cudaMemcpyToSymbol(protos, hostProtos, sizeof(SimProtos)));
    }

    uint iDivUp(uint a, uint b) { return (a % b != 0) ? (a / b + 1) : (a / b); }

    void computeGridSize(uint n, uint blockSize, uint &numBlocks, uint &numThreads)
    {
        numThreads = min(blockSize, n);
        numBlocks = iDivUp(n, numThreads);
    }

    // 计算粒子的哈希值
    void calcHash(uint *gridParticleHash, uint *gridParticleIndex, float *pos, uint numParticles)
    {
        uint numThreads, numBlocks;
        computeGridSize(numParticles, 256, numBlocks, numThreads);

        calcHashD<<<numBlocks, numThreads>>>(gridParticleHash, gridParticleIndex, (float3 *)pos, numParticles);
        getLastCudaError("calcHashD execution failed.");
    }

    // 联合排序粒子的哈希值和索引
    void sortParticles(uint *gridParticleHash, uint *gridParticleIndex, uint numParticles)
    {
        thrust::sort_by_key(
            thrust::device_ptr<uint>(gridParticleHash),
            thrust::device_ptr<uint>(gridParticleHash + numParticles),
            thrust::device_ptr<uint>(gridParticleIndex));
    }

    // 计算粒子的哈希反向索引和邻接关系
    void findCellStart(uint *cellStart, uint *cellEnd, uint *gridParticleHash, uint numParticles, uint numCells)
    {
        uint numThreads, numBlocks;
        computeGridSize(numParticles, 256, numBlocks, numThreads);

        // 置空
        checkCudaErrors(cudaMemset(cellStart, 0xffffffff, numCells * sizeof(uint)));

        findCellStartD<<<numBlocks, numThreads>>>(cellStart, cellEnd, gridParticleHash, numParticles);
        getLastCudaError("findCellStartD execution failed.");
    }

    // 计算粒子碰撞产生的加速度
    void collide(float *accel, float *pos, float *vel, uint *type,
                 uint *gridParticleIndex,
                 uint *cellStart, uint *cellEnd,
                 uint numParticles)
    {
        uint numThreads, numBlocks;
        computeGridSize(numParticles, 256, numBlocks, numThreads);

        collideD<<<numBlocks, numThreads>>>((float3 *)accel, (float3 *)pos, (float3 *)vel, type, gridParticleIndex, cellStart, cellEnd, numParticles);
        getLastCudaError("collideD execution failed.");
    }

    // 更新粒子的位置
    void update(float *pos, float *vel, float *accel, uint *type,
                uint *gridParticleIndex,
                uint numParticles,
                float deltaT)
    {
        uint numThreads, numBlocks;
        computeGridSize(numParticles, 256, numBlocks, numThreads);

        updateD<<<numBlocks, numThreads>>>((float3 *)pos, (float3 *)vel, (float3 *)accel, type, gridParticleIndex, numParticles, deltaT);
        getLastCudaError("updateD execution failed.");
    }
}
