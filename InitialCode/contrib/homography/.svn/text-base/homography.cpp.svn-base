/*
 * homography.cpp
 *
 *  Created on: Nov 7, 2009
 *      Author: mattia
 */

#include "homography.h"



void normalize(int n, double x[][2], double T[3][3]){

    //Calc centroid of points

	double mean[2]= { 0, 0};
	for(int count=0; count<n; count++){
                mean[0] = mean[0]+ x[count][0];
		mean[1] = mean[1]+ x[count][1];

	}


        mean[0]=mean[0]/n;  //Mean x values
        mean[1]=mean[1]/n;  //Mean y values


	//Shift origin to centroid
        double (*newpoint)[2] = new double[n][2];

	for(int count=0; count<n; count++){
		newpoint[count][0] = x[count][0]-mean[0];
		newpoint[count][1] = x[count][1]-mean[1];
	}


        double *dist = new double[n];
	double meandist=0;

	for(int count =0; count<n; count++){
		dist[count]= sqrt(pow(newpoint[count][0],2)+pow(newpoint[count][1],2));
		meandist= meandist+ dist[count];
	}
	meandist=meandist/n;
	double scale = sqrt(2.0)/meandist;

	T[0][0]= scale;
	T[0][1]= 0.0;
	T[0][2]= -scale*mean[0];
	T[1][0]= 0.0;
	T[1][1]= scale;
	T[1][2]= -scale*mean[1];
	T[2][0]= 0.0;
	T[2][1]= 0.0;
	T[2][2]= 1.0;


	for(int count=0; count<n; count++){
		newpoint[count][0]=(T[0][0]*x[count][0])+(T[0][1]*x[count][1])+T[0][2];
		newpoint[count][1]=(T[1][0]*x[count][0])+(T[1][1]*x[count][1])+T[1][2];
		x[count][0] = newpoint[count][0];
		x[count][1] = newpoint[count][1];
	}

};

//Mul two 3x3 matrix T1, T2. Result in T3

void mul3x3(double T1[3][3], double T2[3][3], double T3[3][3]){

	T3[0][0]= T1[0][0]*T2[0][0] + T1[0][1]*T2[1][0] + T1[0][2]*T2[2][0];
	T3[0][1]= T1[0][0]*T2[0][1] + T1[0][1]*T2[1][1] + T1[0][2]*T2[2][1];
	T3[0][2]= T1[0][0]*T2[0][2] + T1[0][1]*T2[1][2] + T1[0][2]*T2[2][2];

	T3[1][0]= T1[1][0]*T2[0][0] + T1[1][1]*T2[1][0] + T1[1][2]*T2[2][0];
	T3[1][1]= T1[1][0]*T2[0][1] + T1[1][1]*T2[1][1] + T1[1][2]*T2[2][1];
	T3[1][2]= T1[1][0]*T2[0][2] + T1[1][1]*T2[1][2] + T1[1][2]*T2[2][2];

	T3[2][0]= T1[2][0]*T2[0][0] + T1[2][1]*T2[1][0] + T1[2][2]*T2[2][0];
	T3[2][1]= T1[2][0]*T2[0][1] + T1[2][1]*T2[1][1] + T1[2][2]*T2[2][1];
	T3[2][2]= T1[2][0]*T2[0][2] + T1[2][1]*T2[1][2] + T1[2][2]*T2[2][2];

};


//Inver of a 3x3 Matrix

void inv3x3(double A[3][3], double invA[3][3]){

	double det = (A[0][0]*A[1][1]*A[2][2]) +
				 (A[0][1]*A[1][2]*A[2][0]) +
				 (A[0][2]*A[1][0]*A[2][1]) -
				 (
				 (A[0][2]*A[1][1]*A[2][0]) +
				 (A[0][0]*A[1][2]*A[2][1]) +
				 (A[0][1]*A[1][0]*A[2][2])
				 );


	invA[0][0]=  ((A[1][1]*A[2][2])-(A[2][1]*A[1][2]))/det;
	invA[0][1]= -((A[0][1]*A[2][2])-(A[2][1]*A[0][2]))/det;
	invA[0][2]=  ((A[0][1]*A[1][2])-(A[1][1]*A[0][2]))/det;

	invA[1][0]= -((A[1][0]*A[2][2])-(A[2][0]*A[1][2]))/det;
	invA[1][1]=  ((A[0][0]*A[2][2])-(A[2][0]*A[0][2]))/det;
	invA[1][2]= -((A[0][0]*A[1][2])-(A[1][0]*A[0][2]))/det;

	invA[2][0]=  ((A[1][0]*A[2][1])-(A[2][0]*A[1][1]))/det;
	invA[2][1]= -((A[0][0]*A[2][1])-(A[2][0]*A[0][1]))/det;
	invA[2][2]=  ((A[0][0]*A[1][1])-(A[1][0]*A[0][1]))/det;


};



//Function for denormalise H matrix

void denormalise(double H[3][3], double T1[3][3], double T2[3][3] ){

	double Htemp[3][3];
	double Ttemp[3][3];
	inv3x3(T2,Ttemp);
	mul3x3(Ttemp,H,Htemp);
	mul3x3(Htemp,T1,H);

};


// Main Funtion

void homography(int n, double x1[][2], double x2[][2], double H[3][3]){



	double T1[3][3], T2[3][3];

	normalize(n,x1,T1);
	normalize(n,x2,T2);
	int row = 2*n;
	int col= 9;

	ap::real_2d_array A;
	A.setbounds(0,row-1,0,col-1);

    int uneeded=0;
    int vtneeded=2;
    int additionalmemory=2;

    ap::real_1d_array W;
    ap::real_2d_array U;
    ap::real_2d_array Vt;
        //prepare the system A to solve with SVD


	for(int count=0, count2=0; count<n*2; count++, count2++ ){

		A(count,0)=x1[count2][0];
		A(count,1)=x1[count2][1];
		A(count,2)=1.0;
		A(count,3)=0.0;
		A(count,4)=0.0;
		A(count,5)=0.0;
		A(count,6)=-x1[count2][0]*x2[count2][0];
		A(count,7)=-x1[count2][1]*x2[count2][0];
		A(count,8)=-x2[count2][0];

		A(count+1,0)=0.0;
		A(count+1,1)=0.0;
		A(count+1,2)=0.0;
		A(count+1,3)=x1[count2][0];
		A(count+1,4)=x1[count2][1];
		A(count+1,5)=1.0;
		A(count+1,6)=-x1[count2][0]*x2[count2][1];
		A(count+1,7)=-x1[count2][1]*x2[count2][1];
		A(count+1,8)=-x2[count2][1];
		count++;

	}

//Solve system with SVD
	rmatrixsvd( A,row,col,uneeded,vtneeded,additionalmemory,W,U,Vt);

	H[0][0]=Vt(8,0);
	H[0][1]=Vt(8,1);
	H[0][2]=Vt(8,2);
	H[1][0]=Vt(8,3);
	H[1][1]=Vt(8,4);
	H[1][2]=Vt(8,5);
	H[2][0]=Vt(8,6);
	H[2][1]=Vt(8,7);
	H[2][2]=Vt(8,8);

//Denormalise

	denormalise(H,  T1,  T2 );


};
