#C++ TO PYTHON CONVERTER WARNING: The following #include directive was ignored:
##include "../mmLib.h"

class mmLib(object): #this class replaces the original namespace 'mmLib'
    class mmAnomaly(object): #this class replaces the original namespace 'mmAnomaly'
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmParticleEnergySettings::mmParticleEnergySettings()
        def __init__(self):
            surveilanceRectDimParam = Size(40, 40)
            filterSizeParam = Size(31, 31)
            surroundingAreaParam = Size(75, 60)
            sigmaParam = 0.05
            threshHighParam = 20
            threshLowParam = 20
            valuePeriodParam = 1000
            profilePeriodParam = 1000
            profileFrameStartParam = 0
            profileLengthParam = 1000
            trackingPeriodParam = 50

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmParticleEnergy::mmParticleEnergy()
        def __init__(self):
            surveilanceRectDim = Size(40, 40)
            filterSize = Size(31, 31)
            surroundingArea = Size(75, 60)
            sigma = 0.05
            threshHigh = 20
            threshLow = 20
            valuePeriod = 1000
            profilePeriod = 1000
            profileFrameStart = 0
            profileLength = 1000
            trackingPeriod = 50
            countFrame = 0
            totalFrameProcessed = -1
            profileEnIndex = 0
            firstFrame = True

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmParticleEnergy::mmParticleEnergy(mmParticleEnergySettings &peSet)
        def __init__(self, peSet):
            profileEnVect = peSet.profileEnVectParam #refernce enrgy vector
            surveilanceRect = peSet.surveilanceRectParam #surveillance area
            surveilanceRectDim = peSet.surveilanceRectDimParam #dimension of the rectangles' surveillance area needed if surroundingAreaParam is not set
            filterSize = peSet.filterSizeParam #dimension of the gaussian filter
            surroundingArea = peSet.surroundingAreaParam #area around a particle to store energy
            sigma = peSet.sigmaParam #variance value for the energy function
            threshHigh = peSet.threshHighParam #high threshold for anomaly detection
            threshLow = peSet.threshLowParam #low threshold for anomaly detection
            valuePeriod = peSet.valuePeriodParam #length of the evaluation period
            profilePeriod = peSet.profilePeriodParam #length of the reference period
            profileFrameStart = peSet.profileFrameStartParam #frame number to start storing the reference energy
            profileLength = peSet.profileLengthParam #frame numbers of the reference period
            trackingPeriod = peSet.trackingPeriodParam #tracking period
            countFrame = 0
            totalFrameProcessed = -1
            profileEnIndex = 0
            firstFrame = True
            peSet.profileEnVectParam.clear()
            peSet.surveilanceRectParam.clear()

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: mmParticleEnergy::~mmParticleEnergy()
        def ~mmParticleEnergy(self):
            matProfile.release()
            matValue.release()
            profileEnVect.clear()
            valueEnVect.clear()
            surveilanceRect.clear()

#initialize module
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmParticleEnergy::setUpModule(Mat frame)
def setUpModule(self, frame):
    if firstFrame:
        firstFrame = False
        needToRec = False if (profileEnVect.size() != 0) else True

        if surveilanceRect.size() == 0:
            i = 0
            while i < frame.cols:
                j = 0
                while j < frame.rows:
                    surveilanceRect.push_back(Rect(i, j, surveilanceRectDim.width, surveilanceRectDim.height))
                    j = j + surveilanceRectDim.height
                i = i + surveilanceRectDim.width

    if needToRec and totalFrameProcessed % profilePeriod == 0:
        matProfile = Mat.zeros(frame.rows, frame.cols, CV_32FC3)
    if (not needToRec) and totalFrameProcessed % valuePeriod == 0:
        matValue = Mat.zeros(frame.rows, frame.cols, CV_32FC3)
        valueEnVect.clear()
    totalFrameProcessed += 1

#store the reference energy
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmParticleEnergy::setProfileEnVect(list<list<float>> energyVect)
def setProfileEnVect(self, energyVect):
    profileEnVect = energyVect

#clear variables after the rec phase
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmParticleEnergy::clearVariables()
def clearVariables(self):
    countFrame = 0
    totalFrameProcessed = -1

#each frame is processed
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: void mmParticleEnergy::processFrame(Mat frame, list<Point2f> particles)
def processFrame(self, frame, particles):
    matStuff = Mat()
    period = None
    energyVectStuff = []
    if needToRec:
        matStuff = matProfile
        period = profilePeriod
    else:
        matStuff = matValue
        period = valuePeriod
    countFrame += 1

    matStuff = computeParticleEnergy(matStuff, particles)
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
    if countFrame != 0 and countFrame % (period / trackingPeriod) == 0:
#C++ TO PYTHON CONVERTER TODO TASK: The following line was determined to be a copy assignment (rather than a reference assignment) - this should be verified and a 'copy_from' method should be created:
#ORIGINAL LINE: matStuff = matStuff / countFrame;
        matStuff.copy_from(matStuff / countFrame)
        GaussianBlur(matStuff, matStuff, filterSize, 0, 0, BORDER_DEFAULT)
        energyVectStuff = computeEnergyPerArea(matStuff)
        countFrame = 0

        if needToRec:
            matProfile = matStuff
            profileEnVect.push_back(energyVectStuff)
            needToRec = False if (totalFrameProcessed == profileFrameStart + profileLength - 1) else True
        else:
            matValue = matStuff
            valueEnVect = energyVectStuff
    matStuff.release()
    energyVectStuff.clear()

