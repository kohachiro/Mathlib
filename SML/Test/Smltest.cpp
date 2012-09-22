// smltest.cpp
// 
// Demonstrate usage of SML classes
// and perform some measurements.

#include <windows.h> 

#include <stdio.h> 
#include <stdarg.h>
#include <time.h>

#include <smlxmatrix.h>

#define	SAMPLES	40

void report(char* format, ...)
{
	// Use vsprintf to print information to Output window.
    va_list marker;
    char buf[500];
    vsprintf(buf, format, va_start(marker, format));
	// Or printf()
    OutputDebugString(buf);
    OutputDebugString("\n");
}


long GetTick(void)
{
	// Count processor clocks
	long t;
	__asm {
		pushad
		cpuid                           ; serial command
		_emit	0x0F                    ; it is...
		_emit	0x31                    ; ... rdtsc
		lea		ecx, t
		mov		dword ptr [ecx], eax    ; # of processor clocks
		; mov	dword ptr [ecx+4], edx  ; if __int64 is used
		popad
	}
	return t;
}


long Duration(long* ticks)
{
	// Find minimum value in an array ticks
	long nclocks = 0;
	for(int i = 0; i < SAMPLES; i++) {
		if (!nclocks || ticks[i] < nclocks) 
			nclocks = ticks[i];
		// if (i > 1 && nclocks != ticks[i]) report("alarm%i: %i != %i", i, ticks[i], nclocks);
	}
	return nclocks;
}


// Repeat measuremnts SAMPLES times.
#define START_MEASUREMENTS \
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL); \
	for(i = 0; i < SAMPLES; i++) { \
		tick = -GetTick();
#define END_MEASUREMENTS \
		ticks[i] = tick + GetTick() - base; \
	} \
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL); \
	report("Duration for %s:\t%i", testname, Duration(ticks));


int main(int argc, char* argv[])
{
	// Various examples.
	int i;
	long tick, base = 0;
	long ticks[SAMPLES];
	char* testname;

	// We are looking for the best value among SAMPLES to
	// eliminate cache delays and effects of cpuid variable timing.

	testname = "base";
	START_MEASUREMENTS;
	END_MEASUREMENTS;
	base = Duration(ticks);     // time required to count processor clocks

	// ================
	// SMLXMatrix Tests
	// ================

	SMLXMatrix m1(3, 3);        // declare initial size
	SMLXMatrix m2(3, 3);
	SMLXMatrix m3;              // size will be set by '='
	SMLXSpatialVector v1(11, 22, 33);
	SMLXSpatialVector v2;

	testname = "3x3 * 3x1";
	m1.Set(2.0);
	m2.Set(3.0);
	m1[1][2] = m1[2][1] = 3;
	
	START_MEASUREMENTS;
	v2 = m1 * v1;
	END_MEASUREMENTS;
	// m1.Output("m1");
	// v1.Output("v1");
	// v2.Output("m1 * v1");

	testname = "3x3 * 3x3";
	START_MEASUREMENTS;
	m3 = m1 * m2;
	END_MEASUREMENTS;
	// m1.Output("m1");
	// m2.Output("m2");
	// m3.Output("m1 * m2");

	testname = "6x6 * 6x6";
	m1.Resize(6, 6);
	m2.Resize(6, 6);
	m1.Set(1);
	m2.Set(2);
	m1[0][5] = 10;

	START_MEASUREMENTS;
	m3 = m1 * m2;
	END_MEASUREMENTS;
	// m1.Output("m1");
	// m2.Output("m2");
	// m3.Output("m1 * m2");

	testname = "6x6 * Transpose(6x6)";
	START_MEASUREMENTS;
	m3 = m1 * Transpose(m2);
	END_MEASUREMENTS;

	testname = "6x6 + 6x6";
	START_MEASUREMENTS;
	m3 = m1 + m3;
	END_MEASUREMENTS;

	testname = "6x6 * 6x6 - Transpose(6x6) * 6x6 - 6x6";
	START_MEASUREMENTS;
	m3 = m1 * m2 - Transpose(m3) * m1 - m2;
	END_MEASUREMENTS;

	// Assuming non-zero diagonal...
	testname = "Invert Without Pivoting(6x6)";
	m1.Identity();
	m1[0][0] = 10;
	m1[3][4] = 2;
	m3 = m1;

	START_MEASUREMENTS;
	m1.Invert();
	END_MEASUREMENTS;
	// m3 = m3 - m1; // discard even number of inversions
	// m3.Output("m3");

	// General case.
	testname = "Invert With Pivoting(6x6)";
	START_MEASUREMENTS;
	m1.GenericInvert();
	END_MEASUREMENTS;

	// =================
	// SMLMatrix3f tests
	// =================

	// TransformPoint and Multiply are inlined for SMLMatrix3f,
	// so timing is not entirely correct 
	// (some subexpressions are optimized out of loop)...

	testname = "TransformPoint 3x3";
	m1.Resize(3, 3);
	SMLMatrix3f m33_1 = (const SMLMatrix3f&) m1;
	SMLMatrix3f m33_2(m33_1);
	SMLMatrix3f m33_3;
	SMLVec3f v3_1(11, 22, 33);
	SMLVec3f v3_2;
	m33_2.Set(1, 2, 3.0);

	START_MEASUREMENTS;
	m33_1.TransformPoint(v3_1, v3_2);
	END_MEASUREMENTS;
	// m33_1.Output("m1");
	// report("m1 * v1 = {%f, %f, %f}", v3_2.x, v3_2.y, v3_2.z);

	testname = "Multiply 3x3";
	START_MEASUREMENTS;
	m33_3.Multiply(m33_1, m33_2);
	END_MEASUREMENTS;

	// =================
	// SMLMatrix4f tests
	// =================

	testname = "Transform 4x4";
	m1.Resize(4, 4);
	SMLMatrix4f m44_1 = (const SMLMatrix4f&) m1;
	SMLMatrix4f m44_2(m44_1);
	SMLMatrix4f m44_3;
	SMLVec4f v4_1(11, 22, 33, 44);
	SMLVec4f v4_2;
	m44_2.Set(1, 2, 3.0);

	START_MEASUREMENTS;
	m44_1.Transform(v4_1, v4_2);
	END_MEASUREMENTS;
	// m44_1.Output("m1");
	// report("m1 * v1 = {%f, %f, %f, %f}", v4_2.x, v4_2.y, v4_2.z, v4_2.w);

	testname = "TransformPoint 4x4";
	START_MEASUREMENTS;
	m44_1.TransformPoint(v3_1, v3_2);
	END_MEASUREMENTS;

	testname = "TransformVector 4x4";
	START_MEASUREMENTS;
	m44_1.TransformPoint(v3_1, v3_2);
	END_MEASUREMENTS;

	testname = "Multiply 4x4";
	START_MEASUREMENTS;
	m44_3.Multiply(m44_1, m44_2);
	END_MEASUREMENTS;

	return 0;
}
