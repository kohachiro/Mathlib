/////////////////////////////////////////////////////////////////////////////
// The Software is provided "AS IS" and possibly with with faults. Intel
// disclaims any and all warranties and guarantees, express, implied or
// otherwise, arising, with respect to the software delivered hereunder,
// including but not limited to the warranty of merchantability, the warranty
// of fitness for a particular purpose, and any warranty of non-infringement
// of the intellectual property rights of any third party.
// Intel neither assumes nor authorizes any person to assume for it any other
// liability.  Customer will use the software at its own risk.  Intel will not
// be liable to customer for any direct or indirect damages incurred in using
// the software.  In no event will Intel be liable for loss of profits, loss of
// use, loss of data, business interruption, nor for punitive, incidental,
// consequential, or special damages of any kind, even if advised of
// the possibility of such damages.
// 
// Copyright (c) 1998 - 2000 Intel Corporation
//
// Third-party brands and names are the property of their respective owners
//
/////////////////////////////////////////////////////////////////////////////
// Approximate Math Library for SSE / SSE2
//  Implementation File
//  Version 2.0
//  Author Alex Klimovitski, Intel GmbH
/////////////////////////////////////////////////////////////////////////////

#include <emmintrin.h>

#include "AMaths.h"

#define	_KATMAI_STEP_B  // Uncomment to generate B0-specific code
							//  This is faster, but won't run on A0 silicon

#if defined(_DEBUG)
#error This file can only be compiled for RELEASE build with OPTMIZATION ON
#endif

/////////////////////////////////////////////////////////////////////////////
// SSE, SSE2, and MMX(TM) constants implementation

#define _PS_CONST(Name, Val) \
static const _MM_ALIGN16 float _ps_##Name[4] = { Val, Val, Val, Val }

#define _PS_EXTERN_CONST(Name, Val) \
const _MM_ALIGN16 float _ps_##Name[4] = { Val, Val, Val, Val }

#define _PS_CONST4(Name, Val0, Val1, Val2, Val3) \
static const _MM_ALIGN16 float _ps_##Name[4] = { Val0, Val1, Val2, Val3 }

#define _PS_CONST_TYPE(Name, Type, Val) \
static const _MM_ALIGN16 Type _ps_##Name[4] = { Val, Val, Val, Val }; \

#define _PS_EXTERN_CONST_TYPE(Name, Type, Val) \
const _MM_ALIGN16 Type _ps_##Name[4] = { Val, Val, Val, Val }; \

#define _EPI32_CONST(Name, Val) \
static const _MM_ALIGN16 __int32 _epi32_##Name[4] = { Val, Val, Val, Val }

#define _PI32_CONST(Name, Val) \
static const __declspec(align(8)) __int32 _pi32_##Name[4] = { Val, Val, Val, Val }

_PS_EXTERN_CONST(am_0, 0.0f);
_PS_EXTERN_CONST(am_1, 1.0f);
_PS_EXTERN_CONST(am_m1, -1.0f);
_PS_EXTERN_CONST(am_0p5, 0.5f);
_PS_EXTERN_CONST(am_1p5, 1.5f);
_PS_EXTERN_CONST(am_pi, (float)AM_PI);
_PS_EXTERN_CONST(am_pi_o_2, (float)(AM_PI / 2.0));
_PS_EXTERN_CONST(am_2_o_pi, (float)(2.0 / AM_PI));
_PS_EXTERN_CONST(am_pi_o_4, (float)(AM_PI / 4.0));
_PS_EXTERN_CONST(am_4_o_pi, (float)(4.0 / AM_PI));
_PS_EXTERN_CONST_TYPE(am_sign_mask, __int32, 0x80000000);
_PS_EXTERN_CONST_TYPE(am_inv_sign_mask, __int32, ~0x80000000);
_PS_EXTERN_CONST_TYPE(am_min_norm_pos, __int32, 0x00800000);
_PS_EXTERN_CONST_TYPE(am_mant_mask, __int32, 0x7f800000);
_PS_EXTERN_CONST_TYPE(am_inv_mant_mask, __int32, ~0x7f800000);

_EPI32_CONST(1, 1);
_EPI32_CONST(2, 2);
_EPI32_CONST(7, 7);
_EPI32_CONST(0x7f, 0x7f);
_EPI32_CONST(0xff, 0xff);

_PI32_CONST(0x80000000, 0x80000000);
#define _pi32_1		_epi32_1
#define _pi32_2		_epi32_2
#define _pi32_7		_epi32_7
_PI32_CONST(inv0x80000000, ~0x80000000);
#define _pi32_0x7f	_epi32_0x7f
#define _pi32_0xff	_epi32_0xff

/////////////////////////////////////////////////////////////////////////////
// Internal misc. functions

#ifndef _KATMAI_STEP_B
#define _mm_movehl_ps(a, b)	_mm_shuffle_ps(b, a, _MM_SHUFFLE(3,2,3,2))
#define _moveh2l_ps(a)		_mm_shuffle_ps(a, a, _MM_SHUFFLE(3,2,3,2))
#define _mm_movelh_ps(a, b)	_mm_shuffle_ps(a, b, _MM_SHUFFLE(1,0,1,0))
#define _movel2h_ps(a)		_mm_shuffle_ps(a, a, _MM_SHUFFLE(1,0,1,0))
#define	ASM_MOVE_H2L(src)    \
			__asm	shufps	src, src, _MM_SHUFFLE(3,2,3,2)

#define	ASM_MOVE_L2H(src)    \
			__asm	shufps	src, src, _MM_SHUFFLE(1,0,1,0)
#else
#define _moveh2l_ps(a)		_mm_movehl_ps(a, a)
#define _movel2h_ps(a)		_mm_movelh_ps(a, a)
#define	ASM_MOVE_H2L( src )    \
			__asm	movhlps	src, src
#define	ASM_MOVE_L2H( src )    \
			__asm	movlhps	src, src
#endif

/////////////////////////////////////////////////////////////////////////////
// sin, cos functions

_PS_CONST(sincos_p0, 0.15707963267948963959e1f);
_PS_CONST(sincos_p1, -0.64596409750621907082e0f);
_PS_CONST(sincos_p2, 0.7969262624561800806e-1f);
_PS_CONST(sincos_p3, -0.468175413106023168e-2f);

static const unsigned __int32 _sincos_masks[] = { 0x0, ~0x0 };
static const unsigned __int32 _sincos_inv_masks[] = { ~0x0, 0x0 };

/////////////////////////////////////////////////////////////////////////////
// am_sin_ps

__m128 __declspec(naked) __stdcall am_sin_ps(__m128 x)  // any x
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		mov		ecx, esp
		andps	xmm7, _ps_am_sign_mask
		mulps	xmm0, _ps_am_2_o_pi
		and		ecx, ~15

		movhlps	xmm2, xmm0
		cvttps2pi	mm0, xmm0
		movq	mm2, mm0
		cvttps2pi	mm1, xmm2
		pslld	mm2, (31 - 1)
		movq	mm3, mm1
		pslld	mm3, (31 - 1)
		pand	mm2, _pi32_0x80000000
		movq	[ecx - 32], mm2
		pand	mm3, _pi32_0x80000000
		movq	[ecx - 32 + 8], mm3

		cvtpi2ps	xmm4, mm1
		pand	mm1, _pi32_1
		ASM_MOVE_L2H(xmm4)
		pxor	mm7, mm7
		movaps	xmm5, _ps_am_1
		cvtpi2ps	xmm4, mm0
		pand	mm0, _pi32_1
		pcmpeqd	mm1, mm7
		subps	xmm0, xmm4
		movq	[ecx - 16 + 8], mm1
		minps	xmm0, xmm5
		pcmpeqd	mm0, mm7
		subps	xmm5, xmm0
		movq	[ecx - 16], mm0

		movaps	xmm6, [ecx - 16]
		andps	xmm0, xmm6
		andnps	xmm6, xmm5
		movaps	xmm3, _ps_sincos_p3
		orps	xmm0, xmm6

		movaps	xmm5, _ps_sincos_p2
		movaps	xmm1, xmm0
		mulps	xmm0, xmm0
		xorps	xmm7, [ecx - 32]
		movaps	xmm2, xmm0
		mulps	xmm0, xmm3
		movaps	xmm3, _ps_sincos_p1
		addps	xmm0, xmm5
		movaps	xmm5, _ps_sincos_p0
		mulps	xmm0, xmm2
		addps	xmm0, xmm3
		mulps	xmm0, xmm2
		orps	xmm1, xmm7
		addps	xmm0, xmm5
		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sin_eps

__m128 __declspec(naked) __stdcall am_sin_eps(__m128 x)  // any x
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		andps	xmm7, _ps_am_sign_mask
		mulps	xmm0, _ps_am_2_o_pi

		pxor	xmm3, xmm3
		movdqa	xmm5, _epi32_1
		movaps	xmm4, _ps_am_1
		cvttps2dq	xmm2, xmm0
		pand	xmm5, xmm2
		pcmpeqd	xmm5, xmm3
		cvtdq2ps	xmm6, xmm2
		pand	xmm2, _epi32_2
		pslld	xmm2, (31 - 1)

		subps	xmm0, xmm6
		minps	xmm0, xmm4
		subps	xmm4, xmm0
		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		movaps	xmm1, xmm0
		mulps	xmm0, xmm0
		xorps	xmm2, xmm7
		orps	xmm1, xmm2
		movaps	xmm7, xmm0
		mulps	xmm0, _ps_sincos_p3
		addps	xmm0, _ps_sincos_p2
		mulps	xmm0, xmm7
		addps	xmm0, _ps_sincos_p1
		mulps	xmm0, xmm7
		addps	xmm0, _ps_sincos_p0
		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sin_ss

