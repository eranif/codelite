<?php

// Start of gd v.

/**
 * Retrieve information about the currently installed GD library
 * @link http://www.php.net/manual/en/function.gd-info.php
 * @return array an associative array.
 *  </p>
 *  <p>
 *   <table>
 *    Elements of array returned by gd_info
 *    
 *     
 *      <tr valign="top">
 *       <td>Attribute</td>
 *       <td>Meaning</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>GD Version</td>
 *       <td>string value describing the installed
 *        libgd version.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Freetype Support</td>
 *       <td>boolean value.  true
 *        if Freetype Support is installed.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Freetype Linkage</td>
 *       <td>string value describing the way in which
 *        Freetype was linked. Expected values are: 'with freetype',
 *        'with TTF library', and 'with unknown library'.  This element will
 *        only be defined if Freetype Support evaluated to
 *        true.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>T1Lib Support</td>
 *       <td>boolean value.  true
 *        if T1Lib support is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>GIF Read Support</td>
 *       <td>boolean value.  true
 *        if support for reading GIF
 *        images is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>GIF Create Support</td>
 *       <td>boolean value.  true
 *        if support for creating GIF
 *        images is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>JPEG Support</td>
 *       <td>boolean value.  true
 *        if JPEG support is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>PNG Support</td>
 *       <td>boolean value.  true
 *        if PNG support is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>WBMP Support</td>
 *       <td>boolean value.  true
 *        if WBMP support is included.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>XBM Support</td>
 *       <td>boolean value.  true
 *        if XBM support is included.</td>
 *      </tr>
 *     
 *    
 *   </table>
 *  </p>
 *  
 *   <p>
 *    Previous to PHP 5.3.0, the JPEG Support attribute was named
 *    JPG Support.
 */
function gd_info () {}

/**
 * Draws an arc
 * @link http://www.php.net/manual/en/function.imagearc.php
 * @param image resource 
 * @param cx int <p>
 *       x-coordinate of the center.
 *      </p>
 * @param cy int <p>
 *       y-coordinate of the center.
 *      </p>
 * @param width int <p>
 *       The arc width.
 *      </p>
 * @param height int <p>
 *       The arc height.
 *      </p>
 * @param start int <p>
 *       The arc start angle, in degrees.
 *      </p>
 * @param end int <p>
 *       The arc end angle, in degrees.
 *       0° is located at the three-o'clock position, and the arc is drawn
 *       clockwise.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagearc ($image, $cx, $cy, $width, $height, $start, $end, $color) {}

/**
 * Draw an ellipse
 * @link http://www.php.net/manual/en/function.imageellipse.php
 * @param image resource 
 * @param cx int <p>
 *       x-coordinate of the center.
 *      </p>
 * @param cy int <p>
 *       y-coordinate of the center.
 *      </p>
 * @param width int <p>
 *       The ellipse width.
 *      </p>
 * @param height int <p>
 *       The ellipse height.
 *      </p>
 * @param color int <p>
 *       The color of the ellipse. A color identifier created with
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imageellipse ($image, $cx, $cy, $width, $height, $color) {}

/**
 * Draw a character horizontally
 * @link http://www.php.net/manual/en/function.imagechar.php
 * @param image resource 
 * @param font int 
 * @param x int <p>
 *       x-coordinate of the start.
 *      </p>
 * @param y int <p>
 *       y-coordinate of the start.
 *      </p>
 * @param c string <p>
 *       The character to draw.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagechar ($image, $font, $x, $y, $c, $color) {}

/**
 * Draw a character vertically
 * @link http://www.php.net/manual/en/function.imagecharup.php
 * @param image resource 
 * @param font int 
 * @param x int <p>
 *       x-coordinate of the start.
 *      </p>
 * @param y int <p>
 *       y-coordinate of the start.
 *      </p>
 * @param c string <p>
 *       The character to draw.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagecharup ($image, $font, $x, $y, $c, $color) {}

/**
 * Get the index of the color of a pixel
 * @link http://www.php.net/manual/en/function.imagecolorat.php
 * @param image resource 
 * @param x int <p>
 *       x-coordinate of the point.
 *      </p>
 * @param y int <p>
 *       y-coordinate of the point.
 *      </p>
 * @return int the index of the color.
 */
function imagecolorat ($image, $x, $y) {}

/**
 * Allocate a color for an image
 * @link http://www.php.net/manual/en/function.imagecolorallocate.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @return int A color identifier or false if the allocation failed.
 */
function imagecolorallocate ($image, $red, $green, $blue) {}

/**
 * Copy the palette from one image to another
 * @link http://www.php.net/manual/en/function.imagepalettecopy.php
 * @param destination resource <p>
 *       The destination image resource.
 *      </p>
 * @param source resource <p>
 *       The source image resource.
 *      </p>
 * @return void 
 */
function imagepalettecopy ($destination, $source) {}

/**
 * Create a new image from the image stream in the string
 * @link http://www.php.net/manual/en/function.imagecreatefromstring.php
 * @param data string 
 * @return resource An image resource will be returned on success. false is returned if
 *   the image type is unsupported, the data is not in a recognised format,
 *   or the image is corrupt and cannot be loaded.
 */
function imagecreatefromstring ($data) {}

/**
 * Get the index of the closest color to the specified color
 * @link http://www.php.net/manual/en/function.imagecolorclosest.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @return int the index of the closest color, in the palette of the image, to
 *   the specified one
 */
function imagecolorclosest ($image, $red, $green, $blue) {}

/**
 * Get the index of the color which has the hue, white and blackness
 * @link http://www.php.net/manual/en/function.imagecolorclosesthwb.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @return int an integer with the index of the color which has 
 *   the hue, white and blackness nearest the given color.
 */
function imagecolorclosesthwb ($image, $red, $green, $blue) {}

/**
 * De-allocate a color for an image
 * @link http://www.php.net/manual/en/function.imagecolordeallocate.php
 * @param image resource 
 * @param color int <p>
 *       The color identifier.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagecolordeallocate ($image, $color) {}

/**
 * Get the index of the specified color or its closest possible alternative
 * @link http://www.php.net/manual/en/function.imagecolorresolve.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @return int a color index.
 */
function imagecolorresolve ($image, $red, $green, $blue) {}

/**
 * Get the index of the specified color
 * @link http://www.php.net/manual/en/function.imagecolorexact.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @return int the index of the specified color in the palette, or -1 if the
 *   color does not exist.
 */
