#ifndef _MATRIX_H
#define _MATRIX_H

#include <xmmintrin.h>
#include <stdio.h>
#include <assert.h>
#include "Vector.h"
#include "Amaths.h"

extern const __m128 _MASKSIGN_;

class  __declspec(dllexport) Matrix
{
public:
	union {
		struct {
			__m128 m0, m1, m2, m3;
		};
		struct {
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
	};
	Matrix()
	{
		m0 = _mm_set_ps( 0.0,0.2,0.1,0.0);
		m1 = _mm_set_ps( 0.0,1.2,1.1,1.0);
		m2 = _mm_set_ps( 0.0,2.2,2.1,2.0);
		m3 = _mm_set_ps( 0.0,3.2,3.1,3.0);
	}
	Matrix(	float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44	)
	{
		m0 = _mm_set_ps( f14,f13,f12,f11);
		m1 = _mm_set_ps( f24,f23,f22,f21);
		m2 = _mm_set_ps( f34,f33,f32,f31);
		m3 = _mm_set_ps( f44,f43,f42,f41);
	}
	//float& operator() (int i, int j) {
	//	assert((0<=i) && (i<=3) && (0<=j) && (j<=3));
	//	return *(((float *)&_11) + (i<<2)+j);
	//}
	//F32vec4& operator() (int i) {
	//	assert((0<=i) && (i<=3));
	//	return *(((F32vec4 *)&_11) + i);
	//}
	//F32vec4& operator[] (int i) {
	//	assert((0<=i) && (i<=3));
	//	return *(((F32vec4 *)&_11) + i);
	//}
	//F32vec4& operator[] (int i) const {
	//	assert((0<=i) && (i<=3));
	//	return *(((F32vec4 *)&_11) + i);
	//}
// ----------------------------------------------------------------------------
//  Name:	identity
//  Desc:	Returns the Identity Matrix.
// ----------------------------------------------------------------------------
	inline void identity()//identity
	{
		m0 = _mm_set_ss( 1);
		m1 = _mm_set_ps( 0 , 0 , 1 , 0 );
		m2 = _mm_set_ps( 0 , 1 , 0 , 0 );
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );
	}
// ----------------------------------------------------------------------------
//  Name:	Zero
//  Desc:	Sets all elements in [Res] to zero.
// ----------------------------------------------------------------------------
	inline void zero()//fill zero
	{
		m0 = m1 = m2 = m3 = _mm_setzero_ps();
	}
	inline void zeroTranslation()//fill m3
	{
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );
	}
	inline void setTranslation(const Vector &v)
	{
		__asm 
		{
			mov		edi,	v
			movups	xmm7,	[edi]
			movups	xmm0,	_ps_am_1
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			mov		esi,	this
			movups	[esi+48],	xmm7
		}
	}
	inline Vector getTranslation()
	{
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	[esi+48]
			movups	xmm0,	_ps_am_0
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			movups	[esi+48],	xmm7
		}
		return Vector(m3); 
	}
	inline void setupTranslation(const Vector &v)//Translation identity matrix
	{
		m0 = _mm_set_ss( 1);
		m1 = _mm_set_ps( 0 , 0 , 1 , 0 );
		m2 = _mm_set_ps( 0 , 1 , 0 , 0 );
		__asm 
		{
			mov		edi,	v
			movups	xmm7,	[edi]
			movups	xmm0,	_ps_am_1
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			mov		esi,	this
			movups	[esi+48],	xmm7
		}
	}
	inline void scale(const Vector &v)
	{
		__asm
		{
			mov		edi,	v
			movups	xmm7,	[edi]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	252	;3,3,3,0
			shufps	xmm6,	xmm6,	247	;3,3,1,3
			shufps	xmm7,	xmm7,	239	;3,2,3,3
			mov		esi,	this
			movups	[esi],	xmm5
			movups	[esi+16],	xmm6
			movups	[esi+32],	xmm7
		}
	}
	inline void scale(const float f)
	{
		m0 = _mm_set_ps( 0.0,0.0,0.0,f);
		m1 = _mm_set_ps( 0.0,0.0,f,0.0);
		m2 = _mm_set_ps( 0.0,f,0.0,0.0);
		m3 = _mm_set_ps( 1.0,0.0,0.0,0.0);
	}
