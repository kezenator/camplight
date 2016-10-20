# CampLight
A WiFi-controlled 12-volt light box for partying in the bush.

# Info
See the [Schematics.pdf](https://github.com/kezenator/camplight/blob/master/Schematic.pdf)
file for a circuit diagram.

See the [Instructions.pdf](https://github.com/kezenator/camplight/blob/master/Instructions.pdf)
file for instructions on how to connect and access the control web-site.

# Building under Windows
The software can be build in Windows.
This allows the web-site and pattern generation
to be tested quickly and easily on any Windows PC.

To build, Visual Studio 2015 is required.

First extract the `src/ThirdParty/boost/boost_1_59_0.7z`
archive as described in `src/ThirdParty/boost/README.txt`

Then, open the `src/CampLight.sln` file and build in either
Win32 or x64 mode.

The application needs to be passed a single command line argument - the
IPv4 socket address to bind the web-server to.

For example: `CampLight.exe 127.0.0.1:80`

Once running, open this address in a web-browser.
You will be able to see the rendered patterns. However, the patterns
cannot be rendered to real LEDs while running on Windows.

# Building on the Raspberry Pi
The software builds on the Raspberry Pi in Raspberian Jessie. It requires g++ 4.9.

The following libraries are required to build the software on the Raspberry Pi:

```
sudo apt-get install uuid-dev libssl-dev
```

Then:

```
cd src
make boost
make run_test_all
make
sudo ./build/camplight 127.0.0.1:80
```

Make can be run in parallel using the -j option.