function imagecolorexact ($image, $red, $green, $blue) {}

/**
 * Set the color for the specified palette index
 * @link http://www.php.net/manual/en/function.imagecolorset.php
 * @param image resource 
 * @param index int <p>
 *       An index in the palette.
 *      </p>
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @param alpha int[optional] <p>
 *       Value of alpha component.
 *      </p>
 * @return void 
 */
function imagecolorset ($image, $index, $red, $green, $blue, $alpha = null) {}

/**
 * Define a color as transparent
 * @link http://www.php.net/manual/en/function.imagecolortransparent.php
 * @param image resource 
 * @param color int[optional] <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return int The identifier of the new (or current, if none is specified)
 *   transparent color is returned.
 */
function imagecolortransparent ($image, $color = null) {}

/**
 * Find out the number of colors in an image's palette
 * @link http://www.php.net/manual/en/function.imagecolorstotal.php
 * @param image resource <p>
 *       An image resource, returned by one of the image creation functions, such
 *       as imagecreatefromgif.
 *      </p>
 * @return int the number of colors in the specified image's palette or 0 for
 *   truecolor images.
 */
function imagecolorstotal ($image) {}

/**
 * Get the colors for an index
 * @link http://www.php.net/manual/en/function.imagecolorsforindex.php
 * @param image resource 
 * @param index int <p>
 *       The color index.
 *      </p>
 * @return array an associative array with red, green, blue and alpha keys that
 *   contain the appropriate values for the specified color index.
 */
function imagecolorsforindex ($image, $index) {}

/**
 * Copy part of an image
 * @link http://www.php.net/manual/en/function.imagecopy.php
 * @param dst_im resource <p>&gd.image.destination;</p>
 * @param src_im resource <p>&gd.image.source;</p>
 * @param dst_x int <p>
 *       x-coordinate of destination point.
 *      </p>
 * @param dst_y int <p>
 *       y-coordinate of destination point.
 *      </p>
 * @param src_x int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param src_y int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param src_w int <p>&gd.source.width;</p>
 * @param src_h int <p>&gd.source.height;</p>
 * @return bool Returns true on success or false on failure.
 */
function imagecopy ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h) {}

/**
 * Copy and merge part of an image
 * @link http://www.php.net/manual/en/function.imagecopymerge.php
 * @param dst_im resource <p>&gd.image.destination;</p>
 * @param src_im resource <p>&gd.image.source;</p>
 * @param dst_x int <p>
 *       x-coordinate of destination point.
 *      </p>
 * @param dst_y int <p>
 *       y-coordinate of destination point.
 *      </p>
 * @param src_x int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param src_y int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param src_w int <p>&gd.source.width;</p>
 * @param src_h int <p>&gd.source.height;</p>
 * @param pct int <p>
 *       The two images will be merged according to pct
 *       which can range from 0 to 100.  When pct = 0,
 *       no action is taken, when 100 this function behaves identically
 *       to imagecopy for pallete images, while it
 *       implements alpha transparency for true colour images.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagecopymerge ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h, $pct) {}

/**
 * Copy and merge part of an image with gray scale
 * @link http://www.php.net/manual/en/function.imagecopymergegray.php
 * @param dst_im resource <p>&gd.image.destination;</p>
 * @param src_im resource <p>&gd.image.source;</p>
 * @param dst_x int <p>
 *       x-coordinate of destination point.
 *      </p>
 * @param dst_y int <p>
 *       y-coordinate of destination point.
 *      </p>
 * @param src_x int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param src_y int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param src_w int <p>&gd.source.width;</p>
 * @param src_h int <p>&gd.source.height;</p>
 * @param pct int <p>
 *        The src_im will be changed to grayscale according 
 *        to pct where 0 is fully grayscale and 100 is 
 *        unchanged. When pct = 100 this function behaves 
 *        identically to imagecopy for pallete images, while 
 *        it implements alpha transparency for true colour images.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagecopymergegray ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h, $pct) {}

/**
 * Copy and resize part of an image
 * @link http://www.php.net/manual/en/function.imagecopyresized.php
 * @param dst_image resource <p>&gd.image.destination;</p>
 * @param src_image resource <p>&gd.image.source;</p>
 * @param dst_x int <p>
 *       x-coordinate of destination point.
 *      </p>
 * @param dst_y int <p>
 *       y-coordinate of destination point.
 *      </p>
 * @param src_x int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param src_y int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param dst_w int <p>
 *       Destination width.
 *      </p>
 * @param dst_h int <p>
 *       Destination height.
 *      </p>
 * @param src_w int <p>&gd.source.width;</p>
 * @param src_h int <p>&gd.source.height;</p>
 * @return bool Returns true on success or false on failure.
 */
function imagecopyresized ($dst_image, $src_image, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h) {}

/**
 * Create a new palette based image
 * @link http://www.php.net/manual/en/function.imagecreate.php
 * @param width int <p>
 *       The image width.
 *      </p>
 * @param height int <p>
 *       The image height.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreate ($width, $height) {}

/**
 * Create a new true color image
 * @link http://www.php.net/manual/en/function.imagecreatetruecolor.php
 * @param width int <p>
 *       Image width.
 *      </p>
 * @param height int <p>
 *       Image height.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatetruecolor ($width, $height) {}

/**
 * Finds whether an image is a truecolor image
 * @link http://www.php.net/manual/en/function.imageistruecolor.php
 * @param image resource 
 * @return bool true if the image is truecolor, false
 *   otherwise.
 */
function imageistruecolor ($image) {}

/**
 * Convert a true color image to a palette image
 * @link http://www.php.net/manual/en/function.imagetruecolortopalette.php
 * @param image resource 
 * @param dither bool <p>
 *       Indicates if the image should be dithered - if it is true then
 *       dithering will be used which will result in a more speckled image but
 *       with better color approximation.
 *      </p>
 * @param ncolors int <p>
 *       Sets the maximum number of colors that should be retained in the palette.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagetruecolortopalette ($image, $dither, $ncolors) {}

/**
 * Set the thickness for line drawing
 * @link http://www.php.net/manual/en/function.imagesetthickness.php
 * @param image resource 
 * @param thickness int <p>
 *       Thickness, in pixels.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesetthickness ($image, $thickness) {}

/**
 * Draw a partial arc and fill it
 * @link http://www.php.net/manual/en/function.imagefilledarc.php
 * @param image resource 
 * @param cx int <p>
 *       x-coordinate of the center.
 *      </p>
 * @param cy int <p>
 *       y-coordinate of the center.
 *      </p>
 * @param width int <p>
 *       The arc width.
 *      </p>
 * @param height int <p>
 *       The arc height.
 *      </p>
 * @param start int <p>
 *       The arc start angle, in degrees.
 *      </p>
 * @param end int <p>
 *       The arc end angle, in degrees.
 *       0° is located at the three-o'clock position, and the arc is drawn
 *       clockwise.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @param style int <p>
 *       A bitwise OR of the following possibilities:
 *       
 *        IMG_ARC_PIE
 * @return bool Returns true on success or false on failure.
 */
