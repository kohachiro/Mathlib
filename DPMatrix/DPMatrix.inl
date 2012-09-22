//////////////////////////////////////////////////////////////////////////////
//
// DPMATRIX.INL
//
//  Implementation of the DPMatrix, DPVector and the DPVector3 classes.
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


extern const __int64 _Sign_NP[2];
#define Sign_NP (*(F64vec2*)&_Sign_NP)


            /***********************************************************/
            /*                Matrix Constructors                      */
            /***********************************************************/

inline DPMatrix::DPMatrix(  double f11, double f12, double f13, double f14,
                            double f21, double f22, double f23, double f24,
                            double f31, double f32, double f33, double f34,
                            double f41, double f42, double f43, double f44  )
{
    _L1a = F64vec2(f12, f11); _L1b = F64vec2(f14, f13);
    _L2a = F64vec2(f22, f21); _L2b = F64vec2(f24, f23);
    _L3a = F64vec2(f32, f31); _L3b = F64vec2(f34, f33);
    _L4a = F64vec2(f42, f41); _L4b = F64vec2(f44, f43);
}

inline DPMatrix::DPMatrix(const double *arr)
{
    _L1a = _mm_loadu_pd(arr);    _L1b = _mm_loadu_pd(arr+2);
    _L2a = _mm_loadu_pd(arr+4);  _L2b = _mm_loadu_pd(arr+6);
    _L3a = _mm_loadu_pd(arr+8);  _L3b = _mm_loadu_pd(arr+10);
    _L4a = _mm_loadu_pd(arr+12); _L4b = _mm_loadu_pd(arr+14);
}

// ----------------------------------------------------------------------------
//  Name:   ZeroMatrix
//  Desc:   Sets all elements in [res] to zero.
// ----------------------------------------------------------------------------
#ifndef MATRICES_H__
inline DPMatrix ZeroMatrix() {
    DPMatrix res;
    res._L1a = res._L1b = res._L2a = res._L2b = _mm_setzero_pd();
    res._L3a = res._L3b = res._L4a = res._L4b = _mm_setzero_pd();
    return res;
}
#endif
inline void DPMatrix::ZeroMatrix() {
    _L1a = _L1b = _L2a = _L2b = _mm_setzero_pd();
    _L3a = _L3b = _L4a = _L4b = _mm_setzero_pd();
}

// ----------------------------------------------------------------------------
//  Name:   IdentityMatrix
//  Desc:   Returns the Identity Matrix.
// ----------------------------------------------------------------------------
#ifndef MATRICES_H__
inline DPMatrix IdentityMatrix() {
    DPMatrix res;
    __m128d zerone = _mm_set_sd(1.0);   //_ZERONE_;
    res._L1a = res._L3b = zerone;
    res._L1b = res._L3a = _mm_setzero_pd();
    zerone = _mm_shuffle_pd(zerone, zerone, 1);
    res._L2a = res._L4b = zerone;
    res._L2b = res._L4a = _mm_setzero_pd();
    return res;
}
#endif
inline void DPMatrix::IdentityMatrix() {
    __m128d zerone = _mm_set_sd(1.0);   //_ZERONE_;
    _L1b = _L3a = _mm_setzero_pd();
    _L1a = _L3b = zerone;
    zerone = _mm_shuffle_pd(zerone, zerone, 1);
    _L2b = _L4a = _mm_setzero_pd();
    _L2a = _L4b = zerone;
}


            /***********************************************************/
            /*                     Matrix Operators                    */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   MatrixMult
