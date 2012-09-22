Mathlib
=======

This is some math library for Intel/AMD CPU. It uses the special SIMD instruction and Newton iteration math library run faster.

Project

AMaths		A set of fast routines to calculate math functions using SSE&SSE2. The functions include sign abs sin cos tan sincos ect..
		and is hundreds times better than what is achievable with lookup tables.

DPMatrix	A matrix library using asm.The SPMatrix is Single-precision and DPMatrix is Double-precision.
		It run time is reduced by half than math.h

GPMatrix	A matrix library using D3Dmath.

SML		An advanced matrix library using SSE2.It support more functions than DPMatrix

D3DMath		An advanced library include AMaths and a matrix library using SSE2		

R3DMath		This is an advanced  library include  math library and matrix library.It using AMaths as base math library.It using  
		_mm_rsqrt_ss instruction and  newton iterationfor for sqrt functions.It also add AMD D3Dmath it faster than SSE2 in AMD CPU.

engine		An D3D + R3DMath demo.