function imagefilledarc ($image, $cx, $cy, $width, $height, $start, $end, $color, $style) {}

/**
 * Draw a filled ellipse
 * @link http://www.php.net/manual/en/function.imagefilledellipse.php
 * @param image resource 
 * @param cx int <p>
 *       x-coordinate of the center.
 *      </p>
 * @param cy int <p>
 *       y-coordinate of the center.
 *      </p>
 * @param width int <p>
 *       The ellipse width.
 *      </p>
 * @param height int <p>
 *       The ellipse height.
 *      </p>
 * @param color int <p>
 *       The fill color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagefilledellipse ($image, $cx, $cy, $width, $height, $color) {}

/**
 * Set the blending mode for an image
 * @link http://www.php.net/manual/en/function.imagealphablending.php
 * @param image resource 
 * @param blendmode bool <p>
 *       Whether to enable the blending mode or not. On true color images 
 *       the default value is true otherwise the default value is false
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagealphablending ($image, $blendmode) {}

/**
 * Set the flag to save full alpha channel information (as opposed to single-color transparency) when saving PNG images
 * @link http://www.php.net/manual/en/function.imagesavealpha.php
 * @param image resource 
 * @param saveflag bool <p>
 *       Whether to save the alpha channel or not. Default to false. 
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesavealpha ($image, $saveflag) {}

/**
 * Allocate a color for an image
 * @link http://www.php.net/manual/en/function.imagecolorallocatealpha.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @param alpha int <p>
 *       A value between 0 and 127.
 *       0 indicates completely opaque while 
 *       127 indicates completely transparent.
 *      </p>
 * @return int A color identifier or false if the allocation failed.
 */
function imagecolorallocatealpha ($image, $red, $green, $blue, $alpha) {}

/**
 * Get the index of the specified color + alpha or its closest possible alternative
 * @link http://www.php.net/manual/en/function.imagecolorresolvealpha.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @param alpha int <p>
 *       A value between 0 and 127.
 *       0 indicates completely opaque while 
 *       127 indicates completely transparent.
 *      </p>
 * @return int a color index.
 */
function imagecolorresolvealpha ($image, $red, $green, $blue, $alpha) {}

/**
 * Get the index of the closest color to the specified color + alpha
 * @link http://www.php.net/manual/en/function.imagecolorclosestalpha.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @param alpha int <p>
 *       A value between 0 and 127.
 *       0 indicates completely opaque while 
 *       127 indicates completely transparent.
 *      </p>
 * @return int the index of the closest color in the palette.
 */
function imagecolorclosestalpha ($image, $red, $green, $blue, $alpha) {}

/**
 * Get the index of the specified color + alpha
 * @link http://www.php.net/manual/en/function.imagecolorexactalpha.php
 * @param image resource 
 * @param red int <p>&gd.value.red;</p>
 * @param green int <p>&gd.value.green;</p>
 * @param blue int <p>&gd.value.blue;</p>
 * @param alpha int <p>
 *       A value between 0 and 127.
 *       0 indicates completely opaque while 
 *       127 indicates completely transparent.
 *      </p>
 * @return int the index of the specified color+alpha in the palette of the
 *   image, or -1 if the color does not exist in the image's palette.
 */
function imagecolorexactalpha ($image, $red, $green, $blue, $alpha) {}

/**
 * Copy and resize part of an image with resampling
 * @link http://www.php.net/manual/en/function.imagecopyresampled.php
 * @param dst_image resource <p>&gd.image.destination;</p>
 * @param src_image resource <p>&gd.image.source;</p>
 * @param dst_x int <p>
 *       x-coordinate of destination point.
 *      </p>
 * @param dst_y int <p>
 *       y-coordinate of destination point.
 *      </p>
 * @param src_x int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param src_y int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param dst_w int <p>
 *       Destination width.
 *      </p>
 * @param dst_h int <p>
 *       Destination height.
 *      </p>
 * @param src_w int <p>&gd.source.width;</p>
 * @param src_h int <p>&gd.source.height;</p>
 * @return bool Returns true on success or false on failure.
 */
function imagecopyresampled ($dst_image, $src_image, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h) {}

/**
 * Rotate an image with a given angle
 * @link http://www.php.net/manual/en/function.imagerotate.php
 * @param image resource 
 * @param angle float <p>
 *       Rotation angle, in degrees. The rotation angle is interpreted as the
 *       number of degrees to rotate the image anticlockwise.
 *      </p>
 * @param bgd_color int <p>
 *       Specifies the color of the uncovered zone after the rotation
 *      </p>
 * @param ignore_transparent int[optional] <p>
 *       If set and non-zero, transparent colors are ignored (otherwise kept).
 *      </p>
 * @return resource an image resource for the rotated image, &return.falseforfailure;.
 */
function imagerotate ($image, $angle, $bgd_color, $ignore_transparent = null) {}

