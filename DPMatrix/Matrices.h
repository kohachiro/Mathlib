//////////////////////////////////////////////////////////////////////////////
//
// MATRICES.H
//
//  Includes the both the DP* classes and the SP* classes together with 
//  conversion function between the classes.
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

#ifndef MATRICES_H__
#define MATRICES_H__

#include "SPMatrix.h"
#include "DPMatrix.h"

DPMatrix Expand(const SPMatrix &);
SPMatrix Shrink(const DPMatrix &);

DPVector Expand(const SPVector &);
SPVector Shrink(const DPVector &);

DPVector3 Expand(const SPVector3 &);
SPVector3 Shrink(const DPVector3 &);

#endif // MATRICES_H__