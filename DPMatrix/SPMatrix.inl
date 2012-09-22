//////////////////////////////////////////////////////////////////////////////
//
// SPMATRIX.INL
//
//  Implementation of the SPMatrix, SPVector and the SPVector3 classes.
//
//  Version 1.0
//  Written by Zvi Devir, Intel MSL
//
//////////////////////////////////////////////////////////////////////////////

//
//   Copyright (c) 2001 Intel Corporation.
//
// Permition is granted to use, copy, distribute and prepare derivative works 
// of this library for any purpose and without fee, provided, that the above 
// copyright notice and this statement appear in all copies.  
// Intel makes no representations about the suitability of this library for 
// any purpose, and specifically disclaims all warranties. 
// See LEGAL.TXT for all the legal information.
//


extern const float __1000_[4];
extern const __int32 __0FFF_[4];
extern const float __ZERONE_[4];
#define _0FFF_      (*(F32vec4*)&__0FFF_)           // 0 * * *
#define _ZERONE_    (*(F32vec4*)&__ZERONE_)         // 1 0 0 1
#define _1000_      (*(F32vec4*)&__1000_)           // 1 0 0 0


            /***********************************************************/
            /*                Matrix Constructors                      */
            /***********************************************************/

inline SPMatrix::SPMatrix(  float f11, float f12, float f13, float f14,
                            float f21, float f22, float f23, float f24,
                            float f31, float f32, float f33, float f34,
                            float f41, float f42, float f43, float f44  )
{
    _L1 = F32vec4(f14, f13, f12, f11);
    _L2 = F32vec4(f24, f23, f22, f21);
    _L3 = F32vec4(f34, f33, f32, f31);
    _L4 = F32vec4(f44, f43, f42, f41);
}

inline SPMatrix::SPMatrix(const float *arr) 
{
    _L1 = _mm_loadu_ps(arr);
    _L2 = _mm_loadu_ps(arr+4);
    _L3 = _mm_loadu_ps(arr+8);
    _L4 = _mm_loadu_ps(arr+12);
}

// ----------------------------------------------------------------------------
//  Name:   ZeroMatrix
//  Desc:   Sets all elements in [Res] to zero.
// ----------------------------------------------------------------------------
#ifndef MATRICES_H__
inline SPMatrix ZeroMatrix() {
    SPMatrix res;
    res._L1 = res._L2 = res._L3 = res._L4 = _mm_setzero_ps();
    return res;
}
#endif
inline void SPMatrix::ZeroMatrix() {
    _L1 = _L2 = _L3 = _L4 = _mm_setzero_ps();
}

// ----------------------------------------------------------------------------
//  Name:   IdentityMatrix
//  Desc:   Returns the Identity Matrix.
// ----------------------------------------------------------------------------
#ifndef MATRICES_H__
inline SPMatrix IdentityMatrix() {
    SPMatrix res;
    __m128 zerone = _ZERONE_;
    _mm_storel_pi((__m64 *)&res._11, zerone);
    _mm_storel_pi((__m64 *)&res._13, _mm_setzero_ps());
    _mm_storeh_pi((__m64 *)&res._21, zerone);
    _mm_storel_pi((__m64 *)&res._23, _mm_setzero_ps());
    _mm_storel_pi((__m64 *)&res._31, _mm_setzero_ps());
    _mm_storel_pi((__m64 *)&res._33, zerone);
    _mm_storel_pi((__m64 *)&res._41, _mm_setzero_ps());
    _mm_storeh_pi((__m64 *)&res._43, zerone);
    return res;
}
#endif
inline void SPMatrix::IdentityMatrix() {
    __m128 zerone = _ZERONE_;
    _mm_storel_pi((__m64 *)&_11, zerone);
    _mm_storel_pi((__m64 *)&_13, _mm_setzero_ps());
    _mm_storeh_pi((__m64 *)&_21, zerone);
    _mm_storel_pi((__m64 *)&_23, _mm_setzero_ps());
    _mm_storel_pi((__m64 *)&_31, _mm_setzero_ps());
    _mm_storel_pi((__m64 *)&_33, zerone);
    _mm_storel_pi((__m64 *)&_41, _mm_setzero_ps());
    _mm_storeh_pi((__m64 *)&_43, zerone);
}


            /***********************************************************/
            /*                     Matrix Operators                    */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   MatrixMult
