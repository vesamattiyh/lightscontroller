FROM ubuntu:plucky

RUN apt-get update && apt-get install openssh-server \
    build-essential \
    pkg-config \
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
    valgrind \
    p7zip-full \
    curl \
    usbutils \
    git -y
    
# RUN mkdir /var/run/sshd && \
RUN mkdir -p ~/esp32

RUN echo 'root:redhat' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

WORKDIR /esp32

ENV PLATFORM_CORE_DIR=~/esp32/platformio PATH="~/esp32/platformio/bin:$PATH"
RUN echo $PATH
RUN python3 -m pip install -U platformio --break-system-packages

EXPOSE 22

# Start SSH server
CMD ["/usr/sbin/sshd", "-D"]