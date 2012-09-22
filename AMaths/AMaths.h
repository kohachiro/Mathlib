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
//  Header File
//  Version 2.0
//  Author Alex Klimovitski, Intel GmbH
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// IMPORTANT: Some am_xxx_ps functions use MMX(TM) and do NOT call _m_empty().
//  After calling any of this functions, insert _m_empty() before any x87 code !!!
/////////////////////////////////////////////////////////////////////////////

#if !defined(_AMATHS_H_)

#if defined(__cplusplus)
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////////////
// Constants

#define AM_PI			(3.14159265358979323846)

#define AM_USE_PS_CONST(Name)	\
extern const float _ps_##Name[4];	\
const __m128 Name = _mm_load_ps(_ps_##Name)

#define AM_USE_PS_INT_CONST(Name)	\
extern const __int32 _ps_##Name[4];	\
const __m128 Name = _mm_load_ps((const float*)_ps_##Name)

#define AM_USE_SS_CONST(Name)	\
extern const float _ps_##Name[4];	\
const __m128 Name = _mm_load_ss(_ps_##Name)

#define AM_USE_SS_INT_CONST(Name)	\
extern const __int32 _ps_##Name[4];	\
const __m128 Name = _mm_load_ss((const float*)_ps_##Name)

// 'float' constants defined by AM Library
//
// Usage									Value
// AM_USE_PS_CONST(am_m1)					-1.0
// AM_USE_PS_CONST(am_0p5)					0.5
// AM_USE_PS_CONST(am_1)					1.0
// AM_USE_PS_CONST(am_1p5)					1.5
// 
// AM_USE_PS_CONST(am_pi)					pi
// AM_USE_PS_CONST(am_pi_o_2)				pi / 2.0
// AM_USE_PS_CONST(am_2_o_pi)				2.0 / pi
// AM_USE_PS_CONST(am_pi_o_4)				pi / 4.0
// AM_USE_PS_CONST(am_4_o_pi)				4.0 / pi
// 
// AM_USE_PS_INT_CONST(am_min_norm_pos)		smallest normalized positive
//											'float' value
//

// Mask constants defined by AM Library
//
// Usage									Value
// AM_USE_PS_INT_CONST(am_sign_mask)		sign mask
// AM_USE_PS_INT_CONST(am_inv_sign_mask)	bit-inversed sign mask
//

/////////////////////////////////////////////////////////////////////////////
// Misc. functions

__m128 __stdcall am_sign_ps(__m128 x);
__m128 __stdcall am_abs_ps(__m128 x);

//__m128 __stdcall am_sign_eps(__m128 x);
#define am_sign_eps		am_sign_ps
//__m128 __stdcall am_abs_eps(__m128 x);
#define am_abs_eps		am_abs_ps

__m128 __stdcall am_sign_ss(__m128 x);
__m128 __stdcall am_abs_ss(__m128 x);

//__m128 __stdcall am_sign_ess(__m128 x);
#define am_sign_ess		am_sign_ss
//__m128 __stdcall am_abs_ess(__m128 x);
#define am_abs_ess		am_abs_ss

/////////////////////////////////////////////////////////////////////////////
// sin, cos, tan functions

__m128 __stdcall am_sin_ps(__m128 x);  // needs _m_empty() !!!
__m128 __stdcall am_cos_ps(__m128 x);  // needs _m_empty() !!!
void __stdcall am_sincos_ps(__m128 x, __m128* s, __m128* c);  // needs _m_empty() !!!
__m128 __stdcall am_tan_ps(__m128 x);  // needs _m_empty() !!!

__m128 __stdcall am_sin_eps(__m128 x);
__m128 __stdcall am_cos_eps(__m128 x);
void __stdcall am_sincos_eps(__m128 x, __m128* s, __m128* c);
__m128 __stdcall am_tan_eps(__m128 x);

__m128 __stdcall am_sin_ss(__m128 x);
__m128 __stdcall am_cos_ss(__m128 x);
void __stdcall am_sincos_ss(__m128 x, __m128* s, __m128* c);
__m128 __stdcall am_tan_ss(__m128 x);

__m128 __stdcall am_sin_ess(__m128 x);
__m128 __stdcall am_cos_ess(__m128 x);
void __stdcall am_sincos_ess(__m128 x, __m128* s, __m128* c);
__m128 __stdcall am_tan_ess(__m128 x);

/////////////////////////////////////////////////////////////////////////////
// asin, acos, atan functions

__m128 __stdcall am_atan_ps(__m128 x);
__m128 __stdcall am_atanr2_ps(__m128 y, __m128 rx); // atan2(y, 1/rx)

__m128 __stdcall am_atan_ss(__m128 x);
__m128 __stdcall am_atanr2_ss(__m128 y, __m128 rx); // atan2(y, 1/rx)

//__m128 __stdcall am_atan_eps(__m128 x);
#define am_atan_eps		am_atan_ps
__m128 __stdcall am_atan_ess(__m128 x);
//__m128 __stdcall am_atanr2_eps(__m128 y, __m128 rx); // atan2(y, 1/rx)
#define am_atanr2_eps	am_atanr2_ps

__m128 __stdcall am_atan_ess(__m128 x);
__m128 __stdcall am_atanr2_ess(__m128 y, __m128 rx); // atan2(y, 1/rx)

__m128 __stdcall am_asin_ps(__m128 x);
__m128 __stdcall am_acos_ps(__m128 x);

//__m128 __stdcall am_asin_eps(__m128 x);
#define am_asin_eps		am_asin_ps
//__m128 __stdcall am_acos_eps(__m128 x);
#define am_acos_eps		am_acos_ps

__m128 __stdcall am_asin_ss(__m128 x);
__m128 __stdcall am_acos_ss(__m128 x);

//__m128 __stdcall am_asin_ess(__m128 x);
#define am_asin_ess		am_asin_ss
//__m128 __stdcall am_acos_ess(__m128 x);
#define am_acos_ess		am_acos_ss

/////////////////////////////////////////////////////////////////////////////
// exp and log functions

__m128 __stdcall am_exp_ps(__m128 x);  // needs _m_empty() !!!
__m128 __stdcall am_exp_ss(__m128 x);

__m128 __stdcall am_exp_eps(__m128 x);
__m128 __stdcall am_exp_ess(__m128 x);

__m128 __stdcall am_exp2_ps(__m128 x);  // needs _m_empty() !!!
__m128 __stdcall am_exp2_ss(__m128 x);

__m128 __stdcall am_exp2_eps(__m128 x);
__m128 __stdcall am_exp2_ess(__m128 x);

__m128 __stdcall am_log_ps(__m128 x);  // needs _m_empty() !!!
__m128 __stdcall am_log_ss(__m128 x);

__m128 __stdcall am_log_eps(__m128 x);
__m128 __stdcall am_log_ess(__m128 x);

__m128 __stdcall am_log2_ps(__m128 x);  // needs _m_empty() !!!
__m128 __stdcall am_log2_ss(__m128 x);

__m128 __stdcall am_log2_eps(__m128 x);
__m128 __stdcall am_log2_ess(__m128 x);

__m128 __stdcall am_pow_ps(__m128 x, __m128 y);  // needs _m_empty() !!!
__m128 __stdcall am_pow_ss(__m128 x, __m128 y);

__m128 __stdcall am_pow_eps(__m128 x, __m128 y);
__m128 __stdcall am_pow_ess(__m128 x, __m128 y);

/////////////////////////////////////////////////////////////////////////////
// Inline implementations

__inline __m128 __stdcall am_sign_ps(__m128 x)
{
	AM_USE_PS_INT_CONST(am_sign_mask);
	return _mm_and_ps(x, am_sign_mask);
}

__inline __m128 __stdcall am_abs_ps(__m128 x)
{
	AM_USE_PS_INT_CONST(am_sign_mask);
	return _mm_andnot_ps(am_sign_mask, x);
}

__inline __m128 __stdcall am_sign_ss(__m128 x)
{
	// and with inverted scalar mask to retain higher elements
	AM_USE_SS_INT_CONST(am_inv_sign_mask);
	return _mm_andnot_ps(am_inv_sign_mask, x);
}

__inline __m128 __stdcall am_abs_ss(__m128 x)
{
	// and with inverted scalar mask to retain higher elements
	AM_USE_SS_INT_CONST(am_sign_mask);
	return _mm_andnot_ps(am_sign_mask, x);
}

__inline __m128 __stdcall am_asin_ps(__m128 x)
{
	AM_USE_PS_CONST(am_1);
	return am_atan_ps(_mm_mul_ps(x, _mm_rsqrt_ps(_mm_mul_ps(_mm_sub_ps(am_1, x), _mm_add_ps(am_1, x)))));
}

__inline __m128 __stdcall am_acos_ps(__m128 x)
{
	AM_USE_PS_CONST(am_1);
	__m128 t = am_atan_ps(_mm_rsqrt_ps(_mm_mul_ps(_mm_add_ps(am_1, x), _mm_rcp_ps(_mm_sub_ps(am_1, x)))));
	return _mm_add_ps(t, t);
}


__inline __m128 __stdcall am_asin_ss(__m128 x)
{
	AM_USE_PS_CONST(am_1);
	return am_atan_ss(_mm_mul_ss(x, _mm_rsqrt_ss(_mm_mul_ss(_mm_sub_ss(am_1, x), _mm_add_ss(am_1, x)))));
}

__inline __m128 __stdcall am_acos_ss(__m128 x)
{
	AM_USE_PS_CONST(am_1);
	__m128 t = am_atan_ss(_mm_rsqrt_ss(_mm_mul_ss(_mm_add_ss(am_1, x), _mm_rcp_ss(_mm_sub_ss(am_1, x)))));
	return _mm_add_ss(t, t);
}


#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // !defined(_AMATHS_H_)

/////////////////////////////////////////////////////////////////////////////
// EOF