//  Desc:   Matrix multiplication of A and B. [Res] = [A]*[B].
// ----------------------------------------------------------------------------
inline void MatrixMult(const SPMatrix& A, const SPMatrix& B, SPMatrix& res) {
    F32vec4 r1, r2;
    F32vec4 B1 = B._L1, B2 = B._L2, B3 = B._L3, B4 = B._L4;

    r1 = F32vec4(A._11) * B1;
    r2 = F32vec4(A._21) * B1;
    r1 += F32vec4(A._12) * B2;
    r2 += F32vec4(A._22) * B2;
    r1 += F32vec4(A._13) * B3;
    r2 += F32vec4(A._23) * B3;
    r1 += F32vec4(A._14) * B4;
    r2 += F32vec4(A._24) * B4;
    res._L1 = r1;
    res._L2 = r2;

    r1 = F32vec4(A._31) * B1;
    r2 = F32vec4(A._41) * B1;
    r1 += F32vec4(A._32) * B2;
    r2 += F32vec4(A._42) * B2;
    r1 += F32vec4(A._33) * B3;
    r2 += F32vec4(A._43) * B3;
    r1 += F32vec4(A._34) * B4;
    r2 += F32vec4(A._44) * B4;
    res._L3 = r1;
    res._L4 = r2;
}

