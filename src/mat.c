/*
 * main.c
 *
 *  Created on: Dec 21, 2011
 *      Author: shlomi vaknin
 *
 *  maman 22
 *
 *  This file implements the matrix manipulation code. its very straight forward.
 */
#include <stdio.h>
#include "mat.h"

/* initializes a matrix mat to the values contained in a11..a44 */
void initMat(Mat* mat,
		float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41, float a42, float a43, float a44) {
	mat->m[0][0] = a11;
	mat->m[0][1] = a12;
	mat->m[0][2] = a13;
	mat->m[0][3] = a14;

	mat->m[1][0] = a21;
	mat->m[1][1] = a22;
	mat->m[1][2] = a23;
	mat->m[1][3] = a24;

	mat->m[2][0] = a31;
	mat->m[2][1] = a32;
	mat->m[2][2] = a33;
	mat->m[2][3] = a34;

	mat->m[3][0] = a41;
	mat->m[3][1] = a42;
	mat->m[3][2] = a43;
	mat->m[3][3] = a44;

}

/* adds matA and matB and store the result in matC */
void add(Mat matA, Mat matB, Mat* matC) {
	int i,j;

	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			matC->m[i][j] = matA.m[i][j] + matB.m[i][j];
		}
	}
}

/* Subtract matA and matB and store the result in matC */
void sub(Mat matA, Mat matB, Mat* matC) {
	int i,j;

	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			matC->m[i][j] = matA.m[i][j] - matB.m[i][j];
		}
	}
}

/* calculate dot product between the row of matA and the col of matB */
float dot(Mat matA, int row, Mat matB, int col) {
	int i;
	float res = 0;

	for (i=0;i<ARITY;i++) {
		res += matA.m[row][i]*matB.m[i][col];
	}

	return res;
}

/* multiply matA and matB and store the result in matC */
void mul(Mat matA, Mat matB, Mat* matC) {
	int i,j;

	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			matC->m[i][j] = dot(matA, i, matB, j);
		}
	}
}

/* print the given matrix to the screen */
void printMat (Mat m) {
	int i,j;
	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			printf("%.2f ",m.m[i][j]);
		}
		printf("\n");
	}
}

/* transposes matrix m, stores the results in matrix o */
void transMat(Mat m, Mat* o) {
	int i,j;
	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			o->m[i][j] = m.m[j][i];
		}
	}
}

/* multiply matrix m by scalar, store the result in matrix o */
void mulScalar(Mat m, float scalar, Mat* o) {
	int i,j;
	for (i=0;i<ARITY;i++) {
		for (j=0;j<ARITY;j++) {
			o->m[i][j]=m.m[i][j]*scalar;
		}
	}
}
