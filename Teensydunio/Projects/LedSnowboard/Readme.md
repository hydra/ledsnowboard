#Notes


#Building

Currently the supported build environments are Cygwin and OSX.  Your mileage on linux may vary.

There is a makefile in $(TEENSYDUINO_HOME)/hardware/teensy/cores/teensy3 which was used as the starting point.  Make sur
e you can build that before reporting Makefile issues.


#Building on Cygwin

Install Arduino 1.0.5 to "D:\Program Files (x86)\Teensy\arduino-1.0.5"
Install Teensyduino >= 1.18-RC1 and use the same path
Install Cygwin

From a cygwin terminal

    export TEENSYDUINO_HOME="/cygdrive/d/progra~1/Teensy/arduino-1.0.5"
    export CYGPATH=cygpath

    make clean
    make

#Building on Cygwin within Eclipse

Eclipse CDT with GCC make and Cygwin make 4.0.0 don't play nice.

To fix the problem with 'multiple target pattern' error messages do this:

define a system environment variable for TEENSYDUINO_HOME to use forward slashes and short paths, e.g.

	SET TEENSYDUINO_HOME=D:/Progra~1/Teensy/arduino-1.0.5

Backup old make and install fixed make

See http://sites.rwalker.com/cygwin/

From cygwin:

	cd
	wget http://sites.rwalker.com/cygwin/make-381-DOS/make-3.82-1.tar.bz2?attredirects=0&d=1 
  	mkdir make
    cd make/
    bzcat ../make-3.82-1.tar.bz2 | tar xvf -
    mv /usr/bin/make.exe /usr/bin/make-linuxonly.exe
    cp usr/bin/make.exe /usr/bin/
    make -v

Once that's done you can launch Eclipse and run the make targets.


#Building on OSX

Install Arduino 1.0.5 to /Applications
Install Teensyduino >= 1.18-RC1 and use the same path.

From a terminal issue

    export TEENSYDUINO_HOME="/Applications/Arduino.app/Contents/Resources/Java"

    make clean
    make

