import math
import numpy as np

def isValidCoord(point, img):
    return point[0] >= 0 and point[1] >= 0 and point[0] < img[0] and point[1] < img[1]

class mmGaussianModelSettings:
    def __init__(self):
        self.numGaussianParam = 3
        self.matchFactorParam = 2.5
        self.lrweightParam = 0.001
        self.lrvarianceParam = 0.001
        self.highVarianceParam = 150
        self.lowWeigthParam = 0.001
        self.tProvaParam = 0.5

        self.varianceParam = 1
        self.meanValueParam = 0
        self.weightParam = 0

        self.mWidthParam = 320
        self.mHeightParam = 240
        self.heightParam = 25
        self.widthParam = 35
        self.threshParam = 6
        self.numAnomalyParam = 3
        self.numNormalParam = 2

class mmGaussianModel:
    def __init__(self, settings = None):
        self.numGaussian = settings.numGaussianParam if hasattr(settings,'numGaussianParam') else 3
        self.matchFactor = settings.matchFactorParam if hasattr(settings,'matchFactorParam') else 2.5
        self.lrweight = settings.lrweightParam if hasattr(settings,'lrweightParam') else 0.001
        self.lrvariance = settings.lrvarianceParam if hasattr(settings,'lrvarianceParam') else 0.001
        self.highVariance = settings.highVarianceParam if hasattr(settings,'highVarianceParam') else 150
        self.lowWeigth = settings.lowWeigthParam if hasattr(settings,'lowWeigthParam') else 0.001
        self.tProva = settings.tProvaParam if hasattr(settings,'tProvaParam') else 0.5

        self.variance = settings.varianceParam if hasattr(settings,'varianceParam') else 1
        self.meanValue = settings.meanValueParam if hasattr(settings,'meanValueParam') else 0
        self.weight = settings.weightParam if hasattr(settings,'weightParam') else 0

        self.mWidth = settings.mWidthParam if hasattr(settings,'mWidthParam') else 320
        self.mHeight = settings.mHeightParam if hasattr(settings,'mHeightParam') else 240
        self.height = settings.heightParam if hasattr(settings,'heightParam') else 25
        self.width = settings.widthParam if hasattr(settings,'widthParam') else 35
        self.thresh = settings.threshParam if hasattr(settings,'threshParam') else 6
        self.numAnomaly = settings.numAnomalyParam if hasattr(settings,'numAnomalyParam') else 3
        self.numNormal = settings.numNormalParam if hasattr(settings,'numNormalParam') else 2

        self.isAnomaly = False
        self.firstFrame = True
        self.countFrame = 0

        initializeModel()
    
    def mmGaussianModelDestroyer():
        return None
    
    def initializeModel(self):
        return None
        #gaussianPixel = (self.variance, self.meanValue, self.weight)
        #gaussian = [0 for i in range(self.mWidth * self.mHeight)] 
        #for i in range(self.mWidth):
        #    for j in range(self.mHeight):
        #        for k in range(self.numGaussian):
        #            index = j*self.mWidth + i
        #            gaussian.append([]) 
    def MagnitudeFunt(self, img, VelocityImg, vectPoints, tempPoints, status, numFrameInit, isInit):
        if numFrameInit == 0:
            for m in range(len(vectPoints)):
                tempPoints.append(vectPoints[m])
        elif numFrameInit == 3:
            dist = 0
            for m in range(len(vectPoints)):
                if status[m] and isPositiveCoord(vectPoints[m], img) and isPositiveCoord(tempPoints[m], img):
                    dist = math.sqrt(pow(abs(tempPoints[m][0] - vectPoints[m][0]), 2) + pow(abs(tempPoints[m][1] - vectPoints[m][1]), 2))
                    x = int(vectPoints[m][0])
                    y = int(vectPoints[m][1])
                    value = int(dist)

                    if(dist > 8):
                        VelocityImg[y][x] = value  #VelocityImg.at<uchar>(y,x)
                    else:
                        VelocityImg[y][x] = 0
            numFrameInit = 0
            isInit = True
        
        numFrameInit+=1
        #img.release();  vectPoints.clear(); status.clear();


    def updateBackgroundModel(self, frame, velocityImage, background):
        return None

    # FIXME: Where gaussian and matchedIndex factor come from
    def findMatchGaussian(self, p, r):
        index = p[1] *self.mWidth + p[0]
        self.sortGD(index)

        result = False          
        isMatch = False
        indexI = 0

        #iterate through all the gaussian distributions in the container and find the first match
        i = 0
        while i<self.numGaussian and not isMatch:
            dist = math.sqrt((r-gaussian[index][i].meanValue) ** 2)
            if dist <= self.matchFactor*(math.sqrt(gaussian[index][i].variance)):
                isMatch = True
                indexI = i
            i += 1
        matchedIndex = indexI

        if isMatch:
            result = self.checkBackground(index)
            self.updateGaussian(index,indexI,r,lrvariance)
        else:
            # get least probable distribution and replace that gaussian distribution such that mean = current pixel and highvariance
            indexGauss = self.getLPGD(index)
            self.replaceGD(index, indexGauss, r, highVariance, lowWeigth)
        # adjust weights
        i = 0
        while i<self.numGaussian:
            #for matched gaussian distribution
            if indexI == i and isMatch == True:
                self.adjustWeight(index,i,self.lrweight,True)
            #for unmatched gaussian distribution
            else:
                self.adjustWeight(index,i,self.lrweight,False)
            i += 1

        return result
    
    # FIXME: Where gaussian factor come from
    def sortGD(self, index):
        i = 0
        while i<self.numGaussian-1:
            j = 0
            while j<self.numGaussian-1-i:
                if self.getWeightVarianceRatio(index,j+1) > self.getWeightVarianceRatio(index,j):
                    swap = gaussian[index][j]
                    gaussian[index][j] = gaussian[index][j+1]
                    gaussian[index][j+1] = swap
                j += 1
            i += 1
    
    # FIXME: Where gaussian factor come from
    def getWeightVarianceRatio(self, index, indexGauss):
        return (gaussian[index][indexGauss].weight/gaussian[index][indexGauss].variance)

    # FIXME: Where gaussian and matchedIndex factor come from
    def checkBackground(self, index):
        i = None
        sumWeights = 0
        backgroundIndex = 0

        i = 0
        while i<self.numGaussian:
            sumWeights += gaussian[index][i].weight

            if sumWeights > self.tProva:
                backgroundIndex = i
                break
            i += 1
        if matchedIndex <= backgroundIndex: 
            BG = True
        else:
            BG = False

        return BG

    # FIXME: Where gaussian factor come from
    def updateGaussian(self, index, indexGauss, r, learningRate):
        gaussian[index][indexGauss].meanValue = ((1-learningRate)*gaussian[index][indexGauss].meanValue) + (learningRate *r)
        gaussian[index][indexGauss].variance = ((1-learningRate)*gaussian[index][indexGauss].variance)+(learningRate *(r-gaussian[index][indexGauss].meanValue) ** 2)
    
    def getLPGD(self, index):
        value = self.getWeightVarianceRatio(index,0)
        tempIndex = 0

        i = 1
        while i<self.numGaussian:
            temp = self.getWeightVarianceRatio(index,i)
            if temp < value:
                value = temp
                tempIndex = i
            i += 1
        return tempIndex

    # FIXME: Where gaussian factor come from
    def replaceGD(self, index, indexGauss, r, variance, weight):
        gaussian[index][indexGauss].variance = variance
        gaussian[index][indexGauss].meanValue = r
        gaussian[index][indexGauss].weight = weight

    # FIXME: Where gaussian factor come from
    def adjustWeight(self, index, indexGauss, learningRate, matched):
        if matched:
            gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight+learningRate
        else:
            gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight
    
    # FIXME: Where particleVect and Rect come from
    def findAnomaly(self, frame):
        r = Rect()
        i = 0
        count = None

        if self.isAnomaly:
            self.countFrame -= 1

        countFirst = self.countFrame

        while i<particleVect.size() and not particleVect.empty():
            tx = (particleVect[i].x-math.ceil(self.width/2) if (particleVect[i].x-math.ceil(self.width/2)<frame.cols) ) else frame.cols-1) if (particleVect[i].x-math.ceil(self.width/2)>=0) ) else 0
            ty = (particleVect[i].y-math.ceil(self.height/2) if (particleVect[i].y-math.ceil(self.height/2)<frame.rows) ) else frame.rows-1) if (particleVect[i].y-math.ceil(self.height/2)>=0) ) else 0
            bx = (particleVect[i].x+math.ceil(self.width/2) if (particleVect[i].x+math.ceil(self.width/2)<frame.cols) ) else frame.cols-1) if (particleVect[i].x+math.ceil(self.width/2)>=0) ) else 0
            by = (particleVect[i].y+math.ceil(self.height/2) if (particleVect[i].y+math.ceil(self.height/2)<frame.rows) ) else frame.rows-1) if (particleVect[i].y+math.ceil(self.height/2)>=0) ) else 0
            r = Rect(tx,ty,(bx-tx),(by-tx))
            count = 0

            j = 0
            while j<particleVect.size():
                if particleVect[j].x > r.x and particleVect[j].x < r.x+r.width and particleVect[j].y > r.y and particleVect[j].y < r.y+r.height:
                    count += 1
                j += 1
            if count> self.thresh:
                if self.isAnomaly:
                    self.countFrame = self.numNormal
                else:
                    self.countFrame += 1

                i = particleVect.size()

            i += 1

        if self.isAnomaly:
            if self.countFrame == 0:
                self.isAnomaly = False
        else:
            if self.countFrame >= self.numAnomaly:
                self.isAnomaly = True
                self.countFrame = self.numNormal
            elif countFirst == self.countFrame:
                self.countFrame = 0

        frame.release()
        particleVect.clear()

    # FIXME: Where particleVect and Rect come from. Integrate mmLib.mmModules.isPointContRect
    def findAnomalyInsideRect(self, frame, anomalyRect):
        r = Rect()
        i = 0
        count = 0

        if self.isAnomaly:
            self.countFrame -= 1

        countFirst = self.countFrame

        while i<particleVect.size() and not particleVect.empty():
            if mmLib.mmModules.isPointContRect(anomalyRect,particleVect[i]):
                count += 1

            if count> self.thresh:
                if self.isAnomaly:
                    self.countFrame = self.numNormal
                else:
                    self.countFrame += 1

                i = particleVect.size()

            i += 1

        if self.isAnomaly:
            if self.countFrame == 0:
                self.isAnomaly = False
        else:
            if self.countFrame >= self.numAnomaly:
                self.isAnomaly = True
                self.countFrame = self.numNormal
            elif countFirst == self.countFrame:
                self.countFrame = 0

        frame.release()
        particleVect.clear()

    def getAnomaly(self):
        return self.isAnomaly