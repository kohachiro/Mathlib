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
// Approximate Math Library for SSE / SSE2
//  Test and Usage Sample File
//  Version 2.0
//  Author Alex Klimovitski, Intel GmbH
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <xmmintrin.h>

#include "readtsc.h"
#include "AMaths.h"

#pragma optimize("a", on)

//#define LONGTEST
//#define LOGGING

/////////////////////////////////////////////////////////////////////////////
// Number of elements in test array == TEST_COUNT * 4

#ifndef LONGTEST
#ifndef LOGGING
#define TEST_COUNT		1000
#else
#define TEST_COUNT		500
#endif
#else
#define TEST_COUNT		10000
#ifdef LOGGING
#undef LOGGING  // logging doesn't make sense with long test
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Handy defines

#define DECLARE_PRIORITY	\
	HANDLE hProc = GetCurrentProcess();\
	HANDLE hThrd = GetCurrentThread();\
	DWORD dwPrtyCls = GetPriorityClass(hProc);\
	DWORD dwPrty = GetThreadPriority(hThrd)

#define BOOST_PRIORITY	\
	if (dwPrtyCls)\
		if (!SetPriorityClass(hProc, REALTIME_PRIORITY_CLASS  /*HIGH_PRIORITY_CLASS*/))\
			dwPrtyCls = 0;\
	if (dwPrty)\
		if (!SetThreadPriority(hThrd, THREAD_PRIORITY_HIGHEST))\
			dwPrty = 0

#define RESTORE_PRIORITY	\
	if (dwPrty)\
		SetThreadPriority(hThrd, dwPrty);\
	if (dwPrtyCls)\
		SetPriorityClass(hProc, dwPrtyCls)


void enable_FTZ()
{
	unsigned csr = _mm_getcsr();
	csr |= (0x1 << 15); // set FTZ
	_mm_setcsr(csr);
}

/////////////////////////////////////////////////////////////////////////////
// Testing framework

class BaseTest
{
public:
	BaseTest(const char* pszName, float fLimit, float fLimit2 = 0.0f)
	{
		m_fLimit = fLimit;
		m_fLimit2 = fLimit2;
		m_pszName = pszName;
	}

protected:
	virtual float RandVal() const
	{
		return (rand() - (RAND_MAX * 0.5f)) * (m_fLimit / (float)RAND_MAX);
	}
	
	virtual float RandVal2(DWORD hint) const
	{
		return (rand() - (RAND_MAX * 0.5f)) * (m_fLimit2 / (float)RAND_MAX);
	}

	virtual float OrdVal(int i) const
	{
		return -m_fLimit + (2.0f * m_fLimit / (TEST_COUNT * 4)) * i;
	}
	
	virtual float OrdVal2(int i, DWORD hint) const
	{
		return -m_fLimit2 + (2.0f * m_fLimit2 / (TEST_COUNT * 4)) * i;
	}

protected:
	float m_fLimit;
	float m_fLimit2;
	const char* m_pszName;
};

class BaseTestPos : public BaseTest
{
public:
	BaseTestPos(const char* pszName, float fLimit, float fLimit2 = 0.0f) : BaseTest(pszName, fLimit, fLimit2)
	{
	}

protected:
	virtual float RandVal() const
	{
		return (rand()) * (m_fLimit / (float)RAND_MAX);
	}

	virtual float RandVal2(DWORD hint) const
	{
		return (rand()) * (m_fLimit2 / (float)RAND_MAX);
	}

	virtual float OrdVal(int i) const
	{
		return (m_fLimit / (TEST_COUNT * 4)) * i;
	}
	
	virtual float OrdVal2(int i, DWORD hint) const
	{
		return (m_fLimit2 / (TEST_COUNT * 4)) * i;
	}
};

template<class Base, double (*x87func)(double), __m128 (__stdcall *psfunc)(__m128), __m128 (__stdcall *ssfunc)(__m128), float (*tfunc)(float)>
class Test : public Base
{
public:
	Test(const char* pszName, float fLimit) : Base(pszName, fLimit)
	{
	}
	
