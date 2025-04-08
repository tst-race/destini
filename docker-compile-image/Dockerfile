# FROM ghcr.io/tst-race/raceboat/raceboat-compile:latest
FROM raceboat-compile:latest

RUN apt-get update && \
    apt-get install -y \
   	libthrift-dev \
	libjpeg62-dev \
	libjsoncpp-dev \
	libboost-all-dev \
	libcpprest-dev \
        libcurl4-openssl-dev \
	libevent-dev \
	libxml2-dev \
	parallel \
	ffmpeg


RUN    cd /usr/local/include && \
       ls /usr/include && \
       ln -s /usr/include/libxml2/libxml

RUN pip install \
        pillow \
	psutil

COPY ../3rd-party/flickcurl-1.26 /tmp/3rd-party/flickcurl-1.26
RUN cd /tmp/3rd-party/flickcurl-1.26 && \
    make install

# You will also need to build and install the veil libraries and python modules (see https://www.github.com/yegnev/veil).
