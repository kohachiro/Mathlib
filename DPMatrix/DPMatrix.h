//////////////////////////////////////////////////////////////////////////////
//
// DPMATRIX.H
//
//  Declaration of the DPMatrix, DPVector and the DPVector3 classes.
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

#ifndef DPMATRIX_H__
#define DPMATRIX_H__

// General switches that control the behaviour of the library:
//#define ZERO_SINGULAR     // Does Inverse() of a singular matrix zero the matrix?
//#define ZERO_VECTOR       // Does normalization of zero vector zero the vector?


#if defined(SPMATRIX_H__) && !defined(MATRICES_H__)
#error Must include MATRICES.H when combining both SPMatrix and DPMatrix classes.
#endif

#ifndef __ICL
#pragma message( "This file should be compiled with Intel C/C++ Compiler" )
#endif

#include <dvec.h>
#include "F64vec4.h"
#pragma pack(push,16) /* Must ensure class & union 16-B aligned */


class DPMatrix;
class DPVector;
class DPVector3;


// ----------------------------------------------------------------------------
//  Name:   DPMatrix
//  Desc:   A matrix of 4x4 doubles.
// ----------------------------------------------------------------------------
class DPMatrix {
public:
    union {
        struct {
            __m256d _L1, _L2, _L3, _L4;
        };
        struct {
            __m128d _L1a, _L1b, 
                    _L2a, _L2b, 
                    _L3a, _L3b, 
                    _L4a, _L4b;
        };
        struct {
            double  _11, _12, _13, _14;
            double  _21, _22, _23, _24;
            double  _31, _32, _33, _34;
            double  _41, _42, _43, _44;
        };
    };

    // Constructors and convertions:

    DPMatrix() {}
    DPMatrix(const DPMatrix&m) : _L1a(m._L1a), _L1b(m._L1b), _L2a(m._L2a), _L2b(m._L2b), 
        _L3a(m._L3a), _L3b(m._L3b), _L4a(m._L4a), _L4b(m._L4b) {}
    DPMatrix(double _11, double _12, double _13, double _14,
             double _21, double _22, double _23, double _24,
             double _31, double _32, double _33, double _34,
             double _41, double _42, double _43, double _44);
    DPMatrix(const double *arr);

    DPMatrix& operator = (const DPMatrix&a) {
        _L1 = a._L1; _L2 = a._L2; _L3 = a._L3; _L4 = a._L4;
        return *this;
    }

    // Accessing elements:

    double& operator () (int i, int j) {
        assert((0<=i) && (i<=3) && (0<=j) && (j<=3));
        return *(((double *)&_11) + (i<<2)+j);
    }
    F64vec4& operator () (int i) {
        assert((0<=i) && (i<=3));
        return *(((F64vec4 *)&_11) + i);
    }
    F64vec4& operator [] (int i) {
        assert((0<=i) && (i<=3));
        return *(((F64vec4 *)&_11) + i);
    }
    F64vec4& operator [] (int i) const {
        assert((0<=i) && (i<=3));
        return *(((F64vec4 *)&_11) + i);
    }

    // Operators and member functions:

    friend DPMatrix operator * (const DPMatrix&, const DPMatrix&);
    friend DPMatrix operator + (const DPMatrix&, const DPMatrix&);
    friend DPMatrix operator - (const DPMatrix&, const DPMatrix&);
    friend DPMatrix operator + (const DPMatrix&);
    friend DPMatrix operator - (const DPMatrix&);
    friend DPMatrix operator * (const DPMatrix&, const double);
    friend DPMatrix operator * (const double, const DPMatrix&);

    DPMatrix& operator *= (const DPMatrix&);
    DPMatrix& operator *= (const double);
    DPMatrix& operator += (const DPMatrix&);
    DPMatrix& operator -= (const DPMatrix&);

    void    Transpose();    // Transposes the matrix
    double  Inverse();      // Inverses the matrix and returns the determinant