import math

#compute anomaly
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: list<bool> mmParticleEnergy::getAnomaly()
def getAnomaly(self):
    anomalyVect = []

    i = 0
    while i < valueEnVect.size():
        if profileEnVect.size() == 1:
            if valueEnVect[i] - profileEnVect[0][i] > threshHigh or valueEnVect[i] - profileEnVect[0][i] < -threshLow:
                anomalyVect.append(True)
            else:
                anomalyVect.append(False)
        else:
            if valueEnVect[i] - profileEnVect[profileEnIndex][i] > threshHigh or valueEnVect[i] - profileEnVect[profileEnIndex][i] < -threshLow:
                anomalyVect.append(True)
            else:
                anomalyVect.append(False)
            profileEnIndex = 0 if (profileEnIndex == profileEnVect.size() - 1) else profileEnIndex + 1
        i += 1
    return list(anomalyVect)

#compute the type of anomaly
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: list<mmLib::mmAnomaly::ANOMALY_TYPE> mmParticleEnergy::getAnomalyType()
def getAnomalyType(self):
    anomalyVect = []
    oneTime = True

    i = 0
    while i < valueEnVect.size():
        if profileEnVect.size() == 1:
            if valueEnVect[i] - profileEnVect[0][i] > threshHigh:
                anomalyVect.append(HIGHANOMALY)
            elif valueEnVect[i] - profileEnVect[0][i] < -threshLow:
                anomalyVect.append(LOWANOMALY)
            else:
                anomalyVect.append(STANDARD)
        else:
            if valueEnVect[i] - profileEnVect[profileEnIndex][i] > threshHigh:
                anomalyVect.append(HIGHANOMALY)
            elif valueEnVect[i] - profileEnVect[profileEnIndex][i] < -threshLow:
                anomalyVect.append(LOWANOMALY)
            else:
                anomalyVect.append(STANDARD)
        i += 1
    if profileEnVect.size() > 1 and oneTime:
        profileEnIndex = 0 if (profileEnIndex == profileEnVect.size() - 1) else profileEnIndex + 1
        oneTime = False
    return list(anomalyVect)

#compute the particle energy matrix as a summation of the previous particle energy matrix
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: Mat mmParticleEnergy::computeParticleEnergy(Mat energyMatrix, list<Point2f> particles)
def computeParticleEnergy(self, energyMatrix, particles):
    pSize = len(particles)
    energyResult = Mat(energyMatrix)

    i = 0
    while i < pSize:
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
        tx = (particles[i].x - math.ceil(surroundingArea.width / 2) if (particles[i].x - math.ceil(surroundingArea.width / 2) < energyMatrix.cols) else energyMatrix.cols - 1) if (particles[i].x - math.ceil(surroundingArea.width / 2) >= 0) else 0
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
        ty = (particles[i].y - math.ceil(surroundingArea.height / 2) if (particles[i].y - math.ceil(surroundingArea.height / 2) < energyMatrix.rows) else energyMatrix.rows - 1) if (particles[i].y - math.ceil(surroundingArea.height / 2) >= 0) else 0
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
        bx = (particles[i].x + math.ceil(surroundingArea.width / 2) if (particles[i].x + math.ceil(surroundingArea.width / 2) > 0) else 0) if (particles[i].x + math.ceil(surroundingArea.width / 2) < energyMatrix.cols) else energyMatrix.cols - 1
#C++ TO PYTHON CONVERTER TODO TASK: C++ to Python Converter cannot determine whether both operands of this division are integer types - if they are then you should change 'lhs / rhs' to 'math.trunc(lhs / float(rhs))':
        by = (particles[i].y + math.ceil(surroundingArea.height / 2) if (particles[i].y + math.ceil(surroundingArea.height / 2) > 0) else 0) if (particles[i].y + math.ceil(surroundingArea.height / 2) < energyMatrix.rows) else energyMatrix.rows - 1
        cont = Rect(tx, ty, (bx - tx), (by - ty))
        energy = 0

        j = 0
        while j < pSize:
            if mmLib.mmModules.isPointContRect(cont, particles[j]):
                dist = particles[i].x - particles[j].x ** 2 + particles[i].y - particles[j].y ** 2
                energy = energy + math.exp(-dist / (2 * sigma ** 2))
            j += 1

        energyResult.at<Vec3f>(particles[i].y, particles[i].x)[1] = energyResult.at<Vec3f>(particles[i].y, particles[i].x)[1] + float(energy)
        i += 1

    return Mat(energyResult)

#compute energy in each single rectangle
#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: list<float> mmParticleEnergy::computeEnergyPerArea(Mat energyMatrix)
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

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: list<float> mmParticleEnergy::getProfileEnVect(int index)
def getProfileEnVect(self, index):
    return profileEnVect[index]

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: list<float> mmParticleEnergy::getValueEnVect()
def getValueEnVect(self):
    return valueEnVect

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: Mat mmParticleEnergy::getMatProfile()
def getMatProfile(self):
    return matProfile

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: Mat mmParticleEnergy::getMatValue()
def getMatValue(self):
    return matValue

#C++ TO PYTHON CONVERTER WARNING: The original C++ declaration of the following method implementation was not found:
#ORIGINAL LINE: bool mmParticleEnergy::getRec()
def getRec(self):
    return needToRec
