## A Real-Time Linux Application for Ballistic Trajectories Analysis
[See the full documentation in this link](https://docs.google.com/document/d/1E4sZPrR8SMirfW4VgnSzsHGLtWLYPDiTgV4Ykeg03AU/edit?usp=sharing)

In this work, an application for ballistic trajectories analysis was created. The part of the application that provides data on the target’s state as captured by a generic sensor is implemented as a real time cyclic task running on an ARM Cortex®‑A7. An x86_64 version of it is provided here as well, to be run from a conventional host PC.\
On the host machine, C++ code will synchronously read and compare that data to the results of an on the fly as-fast-as-possible 6DOF simulation, while interfacing with Google Earth. A thread that takes the target detections and simulation results runs in the background, calculates whether conditions relating the two are met, in which case it'll send a notification. These conditions can be easily modified.

![RedTraj](https://github.com/user-attachments/assets/4337a711-6664-4af9-b5c2-fa2295bb56f8)

The application comprises 3 parts:
1. **rt_sendDetection** - Acting as a client, this program sends the entire contents of a .asc file one line after the other in a periodic fashion, via a unix socket to the host machine, until EOF is reached. The periodicity simulates live target tracking. Intended to run as a real-time task on an ARM Cortex®‑A7. An x86_64 version is given here, for which real-time context code is commented out.
2. **CADAC_6DOF** - A Six Degrees of Freedom simulation for an unguided rocket. Based on the open source CADAC simulation by Professor Petel Zipfel.
3. **MOJO** - Acting as a server, synchronously reads and compares track data to the results of the 6DOF, while interfacing with Google Earth. Runs the background thread that alerts when irregularities are detected.

### Requirements
###### 1. A Unix-like system with support for POSIX threads.
Library libx11-dev is required for X11 window system support. On debian based systems:\
`sudo apt install libx11-dev`\
Make sure g++ is installed on your machine:\
`g++ --version`
###### 2. Google Earth application
Download and install the latest version of the Google Earth app to your machine. For 64 bit systems:
```
sudo wget http://dl.google.com/dl/earth/client/current/google-earth-stable_current_amd64.deb
sudo dpkg -i google-earth-stable*.deb
sudo apt-get -f install
```

### Build from source
```
git clone https://github.com/barakbarlevi/RT-Ballistic-Analyzer.git
cd RT-Ballistic-Analyzer
make
```
This should execute three Makefiles sequentially, one for each of the parts mentioned above, resulting in the creation of the corresponding binaries.\
For demonstration purposes, assuming that the general viewer doesn't have the necessary cross compiler and hardware currently availble, the Makefile performs native compilation and produces an x86 binary named `rt_sendDetection_BINARY_x86`. When this type of compilation is done, no real-time attributes are set.\
In order to compile an ARM binary for a target board, a cross compiler is needed. See the [cross compiling part](https://docs.google.com/document/d/1E4sZPrR8SMirfW4VgnSzsHGLtWLYPDiTgV4Ykeg03AU/edit?tab=t.0#bookmark=id.ufanmj8n2ej) in this project's full documentation or check online for cross compiling for ARM Cortex®‑A7.


### Usage & Examples

###### Step 1: Start the server
`./MOJO_BINARY -j [port] -f [path] -h [heightFirstDetection]`
> [!TIP]
> Recommended first time usage:\
> `./MOJO_BINARY -f .`

Options:
```
-j [port]                    Set the port number (1-65535). If option isn't specified, the default port number is 36961
-f [path]                    Path to the base directory RT-Ballistic-Analyzer. If option isn't specified, the default path is /home/username/RT-Ballistic-Analyzer
-h [heightFirstDetection]    Set the height for first detection in meters (float value). If option isn't specified, the default height is 15000[m]
```

Minimize the terminal window to ensure that Google Earth is fully visible.
###### Step 2: Start Earth, navigate to the launch point site
If you want to avoid openning a new terminal, just launch Google Earth and drag `MOJO/Primary_Controller.kml` into it. Alternatively, this can be done from an available terminal as well:
```
cd .../RT-Ballistic-Analyzer
google-earth-pro "$(readlink -f MOJO/Primary_Controller.kml)"
```
The application opens a pop-up tip window upon startup, which may prevent automatic navigation and camera positioning from the command line. I haven’t found a workaround that simplifies this better than just manually closing the window and navigating.

By default, the detected launch's initial (Lat, Lon) coordinates, specified in `RT-Ballistic-Analyzer/MOJO/inputOriginal.asc`, are set to Vandenberg Air Force Base, CA.

Navigate there by typing “Vandenberg Air Force Base, CA” in the search box and hitting search. You may right click and use the “Show balloon” or “Fly here” options. Place your view to roughly match the one from the GIF in the beginning of this readme.
###### Step 3: Send target detections
In a new terminal window, run the following command to initiate data transfer to the server, which was started in step 1. At this point, trajectories visualization should start in Earth, so It's suggested to minimize any window that may hide it, including this one. 
```
cd .../RT-Ballistic-Analyzer
./rt_sendDetection_BINARY_x86 -i [IP] -j [port] -f [path_to_file] -n [period_ns] -p [priority]
```
> [!TIP]
> Recommended first time usage:\
> `./rt_sendDetection_BINARY_x86`

 Options:
```
-i [IP]                      IPv4 address of the host. If option isn't specified, default IP: 127.0.01
-j [port]                    Port number on which to write. If option isn't specified, the default port number is 36961
-f [path]                    Path to the detections file being transferred. If option isn't specified, the default file path is rt_sendDetection/V180.asc
-n [period_ns]               Set real-time task period. If option isn't specified, default is 15695067.264
-p [rt_priority]             Set real-time task priority
```
Setting real-time attributes occurs only when the `ARM_TARGET` macro is defined. This can be done using GCC's `-D` option during compilation. Only then is `rt_priority` used, and then its default value is `rt_priority = 80`.

> **🔔 Attention**
> The visualization process can be rerun at any time by restarting the server with step 1, followed by repeating step 3, each in their respective shells.\
> **🔔 Attention**
> Typically on VMs, the calculation-visualization process may get stuck in the middle or even get terminated by a segfault. This is possibly due to the VM underperforming. Repeat step 1 followed by step 3, each in their respective shells, for several times. This should help demonstrate that the errors were likely isolated incidents.

#### Examples
The default path without specifing the -f option is `/home/user/RT-Ballistic-Analyzer`.

##### Running entirely on an x86_64 machine
###### Example 1
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer`\
In a second terminal window:
```
cd .../RT-Ballistic-Analyzer
./rt_sendDetection_BINARY_x86                                     #Divergent trajectory, sends "V180.asc"
./rt_sendDetection_BINARY_x86 -f rt_sendDetection/V180-Orig.asc   #Non-divergent trajectory, sends file `V180-Orig.asc
```
###### Example 2
`./MOJO_BINARY -f /home/user/your/path/to/RT-Ballistic-Analyzer -j 45454 -h 14739`\
In a second terminal window:
```
cd .../RT-Ballistic-Analyzer
./rt_sendDetection_BINARY_x86 -j 45454
```
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

\
\
\
[Resume](https://docs.google.com/document/d/1m5FYQYV_6Bp2gykrT0-TXfoafpJeTPsTHQyDyNh_vQM/edit?usp=sharing)  \
[Meeting](https://docs.google.com/document/d/1fK89VsNQZkHkjXS-QmtUNuaOqucvi6YxTYAjbm5rMso/edit?usp=sharing)
