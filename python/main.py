from mmAnomaly.mmParticleAccumulation import Point, mmParticleAccumulationSettings, mmParticleAccumulation
from mmAnomaly.mmParticleEnergy import mmParticleEnergySettings, mmParticleEnergy
from mmAnomaly.mmGaussianModel import mmGaussianModelSettings, mmGaussianModel
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "mmLib/mmLib.h"

if __name__ == "__main__":
    nameVideo = "Uni"
    nameVideoFile = "../VIDEO/"+nameVideo+".avi"
    nameVideoToSave = nameVideo+"4Vid.avi"
    nameVideoText = nameVideo+".txt"
    nameVideoImg = nameVideo+".png"

    #capture a video
    cap = VideoCapture()
    cap.open(nameVideoFile)

    #check if there is a video
    if not cap.isOpened():
        print("ERROR: Stream not found", end = '')
        print("\n", end = '')

    scaleX = 0.25
    scaleY = 0.25
    dimX = cap.get(CV_CAP_PROP_FRAME_WIDTH)
    dimY = cap.get(CV_CAP_PROP_FRAME_HEIGHT)

    fps = cap.get(CV_CAP_PROP_FPS)
    frameW = dimX
    frameH = dimY
    video = VideoWriter(nameVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(frameW, frameH), True)

    #surveillance area vertex
    contours = []
    stuff = []
    stuff.append(Point(70,185))
    stuff.append(Point(190,185))
    stuff.append(Point(190,110))
    stuff.append(Point(460,110))
    stuff.append(Point(460,185))
    stuff.append(Point(460,260))
    stuff.append(Point(70,260))
    stuff.append(Point(70,185))
    contours.append(stuff)

    #create objects
    paSet = mmParticleAccumulationSettings()
    paSet.trackPeriodParam = 49
    paSet.minMovementParam = 5
    paSet.gridGranularityParam = 8

    pAccumul = mmParticleAccumulation(paSet)

    peSet = mmParticleEnergySettings()
    rectVect = []
    rectVect.append(Rect(70,185,120,75))
    rectVect.append(Rect(190,185,120,75))
    rectVect.append(Rect(190,110,120,75))
    peSet.surveilanceRectParam = rectVect
    peSet.sigmaParam = 0.05
    peSet.filterSizeParam = Size(31,31)
    peSet.profileFrameStartParam = 0
    peSet.profileLengthParam = 1000
    peSet.profilePeriodParam = 1000
    peSet.valuePeriodParam = 1000
    peSet.trackingPeriodParam = paSet.trackPeriodParam+1

    pEnergy = mmParticleEnergy(peSet)

    with open('Uni.txt', 'w') as writer:

        writer.write("UNI\n")
        writer.write("Surveilance Area: ")

        i = 0
        while i<len(stuff):
            writer.write(f"({stuff[i].x}, {stuff[i].y}), ")
            i += 1
        writer.write("\n")
        writer.write(f"TrackPeriod={paSet.trackPeriodParam}\n")
        writer.write(f"MinMovement={paSet.minMovementParam}\n")
        writer.write(f"GridGranularity={paSet.gridGranularityParam}\n")
        writer.write("\n")
        writer.write("Surveilance Rect: ")
        i = 0
        while i<len(rectVect):
            writer.write(f"(x,y,w,h)=({rectVect[i].x},{rectVect[i].y},{rectVect[i].width},{rectVect[i].height})")
            i += 1
        writer.write("\n")
        writer.write(f"Sigma={peSet.sigmaParam}\n")
        writer.write(f"FilterSizeParam=({peSet.filterSizeParam.height},{peSet.filterSizeParam.width})\n")
        writer.write(f"Frame recorded=({peSet.profileFrameStartParam}-{peSet.profileFrameStartParam+peSet.profileLengthParam})\n")
        writer.write(f"Profile period={peSet.profilePeriodParam}\n")
        writer.write(f"Value period={peSet.valuePeriodParam}\n")
        writer.write("\n")

    immagine = Mat()
    frame = Mat()
    reference = Mat()
    iteration = Mat()
    iAnomaly = Mat()
    particleAdvection = Mat()
    iOut = Mat()
    oneP = True
    oneV = True
    firstFrame = True
    countIt = 0
    countFrame = 0
    energyProfile = []
    energyValue = []
    maxRef = None
    maxIt = None
    vect = []
    anomalyVect = []

    while True:
        cap >> immagine #does this work?
        resize(immagine,frame,Size(0,0),scaleX,scaleY)

        if immagine.empty():
            break
        immagine.release()
        frame.copyTo(iAnomaly)

        #initailize modelue
        if firstFrame:
            reference = Mat.zeros(frame.rows,frame.cols,CV_32FC3)
            iteration = Mat.zeros(frame.rows,frame.cols,CV_32FC3)
            firstFrame = False

        #initailize module
        pEnergy.setUpModule(frame)
        pAccumul.setUpModule(frame)
        #advect partcile
        pAccumul.computeParticleAdvection(frame)
        #get final particle
        vect = pAccumul.getFinalParticle()

        if pEnergy.getRec():
            #record phase
            if oneV:
                #				cout << "REC PHASE" << endl
                oneV = False
            if (countFrame+1)%(paSet.trackPeriodParam+1)==0 and countFrame > peSet.profileFrameStartParam:
                pEnergy.processFrame(frame,vect)

                if countFrame != 0 and (countFrame+1)%(peSet.profilePeriodParam)==0:
                    with open('Uni.txt', 'a') as writer:
                        writer.write(f"*********************************FRAME: %d*********************************\n",countFrame)
                        energyProfile = pEnergy.getProfileEnVect(countIt)
                        writer.write(f"ENERGY PROFILE: {countIt}")
                        l = 0
                        while l<energyProfile.size():
                            writer.write(f"-->({l}):: {energyProfile[l]} ")
                            l += 1
                        writer.write("\n")
                        writer.write(f"THRESH HIGH: {countIt}")
                        l = 0
                        while l<energyProfile.size():
                            writer.write(f"-->({l}):: {energyProfile[l]+peSet.threshHighParam}")
                            l += 1
                        writer.write("\n")
                        writer.write(f"THRESH LOW: {countIt}")
                        l = 0
                        while l<energyProfile.size():
                            writer.write(f"-->({l}):: {energyProfile[l]-peSet.threshLowParam}")
                            l += 1
                        writer.write("\n")
                    energyProfile.clear()

                    reference = pEnergy.getMatProfile()
                    minMaxLoc(reference, 0, maxRef, 0, 0)
                    reference = reference/maxRef
                    reference = reference *255
                    countIt += 1
            if countFrame>(peSet.profileLengthParam+peSet.profileFrameStartParam) or not pEnergy.getRec():
                print("STOP REC-->FRAME=  ", end = '')
                print(countFrame, end = '')
                print("\n", end = '')
                pEnergy.clearVariables()
                pAccumul.clearVariables()
                #pAccumul = mmParticleAccumulation(paSet) # COMMENTED FROM SOURCE
                cap.open(nameVideoFile)
                countFrame = -1
                countIt = 0
        else:
            #evaluation phase
            if oneP:
                #				cout << "EVALUATION PHASE" << endl
                oneP = False
            if (countFrame+1)%(paSet.trackPeriodParam+1)==0:
                pEnergy.processFrame(frame,vect)

                if countFrame != 0 and (countFrame+1)%(peSet.valuePeriodParam)==0:
                    with open('Uni.txt', 'a') as writer:
                        writer.write(f"*********************************FRAME: {countFrame}*********************************\n")
                        energyValue = pEnergy.getValueEnVect()
                        writer.write(f"ENERGY VALUE: {countIt}")
                        l = 0
                        while l<energyValue.size():
                            writer.write(f"-->(%d):: %f ",l,energyValue[l])
                            l += 1
                        writer.write("\n")
                        anomalyVect = pEnergy.getAnomalyType()
                        if anomalyVect[0] == mmLib.mmAnomaly.STANDARD:
                            writer.write("0::STD ")
                        elif anomalyVect[0] == mmLib.mmAnomaly.HIGHANOMALY:
                            writer.write("0::HIGHANOMALY ")
                        elif anomalyVect[0] == mmLib.mmAnomaly.LOWANOMALY:
                            writer.write("0::LOWANOMALY ")


                        if anomalyVect[1] == mmLib.mmAnomaly.STANDARD:
                            writer.write("1::STD ")
                        elif anomalyVect[1] == mmLib.mmAnomaly.HIGHANOMALY:
                            writer.write("1::HIGHANOMALY ")
                        elif anomalyVect[1] == mmLib.mmAnomaly.LOWANOMALY:
                            writer.write("1::LOWANOMALY ")


                        if anomalyVect[2] == mmLib.mmAnomaly.STANDARD:
                            writer.write("2::STD ")
                        elif anomalyVect[2] == mmLib.mmAnomaly.HIGHANOMALY:
                            writer.write("2::HIGHANOMALY ")
                        elif anomalyVect[2] == mmLib.mmAnomaly.LOWANOMALY:
                            writer.write("2::LOWANOMALY ")
                        writer.write(pFile,"\n")
                        
                    energyValue.clear()
                    iteration = pEnergy.getMatValue()
                    minMaxLoc(iteration, 0, maxIt, 0, 0)
                    iteration = iteration/maxIt
                    iteration = iteration *255
                    countIt += 1
                    if anomalyVect.size() == 0:
                        rectangle(iAnomaly,rectVect[0],Scalar(0,255,0),2)
                        rectangle(iAnomaly,rectVect[1],Scalar(0,255,0),2)
                        rectangle(iAnomaly,rectVect[2],Scalar(0,255,0),2)
                    else:
                        l = 0
                        while l<rectVect.size():
                            if anomalyVect[l] == mmLib.mmAnomaly.STANDARD:
                                rectangle(iAnomaly,rectVect[l],Scalar(0,255,0),2)
                            elif anomalyVect[l] == mmLib.mmAnomaly.HIGHANOMALY:
                                rectangle(iAnomaly,rectVect[l],Scalar(0,0,255),2)
                            else:
                                rectangle(iAnomaly,rectVect[l],Scalar(0,125,255),2)
                            l += 1

        imshow("Video",frame)
        waitKey(10)

        if video.isOpened() and ((not pEnergy.getRec()) or (pEnergy.getRec() and countFrame > peSet.profileFrameStartParam and countFrame < (peSet.profileFrameStartParam+peSet.profileLengthParam))):
            video << iAnomaly

        countFrame += 1
        frame.release()
        iAnomaly.release()
    reference.release()
    iteration.release()
    energyProfile.clear()
    energyValue.clear()
    vect.clear()
    anomalyVect.clear()
    rectVect.clear()
    contours.clear()
    stuff.clear()
    pAccumul = None
    pEnergy = None