// ----------------------------------------------------------------------------
//  Name:   MatrixMult
//  Desc:   Matrix multiplication of A and B. Returns [A]*[B].
// ----------------------------------------------------------------------------
inline SPMatrix MatrixMult(const SPMatrix& A, const SPMatrix& B) {
    SPMatrix res;
    F32vec4 r1, r2;
    F32vec4 B1 = B._L1, B2 = B._L2, B3 = B._L3, B4 = B._L4;

    r1 = F32vec4(A._11) * B1;
    r2 = F32vec4(A._21) * B1;
    r1 += F32vec4(A._12) * B2;
    r2 += F32vec4(A._22) * B2;
    r1 += F32vec4(A._13) * B3;
    r2 += F32vec4(A._23) * B3;
    r1 += F32vec4(A._14) * B4;
    r2 += F32vec4(A._24) * B4;
    res._L1 = r1;
    res._L2 = r2;

    r1 = F32vec4(A._31) * B1;
    r2 = F32vec4(A._41) * B1;
    r1 += F32vec4(A._32) * B2;
    r2 += F32vec4(A._42) * B2;
    r1 += F32vec4(A._33) * B3;
    r2 += F32vec4(A._43) * B3;
    r1 += F32vec4(A._34) * B4;
    r2 += F32vec4(A._44) * B4;
    res._L3 = r1;
    res._L4 = r2;

    return res;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix * SPMatrix
//  Desc:   Matrix multiplication of A and B. Returns [A]*[B].
// ----------------------------------------------------------------------------
inline SPMatrix operator * (const SPMatrix& A, const SPMatrix& B) {
    return MatrixMult(A, B);
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix *= SPMatrix
//  Desc:   Matrix multiplication of A by B. [A] = [A]*[B].
// ----------------------------------------------------------------------------
inline SPMatrix& SPMatrix::operator *= (const SPMatrix& B) {
    F32vec4 r1, r2;
    F32vec4 B1 = B._L1, B2 = B._L2, B3 = B._L3, B4 = B._L4;

    r1 = (F32vec4)_mm_shuffle_ps(_L1,_L1,0x00) * B1;
    r2 = (F32vec4)_mm_shuffle_ps(_L2,_L2,0x00) * B1;
    r1 += (F32vec4)_mm_shuffle_ps(_L1,_L1,0x55) * B2;
    r2 += (F32vec4)_mm_shuffle_ps(_L2,_L2,0x55) * B2;
    r1 += (F32vec4)_mm_shuffle_ps(_L1,_L1,0xAA) * B3;
    r2 += (F32vec4)_mm_shuffle_ps(_L2,_L2,0xAA) * B3;
    r1 += (F32vec4)_mm_shuffle_ps(_L1,_L1,0xFF) * B4;
    r2 += (F32vec4)_mm_shuffle_ps(_L2,_L2,0xFF) * B4;
    _L1 = r1;
    _L2 = r2;

    r1 = (F32vec4)_mm_shuffle_ps(_L3,_L3,0x00) * B1;
    r2 = (F32vec4)_mm_shuffle_ps(_L4,_L4,0x00) * B1;
    r1 += (F32vec4)_mm_shuffle_ps(_L3,_L3,0x55) * B2;
    r2 += (F32vec4)_mm_shuffle_ps(_L4,_L4,0x55) * B2;
    r1 += (F32vec4)_mm_shuffle_ps(_L3,_L3,0xAA) * B3;
    r2 += (F32vec4)_mm_shuffle_ps(_L4,_L4,0xAA) * B3;
    r1 += (F32vec4)_mm_shuffle_ps(_L3,_L3,0xFF) * B4;
    r2 += (F32vec4)_mm_shuffle_ps(_L4,_L4,0xFF) * B4;
    _L3 = r1;
    _L4 = r2;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix * float
//  Desc:   Matrix elements multiplication with scalar. Returns [A]*s.
// ----------------------------------------------------------------------------
inline SPMatrix operator * (const SPMatrix& A, const float s) {
    SPMatrix res;
    F32vec4 S = F32vec4(s);
    res._L1 = A._L1 * S;
    res._L2 = A._L2 * S;
    res._L3 = A._L3 * S;
    res._L4 = A._L4 * S;
    return res;
}
inline SPMatrix operator * (const float s, const SPMatrix& A) {
    SPMatrix res;
    F32vec4 S = F32vec4(s);
    res._L1 = A._L1 * S;
    res._L2 = A._L2 * S;
    res._L3 = A._L3 * S;
    res._L4 = A._L4 * S;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix *= float
//  Desc:   Matrix elements multiplication by scalar. [A] = [A]*s.
// ----------------------------------------------------------------------------
inline SPMatrix& SPMatrix::operator *= (const float s) {
    F32vec4 S = F32vec4(s);
    _L1 = _L1 * S;
    _L2 = _L2 * S;
    _L3 = _L3 * S;
    _L4 = _L4 * S;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix + SPMatrix
//  Desc:   Matrix elements addition between A and B. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline SPMatrix operator + (const SPMatrix& A, const SPMatrix& B) {
    SPMatrix res;
    res._L1 = _mm_add_ps(A._L1, B._L1);
    res._L2 = _mm_add_ps(A._L2, B._L2);
    res._L3 = _mm_add_ps(A._L3, B._L3);
    res._L4 = _mm_add_ps(A._L4, B._L4);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix += SPMatrix
//  Desc:   Matrix addition of A by B. [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline SPMatrix& SPMatrix::operator += (const SPMatrix& B) {
    _L1 = _mm_add_ps(_L1, B._L1);
    _L2 = _mm_add_ps(_L2, B._L2);
    _L3 = _mm_add_ps(_L3, B._L3);
    _L4 = _mm_add_ps(_L4, B._L4);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix - SPMatrix
//  Desc:   Matrix elements substraction between A and B. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline SPMatrix operator - (const SPMatrix& A, const SPMatrix& B) {
    SPMatrix res;
    res._L1 = _mm_sub_ps(A._L1, B._L1);
    res._L2 = _mm_sub_ps(A._L2, B._L2);
    res._L3 = _mm_sub_ps(A._L3, B._L3);
    res._L4 = _mm_sub_ps(A._L4, B._L4);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix -= SPMatrix
//  Desc:   Matrix substraction of A by B. [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline SPMatrix& SPMatrix::operator -= (const SPMatrix& B) {
    _L1 = _mm_sub_ps(_L1, B._L1);
    _L2 = _mm_sub_ps(_L2, B._L1);
    _L3 = _mm_sub_ps(_L3, B._L1);
    _L4 = _mm_sub_ps(_L4, B._L1);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -SPMatrix (Unary minus)
//  Desc:   Matrix elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline SPMatrix operator - (const SPMatrix& A) {
    SPMatrix res;
    __m128 masksign = _MASKSIGNs_;
    res._L1 = _mm_xor_ps(masksign,A._L1);
    res._L2 = _mm_xor_ps(masksign,A._L2);
    res._L3 = _mm_xor_ps(masksign,A._L3);
    res._L4 = _mm_xor_ps(masksign,A._L4);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   +SPMatrix (Unary plus)
//  Desc:   Returns the same matrix [A].
// ----------------------------------------------------------------------------
inline SPMatrix operator + (const SPMatrix& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix::Transpose                           t
//  Desc:   Transpose the 4x4 matrix. Matrix is set to [M] .
// ----------------------------------------------------------------------------
inline void SPMatrix::Transpose() {
    __m128  xmm0 = _mm_unpacklo_ps(_L1,_L2),
            xmm1 = _mm_unpacklo_ps(_L3,_L4),
            xmm2 = _mm_unpackhi_ps(_L1,_L2),
            xmm3 = _mm_unpackhi_ps(_L3,_L4);

    _L1 = _mm_movelh_ps(xmm0,xmm1);
    _L2 = _mm_movehl_ps(xmm1,xmm0);
    _L3 = _mm_movelh_ps(xmm2,xmm3);
    _L4 = _mm_movehl_ps(xmm3,xmm2);
}


            /***********************************************************/
            /*                    Vector[4] Operators                  */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   VectorMult                          ___     ___
//  Desc:   Vector multiplication with matrix. [Res] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline void VectorMult(const SPVector& Vec, const SPMatrix& Mat, SPVector& res) {
    F32vec4 result;
    result = (F32vec4)_mm_shuffle_ps(Vec,Vec,0x00) * Mat._L1;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0x55) * Mat._L2;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0xAA) * Mat._L3;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0xFF) * Mat._L4;
    res = result;
}

// ----------------------------------------------------------------------------
//  Name:   VectorMult                                  ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector VectorMult(const SPVector& Vec, const SPMatrix& Mat) {
    F32vec4 result;
    result = (F32vec4)_mm_shuffle_ps(Vec,Vec,0x00) * Mat._L1;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0x55) * Mat._L2;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0xAA) * Mat._L3;
    result += (F32vec4)_mm_shuffle_ps(Vec,Vec,0xFF) * Mat._L4;
    return result;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector * SPMatrix                         ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector operator * (const SPVector& Vec, const SPMatrix& Mat) {
    return VectorMult(Vec, Mat);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector *= SPMatrix                ___     ___
//  Desc:   Vector multiplication with matrix. [Vec] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator *= (const SPMatrix& Mat) {
    VectorMult(*this, Mat, *this);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector * SPVector                      _   _
//  Desc:   Dot product of the two vectors. Returns [A]·[B].
// ----------------------------------------------------------------------------
inline float operator * (const SPVector& A, const SPVector& B) {
    F32vec4 r = _mm_mul_ps(A,B);
    r = _mm_add_ps(_mm_movehl_ps(r,r),r);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), r);
    return *(float *)&t;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector % SPVector                           _   _