// ----------------------------------------------------------------------------
//  Name:	Transpose                                     t
//  Desc:	Transpose the 4x4 matrix. Matrix is set to [M] .
// ----------------------------------------------------------------------------
	inline void transpose() {
		//__m128	xmm0 = _mm_unpacklo_ps(m0,m1),
		//		xmm1 = _mm_unpacklo_ps(m2,m3),
		//		xmm2 = _mm_unpackhi_ps(m0,m1),
		//		xmm3 = _mm_unpackhi_ps(m2,m3);
		//m0 = _mm_movelh_ps(xmm0,xmm1);
		//m1 = _mm_movehl_ps(xmm1,xmm0);
		//m2 = _mm_movelh_ps(xmm2,xmm3);
		//m3 = _mm_movehl_ps(xmm3,xmm2);
		_asm
		{
			mov		esi,	this
			movups	xmm7,	[esi+48]
			movups	xmm6,	[esi+32]
			movups	xmm5,	[esi+16]
			movups	xmm4,	[esi]
			unpcklps	xmm4,	xmm5
			unpcklps	xmm6,	xmm7
			movaps	xmm0,	xmm4
			movaps	xmm1,	xmm6
			unpckhps	xmm4,	xmm5
			unpckhps	xmm6,	xmm7
			movaps	xmm2,	xmm0
			movaps	xmm3,	xmm4
			movlhps	xmm2,	xmm1
			movhlps xmm1,	xmm0
			movhlps xmm3,	xmm6
			movlhps xmm6,	xmm4
			movups	[esi],	xmm2
			movups	[esi+16],	xmm1
			movups	[esi+32],	xmm3
			movups	[esi+48],	xmm6
		}
	}
	inline void cross(const Vector &v)
	{
		__asm
		{
			mov		esi,	v
			mov		edi,	this
			movups	xmm0,	[esi]
			movups	xmm7,	_ps_am_0
			movaps	xmm1,	xmm0
			movaps	xmm2,	xmm0
			movaps	xmm3,	xmm0
			movaps	xmm4,	xmm0
			shufps	xmm0,	xmm0,	211	;3,1,0,3
			shufps	xmm1,	xmm1,	254	;3,3,3,2
			shufps	xmm2,	xmm2,	207	;3,0,3,3
			shufps	xmm3,	xmm3,	253	;3,3,3,1
			shufps	xmm4,	xmm4,	243	;3,3,0,3
			movaps	xmm5,	xmm7
			movaps	xmm6,	xmm7
			subps	xmm7,	xmm0
			subps	xmm6,	xmm2
			subps	xmm5,	xmm3
			addps	xmm6,	xmm1
			addps	xmm5,	xmm4
			movups [esi],	xmm7
			movups [esi+16],	xmm6
			movups [esi+32],	xmm5
		}
		//m0 = _mm_set_ps( 0 , -v.vec.m128_f32[0] , -v.vec.m128_f32[1] , 0 );
		//m1 = _mm_set_ps( v.vec.m128_f32[2] , 0 , -v.vec.m128_f32[0] , 0 );
		//m2 = _mm_set_ps( -v.vec.m128_f32[1] , v.vec.m128_f32[0] , 0 , 0 );
		//m3 = _mm_set_ps( 0 , 0 , 0 , 0 );
	}
	inline void rotaX(float f)                       // x axis
	{
		//m0{1,0,0,0}
		//m1{0,c,s,0}
		//m2{0,-s,c,0}
		//m3{0,0,0,1}
		__m128 sin,cos;
		am_sincos_ess( _mm_set_ss(f),&sin,&cos );//m3{c,0,0,0}m0{s,0,0,0}
		m0 = _mm_set_ss( 1 );
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	cos
			movups	xmm6,	sin
			movups	xmm5,	_ps_am_0
			subps	xmm5,	xmm6
			shufps	xmm5,	xmm5,	147	;2,1,0,3
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	207	;3,0,3,3
			addps	xmm5,	xmm4
			movups	[esi+32],	xmm5

			shufps	xmm7,	xmm7,	147	;2,1,0,3
			shufps	xmm6,	xmm6,	207	;3,0,3,3
			addps	xmm7,	xmm6
			movups	[esi+16],	xmm7

		}
	}
	inline void rotaY(float f)                       // y axis
	{
	//m0{c,0,-s,0}
	//m1{0,1,0,0}
	//m2{s,0,c,0}
	//m3{0,0,0,1}
		__m128 sin,cos;
		am_sincos_ess( _mm_set_ss(f),&sin,&cos );//m3{c,0,0,0}m0{s,0,0,0}
		m1 = _mm_set_ps( 0 , 0 , 1 , 0 );
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	cos
			movups	xmm6,	sin
			movups	xmm5,	_ps_am_0
			subps	xmm5,	xmm6
			shufps	xmm5,	xmm5,	207	;3,0,3,3
			addps	xmm5,	xmm7
			movups	[esi],	xmm5
			shufps	xmm7,	xmm7,	207	;3,0,3,3
			addps	xmm7,	xmm6
			movups	[esi+32],	xmm7
		}

	}
	inline void rotaZ(float f)                       // z axis
	{
	//m0{c,s,0,0}
	//m1{-s,c,0,0}
	//m2{0,0,1,0}
	//m3{0,0,0,1}
		__m128 sin,cos;
		am_sincos_ess( _mm_set_ss(f),&sin,&cos );//m3{c,0,0,0}m0{s,0,0,0}
		m2 = _mm_set_ps( 0 , 1 , 0 , 0 );//m3{0,0,1,0}
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );//m3{0,0,0,1}
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	cos
			movups	xmm6,	sin
			movups	xmm5,	_ps_am_0
			subps	xmm5,	xmm6
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	147	;2,1,0,3
			addps	xmm5,	xmm4
			movups	[esi+16],	xmm5
			shufps	xmm6,	xmm6,	147	;2,1,0,3
			addps	xmm7,	xmm6
			movups	[esi],	xmm7
		}
	}
	inline void rota(const Vector &vc)
	{		
		__m128 sin,cos;
		am_sincos_eps( vc.vec,&sin,&cos );//m0{sx,sy,sz,0} m1{cx,cy,cz,0}
		m3 = _mm_set_ps( 1 , 0 , 0 , 0 );
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	cos
			movups	xmm6,	sin
			movups	xmm0,	_ps_am_0
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	254	;3,3,3,2
			movaps	xmm4,	xmm6
			shufps	xmm4,	xmm4,	251	;3,3,2,3
			addps	xmm5,	xmm4
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	245	;3,3,1,1
			mulps	xmm5,	xmm4
			movaps	xmm3,	xmm6
			shufps	xmm3,	xmm3,	223	;3,1,3,3
			movups	xmm4,	xmm0
			subps	xmm4,	xmm3
			addps	xmm5,	xmm4
			movups	[esi],	xmm5
			
			movaps	xmm5,	xmm6
			shufps	xmm5,	xmm5,	240	;3,3,0,0
			movaps	xmm4,	xmm6
			shufps	xmm4,	xmm4,	245	;3,3,1,1
			mulps	xmm5,	xmm4
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	254	;3,3,3,2
			movaps	xmm3,	xmm6
			shufps	xmm3,	xmm3,	251	;3,3,2,3
			addps	xmm4,	xmm3
			mulps	xmm5,	xmm4
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	240	;3,3,0,0
			movaps	xmm3,	xmm7
			shufps	xmm3,	xmm3,	223	;3,1,3,3
			addps	xmm4,	xmm3
			movaps	xmm2,	xmm6
			shufps	xmm2,	xmm2,	254	;3,3,3,2
			movups	xmm3,	xmm0
			subps	xmm3,	xmm2
			movaps	xmm2,	xmm6
			shufps	xmm2,	xmm2,	207	;3,0,3,3
			movaps	xmm1,	xmm7
			shufps	xmm1,	xmm1,	251	;3,3,2,3
			addps	xmm2,	xmm1
			addps	xmm3,	xmm2
			mulps	xmm4,	xmm3
			addps	xmm5,	xmm4
			movups	[esi+16],	xmm5

			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	240	;3,3,0,0
			movaps	xmm4,	xmm6
			shufps	xmm4,	xmm4,	245	;3,3,1,1
			mulps	xmm5,	xmm4
			movaps	xmm4,	xmm7
			shufps	xmm4,	xmm4,	254	;3,3,3,2
			movaps	xmm3,	xmm6
			shufps	xmm3,	xmm3,	251	;3,3,2,3
			addps	xmm4,	xmm3
			mulps	xmm5,	xmm4
			movaps	xmm4,	xmm6
			shufps	xmm4,	xmm4,	240	;3,3,0,0
			movaps	xmm3,	xmm7
			shufps	xmm3,	xmm3,	223	;3,1,3,3
			addps	xmm4,	xmm3
			movaps	xmm2,	xmm7
			shufps	xmm2,	xmm2,	251	;3,3,2,3
			movups	xmm3,	xmm0
			subps	xmm3,	xmm2
			movaps	xmm2,	xmm6
			shufps	xmm2,	xmm2,	254	;3,3,3,2
			movaps	xmm1,	xmm7
			shufps	xmm1,	xmm1,	207	;3,0,3,3
			addps	xmm2,	xmm1
			addps	xmm3,	xmm2
			mulps	xmm4,	xmm3
			addps	xmm5,	xmm4
			movups	[esi+32],	xmm5
		}
	//cy*cz,cy*sz,-sy,0
	//sx*sy *cz-cx*sz,sx*sy *sz+cx*cz,sx*cy,0
	//cx*sy *cz+sx*sz,cx*sy *sz-sx*cz,cx*cy,0
	//0,0,0,1	
	}
	inline void rota(float x, float y, float z)
	{
		rota(Vector(x, y, z));
	}