	int DoTest()
	{
		DECLARE_PRIORITY;

		LONGLONG t1, t2;
		double a, b, c, d;
		
		double ea = 0.0;
		double eb = 0.0;
		double ec = 0.0;
		double ed = 0.0;
		
#ifndef LONGTEST
		__m128 x[TEST_COUNT];
		__m128 r[TEST_COUNT];
		__m128 q[TEST_COUNT];
#else
		__m128* x = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* r = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* q = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
#endif
		float* fx = (float*)x;
		float* fr = (float*)r;
		float* fq = (float*)q;

		// set exact results
		srand(0);
		for (int i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fq[i] = fq[i] = fr[i] = (float)x87func(fx[i]);
		}

		// warmup
		psfunc(x[0]);
		ssfunc(x[0]);
		_m_empty();
		if (tfunc != NULL)
			tfunc(fx[0]);

		// Test A (straight sin())
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		x87func(fx[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fq[i] = (float)x87func((float)fx[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		a = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				ea += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
		}

		// Test B (AM_ps approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		psfunc(x[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT; i++)
		{
			q[i] = psfunc(x[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		b = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				eb += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
		}

		// TestC (AM_ss approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		ssfunc(x[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			__m128 t = ssfunc(_mm_load_ss(&fx[i]));
			_mm_store_ss(&fq[i], t);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		c = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				ec += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
		}

		// Test D (table lookup sin)
		if (tfunc != NULL)
		{
			srand(0);
			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				fx[i] = RandVal();
			}

			BOOST_PRIORITY;
			tfunc(fx[0]);
			ReadTsc(&t1);
			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				fq[i] = tfunc(fx[i]);
			}
			ReadTsc(&t2);
			RESTORE_PRIORITY;
			d = (double)(t2 - t1);

			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				if (fr[i] != 0)
					ed += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
			}
		}

		printf("%s:\n", m_pszName);

		if (tfunc != NULL)
		{
			a /= (TEST_COUNT * 4);
			b /= (TEST_COUNT * 4);
			c /= (TEST_COUNT * 4);
			d /= (TEST_COUNT * 4);

			printf("\tClocks\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", a, b, c, d);

			ea /= (TEST_COUNT * 4);
			eb /= (TEST_COUNT * 4);
			ec /= (TEST_COUNT * 4);
			ed /= (TEST_COUNT * 4);

			printf("\tError,%%\t%g\t\t%g\t%g\t%g\n", ea, eb, ec, ed);
		}
		else
		{
			a /= (TEST_COUNT * 4);
			b /= (TEST_COUNT * 4);
			c /= (TEST_COUNT * 4);

			printf("\tClocks\t%.1f\t\t%.1f\t\t%.1f\n", a, b, c);

			ea /= (TEST_COUNT * 4);
			eb /= (TEST_COUNT * 4);
			ec /= (TEST_COUNT * 4);

			printf("\tError,%%\t%g\t\t%g\t%g\n", ea, eb, ec);
		}

#ifdef LOGGING
		// logging part
		
		char fname[256];
		strcat(strcpy(fname, m_pszName), "_ps.csv");
		FILE* f = fopen(fname, "w");

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = OrdVal(i);
			fr[i] = (float)x87func(fx[i]);
		}

		for (i = 0; i < TEST_COUNT; i++)
		{
			q[i] = psfunc(x[i]);
		}
		_m_empty();

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			double t = fabs(fr[i] - fq[i]);
			fprintf(f, "%g, %g\n", fx[i], t);
		}

		fclose(f);
#endif

#ifdef LONGTEST
		_mm_free(x);
		_mm_free(r);
		_mm_free(q);
#endif

		return 0;
	} // Test::DoTest
}; // class Test

template<class Base, void (*x87func)(float, float*, float*), void (__stdcall *psfunc)(__m128, __m128*, __m128*), void (__stdcall *ssfunc)(__m128, __m128*, __m128*), void (*tfunc)(float, float*, float*)>
class TestTwoVals : public Base
{
public:
	TestTwoVals(const char* pszName, float fLimit) : Base(pszName, fLimit)
	{
	}
	
