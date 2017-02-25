#include <cusp/complex.h>
#include <cufft.h>
#include <thrust\device_vector.h>
#include <thrust\device_ptr.h>
#include <cuda_runtime.h>
#include <helper_functions.h>
#include <helper_cuda.h>

using namespace cusp;
using namespace thrust;

const unsigned int TILE_DIM = 32;
const unsigned int BLOCK_ROWS = 32;

__global__ void fftshift(complex<float> * d_idata, complex<float> * d_odata)
{
	__shared__ complex<float> tile[TILE_DIM][TILE_DIM + 1];
	int x = TILE_DIM * blockIdx.x + threadIdx.x;
	int y = TILE_DIM * blockIdx.y + threadIdx.y;
	int width = gridDim.x * TILE_DIM;
	tile[threadIdx.y][threadIdx.x] = d_idata[y*width + x];
	__syncthreads();
	if (blockIdx.x < width / (TILE_DIM * 2) && blockIdx.y < width / (TILE_DIM * 2))
	{
		d_odata[(y + width / 2)*width + x + width / 2] = tile[threadIdx.y][threadIdx.x];
	}
	if (blockIdx.x < width / (TILE_DIM * 2) && blockIdx.y >= width / (TILE_DIM * 2))
	{
		d_odata[(y - width / 2)*width + x + width / 2] = tile[threadIdx.y][threadIdx.x];
	}
	if (blockIdx.x >= width / (TILE_DIM * 2) && blockIdx.y < width / (TILE_DIM * 2))
	{
		d_odata[(y + width / 2)*width + x - width / 2] = tile[threadIdx.y][threadIdx.x];
	}
	if (blockIdx.x >= width / (TILE_DIM * 2) && blockIdx.y >= width / (TILE_DIM * 2))
	{
		d_odata[(y - width / 2)*width + x - width / 2] = tile[threadIdx.y][threadIdx.x];
	}
}

__global__  void ComplexScale(complex<float>* d_idata, 
	complex<float>* d_odata,
	float scale, 
	unsigned int width,
	unsigned int height)
{
	int col = threadIdx.x + blockDim.x * blockIdx.x;
	int row = threadIdx.y + blockIdx.y * blockDim.y;
	if (col< width && row < height)
	{
		d_odata[row*width + col] = d_idata[row*width + col] * scale;
	}
}

extern "C" void pre_cuFft2D(std::complex<float> * h_kspace, 
	std::complex<float> * h_image, 
	bool fft_forward, 
	unsigned int width, 
	unsigned int height)
{
	complex<float> *d_kspace, *d_image;
	device_vector<complex<float>> d_buffer(width * height);
	size_t lpitch;
	cufftHandle plan;
	checkCudaErrors(cufftPlan2d(&plan, height, width, CUFFT_C2C));
	checkCudaErrors(cudaMallocPitch((void **)&d_kspace, &lpitch, width * sizeof(complex<float>), height));
	checkCudaErrors(cudaMallocPitch((void **)&d_image, &lpitch, width * sizeof(complex<float>), height));
	checkCudaErrors(cudaMemcpy2D((void *)d_kspace, lpitch, (void *)h_kspace, sizeof(float)*2*width, sizeof(float)*2*width, height, cudaMemcpyHostToDevice));
	dim3 dimBlock(width, 1, 1);
	dim3 dimGrid(1, height, 1);
	dim3 grid(width / TILE_DIM, height / TILE_DIM, 1);
	dim3 block(TILE_DIM, BLOCK_ROWS, 1);

	fftshift <<< grid, block >>>(d_kspace, raw_pointer_cast(d_buffer.data()));

	if (fft_forward)
	{
		checkCudaErrors(cufftExecC2C(plan, raw_pointer_cast(d_buffer.data()), d_image, CUFFT_FORWARD));
	}
	else
	{
		checkCudaErrors(cufftExecC2C(plan, raw_pointer_cast(d_buffer.data()), d_image, CUFFT_INVERSE));
	}
	fftshift << <grid, block >> >(d_image, raw_pointer_cast(d_buffer.data()));
	float scale = 1 / (float)std::sqrt(width * height);
	ComplexScale << <dimGrid, dimBlock >> >(raw_pointer_cast(d_buffer.data()), d_image, scale, width, height);
	checkCudaErrors(cudaMemcpy2D((void *)h_image, lpitch, (void *)d_image, sizeof(float)*2*width, sizeof(float)*2*width, height, cudaMemcpyDeviceToHost));
	checkCudaErrors(cufftDestroy(plan));
	checkCudaErrors(cudaFree(d_kspace));
	checkCudaErrors(cudaFree(d_image));
}