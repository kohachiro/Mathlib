//////////////////////////////////////////////////////////////////////////////
//
// MATRICES.CPP
//
//  Conversion between the DP* classes and the SP* classes.
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

#include "Matrices.h"


// ----------------------------------------------------------------------------
//  Name:   Expand
//  Desc:   Expands SPMatrix into DPMatrix.
// ----------------------------------------------------------------------------
DPMatrix Expand(const SPMatrix &src) 
{
    DPMatrix dst;
    dst._L1a = _mm_cvtps_pd(src._L1);
    dst._L1b = _mm_cvtps_pd(_mm_movehl_ps(src._L1,src._L1));
    dst._L2a = _mm_cvtps_pd(src._L2);
    dst._L2b = _mm_cvtps_pd(_mm_movehl_ps(src._L2,src._L2));
    dst._L3a = _mm_cvtps_pd(src._L3);
    dst._L3b = _mm_cvtps_pd(_mm_movehl_ps(src._L3,src._L3));
    dst._L4a = _mm_cvtps_pd(src._L4);
    dst._L4b = _mm_cvtps_pd(_mm_movehl_ps(src._L4,src._L4));
    return dst;
}

// ----------------------------------------------------------------------------
//  Name:   Shrink
//  Desc:   Shrinks DPMatrix into SPMatrix.
// ----------------------------------------------------------------------------
SPMatrix Shrink(const DPMatrix &src)
{
    SPMatrix dst;
    dst._L1 = _mm_movelh_ps(_mm_cvtpd_ps(src._L1a),_mm_cvtpd_ps(src._L1b));
    dst._L2 = _mm_movelh_ps(_mm_cvtpd_ps(src._L2a),_mm_cvtpd_ps(src._L2b));
    dst._L3 = _mm_movelh_ps(_mm_cvtpd_ps(src._L3a),_mm_cvtpd_ps(src._L3b));
    dst._L4 = _mm_movelh_ps(_mm_cvtpd_ps(src._L4a),_mm_cvtpd_ps(src._L4b));
    return dst;
}


// ----------------------------------------------------------------------------
//  Name:   Expand
//  Desc:   Expands SPVector into DPVector.
// ----------------------------------------------------------------------------
DPVector Expand(const SPVector &src) 
{
    DPVector dst;
    dst.xy = _mm_cvtps_pd(src);
    dst.zw = _mm_cvtps_pd(_mm_movehl_ps(src,src));
    return dst;
}

// ----------------------------------------------------------------------------
//  Name:   Shrink
//  Desc:   Shrinks DPVector into SPVector.
// ----------------------------------------------------------------------------
SPVector Shrink(const DPVector &src)
{
    SPVector dst;
    dst = _mm_movelh_ps(_mm_cvtpd_ps(src.xy),_mm_cvtpd_ps(src.zw));
    return dst;
}


// ----------------------------------------------------------------------------
//  Name:   Expand
//  Desc:   Expands SPVector3 into DPVector3.
// ----------------------------------------------------------------------------
DPVector3 Expand(const SPVector3 &src) 
{
    DPVector3 dst;
    dst.xy = _mm_cvtps_pd(src);
    dst.z_ = _mm_cvtss_sd(_mm_setzero_pd(), _mm_movehl_ps(src,src));
    return dst;
}

// ----------------------------------------------------------------------------
//  Name:   Shrink
//  Desc:   Shrinks DPVector3 into SPVector3.
// ----------------------------------------------------------------------------
SPVector3 Shrink(const DPVector3 &src)
{
    SPVector3 dst;
    dst = _mm_movelh_ps(_mm_cvtpd_ps(src.xy),_mm_cvtsd_ss(_mm_setzero_ps(),src.z_));
    return dst;
}