	int DoTest()
	{
		DECLARE_PRIORITY;

		LONGLONG t1, t2;
		double a, b, c, d;
		
		double ea = 0.0;
		double eb = 0.0;
		double ec = 0.0;
		double ed = 0.0;
		
#ifndef LONGTEST
		__m128 x[TEST_COUNT];
		__m128 r[TEST_COUNT];
		__m128 s[TEST_COUNT];
		__m128 q[TEST_COUNT];
		__m128 p[TEST_COUNT];
#else
		__m128* x = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* r = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* s = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* q = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* p = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
#endif
		float* fx = (float*)x;
		float* fr = (float*)r;
		float* fs = (float*)s;
		float* fq = (float*)q;
		float* fp = (float*)p;

		// set exact results
		srand(0);
		for (int i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			x87func(fx[i], &fr[i], &fs[i]);
			fq[i] = fr[i];
			fp[i] = fs[i];
		}

		// warmup
		psfunc(x[0], &q[0], &p[0]);
		ssfunc(x[0], &q[0], &p[0]);
		_m_empty();
		if (tfunc != NULL)
			tfunc(fx[0], &fq[0], &fp[0]);

		// Test A (straight sin())
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		x87func(fx[0], &fq[0], &fp[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			x87func(fx[i], &fq[i], &fp[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		a = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				ea += (fabs((fr[i] - fq[i]) * 100.0f / fr[i]) * 0.5 + 
					fabs((fs[i] - fp[i]) * 100.0f / fs[i]) * 0.5);
		}

		// Test B (AM_ps approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		psfunc(x[0], &q[0], &p[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT; i++)
		{
			psfunc(x[i], &q[i], &p[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		b = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				eb += (fabs((fr[i] - fq[i]) * 100.0f / fr[i]) * 0.5 + 
					fabs((fs[i] - fp[i]) * 100.0f / fs[i]) * 0.5);
		}

		// TestC (AM_ss approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
		}

		BOOST_PRIORITY;
		ssfunc(x[0], &q[0], &p[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			__m128 t, tt;
			ssfunc(_mm_load_ss(&fx[i]), &t, &tt);
			_mm_store_ss(&fq[i], t);
			_mm_store_ss(&fp[i], tt);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		c = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0)
				ec += (fabs((fr[i] - fq[i]) * 100.0f / fr[i]) * 0.5 + 
					fabs((fs[i] - fp[i]) * 100.0f / fs[i]) * 0.5);
		}

		// Test D (table lookup sin)
		if (tfunc != NULL)
		{
			srand(0);
			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				fx[i] = RandVal();
			}

			BOOST_PRIORITY;
			tfunc(fx[0], &fq[0], &fp[0]);
			ReadTsc(&t1);
			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				tfunc(fx[i], &fq[i], &fp[i]);
			}
			ReadTsc(&t2);
			RESTORE_PRIORITY;
			d = (double)(t2 - t1);

			for (i = 0; i < TEST_COUNT * 4; i++)
			{
				if (fr[i] != 0)
					ed += (fabs((fr[i] - fq[i]) * 100.0f / fr[i]) * 0.5 + 
						fabs((fs[i] - fp[i]) * 100.0f / fs[i]) * 0.5);
			}
		}

		printf("%s:\n", m_pszName);

		if (tfunc != NULL)
		{
			a /= (TEST_COUNT * 4);
			b /= (TEST_COUNT * 4);
			c /= (TEST_COUNT * 4);
			d /= (TEST_COUNT * 4);

			printf("\tClocks\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\n", a, b, c, d);

			ea /= (TEST_COUNT * 4);
			eb /= (TEST_COUNT * 4);
			ec /= (TEST_COUNT * 4);
			ed /= (TEST_COUNT * 4);

			printf("\tError,%%\t%g\t\t%g\t%g\t%g\n", ea, eb, ec, ed);
		}
		else
		{
			a /= (TEST_COUNT * 4);
			b /= (TEST_COUNT * 4);
			c /= (TEST_COUNT * 4);

			printf("\tClocks\t%.1f\t\t%.1f\t\t%.1f\n", a, b, c);

			ea /= (TEST_COUNT * 4);
			eb /= (TEST_COUNT * 4);
			ec /= (TEST_COUNT * 4);

			printf("\tError,%%\t%g\t\t%g\t%g\n", ea, eb, ec);
		}

#ifdef LOGGING
		// logging part
		
		char fname[256];
		strcat(strcpy(fname, m_pszName), "_ps.csv");
		FILE* f = fopen(fname, "w");

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = OrdVal(i);
			fr[i] = (float)x87func(fx[i]);
		}

		for (i = 0; i < TEST_COUNT; i++)
		{
			q[i] = psfunc(x[i]);
		}
		_m_empty();

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			double t = fabs(fr[i] - fq[i]);
			fprintf(f, "%g, %g\n", fx[i], t);
		}

		fclose(f);
#endif

#ifdef LONGTEST
		_mm_free(x);
		_mm_free(r);
		_mm_free(s);
		_mm_free(q);
		_mm_free(p);
#endif

		return 0;
	} // TestTwoVals::DoTest
}; // class TestTwoVals