//	inline void RotaArbi(const Vector &vcAxis, float a);
//	inline void applyInverseRota(Vector *pvc)
//	{
//		__m128 m0,m1,m2,m3;
//		m3 = _mm_move_ss( _mm_shuffle_ps( pvc->vec , pvc->vec , _MM_SHUFFLE( 3 , 0 , 1 , 2 )) , _mm_setzero_ps());
//		m3 = _mm_shuffle_ps( m3 , m3 , _MM_SHUFFLE( 1 , 2 , 3 , 0 ));
//		m0 = _mm_mul_ps( m3 , m0 );
//		m1 = _mm_mul_ps( m3 , m1 );
//		m2 = _mm_mul_ps( m3 , m2 );
//		m3 = _mm_add_ps(_mm_shuffle_ps( m0 , m0 , _MM_SHUFFLE( 0 , 3 , 3 , 3 )),
//						_mm_shuffle_ps( m0 , m0 , _MM_SHUFFLE( 1 , 3 , 3 , 3 )));
//		m3 = _mm_add_ps(_mm_shuffle_ps( m0 , m0 , _MM_SHUFFLE( 2 , 3 , 3 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m1 , m1 , _MM_SHUFFLE( 3 , 0 , 3 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m1 , m1 , _MM_SHUFFLE( 3 , 1 , 3 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m1 , m1 , _MM_SHUFFLE( 3 , 2 , 3 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m2 , m2 , _MM_SHUFFLE( 3 , 3 , 0 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m2 , m2 , _MM_SHUFFLE( 3 , 3 , 1 , 3 )),m3);
//		m3 = _mm_add_ps(_mm_shuffle_ps( m2 , m2 , _MM_SHUFFLE( 3 , 3 , 2 , 3 )),m3);
//		pvc->vec = _mm_add_ps( _mm_set_ps( 0 , 0 , 0 , 1 ),m3);
//	}
//	// Build rotation matrix around arbitrary axis
//	//inline void RotaArbi(const Vector &_vcAxis, float a){}
//
//	//inline void Billboard(Vector vcPos, Vector vcDir,Vector vcWorldUp = Vector(0,1,0)){}
//	//inline void LookAt(Vector vcPos, Vector vcLookAt,Vector vcWorldUp = Vector(0,1,0)){}
//
//	//inline void TransposeOf(const Matrix &m);       // transpose m, save result in this
// //     
	inline Matrix& operator= (const Matrix &a) {
		m0 = a.m0;
		m1 = a.m1;
		m2 = a.m2;
		m3 = a.m3;
		return *this;
	}

