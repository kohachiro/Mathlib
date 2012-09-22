//////////////////////////////////////////////////////////////////////////////
//
// EXPONENT.CPP
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
    #define ITER 2000
    #define REPS 500
#else
    #define ITER 10
    #define REPS 1
#endif


void PrintMatrix(SPMatrix &Mat) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) 
            printf("%1.5f\t",Mat(i,j));
        printf("\n");
    }
    printf("\n");
}
#ifdef USE_P4
void PrintMatrix(DPMatrix &Mat) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) 
            printf("%1.5f\t",Mat(i,j));
        printf("\n");
    }
    printf("\n");
}
#endif


void main() {

    D3DXMATRIX power1 ( 0.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f, 0.0f  );
    SPMatrix power2 (   0.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f, 0.0f  );

#ifdef USE_P4
    ScalarDP power3 (   0.0, 0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0, 0.0  );
    DPMatrix power4 (   0.0, 0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0, 0.0  );
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
        D3DXMATRIX product, sum;
        float n(0.0f);

        D3DXMatrixIdentity(&sum);
        D3DXMatrixIdentity(&product);

        StartTime(1);
        for (i=1; i<ITER; i++) {
            product *= power1;
            n += 1.0f;
            product /= n;
            sum += product;
        }
        EndTime(1);

        if (!j) PrintMatrix(*(SPMatrix *)&sum);
    }

    //////////////////////////////////////////////
    // Using the SPMatrix class.
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        SPMatrix product, sum;
        float n(0.0f);

        sum.IdentityMatrix();
        product.IdentityMatrix();

        StartTime(2);
        for (i=1; i<ITER; i++) {
            product *= power2;
            n += 1.0f;
            product *= (1.0f/n);
            sum += product;
        }
        EndTime(2);

        if (!j) PrintMatrix(sum);
    }

#ifdef USE_P4
    //////////////////////////////////////////////
    // Using the ScalarDP class (double-precision scalar code).
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        ScalarDP product, sum;
        double n(0.0);

        sum.IdentityMatrix();
        product.IdentityMatrix();

        StartTime(3);
        for (i=1; i<ITER; i++) {
            product *= power3;
            n += 1.0;
            product /= n;
            sum += product;
        }
        EndTime(3);

        if (!j) PrintMatrix(*(DPMatrix *)&sum);
    }

    //////////////////////////////////////////////
    // Using the DPMatrix class.
    //////////////////////////////////////////////
    for (j=0; j<REPS; j++)
    {
        DPMatrix product, sum;
        double n(0.0f);

        sum.IdentityMatrix();
        product.IdentityMatrix();

        StartTime(4);
        for (i=1; i<ITER; i++) {
            product *= power4;
            n += 1.0;
            product *= (1.0/n);
            sum += product;
        }
        EndTime(4);

        if (!j) PrintMatrix(sum);
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
