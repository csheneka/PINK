/**
 * @file   CudaLib/generate_euclidean_distance_matrix_second_step.h
 * @date   Oct 30, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include <stdio.h>
#include <thrust/device_ptr.h>
#include <thrust/device_vector.h>

namespace pink {

/**
 * CUDA Kernel Device code
 *
 * Reduce temp. array d_tmp to final arrays d_euclideanDistanceMatrix and d_bestRotationMatrix.
 */
template <uint16_t block_size, typename T>
__global__
void second_step_kernel(thrust::device_ptr<T> euclideanDistanceMatrix,
    thrust::device_ptr<uint32_t> bestRotationMatrix, thrust::device_ptr<const T> firstStep,
    uint32_t number_of_spatial_transformations, uint32_t number_of_neurons)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i >= number_of_neurons) return;

    thrust::device_ptr<const T> pFirstStep = firstStep + i * number_of_spatial_transformations;
    thrust::device_ptr<T> pDist = euclideanDistanceMatrix + i;

    *pDist = pFirstStep[0];
    bestRotationMatrix[i] = 0;

    for (uint32_t n = 1; n < number_of_spatial_transformations; ++n) {
        if (pFirstStep[n] < *pDist) {
            *pDist = pFirstStep[n];
            bestRotationMatrix[i] = n;
        }
    }
}

/**
 * Host function that prepares data array and passes it to the CUDA kernel.
 */
template <typename T>
void generate_euclidean_distance_matrix_second_step(thrust::device_vector<T>& d_euclideanDistanceMatrix,
    thrust::device_vector<uint32_t>& d_bestRotationMatrix, thrust::device_vector<T> const& d_firstStep,
    uint32_t number_of_spatial_transformations, uint32_t number_of_neurons)
{
    const uint16_t block_size = 16;

    // Setup execution parameters
    dim3 dimBlock(block_size);
    dim3 dimGrid(ceil((T)number_of_neurons / block_size));

    // Start kernel
    second_step_kernel<block_size><<<dimGrid, dimBlock>>>(&d_euclideanDistanceMatrix[0],
        &d_bestRotationMatrix[0], &d_firstStep[0], number_of_spatial_transformations, number_of_neurons);

    cudaError_t error = cudaGetLastError();

    if (error != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch CUDA kernel cuda_generateEuclideanDistanceMatrix_secondStep (error code %s)!\n", cudaGetErrorString(error));
        exit(EXIT_FAILURE);
    }

    cudaDeviceSynchronize();
}

} // namespace pink
