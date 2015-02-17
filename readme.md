This is a plugin for [mpv](https://github.com/mpv-player/mpv "mpv on github") to interract with [Philips Hue](http://www2.meethue.com/en-US/ "Hue website") lamps and try to mimic an ambilight setup.


### Technical

It is written in *C++11*, why ? because I needed some handy containers like unordered sets.

The algorithm is trivial (dumb, even) and could surely be improved. The color is computed from the 8 columns of pixels on the left and right edge.


### Building

I only tried on OS X 10.10 (g++ is an alias to clang++):

	g++ -o ambi.dylib -dynamiclib src/ambi.cpp src/rgb_pixel.cpp src/hue_client.cpp src/filterutils.cpp src/hue_controller.cpp -std=c++11 -Wall -Wextra -O3 -march=native -mtune=native -ffast-math -fPIC -shared -flto -lswscale -lavcodec -lavutil

Building on Linux should be failry simple though and pretty much looks the same.

You can execute the `build.sh` script which will build the library.


### Usage

**mpv have to be built with the `--enable-vf-dlopen-filters` switch.**

Then, simply invoke mpv with `--vf=dlopen=ambi.dylib:HUE_IP_ADDRESS` for example :

	mpv movie --vf=dlopen=ambi.dylib:192.168.1.20


### Limitations

- Too slow for my taste (handle 1080p in my tests)
- Won't build on Windows, but I don't care.
- Too many things hardcoded?


### License

The same as **mpv** I guess (GPLv2), but frankly I don't care, you can do whatever with the code which is not part of **mpv**.
