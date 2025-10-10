#!/usr/bin/env bash
set -e

IMAGE_NAME="esp32-builder"
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
DOCKERFILE="$SCRIPT_DIR/Dockerfile"
BUILD_CTX="$SCRIPT_DIR"
DEST_BIN="$SCRIPT_DIR/build/esp32.bin"

docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$BUILD_CTX"

mkdir -p "$(dirname "$DEST_BIN")"

docker run --rm "$IMAGE_NAME" \
  cat /usr/src/build/esp32.bin > "$DEST_BIN"

esptool.py --chip esp32c3 --port /dev/ttyUSB0 --baud 115200 write_flash 0x1000 firmware.bin \
|| { echo "To find the correct port run: ls /dev/cu.*. Then modify build-esp32.sh"; exit 1; }
