Approximate Math (AM) Library
Release 2.0, October 2000
ReadMe File
Copyright (c) Intel Corporation 1998 - 2000

To read this document using Windows* Notepad, please
enable the Word Wrap feature (menu Edit | Word Wrap)

What Is AM Library?
===================
Ever missed a sine or arctangent instruction among Intel Streaming SIMD Extensions? Ever wished there were a way to calculate logarithm or exponent in about a dozen cycles? Here is a new release of

Approximate Math Library (AM Library) -- a set of fast routines to calculate math functions using Intel(R) Streaming SIMD Extensions (SSE) and Streaming SIMD Extensions 2 (SSE2). The Library offers trigonometric, reverse trigonometric, logarithmic, and exponential functions for packed and scalar arguments. The processing speed is many times faster than that of x87 instructions and even of table lookups. The accuracy of AM Library routines can be adequate for many applications. It is comparable with that of reciprocal SSE instructions, and is hundreds times better than what is achievable with lookup tables.

The AM Library is provided along with the full source code and a usage sample.

 
Package Content
===============
License.pdf	-- legal information. Please read before using AM library!
ReadMe.txt	-- this document
AMaths.pdf	-- AM Library feature description and documentation
AMaths.lib	-- AM Library binary file
AMaths.h	-- the header file for AM Library
AMaths.c	-- the source code of AM Library
AMaths.dsw, AMaths.dsp -- Microsoft* Visual C++* 6.0 project files necessary to build AM library
MathTest.exe	-- test and usage sample of AM library, release build
MathTest.cpp	-- test and usage sample of AM library, source code
MathTest.dsw, MathTest.dsp -- Microsoft* Visual C++* 6.0 project files necessary to build MathTest sample
ReadTsc.h, ReadTsc.obj -- support files for MathTest sample


What's New in This Release
==========================
1. Routines Optimized for Intel(R) Pentium(R) 4 Processor and SSE2.
While all AM Library routines work on Pentium 4 processor, a version optimized for Pentium 4 and SSE2 is now provided for every AM Library routine. It is recommended to use these versions in code optimized for Pentium 4 processor since they provide a significant speedup on this processor over the generic SSE version.

2. Further Performance Optimization
Most AM Library routines have been further optimized for the latest Intel(R) Pentium(R) III and Pentium 4 processors.

3. New Routines
The new AM Library sincos functions (am_sincos_xs) calculate both sine and cosine for the same argument. Calling an AM Library sincos function is faster then calling a sin and a cos function separately.

4. Documented Valid Argument Ranges
The valid argument ranges and the handling of invalid arguments have been established and documented for all AM Library functions. This resolved the following issues reported for the previous releases:
- incorrect handling of large positive and negative arguments by the exponential functions (am_exp_ps, am_exp_ss, am_exp2_ps, am_exp2_ss)
- incorrect handling of small positive arguments by the power functions (am_pow_ps, am_pow_ss)
- inconsistent handling of zero arguments by the power functions (am_pow_ps, am_pow_ss)
- incorrect handling of large positive and negative arguments by the trigonometric functions
Please refer to AMaths.pdf for the detailed information on valid argument ranges.


Information in this document is provided in connection with Intel products. No license, express or implied, by estoppel or otherwise, to any intellectual property rights is granted by this document. Except as provided in Intel's Terms and Conditions of Sale for such products, Intel assumes no liability whatsoever, and Intel disclaims any express or implied warranty, relating to sale and/or use of Intel products including liability or warranties relating to fitness for a particular purpose, merchantability, or infringement of any patent, copyright or other intellectual property right. Intel products are not intended for use in medical, life saving, or life sustaining applications. Intel may make changes to specifications and product descriptions at any time, without notice.

The Software described in this document is provided "AS IS" and possibly with faults. Intel disclaims any and all warranties and guarantees, express, implied or otherwise, arising, with respect to the software delivered hereunder, including but not limited to the warranty of merchantability, the warranty of fitness for a particular purpose, and any warranty of non-infringement of the intellectual property rights of any third party.  Intel neither assumes nor authorizes any person to assume for it any other liability.  Customer will use the software at its own risk.  Intel will not be liable to customer for any direct or indirect damages incurred in using the software.  In no event will Intel be liable for loss of profits, loss of use, loss of data, business interruption, nor for punitive, incidental, consequential, or special damages of any kind, even if advised of the possibility of such damages.

Copyright (c) Intel Corporation 1998 - 2000

*Third-party brands and names are the property of their respective owners
