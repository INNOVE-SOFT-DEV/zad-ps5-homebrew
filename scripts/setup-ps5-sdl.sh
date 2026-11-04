#!/usr/bin/env bash
# Construit le portage SDL2 PS5 + libsamplerate dans ./deps (une seule fois).
# Prerequis : PS5_PAYLOAD_SDK defini (ex. /opt/ps5-payload-sdk), cmake, clang, lld, git, curl.
set -euo pipefail
: "${PS5_PAYLOAD_SDK:?definir PS5_PAYLOAD_SDK}"
source "$PS5_PAYLOAD_SDK/toolchain/prospero.sh"
cd "$(dirname "$0")/.." && mkdir -p deps && cd deps

[ -d SDL ] || git clone --depth 1 https://github.com/ps5-payload-dev/SDL.git SDL
(cd SDL && ${CMAKE} -DCMAKE_BUILD_TYPE=Release -DSDL_OPENGL=NO -DSDL_OPENGLES=NO \
  -DSDL_VULKAN=NO -DSDL_LOADSO=YES -DSDL_SHARED=NO -DSDL_STATIC=YES -DSDL_TESTS=NO \
  -B build -S . && ${MAKE} -C build -j"$(nproc)")

if [ ! -f libsamplerate/src/.libs/libsamplerate.a ]; then
  V=0.2.2
  curl -sSL -o ls.tar.xz "https://github.com/libsndfile/libsamplerate/releases/download/$V/libsamplerate-$V.tar.xz"
  echo "3258da280511d24b49d6b08615bbe824d0cacc9842b0e4caf11c52cf2b043893  ls.tar.xz" | sha256sum -c -
  tar xf ls.tar.xz && rm -rf libsamplerate && mv "libsamplerate-$V" libsamplerate
  (cd libsamplerate && ./configure --prefix="$PWD/out" --host=x86_64-pc-freebsd \
     --enable-static --disable-shared && ${MAKE} -j"$(nproc)")
fi
echo "OK : deps/SDL/build/libSDL2.a et deps/libsamplerate/src/.libs/libsamplerate.a prets"