/**
 * Should antialias functions be used or not
 * @link http://www.php.net/manual/en/function.imageantialias.php
 * @param image resource 
 * @param enabled bool <p>
 *       Whether to enable antialiasing or not.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imageantialias ($image, $enabled) {}

/**
 * Set the tile image for filling
 * @link http://www.php.net/manual/en/function.imagesettile.php
 * @param image resource 
 * @param tile resource <p>
 *       The image resource to be used as a tile.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesettile ($image, $tile) {}

/**
 * Set the brush image for line drawing
 * @link http://www.php.net/manual/en/function.imagesetbrush.php
 * @param image resource 
 * @param brush resource <p>
 *       An image resource.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesetbrush ($image, $brush) {}

/**
 * Set the style for line drawing
 * @link http://www.php.net/manual/en/function.imagesetstyle.php
 * @param image resource 
 * @param style array <p>
 *       An array of pixel colors. You can use the 
 *       IMG_COLOR_TRANSPARENT constant to add a 
 *       transparent pixel.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesetstyle ($image, array $style) {}

/**
 * &gd.image.new;
 * @link http://www.php.net/manual/en/function.imagecreatefrompng.php
 * @param filename string <p>
 *       Path to the PNG image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefrompng ($filename) {}

/**
 * &gd.image.new;
 * @link http://www.php.net/manual/en/function.imagecreatefromgif.php
 * @param filename string <p>
 *       Path to the GIF image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromgif ($filename) {}

/**
 * &gd.image.new;
 * @link http://www.php.net/manual/en/function.imagecreatefromjpeg.php
 * @param filename string <p>
 *       Path to the JPEG image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromjpeg ($filename) {}

/**
 * &gd.image.new;
 * @link http://www.php.net/manual/en/function.imagecreatefromwbmp.php
 * @param filename string <p>
 *       Path to the WBMP image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromwbmp ($filename) {}

/**
 * &gd.image.new;
 * @link http://www.php.net/manual/en/function.imagecreatefromxbm.php
 * @param filename string <p>
 *       Path to the XBM image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromxbm ($filename) {}

/**
 * Create a new image from GD file or URL
 * @link http://www.php.net/manual/en/function.imagecreatefromgd.php
 * @param filename string <p>
 *       Path to the GD file.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromgd ($filename) {}

/**
 * Create a new image from GD2 file or URL
 * @link http://www.php.net/manual/en/function.imagecreatefromgd2.php
 * @param filename string <p>
 *       Path to the GD2 image.
 *      </p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromgd2 ($filename) {}

/**
 * Create a new image from a given part of GD2 file or URL
 * @link http://www.php.net/manual/en/function.imagecreatefromgd2part.php
 * @param filename string <p>
 *       Path to the GD2 image.
 *      </p>
 * @param srcX int <p>
 *       x-coordinate of source point.
 *      </p>
 * @param srcY int <p>
 *       y-coordinate of source point.
 *      </p>
 * @param width int <p>&gd.source.width;</p>
 * @param height int <p>&gd.source.height;</p>
 * @return resource &gd.return.identifier;
 */
function imagecreatefromgd2part ($filename, $srcX, $srcY, $width, $height) {}

/**
 * Output a PNG image to either the browser or a file
 * @link http://www.php.net/manual/en/function.imagepng.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 *      
 *       <p>
 *        &null; is invalid if the quality and
 *        filters arguments are not used.
 *       </p>
 * @param quality int[optional] <p>
 *       Compression level: from 0 (no compression) to 9.
 *      </p>
 * @param filters int[optional] <p>
 *       Allows reducing the PNG file size. It is a bitmask field which may be
 *       set to any combination of the PNG_FILTER_XXX 
 *       constants. PNG_NO_FILTER or 
 *       PNG_ALL_FILTERS may also be used to respectively
 *       disable or activate all filters.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagepng ($image, $filename = null, $quality = null, $filters = null) {}

/**
 * &gd.image.output;
 * @link http://www.php.net/manual/en/function.imagegif.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 * @return bool Returns true on success or false on failure.
 */
function imagegif ($image, $filename = null) {}

/**
 * &gd.image.output;
 * @link http://www.php.net/manual/en/function.imagejpeg.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 *      <p>
 *       To skip this argument in order to provide the 
 *       quality parameter, use &null;.
 *      </p>
 * @param quality int[optional] <p>
 *       quality is optional, and ranges from 0 (worst
 *       quality, smaller file) to 100 (best quality, biggest file). The 
 *       default is the default IJG quality value (about 75).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagejpeg ($image, $filename = null, $quality = null) {}

/**
 * &gd.image.output;
 * @link http://www.php.net/manual/en/function.imagewbmp.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 * @param foreground int[optional] <p>
 *       You can set the foreground color with this parameter by setting an
 *       identifier obtained from imagecolorallocate.
 *       The default foreground color is black.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagewbmp ($image, $filename = null, $foreground = null) {}

/**
 * Output GD image to browser or file
 * @link http://www.php.net/manual/en/function.imagegd.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 * @return bool Returns true on success or false on failure.
 */
function imagegd ($image, $filename = null) {}

/**
 * Output GD2 image to browser or file
 * @link http://www.php.net/manual/en/function.imagegd2.php
 * @param image resource 
 * @param filename string[optional] <p>&gd.image.path;</p>
 * @param chunk_size int[optional] <p>
 *       Chunk size.
 *      </p>
 * @param type int[optional] <p>
 *       Either IMG_GD2_RAW or 
 *       IMG_GD2_COMPRESSED. Default is 
 *       IMG_GD2_RAW.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagegd2 ($image, $filename = null, $chunk_size = null, $type = null) {}

/**
 * Destroy an image
 * @link http://www.php.net/manual/en/function.imagedestroy.php
 * @param image resource 
 * @return bool Returns true on success or false on failure.
 */
function imagedestroy ($image) {}

/**
 * Apply a gamma correction to a GD image
 * @link http://www.php.net/manual/en/function.imagegammacorrect.php
 * @param image resource 
 * @param inputgamma float <p>
 *       The input gamma.
 *      </p>
 * @param outputgamma float <p>
 *       The output gamma.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagegammacorrect ($image, $inputgamma, $outputgamma) {}

/**
 * Flood fill
 * @link http://www.php.net/manual/en/function.imagefill.php
 * @param image resource 
 * @param x int <p>
 *       x-coordinate of start point.
 *      </p>
 * @param y int <p>
 *       y-coordinate of start point.
 *      </p>
 * @param color int <p>
 *       The fill color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagefill ($image, $x, $y, $color) {}

/**
 * Draw a filled polygon
 * @link http://www.php.net/manual/en/function.imagefilledpolygon.php
 * @param image resource 
 * @param points array <p>
 *       An array containing the x and y
 *       coordinates of the polygons vertices consecutively.
 *      </p>
 * @param num_points int <p>
 *       Total number of vertices, which must be at least 3.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagefilledpolygon ($image, array $points, $num_points, $color) {}

/**
 * Draw a filled rectangle
 * @link http://www.php.net/manual/en/function.imagefilledrectangle.php
 * @param image resource 
 * @param x1 int <p>
 *       x-coordinate for point 1.
 *      </p>
 * @param y1 int <p>
 *       y-coordinate for point 1.
 *      </p>
 * @param x2 int <p>
 *       x-coordinate for point 2.
 *      </p>
 * @param y2 int <p>
 *       y-coordinate for point 2.
 *      </p>
 * @param color int <p>
 *       The fill color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagefilledrectangle ($image, $x1, $y1, $x2, $y2, $color) {}

/**
 * Flood fill to specific color
 * @link http://www.php.net/manual/en/function.imagefilltoborder.php
 * @param image resource 
 * @param x int <p>
 *       x-coordinate of start.
 *      </p>
 * @param y int <p>
 *       y-coordinate of start.
 *      </p>
 * @param border int <p>
 *       The border color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @param color int <p>
 *       The fill color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagefilltoborder ($image, $x, $y, $border, $color) {}

/**
 * Get font width
 * @link http://www.php.net/manual/en/function.imagefontwidth.php
 * @param font int 
 * @return int the width of the pixel
 */