template<class Base, double (*x87func)(double, double), __m128 (__stdcall *psfunc)(__m128, __m128), __m128 (__stdcall *ssfunc)(__m128, __m128)>
class TestTwoArgs : public Base
{
public:
	TestTwoArgs(const char* pszName, float fLimit, float fLimit2) : Base(pszName, fLimit, fLimit2)
	{
	}

	int DoTest()
	{
		DECLARE_PRIORITY;

		LONGLONG t1, t2;
		double a, b, c;
		
		double ea = 0.0;
		double eb = 0.0;
		double ec = 0.0;
		
#ifndef LONGTEST
		__m128 x[TEST_COUNT];
		__m128 y[TEST_COUNT];
		__m128 r[TEST_COUNT];
		__m128 q[TEST_COUNT];
#else
		__m128* x = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* y = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* r = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
		__m128* q = (__m128*)_mm_malloc(TEST_COUNT * sizeof(__m128), 32);
#endif
		float* fx = (float*)x;
		float* fy = (float*)y;
		float* fr = (float*)r;
		float* fq = (float*)q;

		// set exact results
		srand(0);
		for (int i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
			fy[i] = RandVal2(0);
		}

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fq[i] = fr[i] = (float)x87func(fx[i], fy[i]);
		}

		// warmup
		psfunc(x[0], y[0]);
		ssfunc(x[0], y[0]);
		_m_empty();

		// Test A (straight atan2())
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
			fy[i] = RandVal2(0);
		}

		BOOST_PRIORITY;
		x87func(fx[0], fy[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fq[i] = (float)x87func(fx[i], fy[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		a = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0.0f)
				ea += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
		}

		// Test B (AM_ps approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
			fy[i] = RandVal2(1);
		}

		BOOST_PRIORITY;
		psfunc(x[0], y[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT; i++)
		{
			q[i] = psfunc(x[i], y[i]);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		b = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0.0f)
				eb += fabs(double(fr[i] - fq[i]) * 100.0 / fr[i]);
		}

		// TestC (AM_ss approximation)
		srand(0);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = RandVal();
			fy[i] = RandVal2(2);
		}

		BOOST_PRIORITY;
		ssfunc(x[0], y[0]);
		ReadTsc(&t1);
		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			__m128 t = ssfunc(_mm_load_ss(&fx[i]), _mm_load_ss(&fy[i]));
			_mm_store_ss(&fq[i], t);
		}
		ReadTsc(&t2);
		RESTORE_PRIORITY;
		_m_empty();
		c = (double)(t2 - t1);

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			if (fr[i] != 0.0f)
				ec += fabs((fr[i] - fq[i]) * 100.0f / fr[i]);
		}

		printf("%s:\n", m_pszName);

		a /= (TEST_COUNT * 4);
		b /= (TEST_COUNT * 4);
		c /= (TEST_COUNT * 4);

		printf("\tClocks\t%.1f\t\t%.1f\t\t%.1f\n", a, b, c);

		ea /= (TEST_COUNT * 4);
		eb /= (TEST_COUNT * 4);
		ec /= (TEST_COUNT * 4);

		printf("\tError,%%\t%g\t\t%g\t%g\n", ea, eb, ec);

#ifdef LOGGING
		// logging part
		
		char fname[256];
		strcat(strcpy(fname, m_pszName), "_ps.csv");
		FILE* f = fopen(fname, "w");

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			fx[i] = OrdVal(i);
			fy[i] = OrdVal2(i, 0);
			fr[i] = (float)x87func(fx[i], fy[i]);
			fy[i] = OrdVal2(i, 1);
		}

		for (i = 0; i < TEST_COUNT; i++)
		{
			q[i] = psfunc(x[i], y[i]);
		}
		_m_empty();

		for (i = 0; i < TEST_COUNT * 4; i++)
		{
			double t = fabs(fr[i] - fq[i]);
			fprintf(f, "%g, %g\n", fx[i], t);
		}

		fclose(f);
#endif

#ifdef LONGTEST
		_mm_free(x);
		_mm_free(y);
		_mm_free(r);
		_mm_free(q);
#endif

		return 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
