# matchmaking-server
The matchmaking server for pkmn-world. Written in C++.

## Building on Windows

### Requirements

* Install CMake and add it to your PATH: https://cmake.org/download/
* Install Microsoft Visual Studio 2015: https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx
* Install Git and add it to your PATH: https://git-scm.com/
* Install Boost: http://www.boost.org/ (https://sourceforge.net/projects/boost/files/boost-binaries/)

### Getting Sources
First of all, Boost needs to be set up. Set the following environment variables:

* `BOOST_HOME` = `C:\Path\to\boost`
* `BOOST_LIBRARYDIR` = `%BOOST_HOME%\lib64-msvc-14.0`
* `BOOST_ROOT` = `%BOOST_HOME%`

Then, download the sources for matchmaking-server:

     C:\Path\to> git clone https://github.com/pkmn-world/matchmaking-server.git

Go to the project folder:

     C:\Path\to>cd matchmaking-server
     C:\Path\to\matchmaking-server>

Initialize the submodules (protobuf):

	C:\Path\to\matchmaking-server> git submodule update --init

### CMake Configuration
Create a folder for the CMake temporary files:

	C:\Path\to\matchmaking-server> mkdir build & cd build
	C:\Path\to\matchmaking-server\build>

Now create the Visual Studio solution:

	C:\Path\to\matchmaking-server\build> cmake -G "Visual Studio 14 2015 Win64" ..

You might get a warning such as:

	CMake Warning (dev) at third-party/protobuf/cmake/install.cmake:41 (message):
	  The file
	  "C:/Path/to/matchmaking-server/third-party/protobuf/src/google/protobuf/repeated_field_reflection.h"
	  is listed in
	  "C:/Path/to/matchmaking-server/third-party/protobuf/cmake/cmake/extract_includes.bat.in"
	  but there not exists.  The file will not be installed.
	Call Stack (most recent call first):
	  third-party/protobuf/cmake/CMakeLists.txt:169 (include)
	This warning is for project developers.  Use -Wno-dev to suppress it.

This can be ignored as it is related to protobuf.

### Compiling

* Open the generated matchmaking-server.sln file in Microsoft Visual Studio 2015.
* Choose "Debug" or "Release" configuration as desired.
* From the Build menu, choose "Build Solution".

And wait for the compilation to finish.

## Building on Linux
TBD