// ----------------------------------------------------------------------------
//  Name:	Vector * Matrix                     ___     ___
//  Desc:	Vector multiplication with matrix. [Res] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
	inline friend Vector operator * (const Vector &v, const Matrix &m)
	{
		//float v0 = elts[0][0]*v[0]+elts[0][1]*v[1]+elts[0][2]*v[2]+elts[0][3]*v[3];
		//float v1 = elts[1][0]*v[0]+elts[1][1]*v[1]+elts[1][2]*v[2]+elts[1][3]*v[3];
		//float v2 = elts[2][0]*v[0]+elts[2][1]*v[1]+elts[2][2]*v[2]+elts[2][3]*v[3];
		//float v3 = elts[3][0]*v[0]+elts[3][1]*v[1]+elts[3][2]*v[2]+elts[3][3]*v[3];
		__m128 r;
		__asm 
		{
			mov		edi,	v
			mov		esi,	m
			movups	xmm3,	[edi]

			movups	xmm0,	_ps_am_1
			shufps	xmm3,	xmm3,	147	;2,1,0,3
			movss	xmm3,	xmm0
			shufps	xmm3,	xmm3,	57	;0,3,2,1

			movups	xmm7,	[esi+48]
			movups	xmm6,	[esi+32]
			movups	xmm5,	[esi+16]
			movups	xmm4,	[esi]

			movaps	xmm2,	xmm3
			movaps	xmm1,	xmm3
			movaps	xmm0,	xmm3

			shufps	xmm0,	xmm0,	0	;0,0,0,0
			shufps	xmm1,	xmm1,	85	;1,1,1,1
			shufps	xmm2,	xmm2,	170	;2,2,2,2
			shufps	xmm3,	xmm3,	255	;3,3,3,3

			mulps	xmm7,	xmm3
			mulps	xmm6,	xmm2
			mulps	xmm5,	xmm1
			mulps	xmm4,	xmm0

			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			addps	xmm7,	xmm4			
			movups	r,	xmm7
		}
		return Vector(r);
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix * Matrix
//  Desc:	Matrix multiplication of A and B. Returns [A]*[B].
// ----------------------------------------------------------------------------
	inline friend Matrix operator * (const Matrix &a, const Matrix &b)
	{
		Matrix c;
		__asm 
		{
			mov		esi,	b
			mov		edi,	a
			movups	xmm0,	[esi]
			movups	xmm1,	[esi+16]
			movups	xmm2,	[esi+32]
			movups	xmm3,	[esi+48]
			mov		esi,	c

			movups	xmm7,	[edi]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi],	xmm7

			movups	xmm7,	[edi+16]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi+16],	xmm7

			movups	xmm7,	[edi+32]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi+32],	xmm7

			movups	xmm7,	[edi+48]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			addps	xmm7,	xmm3
			movups	[esi+48],	xmm7

		}
		return c;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix + Matrix
