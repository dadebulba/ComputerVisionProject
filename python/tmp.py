//====================================================================================================
//The Free Edition of C++ to Python Converter limits conversion output to 100 lines per snippet.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-cplus-to-python.html
//====================================================================================================

#
# * mmBlobDetector.cpp
# *
# *  Created on: Jun 11, 2013
# *      Author: matteo
# 

#C++ TO PYTHON CONVERTER WARNING: The following #include directive was ignored:
##include "../mmLib.h"

from cv import *

import math

class mmLib(object): #this class replaces the original namespace 'mmLib'
    class mmAnomaly(object): #this class replaces the original namespace 'mmAnomaly'
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmGaussianModelSettings::mmGaussianModelSettings()
        def __init__(self):
            numGaussianParam = 3
            matchFactorParam = 2.5
            lrweightParam = 0.001
            lrvarianceParam = 0.001
            highVarianceParam = 150
            lowWeigthParam = 0.001
            tProvaParam = 0.5

            varianceParam = 1
            meanValueParam = 0
            weightParam = 0

            mWidthParam = 320
            mHeightParam = 240
            heightParam = 25
            widthParam = 35
            threshParam = 6
            numAnomalyParam = 3
            numNormalParam = 2

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmGaussianModel::mmGaussianModel()
        def __init__(self):
            numGaussian = 3
            matchFactor = 2.5
            lrweight = 0.001
            lrvariance = 0.001
            highVariance = 150
            lowWeigth = 0.001
            tProva = 0.5

            variance = 1
            meanValue = 0
            weight = 0

            mWidth = 320
            mHeight = 240
            height = 25
            width = 35
            thresh = 6
            numAnomaly = 3
            numNormal = 2

            isAnomaly = False
            firstFrame = True
            countFrame=0
            initializeModel()

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmGaussianModel::mmGaussianModel(mmGaussianModelSettings &gmSets)
        def __init__(self, gmSets):
            numGaussian = gmSets.numGaussianParam #number of gaussian in the model
            matchFactor = gmSets.matchFactorParam #variable for gaussian matching
            lrweight = gmSets.lrweightParam #gaussian weight
            lrvariance = gmSets.lrvarianceParam #gaussian variance
            highVariance = gmSets.highVarianceParam #gaussian variance for a new gaussian
            lowWeigth = gmSets.lowWeigthParam #gaussian weight for a new gaussian
            tProva = gmSets.tProvaParam #variable for a new gaussian

            variance = gmSets.varianceParam #initial gaussian variance
            meanValue = gmSets.meanValueParam #initial gaussian mean value
            weight = gmSets.weightParam #initial gaussian weight

            mWidth = gmSets.mWidthParam #frame width
            mHeight = gmSets.mHeightParam #frame height
            height = gmSets.heightParam #analysis window height
            width = gmSets.widthParam #analysis window width
            thresh = gmSets.threshParam #number of abnormal particle to consider the frame as abnormal
            numAnomaly = gmSets.numAnomalyParam #number of abnormal consecutive frames to raise the alarm
            numNormal = gmSets.numNormalParam #number of normal consecutive frames to raise down the alarm

            isAnomaly = False
            firstFrame = True
            countFrame=0
            initializeModel()

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmGaussianModel::~mmGaussianModel()
        def ~mmGaussianModel(self):
            particleVect.clear()
            if not firstFrame:
                i = 0
                while i<mWidth *mHeight:
                    gaussian[i].erase(gaussian[i].begin(),gaussian[i].end())
                    i += 1
                gaussian.clear()
                gaussian = None

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::initializeModel()
        def initializeModel(self):
            gPixel = gaussianPixel()
            gPixel.variance = variance
            gPixel.meanValue = meanValue
            gPixel.weight = weight

            gaussian = [list() for _ in range(mWidth *mHeight)]

            i = 0
            while i<mWidth:
                j = 0
                while j<mHeight:
                    k = 0
                    while k<numGaussian:
                        index = j *mWidth+i
                        gaussian[index].push_back(gPixel)
                        k += 1
                    j += 1
                i += 1
            swap = gPixel
            firstFrame = False

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: Mat mmGaussianModel::updateBackgroundModel(Mat& frame, Mat velocityImage, Mat background)
        def updateBackgroundModel(self, frame, velocityImage, background):
            temp = Mat(background.rows,background.cols,CV_8UC3,cv.Scalar(0))
            temp = background.clone()
            isBG = False
            i = 0
            while i<mHeight:
                j = 0
                while j<mWidth:
                    if int(velocityImage.at<uchar>(i,j)) != 0:
                        r = int(velocityImage.at<uchar>(i,j))

                        isBG = findMatchGaussian(Point(j,i),r)

                        if isBG:
                            temp.at<Vec3b>(i,j)[2] = 0
                            temp.at<Vec3b>(i,j)[1] = 0
                            temp.at<Vec3b>(i,j)[0] = 0
                        else:
                            temp.at<Vec3b>(i,j)[2] = 255
                            temp.at<Vec3b>(i,j)[1] = 255
                            temp.at<Vec3b>(i,j)[0] = 255

                            Singlept = Point2f()
                            Singlept.y = i
                            Singlept.x = j
                            particleVect.push_back(Singlept)
                            circle(frame, Singlept, 2, Scalar(0,0,255), -1, 8)
                    else:
                        temp.at<Vec3b>(i,j)[2] = 0
                        temp.at<Vec3b>(i,j)[1] = 0
                        temp.at<Vec3b>(i,j)[0] = 0
                    j += 1
                i += 1

            velocityImage.release()
            background.release()
            return Mat(temp)