//  Desc:   Matrix multiplication of A and B. [res] = [A]*[B].
// ----------------------------------------------------------------------------
inline void MatrixMult(const DPMatrix& A, const DPMatrix& B, DPMatrix& res) {
    F64vec4 r1, r2;
    F64vec4 B1 = B._L1, B2 = B._L2, B3 = B._L3, B4 = B._L4;

    r1 = F64vec4(A._11) * B1;
    r2 = F64vec4(A._21) * B1;
    r1 += F64vec4(A._12) * B2;
    r2 += F64vec4(A._22) * B2;
    r1 += F64vec4(A._13) * B3;
    r2 += F64vec4(A._23) * B3;
    r1 += F64vec4(A._14) * B4;
    r2 += F64vec4(A._24) * B4;
    res._L1 = r1;
    res._L2 = r2;

    r1 = F64vec4(A._31) * B1;
    r2 = F64vec4(A._41) * B1;
    r1 += F64vec4(A._32) * B2;
    r2 += F64vec4(A._42) * B2;
    r1 += F64vec4(A._33) * B3;
    r2 += F64vec4(A._43) * B3;
    r1 += F64vec4(A._34) * B4;
    r2 += F64vec4(A._44) * B4;
    res._L3 = r1;
    res._L4 = r2;
}

