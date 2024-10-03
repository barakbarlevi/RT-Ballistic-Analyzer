## A Real-Time Linux Application for Ballistic Trajectories Analysis
xxxx Full document in this link xxxx LINK!\
In this work, an application for ballistic trajectories analysis was created. The part of the application that provides data on the target’s state as captured by a generic sensor is implemented as a real time cyclic task running on an ARM Cortex®‑A7. An x86_64 version of it is provided here as well, to be run from a conventional host PC.\
On the host machine, C++ code will synchronously read and compare that data to the results of an on the fly as-fast-as-possible 6DOF simulation, while interfacing with Google Earth. A thread that takes the target detections and simulation results runs in the background, calculates whether conditions relating the two are met, in which case it'll send a notification if. These conditions can be easily modified.

XXXX GIF

The application comprises 3 parts:
1. **rt_sendDetection** - Acting as a client, this program sends the entire contents of a .asc file one line after the other in a periodic fashion, via a unix socket to the host machine, until EOF is reached. The periodicity simulates live target tracking. Intended to run as a real-time task on an ARM Cortex®‑A7. An x86_64 version is given here, for which real-time context code is commented out.
2. **SIX_DOF** - A Six Degrees of Freedom simulation for an unguided rocket. Based on the open source CADAC simulation by Professor Petel Zipfel.
3. **MOJO** - Acting as a server, synchronously reads and compares track data to the results of the 6DOF, while interfacing with Google Earth. Runs the background thread that alerts when irregularities are detected.

### Requirements
A Unix-like system with support for POSIX threads.
Library libx11-dev is required for X11 window system support:
```bash
  sudo apt install libx11-dev
```

### Build from source
```
git clone https://github.com/barakbarlevi/RT-Ballistic-Analyzer.git
cd RT-Ballistic-Analyzer
make
```
This should execute three Makefiles sequentially, one for each of the parts mentioned above, resulting in the creation of the corresponding binaries.\
In order to execute `./rt_sendDetection` on an ARM machine, a cross compiler is needed. See this documentation xxxx or check online for cross compiling for ARM Cortex®‑A7.\
For demonstration purposes, assuming that the general viewer doesn't have the necessary cross-compiler and hardware currently availble, the Makefile performs native compilation.

### Usage & Examples
###### 1. Open Google Earth
Drag `RT-Ballistic-Analyzer/MOJO/Primary_Controller.kml` into it.
###### 2. Navigate Earth to the launch point site
By default, `RT-Ballistic-Analyzer/MOJO/inputOriginal.asc` contains the (Lat, Lon) coordinates of Vandenberg Air Force Base, CA.
###### 3. Start the server
`./MOJO_BINARY` , or:\
`./MOJO_BINARY -j [port] -f [path] -h [heightFirstDetection]`

Options:
```
-j [port]                    Set the port number (1-65535)
-f [path]                    Path to the base directory RT-Ballistic-Analyzer
-h [heightFirstDetection]    Set the height for first detection in meters (float value)
```
Default values when an option isn't specified:
```
port: 36961
path: /home/username/RT-Ballistic-Analyzer
heightFirstDetection: 15000
```
###### 4. Send target detections
`./rt_sendDetection_BINARY` , or:\
`./rt_sendDetection_BINARY -i [IP] -j [port] -f [path_to_file] -n [period_ns] -p [priority]`

 Options:
```
-i [IP]                      IPv4 address of the host
-j [port]                    Port number on which to write
-f [path]                    Path to the detections file being transferred
-n [period_ns]               Set real-time task period
-p [rt_priority]                Set real-time task priority
```
Default values when an option isn't specified:
```
IP: 127.0.01
port: 36961
path: rt_sendDetection/V180.asc
period_ns = 15695067.264
```
Only when running in a real-time environment is rt_priority used, and then its default value is:\
`rt_priority = 80`\
Setting real-time attributes is commented out in `rt_sendDetection/rt_sendDetection.c`, and can be uncommented when needed.


#### Examples
The default path without specifing the -f option is `/home/user/RT-Ballistic-Analyzer`.

##### Running entirely on an x86_64 machine
###### Example 1
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer`\
Open a new terminal.\
`cd .../RT-Ballistic-Analyzer`\
`./rt_sendDetection_BINARY` - Divergent trajectory, sends "V180.asc".\
`./rt_sendDetection_BINARY -f rt_sendDetection/V180-Orig.asc` - Non-divergent trajectory, sends "V180-Orig.asc".

###### Example 2
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer -j 45454 -h 14739`\
Open a new terminal.\
`cd .../RT-Ballistic-Analyzer`\
`./rt_sendDetection_BINARY -j 45454`

##### Running the rt_sendDetection_BINARY on ARMv7-A
Host machine with IP 192.168.0.3, Could be any other valid IPv4 address.

###### Example 1
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer`\
On target machine:\
`./rt_sendDetection_BINARY_Armv7-A -i 192.168.0.3 -f V180.asc`

###### Example 2
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer -j 55443 -h 14739`\
On target machine:\
`./rt_sendDetection_BINARY_Armv7-A -i 192.168.0.3 -j 55443 -f V180-Orig.asc`