__m128 __declspec(naked) __stdcall am_sin_ss(__m128 x)
{
	__asm
	{
		movss	[esp - 4], xmm0
		movss	xmm1, _ps_am_inv_sign_mask
		mov		eax, [esp - 4]
		mulss	xmm0, _ps_am_2_o_pi
		andps	xmm0, xmm1
		and		eax, 0x80000000

		cvttss2si	ecx, xmm0
		movss	xmm1, _ps_am_1
		mov		edx, ecx
		shl		edx, (31 - 1)
		cvtsi2ss	xmm2, ecx
		and		ecx, 0x1
		and		edx, 0x80000000

		subss	xmm0, xmm2
		movss	xmm6, _sincos_masks[ecx * 4]
		minss	xmm0, xmm1

		movss	xmm5, _ps_sincos_p3
		subss	xmm1, xmm0

		andps	xmm1, xmm6
		andnps	xmm6, xmm0
		orps	xmm1, xmm6
		movss	xmm4, _ps_sincos_p2
		movss	xmm0, xmm1

		mulss	xmm1, xmm1
		movss	xmm7, _ps_sincos_p1
		xor		eax, edx
		movss	xmm2, xmm1
		mulss	xmm1, xmm5
		movss	xmm5, _ps_sincos_p0
		mov		[esp - 4], eax
		addss	xmm1, xmm4
		mulss	xmm1, xmm2
		movss	xmm3, [esp - 4]
		addss	xmm1, xmm7
		mulss	xmm1, xmm2
		orps	xmm0, xmm3
		addss	xmm1, xmm5
		mulss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sin_ess

__m128 __declspec(naked) __stdcall am_sin_ess(__m128 x)  // any x
{
	__asm
	{
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
		cvtdq2ps	xmm6, xmm2
		pand	xmm2, xmm1
		pslld	xmm2, (31 - 1)

		subss	xmm0, xmm6
		movss	xmm3, _ps_sincos_p3
		minss	xmm0, xmm4
		subss	xmm4, xmm0
		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		movaps	xmm1, xmm0
		movss	xmm4, _ps_sincos_p2
		mulss	xmm0, xmm0
		xorps	xmm2, xmm7
		movss	xmm5, _ps_sincos_p1
		orps	xmm1, xmm2
		movaps	xmm7, xmm0
		mulss	xmm0, xmm3
		movss	xmm6, _ps_sincos_p0
		addss	xmm0, xmm4
		mulss	xmm0, xmm7
		addss	xmm0, xmm5
		mulss	xmm0, xmm7
		addss	xmm0, xmm6
		mulss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_cos_ps

__m128 __declspec(naked) __stdcall am_cos_ps(__m128 x)  // any x
{
	__asm
	{
		andps	xmm0, _ps_am_inv_sign_mask
		mov		ecx, esp
		addps	xmm0, _ps_am_pi_o_2
		mulps	xmm0, _ps_am_2_o_pi
		and		ecx, ~15

		movhlps	xmm2, xmm0
		cvttps2pi	mm0, xmm0
		movq	mm2, mm0
		cvttps2pi	mm1, xmm2
		pslld	mm2, (31 - 1)
		movq	mm3, mm1
		pslld	mm3, (31 - 1)
		pand	mm2, _pi32_0x80000000
		movq	[ecx - 32], mm2
		pand	mm3, _pi32_0x80000000
		movq	[ecx - 32 + 8], mm3

		cvtpi2ps	xmm4, mm1
		pand	mm1, _pi32_1
		ASM_MOVE_L2H(xmm4)
		pxor	mm7, mm7
		movaps	xmm5, _ps_am_1
		cvtpi2ps	xmm4, mm0
		pand	mm0, _pi32_1
		pcmpeqd	mm1, mm7
		subps	xmm0, xmm4
		movq	[ecx - 16 + 8], mm1
		minps	xmm0, xmm5
		pcmpeqd	mm0, mm7
		subps	xmm5, xmm0
		movq	[ecx - 16], mm0

		movaps	xmm6, [ecx - 16]
		andps	xmm0, xmm6
		andnps	xmm6, xmm5
		movaps	xmm3, _ps_sincos_p3
		orps	xmm0, xmm6

		movaps	xmm5, _ps_sincos_p2
		movaps	xmm1, xmm0
		mulps	xmm0, xmm0
		movaps	xmm7, [ecx - 32]
		movaps	xmm2, xmm0
		mulps	xmm0, xmm3
		movaps	xmm3, _ps_sincos_p1
		addps	xmm0, xmm5
		movaps	xmm5, _ps_sincos_p0
		mulps	xmm0, xmm2
		addps	xmm0, xmm3
		mulps	xmm0, xmm2
		orps	xmm1, xmm7
		addps	xmm0, xmm5
		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_cos_eps

__m128 __declspec(naked) __stdcall am_cos_eps(__m128 x)  // any x
{
	__asm
	{
		andps	xmm0, _ps_am_inv_sign_mask
		addps	xmm0, _ps_am_pi_o_2
		mulps	xmm0, _ps_am_2_o_pi

		pxor	xmm3, xmm3
		movdqa	xmm5, _epi32_1
		movaps	xmm4, _ps_am_1
		cvttps2dq	xmm2, xmm0
		pand	xmm5, xmm2
		pcmpeqd	xmm5, xmm3
		cvtdq2ps	xmm6, xmm2
		pand	xmm2, _epi32_2
		pslld	xmm2, (31 - 1)

		subps	xmm0, xmm6
		minps	xmm0, xmm4
		subps	xmm4, xmm0
		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		movaps	xmm1, xmm0
		mulps	xmm0, xmm0
		orps	xmm1, xmm2
		movaps	xmm7, xmm0
		mulps	xmm0, _ps_sincos_p3
		addps	xmm0, _ps_sincos_p2
		mulps	xmm0, xmm7
		addps	xmm0, _ps_sincos_p1
		mulps	xmm0, xmm7
		addps	xmm0, _ps_sincos_p0
		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_cos_ss

__m128 __declspec(naked) __stdcall am_cos_ss(__m128 x)
{
	__asm
	{
		movss	xmm1, _ps_am_inv_sign_mask
		andps	xmm0, xmm1
		addss	xmm0, _ps_am_pi_o_2
		mulss	xmm0, _ps_am_2_o_pi

		cvttss2si	ecx, xmm0
		movss	xmm5, _ps_am_1
		mov		edx, ecx
		shl		edx, (31 - 1)
		cvtsi2ss	xmm1, ecx
		and		edx, 0x80000000
		and		ecx, 0x1

		subss	xmm0, xmm1
		movss	xmm6, _sincos_masks[ecx * 4]
		minss	xmm0, xmm5

		movss	xmm1, _ps_sincos_p3
		subss	xmm5, xmm0

		andps	xmm5, xmm6
		movss	xmm7, _ps_sincos_p2
		andnps	xmm6, xmm0
		mov		[esp - 4], edx
		orps	xmm5, xmm6
		movss	xmm0, xmm5

		mulss	xmm5, xmm5
		movss	xmm4, _ps_sincos_p1
		movss	xmm2, xmm5
		mulss	xmm5, xmm1
		movss	xmm1, _ps_sincos_p0
		addss	xmm5, xmm7
		mulss	xmm5, xmm2
		movss	xmm3, [esp - 4]
		addss	xmm5, xmm4
		mulss	xmm5, xmm2
		orps	xmm0, xmm3
		addss	xmm5, xmm1
		mulss	xmm0, xmm5

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_cos_ess

__m128 __declspec(naked) __stdcall am_cos_ess(__m128 x)  // any x
{
	__asm
	{
		movss	xmm1, _ps_am_inv_sign_mask
		movss	xmm2, _ps_am_pi_o_2
		movss	xmm3, _ps_am_2_o_pi
		andps	xmm0, xmm1
		addss	xmm0, xmm2
		mulss	xmm0, xmm3

		pxor	xmm3, xmm3
		movd	xmm5, _epi32_1
		movss	xmm4, _ps_am_1
		cvttps2dq	xmm2, xmm0
		pand	xmm5, xmm2
		movd	xmm1, _epi32_2
		pcmpeqd	xmm5, xmm3
		cvtdq2ps	xmm6, xmm2
		pand	xmm2, xmm1
		pslld	xmm2, (31 - 1)

		subss	xmm0, xmm6
		movss	xmm3, _ps_sincos_p3
		minss	xmm0, xmm4
		subss	xmm4, xmm0
		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		movaps	xmm1, xmm0
		movss	xmm4, _ps_sincos_p2
		mulss	xmm0, xmm0
		movss	xmm5, _ps_sincos_p1
		orps	xmm1, xmm2
		movaps	xmm7, xmm0
		mulss	xmm0, xmm3
		movss	xmm6, _ps_sincos_p0
		addss	xmm0, xmm4
		mulss	xmm0, xmm7
		addss	xmm0, xmm5
		mulss	xmm0, xmm7
		addss	xmm0, xmm6
		mulss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sincos_ps

void __declspec(naked) __stdcall am_sincos_ps(__m128 x, __m128* s, __m128* c)
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		mov		ecx, esp
		andps	xmm7, _ps_am_sign_mask
		mulps	xmm0, _ps_am_2_o_pi
		and		ecx, ~15

		movhlps	xmm2, xmm0
		movq	mm4, _pi32_1
		cvttps2pi	mm0, xmm0
		movq	mm5, _pi32_1
		movq	mm2, mm0
		paddd	mm4, mm0
		movq	mm7, _pi32_0x80000000
		cvttps2pi	mm1, xmm2
		pslld	mm2, (31 - 1)
		pslld	mm4, (31 - 1)
		movq	mm3, mm1
		paddd	mm5, mm1
		pslld	mm3, (31 - 1)
		pand	mm2, mm7
		pslld	mm5, (31 - 1)
		pand	mm4, mm7
		movq	[ecx - 32], mm2
		pand	mm3, mm7
		pand	mm5, mm7
		movq	[ecx - 32 + 8], mm3
		movq	[ecx - 48], mm4
		movq	[ecx - 48 + 8], mm5

		cvtpi2ps	xmm3, mm1
		pand	mm1, _pi32_1
		movaps	xmm4, _ps_am_1
		ASM_MOVE_L2H(xmm3)
		pxor	mm7, mm7
		cvtpi2ps	xmm3, mm0
		pand	mm0, _pi32_1
		pcmpeqd	mm1, mm7
		subps	xmm0, xmm3
		movq	[ecx - 16 + 8], mm1
		minps	xmm0, xmm4
		pcmpeqd	mm0, mm7
		subps	xmm4, xmm0
		movq	[ecx - 16], mm0

		movaps	xmm3, [ecx - 16]
		movaps	xmm6, xmm4
		andps	xmm4, xmm3
		movaps	xmm2, xmm3
		andnps	xmm3, xmm0
		andps	xmm0, xmm2
		orps	xmm4, xmm3

		mov		eax, [esp + 4 + 16]
		mov		edx, [esp + 4 + 16 + 4]

		andnps	xmm2, xmm6
		orps	xmm0, xmm2

		movaps	xmm3, _ps_sincos_p3
		movaps	xmm1, xmm0
		movaps	xmm5, xmm4
		xorps	xmm7, [ecx - 32]
		mulps	xmm0, xmm0
		mulps	xmm4, xmm4
		movaps	xmm2, xmm0
		movaps	xmm6, xmm4
		orps	xmm1, xmm7
		movaps	xmm7, _ps_sincos_p2
		mulps	xmm0, xmm3
		mulps	xmm4, xmm3
		movaps	xmm3, _ps_sincos_p1
		addps	xmm0, xmm7
		addps	xmm4, xmm7
		movaps	xmm7, _ps_sincos_p0
		mulps	xmm0, xmm2
		mulps	xmm4, xmm6
		orps	xmm5, [ecx - 48]
		addps	xmm0, xmm3
		addps	xmm4, xmm3
		mulps	xmm0, xmm2
		mulps	xmm4, xmm6
		addps	xmm0, xmm7
		addps	xmm4, xmm7
		mulps	xmm0, xmm1
		mulps	xmm4, xmm5

		movaps	[eax], xmm0
		movaps	[edx], xmm4

		ret		16 + 4 + 4 + 8
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sincos_eps
void __declspec(naked) __stdcall am_sincos_eps(__m128 x, __m128* s, __m128* c)  // any x
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		andps	xmm7, _ps_am_sign_mask
		mulps	xmm0, _ps_am_2_o_pi

		pxor	xmm3, xmm3
		movdqa	xmm5, _epi32_1
		movaps	xmm4, _ps_am_1

		cvttps2dq	xmm2, xmm0
		pand	xmm5, xmm2
		pcmpeqd	xmm5, xmm3
		movdqa	xmm3, _epi32_1
		movdqa	xmm1, _epi32_2
		cvtdq2ps	xmm6, xmm2
		paddd	xmm3, xmm2
		pand	xmm2, xmm1
		pand	xmm3, xmm1
		subps	xmm0, xmm6
		pslld	xmm2, (31 - 1)
		minps	xmm0, xmm4
		mov		eax, [esp + 4 + 16]
		mov		edx, [esp + 4 + 16 + 4]
		subps	xmm4, xmm0
		pslld	xmm3, (31 - 1)

		movaps	xmm6, xmm4
		xorps	xmm2, xmm7
		movaps	xmm7, xmm5
		andps	xmm6, xmm7
		andnps	xmm7, xmm0
		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		movaps	xmm4, _ps_sincos_p3
		orps	xmm6, xmm7
		orps	xmm0, xmm5
		movaps	xmm5, _ps_sincos_p2

		movaps	xmm1, xmm0
		movaps	xmm7, xmm6
		mulps	xmm0, xmm0
		mulps	xmm6, xmm6
		orps	xmm1, xmm2
		orps	xmm7, xmm3
		movaps	xmm2, xmm0
		movaps	xmm3, xmm6
		mulps	xmm0, xmm4
		mulps	xmm6, xmm4
		movaps	xmm4, _ps_sincos_p1
		addps	xmm0, xmm5
		addps	xmm6, xmm5
		movaps	xmm5, _ps_sincos_p0
		mulps	xmm0, xmm2
		mulps	xmm6, xmm3
		addps	xmm0, xmm4
		addps	xmm6, xmm4
		mulps	xmm0, xmm2
		mulps	xmm6, xmm3
		addps	xmm0, xmm5
		addps	xmm6, xmm5
		mulps	xmm0, xmm1
		mulps	xmm6, xmm7

		movaps	[eax], xmm0
		movaps	[edx], xmm6

		ret		16 + 4 + 4 + 8
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sincos_ss

void __declspec(naked) __stdcall am_sincos_ss(__m128 x, __m128* s, __m128* c)
{
	__asm
	{
		movss	[esp - 12], xmm0
		movss	xmm1, _ps_am_inv_sign_mask
		mov		eax, [esp - 12]
		mulss	xmm0, _ps_am_2_o_pi
		andps	xmm0, xmm1
		and		eax, 0x80000000

		cvttss2si	edx, xmm0
		mov		ecx, edx
		mov		[esp - 12], esi
		mov		esi, edx
		add		edx, 0x1	
		shl		ecx, (31 - 1)
		shl		edx, (31 - 1)

		movss	xmm4, _ps_am_1
		cvtsi2ss	xmm3, esi
		mov		[esp - 8], eax
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
		movss	xmm7, [esp - 8]
		andps	xmm0, xmm2
		mov		[esp - 8], ecx
		mov		[esp - 4], edx
		orps	xmm4, xmm3

		mov		eax, [esp + 4 + 16]
		mov		edx, [esp + 4 + 16 + 4]

		andnps	xmm2, xmm6
		orps	xmm0, xmm2

		movss	xmm2, [esp - 8]
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
		movss	xmm3, [esp - 4]
		mulss	xmm0, xmm2
		mulss	xmm4, xmm6
		orps	xmm5, xmm3
		mov		esi, [esp - 12]
		addss	xmm0, xmm7
		addss	xmm4, xmm7
		mulss	xmm0, xmm1
		mulss	xmm4, xmm5

		// use full stores since caller might reload with full loads
		movaps	[eax], xmm0
		movaps	[edx], xmm4

		ret		16 + 4 + 4 + 8
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_sincos_ess

void __declspec(naked) __stdcall am_sincos_ess(__m128 x, __m128* s, __m128* c)  // any x
{
	__asm
	{
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
		mov		eax, [esp + 4 + 16]
		mov		edx, [esp + 4 + 16 + 4]
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
		movaps	[eax], xmm0
		movaps	[edx], xmm6

		ret		16 + 4 + 4 + 8
	}
}

/////////////////////////////////////////////////////////////////////////////
// tan functions

_PS_CONST(tan_p0, -1.79565251976484877988e7);
_PS_CONST(tan_p1, 1.15351664838587416140e6);
_PS_CONST(tan_p2, -1.30936939181383777646e4);
	
_PS_CONST(tan_q0, -5.38695755929454629881e7);
_PS_CONST(tan_q1, 2.50083801823357915839e7);
_PS_CONST(tan_q2, -1.32089234440210967447e6);
_PS_CONST(tan_q3, 1.36812963470692954678e4);

_PS_CONST(tan_poleval, 3.68935e19);

/////////////////////////////////////////////////////////////////////////////
// am_tan_ps

__m128 __declspec(naked) __stdcall am_tan_ps(__m128 x)  // any x
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		mov		ecx, esp
		andps	xmm7, _ps_am_sign_mask
		movaps	xmm1, xmm0
		mulps	xmm0, _ps_am_4_o_pi

		movq	mm4, _pi32_1
		movq	mm2, _pi32_7
		cvttps2pi	mm0, xmm0
		ASM_MOVE_H2L(xmm0)
		movq	mm5, mm4
		movq	mm3, mm2
		cvttps2pi	mm1, xmm0

		movaps	xmm5, _ps_am_1
		pand	mm4, mm0
		pand	mm5, mm1
		movq	mm7, _pi32_2
		pand	mm2, mm0
		pand	mm3, mm1
		paddd	mm0, mm4
		paddd	mm1, mm5

		cvtpi2ps	xmm0, mm1
		ASM_MOVE_L2H(xmm0)
		cvtpi2ps	xmm0, mm0

		paddd	mm2, mm4
		paddd	mm3, mm5

		mulps	xmm0, _ps_am_pi_o_4
		pxor	mm6, mm6
		subps	xmm1, xmm0
		pand	mm2, mm7
		pand	mm3, mm7
		minps	xmm1, xmm5

		movaps	xmm2, _ps_tan_p2
		pcmpeqd	mm2, mm6
		pcmpeqd	mm3, mm6
		movaps	xmm3, _ps_tan_q3
		and		ecx, ~15

		movaps	xmm0, xmm1
		mulps	xmm1, xmm1
		xorps	xmm6, xmm6
		movq	[ecx - 16], mm2
		movq	[ecx - 16 + 8], mm3

		mulps	xmm2, xmm1
		addps	xmm3, xmm1
		addps	xmm2, _ps_tan_p1
		mulps	xmm3, xmm1
		mulps	xmm2, xmm1
		addps	xmm3, _ps_tan_q2
		addps	xmm2, _ps_tan_p0
		mulps	xmm3, xmm1
		mulps	xmm2, xmm1
		addps	xmm3, _ps_tan_q1
		xorps	xmm0, xmm7
		mulps	xmm3, xmm1
		mulps	xmm2, xmm0
		addps	xmm3, _ps_tan_q0

		cmpneqps	xmm6, xmm1
		rcpps	xmm4, xmm3
		mulps	xmm3, xmm4
		movaps	xmm5, [ecx - 16]
		mulps	xmm3, xmm4
		addps	xmm4, xmm4
		orps	xmm6, xmm5
		subps	xmm4, xmm3

		mulps	xmm2, xmm4
		movaps	xmm1, _ps_am_sign_mask
		movmskps	eax, xmm6
		addps	xmm2, xmm0

		rcpps	xmm4, xmm2
		movaps	xmm0, xmm2
		mulps	xmm2, xmm4
		cmp		eax, 0xf
		mulps	xmm2, xmm4
		addps	xmm4, xmm4
		subps	xmm4, xmm2
		jne		l_pole

		xorps	xmm4, xmm1

		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		ret		16

l_pole:
		movaps	xmm7, xmm1
		movaps	xmm3, _ps_tan_poleval
		andps	xmm1, xmm0
		orps	xmm3, xmm1
		andps	xmm4, xmm6
		andnps	xmm6, xmm3
		orps	xmm4, xmm6

		xorps	xmm4, xmm7

		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_tan_eps

__m128 __declspec(naked) __stdcall am_tan_eps(__m128 x)  // any x
{
	__asm
	{
		movaps	xmm7, xmm0
		andps	xmm0, _ps_am_inv_sign_mask
		andps	xmm7, _ps_am_sign_mask
		movaps	xmm1, xmm0
		mulps	xmm0, _ps_am_4_o_pi

		cvttps2dq	xmm0, xmm0
		movdqa	xmm4, _epi32_1
		movdqa	xmm5, _epi32_7

		pand	xmm4, xmm0
		pand	xmm5, xmm0
		movaps	xmm3, _ps_am_1
		paddd	xmm0, xmm4
		paddd	xmm5, xmm4

		cvtdq2ps	xmm0, xmm0

		mulps	xmm0, _ps_am_pi_o_4
		xorps	xmm6, xmm6
		subps	xmm1, xmm0
		movaps	xmm2, _ps_tan_p2
		minps	xmm1, xmm3
		movaps	xmm3, _ps_tan_q3
		movaps	xmm0, xmm1
		mulps	xmm1, xmm1

		mulps	xmm2, xmm1
		addps	xmm3, xmm1
		addps	xmm2, _ps_tan_p1
		mulps	xmm3, xmm1
		mulps	xmm2, xmm1
		addps	xmm3, _ps_tan_q2
		addps	xmm2, _ps_tan_p0
		mulps	xmm3, xmm1
		mulps	xmm2, xmm1
		addps	xmm3, _ps_tan_q1
		xorps	xmm0, xmm7
		mulps	xmm3, xmm1
		pand	xmm5, _epi32_2
		addps	xmm3, _ps_tan_q0
		mulps	xmm2, xmm0

		cmpneqps	xmm6, xmm1
		rcpps	xmm4, xmm3
		pxor	xmm7, xmm7
		mulps	xmm3, xmm4
		pcmpeqd	xmm5, xmm7
		mulps	xmm3, xmm4
		addps	xmm4, xmm4
		orps	xmm6, xmm5
		subps	xmm4, xmm3

		mulps	xmm2, xmm4
		movaps	xmm1, _ps_am_sign_mask
		movmskps	eax, xmm6
		addps	xmm2, xmm0

		rcpps	xmm4, xmm2
		cmp		eax, 0xf
		movaps	xmm0, xmm2
		mulps	xmm2, xmm4
		mulps	xmm2, xmm4
		addps	xmm4, xmm4
		subps	xmm4, xmm2
		jne		l_pole

		xorps	xmm4, xmm1

		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		ret		16

l_pole:
		movaps	xmm7, xmm1
		movaps	xmm3, _ps_tan_poleval
		andps	xmm1, xmm0
		orps	xmm3, xmm1
		andps	xmm4, xmm6
		andnps	xmm6, xmm3
		orps	xmm4, xmm6

		xorps	xmm4, xmm7

		andps	xmm0, xmm5
		andnps	xmm5, xmm4
		orps	xmm0, xmm5

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_tan_ss

__m128 __declspec(naked) __stdcall am_tan_ss(__m128 x)  // any x
{
	__asm
	{
		movss	[esp - 8], xmm0
		mov		[esp - 4], esi
		movss	xmm1, _ps_am_inv_sign_mask
		mov		eax, [esp - 8]
		andps	xmm0, xmm1
		and		eax, 0x80000000
		movss	xmm1, xmm0
		mulss	xmm0, _ps_am_4_o_pi

		cvttss2si	edx, xmm0
		
		movss	xmm5, _ps_am_1

		mov		ecx, 0x1
		mov		esi, 0x7

		and		ecx, edx
		and		esi, edx
		add		edx, ecx
		add		esi, ecx
		mov		[esp - 8], eax

		cvtsi2ss	xmm0, edx
		xorps	xmm6, xmm6

		mulss	xmm0, _ps_am_pi_o_4
		test	esi, 0x2
		subss	xmm1, xmm0
		movss	xmm2, _ps_tan_p2
		minss	xmm1, xmm5
		movss	xmm3, _ps_tan_q3
		movss	xmm0, xmm1
		mulss	xmm1, xmm1
		movss	xmm7, [esp - 8]

		mulss	xmm2, xmm1
		addss	xmm3, xmm1
		addss	xmm2, _ps_tan_p1
		mulss	xmm3, xmm1
		mulss	xmm2, xmm1
		addss	xmm3, _ps_tan_q2
		addss	xmm2, _ps_tan_p0
		mulss	xmm3, xmm1
		mulss	xmm2, xmm1
		addss	xmm3, _ps_tan_q1
		xorps	xmm0, xmm7
		mulss	xmm3, xmm1
		mulss	xmm2, xmm0
		addss	xmm3, _ps_tan_q0

		rcpss	xmm4, xmm3
		mulss	xmm3, xmm4
		mov		esi, [esp - 4]
		mulss	xmm3, xmm4
		addss	xmm4, xmm4
		subss	xmm4, xmm3

		mulss	xmm2, xmm4
		jz		l_cont
		addss	xmm2, xmm0
		comiss	xmm6, xmm1

		rcpss	xmm4, xmm2
		movss	xmm0, _ps_am_sign_mask
		jz		l_pole
		mulss	xmm2, xmm4
		mulss	xmm2, xmm4
		addss	xmm4, xmm4
		subss	xmm4, xmm2
		xorps	xmm0, xmm4

		ret		16

l_pole:
		movss	xmm1, _ps_tan_poleval
		movss	xmm3, xmm0
		andps	xmm0, xmm2
		orps	xmm0, xmm1

		xorps	xmm0, xmm3

		ret		16

l_cont:
		addss	xmm0, xmm2
		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_tan_ess

__m128 __declspec(naked) __stdcall am_tan_ess(__m128 x)  // any x
{
	__asm
	{
		movss	xmm1, _ps_am_inv_sign_mask
		movd	eax, xmm0
		andps	xmm0, xmm1
		movaps	xmm1, xmm0
		mulss	xmm0, _ps_am_4_o_pi

		cvttss2si	edx, xmm0
		and		eax, 0x80000000

		mov		ecx, 0x1
		movd	xmm7, eax
		mov		eax, 0x7

		movss	xmm5, _ps_am_1

		and		ecx, edx
		and		eax, edx
		add		edx, ecx
		add		eax, ecx

		cvtsi2ss	xmm0, edx
		xorps	xmm6, xmm6

		mulss	xmm0, _ps_am_pi_o_4
		subss	xmm1, xmm0
		movss	xmm2, _ps_tan_p2
		minss	xmm1, xmm5
		movss	xmm3, _ps_tan_q3
		movaps	xmm0, xmm1
		mulss	xmm1, xmm1

		mulss	xmm2, xmm1
		addss	xmm3, xmm1
		addss	xmm2, _ps_tan_p1
		mulss	xmm3, xmm1
		mulss	xmm2, xmm1
		addss	xmm3, _ps_tan_q2
		addss	xmm2, _ps_tan_p0
		mulss	xmm3, xmm1
		mulss	xmm2, xmm1
		addss	xmm3, _ps_tan_q1
		xorps	xmm0, xmm7
		mulss	xmm3, xmm1
		mulss	xmm2, xmm0
		addss	xmm3, _ps_tan_q0

		rcpss	xmm4, xmm3
		mulss	xmm3, xmm4
		mulss	xmm3, xmm4
		addss	xmm4, xmm4
		test	eax, 0x2
		subss	xmm4, xmm3

		mulss	xmm2, xmm4
		jz		l_cont
		addss	xmm2, xmm0
		comiss	xmm6, xmm1

		rcpss	xmm4, xmm2
		movss	xmm0, _ps_am_sign_mask
		jz		l_pole
		mulss	xmm2, xmm4
		mulss	xmm2, xmm4
		addss	xmm4, xmm4
		subss	xmm4, xmm2
		xorps	xmm0, xmm4

		ret		16

l_pole:
		movss	xmm1, _ps_tan_poleval
		movaps	xmm3, xmm0
		andps	xmm0, xmm2
		orps	xmm0, xmm1

		xorps	xmm0, xmm3

		ret		16

l_cont:
		addss	xmm0, xmm2
		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// atan, atan2 functions

_PS_CONST(atan_t0, -0.91646118527267623468e-1f);
_PS_CONST(atan_t1, -0.13956945682312098640e1f);
_PS_CONST(atan_t2, -0.94393926122725531747e2f);
_PS_CONST(atan_t3,  0.12888383034157279340e2f);

_PS_CONST(atan_s0,  0.12797564625607904396e1f);
_PS_CONST(atan_s1,  0.21972168858277355914e1f);
_PS_CONST(atan_s2,  0.68193064729268275701e1f);
_PS_CONST(atan_s3,  0.28205206687035841409e2f);

/////////////////////////////////////////////////////////////////////////////
// am_atan_ps

__m128 __declspec(naked) __stdcall am_atan_ps(__m128 x)  // any x
{
	__asm
	{
		movaps	xmm5, _ps_am_1
		movaps	xmm6, _ps_am_m1
		rcpps	xmm4, xmm0

		cmpltps	xmm5, xmm0
		cmpnleps	xmm6, xmm0
		movaps	xmm1, _ps_atan_s0
		orps	xmm5, xmm6

		andps	xmm4, xmm5
		movaps	xmm2, _ps_atan_t0
		movaps	xmm7, xmm5
		andnps	xmm5, xmm0
		movaps	xmm3, _ps_atan_s1
		orps	xmm4, xmm5
		movaps	xmm0, xmm4

		movaps	xmm6, _ps_atan_t1
		mulps	xmm4, xmm4

		addps	xmm1, xmm4
		movaps	xmm5, _ps_atan_s2
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm2
		movaps	xmm2, _ps_atan_t2
		addps	xmm3, xmm4
		addps	xmm1, xmm3

		movaps	xmm3, _ps_atan_s3
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm6
		movaps	xmm6, _ps_atan_t3
		addps	xmm5, xmm4
		addps	xmm1, xmm5

		movaps	xmm5, _ps_am_sign_mask
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm2
		addps	xmm3, xmm4
		movaps	xmm4, _ps_am_pi_o_2
		mulps	xmm6, xmm0
		addps	xmm1, xmm3

		andps	xmm0, xmm5
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm6

		orps	xmm0, xmm4
		subps	xmm0, xmm1

		andps	xmm0, xmm7
		andnps	xmm7, xmm1
		orps	xmm0, xmm7

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_atan_ss

__m128 __declspec(naked) __stdcall am_atan_ss(__m128 x)  // any x
{
	__asm
	{
		movss	xmm1, _ps_am_sign_mask
		rcpss	xmm4, xmm0
		orps	xmm1, xmm0
		movss	xmm6, xmm4
		comiss	xmm1, _ps_am_m1
		movss	xmm3, _ps_atan_t0
		jc		l_big  // 'c' is 'lt' for comiss

		movss	xmm2, xmm0
		mulss	xmm2, xmm2

		movss	xmm1, _ps_atan_s0
		addss	xmm1, xmm2

		movss	xmm7, _ps_atan_s1
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t1
		addss	xmm7, xmm2
		addss	xmm1, xmm7
			
		movss	xmm7, _ps_atan_s2
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t2
		addss	xmm7, xmm2
		addss	xmm1, xmm7

		movss	xmm7, _ps_atan_s3
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t3
		addss	xmm7, xmm2
		mulss	xmm0, xmm3
		addss	xmm1, xmm7

		rcpss	xmm1, xmm1
		mulss	xmm0, xmm1

		ret		16

l_big:
		mulss	xmm6, xmm6

		movss	xmm5, _ps_atan_s0
		addss	xmm5, xmm6

		movss	xmm7, _ps_atan_s1
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t1
		addss	xmm7, xmm6
		addss	xmm5, xmm7

		movss	xmm7, _ps_atan_s2
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t2
		addss	xmm7, xmm6
		addss	xmm5, xmm7

		movss	xmm7, _ps_atan_s3
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t3
		addss	xmm7, xmm6
		movss	xmm2, _ps_am_sign_mask
		mulss	xmm4, xmm3
		addss	xmm5, xmm7

		movss	xmm7, _ps_am_pi_o_2
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm4

		andps	xmm0, xmm2
		orps	xmm0, xmm7
		subss	xmm0, xmm5

		ret		16
	}
}            

/////////////////////////////////////////////////////////////////////////////
// am_atan_ess

__m128 __declspec(naked) __stdcall am_atan_ess(__m128 x)  // any x
{
	__asm
	{
		movss	xmm1, _ps_am_sign_mask
		rcpss	xmm4, xmm0
		orps	xmm1, xmm0
		movss	xmm6, xmm4
		comiss	xmm1, _ps_am_m1
		movss	xmm3, _ps_atan_t0
		jnc		l_small  // 'c' is 'lt' for comiss

//l_big:
		mulss	xmm6, xmm6

		movss	xmm5, _ps_atan_s0
		addss	xmm5, xmm6

		movss	xmm7, _ps_atan_s1
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t1
		addss	xmm7, xmm6
		addss	xmm5, xmm7

		movss	xmm7, _ps_atan_s2
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t2
		addss	xmm7, xmm6
		addss	xmm5, xmm7

		movss	xmm7, _ps_atan_s3
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm3
		movss	xmm3, _ps_atan_t3
		addss	xmm7, xmm6
		movss	xmm2, _ps_am_sign_mask
		mulss	xmm4, xmm3
		addss	xmm5, xmm7

		movss	xmm7, _ps_am_pi_o_2
		rcpss	xmm5, xmm5
		mulss	xmm5, xmm4

		andps	xmm0, xmm2
		orps	xmm0, xmm7
		subss	xmm0, xmm5

		ret		16

l_small:
		movaps	xmm2, xmm0
		mulss	xmm2, xmm2

		movss	xmm1, _ps_atan_s0
		addss	xmm1, xmm2

		movss	xmm7, _ps_atan_s1
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t1
		addss	xmm7, xmm2
		addss	xmm1, xmm7
			
		movss	xmm7, _ps_atan_s2
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t2
		addss	xmm7, xmm2
		addss	xmm1, xmm7

		movss	xmm7, _ps_atan_s3
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm3
		movss	xmm3, _ps_atan_t3
		addss	xmm7, xmm2
		mulss	xmm0, xmm3
		addss	xmm1, xmm7

		rcpss	xmm1, xmm1
		mulss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_atanr2_ps

__m128 __declspec(naked) __stdcall am_atanr2_ps(__m128 y, __m128 rx)
{
	__asm
	{
		movaps	xmm2, _ps_am_sign_mask
		xorps	xmm3, xmm3
		mov		ecx, esp
		movaps	xmm5, _ps_am_1
		andps	xmm2, xmm0
		mulps	xmm0, xmm1
		and		ecx, ~15
		orps	xmm2, _ps_am_pi
		cmpleps	xmm3, xmm1

		movaps	xmm6, _ps_am_m1
		rcpps	xmm4, xmm0

		cmpltps	xmm5, xmm0
		cmpnleps	xmm6, xmm0
		movaps	xmm1, _ps_atan_s0
		orps	xmm5, xmm6

		movaps	[ecx - 16], xmm2
		movaps	[ecx - 32], xmm3

		andps	xmm4, xmm5
		movaps	xmm2, _ps_atan_t0
		movaps	xmm7, xmm5
		andnps	xmm5, xmm0
		movaps	xmm3, _ps_atan_s1
		orps	xmm4, xmm5
		movaps	xmm0, xmm4

		movaps	xmm6, _ps_atan_t1
		mulps	xmm4, xmm4

		addps	xmm1, xmm4
		movaps	xmm5, _ps_atan_s2
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm2
		movaps	xmm2, _ps_atan_t2
		addps	xmm3, xmm4
		addps	xmm1, xmm3

		movaps	xmm3, _ps_atan_s3
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm6
		movaps	xmm6, _ps_atan_t3
		addps	xmm5, xmm4
		addps	xmm1, xmm5

		movaps	xmm5, _ps_am_sign_mask
		rcpps	xmm1, xmm1
		mulps	xmm1, xmm2
		addps	xmm3, xmm4
		movaps	xmm4, _ps_am_pi_o_2
		mulps	xmm6, xmm0
		addps	xmm1, xmm3

		andps	xmm0, xmm5
		rcpps	xmm1, xmm1
		movaps	xmm3, [ecx - 32]
		mulps	xmm1, xmm6

		orps	xmm0, xmm4
		subps	xmm0, xmm1
		movaps	xmm2, [ecx - 16]

		andps	xmm0, xmm7
		andnps	xmm7, xmm1
		orps	xmm0, xmm7

		movaps	xmm1, xmm0
		andps	xmm0, xmm3
		addps	xmm1, xmm2
		andnps	xmm3, xmm1
		orps	xmm0, xmm3

		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_atanr2_ss

__m128 __declspec(naked) __stdcall am_atanr2_ss(__m128 y, __m128 rx)
{
	__asm
	{
		// NOTE: substract enough to survive call to am_atan_ss, i.e.
		//  16 byte for the argument + 4 byte for return address
		movss	[esp - 20 - 4], xmm0
		mulss	xmm0, xmm1
		movss	[esp - 20 - 8], xmm1

		sub		esp, 16
		// HACK HACK:
		// We assume am_atan_ss doesn't expect the argument on the stack, just in xmm0
		// This is true for our version of am_atan_ss
		call	am_atan_ss

		xorps	xmm7, xmm7
		movss	xmm1, [esp - 20 - 8]
		comiss	xmm1, xmm7
		movss	xmm4, [esp - 20 - 4]
		jnc		l_pos   // 'nc' is 'ge' for comiss

		andps	xmm4, _ps_am_sign_mask
		orps	xmm4, _ps_am_pi

		addss	xmm0, xmm4

l_pos:
		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_atanr2_ess

__m128 __declspec(naked) __stdcall am_atanr2_ess(__m128 y, __m128 rx)
{
	__asm
	{
		movss	xmm2, _ps_am_sign_mask
		xorps	xmm3, xmm3
		movss	xmm5, _ps_am_1
		andps	xmm2, xmm0
		mulss	xmm0, xmm1
		orps	xmm2, _ps_am_pi
		cmpless	xmm3, xmm1

		movss	xmm6, _ps_am_m1
		rcpss	xmm4, xmm0

		cmpltss	xmm5, xmm0
		cmpnless	xmm6, xmm0
		movss	xmm1, _ps_atan_s0
		orps	xmm5, xmm6

		movss	[esp - 4], xmm2
		movss	[esp - 8], xmm3

		andps	xmm4, xmm5
		movss	xmm2, _ps_atan_t0
		movaps	xmm7, xmm5
		andnps	xmm5, xmm0
		movss	xmm3, _ps_atan_s1
		orps	xmm4, xmm5
		movaps	xmm0, xmm4

		movss	xmm6, _ps_atan_t1
		mulss	xmm4, xmm4

		addss	xmm1, xmm4
		movss	xmm5, _ps_atan_s2
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm2
		movss	xmm2, _ps_atan_t2
		addss	xmm3, xmm4
		addss	xmm1, xmm3

		movss	xmm3, _ps_atan_s3
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm6
		movss	xmm6, _ps_atan_t3
		addss	xmm5, xmm4
		addss	xmm1, xmm5

		movss	xmm5, _ps_am_sign_mask
		rcpss	xmm1, xmm1
		mulss	xmm1, xmm2
		addss	xmm3, xmm4
		movss	xmm4, _ps_am_pi_o_2
		mulss	xmm6, xmm0
		addss	xmm1, xmm3

		andps	xmm0, xmm5
		rcpss	xmm1, xmm1
		movss	xmm3, [esp - 8]
		mulss	xmm1, xmm6

		orps	xmm0, xmm4
		subss	xmm0, xmm1
		movss	xmm2, [esp - 4]

		andps	xmm0, xmm7
		andnps	xmm7, xmm1
		orps	xmm0, xmm7

		movaps	xmm1, xmm0
		andps	xmm0, xmm3
		addss	xmm1, xmm2
		andnps	xmm3, xmm1
		orps	xmm0, xmm3

		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// exp functions

_PS_CONST(exp_hi,	88.3762626647949f);
_PS_CONST(exp_lo,	-88.3762626647949f);

_PS_CONST(exp_rln2, 1.4426950408889634073599f);

_PS_CONST(exp_p0, 1.26177193074810590878e-4f);
_PS_CONST(exp_p1, 3.02994407707441961300e-2f);

_PS_CONST(exp_q0, 3.00198505138664455042e-6f);
_PS_CONST(exp_q1, 2.52448340349684104192e-3f);
_PS_CONST(exp_q2, 2.27265548208155028766e-1f);
_PS_CONST(exp_q3, 2.00000000000000000009e0f);

_PS_CONST(exp_c1, 6.93145751953125e-1f);
_PS_CONST(exp_c2, 1.42860682030941723212e-6f);

/////////////////////////////////////////////////////////////////////////////
// am_exp_ps

__m128 __declspec(naked) __stdcall am_exp_ps(__m128 x)
{
	__asm
	{
		minps	xmm0, _ps_exp_hi
		movaps	xmm1, _ps_exp_rln2
		maxps	xmm0, _ps_exp_lo
		mulps	xmm1, xmm0
		xorps	xmm7, xmm7
		mov		ecx, esp
		addps	xmm1, _ps_am_0p5

		movaps	xmm2, xmm1

		cvttps2pi	mm0, xmm1
		cmpltps	xmm2, xmm7
		ASM_MOVE_H2L(xmm1)
		andps	xmm2, _ps_am_1
		cvttps2pi	mm1, xmm1
		movq	mm5, _pi32_0x7f

		cvtps2pi	mm2, xmm2  // needn't truncate
		ASM_MOVE_H2L(xmm2)
		cvtps2pi	mm3, xmm2  // needn't truncate

		psubd	mm0, mm2
		psubd	mm1, mm3
		and		ecx, ~15

		cvtpi2ps	xmm1, mm1
		ASM_MOVE_L2H(xmm1)
		paddd	mm1, mm5
		cvtpi2ps	xmm1, mm0
		paddd	mm0, mm5

		movaps	xmm2, xmm1
		mulps	xmm1, _ps_exp_c1
		mulps	xmm2, _ps_exp_c2
		subps	xmm0, xmm1
		pslld	mm0, 23
		subps	xmm0, xmm2
		pslld	mm1, 23

		movaps	xmm2, xmm0
		movq	[ecx - 16], mm0
		mulps	xmm2, xmm2
		movq	[ecx - 16 + 8], mm1

		movaps	xmm6, _ps_exp_q0
		movaps	xmm4, _ps_exp_p0

		mulps	xmm6, xmm2
		movaps	xmm7, _ps_exp_q1
		mulps	xmm4, xmm2
		movaps	xmm5, _ps_exp_p1

		addps	xmm6, xmm7
		addps	xmm4, xmm5

		movaps	xmm7, _ps_exp_q2
		mulps	xmm6, xmm2
		mulps	xmm4, xmm2

		addps	xmm6, xmm7
		mulps	xmm4, xmm0

		movaps	xmm7, _ps_exp_q3
		mulps	xmm6, xmm2
		addps	xmm4, xmm0
		addps	xmm6, xmm7
		movaps	xmm0, [ecx - 16]

		subps	xmm6, xmm4
		movaps	xmm7, _ps_am_1
		rcpps	xmm6, xmm6  
		mulps	xmm4, xmm6
		addps	xmm4, xmm4
		addps	xmm4, xmm7

		mulps	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp_eps

__m128 __declspec(naked) __stdcall am_exp_eps(__m128 x)
{
	__asm
	{
		minps	xmm0, _ps_exp_hi
		movaps	xmm1, _ps_exp_rln2
		maxps	xmm0, _ps_exp_lo
		mulps	xmm1, xmm0
		xorps	xmm2, xmm2
		addps	xmm1, _ps_am_0p5

		cmpnltps	xmm2, xmm1
		pand	xmm2, _epi32_1

		cvttps2dq	xmm1, xmm1
		movaps	xmm4, _ps_exp_c2

		psubd	xmm1, xmm2
		movaps	xmm5, _ps_exp_c1

		cvtdq2ps	xmm3, xmm1

		mulps	xmm4, xmm3
		mulps	xmm5, xmm3
		movaps	xmm6, _ps_exp_q0
		subps	xmm0, xmm4
		movaps	xmm4, _ps_exp_p0
		subps	xmm0, xmm5

		paddd	xmm1, _epi32_0x7f
		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		movaps	xmm5, _ps_exp_q1
		mulps	xmm6, xmm0
		movaps	xmm3, _ps_exp_p1
		mulps	xmm4, xmm0
		addps	xmm6, xmm5
		movaps	xmm5, _ps_exp_q2
		addps	xmm4, xmm3
		movaps	xmm3, _ps_exp_q3
		mulps	xmm6, xmm0
		mulps	xmm4, xmm0
		addps	xmm6, xmm5
		mulps	xmm4, xmm2
		mulps	xmm6, xmm0
		movaps	xmm0, _ps_am_1
		addps	xmm2, xmm4
		addps	xmm6, xmm3

		pslld	xmm1, 23
		subps	xmm6, xmm2
		rcpps	xmm6, xmm6
		mulps	xmm2, xmm6
		addps	xmm2, xmm2
		addps	xmm0, xmm2

		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp_ss

__m128 __declspec(naked) __stdcall am_exp_ss(__m128 x)
{
	__asm
	{
		minss	xmm0, _ps_exp_hi
		movss	xmm1, _ps_exp_rln2
		maxss	xmm0, _ps_exp_lo
		mulss	xmm1, xmm0
		movss	xmm7, _ps_am_0
		addss	xmm1, _ps_am_0p5
		xor		ecx, ecx

		mov		edx, 1
		comiss	xmm1, xmm7
		cvttss2si	eax, xmm1
		cmovc	ecx, edx  // 'c' is 'lt' for comiss
		sub		eax, ecx

		cvtsi2ss	xmm1, eax
		add		eax, 0x7f

		movss	xmm2, xmm1
		mulss	xmm1, _ps_exp_c1
		mulss	xmm2, _ps_exp_c2
		subss	xmm0, xmm1
		shl		eax, 23
		subss	xmm0, xmm2

		movss	xmm2, xmm0
		mov 	[esp - 4], eax
		mulss	xmm2, xmm2

		movss	xmm6, _ps_exp_q0
		movss	xmm4, _ps_exp_p0

		mulss	xmm6, xmm2
		movss	xmm7, _ps_exp_q1
		mulss	xmm4, xmm2
		movss	xmm5, _ps_exp_p1

		addss	xmm6, xmm7
		addss	xmm4, xmm5

		movss	xmm7, _ps_exp_q2
		mulss	xmm6, xmm2
		mulss	xmm4, xmm2

		addss	xmm6, xmm7
		mulss	xmm4, xmm0

		movss	xmm7, _ps_exp_q3
		mulss	xmm6, xmm2
		addss	xmm4, xmm0
		addss	xmm6, xmm7
		movss	xmm0, [esp - 4]

		subss	xmm6, xmm4
		rcpss	xmm6, xmm6  
		movss	xmm7, _ps_am_1
		mulss	xmm4, xmm6
		addss	xmm4, xmm4
		addss	xmm4, xmm7

		mulss	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp_ess

__m128 __declspec(naked) __stdcall am_exp_ess(__m128 x)
{
	__asm
	{
		minss	xmm0, _ps_exp_hi
		movss	xmm1, _ps_exp_rln2
		maxss	xmm0, _ps_exp_lo
		mulss	xmm1, xmm0
		movd	xmm3, _epi32_1
		xorps	xmm2, xmm2
		addss	xmm1, _ps_am_0p5

		cmpnltss	xmm2, xmm1
		pand	xmm2, xmm3

		cvttps2dq	xmm1, xmm1
		movss	xmm4, _ps_exp_c2

		psubd	xmm1, xmm2
		movss	xmm5, _ps_exp_c1

		cvtdq2ps	xmm3, xmm1

		movd	xmm7, _epi32_0x7f

		mulss	xmm4, xmm3
		mulss	xmm5, xmm3
		movss	xmm6, _ps_exp_q0
		subss	xmm0, xmm4
		movss	xmm4, _ps_exp_p0
		subss	xmm0, xmm5

		paddd	xmm1, xmm7
		movss	xmm2, xmm0
		mulss	xmm0, xmm0

		movss	xmm5, _ps_exp_q1
		mulss	xmm6, xmm0
		movss	xmm3, _ps_exp_p1
		mulss	xmm4, xmm0
		addss	xmm6, xmm5
		movss	xmm5, _ps_exp_q2
		addss	xmm4, xmm3
		movss	xmm3, _ps_exp_q3
		mulss	xmm6, xmm0
		mulss	xmm4, xmm0
		addss	xmm6, xmm5
		mulss	xmm4, xmm2
		mulss	xmm6, xmm0
		movss	xmm0, _ps_am_1
		addss	xmm2, xmm4
		addss	xmm6, xmm3

		pslld	xmm1, 23
		subss	xmm6, xmm2
		rcpss	xmm6, xmm6
		mulss	xmm2, xmm6
		addss	xmm2, xmm2
		addss	xmm0, xmm2

		mulss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// exp2 functions

_PS_CONST(exp2_hi, 127.4999961853f);
_PS_CONST(exp2_lo, -127.4999961853f);

_PS_CONST(exp2_p0, 2.30933477057345225087e-2f);
_PS_CONST(exp2_p1, 2.02020656693165307700e1f);
_PS_CONST(exp2_p2, 1.51390680115615096133e3f);

_PS_CONST(exp2_q0, 2.33184211722314911771e2f);
_PS_CONST(exp2_q1, 4.36821166879210612817e3f);

/////////////////////////////////////////////////////////////////////////////
// am_exp2_ps

__m128 __declspec(naked) __stdcall am_exp2_ps(__m128 x)
{
	__asm
	{
		minps	xmm0, _ps_exp2_hi
		movaps	xmm1, _ps_am_0p5
		maxps	xmm0, _ps_exp2_lo
		xorps	xmm7, xmm7

		addps	xmm1, xmm0
		mov		ecx, esp

		movaps	xmm2, xmm1

		cvttps2pi	mm0, xmm1
		cmpltps	xmm2, xmm7
		ASM_MOVE_H2L(xmm1)
		andps	xmm2, _ps_am_1
		cvttps2pi	mm1, xmm1
		movq	mm6, _pi32_0x7f

		cvtps2pi	mm2, xmm2
		ASM_MOVE_H2L(xmm2)
		movq	mm7, _pi32_0xff
		cvtps2pi	mm3, xmm2

		psubd	mm0, mm2
		psubd	mm1, mm3
		and		ecx, ~15

		cvtpi2ps	xmm1, mm1
		ASM_MOVE_L2H(xmm1)
		paddd	mm1, mm6
		cvtpi2ps	xmm1, mm0
		paddd	mm0, mm6

		subps	xmm0, xmm1
		movaps	xmm2, xmm0
		mulps	xmm2, xmm2

		movaps	xmm6, _ps_exp2_q0
		movaps	xmm4, _ps_exp2_p0

		mulps	xmm6, xmm2
		movaps	xmm7, _ps_exp2_q1
		mulps	xmm4, xmm2
		movaps	xmm5, _ps_exp2_p1

		addps	xmm6, xmm7
		pslld	mm0, 23
		addps	xmm4, xmm5

		movaps	xmm5, _ps_exp2_p2
		mulps	xmm4, xmm2
		pslld	mm1, 23

		addps	xmm4, xmm5
		movq	[ecx - 16], mm0

		mulps	xmm4, xmm0
		movq	[ecx - 16 + 8], mm1

		subps	xmm6, xmm4
		movaps	xmm7, _ps_am_1
		rcpps	xmm6, xmm6  
		mulps	xmm4, xmm6
		movaps	xmm0, [ecx - 16]
		addps	xmm4, xmm4
		addps	xmm4, xmm7

		mulps	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp2_eps

__m128 __declspec(naked) __stdcall am_exp2_eps(__m128 x)
{
	__asm
	{
		minps	xmm0, _ps_exp2_hi
		movaps	xmm1, _ps_am_0p5
		maxps	xmm0, _ps_exp2_lo
		xorps	xmm2, xmm2
		addps	xmm1, xmm0

		cmpnltps	xmm2, xmm1
		pand	xmm2, _epi32_1

		cvttps2dq	xmm1, xmm1

		psubd	xmm1, xmm2

		movaps	xmm4, _ps_exp2_p0
		movaps	xmm6, _ps_exp2_q0

		cvtdq2ps	xmm2, xmm1

		subps	xmm0, xmm2

		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		paddd	xmm1, _epi32_0x7f

		mulps	xmm4, xmm0
		mulps	xmm6, xmm0
		addps	xmm4, _ps_exp2_p1
		addps	xmm6, _ps_exp2_q1

		mulps	xmm4, xmm0
		addps	xmm4, _ps_exp2_p2

		mulps	xmm2, xmm4

		subps	xmm6, xmm2
		pslld	xmm1, 23
		rcpps	xmm6, xmm6
		movaps	xmm0, _ps_am_1
		mulps	xmm2, xmm6
		addps	xmm2, xmm2
		addps	xmm0, xmm2

		mulps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp2_ss

__m128 __declspec(naked) __stdcall am_exp2_ss(__m128 x)
{
	__asm
	{
		minss	xmm0, _ps_exp2_hi
		movss	xmm5, _ps_am_0p5
		maxss	xmm0, _ps_exp2_lo
		xorps	xmm7, xmm7
		addss	xmm5, xmm0
		xor		ecx, ecx

		mov		edx, 1
		comiss	xmm5, xmm7
		cvttss2si	eax, xmm5
		cmovc	ecx, edx  // 'c' is 'lt' for comiss
		sub		eax, ecx

		cvtsi2ss	xmm5, eax
		add		eax, 0x7f

		subss	xmm0, xmm5

		movss	xmm2, xmm0
		mulss	xmm2, xmm2

		movss	xmm6, _ps_exp2_q0
		movss	xmm4, _ps_exp2_p0

		mulss	xmm6, xmm2
		movss	xmm7, _ps_exp2_q1
		mulss	xmm4, xmm2
		movss	xmm5, _ps_exp2_p1

		shl		eax, 23
		addss	xmm6, xmm7
		addss	xmm4, xmm5

		movss	xmm5, _ps_exp2_p2
		mulss	xmm4, xmm2

		addss	xmm4, xmm5

		mulss	xmm4, xmm0

		mov 	[esp - 4], eax
		subss	xmm6, xmm4
		movss	xmm7, _ps_am_1
		rcpss	xmm6, xmm6  
		mulss	xmm4, xmm6
		movss	xmm0, [esp - 4]
		addss	xmm4, xmm4
		addss	xmm4, xmm7

		mulss	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_exp2_ss

__m128 __declspec(naked) __stdcall am_exp2_ess(__m128 x)
{
	__asm
	{
		minss	xmm0, _ps_exp2_hi
		movss	xmm5, _ps_am_0p5
		maxss	xmm0, _ps_exp2_lo
		xorps	xmm7, xmm7
		addss	xmm5, xmm0
		xor		ecx, ecx

		mov		edx, 1
		comiss	xmm5, xmm7
		cvttss2si	eax, xmm5
		cmovc	ecx, edx  // 'c' is 'lt' for comiss
		sub		eax, ecx

		cvtsi2ss	xmm5, eax
		add		eax, 0x7f

		subss	xmm0, xmm5

		movss	xmm2, xmm0
		mulss	xmm0, xmm0

		movss	xmm6, _ps_exp2_q0
		movss	xmm4, _ps_exp2_p0

		mulss	xmm6, xmm0
		movss	xmm7, _ps_exp2_q1
		mulss	xmm4, xmm0
		movss	xmm5, _ps_exp2_p1

		shl		eax, 23
		addss	xmm6, xmm7
		addss	xmm4, xmm5

		movss	xmm5, _ps_exp2_p2
		mulss	xmm4, xmm0

		movd 	xmm0, eax

		addss	xmm4, xmm5
		mulss	xmm4, xmm2

		subss	xmm6, xmm4
		movss	xmm7, _ps_am_1
		rcpss	xmm6, xmm6  
		mulss	xmm4, xmm6
		addss	xmm4, xmm4
		addss	xmm4, xmm7

		mulss	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// log functions

_PS_CONST(log_p0, -7.89580278884799154124e-1f);
_PS_CONST(log_p1, 1.63866645699558079767e1f);
_PS_CONST(log_p2, -6.41409952958715622951e1f);

_PS_CONST(log_q0, -3.56722798256324312549e1f);
_PS_CONST(log_q1, 3.12093766372244180303e2f);
_PS_CONST(log_q2, -7.69691943550460008604e2f);

_PS_CONST(log_c0, 0.693147180559945f);

/////////////////////////////////////////////////////////////////////////////
// am_log_ps

__m128 __declspec(naked) __stdcall am_log_ps(__m128 x)
{
	__asm
	{
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		mov		ecx, esp
		movaps	xmm7, _ps_am_inv_mant_mask
		and		ecx, ~15
		movaps	xmm1, _ps_am_1
		movaps	[ecx - 16], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm1
		movaps	xmm7, xmm0

		addps	xmm7, xmm1
		subps	xmm0, xmm1
		movq	mm0, [ecx - 16]
		rcpps	xmm7, xmm7  
		mulps	xmm0, xmm7
		movq	mm1, [ecx - 16 + 8]
		addps	xmm0, xmm0

		movq	mm7, _pi32_0x7f
		movaps	xmm2, xmm0
		mulps	xmm2, xmm2

		movaps	xmm4, _ps_log_p0
		psrld	mm1, 23
		movaps	xmm6, _ps_log_q0
		psrld	mm0, 23

		mulps	xmm4, xmm2
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm2
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		psubd	mm1, mm7
		addps	xmm6, xmm7
		psubd	mm0, mm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm2
		cvtpi2ps	xmm1, mm1
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm2

		addps	xmm4, xmm5
		ASM_MOVE_L2H(xmm1)
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_c0
		mulps	xmm4, xmm2
		cvtpi2ps	xmm1, mm0
		rcpps	xmm6, xmm6  

		mulps	xmm4, xmm0
		mulps	xmm1, xmm5
		mulps	xmm4, xmm6

		addps	xmm0, xmm1
		addps	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log_eps

__m128 __declspec(naked) __stdcall am_log_eps(__m128 x)
{
	__asm
	{
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movaps	xmm1, _ps_am_1
		movaps	xmm3, xmm0

		andps	xmm0, _ps_am_inv_mant_mask
		orps	xmm0, xmm1

		movaps	xmm4, xmm0
		subps	xmm0, xmm1
		addps	xmm4, xmm1
		psrld	xmm3, 23
		rcpps	xmm4, xmm4
		mulps	xmm0, xmm4
		psubd	xmm3, _epi32_0x7f
		addps	xmm0, xmm0

		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		movaps	xmm4, _ps_log_p0
		movaps	xmm6, _ps_log_q0

		mulps	xmm4, xmm0
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm0
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm0
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm0

		addps	xmm4, xmm5
		movaps	xmm5, _ps_log_c0
		addps	xmm6, xmm7
		cvtdq2ps	xmm1, xmm3

		mulps	xmm0, xmm4
		rcpps	xmm6, xmm6

		mulps	xmm0, xmm6
		mulps	xmm0, xmm2

		mulps	xmm1, xmm5

		addps	xmm0, xmm2
		addps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log_ss

__m128 __declspec(naked) __stdcall am_log_ss(__m128 x)
{
	__asm
	{
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movss	xmm7, _ps_am_inv_mant_mask
		movss	xmm1, _ps_am_1
		movss	[esp - 4], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm1
		movss	xmm7, xmm0
		mov		edx, [esp - 4]

		addss	xmm7, xmm1
		subss	xmm0, xmm1
		rcpss	xmm7, xmm7  
		mulss	xmm0, xmm7
		addss	xmm0, xmm0

		shr		edx, 23

		movss	xmm2, xmm0
		sub		edx, 0x7f
		mulss	xmm2, xmm2

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		mulss	xmm4, xmm2
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm2
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm2
		cvtsi2ss	xmm1, edx
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm2

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_c0
		mulss	xmm4, xmm2
		rcpss	xmm6, xmm6  

		mulss	xmm4, xmm0
		mulss	xmm1, xmm5
		mulss	xmm4, xmm6

		addss	xmm0, xmm1
		addss	xmm0, xmm4

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log_ess

__m128 __declspec(naked) __stdcall am_log_ess(__m128 x)
{
	__asm
	{
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movss	xmm1, _ps_am_1
		movd	edx, xmm0

		andps	xmm0, _ps_am_inv_mant_mask
		orps	xmm0, xmm1

		movaps	xmm4, xmm0
		subss	xmm0, xmm1
		addss	xmm4, xmm1
		shr		edx, 23
		rcpss	xmm4, xmm4
		mulss	xmm0, xmm4
		addss	xmm0, xmm0

		movaps	xmm2, xmm0
		mulss	xmm0, xmm0
		sub		edx, 0x7f

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		mulss	xmm4, xmm0
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm0
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm0
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm0

		addss	xmm4, xmm5
		movss	xmm5, _ps_log_c0
		addss	xmm6, xmm7
		cvtsi2ss	xmm1, edx

		mulss	xmm0, xmm4
		rcpss	xmm6, xmm6

		mulss	xmm0, xmm6
		mulss	xmm0, xmm2

		mulss	xmm1, xmm5

		addss	xmm0, xmm2
		addss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// log2 functions

_PS_CONST(log2_c0, 1.44269504088896340735992f);

/////////////////////////////////////////////////////////////////////////////
// am_log2_ps

__m128 __declspec(naked) __stdcall am_log2_ps(__m128 x)
{
	__asm
	{
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		mov		ecx, esp
		movaps	xmm7, _ps_am_inv_mant_mask
		and		ecx, ~15
		movaps	xmm1, _ps_am_1
		movaps	[ecx - 16], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm1
		movaps	xmm7, xmm0

		addps	xmm7, xmm1
		subps	xmm0, xmm1
		movq	mm0, [ecx - 16]
		rcpps	xmm7, xmm7  
		mulps	xmm0, xmm7
		movq	mm1, [ecx - 16 + 8]
		addps	xmm0, xmm0

		movq	mm7, _pi32_0x7f
		psrld	mm0, 23
		psrld	mm1, 23

		movaps	xmm2, xmm0
		psubd	mm0, mm7
		mulps	xmm2, xmm2
		psubd	mm1, mm7

		movaps	xmm4, _ps_log_p0
		movaps	xmm6, _ps_log_q0

		mulps	xmm4, xmm2
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm2
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm2
		cvtpi2ps	xmm1, mm1
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm2

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log2_c0
		mulps	xmm4, xmm2
		rcpps	xmm6, xmm6  
		ASM_MOVE_L2H(xmm1)

		mulps	xmm4, xmm6
		cvtpi2ps	xmm1, mm0
		mulps	xmm4, xmm0
		addps	xmm0, xmm4
		mulps	xmm0, xmm5

		addps	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log2_eps

__m128 __declspec(naked) __stdcall am_log2_eps(__m128 x)
{
	__asm
	{
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movaps	xmm1, _ps_am_1
		movaps	xmm3, xmm0

		andps	xmm0, _ps_am_inv_mant_mask
		orps	xmm0, xmm1

		movaps	xmm4, xmm0
		subps	xmm0, xmm1
		addps	xmm4, xmm1
		psrld	xmm3, 23
		rcpps	xmm4, xmm4
		mulps	xmm0, xmm4
		psubd	xmm3, _epi32_0x7f
		addps	xmm0, xmm0

		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		movaps	xmm4, _ps_log_p0
		movaps	xmm6, _ps_log_q0

		mulps	xmm4, xmm0
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm0
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm0
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm0

		addps	xmm4, xmm5
		movaps	xmm5, _ps_log2_c0
		addps	xmm6, xmm7
		cvtdq2ps	xmm1, xmm3

		mulps	xmm0, xmm4
		rcpps	xmm6, xmm6

		mulps	xmm0, xmm6

		mulps	xmm0, xmm2
		mulps	xmm2, xmm5
		mulps	xmm0, xmm5
		addps	xmm2, xmm1
		addps	xmm0, xmm2

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log2_ss

__m128 __declspec(naked) __stdcall am_log2_ss(__m128 x)
{
	__asm
	{
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movss	xmm7, _ps_am_inv_mant_mask
		movss	xmm1, _ps_am_1
		movss	[esp - 4], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm1
		movss	xmm7, xmm0
		mov		edx, [esp - 4]

		addss	xmm7, xmm1
		subss	xmm0, xmm1
		rcpss	xmm7, xmm7  
		mulss	xmm0, xmm7
		addss	xmm0, xmm0

		shr		edx, 23

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		movss	xmm2, xmm0
		sub		edx, 0x7f
		mulss	xmm2, xmm2

		mulss	xmm4, xmm2
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm2
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm2
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm2

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log2_c0
		mulss	xmm4, xmm2
		rcpss	xmm6, xmm6  

		cvtsi2ss	xmm1, edx

		mulss	xmm6, xmm0
		mulss	xmm4, xmm6
		addss	xmm0, xmm4
		mulss	xmm0, xmm5

		addss	xmm0, xmm1

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_log2_ess

__m128 __declspec(naked) __stdcall am_log2_ess(__m128 x)
{
	__asm
	{
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movss	xmm1, _ps_am_1
		movaps	xmm3, xmm0

		andps	xmm0, _ps_am_inv_mant_mask
		orps	xmm0, xmm1

		movaps	xmm4, xmm0
		movss	xmm7, _epi32_0x7f
		subss	xmm0, xmm1
		addss	xmm4, xmm1
		psrld	xmm3, 23
		rcpss	xmm4, xmm4
		mulss	xmm0, xmm4
		psubd	xmm3, xmm7
		addss	xmm0, xmm0

		movaps	xmm2, xmm0
		mulss	xmm0, xmm0

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		mulss	xmm4, xmm0
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm0
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm0
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm0

		addss	xmm4, xmm5
		movss	xmm5, _ps_log2_c0
		addss	xmm6, xmm7
		cvtdq2ps	xmm1, xmm3

		mulss	xmm0, xmm4
		rcpss	xmm6, xmm6

		mulss	xmm0, xmm6

		mulss	xmm0, xmm2
		mulss	xmm2, xmm5
		mulss	xmm0, xmm5
		addss	xmm2, xmm1
		addss	xmm0, xmm2

		ret		16
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_pow_ps

extern float* GetTraceBuffer();

__m128 __declspec(naked) __stdcall am_pow_ps(__m128 x, __m128 y)
{
	__asm
	{
		xorps	xmm4, xmm4
		cmpltps	xmm4, xmm0
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		mov		ecx, esp
		movaps	xmm7, _ps_am_inv_mant_mask
		and		ecx, ~15
		movaps	xmm3, _ps_am_1
		movaps	[ecx - 16], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm3
		movaps	xmm7, xmm0

		subps	xmm0, xmm3
		addps	xmm7, xmm3
		movq	mm0, [ecx - 16]
		rcpps	xmm7, xmm7  
		mulps	xmm0, xmm7
		movq	mm1, [ecx - 16 + 8]
		addps	xmm0, xmm0

		movq	mm7, _pi32_0x7f
		psrld	mm0, 23
		psrld	mm1, 23
		movaps	[ecx - 32], xmm4

		movaps	xmm2, xmm0
		psubd	mm0, mm7
		mulps	xmm2, xmm2
		psubd	mm1, mm7

		movaps	xmm4, _ps_log_p0
		movaps	xmm6, _ps_log_q0

		mulps	xmm4, xmm2
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm2
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm2
		cvtpi2ps	xmm3, mm1
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm2

		ASM_MOVE_L2H(xmm3)
		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log2_c0
		mulps	xmm4, xmm2
		cvtpi2ps	xmm3, mm0
		rcpps	xmm6, xmm6  

		mulps	xmm5, xmm1
		mulps	xmm4, xmm6
		movaps	xmm6, _ps_exp2_hi
		mulps	xmm4, xmm0
		addps	xmm0, xmm4
		movaps	xmm4, _ps_exp2_lo
		mulps	xmm3, xmm1
		mulps	xmm0, xmm5
		movaps	xmm5, _ps_am_1
		xorps	xmm7, xmm7

		addps	xmm0, xmm3
		movaps	xmm3, _ps_am_0p5

		minps	xmm0, xmm6
		maxps	xmm0, xmm4

		addps	xmm3, xmm0

		movaps	xmm2, xmm3

		cvttps2pi	mm0, xmm3
		cmpltps	xmm2, xmm7
		ASM_MOVE_H2L(xmm3)
		andps	xmm2, xmm5
		cvttps2pi	mm1, xmm3
		movq	mm5, _pi32_0x7f

		cvtps2pi	mm2, xmm2  // needn't truncate
		ASM_MOVE_H2L(xmm2)
		cvtps2pi	mm3, xmm2  // needn't truncate

		psubd	mm0, mm2
		psubd	mm1, mm3

		cvtpi2ps	xmm3, mm1
		ASM_MOVE_L2H(xmm3)
		paddd	mm1, mm5
		cvtpi2ps	xmm3, mm0
		paddd	mm0, mm5

		subps	xmm0, xmm3

		movaps	xmm2, xmm0
		mulps	xmm2, xmm2

		movaps	xmm6, _ps_exp2_q0
		movaps	xmm4, _ps_exp2_p0

		mulps	xmm6, xmm2
		movaps	xmm7, _ps_exp2_q1
		mulps	xmm4, xmm2
		movaps	xmm5, _ps_exp2_p1

		addps	xmm6, xmm7
		pslld	mm0, 23
		addps	xmm4, xmm5

		movaps	xmm5, _ps_exp2_p2
		mulps	xmm4, xmm2
		pslld	mm1, 23
		movaps	xmm3, [ecx - 32]

		addps	xmm4, xmm5
		movq	[ecx - 16], mm0

		mulps	xmm4, xmm0
		movq	[ecx - 16 + 8], mm1

		subps	xmm6, xmm4
		movaps	xmm7, _ps_am_1
		rcpps	xmm6, xmm6  
		mulps	xmm4, xmm6
		movaps	xmm0, [ecx - 16]
		addps	xmm4, xmm4
		addps	xmm4, xmm7

		mulps	xmm0, xmm4
		andps	xmm0, xmm3

		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_pow_eps

__m128 __declspec(naked) __stdcall am_pow_eps(__m128 x, __m128 y)
{
	__asm
	{
		xorps	xmm5, xmm5
		cmpltps	xmm5, xmm0
		mov		ecx, esp
		maxps	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movaps	xmm7, _ps_am_1
		movaps	xmm3, xmm0
		and		ecx, ~15

		andps	xmm0, _ps_am_inv_mant_mask
		orps	xmm0, xmm7

		movaps	[ecx - 16], xmm5

		movaps	xmm4, xmm0
		subps	xmm0, xmm7
		addps	xmm4, xmm7
		psrld	xmm3, 23
		rcpps	xmm4, xmm4
		mulps	xmm0, xmm4
		psubd	xmm3, _epi32_0x7f
		addps	xmm0, xmm0

		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		movaps	xmm4, _ps_log_p0
		movaps	xmm6, _ps_log_q0

		mulps	xmm4, xmm0
		movaps	xmm5, _ps_log_p1
		mulps	xmm6, xmm0
		movaps	xmm7, _ps_log_q1

		addps	xmm4, xmm5
		addps	xmm6, xmm7

		movaps	xmm5, _ps_log_p2
		mulps	xmm4, xmm0
		movaps	xmm7, _ps_log_q2
		mulps	xmm6, xmm0

		addps	xmm4, xmm5
		movaps	xmm5, _ps_log2_c0
		addps	xmm6, xmm7
		cvtdq2ps	xmm7, xmm3

		mulps	xmm0, xmm4
		rcpps	xmm6, xmm6

		mulps	xmm0, xmm6
		movaps	xmm4, _ps_exp2_hi
		mulps	xmm0, xmm2
		movaps	xmm6, _ps_exp2_lo
		mulps	xmm2, xmm5
		mulps	xmm0, xmm5
		addps	xmm2, xmm7
		movaps	xmm3, _ps_am_0p5
		addps	xmm0, xmm2
		xorps	xmm2, xmm2

		mulps	xmm0, xmm1

		minps	xmm0, xmm4
		movaps	xmm4, _ps_exp2_p0
		maxps	xmm0, xmm6
		movaps	xmm6, _ps_exp2_q0

		addps	xmm3, xmm0

		cmpnltps	xmm2, xmm3
		pand	xmm2, _epi32_1

		cvttps2dq	xmm3, xmm3

		psubd	xmm3, xmm2
		movaps	xmm5, _ps_exp2_p1

		cvtdq2ps	xmm2, xmm3
		movaps	xmm7, _ps_exp2_q1

		subps	xmm0, xmm2

		movaps	xmm2, xmm0
		mulps	xmm0, xmm0

		paddd	xmm3, _epi32_0x7f

		mulps	xmm4, xmm0
		mulps	xmm6, xmm0
		addps	xmm4, xmm5
		addps	xmm6, xmm7

		mulps	xmm4, xmm0
		movaps	xmm5, [ecx - 16]
		pslld	xmm3, 23
		addps	xmm4, _ps_exp2_p2

		mulps	xmm2, xmm4

		movaps	xmm0, _ps_am_1
		subps	xmm6, xmm2
		andps	xmm3, xmm5
		rcpps	xmm6, xmm6
		mulps	xmm2, xmm6
		addps	xmm2, xmm2
		addps	xmm0, xmm2

		mulps	xmm0, xmm3

		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_pow_ss

__m128 __declspec(naked) __stdcall am_pow_ss(__m128 x, __m128 y)
{
	__asm
	{
		xorps	xmm7, xmm7
		comiss	xmm7, xmm0
		movss	xmm7, _ps_am_inv_mant_mask
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		jnc		l_zerobase
		movss	xmm3, _ps_am_1
		movss	[esp - 4], xmm0

		andps	xmm0, xmm7
		orps	xmm0, xmm3
		movss	xmm7, xmm0

		addss	xmm7, xmm3
		subss	xmm0, xmm3
		mov		edx, [esp - 4]
		rcpss	xmm7, xmm7  
		mulss	xmm0, xmm7
		addss	xmm0, xmm0

		shr		edx, 23

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		sub		edx, 0x7f
		movss	xmm2, xmm0
		mulss	xmm2, xmm2

		mulss	xmm4, xmm2
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm2
		cvtsi2ss	xmm3, edx
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		mulss	xmm3, xmm1
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm2
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm2

		addss	xmm4, xmm5
		mulss	xmm1, _ps_log2_c0
		addss	xmm6, xmm7

		mulss	xmm4, xmm2
		rcpss	xmm6, xmm6  

		mulss	xmm6, xmm0
		mulss	xmm4, xmm6
		movss	xmm6, _ps_exp2_hi
		addss	xmm0, xmm4
		movss	xmm4, _ps_exp2_lo
		xorps	xmm7, xmm7
		movss	xmm5, _ps_am_0p5
		mulss	xmm0, xmm1

		addss	xmm0, xmm3
		xor		ecx, ecx

		minss	xmm0, xmm6
		mov		edx, 1
		maxss	xmm0, xmm4

		addss	xmm5, xmm0

		comiss	xmm5, xmm7
		cvttss2si	eax, xmm5
		cmovc	ecx, edx  // 'c' is 'lt' for comiss
		sub		eax, ecx

		cvtsi2ss	xmm5, eax
		add		eax, 0x7f

		subss	xmm0, xmm5

		movss	xmm2, xmm0
		mulss	xmm2, xmm2

		movss	xmm6, _ps_exp2_q0
		movss	xmm4, _ps_exp2_p0

		mulss	xmm6, xmm2
		movss	xmm7, _ps_exp2_q1
		mulss	xmm4, xmm2
		movss	xmm5, _ps_exp2_p1

		shl		eax, 23
		addss	xmm6, xmm7
		addss	xmm4, xmm5

		movss	xmm5, _ps_exp2_p2
		mulss	xmm4, xmm2

		addss	xmm4, xmm5

		mulss	xmm4, xmm0

		mov 	[esp - 4], eax
		subss	xmm6, xmm4
		movss	xmm7, _ps_am_1
		rcpss	xmm6, xmm6  
		mulss	xmm4, xmm6
		movss	xmm0, [esp - 4]
		addss	xmm4, xmm4
		addss	xmm4, xmm7

		mulss	xmm0, xmm4

		ret		32

l_zerobase:
		xorps	xmm0, xmm0

		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// am_pow_ess

__m128 __declspec(naked) __stdcall am_pow_ess(__m128 x, __m128 y)
{
	__asm
	{
		xorps	xmm5, xmm5
		movss	xmm2, _ps_am_inv_mant_mask
		cmpltss	xmm5, xmm0
		maxss	xmm0, _ps_am_min_norm_pos  // cut off denormalized stuff
		movss	xmm7, _ps_am_1
		movaps	xmm3, xmm0

		andps	xmm0, xmm2
		orps	xmm0, xmm7

		movss	[esp - 4], xmm5

		movaps	xmm4, xmm0
		movd	xmm2, _epi32_0x7f
		subss	xmm0, xmm7
		addss	xmm4, xmm7
		psrld	xmm3, 23
		rcpss	xmm4, xmm4
		mulss	xmm0, xmm4
		psubd	xmm3, xmm2
		addss	xmm0, xmm0

		movaps	xmm2, xmm0
		mulss	xmm0, xmm0

		movss	xmm4, _ps_log_p0
		movss	xmm6, _ps_log_q0

		mulss	xmm4, xmm0
		movss	xmm5, _ps_log_p1
		mulss	xmm6, xmm0
		movss	xmm7, _ps_log_q1

		addss	xmm4, xmm5
		addss	xmm6, xmm7

		movss	xmm5, _ps_log_p2
		mulss	xmm4, xmm0
		movss	xmm7, _ps_log_q2
		mulss	xmm6, xmm0

		addss	xmm4, xmm5
		movss	xmm5, _ps_log2_c0
		addss	xmm6, xmm7
		cvtdq2ps	xmm7, xmm3

		mulss	xmm0, xmm4
		rcpss	xmm6, xmm6

		mulss	xmm0, xmm6
		movss	xmm4, _ps_exp2_hi
		mulss	xmm0, xmm2
		movss	xmm6, _ps_exp2_lo
		mulss	xmm2, xmm5
		mulss	xmm0, xmm5
		addss	xmm2, xmm7
		movss	xmm3, _ps_am_0p5
		addss	xmm0, xmm2
		xorps	xmm2, xmm2
		movd	xmm5, _epi32_1

		mulss	xmm0, xmm1

		minss	xmm0, xmm4
		movss	xmm4, _ps_exp2_p0
		maxss	xmm0, xmm6
		movss	xmm6, _ps_exp2_q0

		addss	xmm3, xmm0

		cmpnltss	xmm2, xmm3
		pand	xmm2, xmm5

		cvttps2dq	xmm3, xmm3

		psubd	xmm3, xmm2

		cvtdq2ps	xmm2, xmm3

		subss	xmm0, xmm2

		movaps	xmm2, xmm0
		mulss	xmm0, xmm0

		paddd	xmm3, _epi32_0x7f

		mulss	xmm4, xmm0
		mulss	xmm6, xmm0
		addss	xmm4, _ps_exp2_p1
		addss	xmm6, _ps_exp2_q1

		mulss	xmm4, xmm0
		addss	xmm4, _ps_exp2_p2

		mulss	xmm2, xmm4

		movss	xmm0, _ps_am_1
		subss	xmm6, xmm2
		pslld	xmm3, 23
		rcpss	xmm6, xmm6
		movss	xmm5, [esp - 4]
		mulss	xmm2, xmm6
		andps	xmm3, xmm5
		addss	xmm2, xmm2
		addss	xmm0, xmm2

		mulss	xmm0, xmm3


		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////

__m128 __declspec(naked) __stdcall am_dummy_ps(__m128 x)
{
	__asm
	{
		ret		16
	}
}

__m128 __declspec(naked) __stdcall am_dummy2_ps(__m128 x, __m128 y)
{
	__asm
	{
		ret		32
	}
}

/////////////////////////////////////////////////////////////////////////////
// EOF