// ----------------------------------------------------------------------------
//  Name:   MatrixMult
//  Desc:   Matrix multiplication of A and B. Returns [A]*[B].
// ----------------------------------------------------------------------------
inline DPMatrix MatrixMult(const DPMatrix& A, const DPMatrix& B) {
    DPMatrix res;
    F64vec4 r1, r2;
    F64vec4 B1 = B._L1, B2 = B._L2, B3 = B._L3, B4 = B._L4;

    r1 = F64vec4(A._11) * B1;
    r2 = F64vec4(A._21) * B1;
    r1 += F64vec4(A._12) * B2;
    r2 += F64vec4(A._22) * B2;
    r1 += F64vec4(A._13) * B3;
    r2 += F64vec4(A._23) * B3;
    r1 += F64vec4(A._14) * B4;
    r2 += F64vec4(A._24) * B4;
    res._L1 = r1;
    res._L2 = r2;

    r1 = F64vec4(A._31) * B1;
    r2 = F64vec4(A._41) * B1;
    r1 += F64vec4(A._32) * B2;
    r2 += F64vec4(A._42) * B2;
    r1 += F64vec4(A._33) * B3;
    r2 += F64vec4(A._43) * B3;
    r1 += F64vec4(A._34) * B4;
    r2 += F64vec4(A._44) * B4;
    res._L3 = r1;
    res._L4 = r2;

    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix * DPMatrix
//  Desc:   Matrix multiplication of A and B. Returns [A]*[B].
// ----------------------------------------------------------------------------
inline DPMatrix operator * (const DPMatrix& A, const DPMatrix& B) {
    return MatrixMult(A, B);
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix *= DPMatrix
//  Desc:   Matrix multiplication of A by B. [A] = [A]*[B].
// ----------------------------------------------------------------------------
inline DPMatrix& DPMatrix::operator *= (const DPMatrix& B) {
    MatrixMult(*this, B, *this);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix * double
//  Desc:   Matrix elements multiplication with scalar. Returns [A]*s.
// ----------------------------------------------------------------------------
inline DPMatrix operator * (const DPMatrix& A, const double s) {
    DPMatrix res;
    F64vec4 S = F64vec4(s);
    res._L1 = A._L1 * S;
    res._L2 = A._L2 * S;
    res._L3 = A._L3 * S;
    res._L4 = A._L4 * S;
    return res;
}
inline DPMatrix operator * (const double s, const DPMatrix& A) {
    DPMatrix res;
    F64vec4 S = F64vec4(s);
    res._L1 = A._L1 * S;
    res._L2 = A._L2 * S;
    res._L3 = A._L3 * S;
    res._L4 = A._L4 * S;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix *= double
//  Desc:   Matrix elements multiplication by scalar. [A] = [A]*s.
// ----------------------------------------------------------------------------
inline DPMatrix& DPMatrix::operator *= (const double s) {
    F64vec4 S = F64vec4(s);
    _L1 = _L1 * S;
    _L2 = _L2 * S;
    _L3 = _L3 * S;
    _L4 = _L4 * S;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix + DPMatrix
//  Desc:   Matrix addition between A and B. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline DPMatrix operator + (const DPMatrix& A, const DPMatrix& B) {
    DPMatrix res;
    res._L1 = (F64vec4)A._L1 + (F64vec4)B._L1;
    res._L2 = (F64vec4)A._L2 + (F64vec4)B._L2;
    res._L3 = (F64vec4)A._L3 + (F64vec4)B._L3;
    res._L4 = (F64vec4)A._L4 + (F64vec4)B._L4;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix += DPMatrix
//  Desc:   Matrix addition of A by B. [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline DPMatrix& DPMatrix::operator += (const DPMatrix& B) {
    _L1 = (F64vec4)_L1 + (F64vec4)B._L1;
    _L2 = (F64vec4)_L2 + (F64vec4)B._L2;
    _L3 = (F64vec4)_L3 + (F64vec4)B._L3;
    _L4 = (F64vec4)_L4 + (F64vec4)B._L4;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix - DPMatrix
//  Desc:   Matrix substraction between A and B. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline DPMatrix operator - (const DPMatrix& A, const DPMatrix& B) {
    DPMatrix res;
    res._L1 = (F64vec4)A._L1 - (F64vec4)B._L1;
    res._L2 = (F64vec4)A._L2 - (F64vec4)B._L2;
    res._L3 = (F64vec4)A._L3 - (F64vec4)B._L3;
    res._L4 = (F64vec4)A._L4 - (F64vec4)B._L4;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix -= DPMatrix
//  Desc:   Matrix substraction of A by B. [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline DPMatrix& DPMatrix::operator -= (const DPMatrix& B) {
    _L1 = (F64vec4)_L1 - (F64vec4)B._L1;
    _L2 = (F64vec4)_L2 - (F64vec4)B._L2;
    _L3 = (F64vec4)_L3 - (F64vec4)B._L3;
    _L4 = (F64vec4)_L4 - (F64vec4)B._L4;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -DPMatrix (Unary minus)
//  Desc:   Matrix elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline DPMatrix operator - (const DPMatrix& A) {
    DPMatrix res;
    res._L1a = _mm_neg_pd(A._L1a);
    res._L1b = _mm_neg_pd(A._L1b);
    res._L2a = _mm_neg_pd(A._L2a);
    res._L2b = _mm_neg_pd(A._L2b);
    res._L3a = _mm_neg_pd(A._L3a);
    res._L3b = _mm_neg_pd(A._L3b);
    res._L4a = _mm_neg_pd(A._L4a);
    res._L4b = _mm_neg_pd(A._L4b);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   +DPMatrix (Unary plus)
//  Desc:   Returns the same matrix [A].
// ----------------------------------------------------------------------------
inline DPMatrix operator + (const DPMatrix& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix::Transpose                           t
//  Desc:   Transpose the 4x4 matrix. Matrix is set to [M] .
// ----------------------------------------------------------------------------
inline void DPMatrix::Transpose() {
    __m128d t1,t2;
    t1 = _L2a; 
    _L2a = _mm_unpacklo_pd(_L1a, _L2a);
    _L1a = _mm_unpackhi_pd(_L1a, t1);
    t2 = _L4b;
    _L4b = _mm_unpacklo_pd(_L3b, _L4b);
    _L3b = _mm_unpackhi_pd(_L3b, t2);
    t1 = _L1b; t2 = _L2b;
    _L1b = _mm_unpacklo_pd(_L3a,_L4a);
    _L2b = _mm_unpackhi_pd(_L3a,_L4a);
    _L3a = _mm_unpacklo_pd(t1,t2);
    _L4a = _mm_unpackhi_pd(t1,t2);
}


            /***********************************************************/
            /*                    Vector[4] Operators                  */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   VectorMult                          ___     ___
//  Desc:   Vector multiplication with matrix. [res] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline void VectorMult(const DPVector& Vec, const DPMatrix& Mat, DPVector& res) {
    F64vec4 result;
    result = F64vec4(_mm_unpacklo_pd(Vec.xy, Vec.xy)) * Mat._L1;
    result += F64vec4(_mm_unpackhi_pd(Vec.xy, Vec.xy)) * Mat._L2;
    result += F64vec4(_mm_unpacklo_pd(Vec.zw, Vec.zw)) * Mat._L3;
    result += F64vec4(_mm_unpackhi_pd(Vec.zw, Vec.zw)) * Mat._L4;
    res = result;
}

// ----------------------------------------------------------------------------
//  Name:   VectorMult                                  ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector VectorMult(const DPVector& Vec, const DPMatrix& Mat) {
    F64vec4 result;
    result = F64vec4(_mm_unpacklo_pd(Vec.xy, Vec.xy)) * Mat._L1;
    result += F64vec4(_mm_unpackhi_pd(Vec.xy, Vec.xy)) * Mat._L2;
    result += F64vec4(_mm_unpacklo_pd(Vec.zw, Vec.zw)) * Mat._L3;
    result += F64vec4(_mm_unpackhi_pd(Vec.zw, Vec.zw)) * Mat._L4;
    return result;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector * DPMatrix                         ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector operator * (const DPVector& Vec, const DPMatrix& Mat) {
    return VectorMult(Vec, Mat);
}

// ----------------------------------------------------------------------------
//  Name:   DPVector *= DPMatrix                ___     ___
//  Desc:   Vector multiplication with matrix. [Vec] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator *= (const DPMatrix& Mat) {
    VectorMult(*this, Mat, *this);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector * DPVector                      _   _
//  Desc:   Dot product of the two vectors. Returns [A]·[B].
// ----------------------------------------------------------------------------
inline double operator * (const DPVector& A, const DPVector& B) {
    F64vec4 r = (F64vec4)A * (F64vec4)B;
    F64vec2 s = _mm_add_pd(((__m256d)r).vec1,((__m256d)r).vec2);
    s = _mm_add_sd(s, _mm_unpacklo_pd(s,s));
    return *(double *)&s;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector % DPVector                           _   _
//  Desc:   Cross product of the two 3D vectors. Returns [A]x[B].
// ----------------------------------------------------------------------------
inline DPVector operator % (const DPVector& A, const DPVector& B) {
    DPVector res;
    F64vec2 l1, l2, m1, m2;
    l1 = _mm_mul_pd(_mm_unpacklo_pd(A.zw, A.zw), B.xy);
    l2 = _mm_mul_pd(_mm_unpacklo_pd(B.zw, B.zw), A.xy);
    m1 = l1-l2;
    m1 = _mm_shuffle_pd(m1, m1, 1);
    m1 = _mm_xor_pd(m1, Sign_NP);

    l1 = _mm_mul_pd(B.xy, _mm_shuffle_pd(A.xy, A.xy, 1));
    l2 = _mm_mul_pd(A.zw, B.zw);
    l2 = _mm_unpackhi_pd(l1, l2);
    m2 = _mm_sub_sd(l2, l1);

    res.xy = m1;
    res.zw = m2;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector * double                                 _
//  Desc:   Multiplies the vector elements by scalar. Returns [V]*s.
// ----------------------------------------------------------------------------
inline DPVector operator * (const DPVector& V, const double s) {
    return V.vec * F64vec4(s);
}
inline DPVector operator * (const double s, const DPVector& V) {
    return V.vec * F64vec4(s);
}

// ----------------------------------------------------------------------------
//  Name:   DPVector *= double                ___     ___
//  Desc:   Vector multiplication by double. [Vec] = [Vec]*s.
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator *= (const double s) {
    vec = vec * F64vec4(s);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector | DPVector
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline DPVector operator | (const DPVector& A, const DPVector& B) {
    return (F64vec4)A.vec * (F64vec4)B.vec;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector |= DPVector
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator |= (const DPVector& B) {
    vec = (F64vec4)vec * (F64vec4)B.vec;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector + DPVector       _   _
//  Desc:   Vector addition. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline DPVector operator + (const DPVector& A, const DPVector& B) {
    return (F64vec4)A.vec + (F64vec4)B.vec;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector - DPVector           _   _
//  Desc:   Vector substraction. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline DPVector operator - (const DPVector& A, const DPVector& B) {
    return (F64vec4)A.vec - (F64vec4)B.vec;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector += DPVector    _     _   _
//  Desc:   Vector addition.       [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator += (const DPVector& B) {
    vec = (F64vec4)vec + (F64vec4)B.vec;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector -= DPVector    _     _   _
//  Desc:   Vector substraction.   [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator -= (const DPVector& B) {
    vec = (F64vec4)vec - (F64vec4)B.vec;
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -DPVector (Unary minus)                _
//  Desc:   Vectors elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline DPVector operator - (const DPVector& A) {
    DPVector res;
    res.xy = _mm_neg_pd(A.xy);
    res.zw = _mm_neg_pd(A.zw);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   +DPVector (Unary plus)
//  Desc:   Returns the same vector.
// ----------------------------------------------------------------------------
inline DPVector operator + (const DPVector& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector::Length
//  Desc:   Returns the length of the vector.
// ----------------------------------------------------------------------------
inline double DPVector::Length() {
    __m128d r = _mm_add_pd(_mm_mul_pd(xy,xy),_mm_mul_pd(zw,zw));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    r = _mm_sqrt_sd(r,r);
    return *(double *)&r;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector::Normalize
//  Desc:   Normalized the vector.
// ----------------------------------------------------------------------------
inline DPVector& DPVector::Normalize() {
    __m128d r = _mm_add_pd(_mm_mul_pd(xy,xy),_mm_mul_pd(zw,zw));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    __m128d t = _mm_sqrt_sd(r,r);
    t = _mm_div_sd(_mm_set_sd(1.0),t);
    
    #ifdef ZERO_VECTOR
        t = _mm_and_pd(_mm_cmpneq_sd(r, _mm_setzero_pd()), t);
    #endif

    t = _mm_unpacklo_pd(t,t);
    xy = _mm_mul_pd(xy, t);
    zw = _mm_mul_pd(zw, t);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   ~DPVector [Normalize]
//  Desc:   Normalized the source vector.
// ----------------------------------------------------------------------------
inline DPVector operator ~ (const DPVector& V) {
    DPVector res;
    __m128d r = _mm_add_pd(_mm_mul_pd(V.xy,V.xy),_mm_mul_pd(V.zw,V.zw));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    __m128d t = _mm_sqrt_sd(r,r);
    t = _mm_div_sd(_mm_set_sd(1.0),t);
    #ifdef ZERO_VECTOR
        t = _mm_and_pd(_mm_cmpneq_sd(r, _mm_setzero_pd()), t);
    #endif

    t = _mm_unpacklo_pd(t,t);
    res.xy = _mm_mul_pd(V.xy, t);
    res.zw = _mm_mul_pd(V.zw, t);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector = DPVector3 (assignment)
//  Desc:   Convert a DPVector3 into DPVector.
// ----------------------------------------------------------------------------
inline DPVector& DPVector::operator = (const DPVector3& a) {
    xy = a.xy;
    zw = _mm_unpacklo_pd(a.z_, _mm_set_sd(1.0));
    return *this;
}


            /***********************************************************/
            /*                    Vector[3] Operators                  */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   VectorMult                          ___     ___
//  Desc:   Vector multiplication with matrix. [Res] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline void VectorMult(const DPVector3& Vec, const DPMatrix& Mat, DPVector& Res) {
    F64vec4 result = Mat._L4;
    result += F64vec4(_mm_unpacklo_pd(Vec.xy,Vec.xy)) * Mat._L1;
    result += F64vec4(_mm_unpackhi_pd(Vec.xy,Vec.xy)) * Mat._L2;
    result += F64vec4(_mm_unpacklo_pd(Vec.z_,Vec.z_)) * Mat._L3;
    Res = result;
}
inline void VectorMult(const DPVector3& Vec, const DPMatrix& Mat, DPVector3& Res) {
    F64vec4 result = Mat._L4;
    result += F64vec4(_mm_unpacklo_pd(Vec.xy,Vec.xy)) * Mat._L1;
    result += F64vec4(_mm_unpackhi_pd(Vec.xy,Vec.xy)) * Mat._L2;
    result += F64vec4(_mm_unpacklo_pd(Vec.z_,Vec.z_)) * Mat._L3;

    __m128d w = _mm_unpackhi_pd(result.vec2, result.vec2);
    w = _mm_div_sd(_mm_set_sd(1.0), w);
    w = _mm_unpacklo_pd(w,w);
    Res.xy = _mm_mul_pd(result.vec1, w);
    Res.z_ = _mm_mul_sd(result.vec2, w);
}

// ----------------------------------------------------------------------------
//  Name:   VectorMult                                  ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector VectorMult(const DPVector3& Vec, const DPMatrix& Mat) {
    F64vec4 result = Mat._L4;
    result += F64vec4(_mm_unpacklo_pd(Vec.xy,Vec.xy)) * Mat._L1;
    result += F64vec4(_mm_unpackhi_pd(Vec.xy,Vec.xy)) * Mat._L2;
    result += F64vec4(_mm_unpacklo_pd(Vec.z_,Vec.z_)) * Mat._L3;
    return result;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 * DPMatrix                        ___
//  Desc:   Vector multiplication with matrix. Returns [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector operator * (const DPVector3& Vec, const DPMatrix& Mat) {
    return VectorMult(Vec, Mat);
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 *= DPMatrix               ___     ___
//  Desc:   Vector multiplication with matrix. [Vec] = [Vec]*[Mat].
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator *= (const DPMatrix& Mat) {
    VectorMult(*this, Mat, *this);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 * DPVector3                    _   _
//  Desc:   Dot product of the two vectors. Returns [A]·[B].
// ----------------------------------------------------------------------------
inline double operator * (const DPVector3& A, const DPVector3& B) {
    __m128d r = _mm_mul_pd(A.xy, B.xy);
    r = _mm_add_sd(r, _mm_mul_sd(A.z_, B.z_));
    r = _mm_add_sd(r, _mm_unpackhi_pd(r,r));
    return *(double *)&r;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 % DPVector3                      _   _
//  Desc:   Cross product of the two vectors. Returns [A]x[B].
// ----------------------------------------------------------------------------
inline DPVector3 operator % (const DPVector3& A, const DPVector3& B) {
    DPVector3 res;
    F64vec2 l1, l2, m1, m2;
    l1 = _mm_mul_pd(_mm_unpacklo_pd(A.z_, A.z_), B.xy);
    l2 = _mm_mul_pd(_mm_unpacklo_pd(B.z_, B.z_), A.xy);
    m1 = l1-l2;
    m1 = _mm_shuffle_pd(m1, m1, 1);
    m1 = _mm_xor_pd(m1, Sign_NP);
    l1 = _mm_mul_pd(B.xy, _mm_shuffle_pd(A.xy, A.xy, 1));
    m2 = _mm_sub_sd(l1, _mm_unpackhi_pd(l1, l1));
    res.xy = m1;
    res.z_ = m2;
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 * double                               _
//  Desc:   Multiply the vector elements by scalar. Returns [V]*s.
// ----------------------------------------------------------------------------
inline DPVector3 operator * (const DPVector3& V, const double s) {
    F64vec2 S(s);
    DPVector3 res;
    res.xy = _mm_mul_pd(V.xy, S);
    res.z_ = _mm_mul_sd(V.z_, S);
    return res;
}
inline DPVector3 operator * (const double s, const DPVector3& V) {
    F64vec2 S(s);
    DPVector3 res;
    res.xy = _mm_mul_pd(V.xy, S);
    res.z_ = _mm_mul_sd(V.z_, S);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 *= double               ___     ___
//  Desc:   Vector multiplication by double. [Vec] = [Vec]*s.
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator *= (const double s) {
    F64vec2 S(s);
    xy = _mm_mul_pd(xy, S);
    z_ = _mm_mul_sd(z_, S);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 | DPVector3
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline DPVector3 operator | (const DPVector3& A, const DPVector3& B) {
    DPVector3 res;
    res.xy = _mm_mul_pd(A.xy, B.xy);
    res.z_ = _mm_mul_sd(A.z_, B.z_);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector |= DPVector
//  Desc:   Vector elements product.
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator |= (const DPVector3& B) {
    xy = _mm_mul_pd(xy, B.xy);
    z_ = _mm_mul_sd(z_, B.z_);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 + DPVector3     _   _
//  Desc:   Vector addition. Returns [A]+[B].
// ----------------------------------------------------------------------------
inline DPVector3 operator + (const DPVector3& A, const DPVector3& B) {
    DPVector3 res;
    res.xy = _mm_add_pd(A.xy, B.xy);
    res.z_ = _mm_add_sd(A.z_, B.z_);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 - DPVector3         _   _
//  Desc:   Vector substraction. Returns [A]-[B].
// ----------------------------------------------------------------------------
inline DPVector3 operator - (const DPVector3& A, const DPVector3& B) {
    DPVector3 res;
    res.xy = _mm_sub_pd(A.xy, B.xy);
    res.z_ = _mm_sub_sd(A.z_, B.z_);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 += DPVector3  _     _   _
//  Desc:   Vector addition.       [A] = [A]+[B].
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator += (const DPVector3& B) {
    xy = _mm_add_pd(xy, B.xy);
    z_ = _mm_add_sd(z_, B.z_);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 -= DPVector3  _     _   _
//  Desc:   Vector substraction.   [A] = [A]-[B].
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator -= (const DPVector3& B) {
    xy = _mm_sub_pd(xy, B.xy);
    z_ = _mm_sub_sd(z_, B.z_);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   -DPVector3 (Unary minus)                _
//  Desc:   Vectors elements are negate. Returns -[A].
// ----------------------------------------------------------------------------
inline DPVector3 operator - (const DPVector3& A) {
    DPVector3 res;
    res.xy = _mm_neg_pd(A.xy);
    res.z_ = _mm_neg_pd(A.z_);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   +DPVector3 (Unary plus)
//  Desc:   Returns the same vector.
// ----------------------------------------------------------------------------
inline DPVector3 operator + (const DPVector3& A) {
    return A;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3::Length
//  Desc:   Returns the length of the vector.
// ----------------------------------------------------------------------------
inline double DPVector3::Length() {
    __m128d r = _mm_add_sd(_mm_mul_pd(xy,xy),_mm_mul_sd(z_,z_));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    r = _mm_sqrt_sd(r,r);
    return *(double *)&r;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3::Normalize
//  Desc:   Normalized the vector.
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::Normalize() {
    __m128d r = _mm_add_sd(_mm_mul_pd(xy,xy),_mm_mul_sd(z_,z_));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    __m128d t = _mm_sqrt_sd(r,r);
    t = _mm_div_sd(_mm_set_sd(1.0),t);
    #ifdef ZERO_VECTOR
        t = _mm_and_pd(_mm_cmpneq_sd(r, _mm_setzero_pd()), t);
    #endif
    t = _mm_unpacklo_pd(t,t);
    xy = _mm_mul_pd(xy, t);
    z_ = _mm_mul_sd(z_, t);
    return *this;
}

// ----------------------------------------------------------------------------
//  Name:   ~DPVector3 [Normalize]
//  Desc:   Normalized the source vector.
// ----------------------------------------------------------------------------
inline DPVector3 operator ~ (const DPVector3& V) {
    DPVector3 res;
    __m128d r = _mm_add_sd(_mm_mul_pd(V.xy,V.xy),_mm_mul_sd(V.z_,V.z_));
    r = _mm_add_sd(_mm_unpackhi_pd(r,r),r);
    __m128d t = _mm_sqrt_sd(r,r);
    t = _mm_div_sd(_mm_set_sd(1.0),t);
    #ifdef ZERO_VECTOR
        t = _mm_and_pd(_mm_cmpneq_sd(r, _mm_setzero_pd()), t);
    #endif
    t = _mm_unpacklo_pd(t,t);
    res.xy = _mm_mul_pd(V.xy, t);
    res.z_ = _mm_mul_sd(V.z_, t);
    return res;
}

// ----------------------------------------------------------------------------
//  Name:   DPVector3 = DPVector (assignment)
//  Desc:   Convert a DPVector into DPVector3.
// ----------------------------------------------------------------------------
inline DPVector3& DPVector3::operator = (const DPVector& a) {
    __m128d w = _mm_unpackhi_pd(a.zw, a.zw);
    w = _mm_div_sd(_mm_set_sd(1.0), w);
    w = _mm_unpacklo_pd(w,w);
    xy = _mm_mul_pd(a.xy, w);
    z_ = _mm_mul_sd(a.zw, w);
    return *this;
}
