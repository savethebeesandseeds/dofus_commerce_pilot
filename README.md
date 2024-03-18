### Download or clone this Repo

> https://github.com/savethebeesandseeds/dofus_commerce_pilot

### To build create a docker container 
Open CMD and type these commands:

> cd /path/to/dofus_commerce_pilot

> docker pull debian:11

> docker run -it --name=commerce_pilot -v .:/src debian:11 

> docker exec -it commerce_pilot /bin/bash

### Install linux dependencies 

> apt update && apt install mingw-w64 make cmake g++ python3 pip pkg-config git --no-install-recommends -y

> apt install --reinstall ca-certificates --no-install-recommends -y

### Download mingw-std-threads (as MinGW does not support linux threads)

> mkdir /external && cd /external

> git clone https://github.com/meganz/mingw-std-threads

### This project requires both OpenCV and Tesseract to work on Windows

> see: https://github.com/savethebeesandseeds/tesseract_mingw

> see: https://github.com/savethebeesandseeds/opencv_mingw

### follow those instructions to build, and continue with

Basically to build you need to have those two succesfull builds in your local ./build folder. 

### Build

make test64 -j$(nproc)