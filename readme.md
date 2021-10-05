# Anomaly Detection modules

## Module 2
- Get the next image flow using Lucas-Kanade optical flow pyramid method
- Get foreground points and outline them as anomalies using MixtureOfGaussian method 
- If number anomalies are over the specified threshold then the rectangle becomes red

## Module 3
- Define the regions of interest inside the scene (implemented as rectangles)
- Accumulation phase: the frames between _profileFrameStartParam_ and _profilePeriodParam_ are used to build a model that defines the standard motion of the crowd
- Evaluation phase: the computed model is used to check if inside the region of interest there is minor or major presence of people. This is shown coloring each RoI rectangle:
	- grey: no anomaly
	- green: standard anomaly
	- blue/red: low/high anomaly with respect to the computed particle energy
