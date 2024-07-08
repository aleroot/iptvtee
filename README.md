#  iptvtee 
iptvtee is a small utility to check quality of M3U streams.

## usage

```
    iptvtee [--format=txt|url|json|csv|m3u --jobs=15 --time=60 --runs=3 --score=1 --max=5 --page=url] file.m3u
```
### Parameters descritpion 

 - format: export format printed out on stdout
 - jobs: number of concurrent iptv flows runned concurrenlty
 - time: reference time for playback of the flow(default: 60 seconds)
 - runs: number of repeated runs of the test playback(default: 1 run)
 - score: minimum score of the flow. tested flow under this minimum are filtered out from the result.
 - page: an HTML url page to scrape for .m3u links
 - filter: input filter expresion, checking for test contains in title of the M3U.
 - max: the max number of M3U elements to test in a playlist.

### usage examples

**_Test m3u file_**
```
iptvtee --time=5 --score=1 M3u-Server.m3u8
```
_Test an M3U file for 5 seconds and filter out links with score below 1._

**_Test m3u file from STDIN_**
```
echo M3u-Server.m3u8 | VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins iptvtee
```
_Test an M3U file passed from standard input._

**_Test m3u input from STDIN_**
```
cat file1.m3u8 file2.m3u8 | VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins iptvtee
```
_Test an M3U input(multiple files) passed from standard input._

**_Test m3u urls_**
```
iptvtee --time=5 --filter=F1 http://url1.org:8000/get.php?username=x&password=x&type=m3u http://url2.org:8000/get.php?username=y&password=y&type=m3u
```
_Test an M3U file for 5 seconds and filter out links with score below 1._

**_Test a text file_**
```
iptvtee --max=5 m3u-links.txt
```
_Test a text file containing a list of m3u urls, check maximum 5 elements per m3u url in the text file._

**_Scrape web page_**
```
iptvtee --page=https://example.com/page.html --format=json --time=5 
```
_Scrape all m3u links from a web page and try to test them all for 5 seconds, output in JSON format._

### configuration

The application is using vlcpp library to access an instance of VLC, so in order to be able to run the application correclty the `VLC_PLUGIN_PATH` environment variable must be set and point to the path of VLC plugins.
On MacOs the VLC plugins path is usually: `/Applications/VLC.app/Contents/MacOS/plugins`, so the variable should be set in this way: `VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins`.

## Build

In order to be able to build the project a compiler compatible with [C++20](https://en.wikipedia.org/wiki/C%2B%2B20) standard is required.

* ### Linux
To build on Linux just run make on the main project directory.
Just make sure the dependencies libraries development packages are available on the system.
To install in `/usr/local/bin` just run `make install` after `make`.

* ### macOS
To build on MacOS just open the iptvtee.xcodeproj and build it from there.
**Note** that the xcodeproj file contains the following changed settings:

    1. Headers Search Path(Target section) for the headers of the library to be imported(LibVLC, libCurl)
    2. Library Search Path(Target section) for the binaries to be linked dynamically by the linker
    3. Other Linker Flags for the linker directive to link the library by name(-lvlc -lcurl) 
    4. Runpath Search Paths for the Runtime search paths needed for @rpath variable

In case the compiler and/or linker complain about some missing path, you need to adjust the paths accordingly to your system...

### Dependencies
The two main dependencies are libcurl and libVLC.

## Build

The unit tests are objective-c++ unit tests that can be run within the XCode IDE or with `xcodebuild`. 

## libiptvtee dynamic library
### Building libiptvtee dynamic library
The library is useful for python `ctypes` binding in addition that to other C/C++ applications.

```bash
IPTVTEE_PATH=$HOME/Downloads/iptvtee
cd  $IPTVTEE_PATH/iptvtee
g++ -std=c++20 -shared -o libiptvtee.so -fPIC iptv/analyzer.cpp iptv/report.cpp iptv/exporter.cpp iptv/evaluator.cpp iptv/playlist.cpp iptv/http/downloader.cpp -lcurl -lvlc -I/Applications/VLC.app/Contents/MacOS/include/ -I$IPTVTEE_PATH/libs -Iutils/ -L/Applications/VLC.app/Contents/MacOS/lib/
```

### Using libiptvtee dynamic library from Python
In order to use the library from Python a wrapper is necessary in order to call the C exported functions of the library to the python runtime. 
See iptvtee.py in the root of this project for an example of the wrapper.

#### Example test script to use the wrapper

Contained inside iptvtee.py there is an example wrapper implementation for Python, to be able to score 
an m3u flow or an entire playlist from a Python script like in the example below:

```python
#!/usr/bin/python3
"""test.py - demo of using a c++ class from within Python"""
from iptvtee import Analyzer

#Create an Analyzer object with a url to test
f = Analyzer(b"https://example.com/test.m3u8")

x = f.score()
print("Result is: {}".format(x))
print("Type of result is {}".format(type(x)))
```