#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: bool mmGaussianModel::findMatchGaussian(Point p, int r)
        def findMatchGaussian(self, p, r):
            index = p.y *mWidth + p.x
            sortGD(index)

            result = False          
            isMatch = False
            indexI = 0

            #iterate through all the gaussian distributions in the container and find the first match
            i = 0
            while i<numGaussian and not isMatch:
                dist = math.sqrt((r-gaussian[index][i].meanValue) ** 2)
                if dist <= matchFactor*(math.sqrt(gaussian[index][i].variance)):
                    isMatch = True
                    indexI = i
                i += 1
            matchedIndex = indexI

            if isMatch:
                result = checkBackground(index)
                updateGaussian(index,indexI,r,lrvariance)
            else:
                # get least probable distribution and replace that gaussian distribution such that mean = current pixel and highvariance
                indexGauss = getLPGD(index)
                replaceGD(index, indexGauss, r, highVariance, lowWeigth)
            # adjust weights
            i = 0
            while i<numGaussian:
                #for matched gaussian distribution
                if indexI == i and isMatch == True:
                    adjustWeight(index,i,lrweight,True)
                #for unmatched gaussian distribution
                else:
                    adjustWeight(index,i,lrweight,False)
                i += 1

            return result



        #sort according to weight/variance.
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::sortGD(int index)
        def sortGD(self, index):
            i = 0
            while i<numGaussian-1:
                j = 0
                while j<numGaussian-1-i:
                    if getWeightVarianceRatio(index,j+1) > getWeightVarianceRatio(index,j):
                        swap = gaussian[index][j]
                        gaussian[index][j] = gaussian[index][j+1]
                        gaussian[index][j+1] = swap
                    j += 1
                i += 1

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: double mmGaussianModel::getWeightVarianceRatio(int index, int indexGauss)
        def getWeightVarianceRatio(self, index, indexGauss):
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
            return (gaussian[index][indexGauss].weight/gaussian[index][indexGauss].variance)

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: bool mmGaussianModel::checkBackground(int index)
        def checkBackground(self, index):
            i = None
            sumWeights = 0
            backgroundIndex = 0

            i = 0
            while i<numGaussian:
                sumWeights += gaussian[index][i].weight

                if sumWeights > tProva:
                    backgroundIndex = i
                    break
                i += 1
            if matchedIndex <= backgroundIndex: 
                BG = True
            else:
                BG = False

            return BG

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::updateGaussian(int index, int indexGauss, int r, double learningRate)
        def updateGaussian(self, index, indexGauss, r, learningRate):
            gaussian[index][indexGauss].meanValue = ((1-learningRate)*gaussian[index][indexGauss].meanValue) + (learningRate *r)
            gaussian[index][indexGauss].variance = ((1-learningRate)*gaussian[index][indexGauss].variance)+(learningRate *(r-gaussian[index][indexGauss].meanValue) ** 2)

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: int mmGaussianModel::getLPGD(int index)
        def getLPGD(self, index):
            value = getWeightVarianceRatio(index,0)
            tempIndex = 0

            i = 1
            while i<numGaussian:
                temp = getWeightVarianceRatio(index,i)
                if temp < value:
                    value = temp
                    tempIndex = i
                i += 1
            return tempIndex

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::replaceGD(int index, int indexGauss, double r, double variance, double weight)
        def replaceGD(self, index, indexGauss, r, variance, weight):
            gaussian[index][indexGauss].variance = variance
            gaussian[index][indexGauss].meanValue = r
            gaussian[index][indexGauss].weight = weight

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::adjustWeight(int index, int indexGauss, double learningRate, bool matched)
        def adjustWeight(self, index, indexGauss, learningRate, matched):
            if matched:
                gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight+learningRate
            else:
                gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight


#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::findAnomaly(Mat frame)
        def findAnomaly(self, frame):
            r = Rect()
            i = 0
            count = None

            if isAnomaly:
#C++ TO PYTHON CONVERTER WARNING: An assignment within expression was extracted from the following statement:
#ORIGINAL LINE: countFrame = countFrame--;
                countFrame = countFrame
                countFrame -= 1

            countFirst = countFrame

            while i<particleVect.size() and not particleVect.empty():
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
                tx = (particleVect[i].x-math.ceil(width/2) if (particleVect[i].x-math.ceil(width/2)<frame.cols) ) else frame.cols-1) if (particleVect[i].x-math.ceil(width/2)>=0) ) else 0
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
                ty = (particleVect[i].y-math.ceil(height/2) if (particleVect[i].y-math.ceil(height/2)<frame.rows) ) else frame.rows-1) if (particleVect[i].y-math.ceil(height/2)>=0) ) else 0
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
                bx = (particleVect[i].x+math.ceil(width/2) if (particleVect[i].x+math.ceil(width/2)<frame.cols) ) else frame.cols-1) if (particleVect[i].x+math.ceil(width/2)>=0) ) else 0
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
                by = (particleVect[i].y+math.ceil(height/2) if (particleVect[i].y+math.ceil(height/2)<frame.rows) ) else frame.rows-1) if (particleVect[i].y+math.ceil(height/2)>=0) ) else 0
                r = Rect(tx,ty,(bx-tx),(by-tx))
                count = 0

                j = 0
                while j<particleVect.size():
                    if particleVect[j].x > r.x and particleVect[j].x < r.x+r.width and particleVect[j].y > r.y and particleVect[j].y < r.y+r.height:
                        count += 1
                    j += 1
                if count> thresh:
                    if isAnomaly:
                        countFrame = numNormal
                    else:
#C++ TO PYTHON CONVERTER WARNING: An assignment within expression was extracted from the following statement:
#ORIGINAL LINE: countFrame = countFrame++;      
                        countFrame = countFrame
                        countFrame += 1
                    #
                    i = particleVect.size()

                i += 1

            if isAnomaly:
                if countFrame == 0:
                    isAnomaly = False
            else:
                if countFrame >= numAnomaly:
                    isAnomaly = True
                    countFrame = numNormal
                elif countFirst == countFrame:
                    countFrame = 0

            frame.release()
            particleVect.clear()


#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmGaussianModel::findAnomalyInsideRect(Mat frame,Rect anomalyRect)
        def findAnomalyInsideRect(self, frame, anomalyRect):
            r = Rect()
            i = 0
            count = 0

            if isAnomaly:
#C++ TO PYTHON CONVERTER WARNING: An assignment within expression was extracted from the following statement:
#ORIGINAL LINE: countFrame = countFrame--;
                countFrame = countFrame
                countFrame -= 1

            countFirst = countFrame

            while i<particleVect.size() and not particleVect.empty():
                if mmLib.mmModules.isPointContRect(anomalyRect,particleVect[i]):
                    count += 1

                if count> thresh:
                    if isAnomaly:
                        countFrame = numNormal
                    else:
#C++ TO PYTHON CONVERTER WARNING: An assignment within expression was extracted from the following statement:
#ORIGINAL LINE: countFrame = countFrame++;
                        countFrame = countFrame
                        countFrame += 1

                    i = particleVect.size()

                i += 1

            if isAnomaly:
                if countFrame == 0:
                    isAnomaly = False
            else:
                if countFrame >= numAnomaly:
                    isAnomaly = True
                    countFrame = numNormal
                elif countFirst == countFrame:
                    countFrame = 0

            frame.release()
            particleVect.clear()

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: bool mmGaussianModel::getAnomaly()
        def getAnomaly(self):
            return isAnomaly
