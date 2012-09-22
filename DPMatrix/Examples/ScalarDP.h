//////////////////////////////////////////////////////////////////////////////
//
// SCALARDP.H
//
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

#ifndef SCALARDP_H__
#define SCALARDP_H__

#pragma pack(push,16) /* Must ensure class & union 16-B aligned */


class ScalarDP {
public:
    struct {
        double  _11, _12, _13, _14;
        double  _21, _22, _23, _24;
        double  _31, _32, _33, _34;
        double  _41, _42, _43, _44;
    };

// Constructors and convertions...
    ScalarDP() {}
    ScalarDP( double _11, double _12, double _13, double _14,
              double _21, double _22, double _23, double _24,
              double _31, double _32, double _33, double _34,
              double _41, double _42, double _43, double _44 );

    double& operator() (int i, int j) {
        assert((0<=i) && (i<=3) && (0<=j) && (j<=3));
        return *(((double *)&_11) + (i<<2)+j);
    }

    ScalarDP& operator= (const ScalarDP& a);

    ScalarDP& operator *= (const ScalarDP&);
    ScalarDP& operator *= (double);
    ScalarDP& operator /= (double);
    ScalarDP& operator += (const ScalarDP&);

    friend ScalarDP operator * (const ScalarDP&, const ScalarDP&);

    double Inverse();

    // Other Constructors:
    void IdentityMatrix();
};


inline ScalarDP::ScalarDP( double d11, double d12, double d13, double d14,
                           double d21, double d22, double d23, double d24,
                           double d31, double d32, double d33, double d34,
                           double d41, double d42, double d43, double d44 )
{
    _11 = d11; _12 = d12; _13 = d13; _14 = d14;
    _21 = d21; _22 = d22; _23 = d23; _24 = d24;
    _31 = d31; _32 = d32; _33 = d33; _34 = d34;
    _41 = d41; _42 = d42; _43 = d43; _44 = d44;
}

inline ScalarDP& ScalarDP::operator = (const ScalarDP& a)
{
    _11 = a._11; _12 = a._12; _13 = a._13; _14 = a._14;
    _21 = a._21; _22 = a._22; _23 = a._23; _24 = a._24;
    _31 = a._31; _32 = a._32; _33 = a._33; _34 = a._34;
    _41 = a._41; _42 = a._42; _43 = a._43; _44 = a._44;
    return *this;
}

inline ScalarDP& ScalarDP::operator *= (const ScalarDP& a)
{
    double t11, t12, t13, t14;
    double t21, t22, t23, t24;
    double t31, t32, t33, t34;
    double t41, t42, t43, t44;

    t11 = _11*a._11 + _12*a._21 + _13*a._31 + _14*a._41;
    t12 = _11*a._12 + _12*a._22 + _13*a._32 + _14*a._42;
    t13 = _11*a._13 + _12*a._23 + _13*a._33 + _14*a._43;
    t14 = _11*a._14 + _12*a._24 + _13*a._34 + _14*a._44;

    t21 = _21*a._11 + _22*a._21 + _23*a._31 + _24*a._41;
    t22 = _21*a._12 + _22*a._22 + _23*a._32 + _24*a._42;
    t23 = _21*a._13 + _22*a._23 + _23*a._33 + _24*a._43;
    t24 = _21*a._14 + _22*a._24 + _23*a._34 + _24*a._44;

    t31 = _31*a._11 + _32*a._21 + _33*a._31 + _34*a._41;
    t32 = _31*a._12 + _32*a._22 + _33*a._32 + _34*a._42;
    t33 = _31*a._13 + _32*a._23 + _33*a._33 + _34*a._43;
    t34 = _31*a._14 + _32*a._24 + _33*a._34 + _34*a._44;

    t41 = _41*a._11 + _42*a._21 + _43*a._31 + _44*a._41;
    t42 = _41*a._12 + _42*a._22 + _43*a._32 + _44*a._42;
    t43 = _41*a._13 + _42*a._23 + _43*a._33 + _44*a._43;
    t44 = _41*a._14 + _42*a._24 + _43*a._34 + _44*a._44;

    _11 = t11; _12 = t12; _13 = t13; _14 = t14;
    _21 = t21; _22 = t22; _23 = t23; _24 = t24;
    _31 = t31; _32 = t32; _33 = t33; _34 = t34;
    _41 = t41; _42 = t42; _43 = t43; _44 = t44;

    return *this;
}

inline ScalarDP operator * (const ScalarDP& A, const ScalarDP& B) 
{
    ScalarDP res;

    res._11 = A._11*B._11 + A._12*B._21 + A._13*B._31 + A._14*B._41;
    res._12 = A._11*B._12 + A._12*B._22 + A._13*B._32 + A._14*B._42;
    res._13 = A._11*B._13 + A._12*B._23 + A._13*B._33 + A._14*B._43;
    res._14 = A._11*B._14 + A._12*B._24 + A._13*B._34 + A._14*B._44;

    res._21 = A._21*B._11 + A._22*B._21 + A._23*B._31 + A._24*B._41;
    res._22 = A._21*B._12 + A._22*B._22 + A._23*B._32 + A._24*B._42;
    res._23 = A._21*B._13 + A._22*B._23 + A._23*B._33 + A._24*B._43;
    res._24 = A._21*B._14 + A._22*B._24 + A._23*B._34 + A._24*B._44;

    res._31 = A._31*B._11 + A._32*B._21 + A._33*B._31 + A._34*B._41;
    res._32 = A._31*B._12 + A._32*B._22 + A._33*B._32 + A._34*B._42;
    res._33 = A._31*B._13 + A._32*B._23 + A._33*B._33 + A._34*B._43;
    res._34 = A._31*B._14 + A._32*B._24 + A._33*B._34 + A._34*B._44;

    res._41 = A._41*B._11 + A._42*B._21 + A._43*B._31 + A._44*B._41;
    res._42 = A._41*B._12 + A._42*B._22 + A._43*B._32 + A._44*B._42;
    res._43 = A._41*B._13 + A._42*B._23 + A._43*B._33 + A._44*B._43;
    res._44 = A._41*B._14 + A._42*B._24 + A._43*B._34 + A._44*B._44;

    return res;
}