function imagefontwidth ($font) {}

/**
 * Get font height
 * @link http://www.php.net/manual/en/function.imagefontheight.php
 * @param font int 
 * @return int the height of the pixel.
 */
function imagefontheight ($font) {}

/**
 * Enable or disable interlace
 * @link http://www.php.net/manual/en/function.imageinterlace.php
 * @param image resource 
 * @param interlace int[optional] <p>
 *       If non-zero, the image will be interlaced, else the interlace bit is
 *       turned off.
 *      </p>
 * @return int 1 if the interlace bit is set for the image, 0 otherwise.
 */
function imageinterlace ($image, $interlace = null) {}

/**
 * Draw a line
 * @link http://www.php.net/manual/en/function.imageline.php
 * @param image resource 
 * @param x1 int <p>
 *       x-coordinate for first point.
 *      </p>
 * @param y1 int <p>
 *       y-coordinate for first point.
 *      </p>
 * @param x2 int <p>
 *       x-coordinate for second point.
 *      </p>
 * @param y2 int <p>
 *       y-coordinate for second point.
 *      </p>
 * @param color int <p>
 *       The line color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imageline ($image, $x1, $y1, $x2, $y2, $color) {}

/**
 * Load a new font
 * @link http://www.php.net/manual/en/function.imageloadfont.php
 * @param file string <p>
 *       The font file format is currently binary and architecture
 *       dependent.  This means you should generate the font files on the
 *       same type of CPU as the machine you are running PHP on.
 *      </p>
 *      <p>
 *       <table>
 *        Font file format
 *        
 *         
 *         <tr valign="top">
 *          <td>byte position</td>
 *          <td>C data type</td>
 *          <td>description</td>
 *         </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>byte 0-3</td>
 *           <td>int</td>
 *           <td>number of characters in the font</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>byte 4-7</td>
 *           <td>int</td>
 *           <td>
 *            value of first character in the font (often 32 for space)
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>byte 8-11</td>
 *           <td>int</td>
 *           <td>pixel width of each character</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>byte 12-15</td>
 *           <td>int</td>
 *           <td>pixel height of each character</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>byte 16-</td>
 *           <td>char</td>
 *           <td>
 *            array with character data, one byte per pixel in each
 *            character, for a total of (nchars*width*height) bytes.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @return int The font identifier which is always bigger than 5 to avoid conflicts with
 *   built-in fonts or false on errors.
 */
function imageloadfont ($file) {}

/**
 * Draws a polygon
 * @link http://www.php.net/manual/en/function.imagepolygon.php
 * @param image resource 
 * @param points array <p>
 *       An array containing the polygon's vertices, e.g.:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>points[0]</td>
 *           <td>= x0</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>points[1]</td>
 *           <td>= y0</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>points[2]</td>
 *           <td>= x1</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>points[3]</td>
 *           <td>= y1</td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 * @param num_points int <p>
 *       Total number of points (vertices).
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagepolygon ($image, array $points, $num_points, $color) {}

/**
 * Draw a rectangle
 * @link http://www.php.net/manual/en/function.imagerectangle.php
 * @param image resource 
 * @param x1 int <p>
 *       Upper left x coordinate.
 *      </p>
 * @param y1 int <p>
 *       Upper left y coordinate
 *       0, 0 is the top left corner of the image.
 *      </p>
 * @param x2 int <p>
 *       Bottom right x coordinate.
 *      </p>
 * @param y2 int <p>
 *       Bottom right y coordinate.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagerectangle ($image, $x1, $y1, $x2, $y2, $color) {}

/**
 * Set a single pixel
 * @link http://www.php.net/manual/en/function.imagesetpixel.php
 * @param image resource 
 * @param x int <p>
 *       x-coordinate.
 *      </p>
 * @param y int <p>
 *       y-coordinate.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagesetpixel ($image, $x, $y, $color) {}

/**
 * Draw a string horizontally
 * @link http://www.php.net/manual/en/function.imagestring.php
 * @param image resource 
 * @param font int 
 * @param x int <p>
 *       x-coordinate of the upper left corner.
 *      </p>
 * @param y int <p>
 *       y-coordinate of the upper left corner.
 *      </p>
 * @param string string <p>
 *       The string to be written.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagestring ($image, $font, $x, $y, $string, $color) {}

/**
 * Draw a string vertically
 * @link http://www.php.net/manual/en/function.imagestringup.php
 * @param image resource 
 * @param font int 
 * @param x int <p>
 *       x-coordinate of the bottom left corner.
 *      </p>
 * @param y int <p>
 *       y-coordinate of the bottom left corner.
 *      </p>
 * @param string string <p>
 *       The string to be written.
 *      </p>
 * @param color int <p>
 *       A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagestringup ($image, $font, $x, $y, $string, $color) {}

/**
 * Get image width
 * @link http://www.php.net/manual/en/function.imagesx.php
 * @param image resource 
 * @return int Return the width of the image or false on 
 *   errors.
 */
function imagesx ($image) {}

/**
 * Get image height
 * @link http://www.php.net/manual/en/function.imagesy.php
 * @param image resource 
 * @return int Return the height of the image or false on 
 *   errors.
 */
function imagesy ($image) {}

/**
 * Draw a dashed line
 * @link http://www.php.net/manual/en/function.imagedashedline.php
 * @param image resource 
 * @param x1 int <p>
 *       Upper left x coordinate.
 *      </p>
 * @param y1 int <p>
 *       Upper left y coordinate 0, 0 is the top left corner of the image.
 *      </p>
 * @param x2 int <p>
 *       Bottom right x coordinate.
 *      </p>
 * @param y2 int <p>
 *       Bottom right y coordinate.
 *      </p>
 * @param color int <p>
 *       The fill color. A color identifier created with 
 *       imagecolorallocate.
 *      </p>
 * @return bool Always returns true
 */