//  Desc:   Cross product of the two 3D vectors. Returns [A]x[B].
// ----------------------------------------------------------------------------
inline SPVector operator % (const SPVector& A, const SPVector& B) {
    F32vec4 l1, l2, m1, m2;
    l1 = _mm_shuffle_ps(A,A, _MM_SHUFFLE(3,1,0,2));
    l2 = _mm_shuffle_ps(B,B, _MM_SHUFFLE(3,0,2,1));
    m2 = l1*l2 & _0FFF_;
    l1 = _mm_shuffle_ps(A,A, _MM_SHUFFLE(3,0,2,1));
    l2 = _mm_shuffle_ps(B,B, _MM_SHUFFLE(3,1,0,2));
    m1 = l1*l2;
    return m1-m2;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector * float                                 _
//  Desc:   Multiplies the vector elements by scalar. Returns [V]*s.
// ----------------------------------------------------------------------------
inline SPVector operator * (const SPVector& V, const float s) {
    return V.vec * F32vec4(s);
}
inline SPVector operator * (const float s, const SPVector& V) {
    return V.vec * F32vec4(s);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector *= float                ___     ___
//  Desc:   Vector multiplication by float. [Vec] = [Vec]*s.
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator *= (const float s) {
    vec = vec * F32vec4(s);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector | SPVector
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline SPVector operator | (const SPVector& A, const SPVector& B) {
    return _mm_mul_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector |= SPVector
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator |= (const SPVector& B) {
    vec = _mm_mul_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector + SPVector       _   _
//  Desc:   Vector addition. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline SPVector operator + (const SPVector& A, const SPVector& B) {
    return _mm_add_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector - SPVector           _   _
//  Desc:   Vector substraction. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline SPVector operator - (const SPVector& A, const SPVector& B) {
    return _mm_sub_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector += SPVector    _     _   _
//  Desc:   Vector addition.       [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator += (const SPVector& B) {
    vec = _mm_add_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector -= SPVector    _     _   _
//  Desc:   Vector substraction.   [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator -= (const SPVector& B) {
    vec = _mm_sub_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -SPVector (Unary minus)                _
//  Desc:   Vectors elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline SPVector operator - (const SPVector& A) {
    return _mm_xor_ps(_MASKSIGNs_,A);
}

// ----------------------------------------------------------------------------
//  Name:   +SPVector (Unary plus)
//  Desc:   Returns the same vector.
// ----------------------------------------------------------------------------
inline SPVector operator + (const SPVector& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector::Length
//  Desc:   Returns the length of the vector.
// ----------------------------------------------------------------------------
inline float SPVector::Length() {
    F32vec4 r = _mm_mul_ps(vec,vec);
    r = _mm_add_ps(_mm_movehl_ps(r,r),r);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), r);
    t = sqrt(t);
    return *(float *)&t;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector::Normalize
//  Desc:   Normalized the (X,Y,Z) elements of the vector.
// ----------------------------------------------------------------------------
inline SPVector& SPVector::Normalize() {
    F32vec4 r = _mm_mul_ps(vec,vec);
    r = _mm_add_ps(_mm_movehl_ps(r,r),r);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), r);
    #ifdef ZERO_VECTOR
        t = _mm_cmpneq_ss(t, _mm_setzero_ps()) & rsqrt_nr(t);
    #else
        t = rsqrt_nr(t);
    #endif
    vec = _mm_mul_ps(vec, _mm_shuffle_ps(t,t,0x00));
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   ~SPVector [Normalize]
//  Desc:   Normalized the source vector.
// ----------------------------------------------------------------------------
inline SPVector operator ~ (const SPVector& V) {
    F32vec4 r = _mm_mul_ps(V,V);
    r = _mm_add_ps(_mm_movehl_ps(r,r),r);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), r);
    #ifdef ZERO_VECTOR
        t = _mm_cmpneq_ss(t, _mm_setzero_ps()) & rsqrt_nr(t);
    #else
        t = rsqrt_nr(t);
    #endif
    r = _mm_mul_ps(V, _mm_shuffle_ps(t,t,0x00));
    return r;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector = SPVector3 (assignment)
//  Desc:   Convert a SPVector3 into SPVector.
// ----------------------------------------------------------------------------
inline SPVector& SPVector::operator = (const SPVector3& a) {
    vec = _mm_and_ps(a.vec, _0FFF_);
    vec = _mm_or_ps(vec, _1000_);   // Set W to one.
    return *this;
}


            /***********************************************************/
            /*                    Vector[3] Operators                  */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   VectorMult                          ___     ___
//  Desc:   Vector multiplication with matrix. [Res] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline void VectorMult(const SPVector3& Vec, const SPMatrix& Mat, SPVector& res) {
    F32vec4 result = Mat._L4;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x00)) * Mat._L1;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x55)) * Mat._L2;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0xAA)) * Mat._L3;
    res = result;
}
inline void VectorMult(const SPVector3& Vec, const SPMatrix& Mat, SPVector3& res) {
    F32vec4 result = Mat._L4;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x00)) * Mat._L1;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x55)) * Mat._L2;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0xAA)) * Mat._L3;

    F32vec1 W = _mm_shuffle_ps(result, result, 0xFF);
    W = F32vec1(1.0f)/W;
    res = result * _mm_shuffle_ps(W, W, 0x00);
}

