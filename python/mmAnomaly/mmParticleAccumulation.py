import cv2
import numpy as np
import math

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class mmParticleAccumulationSettings:
    def __init__(self):
        self.minMovementParam = 20
        self.gridGranularityParam = 8
        self.trackPeriodParam = 50
        self.surveilanceAreaParam = []

class mmParticleAccumulation:
    def __init__(self):
        self.surveilanceArea = []

        stuff = []
        stuff.append(Point(0,0)) 
        stuff.append(Point(0,0)) 
        stuff.append(Point(0,0)) 
        stuff.append(Point(0,0))
        self.surveilanceArea.append(stuff); # surveilance area

        self.minMovement = 20        #min movement for a particle to be considered active
        self.gridGranularity = 8     #the system select one particle each gridGranularity pixel
        self.trackPeriod = 50        #temporal window size for energy accumulation
        self.totalFrameProcessed = 0
        self.countFrame = 0
        self.firstFrame = True

        self.initStaticFields()

    '''
    def __init__(self, paS):
        if not isinstance(paS, mmParticleAccumulationSettings):
            raise Exception("Type error")

        self.surveilanceArea = paS.surveilanceAreaParam     #surveilenace area vertex
        self.minMovement = paS.minMovementParam             #min movement for a particle to be considered as active
        self.gridGranularity = paS.gridGranularityParam     #grid dimensionality
        self.trackPeriod = paS.trackPeriodParam             #tracking period
        self.totalFrameProcessed = 0
        self.countFrame = 0
        self.firstFrame = True

        self.initStaticFields()
    '''

    def initStaticFields(self):
        self.actualPos = []
        self.finalParticle = []
        self.startParticle = []
        self.pathLenght = []
        self.pathLenghtFin = []
        self.pathLenghtX = []
        self.pathLenghtXFin = []
        self.pathLenghtY = []
        self.pathLenghtYFin = []
        self.firstFrameParticle = []
        self.prevFrame = None
        self.particleAdvection = None
        self.finalParticleMat = None

    def releaseAll(self):
        if self.prevFrame is not None:
            self.prevFrame.release()
        if self.particleAdvection is not None:
            self.particleAdvection.release()
        if self.finalParticleMat is not None:
            self.finalParticleMat.release()

    def setUpModule(self, frame):
        if self.firstFrame:
            self.firstFrame = False

            if len(self.surveilanceArea) == 0:
                stuff = []
                numRows, numCols = frame.shape

                stuff.append(Point(0,0))
                stuff.append(Point(0,numRows-1))
                stuff.append(Point(numCols-1,numRows-1))
                stuff.append(Point(numCols-1,0))

                self.surveilanceArea.append(stuff)

        frame.release()

    def selectParticle(self, frame):
        self.actualPos = []
        numRows, numCols = frame.shape

        for i in range(0, numCols, self.gridGranularity):
            for j in range (0, numRows, self.gridGranularity):
                for k in range (0, len(self.surveilanceArea)):
                    dist = cv2.pointPolygonTest(self.surveilanceArea[k], (i,j), True)
                    if dist >= 0:
                        self.actualPos.append(Point(i,j))

        frame.release()

    def clearVariables(self):
        self.totalFrameProcessed = 0
        self.countFrame = 0

    def computeParticleAdvection(self, frame):
        Energy = -1
        self.finalParticle = []
        numRows, numCols = frame.shape

        if self.countFrame == 0:
            self.selectParticle(frame)

            self.pathLenght = []
            self.pathLenghtFin = []
            self.pathLenghtX = []
            self.pathLenghtXFin = []
            self.pathLenghtY = []
            self.pathLenghtYFin = []
            self.firstFrameParticle = []

            for i in range (0, len(self.actualPos)):
                self.pathLenght.append(0)
                self.pathLenghtX.append(0)
                self.pathLenghtY.append(0)

            self.particleAdvection = np.zeros((numRows, numCols, 1), dtype = "uint8")
            self.finalParticleMat = np.zeros((numRows, numCols, 1), dtype = "uint8")
            test = np.zeros((numRows, numCols, 1), dtype = "uint8")
        else:
            finalPos = []
            movingParticle = []
            status = []
            err = []

            termcrit = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_MAX_ITER, 4, 0.04)
            winSize = (20, 20)

            # Optical flow
            cv2.calcOpticalFlowPyrLK(
                self.prevFrame,
                frame,
                self.actualPos,
                finalPos,
                status,
                err,
                winSize,
                0,
                termcrit
            )

            for i in range(len(finalPos)):
                cv2.line(
                    self.particleAdvection,
                    self.actualPos[i],
                    finalPos[i],
                    (0,255,0),
                    2, 8, 0
                )

            # Path lenght update
            for i in range(len(finalPos)):
                for k in range(len(self.surveilanceArea)):
                    dist = cv2.pointPolygonTest(self.surveilanceArea[k], finalPos[i], True)
                    if dist >= 0:
                        self.pathLenght[i] = self.pathLenght[i] + math.sqrt((finalPos[i].x - self.actualPos[i].x)**2 + (finalPos[i].y - self.actualPos[i].y)**2)
                        self.pathLenghtX[i] = self.pathLenghtX[i] + abs(finalPos[i].x - self.actualPos[i].x)
                        self.pathLenghtY[i] = self.pathLenghtY[i] + abs(finalPos[i].y - self.actualPos[i].y)
                    else:
                        self.pathLenght[i] = 0
                        self.pathLenghtX[i] = 0
                        self.pathLenghtY[i] = 0

            # Particle start
            if self.countFrame == 1:
                self.startParticle = self.actualPos

            # Analysis frame
            if self.countFrame == self.trackPeriod:
                for i in range(len(self.actualPos)):
                    for k in range(len(self.surveilanceArea)):
                        dist = cv2.pointPolygonTest(self.surveilanceArea[k], finalPos[i], True)
                        if self.pathLenght[i] > self.minMovement and dist >= 0:
                            movingParticle.append(finalPos[i])
                            self.firstFrameParticle.append(self.startParticle)
                            self.pathLenghtFin.append(self.pathLenght[i])
                            self.pathLenghtXFin.append(self.pathLenghtX[i])
                            self.pathLenghtYFin.append(self.pathLenghtY[i])

                self.finalParticle = movingParticle
                self.startParticle = []
                self.countFrame = -1

            self.actualPos = finalPos

        cv2.copyTo(frame, dst=self.prevFrame)
        frame.release()

        self.countFrame += 1
        self.totalFrameProcessed += 1