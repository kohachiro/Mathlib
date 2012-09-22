#ifndef _VECTOR_H
#define _VECTOR_H

#include <xmmintrin.h>
#include <stdio.h>
#include <assert.h>
#include "Amaths.h"

class  __declspec(dllexport) Vector
{
public:
	union {
		__m128 vec;
		struct { 
			float	x,y,z,w;
		};
	};
	Vector() {}
	Vector(__m128 v) { vec=v; }
	Vector(const Vector& v){ vec=v.vec; }
	Vector(float x, float y, float z,float w=0){vec=_mm_set_ps( w, z , y , x );}
	Vector(float f)	{ vec = _mm_set_ps1(f); }

	/* Explicitly initialize each of 4 SP FPs with same double */
	Vector(double d)	{ vec = _mm_set_ps1((float) d); }

	/* Assignment operations */

	Vector& operator =(float f) { vec = _mm_set_ps1(f); return *this; }

	Vector& operator =(double d) { vec = _mm_set_ps1((float) d); return *this; }

	/* Conversion functions */
	operator  __m128() const	{ return vec; }		/* Convert to __m128 */

 	/* Logical Operators */
	friend Vector operator &(const Vector &a, const Vector &b) { return _mm_and_ps(a,b); }
	friend Vector operator |(const Vector &a, const Vector &b) { return _mm_or_ps(a,b); }
	friend Vector operator ^(const Vector &a, const Vector &b) { return _mm_xor_ps(a,b); }

	//* Arithmetic Operators */
	friend float operator *(const Vector &a, const Vector &b) 
	{
		float f;
		__asm 
		{
			mov		esi,	a
			mov		edi,	b
			movups	xmm0,	[esi]
			movups	xmm1,	[edi]
			mulps	xmm0,	xmm1
			movaps	xmm1,	xmm0
			movhlps	xmm1,	xmm1	;f2,f3,f2,f3
			addps	xmm1,	xmm0	;f2+f0,f3+f1,f2+f2,f3+f3
			movaps	xmm0,	xmm1
			shufps	xmm0,	xmm0,	1
			addss	xmm0,	xmm1
			movss	f,	xmm0
		}
		return f;
	}
	friend Vector operator +(const Vector &a, const Vector &b) { return _mm_add_ps(a,b); }
	friend Vector operator -(const Vector &a, const Vector &b) { return _mm_sub_ps(a,b); }
	//friend Vector operator *(const Vector &a, const Vector &b) { return _mm_mul_ps(a,b); }
	friend Vector operator /(const Vector &a, const Vector &b) { return _mm_div_ps(a,b); }
	friend Vector operator *(const float f, const Vector &b) { return _mm_mul_ps(_mm_set_ps1(f),b); }

	Vector& operator =(const Vector &a) { vec = a.vec; return *this; }
	Vector& operator =(__m128 &a) { vec = a; return *this; }
	Vector& operator +=(const Vector &a) { return *this = _mm_add_ps(vec,a); }
	Vector& operator -=(const Vector &a) { return *this = _mm_sub_ps(vec,a); }
	//Vector& operator *=(const Vector &a) { return *this = _mm_mul_ps(vec,a); }
	Vector& operator /=(const Vector &a) { return *this = _mm_div_ps(vec,a); }
	Vector& operator &=(const Vector &a) { return *this = _mm_and_ps(vec,a); }
	Vector& operator |=(const Vector &a) { return *this = _mm_or_ps(vec,a); }
	Vector& operator ^=(const Vector &a) { return *this = _mm_xor_ps(vec,a); }
	Vector& operator *=(float f) { return *this =_mm_mul_ps(_mm_set_ps1(f),vec); }
	
	float operator *=(const Vector &a) 
	{
		float f;
		__asm 
		{
			mov		esi,	a
			mov		edi,	this
			movups	xmm0,	[esi]
			movups	xmm1,	[edi]
			mulps	xmm0,	xmm1
			movaps	xmm1,	xmm0
			movhlps	xmm1,	xmm1	;f2,f3,f2,f3
			addps	xmm1,	xmm0	;f2+f0,f3+f1,f2+f2,f3+f3
			movaps	xmm0,	xmm1
			shufps	xmm0,	xmm0,	1
			addss	xmm0,	xmm1
			movss	f,	xmm0
		}
		return f;
	}	



	//float& operator() (int i) {
	//	assert((0<=i) && (i<=3));
	//	return *(((float *)&vec) + i);
	//}
	//float& operator[] (int i) {
	//	assert((0<=i) && (i<=3));
	//	return *(((float *)&vec) + i);
	//}
	//float& operator[] (int i) const {
	//	assert((0<=i) && (i<=3));
	//	return *(((float *)&vec) + i);
	//}

