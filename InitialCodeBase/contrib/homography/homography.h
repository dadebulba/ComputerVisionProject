/*
 * homography.h
 *
 *  Created on: Nov 7, 2009
 *      Author: mattia


 Compute 2D Homography with "The direct Linear Transformation (DLT) algorithm"

Argument:
n        = number of points
x1[n][2] = nx2 set of points
x2[n][2] = nx2 set of points such as x1<-->x2
H[3][3]  = 3x3 homography Matrix
 */


#ifndef HOMOGRAPHY_H_
#define HOMOGRAPHY_H_

#include <math.h>
#include "SVD/svd.h"
using namespace std;


void inv3x3(double A[3][3], double invA[3][3]);
void mul3x3(double T1[3][3], double T2[3][3], double T3[3][3]);
void normalize(int n, double x[][2], double T[3][3]);
void denormalise(double H[3][3], double T1[3][3], double T2[3][3] );

//Main function
void homography(int n, double x1[][2], double x2[][2], double H[3][3]);


#endif /* HOMOGRAPHY_H_ */