function imagedashedline ($image, $x1, $y1, $x2, $y2, $color) {}

/**
 * Give the bounding box of a text using TrueType fonts
 * @link http://www.php.net/manual/en/function.imagettfbbox.php
 * @param size float <p>
 *       The font size.
 *       
 *        
 *         In GD 1, this is measured in pixels. In GD 2, this is measured in
 *         points.
 *        
 *       
 *      </p>
 * @param angle float <p>
 *       Angle in degrees in which text will be measured.
 *      </p>
 * @param fontfile string <p>
 *       The name of the TrueType font file (can be a URL). Depending on
 *       which version of the GD library that PHP is using, it may attempt to
 *       search for files that do not begin with a leading '/' by appending
 *       '.ttf' to the filename and searching along a library-defined font path.
 *      </p>
 * @param text string <p>
 *       The string to be measured.
 *      </p>
 * @return array imagettfbbox returns an array with 8
 *   elements representing four points making the bounding box of the
 *   text on success and false on error.
 *   
 *    
 *     
 *      <tr valign="top">
 *       <td>key</td>
 *       <td>contents</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>0</td>
 *       <td>lower left corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>1</td>
 *       <td>lower left corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>2</td>
 *       <td>lower right corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>3</td>
 *       <td>lower right corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>4</td>
 *       <td>upper right corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>5</td>
 *       <td>upper right corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>6</td>
 *       <td>upper left corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>7</td>
 *       <td>upper left corner, Y position</td>
 *      </tr>
 *     
 *    
 *   
 *  </p>
 *  <p>
 *   The points are relative to the text regardless of the
 *   angle, so "upper left" means in the top left-hand 
 *   corner seeing the text horizontally.
 */
function imagettfbbox ($size, $angle, $fontfile, $text) {}

/**
 * Write text to the image using TrueType fonts
 * @link http://www.php.net/manual/en/function.imagettftext.php
 * @param image resource 
 * @param size float <p>&gd.font.size;</p>
 * @param angle float <p>
 *       The angle in degrees, with 0 degrees being left-to-right reading text.
 *       Higher values represent a counter-clockwise rotation. For example, a 
 *       value of 90 would result in bottom-to-top reading text.
 *      </p>
 * @param x int <p>
 *       The coordinates given by x and
 *       y will define the basepoint of the first
 *       character (roughly the lower-left corner of the character). This
 *       is different from the imagestring, where
 *       x and y define the
 *       upper-left corner of the first character. For example, "top left"
 *       is 0, 0.
 *      </p>
 * @param y int <p>
 *       The y-ordinate. This sets the position of the fonts baseline, not the
 *       very bottom of the character.
 *      </p>
 * @param color int <p>
 *       The color index. Using the negative of a color index has the effect of
 *       turning off antialiasing. See imagecolorallocate.
 *      </p>
 * @param fontfile string <p>
 *       The path to the TrueType font you wish to use.
 *      </p>
 *      <p>
 *       Depending on which version of the GD library PHP is using, when
 *       fontfile does not begin with a leading
 *       / then .ttf will be appended
 *       to the filename and the library will attempt to search for that
 *       filename along a library-defined font path.
 *      </p>
 *      <p>
 *       When using versions of the GD library lower than 2.0.18, a space character,
 *       rather than a semicolon, was used as the 'path separator' for different font files.
 *       Unintentional use of this feature will result in the warning message:
 *       Warning: Could not find/open font. For these affected versions, the
 *       only solution is moving the font to a path which does not contain spaces.
 *      </p>
 *      <p>
 *       In many cases where a font resides in the same directory as the script using it
 *       the following trick will alleviate any include problems.
 *       
 * ]]>
 *       
 *      </p>
 * @param text string <p>
 *       The text string in UTF-8 encoding.
 *      </p>
 *      <p>
 *       May include decimal numeric character references (of the form:
 *       &#8364;) to access characters in a font beyond position 127.
 *       The hexadecimal format (like &#xA9;) is supported.
 *       Strings in UTF-8 encoding can be passed directly.
 *      </p>
 *      <p>
 *       Named entities, such as &copy;, are not supported. Consider using 
 *       html_entity_decode
 *       to decode these named entities into UTF-8 strings (html_entity_decode()
 *       supports this as of PHP 5.0.0).
 *      </p>
 *      <p>
 *       If a character is used in the string which is not supported by the
 *       font, a hollow rectangle will replace the character.
 *      </p>
 * @return array an array with 8 elements representing four points making the
 *   bounding box of the text. The order of the points is lower left, lower 
 *   right, upper right, upper left. The points are relative to the text
 *   regardless of the angle, so "upper left" means in the top left-hand 
 *   corner when you see the text horizontally.
 *   Returns false on error.
 */
function imagettftext ($image, $size, $angle, $x, $y, $color, $fontfile, $text) {}

/**
 * Give the bounding box of a text using fonts via freetype2
 * @link http://www.php.net/manual/en/function.imageftbbox.php
 * @param size float <p>&gd.font.size;</p>
 * @param angle float <p>
 *       Angle in degrees in which text will be 
 *       measured.
 *      </p>
 * @param fontfile string <p>
 *       The name of the TrueType font file (can be a URL). Depending on
 *       which version of the GD library that PHP is using, it may attempt to
 *       search for files that do not begin with a leading '/' by appending
 *       '.ttf' to the filename and searching along a library-defined font path.
 *      </p>
 * @param text string <p>
 *       The string to be measured.
 *      </p>
 * @param extrainfo array[optional] <p>
 *       <table>
 *       Possible array indexes for extrainfo
 *        
 *         
 *          <tr valign="top">
 *           <td>Key</td>
 *           <td>Type</td>
 *           <td>Meaning</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>linespacing</td>
 *           <td>float</td>
 *           <td>Defines drawing linespacing</td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @return array imageftbbox returns an array with 8
 *   elements representing four points making the bounding box of the
 *   text:
 *   
 *    
 *     
 *      <tr valign="top">
 *       <td>0</td>
 *       <td>lower left corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>1</td>
 *       <td>lower left corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>2</td>
 *       <td>lower right corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>3</td>
 *       <td>lower right corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>4</td>
 *       <td>upper right corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>5</td>
 *       <td>upper right corner, Y position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>6</td>
 *       <td>upper left corner, X position</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>7</td>
 *       <td>upper left corner, Y position</td>
 *      </tr>
 *     
 *    
 *   
 *  </p>
 *  <p>
 *   The points are relative to the text regardless of the
 *   angle, so "upper left" means in the top left-hand 
 *   corner seeing the text horizontally.
 */
