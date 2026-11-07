#!/bin/sh

zip ../wxgui.zip *.png *.wxcp *.cpp *.h LICENSE
# The SVG icons of the dark and the light themes (bitmaps/dark/<name>.svg, bitmaps/light/<name>.svg)
(cd ../assets && zip ../wxgui.zip bitmaps/dark/*.svg bitmaps/light/*.svg)