    double  Determinant();  // Returns the determinant
    double  MinValue();     // Returns the minimum absolute value of the matrix
    double  MaxValue();     // Returns the maximum absolute value of the matrix

    // Other Constructors:

    void ZeroMatrix();
    void IdentityMatrix();
    void TranslateMatrix(const double dx, const double dy, const double dz);
    void ScaleMatrix(const double a, const double b, const double c);
    void ScaleMatrix(const double a);
    void RotateXMatrix(const double rads);
    void RotateYMatrix(const double rads);
    void RotateZMatrix(const double rads);
};

// ----------------------------------------------------------------------------
//  Name:   DPVector, DPVector4
//  Desc:   A vector of 4 doubles.
// ----------------------------------------------------------------------------
class DPVector {
public:
    union {
        __m256d vec;
        struct {
            __m128d xy, zw;
        };
        struct { 
            double  x,y,z,w;
        };
    };

    // Constructors and convertions:

    DPVector() {}
    DPVector(const DPVector& v) : vec(v.vec) {}
    DPVector(const F64vec4& m) : vec(m) {}
    DPVector(const double x, const double y, const double z, const double w=0.0f) : vec(F64vec4(w,z,y,x)) {}
    DPVector(const double* arr) : xy(_mm_loadu_pd(arr)), zw(_mm_loadu_pd(arr+2)) {}
    operator F64vec4() const { return vec; }

    DPVector& operator = (const DPVector& a) { xy = a.xy; zw = a.zw; return *this; }
    DPVector& operator = (const F64vec4& a) { xy = a.vec1; zw = a.vec2; return *this; }
    DPVector& operator = (const DPVector3&);

    // Accessing elements:

    double& operator () (int i) {
        assert((0<=i) && (i<=3));
        return *(((double *)&vec) + i);
    }
    double& operator [] (int i) {
        assert((0<=i) && (i<=3));
        return *(((double *)&vec) + i);
    }
    double& operator [] (int i) const {
        assert((0<=i) && (i<=3));
        return *(((double *)&vec) + i);
    }

    // Operators and member functions:

    friend DPVector operator * (const DPVector&, const DPMatrix&);
    friend double operator * (const DPVector&, const DPVector&);            // Dot Product
    friend DPVector operator % (const DPVector&, const DPVector&);          // Cross Product
    friend DPVector operator * (const DPVector&, const double);
    friend DPVector operator * (const double, const DPVector&);
    friend DPVector operator | (const DPVector&, const DPVector&);          // Elements Product
    friend DPVector operator + (const DPVector&);
    friend DPVector operator + (const DPVector&, const DPVector&);
    friend DPVector operator - (const DPVector&);
    friend DPVector operator - (const DPVector&, const DPVector&);
    friend DPVector operator ~ (const DPVector&);                           // Normalize

    DPVector& operator *= (const DPMatrix&);
    DPVector& operator *= (const double);
    DPVector& operator |= (const DPVector&);
    DPVector& operator += (const DPVector&);
    DPVector& operator -= (const DPVector&);

    double Length();
    DPVector& Normalize();
};
#define DPVector4 DPVector

// ----------------------------------------------------------------------------
//  Name:   DPVector3
//  Desc:   A vector of 3 doubles.
// ----------------------------------------------------------------------------
class DPVector3 {
public:
    union {
        __m256d vec;
        struct {
            __m128d xy, z_;
        };
        struct { 
            double  x,y,z;
        //  double  _spacer_;
        };
    };

    // Constructors and convertions:

    DPVector3() {}
    DPVector3(const DPVector3& v) : vec(v.vec) {}
    DPVector3(const F64vec4& m) : vec(m) {}
    DPVector3(const double x, const double y, const double z) : vec(F64vec4(1.0,z,y,x)) {}
    DPVector3(const double* arr) : xy(_mm_loadu_pd(arr)), z_(_mm_load_sd(arr+2)) {}

