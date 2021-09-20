/*
 * mmDatabase.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: paolo
 */

#include "../mmLib.h"

namespace mmLib {
namespace mmModules{
mmDatabase::mmDatabase() {
idMax = 0;

}

mmDatabase::~mmDatabase() {

}

int mmDatabase::size(){
	return lastBlobList.size();
}

void mmDatabase::addId(mmBlob &blob){
	blob.blobID = idMax;
	lastBlobList.push_back(blob);
	idList.push_back(idMax);
	idMax++;
	//printf("Blob ID: %d\n", blob.blobID);
}



matchHelper mmDatabase::matchIt(vector<mmBlob> &blobList, int i){

	matchHelper mth;

	if(blobList.size() == 0)
	{
		printf("Errore imprevisto in mmDatabase::matchIt()\n");
		return mth;
	}

	for(int j=0; j< blobList.size(); j++)
	{
		mmBlob blobOld = lastBlobList[i];
		float* histBlueNew = new float[256];
		float* histRedNew = new float[256];
		float* histGreenNew = new float[256];
		float* histBlueOld = new float[256];
		float* histRedOld = new float[256];
		float* histGreenOld = new float[256];
		blobList[j].getBins(histBlueNew, histGreenNew, histRedNew);
		blobOld.getBins(histBlueOld, histGreenOld, histRedOld);

		float mseBlue = mmHistogramMatchingMSE(histBlueNew, histBlueOld);
		float mseGreen = mmHistogramMatchingMSE(histGreenNew, histGreenOld);
		float mseRed = mmHistogramMatchingMSE(histRedNew, histRedOld);
		float mse = mseBlue * 0.25 + mseRed * 0.25 + mseGreen * 0.5;

		if (j==0)
		{
			mth.mse = mse;
			mth.blobListID = j;
			mth.dbID = i;
		}

		if (mse < mth.mse)
		{
			mth.mse = mse;
			mth.blobListID = j;
			mth.dbID = i;
		}
	}
	return mth;
}

bool sortMatchHelper(const matchHelper& m1, const matchHelper& m2){
	return m1.mse < m2.mse;
}

void mmDatabase::checkItTwice(vector<mmBlob> &blobList, float threshold){
	//printf("Entra in checkItTwice - Threshold: %f - BloblistSize: %d\n", threshold, blobList.size());
	if (blobList.size() == 0)
		return;

	// inizializzo i blob ad un valore < 0
	for (int i=0; i<blobList.size(); i++)
	{
		blobList[i].blobID = -1;
	}

	// Se non esiste un db lo creo con il primo blob
	//printf("lastBlobSize: %d\n", lastBlobList.size());
	if (lastBlobList.size() == 0)
	{
		addId(blobList[0]);
	}
	//printf("lastBlobSize2: %d\n", lastBlobList.size());
	// Controlliamo se ci sono blob conosciuti
	vector<matchHelper> mhVector;
	for (int i=0; i<lastBlobList.size(); i++)
	{
		matchHelper mth = matchIt(blobList, i);
		//printf("MTH - mse = %f - blobListID = %d - dbID = %d\n",mth.mse, mth.blobListID, mth.dbID);
		mhVector.push_back(mth);
	}
	//printf("mhvector size: %d\n", mhVector.size());

	// Riordiniamo mhVector in mhVectorReordered
	std::sort(mhVector.begin(), mhVector.end(), sortMatchHelper);

	// assegno in ordine di mse gli Id dal minore al maggiore senza darlo doppio
	//printf("MhVector size: %d\n", mhVector.size());

	for (int i=0; i<mhVector.size(); i++)
	{
		if (mhVector[i].mse < threshold)
		{
			bool doublecheck = false;
			for (int j=0; j<i; j++)
			{
				if (mhVector[i].blobListID == mhVector[j].blobListID)
				{
					doublecheck = true;
					break;
				}
			}
			if (doublecheck == false)
			{

				//printf("blobList[%d].blobID = %d - lastBlobList[%d].blobID = %d\n", mhVector[i].blobListID, blobList[mhVector[i].blobListID].blobID, mhVector[i].dbID, lastBlobList[mhVector[i].dbID].blobID);
				blobList[mhVector[i].blobListID].blobID = lastBlobList[mhVector[i].dbID].blobID;
				blobList[mhVector[i].blobListID].assignedMSE = mhVector[i].mse;
				lastBlobList[mhVector[i].dbID] = blobList[mhVector[i].blobListID];
			}
		}
		else break;
	}

	for (int i=0; i<blobList.size(); i++)
	{
		if (blobList[i].blobID < 0)
		{
			addId(blobList[i]);
		}
	}
}


void mmDatabase::printBlobList(){

	int sizeList = lastBlobList.size();
	printf("Printing blob list of size: %d\n", sizeList);
	for (int i=0; i<sizeList; i++){
		mmBlob blob = lastBlobList[i];
		printf("Id: %d\n", idList[i]);
	}
}


mmKalmanDatabase::mmKalmanDatabase(){
	maxId = 0;
	//framecounter = 0;
}

int mmKalmanDatabase::findCloser (KalmanIstance ist, vector<mmBlob> blobList, double &distMin){
	int index;
	if (blobList.size() > 0){
		distMin = mmLib::mmModules::PointDistance(blobList[0].centroid(), ist.pt);
		index = 0;
	}
	else
		index = -1;

	for (int i=1; i<blobList.size(); i++){
		double dist = mmLib::mmModules::PointDistance(blobList[i].centroid(), ist.pt);
		if (dist < distMin){
			index = i;
			distMin = dist;
		}
	}
	return index;
}
}
}


