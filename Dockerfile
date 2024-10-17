FROM ubuntu:18.04

## To avoid tzdata asking for geographic location...
ENV DEBIAN_FRONTEND=noninteractive
ENV APP_ROOT=/home/user/RCA
ENV XDG_RUNTIME_DIR=/tmp

## Build packages, as root, in the root's home folder.
ENV DIRPATH=/root/
WORKDIR $DIRPATH

## ------------------------------- Custom Dependencies -------------------------------
RUN apt-get update && \ 
    apt-get install -y \
      build-essential \
      pkg-config \
      cmake \
      libv4l-dev \
      libavcodec-dev \
      libavdevice-dev \
      libavfilter-dev \
      libavformat-dev \
      libavutil-dev \
      libpostproc-dev \
      libswresample-dev \
      libswscale-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*


## Add Cusom User (add home folder, set shell)
RUN useradd -m --shell /bin/bash --groups video,dialout user 
USER user

## Add alias source to ./bashrc
RUN echo "\n\
if [ -f /home/user/RCA/aliases_target.sh ]; then \n\ 
    source ./aliases_target.sh \n\ 
fi\n" >> ~/.bashrc

## ------------------------------ MYNT-EYE Dependencies ------------------------------


## ------------------------------- Kimera Dependencies -------------------------------
## Install required build packages (Based on Kimera Dockefile).
## Link: https://github.com/MIT-SPARK/Kimera-VIO/blob/master/Dockerfile_20_04
# RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
# RUN apt-get update && \
#     apt-get install -y \
#       build-essential \
#       cmake \
#       gfortran \
#       git \
#       libatlas-base-dev \
#       libboost-all-dev \
#       libeigen3-dev \
#       libgflags-dev \
#       libgoogle-glog-dev \
#       libmetis-dev \
#       libopencv-dev \
#       libopencv-contrib-dev \
#       libtbb-dev \
#       pkg-config \
#       xvfb \
#       python3 \
#       python3-dev \
#       python3-pip \
#       python3-tk && \
#     apt-get clean && \
#     rm -rf /var/lib/apt/lists/*

# ADD https://api.github.com/repos/borglab/gtsam/git/refs/heads/develop version.json
# RUN git clone https://github.com/borglab/gtsam.git && \
#     cd gtsam && \
#     git checkout 4.2 && \
#     mkdir build && \
#     cd build && \
#     cmake -DCMAKE_INSTALL_PREFIX=/usr/local \
#           -DGTSAM_BUILD_WITH_MARCH_NATIVE=OFF \
#           -DGTSAM_BUILD_TESTS=OFF \
#           -DGTSAM_BUILD_EXAMPLES_ALWAYS=OFF \
#           -DCMAKE_BUILD_TYPE=Release \
#           -DGTSAM_BUILD_UNSTABLE=ON \
#           -DGTSAM_POSE3_EXPMAP=ON \
#           -DGTSAM_ROT3_EXPMAP=ON \
#           -DGTSAM_TANGENT_PREINTEGRATION=OFF \
#           -DGTSAM_USE_SYSTEM_EIGEN=ON \
#           -DGTSAM_USE_SYSTEM_METIS=ON \
#           .. && \
#     make -j$(nproc) install

# # Install Open_GV
# RUN git clone https://github.com/marcusabate/opengv && \
#     cd opengv && \
#     git apply march_native_disable.patch && \
#     mkdir build && \
#     cd build && \
#     cmake -DCMAKE_BUILD_TYPE=Release \
#           -DCMAKE_INSTALL_PREFIX=/usr/local \
#           .. && \
#     make -j$(nproc) install

# # Install DBoW2
# COPY .patches/fix_vocab.patch dbow2.patch
# RUN git clone https://github.com/dorian3d/DBoW2.git
# RUN cd DBoW2 && \
#     git apply ../dbow2.patch && \
#     mkdir build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc) install

# # Install RobustPGO
# ADD https://api.github.com/repos/MIT-SPARK/Kimera-RPGO/git/refs/heads/master version.json
# RUN git clone https://github.com/MIT-SPARK/Kimera-RPGO.git
# RUN cd Kimera-RPGO && \
#     mkdir build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc) install

# # Install Kimera-VIO-Evaluation
# RUN python3 -m pip install --upgrade pip
# ADD https://api.github.com/repos/MIT-SPARK/Kimera-VIO-Evaluation/git/refs/heads/fix/python3 version.json
# RUN python3 -m pip install git+https://github.com/MIT-SPARK/Kimera-VIO-Evaluation.git@fix/python3