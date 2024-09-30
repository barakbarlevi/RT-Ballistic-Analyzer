## A Real-Time Linux Application for Ballistic Trajectories Analysis
xxxx rename repo to: Real-time ballistic analyzer xxxx\
xxxx Full document in this link xxxx LINK!\
In this work, an application for ballistic trajectories analysis was created. The part of the application that provides data on the target’s state as captured by a generic sensor is implemented as a real time cyclic task running on an Cortex®‑A7. An x86_64 version is provided here as well. On the host machine, C++ code will synchronously read and compare that data to the results of an on the fly as-fast-as-possible 6DOF simulation, while interfacing with Google Earth. A thread that takes the target detections and simulation results runs in the background, and is used to calculate whether conditions relating the two are met. These conditions can be easily modified.

### Requirements
XXXX
### Build and execute
