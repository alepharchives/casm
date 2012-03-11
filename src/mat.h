/*
 * mat.h
 *
 *  Created on: Nov 21, 2011
 *      Author: shlomi vaknin
 */

#ifndef MAT_H_
#define MAT_H_

/* the arity of our matrix - 4*4 */
#define ARITY 4

/* define our matrix structure */
typedef struct mat {
	float m[ARITY][ARITY];
} Mat;

/* define all our function prototypes */

/* initializes a matrix mat to the values contained in a11..a44 */
void initMat(Mat* mat,
		float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41, float a42, float a43, float a44);

/* adds matA and matB and store the result in matC */
void add(Mat matA, Mat matB, Mat* matC);

/* Subtract matA and matB and store the result in matC */
void sub(Mat matA, Mat matB, Mat* matC);

/* Multiply matA and matB and store the result in matC */
void mul(Mat matA, Mat matB, Mat* matC);

/* multiply matrix m by scalar, store the result in matrix o */
void mulScalar(Mat m, float scalar, Mat* o);

/* transposes matrix m, stores the results in matrix o */
void transMat(Mat m, Mat* o);

/* print the given matrix to the screen */
void printMat (Mat m);

/* calculate dot product between the row of matA and the col of matB */
float dot(Mat matA, int row, Mat matB, int col);

#endif /* MAT_H_ */
