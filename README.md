## A Real-Time Linux Application for Ballistic Trajectories Analysis
xxxx rename repo to: Real-time ballistic analyzer xxxx\
xxxx Full document in this link xxxx LINK!\
In this work, an application for ballistic trajectories analysis was created. The part of the application that provides data on the target’s state as captured by a generic sensor is implemented as a real time cyclic task running on an ARM Cortex®‑A7. An x86_64 version is provided here as well. On the host machine, C++ code will synchronously read and compare that data to the results of an on the fly as-fast-as-possible 6DOF simulation, while interfacing with Google Earth. A thread that takes the target detections and simulation results runs in the background, and is used to calculate whether conditions relating the two are met. These conditions can be easily modified.

XXXX GIF

The application comprises 3 parts:
1. **rt_sendDetection** - Acting as a client, this program sends the entire contents of a .asc file one line after the other in a periodic fashion, via a unix socket to the host machine, until EOF is reached. The periodicity simulates live target tracking. Intended to run as a real-time task on an ARM Cortex®‑A7. Can be run on x86_64 as well.
2. **SIX_DOF** - A Six Degrees of Freedom simulation for an unguided rocket. Based on the open source CADAC simulation by Prefessor Petel Zipfel.
3. **MOJO** - Acting as a server, synchronously reads and compares track data to the results of the 6-DOF, while interfacing with Google Earth. Runs the background thread that alerts when irregularities are detected.

### Requirements
Should work on conventional Debian machine. No package installations required.
XXXX is it though? rephrase? 

### Build and execute
```
git clone https://github.com/barakbarlevi/RT-Ballistic-Analyzer.git
cd RT-Ballistic-Analyzer
make
```
This should execute three makefiles sequentially, one for each of the parts mentioned above, resulting in the creation of the corresponding binaries.
