#!/bin/sh

zip ../wxgui.zip *.wxcp *.cpp *.h LICENSE
# The SVG icons of the dark and the light themes: bitmaps/dark/<name>.svg and bitmaps/light/<name>.svg
(cd ../assets && zip ../wxgui.zip bitmaps/dark/*.svg bitmaps/light/*.svg)
