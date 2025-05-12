FROM ubuntu:22.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    gcc-avr \
    avrdude \
    arduino-core-avr \
    make \
    && apt-get clean

WORKDIR /usr/src/

COPY . .

RUN make

CMD ["./main"]
