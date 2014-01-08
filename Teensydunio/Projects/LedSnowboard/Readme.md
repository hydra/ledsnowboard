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

#Building on OSX

Install Arduino 1.0.5 to /Applications
Install Teensyduino >= 1.18-RC1 and use the same path.

From a terminal issue

    export TEENSYDUINO_HOME="/Applications/Arduino.app/Contents/Resources/Java"

    make clean
    make

