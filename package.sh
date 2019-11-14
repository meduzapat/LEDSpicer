#!/bin/bash
apt-get update
apt-get install -y build-essential autoconf libtool autotools-dev libusb-dev libtinyxml-dev libpulse-dev libpthread-stubs0-dev libtinyxml2-dev libusb-1.0-0-dev
./autogen.sh
./configure CXXFLAGS='-s -O3' --prefix=/usr --sysconfdir=/etc \
  --enable-pulseaudio \
  --enable-nanoled \
  --enable-pacdrive \
  --enable-pacled64 \
  --enable-ultimateio \
  --enable-ledwiz32
make
make DESTDIR=/tmp/project install
apt-get install -y ruby ruby-dev
gem install fpm
mv /tmp/project/tmp/project/usr/* /tmp/project/usr/
rm -rf /tmp/project/tmp
pushd . >/dev/null
cd /tmp/project

version=$(cat usr/lib/pkgconfig/ledspicer.pc | grep Version | awk '{print $2}')
# Dont capture the compiled devices
excludes=$(for i in $(find ./usr/lib/ledspicer/devices \( -name "*.la" -o -name "*.so" \)); do echo --exclude **/`basename $i`; done)

read -r -d '' DESCRIPTION << EOM
LEDSpicer LED controller and animator
 LEDSpicer is a program that will handle several LED controller boards to create animations, effects and display profiles, to be used with arcade cabinets, or other projects that require an interaction with their LEDs controlers.
 .
 This package will install the binaries: ledspicerd (main daemon), emitter (command line application to send commands to ledspicerd), and inputseeker (utility program to detect input devices and codes)
EOM

fpm -s dir \
  -t deb \
  -n ledspicer \
  -v ${version} \
  --iteration 1 \
  $excludes \
  --depends libtinyxml2-dev \
  --depends libusb-1.0-0-dev \
  --description "'${DESCRIPTION}'" \
  .

fpm -s dir \
  -t deb \
  -n ledspicer-ledwiz32 \
  -v ${version} \
  --iteration 1 \
  --depends ledspicer \
  --description "This package contains the output plugin for the Groovy Game Gear LedWiz, a 32 leds controller." \
  ./usr/lib/ledspicer/devices/LedWiz32.so

fpm -s dir \
  -t deb \
  -n ledspicer-nanoled \
  -v ${version} \
  --iteration 1 \
  --depends ledspicer \
  --description "This package contains the output plugin for the Ultimarc NanoLed, a 60 leds controller." \
  ./usr/lib/ledspicer/devices/UltimarcNanoLed.so


fpm -s dir \
  -t deb \
  -n ledspicer-pacled64 \
  -v ${version} \
  --iteration 1 \
  --depends ledspicer \
  --description "This package contains the output plugin for the Ultimarc PacLed 64, a 64 leds controller." \
  ./usr/lib/ledspicer/devices/UltimarcPacLed64.so

fpm -s dir \
  -t deb \
  -n ledspicer-pacdrive \
  -v ${version} \
  --iteration 1 \
  --depends ledspicer \
  --description "This package contains the output plugin for the Ultimarc PacDrive, a 16 leds controller." \
  ./usr/lib/ledspicer/devices/UltimarcPacDrive.so


fpm -s dir \
  -t deb \
  -n ledspicer-ultimaio \
  -v ${version} \
  --iteration 1 \
  --depends ledspicer \
  --description "This package contains the output plugin for the Ultimarc Ultimate IO, a 96 leds controller." \
  ./usr/lib/ledspicer/devices/UltimarcUltimate.so
popd >/dev/null
rm *.deb
mv /tmp/project/*.deb .
