#Led Snowboard

##Development

Development for this application is done using Eclipse 4.3 with the CDT.

The Github Flavored Markdown plugin is can be also installed, see here:

https://github.com/satyagraha/gfm_viewer

##Project structure and configuration

Due to the somewhat complicated Arduino/Teensyduino layout and the desire to NOT duplicate files
from the Teensyduino installation and commit them within this repository there are a few eclipse
projects - A workspace project, at the same directory hierarchy as this file, one project for each
executable project and one project for each library not included with Teensyduino.

Each executable project references the libraries using workspace relative locations.

In order for the Makefiles to work they need to know the location of the Teensyduino installation.  This
is achieved via a Project Setting - `C/C++ / Build / Environment` called `TEENSYDUINO_HOME`. 

On Windows that would likely be `C:\Program Files (x86)\Arduino-1.0.5`
On OSX that would likely be `/Applications/Arduino.app/Contents/Resources/Java`
 
Additionally each executable project needs a path variable called `TEENSYDUINO` which points to the same
location so that Eclipse can resolve the include files.

# Building

See the Readme.md files in each executable project for further details. 