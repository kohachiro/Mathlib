#include "cpuinfo.h"
#include "AMaths.h"
#include "amd3dx.h"
#include <math.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>

namespace R3
{
	class MathLib:public Singleton<MathLib>
	{
//#define am_sign_eps		am_sign_ps
//#define am_sign_ess		am_sign_ss
//#define am_abs_eps		am_abs_ps
//#define am_abs_ess		am_abs_ss
//#define am_atan_eps		am_atan_ps
//#define am_atanr2_eps		am_atanr2_ps
//#define am_asin_eps		am_asin_ps
//#define am_asin_ess		am_asin_ss
//#define am_acos_eps		am_acos_ps
//#define am_acos_ess		am_acos_ss
	public:
			friend class Singleton<MathLib>;
	const __m128  xmms1_5={ (1.5),(1.5),(1.5),(1.5) };
	const __m128  xmms_0_5={ (-0.5),(-0.5),(-0.5),(-0.5) };
	const __m128  xmms2_0={ (2.0),(2.0),(2.0),(2.0) };
	private:
		MathLib()
		{

		}
		float  __cdecl sin_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_sin_ss(_mm_set_ss(_X)) );
			return *p;		
		}
		float  __cdecl sin_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_sin_ess(_mm_set_ss(_X)) );
			return *p;	
		}
		float  __cdecl cos_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_cos_ss(_mm_set_ss(_X)) );
			return *p;		
		}
		float  __cdecl cos_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_cos_ess(_mm_set_ss(_X)) );
			return *p;		
		}
		float  __cdecl tan_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_cos_ss(_mm_set_ss(_X)) );
			return *p;		
		}
		float  __cdecl tan_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_cos_ess(_mm_set_ss(_X)) );
			return *p;		
		}
		void  __cdecl sincos_ss(float x, float* s, float* c)
		{
			__asm
			{
				movss	xmm0, x
				movaps	xmm7, xmm0
				movss	xmm1, _ps_am_inv_sign_mask
				movss	xmm2, _ps_am_sign_mask
				movss	xmm3, _ps_am_2_o_pi
				andps	xmm0, xmm1
				andps	xmm7, xmm2
				mulss	xmm0, xmm3

				pxor	xmm3, xmm3
				movd	xmm5, _epi32_1
				movss	xmm4, _ps_am_1

				cvttps2dq	xmm2, xmm0
				pand	xmm5, xmm2
				movd	xmm1, _epi32_2
				pcmpeqd	xmm5, xmm3
				movd	xmm3, _epi32_1
				cvtdq2ps	xmm6, xmm2
				paddd	xmm3, xmm2
				pand	xmm2, xmm1
				pand	xmm3, xmm1
				subss	xmm0, xmm6
				pslld	xmm2, (31 - 1)
				minss	xmm0, xmm4

				mov		eax, s     // mov eax, [esp + 4 + 16]
				mov		edx, c	   // mov edx, [esp + 4 + 16 + 4]

				subss	xmm4, xmm0
				pslld	xmm3, (31 - 1)

				movaps	xmm6, xmm4
				xorps	xmm2, xmm7
				movaps	xmm7, xmm5
				andps	xmm6, xmm7
				andnps	xmm7, xmm0
				andps	xmm0, xmm5
				andnps	xmm5, xmm4
				movss	xmm4, _ps_sincos_p3
				orps	xmm6, xmm7
				orps	xmm0, xmm5
				movss	xmm5, _ps_sincos_p2

				movaps	xmm1, xmm0
				movaps	xmm7, xmm6
				mulss	xmm0, xmm0
				mulss	xmm6, xmm6
				orps	xmm1, xmm2
				orps	xmm7, xmm3
				movaps	xmm2, xmm0
				movaps	xmm3, xmm6
				mulss	xmm0, xmm4
				mulss	xmm6, xmm4
				movss	xmm4, _ps_sincos_p1
				addss	xmm0, xmm5
				addss	xmm6, xmm5
				movss	xmm5, _ps_sincos_p0
				mulss	xmm0, xmm2
				mulss	xmm6, xmm3
				addss	xmm0, xmm4
				addss	xmm6, xmm4
				mulss	xmm0, xmm2
				mulss	xmm6, xmm3
				addss	xmm0, xmm5
				addss	xmm6, xmm5
				mulss	xmm0, xmm1
				mulss	xmm6, xmm7

				// use full stores since caller might reload with full loads
				movss	[eax], xmm0
				movss	[edx], xmm6

			}
		}
		void  __cdecl sincos_ess(float x, float* s, float* c)
		{
			float t4, t8, t12;
			__asm
			{
				movss	xmm0, x
				movss	t12, xmm0
				movss	xmm1, _ps_am_inv_sign_mask
				mov		eax, t12
				mulss	xmm0, _ps_am_2_o_pi
				andps	xmm0, xmm1
				and		eax, 0x80000000

				cvttss2si	edx, xmm0
				mov		ecx, edx
				mov		t12, esi
				mov		esi, edx
				add		edx, 0x1	
				shl		ecx, (31 - 1)
				shl		edx, (31 - 1)

				movss	xmm4, _ps_am_1
				cvtsi2ss	xmm3, esi
				mov		t8, eax
				and		esi, 0x1

				subss	xmm0, xmm3
				movss	xmm3, _sincos_inv_masks[esi * 4]
				minss	xmm0, xmm4

				subss	xmm4, xmm0

				movss	xmm6, xmm4
				andps	xmm4, xmm3
				and		ecx, 0x80000000
				movss	xmm2, xmm3
				andnps	xmm3, xmm0
				and		edx, 0x80000000
				movss	xmm7, t8
				andps	xmm0, xmm2
				mov		t8, ecx
				mov		t4, edx
				orps	xmm4, xmm3

				mov		eax, s     //mov eax, [esp + 4 + 16]
				mov		edx, c //mov edx, [esp + 4 + 16 + 4]

				andnps	xmm2, xmm6
				orps	xmm0, xmm2

				movss	xmm2, t8
				movss	xmm1, xmm0
				movss	xmm5, xmm4
				xorps	xmm7, xmm2
				movss	xmm3, _ps_sincos_p3
				mulss	xmm0, xmm0
				mulss	xmm4, xmm4
				movss	xmm2, xmm0
				movss	xmm6, xmm4
				orps	xmm1, xmm7
				movss	xmm7, _ps_sincos_p2
				mulss	xmm0, xmm3
				mulss	xmm4, xmm3
				movss	xmm3, _ps_sincos_p1
				addss	xmm0, xmm7
				addss	xmm4, xmm7
				movss	xmm7, _ps_sincos_p0
				mulss	xmm0, xmm2
				mulss	xmm4, xmm6
				addss	xmm0, xmm3
				addss	xmm4, xmm3
				movss	xmm3, t4
				mulss	xmm0, xmm2
				mulss	xmm4, xmm6
				orps	xmm5, xmm3
				mov		esi, t12
				addss	xmm0, xmm7
				addss	xmm4, xmm7
				mulss	xmm0, xmm1
				mulss	xmm4, xmm5

				// use full stores since caller might reload with full loads
				movss	[eax], xmm0
				movss	[edx], xmm4
			}
		}

		void  __cdecl sincos(float x, float* s, float* c)
		{
			__asm
			{
				fld		dword ptr x
				mov		eax, s
				mov		edx, c
				fsincos
				fstp	dword ptr [edx]
				fstp	dword ptr [eax]
			}
		}

		float  __cdecl asin_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_asin_ss(_mm_set_ss(_X)) );
			return *p;
		}

		float  __cdecl acos_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_acos_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl atan_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_atan_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl pow_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_pow_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl pow_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_pow_ess(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl exp_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_exp_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl exp_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_exp_ess(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl log_ss( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_log_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl log_ess( float _X)
		{
			float * p ;
			_mm_store_ss( p ,am_log_ess(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl sqrt_ss( float _X)
		{//2位精度
			float * p ;
			_mm_store_ss( p ,_mm_sqrt_ss(_mm_set_ss(_X)) );
			return *p;
		}

		float  __cdecl sqrt_ss4( float a)
		{//4位精度 1.5*a*x-0.5a*a*x*x*x
			__m128 x0, x1, x2, x3;
			float *p;

			x3 = _mm_set_ss (a);//a
			x0 = _mm_and_ps ( _mm_cmpneq_ss (x3, _mm_setzero_ps ()), _mm_rsqrt_ss (x3));
			x1 = _mm_mul_ss (xmms1_5, x3);// 1.5a
			x2 = _mm_mul_ss (xmms_0_5, x3);// -0.5a
			x2 = _mm_mul_ss (x2, x3);// -0.5aa

			x3 = _mm_mul_ss (x2,x0);//-0.5aax
			x3 = _mm_mul_ss (x3,x0);//-0.5aaxx
			x3 = _mm_add_ss (x3,x1);//1.5a-0.5aaxx
			x0 = _mm_mul_ss (x3,x0);//1.5ax-0.5aaxxx

			_mm_store_ss( p ,x0 );
			return *p;
		}
		float  __cdecl sqrt_ss8( float a)
		{//8位精度 1.5*a*x-0.5a*a*x*x*x
			__m128 x0, x1, x2, x3;
			float *p;
			x3 = _mm_set_ss (a);//a
			x0 = _mm_and_ps ( _mm_cmpneq_ss (x3, _mm_setzero_ps ()), _mm_rsqrt_ss (x3));
			x1 = _mm_mul_ss (xmms1_5, x3);// 1.5a
			x2 = _mm_mul_ss (xmms_0_5, x3);// -0.5a
			x2 = _mm_mul_ss (x2, x3);// -0.5aa

			x3 = _mm_mul_ss (x2,x0);//-0.5aax
			x3 = _mm_mul_ss (x3,x0);//-0.5aaxx
			x3 = _mm_add_ss (x3,x1);//1.5a-0.5aaxx
			x0 = _mm_mul_ss (x3,x0);//1.5ax-0.5aaxxx

			x3 = _mm_mul_ss (x2,x0);//-0.5aax
			x3 = _mm_mul_ss (x3,x0);//-0.5aaxx
			x3 = _mm_add_ss (x3,x1);//1.5a-0.5aaxx
			x0 = _mm_mul_ss (x3,x0);//1.5ax-0.5aaxxx

			_mm_store_ss( p ,x0 );
			return *p;
		}
		float  __cdecl rsqrt_ss( float _X)
		{//2位精度
			float * p ;
			_mm_store_ss( p ,_mm_rsqrt_ss(_mm_set_ss(_X)) );
			return *p;
		}
		float  __cdecl rsqrt_ss4( float a)
		{//4位精度 牛顿迭代1.5*x-0.5a*x*x*x
			__m128 x0, x1, x2;
			float *p;
			x2 = _mm_set_ss (a);//a
			x0 = _mm_rsqrt_ss (x2);//x
			x1 = _mm_mul_ss (xmms_0_5, x2);// -0.5a

			x2 = _mm_mul_ss (x1,x0);//-0.5ax
			x2 = _mm_mul_ss (x2,x0);//-0.5axx
			x2 = _mm_add_ss (xmms1_5,x2);//1.5-0.5axx
			x0 = _mm_mul_ss (x2,x0);//1.5x-0.5axxx

			_mm_store_ss( p ,x0 );
			return *p;
		}
		float  __cdecl rsqrt_ss8( float a)
		{//8位精度 牛顿迭代1.5*x-0.5a*x*x*x
			__m128 x0, x1, x2;
			float *p;
			x2 = _mm_set_ss (a);//a
			x0 = _mm_rsqrt_ss (x2);//x
			x1 = _mm_mul_ss (xmms_0_5, x2);// -0.5a

			x2 = _mm_mul_ss (x1,x0);//-0.5ax
			x2 = _mm_mul_ss (x2,x0);//-0.5axx
			x2 = _mm_add_ss (xmms1_5,x2);//1.5-0.5axx
			x0 = _mm_mul_ss (x2,x0);//1.5x-0.5axxx

			x2 = _mm_mul_ss (x1,x0);//-0.5ax
			x2 = _mm_mul_ss (x2,x0);//-0.5axx
			x2 = _mm_add_ss (xmms1_5,x2);//1.5-0.5axx
			x0 = _mm_mul_ss (x2,x0);//1.5x-0.5axxx

			_mm_store_ss( p ,x0 );
			return *p;
		}

	}
}

