/////////////////////////////////////////////////////////////////////////////
// The Software is provided "AS IS" and possibly with with faults. Intel
// disclaims any and all warranties and guarantees, express, implied or
// otherwise, arising, with respect to the software delivered hereunder,
// including but not limited to the warranty of merchantability, the warranty
// of fitness for a particular purpose, and any warranty of non-infringement
// of the intellectual property rights of any third party.
// Intel neither assumes nor authorizes any person to assume for it any other
// liability.  Customer will use the software at its own risk.  Intel will not
// be liable to customer for any direct or indirect damages incurred in using
// the software.  In no event will Intel be liable for loss of profits, loss of
// use, loss of data, business interruption, nor for punitive, incidental,
// consequential, or special damages of any kind, even if advised of
// the possibility of such damages.
// 
// Copyright (c) 1998 - 2000 Intel Corporation
//
// Third-party brands and names are the property of their respective owners
//
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

BOOL CanReadTsc(void);

void ReadTsc(LONGLONG* pTsc);
void ReadTscSync(LONGLONG* pTsc);

#if _MSC_VER >= 1200

#define _ReadTsc(/*LONGLONG*/ pTsc) \
	__asm \
	{ \
		__asm rdtsc \
		__asm mov dword ptr pTsc, eax \
		__asm mov dword ptr pTsc[4], edx \
	}

#define _ReadTscSync(/*LONGLONG*/ pTsc) \
	__asm \
	{ \
		__asm push ebx \
		__asm cpuid \
		__asm rdtsc \
		__asm mov dword ptr pTsc, eax \
		__asm mov dword ptr pTsc[4], edx \
		__asm pop ebx \
	}

#endif // _MSC_VER >= 1200

#ifdef __cplusplus
}
#endif