inline ScalarDP& ScalarDP::operator *= (double d)
{
    _11 *= d; _12 *= d; _13 *= d; _14 *= d;
    _21 *= d; _22 *= d; _23 *= d; _24 *= d;
    _31 *= d; _32 *= d; _33 *= d; _34 *= d;
    _41 *= d; _42 *= d; _43 *= d; _44 *= d;
    return *this;
}

inline ScalarDP& ScalarDP::operator /= (double d)
{
    d = 1.0/d;
    _11 *= d; _12 *= d; _13 *= d; _14 *= d;
    _21 *= d; _22 *= d; _23 *= d; _24 *= d;
    _31 *= d; _32 *= d; _33 *= d; _34 *= d;
    _41 *= d; _42 *= d; _43 *= d; _44 *= d;
    return *this;
}

inline ScalarDP& ScalarDP::operator += (const ScalarDP& a)
{
    _11 += a._11; _12 += a._12; _13 += a._13; _14 += a._14; 
    _21 += a._21; _22 += a._22; _23 += a._23; _24 += a._24; 
    _31 += a._31; _32 += a._32; _33 += a._33; _34 += a._34; 
    _41 += a._41; _42 += a._42; _43 += a._43; _44 += a._44; 
    return *this;
}

inline void ScalarDP::IdentityMatrix() 
{
    _11 = _22 = _33 = _44 = 1.0;
    _12 = _13 = _14 = _21 = 0.0;
    _23 = _24 = _31 = _32 = 0.0;
    _34 = _41 = _42 = _43 = 0.0;
}


inline double ScalarDP::Inverse() 
{ 
    double dB = _13*_24 - _14*_23;
    double dC = _31*_42 - _32*_41;

    double DC11 = _44*_31 - _34*_41;
    double DC12 = _44*_32 - _34*_42;
    double DC21 = _33*_41 - _43*_31;
    double DC22 = _33*_42 - _43*_32;

    double dD = _33*_44 - _34*_43;
    double dA = _11*_22 - _12*_21;

    double AB11 = _22*_13 - _12*_23;
    double AB12 = _22*_14 - _12*_24;
    double AB21 = _11*_23 - _21*_13;
    double AB22 = _11*_24 - _21*_14;
    double tr = AB11*DC11 + AB12*DC21 + AB21*DC12 + AB22*DC22;
    double det = dA*dD + dB*dC - tr;

    double CAB11 = _31*AB11 + _32*AB21;
    double CAB12 = _31*AB12 + _32*AB22;
    double CAB21 = _41*AB11 + _42*AB21;
    double CAB22 = _41*AB12 + _42*AB22;

    double iD11 = _33*dA - CAB11;
    double iD12m = CAB12 - _34*dA;
    double iD21m = CAB21 - _43*dA;
    double iD22 = _44*dA - CAB22;

    double BDC11 = _13*DC11 + _14*DC21;
    double BDC12 = _13*DC12 + _14*DC22;
    double BDC21 = _23*DC11 + _24*DC21;
    double BDC22 = _23*DC12 + _24*DC22;

    double iA11 = _11*dD - BDC11;
    double iA12m = BDC12 - _12*dD;
    double iA21m = BDC21 - _21*dD;
    double iA22 = _22*dD - BDC22;

    double rd = 1.0/det;

    double DBA11 = _33*AB22 - _34*AB21;
    double DBA12 = _34*AB11 - _33*AB12;
    double DBA21 = _43*AB22 - _44*AB21;
    double DBA22 = _44*AB11 - _43*AB12;

    double ACD11 = _11*DC22 - _12*DC21;
    double ACD12 = _12*DC11 - _11*DC12;
    double ACD21 = _21*DC22 - _22*DC21;
    double ACD22 = _22*DC11 - _21*DC12;

    double iB11 = _31*dB - DBA11;
    double iB12m = DBA12 - _32*dB;
    double iB21m = DBA21 - _41*dB;
    double iB22 = _42*dB - DBA22;
    
    double iC11 = _13*dC - ACD11;
    double iC12m = ACD12 - _14*dC;
    double iC21m = ACD21 - _23*dC;
    double iC22 = _24*dC - ACD22;

    _11 = iA22 * rd;
    _12 = iA12m * rd;
    _21 = iA21m * rd;
    _22 = iA11 * rd;
    
    _13 = iB22 * rd;
    _14 = iB12m * rd;
    _23 = iB21m * rd;
    _24 = iB11 * rd;
    
    _31 = iC22 * rd;
    _32 = iC12m * rd;
    _41 = iC21m * rd;
    _42 = iC11 * rd;
    
    _33 = iD22 * rd;
    _34 = iD12m * rd;
    _43 = iD21m * rd;
    _44 = iD11 * rd;

    return det;
}


#endif // ScalarDP_H__
