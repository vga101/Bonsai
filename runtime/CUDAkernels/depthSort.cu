#include "bonsai.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include "vector_math.h"
#include <cassert>

#ifdef USE_OPENGL
  #include <GL/glew.h>
  #include <cuda_gl_interop.h>
#endif

// calculate eye-space depth for each particle
KERNEL_DECLARE(calcDepthKernel)(float4 *pos, float *depth, int *indices, float4 modelViewZ, int numParticles)
{
    int i = blockIdx.x*blockDim.x + threadIdx.x;
	if (i >= numParticles) return;

	float4 p = pos[i];
	float z = dot(make_float4(p.x, p.y, p.z, 1.0f), modelViewZ);
	
	depth[i] = z;
	indices[i] = i;
}

void thrustSort(float* keys, int* values, int count)
{
    thrust::device_ptr<float> dkeys(keys);
    thrust::device_ptr<int> dvalues(values);
    thrust::sort_by_key(dkeys, dkeys + count, dvalues);
}

extern "C"
void initCUDA()
{
  #ifdef USE_OPENGL
    cudaGLSetGLDevice(0);
  #endif
}

extern "C"
void depthSortCUDA(float4 *pos, float *depth, int *indices, float4 modelViewZ, int numParticles)
{
	int numThreads = 256;
	int numBlocks = (numParticles + numThreads - 1) / numThreads;
    calcDepthKernel<<< numBlocks, numThreads >>>(pos, depth, indices, modelViewZ, numParticles);

	thrustSort(depth, indices, numParticles);
}

// integer hash function (credit: rgba/iq)
  __device__
  int ihash(int n)
  {
      n=(n<<13)^n;
      return (n*(n*n*15731+789221)+1376312589) & 0x7fffffff;
  }

  // returns random float between 0 and 1
  __device__
  float frand(int n)
  {
	  return ihash(n) / 2147483647.0f;
  }

#if 0  /* Simon's code */
__global__
void assignColorsKernel(float4 *colors, int *ids, int numParticles, 
	float4 color2, float4 color3, float4 color4, 
	float4 starColor, float4 bulgeColor, float4 darkMatterColor, float4 dustColor,
	int m_brightFreq)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	if( tid >= numParticles ) return;

	int id =  ids[tid];

	float r = frand(id);
	//float4 color = { r, 1-r, 0.5f, 1.0f };
	//float4 color = { 1.0f, 0.0f, 0.0f, 1.0f };

	float4 color;

      if (id >= 0 && id < 40000000)     //Disk
      {
        color = ((id % m_brightFreq) != 0) ? 
        starColor :
        ((id / m_brightFreq) & 1) ? color2 : color3;
      } else if (id >= 40000000 && id < 50000000)     // Glowing stars in spiral arms
      {
        color = ((id%4) == 0) ? color4 : color3;
      }
				else if (id >= 50000000 && id < 70000000) //Dust
				{
					color = dustColor * make_float4(r, r, r, 1.0f);
				} 
				else if (id >= 70000000 && id < 100000000) // Glow massless dust particles
				{
					color = color3;  /*  adds glow in purple */
				}
      else if (id >= 100000000 && id < 200000000) //Bulge
      {
		  //colors[i] = starColor;
        color = bulgeColor;
	  } 
      else //>= 200000000, Dark matter
      {
        color = darkMatterColor;
		  //colors[i] = darkMatterColor * make_float4(r, r, r, 1.0f);
      }            
      
  
	colors[tid] = color;
}
#else  /* Ev's code :) */

class StarSampler
{
	private:
		float slope;
		float slope1;
		float slope1inv;
		float Mu_lo;
		float C;
		int   N;
		float *Masses;
		float4 *Colours;

	public:

		__device__ StarSampler(
				const float _N, 
				float  *_Masses, 
				float4 *_Colours,
				const float _slope = -2.35) : 
			slope(_slope), N(_N), Masses(_Masses), Colours(_Colours)
	{
		const float Mhi = Masses[0];
		const float Mlo = Masses[N-1];
		slope1    = slope + 1.0f;
//		assert(slope1 != 0.0f);
	  slope1inv	= 1.0f/slope1;

		Mu_lo = __powf(Mlo, slope1);
		C = (powf(Mhi, slope1) - powf(Mlo, slope1));
	}

		__device__ float sampleMass(const int id)  const
		{
			const float Mu = C*frand(id) + Mu_lo;
//			assert(Mu > 0.0);
			const float M   = __powf(Mu, slope1inv);
			return M;
		}

		__device__ float4 getColour(const float M) const
		{
			int beg = 0;
			int end = N;
			int mid = (beg + end) >> 1;
			while (end - beg > 1)
			{
				if (Masses[mid] > M)
					beg = mid;
				else 
					end = mid;
				mid = (beg + end) >> 1;
			}

			return Colours[mid];
		}
};

KERNEL_DECLARE(assignColorsKernel) (float4 *colors, int *ids, float4 *col, int numParticles, 
		float4 color2, float4 color3, float4 color4, 
		float4 starColor, float4 bulgeColor, float4 darkMatterColor, float4 dustColor,
		int m_brightFreq, float4 t_current)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	if( tid >= numParticles ) return;

	int id =  ids[tid];
	
	float r = frand(id);
	float4 color;
	
        // setup //
	if(id > 100000000 && id < 200000000){
		//every 3000th particle is lit up
		//shiny particles for all players!
		color = (((id%3000) == 0) || ((id%3000) == 1) || ((id%3000) == 2) || ((id%3000) == 3)) ? make_float4(col[tid].x+0.3,col[tid].y+0.3,col[tid].z+0.3,col[tid].w*25.0) : col[tid];
	}
	else{
	  color = col[tid];
	}
	colors[tid] = color;	

}
#endif

	extern "C"
void assignColors(float4 *colors, int *ids, float4 *col, int numParticles, //int *col,int *ids, , float4 *col //, real4 *col
		float4 color2, float4 color3, float4 color4, 
		float4 starColor, float4 bulgeColor, float4 darkMatterColor, float4 dustColor,
		int m_brightFreq, float4  t_current)
{
	int numThreads = 256;
// 	printf("number of particles %d\n",numParticles);
	
	int numBlocks = (numParticles + numThreads - 1) / numThreads;
	assignColorsKernel<<< numBlocks, numThreads >>>(colors, ids, col, numParticles, 
			color2, color3, color4, starColor, bulgeColor, darkMatterColor, dustColor, m_brightFreq, t_current);
}