function imageftbbox ($size, $angle, $fontfile, $text, array $extrainfo = null) {}

/**
 * Write text to the image using fonts using FreeType 2
 * @link http://www.php.net/manual/en/function.imagefttext.php
 * @param image resource 
 * @param size float <p>
 *       The font size to use in points.
 *      </p>
 * @param angle float <p> 
 *       The angle in degrees, with 0 degrees being left-to-right reading text.
 *       Higher values represent a counter-clockwise rotation. For example, a 
 *       value of 90 would result in bottom-to-top reading text.
 *      </p>
 * @param x int <p>
 *       The coordinates given by x and
 *       y will define the basepoint of the first
 *       character (roughly the lower-left corner of the character). This
 *       is different from the imagestring, where
 *       x and y define the
 *       upper-left corner of the first character. For example, "top left"
 *       is 0, 0.
 *      </p>
 * @param y int <p>
 *       The y-ordinate. This sets the position of the fonts baseline, not the
 *       very bottom of the character.
 *      </p>
 * @param color int <p>
 *       The index of the desired color for the text, see 
 *       imagecolorexact.
 *      </p>
 * @param fontfile string <p>
 *       The path to the TrueType font you wish to use.
 *      </p>
 *      <p>
 *       Depending on which version of the GD library PHP is using, when
 *       fontfile does not begin with a leading
 *       / then .ttf will be appended
 *       to the filename and the library will attempt to search for that
 *       filename along a library-defined font path.
 *      </p>
 *      <p>
 *       When using versions of the GD library lower than 2.0.18, a space character,
 *       rather than a semicolon, was used as the 'path separator' for different font files.
 *       Unintentional use of this feature will result in the warning message:
 *       Warning: Could not find/open font. For these affected versions, the
 *       only solution is moving the font to a path which does not contain spaces.
 *      </p>
 *      <p>
 *       In many cases where a font resides in the same directory as the script using it
 *       the following trick will alleviate any include problems.
 *       
 * ]]>
 *       
 *      </p>
 * @param text string <p>
 *       Text to be inserted into image. 
 *      </p>
 * @param extrainfo array[optional] <p>
 *       <table>
 *       Possible array indexes for extrainfo
 *        
 *         
 *          <tr valign="top">
 *           <td>Key</td>
 *           <td>Type</td>
 *           <td>Meaning</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>linespacing</td>
 *           <td>float</td>
 *           <td>Defines drawing linespacing</td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @return array This function returns an array defining the four points of the box, starting in the lower left and moving counter-clockwise:
 *   
 *    
 *     
 *      <tr valign="top">
 *       <td>0</td>
 *       <td>lower left x-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>1</td>
 *       <td>lower left y-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>2</td>
 *       <td>lower right x-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>3</td>
 *       <td>lower right y-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>4</td>
 *       <td>upper right x-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>5</td>
 *       <td>upper right y-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>6</td>
 *       <td>upper left x-coordinate</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>7</td>
 *       <td>upper left y-coordinate</td>
 *      </tr>
 */
function imagefttext ($image, $size, $angle, $x, $y, $color, $fontfile, $text, array $extrainfo = null) {}

/**
 * Return the image types supported by this PHP build
 * @link http://www.php.net/manual/en/function.imagetypes.php
 * @return int a bit-field corresponding to the image formats supported by the
 *   version of GD linked into PHP.  The following bits are returned, 
 *   IMG_GIF | IMG_JPG |
 *   IMG_PNG | IMG_WBMP | 
 *   IMG_XPM.
 */
function imagetypes () {}

/**
 * Convert JPEG image file to WBMP image file
 * @link http://www.php.net/manual/en/function.jpeg2wbmp.php
 * @param jpegname string <p>
 *       Path to JPEG file.
 *      </p>
 * @param wbmpname string <p>
 *       Path to destination WBMP file.
 *      </p>
 * @param dest_height int <p>
 *       Destination image height.
 *      </p>
 * @param dest_width int <p>
 *       Destination image width.
 *      </p>
 * @param threshold int <p>
 *       Threshold value, between 0 and 8 (inclusive).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function jpeg2wbmp ($jpegname, $wbmpname, $dest_height, $dest_width, $threshold) {}

/**
 * Convert PNG image file to WBMP image file
 * @link http://www.php.net/manual/en/function.png2wbmp.php
 * @param pngname string <p>
 *       Path to PNG file.
 *      </p>
 * @param wbmpname string <p>
 *       Path to destination WBMP file.
 *      </p>
 * @param dest_height int <p>
 *       Destination image height.
 *      </p>
 * @param dest_width int <p>
 *       Destination image width.
 *      </p>
 * @param threshold int <p>
 *       Threshold value, between 0 and 8 (inclusive).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function png2wbmp ($pngname, $wbmpname, $dest_height, $dest_width, $threshold) {}

/**
 * &gd.image.output;
 * @link http://www.php.net/manual/en/function.image2wbmp.php
 * @param image resource 
 * @param filename string[optional] <p>
 *       Path to the saved file. If not given, the raw image stream will be
 *       outputted directly.
 *      </p>
 * @param threshold int[optional] <p>
 *       Threshold value, between 0 and 255 (inclusive).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function image2wbmp ($image, $filename = null, $threshold = null) {}

/**
 * Set the alpha blending flag to use the bundled libgd layering effects
 * @link http://www.php.net/manual/en/function.imagelayereffect.php
 * @param image resource 
 * @param effect int <p>
 *       One of the following constants:
 *       
 *        
 *         IMG_EFFECT_REPLACE
 *         
 *          
 *           Use pixel replacement (equivalent of passing true to
 *           imagealphablending)
 * @return bool Returns true on success or false on failure.
 */
function imagelayereffect ($image, $effect) {}

