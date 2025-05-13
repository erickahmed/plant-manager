#!/usr/bin/env bash
set -e

IMAGE_NAME="vase-control-builder"
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
IMAGE_NAME="vase-control-builder"
DOCKERFILE="$SCRIPT_DIR/Dockerfile"
BUILD_CTX="$SCRIPT_DIR"
DEST_HEX="$SCRIPT_DIR/build/moisture.hex"
MCU="atmega328p"; PRG="usbasp"; AVR_FLAGS="-B 10"

docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$BUILD_CTX"

mkdir -p "$(dirname "$DEST_HEX")"

docker run --rm "$IMAGE_NAME" \
  cat /usr/src/build/moisture.hex > "$DEST_HEX"

command -v avrdude >/dev/null || { echo "Install avrdude"; exit 1; }
avrdude -v -p "$MCU" -c "$PRG" $AVR_FLAGS -D -U flash:w:"$DEST_HEX":i
