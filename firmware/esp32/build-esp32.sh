#!/usr/bin/env bash
set -e

IMAGE_NAME="esp32-builder"
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
DOCKERFILE="$SCRIPT_DIR/Dockerfile"
BUILD_CTX="$SCRIPT_DIR"
DEST_DIR="$SCRIPT_DIR/build"

docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$BUILD_CTX"

mkdir -p "$DEST_DIR"

docker run --rm -v "$DEST_DIR":/output "$IMAGE_NAME" cp -r /usr/src/build/. /output/

#echo "Erasing flash..."
#esptool --chip esp32c3 --port /dev/tty.usbmodem101 erase-flash

esptool --chip esp32c3 --port /dev/tty.usbmodem101 --baud 115200 write-flash \
  --flash-mode dio \
  --flash-freq 40m \
  --flash-size 4MB \
  0x0000 "$DEST_DIR/bootloader.bin" \
  0x8000 "$DEST_DIR/partition-table.bin" \
  0x10000 "$DEST_DIR/plant-manager.bin"

screen -fn /dev/tty.usbmodem101 115200