    DPVector3& operator = (const DPVector3& a) { xy = a.xy; z_ = a.z_; return *this; }
    DPVector3& operator = (const DPVector& a);
    DPVector3& operator = (const F64vec4& a) { xy = a.vec1; z_ = a.vec2; return *this; }

    // Accessing elements:

    double& operator () (int i) {
        assert((0<=i) && (i<=2));
        return *(((double *)&vec) + i);
    }
    double& operator [] (int i) {
        assert((0<=i) && (i<=2));
        return *(((double *)&vec) + i);
    }
    double& operator [] (int i) const {
        assert((0<=i) && (i<=2));
        return *(((double *)&vec) + i);
    }

    // Operators and member functions:

    friend DPVector operator * (const DPVector3&, const DPMatrix&);
    friend double operator * (const DPVector3&, const DPVector3&);        // Dot Product
    friend DPVector3 operator % (const DPVector3&, const DPVector3&);     // Cross Product
    friend DPVector3 operator * (const DPVector3&, const double);
    friend DPVector3 operator * (const double, const DPVector3&);
    friend DPVector3 operator | (const DPVector3&, const DPVector3&);     // Elements Product
    friend DPVector3 operator + (const DPVector3&);
    friend DPVector3 operator + (const DPVector3&, const DPVector3&);
    friend DPVector3 operator - (const DPVector3&);
    friend DPVector3 operator - (const DPVector3&, const DPVector3&);
    friend DPVector3 operator ~ (const DPVector3&);                        // Normalize

    DPVector3& operator *= (const DPMatrix&);
    DPVector3& operator *= (const double);
    DPVector3& operator |= (const DPVector3&);
    DPVector3& operator += (const DPVector3&);
    DPVector3& operator -= (const DPVector3&);

    double Length();
    DPVector3& Normalize();

    friend void VectorMult(const DPVector3& Vec, const DPMatrix& Mat, DPVector& Res);
    friend void VectorMult(const DPVector3& Vec, const DPMatrix& Mat, DPVector3& Res);
    friend DPVector VectorMult(const DPVector3& Vec, const DPMatrix& Mat);
};

#pragma pack(pop) 


// Stand alone constructors:

#ifndef MATRICES_H__
DPMatrix ZeroMatrix();
DPMatrix IdentityMatrix();
DPMatrix TranslateMatrix(const double dx, const double dy, const double dz);
DPMatrix ScaleMatrix(const double a, const double b, const double c);
DPMatrix ScaleMatrix(const double a);
DPMatrix RotateXMatrix(const double rads);
DPMatrix RotateYMatrix(const double rads);
DPMatrix RotateZMatrix(const double rads);
#endif

// Other functions:

inline void MatrixMult(const DPMatrix& A, const DPMatrix& B, DPMatrix& Res);
inline DPMatrix MatrixMult(const DPMatrix& A, const DPMatrix& B);
inline void VectorMult(const DPVector& Vec, const DPMatrix& Mat, DPVector& Res);
inline DPVector VectorMult(const DPVector& Vec, const DPMatrix& Mat);

inline DPMatrix MatrixInverse(DPMatrix& m)      { DPMatrix l=m; l.Inverse(); return l; }
inline double Determinant(DPMatrix& m)          { return m.Determinant(); }
inline DPMatrix MatrixTranspose(DPMatrix& m)    { DPMatrix l=m; l.Transpose(); return l; }


// Some useful macros:

extern const __int64 __MASKSIGNd_[2];
#define _MASKSIGNd_ (*(F64vec2*)&__MASKSIGNd_)

#define _mm_abs_pd(vec)     _mm_andnot_pd(_MASKSIGNd_,vec)
#define _mm_neg_pd(vec)     _mm_xor_pd(_MASKSIGNd_,vec)


// Include all the inline functions
#include "DPMatrix.inl"


#endif // DPMATRIX_H__
