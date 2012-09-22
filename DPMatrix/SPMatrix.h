//////////////////////////////////////////////////////////////////////////////
//
// SPMATRIX.H
//
//  Declaration of the SPMatrix, SPVector and the SPVector3 classes.
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

#ifndef SPMATRIX_H__
#define SPMATRIX_H__

// General switches that control the behaviour of the library:
//#define USE_AMLIB         // Enable the use of the AMath library.
//#define ZERO_SINGULAR     // Does Inverse() of a singular matrix zero the matrix?
//#define ZERO_VECTOR       // Does normalization of zero vector zero the vector?


#if defined(DPMATRIX_H__) && !defined(MATRICES_H__)
#error Must include MATRICES.H when combining both SPMatrix and DPMatrix classes.
#endif

#ifndef __ICL
#pragma message( "This file should be compiled with Intel C/C++ Compiler")
#endif

#include <fvec.h>
#pragma pack(push,16)


class SPMatrix;
class SPVector;
class SPVector3;


// ----------------------------------------------------------------------------
//  Name:   SPMatrix
//  Desc:   A matrix of 4x4 floats.
// ----------------------------------------------------------------------------
class SPMatrix {
public:
    union {
        struct {
            __m128 _L1, _L2, _L3, _L4;
        };
        struct {
            float   _11, _12, _13, _14;
            float   _21, _22, _23, _24;
            float   _31, _32, _33, _34;
            float   _41, _42, _43, _44;
        };
    };

    // Constructors and convertions:

    SPMatrix() {}
    SPMatrix(const SPMatrix& m) : _L1(m._L1), _L2(m._L2), _L3(m._L3), _L4(m._L4) {}
    SPMatrix(float _11, float _12, float _13, float _14,
             float _21, float _22, float _23, float _24,
             float _31, float _32, float _33, float _34,
             float _41, float _42, float _43, float _44);
    SPMatrix(const float* arr);

    SPMatrix& operator = (const SPMatrix& a) {
        _L1 = a._L1; _L2 = a._L2; _L3 = a._L3; _L4 = a._L4;
        return *this;
    }

    // Accessing elements:

    float& operator () (int i, int j) {
        assert((0<=i) && (i<=3) && (0<=j) && (j<=3));
        return *(((float *)&_11) + (i<<2)+j);
    }
    F32vec4& operator () (int i) {
        assert((0<=i) && (i<=3));
        return *(((F32vec4 *)&_11) + i);
    }
    F32vec4& operator [] (int i) {
        assert((0<=i) && (i<=3));
        return *(((F32vec4 *)&_11) + i);
    }
    F32vec4& operator [] (int i) const {
        assert((0<=i) && (i<=3));
        return *(((F32vec4 *)&_11) + i);
    }

    // Operators and member functions:

    friend SPMatrix operator * (const SPMatrix&, const SPMatrix&);
    friend SPMatrix operator + (const SPMatrix&, const SPMatrix&);
    friend SPMatrix operator - (const SPMatrix&, const SPMatrix&);
    friend SPMatrix operator + (const SPMatrix&);
    friend SPMatrix operator - (const SPMatrix&);
    friend SPMatrix operator * (const SPMatrix&, const float);
    friend SPMatrix operator * (const float, const SPMatrix&);

    SPMatrix& operator *= (const SPMatrix&);
    SPMatrix& operator *= (const float);
    SPMatrix& operator += (const SPMatrix&);
    SPMatrix& operator -= (const SPMatrix&);

    void    Transpose();    // Transposes the matrix
    float   Inverse();      // Inverses the matrix and returns the determinant

    float   Determinant();  // Returns the determinant
    float   MinValue();     // Returns the minimum absolute value of the matrix
    float   MaxValue();     // Returns the maximum absolute value of the matrix

    // Other Constructors:

    void ZeroMatrix();
    void IdentityMatrix();
    void TranslateMatrix(const float dx, const float dy, const float dz);
    void ScaleMatrix(const float a, const float b, const float c);
    void ScaleMatrix(const float a);
    void RotateXMatrix(const float rads);
    void RotateYMatrix(const float rads);
    void RotateZMatrix(const float rads);
#ifdef USE_AMLIB    // Aproximations using AMath 
    void RotateXMatrixA(const float rads);
    void RotateYMatrixA(const float rads);
    void RotateZMatrixA(const float rads);
#endif
};

// ----------------------------------------------------------------------------
//  Name:   SPVector, SPVector4
//  Desc:   A vector of 4 floats.
// ----------------------------------------------------------------------------
class SPVector {
public:
    union {
        __m128 vec;
        struct { 
            float   x,y,z,w;
        };
    };

    // Constructors and convertions:

    SPVector() {}
    SPVector(const SPVector& v) : vec(v.vec) {}
    SPVector(const __m128 &m) : vec(m) {}
    SPVector(const F32vec4 &m) : vec(m) {}
    SPVector(const float x, const float y, const float z, const float w=1.0f) : vec(F32vec4(w,z,y,x)) {}
    SPVector(const float *arr) : vec(_mm_loadu_ps(arr)) {}
    operator __m128() const { return vec; }
    operator F32vec4() const { return vec; }

    SPVector& operator = (const SPVector& a) { vec = a.vec; return *this; }
    SPVector& operator = (const SPVector3&);
    SPVector& operator = (const F32vec4 &a) { vec = a; return *this; }
    SPVector& operator = (const __m128 &a) { vec = a; return *this; }

    // Accessing elements

    float& operator () (int i) {
        assert((0<=i) && (i<=3));
        return *(((float *)&vec) + i);
    }
    float& operator [] (int i) {
        assert((0<=i) && (i<=3));
        return *(((float *)&vec) + i);
    }
    float& operator [] (int i) const {
        assert((0<=i) && (i<=3));
        return *(((float *)&vec) + i);
    }

    // Operators and member functions:

    friend SPVector operator * (const SPVector&, const SPMatrix&);
    friend float operator * (const SPVector&, const SPVector&);             // Dot Product
    friend SPVector operator % (const SPVector&, const SPVector&);          // Cross Product
    friend SPVector operator | (const SPVector&, const SPVector&);          // Elements Product
    friend SPVector operator * (const SPVector&, const float);
    friend SPVector operator * (const float, const SPVector&);
    friend SPVector operator + (const SPVector&);
    friend SPVector operator + (const SPVector&, const SPVector&);
    friend SPVector operator - (const SPVector&);
    friend SPVector operator - (const SPVector&, const SPVector&);
    friend SPVector operator ~ (const SPVector&);                           // Normalize

    SPVector& operator *= (const SPMatrix&);
    SPVector& operator *= (const float);
    SPVector& operator |= (const SPVector&);
    SPVector& operator += (const SPVector&);
    SPVector& operator -= (const SPVector&);

    float Length();
    SPVector& Normalize();
};
#define SPVector4 SPVector

// ----------------------------------------------------------------------------
//  Name:   SPVector3
//  Desc:   A vector of 3 floats.
// ----------------------------------------------------------------------------
class SPVector3 {
public:
    union {
        __m128 vec;
        struct { 
            float   x,y,z;
        //  float   _spacer_;
        };
    };

    // Constructors and convertions:

    SPVector3() {}
    SPVector3(const SPVector3& v) : vec(v.vec) {}
    SPVector3(const __m128 &m) : vec(m) {}
    SPVector3(const F32vec4 &m) : vec(m) {}
    SPVector3(const float x, const float y, const float z) : vec(F32vec4(0.0f,z,y,x)) {}
    SPVector3(const float *arr) : vec(_mm_loadl_pi(_mm_movelh_ps(_mm_load_ss(arr+2),_mm_load_ss(arr+2)), (__m64*)arr)) {}
    operator __m128() const { return vec; }
    operator F32vec4() const { return vec; }

