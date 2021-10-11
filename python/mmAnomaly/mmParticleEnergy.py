import math

class mmParticleEnergySettings:
    def __init__(self):
        self.surveilanceRectDimParam = (40, 40)
        self.filterSizeParam = (31, 31)
        self.surroundingAreaParam = (75, 60)
        self.sigmaParam = 0.05
        self.threshHighParam = 20
        self.threshLowParam = 20
        self.valuePeriodParam = 1000
        self.profilePeriodParam = 1000
        self.profileFrameStartParam = 0
        self.profileLengthParam = 1000
        self.trackingPeriodParam = 50

class mmParticleEnergy:
    # FIXME: profileEnVectParam and surveilanceRectParam
    def __init__(self, peSet = None):
        self.surveilanceRectDim = peSet.surveilanceRectDimParam if hasattr(peSet,'surveilanceRectDimParam') else (40, 40)
        self.filterSize = peSet.filterSizeParam if hasattr(peSet,'filterSizeParam') else (31, 31)
        self.surroundingArea = peSet.surroundingAreaParam if hasattr(peSet,'surroundingAreaParam') else (75, 60)
        self.sigma = peSet.sigmaParam if hasattr(peSet,'sigmaParam') else 0.05
        self.threshHigh = peSet.threshHighParam if hasattr(peSet,'threshHighParam') else 20
        self.threshLow = peSet.threshLowParam if hasattr(peSet,'threshLowParam') else 20
        self.valuePeriod = peSet.valuePeriodParam if hasattr(peSet,'valuePeriodParam') else 1000
        self.profilePeriod = peSet.profilePeriodParam if hasattr(peSet,'profilePeriodParam') else 1000
        self.profileFrameStart = peSet.profileFrameStartParam if hasattr(peSet,'profileFrameStartParam') else 0
        self.profileLength = peSet.profileLengthParam if hasattr(peSet,'profileLengthParam') else 1000
        self.trackingPeriod = peSet.trackingPeriodParam if hasattr(peSet,'trackingPeriodParam') else 50
        self.countFrame = 0
        self.totalFrameProcessed = -1
        self.profileEnIndex = 0
        self.firstFrame = True
        peSet.profileEnVectParam.clear()
        peSet.surveilanceRectParam.clear()
    
    # FIXME: profileEnVect, surveilanceRect, matProfile, matValue missing
    def setUpModule(self, frame):
        if self.firstFrame:
            self.firstFrame = False
            needToRec = False if (profileEnVect.size() != 0) else True

            if surveilanceRect.size() == 0:
                i = 0
                while i < frame.cols:
                    j = 0
                    while j < frame.rows:
                        surveilanceRect.append(Rect(i, j, self.surveilanceRectDim.width, self.surveilanceRectDim.height))
                        j = j + self.surveilanceRectDim.height
                    i = i + self.surveilanceRectDim.width

        if needToRec and self.totalFrameProcessed % self.profilePeriod == 0:
            matProfile = Mat.zeros(frame.rows, frame.cols, CV_32FC3)
        if (not needToRec) and self.totalFrameProcessed % self.valuePeriod == 0:
            matValue = Mat.zeros(frame.rows, frame.cols, CV_32FC3)
            valueEnVect.clear()
        self.totalFrameProcessed += 1

    def clearVariables(self):
        self.countFrame = 0
        self.totalFrameProcessed = -1
    
    # FIXME needToRec, matProfile, matValue, valuePeriod, profileEnVect missing
    def processFrame(self, frame, particles):
        matStuff = Mat()
        period = None
        energyVectStuff = []
        if needToRec:
            matStuff = matProfile
            period = self.profilePeriod
        else:
            matStuff = matValue
            period = self.valuePeriod
        countFrame += 1
    
        matStuff = computeParticleEnergy(matStuff, particles)
        if countFrame != 0 and countFrame % (period / self.trackingPeriod) == 0:
            matStuff.copy_from(matStuff / countFrame)
            GaussianBlur(matStuff, matStuff, self.filterSize, 0, 0, BORDER_DEFAULT)
            energyVectStuff = self.computeEnergyPerArea(matStuff)
            countFrame = 0

            if needToRec:
                matProfile = matStuff
                profileEnVect.append(energyVectStuff)
                needToRec = False if (self.totalFrameProcessed == self.profileFrameStart + self.profileLength - 1) else True
            else:
                matValue = matStuff
                valueEnVect = energyVectStuff
        matStuff.release()
        energyVectStuff.clear()

    # FIXME: valueEnVect, profileEnVect missing
    def getAnomaly(self):
        anomalyVect = []

        i = 0
        while i < valueEnVect.size():
            if profileEnVect.size() == 1:
                if valueEnVect[i] - profileEnVect[0][i] > self.threshHigh or valueEnVect[i] - profileEnVect[0][i] < -self.threshLow:
                    anomalyVect.append(True)
                else:
                    anomalyVect.append(False)
            else:
                if valueEnVect[i] - profileEnVect[self.profileEnIndex][i] > self.threshHigh or valueEnVect[i] - profileEnVect[self.profileEnIndex][i] < -self.threshLow:
                    anomalyVect.append(True)
                else:
                    anomalyVect.append(False)
                self.profileEnIndex = 0 if (self.profileEnIndex == profileEnVect.size() - 1) else self.profileEnIndex + 1
            i += 1
        return list(anomalyVect)

    # FIXME: valueEnVect, profileEnVect enum:(HIGHANOMALY, LOWANOMALY, STANDARD() missing
    def getAnomalyType(self):
        anomalyVect = []
        oneTime = True

        i = 0
        while i < valueEnVect.size():
            if profileEnVect.size() == 1:
                if valueEnVect[i] - profileEnVect[0][i] > self.threshHigh:
                    anomalyVect.append(HIGHANOMALY)
                elif valueEnVect[i] - profileEnVect[0][i] < -self.threshLow:
                    anomalyVect.append(LOWANOMALY)
                else:
                    anomalyVect.append(STANDARD)
            else:
                if valueEnVect[i] - profileEnVect[self.profileEnIndex][i] > self.threshHigh:
                    anomalyVect.append(HIGHANOMALY)
                elif valueEnVect[i] - profileEnVect[self.profileEnIndex][i] < -self.threshLow:
                    anomalyVect.append(LOWANOMALY)
                else:
                    anomalyVect.append(STANDARD)
            i += 1
        if profileEnVect.size() > 1 and oneTime:
            self.profileEnIndex = 0 if (self.profileEnIndex == profileEnVect.size() - 1) else self.profileEnIndex + 1
            oneTime = False
        return list(anomalyVect)

    #compute the particle energy matrix as a summation of the previous particle energy matrix
    # missing mmLib.mmModules.isPointContRect(cont, particles[j]):
    def computeParticleEnergy(self, energyMatrix, particles):
        pSize = len(particles)
        energyResult = Mat(energyMatrix)

        i = 0
        while i < pSize:
            tx = (particles[i].x - math.ceil(self.surroundingArea.width / 2) if (self.particles[i].x - math.ceil(self.surroundingArea.width / 2) < energyMatrix.cols) else energyMatrix.cols - 1) if (particles[i].x - math.ceil(self.surroundingArea.width / 2) >= 0) else 0
            ty = (particles[i].y - math.ceil(self.surroundingArea.height / 2) if (self.particles[i].y - math.ceil(self.surroundingArea.height / 2) < energyMatrix.rows) else energyMatrix.rows - 1) if (particles[i].y - math.ceil(self.surroundingArea.height / 2) >= 0) else 0
            bx = (particles[i].x + math.ceil(self.surroundingArea.width / 2) if (self.particles[i].x + math.ceil(self.surroundingArea.width / 2) > 0) else 0) if (particles[i].x + math.ceil(self.surroundingArea.width / 2) < energyMatrix.cols) else energyMatrix.cols - 1
            by = (particles[i].y + math.ceil(self.surroundingArea.height / 2) if (self.particles[i].y + math.ceil(self.surroundingArea.height / 2) > 0) else 0) if (particles[i].y + math.ceil(self.surroundingArea.height / 2) < energyMatrix.rows) else energyMatrix.rows - 1
            cont = Rect(tx, ty, (bx - tx), (by - ty))
            energy = 0

            j = 0
            while j < pSize:
                if mmLib.mmModules.isPointContRect(cont, particles[j]):
                    dist = particles[i].x - particles[j].x ** 2 + particles[i].y - particles[j].y ** 2
                    energy = energy + math.exp(-dist / (2 * self.sigma ** 2))
                j += 1

            energyResult.at<Vec3f>(particles[i].y, particles[i].x)[1] = energyResult.at<Vec3f>(particles[i].y, particles[i].x)[1] + float(energy)
            i += 1

        return Mat(energyResult)

    #compute energy in each single rectangle
    #FIXME: surveilanceRect, rectangle function,  missing 
    def computeEnergyPerArea(self, energyMatrix):
        mask = Mat()
        stuff = Mat()
        energyVector = []

        l = 0
        while l < surveilanceRect.size():
            mask = Mat.zeros(energyMatrix.rows, energyMatrix.cols, CV_8U)
            rectangle(mask, surveilanceRect[l], Scalar(255, 255, 255), CV_FILLED)
            energyMatrix.copyTo(stuff, mask)
            mask.release()
            res = sum(stuff)
            energyVector.append(res.val[1])
            stuff.release()
            l += 1
        return list(energyVector)