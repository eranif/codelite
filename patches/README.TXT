This file contains 4 files that should be applied locally to your wxWidgets sources before building.

Copy the following files:

1. PlatWX.cpp
2. stc.cpp
3. ScintillaWX.cpp

copy the above files to WX-SOURCES-PATH/src/stc/

Next, copy the file:
cp editors.cpp WX-SOURCES-PATH/src/propgrid

build wxWidgets normally