	inline void	zero() { vec=_mm_setzero_ps();}
	inline void	wZero() 
	{
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	[esi]
			movups	xmm0,	_ps_am_0
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			movups	[esi],	xmm7
		}
	}
	inline void	wOne() 
	{
		__asm 
		{
			mov		esi,	this
			movups	xmm7,	[esi]
			movups	xmm0,	_ps_am_1
			shufps	xmm7,	xmm7,	147	;2,1,0,3
			movss	xmm7,	xmm0
			shufps	xmm7,	xmm7,	57	;0,3,2,1
			movups	[esi],	xmm7
		}
	}
	inline void set(float x, float y, float z, float w=0){vec=_mm_set_ps( w ,z , y , x );}
	inline void add(const Vector& vec1, const Vector& vec2){vec = _mm_add_ps(vec1,vec2);}
	inline void sub(const Vector& vec1, const Vector& vec2){vec = _mm_sub_ps(vec1,vec2);}
	inline void difference(const Vector &vec1, const Vector &vec2) {vec = _mm_sub_ps(vec1,vec2);}
	inline void scale(float f, const Vector& v){vec = _mm_mul_ps(_mm_set_ps1(f),v);}
	inline float distance(const Vector& v1)  //legnth form v1
	{
		float f;
		__asm {
			mov esi, v1
			movups xmm1,  [esi]

			mov    esi,   this				; vector u
			movups xmm0,  [esi]				; first vector in xmm0

			subps  xmm0,  xmm1				; sub
			movaps xmm2,  xmm0				; copy original vector
			mulps  xmm0,  xmm0				; mul with 2nd vector
			movaps xmm1,  xmm0				; copy result
			shufps xmm1,  xmm1, 4Eh			; shuffle: f1,f0,f3,f2
			addps  xmm0,  xmm1				; add: f3+f1,f2+f0,f1+f3,f0+f2 
			movaps xmm1,  xmm0				; copy results
			shufps xmm1,  xmm1, 11h			; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
			addps  xmm0,  xmm1				; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
			sqrtss xmm0,  xmm0				; sqroot from least bit value
			movss  f, xmm0					; move result from xmm0 to edi
		}
		return f;
	}
	inline float sqrDistance(const Vector& v1) 
	{
		float f;
		__asm {
			mov esi, v1
			movups xmm1, [esi]

			mov    esi,   this				; vector u
			movups xmm0,  [esi]				; first vector in xmm0

			subps  xmm0,  xmm1				; sub
			movaps xmm2,  xmm0				; copy original vector
			mulps  xmm0,  xmm0				; mul with 2nd vector
			movaps xmm1,  xmm0				; copy result
			shufps xmm1,  xmm1, 4Eh			; shuffle: f1,f0,f3,f2
			addps  xmm0,  xmm1				; add: f3+f1,f2+f0,f1+f3,f0+f2 
			movaps xmm1,  xmm0				; copy results
			shufps xmm1,  xmm1, 11h			; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
			addps  xmm0,  xmm1				; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
			movss  f, xmm0					; move result from xmm0 to edi
		}
		return f;
	}

	inline void normalize()                   // normalize
	{
		__asm {
			 mov    esi,   this            ; vector u
			 movups xmm0,  [esi]           ; first vector in xmm0
			 movaps xmm2,  xmm0            ; copy original vector
			 mulps  xmm0,  xmm0            ; mul with 2nd vector
			 movaps xmm1,  xmm0            ; copy result
			 shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
			 addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
			 movaps xmm1,  xmm0            ; copy results
			 shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
			 addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
			 rsqrtps xmm0,  xmm0           ; recip. sqrt (faster than ss + shufps)
			 mulps   xmm2,  xmm0           ; mul by reciprocal
			 movups  [esi], xmm2           ; bring back result
		}
	}
	inline float length()
	{
		float f;
		__asm {
			 mov    esi,   this            ; vector u
			 movups xmm0,  [esi]           ; first vector in xmm0	0,1,2,3
			 mulps  xmm0,  xmm0            ; mul with 2nd vector 
			 movaps xmm1,  xmm0            ; copy result	
			 shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
			 addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
			 movaps xmm1,  xmm0            ; copy results
			 shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3  0 0 2 3
			 addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
			 sqrtss xmm0,  xmm0            ; sqroot from least bit value
			 movss  f, xmm0                ; move result from xmm0 to edi
         }
		return f;
	}
	inline float sqrLength()           // square length
	{
		float f;
		__asm {
			 mov    esi,   this            ; vector u
			 movups xmm0,  [esi]           ; first vector in xmm0
			 mulps  xmm0,  xmm0            ; mul with 2nd vector
			 movaps xmm1,  xmm0            ; copy result
			 shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
			 addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
			 movaps xmm1,  xmm0            ; copy results
			 shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
			 addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
			 movss  f, xmm0                ; move result from xmm0 to edi
         }
		return f;
	}
	inline void  negate()                      // vector mult -1
	{
		__asm {
			mov		esi,	this				; vector u
			movups	xmm0,	[esi]				; first vector in xmm0
			movups	xmm1,	_ps_am_0			;
			subps	xmm1,	xmm0				;
			movups	[esi],	xmm1				; bring back result
		}
	}
	inline Vector cross(const Vector &v1,const Vector &v2)//²æ³Ë
	{
		__m128 m;
		_asm{
			mov esi, v1
			mov edi, v2
			movups	xmm0, [esi]
			movups	xmm1, [edi]
			movaps	xmm2, xmm0
			movaps	xmm3, xmm1
			shufps	xmm0, xmm0, 0xC9		; 3 , 0 , 2 , 1
			shufps	xmm1, xmm1, 0xD2		; 3 , 1 , 0 , 2
			shufps	xmm2, xmm2, 0xD2		; 3 , 1 , 0 , 2
			shufps	xmm3, xmm3, 0xC9		; 3 , 0 , 2 , 1
			mulps	xmm0, xmm1
			mulps	xmm2, xmm3			
			subps	xmm2, xmm0
			movaps	m,	xmm2
		}
		return Vector(m);
	}
	inline void  print()
	{
		float f[4];
		_mm_store_ps(&f[0],vec);
		printf("%.6f\t%.6f\t%.6f\t%.6f\n",f[0],f[1],f[2],f[3]);
	}
	inline float angleWith( Vector &v )          // angle in rad
	{
		float f;
		_mm_store_ss(&f,am_acos_ss(_mm_set_ss((*this)*v/this->length()*v.length())));
		return f;		
	}
};  // class
#endif