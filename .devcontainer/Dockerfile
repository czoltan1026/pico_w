
FROM ubuntu:22.04

ENV LANG C.UTF-8
ENV PICO_SDK_PATH=/workspaces/pico-sdk

RUN apt update && apt install -y \
    automake \
    autoconf \
    build-essential \
    ccache \
    clangd \
    cmake \
    git \
    gcc-arm-none-eabi \
    gdb-multiarch \
    libasio-dev \
    libtool \
    libhidapi-dev \
    minicom \
    pkg-config \
    libusb-1.0-0-dev \
    texinfo \
    p7zip-full \
    python3-pip \
    wget \
    && apt clean

RUN pip install compiledb

WORKDIR /workspaces

RUN git clone --branch=1.5.1 https://github.com/raspberrypi/pico-sdk.git --depth=1
RUN cd pico-sdk && git submodule update --init --depth=1 -j 12

RUN cd pico-sdk && mkdir build && cd build && cmake .. && make -j `nproc`

RUN git clone --depth 1 --branch=1.1.2 https://github.com/raspberrypi/picotool.git  && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j `nproc` && \
    cp /workspaces/picotool/build/picotool /bin/picotool

RUN git clone https://github.com/openocd-org/openocd.git --branch v0.12.0 --depth=1 --no-single-branch
RUN cd openocd &&  ./bootstrap && ./configure --enable-cmsis-dap && make -j`nproc` CFLAGS='-Wno-error' CXXFLAGS='-Wno-error'  && make install -j`nproc`

RUN wget https://altushost-swe.dl.sourceforge.net/project/asio/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz
RUN 7z x -y -so asio-1.30.2.tar.gz | 7z x -y -si -ttar
RUN cd asio-1.30.2 && chmod +x configure && ./configure && make install -j `nproc`


RUN mkdir -p /etc/udev/rules.d/ 
RUN cp /workspaces/picotool/udev/99-picotool.rules /usr/lib/udev/rules.d/
RUN cp /workspaces/openocd/contrib/60-openocd.rules /usr/lib/udev/rules.d/
RUN sed -i "s|660|666|g" /usr/lib/udev/rules.d/99-picotool.rules
RUN sed -i "s|ATTRS{idProduct}==\"0004\"|ATTRS{idProduct}==\"0003\"|g" /usr/lib/udev/rules.d/60-openocd.rules
RUN rm -rf /workspaces/picotool

EXPOSE 2020
EXPOSE 8080
EXPOSE 4241

CMD ["/bin/bash"]
