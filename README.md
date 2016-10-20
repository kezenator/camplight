# CampLight
A WiFi-controlled 12-volt light box for partying in the bush.

# Info
See the Schematics.pdf file for a circuit diagram.
See the Instructions.pdf file for instructions on how the system is used.

# Building under Windows
The software can be build in Windows.
This allows the web-site and pattern generation
to be tested quickly and easily on any Windows PC.

To build, Visual Studio 2015 is required.

First extract the src/ThirdParty/boost/boost_1_59_0.7z
as described in src/ThirdParty/boost/README.txt

Then, open the src/CampLight.sln file and build in either
Win32 or x64 mode.

# Building on the Raspberry Pi
The software builds on the Raspberry Pi in Raspberian Jessie. It requires g++ 4.9.

The following libraries are required to build the software on the Raspberry Pi:

sudo apt-get install uuid-dev libssl-dev

Then:

cd src
make boost
make run_test_all
make

Make can be run in parallel using the -j option.
