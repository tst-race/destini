# sri-race-plugins  --  RACE Plugins for SRI

Under 'source', you will find RACEBOAT plugins for JPEG steg
('source/encodings'), Flickr transport ('source/flickr_transport'),
and mjpeg_streamer transport ('source/mjpg_transport').

## Dependencies

The following apt-get dependencies are needed for building the SRI
plugins:

	apt-get -y install libthrift-dev
	apt-get -y install libjpeg62-dev
	apt-get -y install libjsoncpp-dev
	apt-get -y install libevent-dev
	apt-get -y install libboost-all-dev
	apt-get -y install libcpprest-dev
	apt-get -y install libxml2-dev
	apt-get -y install parallel
	apt-get -y install psutil
	apt-get -y install ffmpeg
	

You will need to create a softlink for libxml:

    pushd /usr/local/include
    ls /usr/include
    ln -s /usr/include/libxml2/libxml
    popd



Python dependencies are here:

       pip install pillow


You will also need to build and install the veil libraries and python
modules (see https://www.github.com/yegnev/veil).

## Build

To build the plugins, run the race-sdk docker container with /code mapped to
a directory containing both sri-race-plugins (this repo) and race-core
as siblings.  For example, suppose the parent directory is called
'/opt/projects/RACE/twosix'.  Start the container with the command
line switches:

	   "-v /opt/projects/RACE/twosix:/code -w /code"


Then we should see these in the container:

	   /code/sri-race-plugins
	   /code/race-core


Assuming that race-core has been built and installed in this
container, you can build the SRI plugins by invoking the build.sh
script:

	cd /code/sri-race-plugins
	bash build.sh
	bash install.sh

This should install the third-party flickcurl library, build and
install the SRI plugins, and install Flickr authentication
information.

## Staging in /etc/race

Shared objects and data files for the RaceBoat plugin will need to be
staged in the /etc/race directory within the docker container.  You
can use the "stage.sh" script to place shared libraries and
authentication files in the appropriate places.


## Testing

The race-cli utility can be used to test the plugins by sending and
receiving messages through Flickr via the "jelFlickr" channel.  The
send and recv addresses should contain a "user" key that can have a
value between "user1" and "user6" inclusive.

These user names are keys for accessing authentication information for
distinct users.  The master JSON file for authentication is in
"source/flickr_transport/flickr_auth/rw_auth.json".

To send a message:

   	race-cli  --send  --send-channel=jelFlickr --send-address='{"hashtag":"client-to-server","hostname":"twosix-whiteboard","port":5000,"user":"user1"}'

To receive the message (within 1/2 hour of sending):

   	race-cli  --recv  --recv-channel=jelFlickr --recv-address='{"hashtag":"client-to-server","hostname":"twosix-whiteboard","maxTries":120,"port":5000,"timestamp":0.0,"user":"user2"}' --num-packages=1