// Table lookup cosine stuff

#define COS_TABLE_SIZE	256
#define COS_2PI_MASK	(COS_TABLE_SIZE * 4 - 1)

static float *CosTable = NULL;

extern "C" unsigned int __cdecl _ftol(float val)
{
	unsigned int temp;
	_asm {
		fistp dword ptr [temp]
		mov eax,[temp]
	};
}

void init_tmath(void)
{
	int i;
	float x = 0.0f;
	float dx = (AM_PI / 2.0f) / (COS_TABLE_SIZE-1);

	CosTable = (float*) malloc( sizeof(float) * COS_TABLE_SIZE );

	// build cosine table (goes from 0 to 1/2 PI)
	for(i = 0;i < COS_TABLE_SIZE;i++)
	{
		CosTable[i] = (float) cos( x );
		x += dx;
	}
}

void uninit_tmath(void)
{
	free(CosTable);
}

float tcos(float val)
{
	ULONG ULVal;
	float Cos;
	ULVal = (ULONG) (val * ( ( COS_TABLE_SIZE * 4) / (2.0f * AM_PI) ) );

	ULVal &= COS_2PI_MASK;

	// negative half of cos table
	if(ULVal >= (COS_TABLE_SIZE * 2) )
	{
		if( ULVal >= (COS_TABLE_SIZE * 3) )
		{
			// last quadrant
			Cos = CosTable[ (COS_TABLE_SIZE * 4) - 1 - ULVal ];
		} else
		{
			// 3rd quadrant
			Cos = - CosTable[ ULVal - (COS_TABLE_SIZE *2) ];
		}
	} else
	{
		if(ULVal >= COS_TABLE_SIZE)
		{
			// 2nd quadrant
			Cos = -CosTable[(COS_TABLE_SIZE * 2) - 1 - ULVal ];
		} else
		{
			// 1st quadrant
			Cos = CosTable[ ULVal ];
		}
	}

	return Cos;
}


__inline float tsin(float val)
{
	return tcos(val - (AM_PI / 2.0f));
}

__inline void tsincos(float val, float* s, float* c)
{
	*s = tsin(val);
	*c = tcos(val);
}

__inline float ttan(float val)
{
	return tsin(val) / tcos(val);
}


/////////////////////////////////////////////////////////////////////////////
// Test sin stuff