    SPVector3& operator = (const SPVector3& a) { vec = a.vec; return *this; }
    SPVector3& operator = (const SPVector& a);
    SPVector3& operator = (const F32vec4 &a) { vec = a; return *this; }
    SPVector3& operator = (const __m128 &a) { vec = a; return *this; }

    // Accessing elements:

    float& operator () (int i) {
        assert((0<=i) && (i<=2));
        return *(((float *)&vec) + i);
    }
    float& operator [] (int i) {
        assert((0<=i) && (i<=2));
        return *(((float *)&vec) + i);
    }
    float& operator [] (int i) const {
        assert((0<=i) && (i<=2));
        return *(((float *)&vec) + i);
    }

    // Operators and member functions:

    friend SPVector operator * (const SPVector3&, const SPMatrix&);
    friend float operator * (const SPVector3&, const SPVector3&);           // Dot Product
    friend SPVector3 operator % (const SPVector3&, const SPVector3&);       // Cross Product
    friend SPVector3 operator * (const SPVector3&, const float);
    friend SPVector3 operator * (const float, const SPVector3&);
    friend SPVector3 operator | (const SPVector3&, const SPVector3&);       // Elements Product
    friend SPVector3 operator + (const SPVector3&);
    friend SPVector3 operator + (const SPVector3&, const SPVector3&);
    friend SPVector3 operator - (const SPVector3&);
    friend SPVector3 operator - (const SPVector3&, const SPVector3&);
    friend SPVector3 operator ~ (const SPVector3&);                         // Normalize

    SPVector3& operator *= (const SPMatrix&);
    SPVector3& operator *= (const float);
    SPVector3& operator |= (const SPVector3&);
    SPVector3& operator += (const SPVector3&);
    SPVector3& operator -= (const SPVector3&);

    float Length();
    SPVector3& Normalize();
};

#pragma pack(pop) 


// Stand alone constructors:

#ifndef MATRICES_H__
SPMatrix ZeroMatrix();
SPMatrix IdentityMatrix();
SPMatrix TranslateMatrix(const float dx, const float dy, const float dz);
SPMatrix ScaleMatrix(const float a, const float b, const float c);
SPMatrix ScaleMatrix(const float a);
SPMatrix RotateXMatrix(const float rads);
SPMatrix RotateYMatrix(const float rads);
SPMatrix RotateZMatrix(const float rads);

#ifdef USE_AMLIB    // Aproximations using AMaths //
SPMatrix RotateXMatrixA(const float rads);
SPMatrix RotateYMatrixA(const float rads);
SPMatrix RotateZMatrixA(const float rads);
#endif
#endif


// Other functions:

void MatrixMult(const SPMatrix& A, const SPMatrix& B, SPMatrix& Res);
void VectorMult(const SPVector& Vec, const SPMatrix& Mat, SPVector& Res);
SPMatrix MatrixMult(const SPMatrix& A, const SPMatrix& B);

inline SPMatrix MatrixInverse(SPMatrix& m)      { SPMatrix l=m; l.Inverse(); return l; }
inline float Determinant(SPMatrix& m)           { return m.Determinant(); }
inline SPMatrix MatrixTranspose(SPMatrix& m)    { SPMatrix l=m; l.Transpose(); return l; }


// Some useful macros:

extern const __int32 __MASKSIGNs_[4];
#define _MASKSIGNs_ (*(F32vec4*)&__MASKSIGNs_)

#define _mm_ror_ps(vec,i)   (((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))) : (vec))
#define _mm_rol_ps(vec,i)   (((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))) : (vec))
#define _mm_abs_ps(vec)     _mm_andnot_ps(_MASKSIGNs_,vec)
#define _mm_neg_ps(vec)     _mm_xor_ps(_MASKSIGNs_,vec)


// Include all the inlined functions
#include "SPMatrix.inl"


#endif // SPMATRIX_H__
