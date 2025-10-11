FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    wget \
    flex \
    bison \
    gperf \
    python3 \
    python3-pip \
    python3-venv \
    cmake \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0 \
    && apt-get autoremove

WORKDIR /usr/src/

ENV IDF_PATH=/usr/src/esp/esp-idf
ENV IDF_PATH_FORCE=1

RUN mkdir -p /usr/src/esp/firmware
RUN git clone --recursive https://github.com/espressif/esp-idf.git $IDF_PATH

RUN cd $IDF_PATH && ./install.sh esp32c3

COPY . /usr/src/esp/firmware/

WORKDIR /usr/src/esp/firmware/
RUN rm -rf build && /bin/bash -c "source $IDF_PATH/export.sh && idf.py set-target esp32c3 && idf.py build"

# Copy binary to standard location for extraction
RUN mkdir -p /usr/src/build && cp build/plant-manager.bin /usr/src/build/esp32.bin
