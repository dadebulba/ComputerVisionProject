# Anomaly Detection modules

## Module 2
### Initialization
Params to tune:
- threshParam: number of abnormal particle to consider the frame as abnormal
- numAnomalyParam: number of abnormal consecutive frames to raise the alarm
- numNormalParam: number of normal consecutive frames to raise down the alarm

### How it works
1. An initial grid of points (one every 8 pixels) is considered on the image and used as starting point.
2. Using the Lucas-Kanade method, given the previous points to track, we identify the optical flow vectors of those points between 2 frames.
3. Every 3 frames, the displacement between the first and third frame points is computed iff the flow was identified in the previous step for those specific points
4. Using the Mixture of Gaussian method (where the number of Gaussian is fixed to 3), the background and foreground are separated for the selected point of the previous step, the remaining point that were excluded are automatically associated to the background. The foreground points are then drawn to the image as red dots.
5. If the selected foreground points are more than the defined 'threshParam' for 'numAnomalyParam' consecutive frames, the anomaly is detected and will remain active for 'numNormalParam' number of frames

## Module 3
- Define the regions of interest inside the scene (implemented as rectangles)
- Accumulation phase: the frames between _profileFrameStartParam_ and _profilePeriodParam_ are used to build a model that defines the standard motion of the crowd
- Evaluation phase: the computed model is used to check if inside the region of interest there is minor or major presence of people. This is shown coloring each RoI rectangle:
	- grey: no anomaly
	- green: standard anomaly
	- blue/red: low/high anomaly with respect to the computed particle energy
