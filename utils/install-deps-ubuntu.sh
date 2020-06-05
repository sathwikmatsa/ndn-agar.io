#!/bin/bash

# upgrade g++ to version 10
sudo apt install g++-10

# install sdl2
sudo apt-get install libsdl2-dev

# install sdl2_image
sudo apt-get install libsdl2-image-dev

# install sdl2_ttf
sudo apt-get install libsdl2-ttf-dev

# install spdlog
sudo apt-get install libspdlog-dev
sudo apt-get install libfmt-dev

## setup yojimbo

mkdir tmp
cd tmp
# install premake5
wget -nc https://github.com/premake/premake-core/releases/download/v5.0.0-alpha15/premake-5.0.0-alpha15-linux.tar.gz

tar -xvf premake-5.0.0-alpha15-linux.tar.gz
sudo cp premake5 /usr/bin/

# install yojimbo deps
sudo apt-get install libsodium-dev
sudo apt-get install libmbedtls-dev

# download yojimbo src
git clone https://github.com/networkprotocol/yojimbo.git
cd yojimbo
git submodule update --init --recursive

# build yojimbo
sudo apt install fd-find
sudo mv /usr/bin/fdfind /usr/bin/fd
./../../utils/prefix_macro_yojimbo.sh
sudo mv /usr/bin/fd /usr/bin/fdfind
premake5 gmake
make all

# copy yojimbo files to ndn-agario src
cp bin/libyojimbo.a ./../../lib/
cp *.h ./../../include/yojimbo/
cp netcode.io/*.h ./../../include/yojimbo/
cp reliable.io/*.h ./../../include/yojimbo/
cp tlsf/*.h ./../../include/yojimbo/

# remove yojimbo build
cd ../../
rm -rf tmp
