//////////////////////////////////////////////////////////////////////////////
//
// SQRT.CPP
//
//  Sample of exponent calculation of a matrix in four different
//  implementations.
//  Remark: Add the TSC.CPP file to the project.
//
//  Written by Zvi Devir, Intel MSL
//
//////////////////////////////////////////////////////////////////////////////

#define USE_P4


#include <stdio.h>
#ifdef USE_P4
    #include "..\Matrices.h"
    #include "ScalarDP.h"
#else
    #include "..\SPMatrix.h"
#endif
#include <d3dxmath.h>

#include "TSC.h"


#ifndef _DEBUG
    #define ITER 50
    #define REPS 2000
#else
    #define ITER 50
    #define REPS 1
#endif


void PrintMatrix(SPMatrix &Mat) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) 
            printf("%1.4f\t",Mat(i,j));
        printf("\n");
    }
    printf("\n");
}
#ifdef USE_P4
void PrintMatrix(DPMatrix &Mat) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) 
            printf("%1.4f\t",Mat(i,j));
        printf("\n");
    }
    printf("\n");
}
#endif


void main() {

    SPMatrix source (   2.0f,-1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, 3.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 1.0f,
                        0.0f,-3.0f, 0.0f, 2.0f  );

    D3DXMATRIX power1; 
    SPMatrix power2; 
#ifdef USE_P4
    ScalarDP power3;
    DPMatrix power4;
#endif

    power2 = source*source;
    power1 = *(D3DXMATRIX*)&power2;
#ifdef USE_P4
    power4 = Expand(power2);
    power3 = *(ScalarDP*)&power4;
#endif


    int i,j;
    __int64 total1=0, total2=0, total3=0, total4=0;

    #ifndef _DEBUG
        SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
    #endif

    //////////////////////////////////////////////
    // Using Microsoft D3DXMATRIX (scalar code).
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        D3DXMATRIX sqrt, rsqrt;
        D3DXMatrixIdentity(&sqrt);

        StartTime(1);
        for (i=1; i<ITER; i++) {
            D3DXMatrixInverse(&rsqrt, NULL, &sqrt);
            sqrt += power1*rsqrt;
            sqrt *= 0.5f;
        }
        EndTime(1);

        if (!j) PrintMatrix(*(SPMatrix *)&sqrt);
        if (!j) PrintMatrix(*(SPMatrix *)&(sqrt*sqrt));
    }

    //////////////////////////////////////////////
    // Using the SPMatrix class.
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        SPMatrix sqrt, rsqrt;
        sqrt.IdentityMatrix();

        StartTime(2);
        for (i=1; i<ITER; i++) {
            rsqrt = sqrt;
            rsqrt.Inverse();
            sqrt += power2*rsqrt;
            sqrt *= 0.5f;
        }
        EndTime(2);

        if (!j) PrintMatrix(sqrt);
        if (!j) PrintMatrix(sqrt*sqrt);
    }

#ifdef USE_P4
    //////////////////////////////////////////////
    // Using the ScalarDP class (double-precision scalar code).
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        ScalarDP sqrt, rsqrt;
        sqrt.IdentityMatrix();

        StartTime(3);
        for (i=1; i<ITER; i++) {
            rsqrt = sqrt;
            rsqrt.Inverse();
            sqrt += power3*rsqrt;
            sqrt *= 0.5;
        }
        EndTime(3);

        if (!j) PrintMatrix(*(DPMatrix *)&sqrt);
        if (!j) PrintMatrix(*(DPMatrix *)&(sqrt*sqrt));
    }

    //////////////////////////////////////////////
    // Using the DPMatrix class.
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        DPMatrix sqrt, rsqrt;
        sqrt.IdentityMatrix();

        StartTime(4);
        for (i=1; i<ITER; i++) {
            rsqrt = sqrt;
            rsqrt.Inverse();
            sqrt += power4*rsqrt;
            sqrt *= 0.5;
        }
        EndTime(4);

        if (!j) PrintMatrix(sqrt);
        if (!j) PrintMatrix(sqrt*sqrt);
    }
#endif

    #ifndef _DEBUG
        SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
    #endif

    printf("Avarage time using the D3DXMATRIX class:  %4.2lf\n", double(total1)/double(REPS*ITER));
    printf("Avarage time using the SPMatrix class:    %4.2lf\n", double(total2)/double(REPS*ITER));
#ifdef USE_P4
    printf("Avarage time using the ScalarDP class:    %4.2lf\n", double(total3)/double(REPS*ITER));
    printf("Avarage time using the DPMatrix class:    %4.2lf\n", double(total4)/double(REPS*ITER));
#endif

    printf("\n");
    printf("Speedup of SPMatrix:  %1.3lf\n", double(total1)/double(total2));
#ifdef USE_P4
    printf("Speedup of DPMatrix:  %1.3lf\n", double(total3)/double(total4));
#endif
}