typedef Test<BaseTest, sin, am_sin_ps, am_sin_ss, tsin> TestSin;
#ifndef LOGGING
TestSin test_sin("sin", 1e3f);
#else
TestSin test_sin("sin", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test sin stuff

typedef Test<BaseTest, sin, am_sin_eps, am_sin_ess, tsin> TestSinE;
#ifndef LOGGING
TestSinE test_sine("sin-E", 1e3f);
#else
TestSinE test_sine("sin-E", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test cos stuff

typedef Test<BaseTest, cos, am_cos_ps, am_cos_ss, tcos> TestCos;
#ifndef LOGGING
TestCos test_cos("cos", 1e3f);
#else
TestCos test_cos("cos", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test cos stuff

typedef Test<BaseTest, cos, am_cos_eps, am_cos_ess, tcos> TestCosE;
#ifndef LOGGING
TestCosE test_cose("cos-E", 1e3f);
#else
TestCosE test_cose("cos-E", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test sincos stuff

void _sincos(double x, double* s, double* c)
{
	__asm
	{
		fld		qword ptr x
		mov		eax, s
		mov		edx, c
		fsincos
		fstp	qword ptr [edx]
		fstp	qword ptr [eax]
	}
}

void _sincosf(float x, float* s, float* c)
{
	__asm
	{
		fld		dword ptr x
		mov		eax, s
		mov		edx, c
		fsincos
		fstp	dword ptr [edx]
		fstp	dword ptr [eax]
	}
}

typedef TestTwoVals<BaseTest, _sincosf, am_sincos_ps, am_sincos_ss, tsincos> TestSinCos;
#ifndef LOGGING
TestSinCos test_sincos("sincos", 1e3f);
#else
TestSinCos test_sincos("sincos", AM_PI * 2.2f);
#endif

typedef TestTwoVals<BaseTest, _sincosf, am_sincos_eps, am_sincos_ess, tsincos> TestSinCosE;
#ifndef LOGGING
TestSinCosE test_sincose("sincos-E", 1e3f);
#else
TestSinCosE test_sincose("sincos-E", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test tan stuff

typedef Test<BaseTest, tan, am_tan_ps, am_tan_ss, NULL> TestTan;
#ifndef LOGGING
TestTan test_tan("tan", 1e3f);
#else
TestTan test_tan("tan", AM_PI * 2.2f);
#endif

typedef Test<BaseTest, tan, am_tan_eps, am_tan_ess, NULL> TestTanE;
#ifndef LOGGING
TestTanE test_tane("tan-E", 1e3f);
#else
TestTanE test_tane("tan-E", AM_PI * 2.2f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test atan stuff

typedef Test<BaseTest, atan, am_atan_ps, am_atan_ss, NULL> TestAtan;
#ifndef LOGGING
TestAtan test_atan("atan", 1e3f);
#else
TestAtan test_atan("atan", 1e3f);
#endif

typedef Test<BaseTest, atan, am_atan_eps, am_atan_ess, NULL> TestAtanE;
#ifndef LOGGING
TestAtanE test_atane("atan-E", 1e3f);
#else
TestAtanE test_atane("atan-E", 1e3f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test atan2 stuff

template<__m128 (__stdcall *psfunc)(__m128, __m128), __m128 (__stdcall *ssfunc)(__m128, __m128)>
class TestAtan2 : public TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>
{
public:
	TestAtan2(const char* pszName, float fLimit, float fLimit2) : TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>(pszName, fLimit, fLimit2)
	{ }

protected:
	virtual float RandVal2(DWORD hint) const
	{
		if (hint == 0)
			return TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>::RandVal2(hint);
		else
			return 1.0f / TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>::RandVal2(hint);
	}

	virtual float OrdVal2(int i, DWORD hint) const
	{
		if (hint == 0)
			return TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>::OrdVal2(i, hint);
		else
			return 1.0f / TestTwoArgs<BaseTest, atan2, psfunc, ssfunc>::OrdVal2(i, hint);
	}
};

#ifndef LOGGING
TestAtan2<am_atanr2_ps, am_atanr2_ss> test_atan2("atan2", 1e3f, 1e3f);
#else
TestAtan2<am_atanr2_ps, am_atanr2_ss> test_atan2("atan2", 1e3f, 1e3f);
#endif

#ifndef LOGGING
TestAtan2<am_atanr2_eps, am_atanr2_ess> test_atan2e("atan2e", 1e3f, 1e3f);
#else
TestAtan2<am_atanr2_eps, am_atanr2_ess> test_atan2e("atan2e", 1e3f, 1e3f);
#endif

/////////////////////////////////////////////////////////////////////////////
// Test asin stuff

typedef Test<BaseTest, asin, am_asin_ps, am_asin_ss, NULL> TestAsin;
TestAsin test_asin("asin", 1.0f);

/////////////////////////////////////////////////////////////////////////////
// Test acos stuff

typedef Test<BaseTest, acos, am_acos_ps, am_acos_ss, NULL> TestAcos;
TestAcos test_acos("acos", 1.0f);


/////////////////////////////////////////////////////////////////////////////
// Test exp stuff

typedef Test<BaseTest, exp, am_exp_ps, am_exp_ss, NULL> TestExp;
TestExp test_exp("exp", 88.0f);

typedef Test<BaseTest, exp, am_exp_eps, am_exp_ess, NULL> TestExpE;
TestExpE test_expe("exp-E", 88.0f);

/////////////////////////////////////////////////////////////////////////////
// Test exp2 stuff

double exp2(double x)
{
//	return pow(2.0, x);
	__asm
	{
		fld		x					// x
		frndint						// i
		fld		x					// x, i
		fsub	st, st(1)			// f, i

		f2xm1						// 2^f - 1, i
		
		fld1						// 1, 2^f - 1, i
		fadd						// 2^f, i
		fscale						// 2^f * 2^i, i
		fstp	st(1)				// 2^f * 2^i - 1
	}
}

typedef Test<BaseTest, exp2, am_exp2_ps, am_exp2_ss, NULL> TestExp2;
TestExp2 test_exp2("exp2", 127.0f);

typedef Test<BaseTest, exp2, am_exp2_eps, am_exp2_ess, NULL> TestExp2E;
TestExp2E test_exp2e("exp2-E", 127.0f);

/////////////////////////////////////////////////////////////////////////////
// Test log stuff

typedef Test<BaseTestPos, log, am_log_ps, am_log_ss, NULL> TestLog;
TestLog test_log("log", 1e3f);

typedef Test<BaseTestPos, log, am_log_eps, am_log_ess, NULL> TestLogE;
TestLogE test_loge("log-E", 1e3f);

/////////////////////////////////////////////////////////////////////////////
// Test log2 stuff

double log2(double x)
{
//	static const double rlog2 = 1.0f / log(2.0f);
//	return log(x) * rlog2;
	__asm
	{
		fld1
		fld		x
		fyl2x
	}
}

typedef Test<BaseTestPos, log2, am_log2_ps, am_log2_ss, NULL> TestLog2;
TestLog2 test_log2("log2", 1e3f);

typedef Test<BaseTestPos, log2, am_log2_eps, am_log2_ess, NULL> TestLog2E;
TestLog2E test_log2e("log2-E", 1e3f);

/////////////////////////////////////////////////////////////////////////////
// Test pow stuff

class TestPow : public TestTwoArgs<BaseTestPos, pow, am_pow_ps, am_pow_ss>
{
public:
	TestPow(float fLimit, float fLimit2) : TestTwoArgs<BaseTestPos, pow, am_pow_ps, am_pow_ss>("pow", fLimit, fLimit2)
	{ }

protected:
	virtual float RandVal() const
	{
		float t;
		do {
			t = BaseTestPos::RandVal(); // x can't be 0
		} while (t == 0.0f);
		return t;
	}

	virtual float OrdVal(int i) const
	{
		float t;
		do {
			t = BaseTestPos::OrdVal(i); // x can't be 0
		} while (t == 0.0f);
		return t;
	}

	virtual float RandVal2(DWORD hint) const
	{
		return BaseTest::RandVal2(hint); // y can be negative or positive
	}

	virtual float OrdVal2(int i, DWORD hint) const
	{
		return BaseTest::OrdVal2(i, hint); // y can be negative or positive
	}
};

class TestPowE : public TestTwoArgs<BaseTestPos, pow, am_pow_eps, am_pow_ess>
{
public:
	TestPowE(float fLimit, float fLimit2) : TestTwoArgs<BaseTestPos, pow, am_pow_eps, am_pow_ess>("pow-E", fLimit, fLimit2)
	{ }

protected:
	virtual float RandVal() const
	{
		float t;
		do {
			t = BaseTestPos::RandVal(); // x can't be 0
		} while (t == 0.0f);
		return t;
	}

	virtual float OrdVal(int i) const
	{
		float t;
		do {
			t = BaseTestPos::OrdVal(i); // x can't be 0
		} while (t == 0.0f);
		return t;
	}

	virtual float RandVal2(DWORD hint) const
	{
		return BaseTest::RandVal2(hint); // y can be negative or positive
	}

	virtual float OrdVal2(int i, DWORD hint) const
	{
		return BaseTest::OrdVal2(i, hint); // y can be negative or positive
	}
};

TestPow test_pow(26.0f, 26.0f);  // pow(26, 26) = 6.15611958020716e36
TestPowE test_powe(26.0f, 26.0f);  // pow(26, 26) = 6.15611958020716e36

/////////////////////////////////////////////////////////////////////////////
// main

int main()
{
	enable_FTZ();

	init_tmath();

	printf("Test\tParam\tx87 func\tAM_ps\t\tAM_ss\t\tTable\n");

	test_sin.DoTest();
	test_sine.DoTest();
	test_cos.DoTest();
	test_cose.DoTest();
	test_sincos.DoTest();
	test_sincose.DoTest();
	test_tan.DoTest();
	test_tane.DoTest();

	printf("Press ENTER to continue");
	getchar();

	test_atan.DoTest();
	test_atane.DoTest();
	test_atan2.DoTest();
	test_atan2e.DoTest();
	test_asin.DoTest();
	test_acos.DoTest();

	printf("Press ENTER to continue");
	getchar();

	test_exp.DoTest();
	test_expe.DoTest();
	test_exp2.DoTest();
	test_exp2e.DoTest();
	test_log.DoTest();
	test_loge.DoTest();
	test_log2.DoTest();
	test_log2e.DoTest();

	printf("Press ENTER to continue");
	getchar();

	test_pow.DoTest();
	test_powe.DoTest();

	printf("Press ENTER to exit");
	getchar();

	uninit_tmath();
}
