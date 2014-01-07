#Notes

Install Arduino 1.0.5 to "D:\Program Files (x86)\Teensy\arduino-1.0.5"
Install Teensyduino and use the same path.
Install Cygwin

#Building

Currently the only supported build environment is Cygwin.  Your milage on OSX/Linux may vary but the only thing you need
 to fix should be the use og cygpath in the Makefile.

There is a makefile in $(TEENSYDUINO_HOME)/hardware/teensy/cores/teensy3 which was used as the starting point.  Make sur
e you can build that before reporting Makefile issues.


#Building on Cygwin

    export TEENSYDUINO_HOME="/cygdrive/d/progra~1/Teensy/arduino-1.0.5"
    export CYGPATH=cygpath

    make clean
    make

