#!/bin/bash

FLAGS=""
LIBNAME="ambi"

if [ $1 == "debug" ]; then
	FLAGS="-DDEBUG"
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
	LIBNAME="$LIBNAME.dylib"
else
	LIBNAME="$LIBNAME.so"
fi

echo "g++ -o $LIBNAME -dynamiclib src/ambi.cpp src/rgb_pixel.cpp src/point.cpp src/hue_client.cpp src/filterutils.cpp src/hue_controller.cpp -std=c++11 -Wall -Wextra -O3 -march=native -mtune=native -ffast-math -fPIC -shared -flto -lswscale -lavcodec -lavutil $FLAGS"
g++ -o $LIBNAME -dynamiclib src/ambi.cpp src/rgb_pixel.cpp src/point.cpp src/hue_client.cpp src/filterutils.cpp src/hue_controller.cpp -std=c++11 -Wall -Wextra -O3 -march=native -mtune=native -ffast-math -fPIC -shared -flto -lswscale -lavcodec -lavutil $FLAGS
