#!/bin/bash

LIBNAME="ambi"
FLAGS=""
LIBS=""
FILES="src/ambi.cpp src/rgba_pixel.cpp src/point.cpp src/hue_client.cpp src/filterutils.cpp src/hue_controller.cpp src/img_formats.cpp"
USE_FFMPEG="1"
USE_OPENCL="1"

if [ "$USE_FFMPEG" == "1" ]; then
	FLAGS="$FLAGS -DNYX_USE_FFMPEG"
	LIBS="$LIBS -lswscale -lavcodec -lavutil"
else
	if [ "$USE_OPENCL" == "1" ]; then
		FILES="$FILES src/cl_global.cpp"
		FLAGS="$FLAGS -DNYX_USE_OPENCL"
		if [[ "$OSTYPE" == "darwin"* ]]; then
			LIBS="$LIBS -framework OpenCL"
		else
			LIBS="$LIBS -lOpenCL"
		fi
	fi
fi

# Build with debug
if [ "$1" == "debug" ]; then
	FLAGS="$FLAGS -DDEBUG"
fi

# primitive os detection (Linux / OS X)
if [[ "$OSTYPE" == "darwin"* ]]; then
	LIBNAME="$LIBNAME.dylib"
else
	LIBNAME="$LIBNAME.so"
fi

# Echo to stdout
set -x

g++ -o $LIBNAME $FILES -dynamiclib -std=c++11 -Wall -Wextra -O3 -march=native -mtune=native -ffast-math -fPIC -shared -flto -funroll-loops $LIBS $FLAGS
