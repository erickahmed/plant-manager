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

RUN mkdir -p ~/esp/firmware
RUN git clone --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
RUN cd ~/esp/esp-idf && ./install.sh esp32
ENV IDF_PATH=~/esp/esp-idf
RUN . $IDF_PATH/export.sh

COPY . ~/esp/firmware/

RUN cd ~/esp/firmware/
RUN idf.py set-target esp32
RUN idf.py menuconfig
RUN idf.py build
