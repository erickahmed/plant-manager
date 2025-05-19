FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends\
    build-essential \
    gcc-avr \
    avrdude \
    arduino-core-avr \
    make \
    && apt-get clean

WORKDIR /usr/src/

COPY . .

RUN make all
