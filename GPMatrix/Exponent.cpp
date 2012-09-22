//////////////////////////////////////////////////////////////////////////////
//
// EXPONENT.CPP
//
//  Sample of exponent calculation of a matrix in three different
//  implementations.
//  Remark: Add the TSC.CPP file to the project.
//
//  Written by Zvi Devir, Intel MSL
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "utils.h"
#include <d3dxmath.h>

#ifdef NDEBUG
	#define ITER 10000
	#define REPS 1000
#else
	#define ITER 10
	#define REPS 1
#endif


__int64 Clock();		// Read TSC clock (see TSC.CPP).
#define StartTime(i)	clock1 = Clock();
#define EndTime(i)		clock2 = Clock(); total##i += clock2-clock1;


void PrintMatrix(GPMatrix &Mat, char *format="2.2") {
	int i,j;
	char fmt[20] = "%";

	for (i=0; format[i]; i++) fmt[i+1] = format[i];
	fmt[++i] = 'f';		fmt[++i] = '\t';

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) 
			printf(fmt,Mat(i,j));
		printf("\n");
	}
	printf("\n");
}



void main() {

	GPMatrix power(	0.0f, 0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f	);
	D3DXMATRIX power1(*(D3DXMATRIX *)&power);
	
	int i,j;
	__int64 clock1, clock2, total1=0, total2=0, total3=0;


	//////////////////////////////////////////////
	// Using Microsoft D3DXMATRIX (scalar code).
	//////////////////////////////////////////////
	for (j=0; j<REPS; j++)
	{
		D3DXMATRIX product, sum;
		float n(0.0f);

		D3DXMatrixIdentity(&sum);
		D3DXMatrixIdentity(&product);

		StartTime(3);
		for (i=1; i<ITER; i++) {
			product *= power1;
			n += 1.0f;
			product /= n;
			sum += product;
		}
		EndTime(3);

		if (!j) PrintMatrix(*(GPMatrix *)&sum, "1.5");
	}

	//////////////////////////////////////////////
	// Using the GPMatrix class.
	//////////////////////////////////////////////
	for (j=0; j<REPS; j++)
	{
		GPMatrix product, sum;
		float n(0.0f);

		sum.IdentityMatrix();
		product.IdentityMatrix();

		StartTime(1);
		for (i=1; i<ITER; i++) {
			product *= power;
			n += 1.0f;
			product *= (1.0f/n);
			sum += product;
		}
		EndTime(1);

		if (!j) PrintMatrix(sum, "1.5");
	}


	//////////////////////////////////////////////
	// Using inlined assembly.
	//////////////////////////////////////////////
	for (j=0; j<REPS; j++)
	{
		float rcp,n;
		GPMatrix product, sum;

		sum.IdentityMatrix();
		product.IdentityMatrix();

		StartTime(2);
		__asm fldz;		// n = 0.0f
		for (i=1; i<ITER; i++) __asm {
			lea		eax, sum
			lea		ecx, product
			lea		edx, power

		// n += 1.0
		// rcp = 1.0/n

			fld1
			faddp
			fld1
			fdiv	st(0), st(1)
			fstp	rcp


		// product *= power

			movss	xmm0, dword ptr [ecx]
			movaps	xmm1, xmmword ptr [edx]
			shufps	xmm0, xmm0, 0
			movss	xmm2, dword ptr [ecx+4]
			mulps	xmm0, xmm1
			shufps	xmm2, xmm2, 0
			movaps	xmm3, xmmword ptr [edx+10h]
			movss	xmm7, dword ptr [ecx+8]
			mulps	xmm2, xmm3
			shufps	xmm7, xmm7, 0
			addps	xmm0, xmm2
			movaps	xmm4, xmmword ptr [edx+20h]
			movss	xmm2, dword ptr [ecx+0Ch]
			mulps	xmm7, xmm4
			shufps	xmm2, xmm2, 0
			addps	xmm0, xmm7
			movaps	xmm5, xmmword ptr [edx+30h]
			movss	xmm6, dword ptr [ecx+10h]
			mulps	xmm2, xmm5
			movss	xmm7, dword ptr [ecx+14h]
			shufps	xmm6, xmm6, 0
			addps	xmm0, xmm2
			shufps	xmm7, xmm7, 0
			movlps	qword ptr [ecx], xmm0
			movhps	qword ptr [ecx+8], xmm0
			mulps	xmm7, xmm3
			movss	xmm0, dword ptr [ecx+18h]
			mulps	xmm6, xmm1
			shufps	xmm0, xmm0, 0
			addps	xmm6, xmm7
			mulps	xmm0, xmm4
			movss	xmm2, dword ptr [ecx+24h]
			addps	xmm6, xmm0
			movss	xmm0, dword ptr [ecx+1Ch]
			movss	xmm7, dword ptr [ecx+20h]
			shufps	xmm0, xmm0, 0
			shufps	xmm7, xmm7, 0
			mulps	xmm0, xmm5
			mulps	xmm7, xmm1
			addps	xmm6, xmm0
			shufps	xmm2, xmm2, 0
			movlps	qword ptr [ecx+10h], xmm6
			movhps	qword ptr [ecx+18h], xmm6
			mulps	xmm2, xmm3
			movss	xmm6, dword ptr [ecx+28h]
			addps	xmm7, xmm2
			shufps	xmm6, xmm6, 0
			movss	xmm2, dword ptr [ecx+2Ch]
			mulps	xmm6, xmm4
			shufps	xmm2, xmm2, 0
			addps	xmm7, xmm6
			mulps	xmm2, xmm5
			movss	xmm0, dword ptr [ecx+34h]
			addps	xmm7, xmm2
			shufps	xmm0, xmm0, 0
			movlps	qword ptr [ecx+20h], xmm7
			movss	xmm2, dword ptr [ecx+30h]
			movhps	qword ptr [ecx+28h], xmm7
			mulps	xmm0, xmm3
			shufps	xmm2, xmm2, 0
			movss	xmm6, dword ptr [ecx+38h]
			mulps	xmm2, xmm1
			shufps	xmm6, xmm6, 0
			addps	xmm2, xmm0
			mulps	xmm6, xmm4
			movss	xmm7, dword ptr [ecx+3Ch]
			shufps	xmm7, xmm7, 0
			addps	xmm2, xmm6
			mulps	xmm7, xmm5
			addps	xmm2, xmm7
			movaps	xmmword ptr [ecx+30h], xmm2


		// product *= rcp;

			movss	xmm0, rcp
			movaps	xmm1, xmmword ptr [ecx]
			shufps	xmm0, xmm0, 0
			movaps	xmm2, xmmword ptr [ecx+10h]
			mulps	xmm1, xmm0
			movaps	xmm3, xmmword ptr [ecx+20h]
			mulps	xmm2, xmm0
			movaps	xmmword ptr [ecx], xmm1
			mulps	xmm3, xmm0
			movaps	xmmword ptr [ecx+10h], xmm2
			mulps	xmm0, xmmword ptr [ecx+30h]
			movaps	xmmword ptr [ecx+20h], xmm3
			movaps	xmmword ptr [ecx+30h], xmm0


		// sum += product;

			movaps	xmm0, xmmword ptr [eax]
			addps	xmm0, xmmword ptr [ecx]
			movaps	xmmword ptr [eax], xmm0
			movaps	xmm1, xmmword ptr [eax+10h]
			addps	xmm1, xmmword ptr [ecx+10h]
			movaps	xmmword ptr [eax+10h], xmm1
			movaps	xmm2, xmmword ptr [eax+20h]
			addps	xmm2, xmmword ptr [ecx+20h]
			movaps	xmmword ptr [eax+20h], xmm2
			movaps	xmm3, xmmword ptr [eax+30h]
			addps	xmm3, xmmword ptr [ecx+30h]
			movaps	xmmword ptr [eax+30h], xmm3
		}
		__asm fstp rcp;	// Clean up ST
		EndTime(2);

		if (!j) PrintMatrix(sum, "1.5");
	}



	printf("Avarage time using the GPMatrix class:    %4.2lf\n", double(total1)/double(REPS*ITER));
	printf("Avarage time using the inlined assembly:  %4.2lf\n", double(total2)/double(REPS*ITER));
	printf("Avarage time using the D3DXMATRIX class:  %4.2lf\n", double(total3)/double(REPS*ITER));

}