// ----------------------------------------------------------------------------
//  Name:   VectorMult                                  ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector VectorMult(const SPVector3& Vec, const SPMatrix& Mat) {
    F32vec4 result = Mat._L4;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x00)) * Mat._L1;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0x55)) * Mat._L2;
    result += F32vec4(_mm_shuffle_ps(Vec,Vec,0xAA)) * Mat._L3;
    return result;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 * SPMatrix                        ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector operator * (const SPVector3& Vec, const SPMatrix& Mat) {
    return VectorMult(Vec, Mat);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 *= SPMatrix               ___     ___
//  Desc:   Vector multiplication with matrix. [Vec] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator *= (const SPMatrix& Mat) {
    VectorMult(*this, Mat, *this);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 * SPVector3                    _   _
//  Desc:   Dot product of the two vectors. Returns [A]·[B].
// ----------------------------------------------------------------------------
inline float operator * (const SPVector3& A, const SPVector3& B) {
    F32vec4 r = _mm_mul_ps(A,B);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ps(_mm_movehl_ps(r,r),r));
    return *(float *)&t;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 % SPVector3                      _   _
//  Desc:   Cross product of the two vectors. Returns [A]x[B].
// ----------------------------------------------------------------------------
inline SPVector3 operator % (const SPVector3& A, const SPVector3& B) {
    F32vec4 l1, l2, m1, m2;
    l1 = _mm_shuffle_ps(A,A, _MM_SHUFFLE(3,1,0,2));
    l2 = _mm_shuffle_ps(B,B, _MM_SHUFFLE(3,0,2,1));
    m2 = l1*l2;
    l1 = _mm_shuffle_ps(A,A, _MM_SHUFFLE(3,0,2,1));
    l2 = _mm_shuffle_ps(B,B, _MM_SHUFFLE(3,1,0,2));
    m1 = l1*l2;
    return m1-m2;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 * float                                _
//  Desc:   Multiply the vector elements by scalar. Returns [V]*s.
// ----------------------------------------------------------------------------
inline SPVector3 operator * (const SPVector3& V, const float s) {
    return V.vec * F32vec4(s);
}
inline SPVector3 operator * (const float s, const SPVector3& V) {
    return V.vec * F32vec4(s);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 *= float               ___     ___
//  Desc:   Vector multiplication by float. [Vec] = [Vec]*s.
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator *= (const float s) {
    vec = vec * F32vec4(s);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 | SPVector3
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline SPVector3 operator | (const SPVector3& A, const SPVector3& B) {
    return _mm_mul_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 |= SPVector3
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator |= (const SPVector3& B) {
    vec = _mm_mul_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 + SPVector3     _   _
//  Desc:   Vector addition. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline SPVector3 operator + (const SPVector3& A, const SPVector3& B) {
    return _mm_add_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 - SPVector3         _   _
//  Desc:   Vector substraction. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline SPVector3 operator - (const SPVector3& A, const SPVector3& B) {
    return _mm_sub_ps(A.vec, B.vec);
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 += SPVector3  _     _   _
//  Desc:   Vector addition.       [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator += (const SPVector3& B) {
    vec = _mm_add_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 -= SPVector3  _     _   _
//  Desc:   Vector substraction.   [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator -= (const SPVector3& B) {
    vec = _mm_sub_ps(vec, B.vec);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -SPVector3 (Unary minus)                _
//  Desc:   Vectors elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline SPVector3 operator - (const SPVector3& A) {
    return _mm_xor_ps(_MASKSIGNs_,A);
}

// ----------------------------------------------------------------------------
//  Name:   +SPVector3 (Unary plus)
//  Desc:   Returns the same vector.
// ----------------------------------------------------------------------------
inline SPVector3 operator + (const SPVector3& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3::Length
//  Desc:   Returns the length of the vector.
// ----------------------------------------------------------------------------
inline float SPVector3::Length() {
    F32vec4 r = _mm_mul_ps(vec,vec);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ss(_mm_movehl_ps(r,r),r));
    t = sqrt(t);
    return *(float *)&t;
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3::Normalize
//  Desc:   Normalized the (X,Y,Z) elements of the vector.
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::Normalize() {
    F32vec4 r = _mm_mul_ps(vec,vec);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ss(_mm_movehl_ps(r,r),r));
    #ifdef ZERO_VECTOR
        t = _mm_cmpneq_ss(t, _mm_setzero_ps()) & rsqrt_nr(t);
    #else
        t = rsqrt_nr(t);
    #endif
    vec = _mm_mul_ps(vec, _mm_shuffle_ps(t,t,0x00));
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   ~SPVector3 [Normalize]
//  Desc:   Normalized the source vector.
// ----------------------------------------------------------------------------
inline SPVector3 operator ~ (const SPVector3& V) {
    F32vec4 r = _mm_mul_ps(V.vec,V.vec);
    F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ss(_mm_movehl_ps(r,r),r));
    #ifdef ZERO_VECTOR
        t = _mm_cmpneq_ss(t, _mm_setzero_ps()) & rsqrt_nr(t);
    #else
        t = rsqrt_nr(t);
    #endif
    return _mm_mul_ps(V.vec, _mm_shuffle_ps(t,t,0x00));
}

// ----------------------------------------------------------------------------
//  Name:   SPVector3 = SPVector (assignment)
//  Desc:   Convert a SPVector into SPVector3.
// ----------------------------------------------------------------------------
inline SPVector3& SPVector3::operator = (const SPVector& a) {
    F32vec1 W = _mm_shuffle_ps(a, a, 0xFF);
    W = F32vec1(1.0f)/W;
    vec = (F32vec4)a.vec * _mm_shuffle_ps(W, W, 0x00);
    return *this;
}
