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

### Build from source
```
git clone https://github.com/barakbarlevi/RT-Ballistic-Analyzer.git
cd RT-Ballistic-Analyzer
make
```
This should execute three Makefiles sequentially, one for each of the parts mentioned above, resulting in the creation of the corresponding binaries.\
In order to execute `./rt_sendDetection` on an ARM machine, a cross compiler is needed. See this documentation xxxx or check online for cross compiling for ARM Cortex®‑A7.\
For demonstration purposes, assuming that the general viewer doesn't have the necessary cross-compiler and hardware currently availble, the Makefile performs native compilation.

### Usage
1. Open Google Earth. Drag `RT-Ballistic-Analyzer/MOJO/Primary_Controller.kml` into it. $${\color{red}XXXX ADD ONE TO GITHUB, CURRENTLY IGNORED}$$
2. Navigate to the launch point site. By default, `RT-Ballistic-Analyzer/MOJO/inputOriginal.asc` contains the (Lat, Lon) coordinates of Vandenberg Air Force Base, CA.
3. Start the server:\
`./MOJO_BINARY -j [port] -f [path] -h [heightFirstDetection]` - testests
`./MOJO_BINARY` - testest\

4. Send target detections:\
 `./rt_sendDetection_BINARY -i [IP] -j [port] -f [path_to_file] -n [period_ns] -p [priority]` - sdfsdfdsf\
 `./rt_sendDetection_BINARY` - sdfsdfsd

#### Examples
The default path without specifing the -f option is `/home/user/RT-Ballistic-Analyzer`.

##### Running entirely on an x86_64 machine
###### Example 1
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer`
Open new terminal
Divergent trajectory: `./rt_sendDetection_BINARY`
Non-divergent trajectory: `./rt_sendDetection_BINARY -f rt_sendDetection/V180-Orig.asc`

###### Example 2
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer -j 45454 -h 14739`
Open new terminal
`./rt_sendDetection_BINARY -j 45454`

##### Running the rt_sendDetection_BINARY on ARMv7-A
For example for a host machine with IP 192.168.0.3, Could be any other valid IPv4 address.

###### Example 1
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer`
On target machine:
`./rt_sendDetection_BINARY_Armv7-A -i 192.168.0.3 -f V180.asc`

###### Example 2
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer -j 55443 -h 14739`
Open new terminal:
`./rt_sendDetection_BINARY_Armv7-A -i 192.168.0.3 -j 55443 -f V180-Orig.asc`

