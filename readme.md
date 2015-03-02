This is a plugin for [mpv](https://github.com/mpv-player/mpv "mpv on github") to interract with [Philips Hue](http://www2.meethue.com/en-US/ "Hue website") lamps and try to mimic an ambilight setup.

There're some examples videos at the bottom of the [following page](http://cocoaintheshell.com/2015/02/mpv-philips-hue-ambilight/ "Philips Hue + mpv = Ambilight").


### Technical

It is written in *C++11*, why ? because I needed some handy containers like unordered sets.

The algorithm is trivial (dumb, even) and could surely be improved. The color for left and right lamp is computed from the 4 columns of pixels on the left and right edge of the frame and for the middle a square of 64x64 at the center of the frame.

You need ffmpeg to be installed, but that's not a problem since you already use mpv.


### Building

I only tried on OS X 10.10 but building on Linux should be the same, just execute the `build.sh` script.


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