/**
 * Output XBM image to browser or file
 * @link http://www.php.net/manual/en/function.imagexbm.php
 * @param image resource 
 * @param filename string <p>&gd.image.path;</p>
 * @param foreground int[optional] <p>
 *       You can set the foreground color with this parameter by setting an
 *       identifier obtained from imagecolorallocate.
 *       The default foreground color is black.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagexbm ($image, $filename, $foreground = null) {}

/**
 * Makes the colors of the palette version of an image more closely match the true color version
 * @link http://www.php.net/manual/en/function.imagecolormatch.php
 * @param image1 resource <p>
 *       A truecolor image link resource.
 *      </p>
 * @param image2 resource <p>
 *       A palette image link resource pointing to an image that has the same
 *       size as image1.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imagecolormatch ($image1, $image2) {}

/**
 * Applies a filter to an image
 * @link http://www.php.net/manual/en/function.imagefilter.php
 * @param image resource 
 * @param filtertype int <p>
 *       filtertype can be one of the following:
 *       
 *        
 *         
 *          IMG_FILTER_NEGATE: Reverses all colors of
 *          the image.
 * @param arg1 int[optional] <p>
 *       
 *        
 *         
 *          IMG_FILTER_BRIGHTNESS: Brightness level.
 * @param arg2 int[optional] <p>
 *       
 *        
 *         
 *          IMG_FILTER_COLORIZE: &gd.value.green;
 * @param arg3 int[optional] <p>
 *       
 *        
 *         
 *          IMG_FILTER_COLORIZE: &gd.value.blue;
 * @param arg4 int[optional] <p>
 *       
 *        
 *         
 *          IMG_FILTER_COLORIZE: Alpha channel, A value 
 *          between 0 and 127. 0 indicates completely opaque while 127 indicates 
 *          completely transparent.
 * @return bool Returns true on success or false on failure.
 */
function imagefilter ($image, $filtertype, $arg1 = null, $arg2 = null, $arg3 = null, $arg4 = null) {}

/**
 * Apply a 3x3 convolution matrix, using coefficient and offset
 * @link http://www.php.net/manual/en/function.imageconvolution.php
 * @param image resource 
 * @param matrix array <p>
 *       A 3x3 matrix: an array of three arrays of three floats.
 *      </p>
 * @param div float <p>
 *       The divisor of the result of the convolution, used for normalization.
 *      </p>
 * @param offset float <p>
 *       Color offset.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imageconvolution ($image, array $matrix, $div, $offset) {}


/**
 * Used as a return value by imagetypes
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_GIF', 1);

/**
 * Used as a return value by imagetypes
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_JPG', 2);

/**
 * Used as a return value by imagetypes
 *    
 *    
 *     <p>
 *      This constant has the same value as IMG_JPG
 *     </p>
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_JPEG', 2);

/**
 * Used as a return value by imagetypes
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_PNG', 4);

/**
 * Used as a return value by imagetypes
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_WBMP', 8);

/**
 * Used as a return value by imagetypes
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_XPM', 16);

/**
 * Special color option which can be used in stead of color allocated with
 *     imagecolorallocate or
 *     imagecolorallocatealpha
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_COLOR_TILED', -5);

/**
 * Special color option which can be used in stead of color allocated with
 *     imagecolorallocate or
 *     imagecolorallocatealpha
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_COLOR_STYLED', -2);

/**
 * Special color option which can be used in stead of color allocated with
 *     imagecolorallocate or
 *     imagecolorallocatealpha
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_COLOR_BRUSHED', -3);

/**
 * Special color option which can be used in stead of color allocated with
 *     imagecolorallocate or
 *     imagecolorallocatealpha
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_COLOR_STYLEDBRUSHED', -4);

/**
 * Special color option which can be used in stead of color allocated with
 *     imagecolorallocate or
 *     imagecolorallocatealpha
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_COLOR_TRANSPARENT', -6);

/**
 * A style constant used by the imagefilledarc function.
 *    
 *    
 *     <p>
 *      This constant has the same value as IMG_ARC_PIE
 *     </p>
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_ARC_ROUNDED', 0);

/**
 * A style constant used by the imagefilledarc function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_ARC_PIE', 0);

/**
 * A style constant used by the imagefilledarc function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_ARC_CHORD', 1);

/**
 * A style constant used by the imagefilledarc function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_ARC_NOFILL', 2);

/**
 * A style constant used by the imagefilledarc function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_ARC_EDGED', 4);

/**
 * A type constant used by the imagegd2 function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_GD2_RAW', 1);

/**
 * A type constant used by the imagegd2 function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_GD2_COMPRESSED', 2);

/**
 * Alpha blending effect used by the imagelayereffect function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_EFFECT_REPLACE', 0);

/**
 * Alpha blending effect used by the imagelayereffect function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_EFFECT_ALPHABLEND', 1);

/**
 * Alpha blending effect used by the imagelayereffect function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_EFFECT_NORMAL', 2);

/**
 * Alpha blending effect used by the imagelayereffect function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_EFFECT_OVERLAY', 3);

/**
 * When the bundled version of GD is used this is 1 otherwise 
 *     its set to 0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_BUNDLED', 1);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_NEGATE', 0);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_GRAYSCALE', 1);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_BRIGHTNESS', 2);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_CONTRAST', 3);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_COLORIZE', 4);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_EDGEDETECT', 5);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_GAUSSIAN_BLUR', 7);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_SELECTIVE_BLUR', 8);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_EMBOSS', 6);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_MEAN_REMOVAL', 9);

/**
 * Special GD filter used by the imagefilter function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_SMOOTH', 10);

/**
 * Special GD filter used by the imagefilter function. 
 *     (Available as of PHP 5.3.0)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FILTER_PIXELATE', 11);

/**
 * The GD version PHP was compiled against.
 *     (Available as of PHP 5.2.4)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_VERSION', "2.0.35");

/**
 * The GD major version PHP was compiled against.
 *     (Available as of PHP 5.2.4)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_MAJOR_VERSION', 2);

/**
 * The GD minor version PHP was compiled against.
 *     (Available as of PHP 5.2.4)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_MINOR_VERSION', 0);

/**
 * The GD release version PHP was compiled against.
 *     (Available as of PHP 5.2.4)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_RELEASE_VERSION', 35);

/**
 * The GD "extra" version (beta/rc..) PHP was compiled against.
 *     (Available as of PHP 5.2.4)
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('GD_EXTRA_VERSION', "");

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_NO_FILTER', 0);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_FILTER_NONE', 8);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_FILTER_SUB', 16);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_FILTER_UP', 32);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_FILTER_AVG', 64);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_FILTER_PAETH', 128);

/**
 * A special PNG filter, used by the imagepng function.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('PNG_ALL_FILTERS', 248);

// End of gd v.
?>