//  Desc:	Matrix elements addition between A and B. Returns [A]+[B].
// ----------------------------------------------------------------------------
	inline friend Matrix operator + (const Matrix &a, const Matrix &b)
	{
		Matrix Res;
		Res.m0 = _mm_add_ps(a.m0,b.m0);
		Res.m1 = _mm_add_ps(a.m1,b.m1);
		Res.m2 = _mm_add_ps(a.m2,b.m2);
		Res.m3 = _mm_add_ps(a.m3,b.m3);
		return Res;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix - Matrix
//  Desc:	Matrix elements substraction between A and B. Returns [A]-[B].
// ----------------------------------------------------------------------------
	inline friend Matrix operator - (const Matrix&a, const Matrix&b)
	{
		Matrix Res;
		Res.m0 = _mm_sub_ps(a.m0,b.m0);
		Res.m1 = _mm_sub_ps(a.m1,b.m1);
		Res.m2 = _mm_sub_ps(a.m2,b.m2);
		Res.m3 = _mm_sub_ps(a.m3,b.m3);
		return Res;
	}
// ----------------------------------------------------------------------------
//  Name:	+Matrix (Unary plus)
//  Desc:	Returns the same matrix [A].
// ----------------------------------------------------------------------------
	inline friend Matrix operator + (const Matrix&a)
	{
		return a;
	}
// ----------------------------------------------------------------------------
//  Name:	-Matrix (Unary minus)
//  Desc:	Matrix elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
	inline friend Matrix operator - (const Matrix&a)
	{
		Matrix Res;
		Res.m0 = _mm_xor_ps(_MASKSIGN_,a.m0);
		Res.m1 = _mm_xor_ps(_MASKSIGN_,a.m1);
		Res.m2 = _mm_xor_ps(_MASKSIGN_,a.m2);
		Res.m3 = _mm_xor_ps(_MASKSIGN_,a.m3);
		return Res;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix * float
//  Desc:	Matrix elements multiplication with scalar. Returns [A]*f.
// ----------------------------------------------------------------------------
	inline friend Matrix operator * (const Matrix&a, const float f)
	{
		Matrix Res;
		Res.m0 = _mm_mul_ps( a.m0 , _mm_set_ps1(f));
		Res.m1 = _mm_mul_ps( a.m1 , _mm_set_ps1(f));
		Res.m2 = _mm_mul_ps( a.m2 , _mm_set_ps1(f));
		Res.m3 = _mm_mul_ps( a.m3 , _mm_set_ps1(f));
		return Res;
	}
	inline friend Matrix operator * (const float f, const Matrix&a)
	{
		Matrix Res;
		Res.m0 = _mm_mul_ps( a.m0 , _mm_set_ps1(f));
		Res.m1 = _mm_mul_ps( a.m1 , _mm_set_ps1(f));
		Res.m2 = _mm_mul_ps( a.m2 , _mm_set_ps1(f));
		Res.m3 = _mm_mul_ps( a.m3 , _mm_set_ps1(f));
		return Res;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix *= Matrix
//  Desc:	Matrix multiplication of A by B. [A] = [A]*[B].
// ----------------------------------------------------------------------------
	inline Matrix& operator *= (const Matrix &a) 
	{
		/*r.m11=a.m11*b.m11+a.m12*b.m21+a.m13*b.m31;
		r.m12=a.m11*b.m12+a.m12*b.m22+a.m13*b.m32;
		r.m13=a.m11*b.m13+a.m12*b.m23+a.m13*b.m33;
		r.m21=a.m21*b.m11+a.m22*b.m21+a.m23*b.m31;
		r.m22=a.m21*b.m12+a.m22*b.m22+a.m23*b.m32;
		r.m23=a.m21*b.m13+a.m22*b.m23+a.m23*b.m33;
		r.m31=a.m31*b.m11+a.m32*b.m21+a.m33*b.m31;
		r.m32=a.m31*b.m12+a.m32*b.m22+a.m33*b.m32;
		r.m33=a.m31*b.m13+a.m32*b.m23+a.m33*b.m33;
		r.m41=a.m41*b.m11+a.m42*b.m21+a.m43*b.m31 + b.m41;
		r.m42=a.m41*b.m12+a.m42*b.m22+a.m43*b.m32 + b.m42;
		r.m43=a.m41*b.m13+a.m42*b.m23+a.m43*b.m33 + b.m43;*/
		__asm 
		{
			mov		esi,	this
			mov		edi,	a
			movups	xmm0,	[esi]
			movups	xmm1,	[esi+16]
			movups	xmm2,	[esi+32]
			movups	xmm3,	[esi+48]

			movups	xmm7,	[edi]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi],	xmm7

			movups	xmm7,	[edi+16]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi+16],	xmm7

			movups	xmm7,	[edi+32]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movups	[esi+32],	xmm7

			movups	xmm7,	[edi+48]
			movaps	xmm6,	xmm7
			movaps	xmm5,	xmm7
			shufps	xmm5,	xmm5,	0	;0,0,0,0
			shufps	xmm6,	xmm6,	85	;1,1,1,1
			shufps	xmm7,	xmm7,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			addps	xmm7,	xmm3
			movups	[esi+48],	xmm7

		}
		return *this;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix *= float
//  Desc:	Matrix elements multiplication by scalar. [A] = [A]*f.
// ----------------------------------------------------------------------------
	inline Matrix & operator *= (const float f)
	{
		m0 = _mm_mul_ps(m0,_mm_set_ps1(f));
		m1 = _mm_mul_ps(m1,_mm_set_ps1(f));
		m2 = _mm_mul_ps(m2,_mm_set_ps1(f));
		m3 = _mm_mul_ps(m3,_mm_set_ps1(f));
		return *this;	
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix += Matrix
//  Desc:	Matrix addition of A by B. [A] = [A]+[B].
// ----------------------------------------------------------------------------
	inline Matrix & operator += (const Matrix &a)
	{
		m0 = _mm_add_ps(m0,a.m0);
		m1 = _mm_add_ps(m1,a.m1);
		m2 = _mm_add_ps(m2,a.m2);
		m3 = _mm_add_ps(m3,a.m3);
		return *this;
	}
// ----------------------------------------------------------------------------
//  Name:	Matrix -= Matrix
//  Desc:	Matrix substraction of A by B. [A] = [A]-[B].
// ----------------------------------------------------------------------------
	inline Matrix & operator -= (const Matrix &a)
	{
		m0 = _mm_sub_ps(m0,a.m0);
		m1 = _mm_sub_ps(m1,a.m1);
		m2 = _mm_sub_ps(m2,a.m2);
		m3 = _mm_sub_ps(m3,a.m3);
		return *this;
	}

// ----------------------------------------------------------------------------
//  Name:	Vector RotateWith(Vector)
//  Desc:	
// ----------------------------------------------------------------------------
	inline Vector RotateWith(const Vector &v) 
	{   // applying rotational part of matrix only
		//float _x = x * m._11 + y * m._21 + z * m._31;
		//float _y = x * m._12 + y * m._22 + z * m._32;
		//float _z = x * m._13 + y * m._23 + z * m._33;
		//x = _x;   y = _y;   z = _z;
		__m128 m;
		_asm
		{
			mov esi, v
			mov edi, this
			movups	xmm0,	[esi]
			movups	xmm7,	[edi]
			movups	xmm6,	[edi+16]
			movups	xmm5,	[edi+32]
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm0
			mulps	xmm5,	xmm0
			movaps	xmm0,	xmm7
			movaps	xmm1,	xmm7
			shufps	xmm0,	xmm0,	147	;3,3,3,1
			shufps	xmm1,	xmm1,	147	;3,3,1,3
			shufps	xmm7,	xmm7,	147	;3,1,3,3
			addps	xmm7,	xmm1
			addps	xmm7,	xmm0
			
			movaps	xmm0,	xmm6
			movaps	xmm1,	xmm6
			shufps	xmm0,	xmm0,	147	;3,3,3,1
			shufps	xmm1,	xmm1,	147	;3,3,1,3
			shufps	xmm6,	xmm6,	147	;3,1,3,3
			addps	xmm7,	xmm1
			addps	xmm7,	xmm0
			addps	xmm7,	xmm6

			movaps	xmm0,	xmm5
			movaps	xmm1,	xmm5
			shufps	xmm0,	xmm0,	147	;3,3,3,1
			shufps	xmm1,	xmm1,	147	;3,3,1,3
			shufps	xmm5,	xmm5,	147	;3,1,3,3
			addps	xmm7,	xmm1
			addps	xmm7,	xmm0
			addps	xmm7,	xmm5
			movaps	m,	xmm7
		}
		return Vector(m);
	}
	inline Vector InvRotateWith(const Vector &v) 
	{ //rotate the vector with the inverse rotation part of the given  matrix, ignoring translation of the matrix if present.
		//float _x = x * m._11 + y * m._12 + z * m._13;
		//float _y = x * m._21 + y * m._22 + z * m._23;
		//float _z = x * m._31 + y * m._32 + z * m._33;
		//x = _x;   y = _y;   z = _z;
		__m128 m;
		_asm
		{
			mov esi, v
			mov edi, this
			movups	xmm0,	[esi]
			movups	xmm7,	[edi]
			movups	xmm6,	[edi+16]
			movups	xmm5,	[edi+32]
			movaps	xmm1,	xmm0
			movaps	xmm2,	xmm0
			shufps	xmm0,	xmm0,	0	;0,0,0,0
			shufps	xmm1,	xmm1,	85	;1,1,1,1
			shufps	xmm2,	xmm2,	170	;2,2,2,2
			mulps	xmm7,	xmm0
			mulps	xmm6,	xmm1
			mulps	xmm5,	xmm2
			addps	xmm7,	xmm6
			addps	xmm7,	xmm5
			movaps	m,	xmm7
		}
		return Vector(m);
	}
	inline void  print()
	{
		float f[4];
		_mm_store_ps(&f[0],m0);
		printf("%.6f\t%.6f\t%.6f\t%.6f\n",f[0],f[1],f[2],f[3]);
		_mm_store_ps(&f[0],m1);
		printf("%.6f\t%.6f\t%.6f\t%.6f\n",f[0],f[1],f[2],f[3]);
		_mm_store_ps(&f[0],m2);
		printf("%.6f\t%.6f\t%.6f\t%.6f\n",f[0],f[1],f[2],f[3]);
		_mm_store_ps(&f[0],m3);
		printf("%.6f\t%.6f\t%.6f\t%.6f\n",f[0],f[1],f[2],f[3]);
	}
};  // class
#endif