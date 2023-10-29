
FROM ubuntu:22.04

ENV LANG C.UTF-8
ENV PICO_SDK_PATH=/workspaces/pico-sdk

RUN apt update && apt install -y \
    build-essential \
    clangd \
    cmake \
    git \
    gcc-arm-none-eabi \
    gdb-multiarch \
    minicom \
    pkg-config \
    libusb-1.0-0-dev \
    python3-pip \
    && apt clean

RUN pip install compiledb

WORKDIR /workspaces

RUN git clone --branch 1.5.1 https://github.com/raspberrypi/pico-sdk.git --depth=1 &&\
    cd pico-sdk && git submodule update --init --depth=1 -j 100

RUN cd pico-sdk && mkdir build && cd build && cmake .. && make -j `nproc`

RUN git clone --depth 1 --branch 1.1.2 https://github.com/raspberrypi/picotool.git  && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j `nproc` && \
    cp /workspaces/picotool/build/picotool /bin/picotool

RUN mkdir -p /etc/udev/rules.d/ 
RUN cp /workspaces/picotool/udev/99-picotool.rules /etc/udev/rules.d/
RUN rm -rf /workspaces/picotool

EXPOSE 2020

CMD ["/bin/bash"]
