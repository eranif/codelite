<?php

// Start of gd v.

function gd_info () {}

/**
 * @param im
 * @param cx
 * @param cy
 * @param w
 * @param h
 * @param s
 * @param e
 * @param col
 */
function imagearc ($im, $cx, $cy, $w, $h, $s, $e, $col) {}

/**
 * @param im
 * @param cx
 * @param cy
 * @param w
 * @param h
 * @param color
 */
function imageellipse ($im, $cx, $cy, $w, $h, $color) {}

/**
 * @param im
 * @param font
 * @param x
 * @param y
 * @param c
 * @param col
 */
function imagechar ($im, $font, $x, $y, $c, $col) {}

/**
 * @param im
 * @param font
 * @param x
 * @param y
 * @param c
 * @param col
 */
function imagecharup ($im, $font, $x, $y, $c, $col) {}

/**
 * @param im
 * @param x
 * @param y
 */
function imagecolorat ($im, $x, $y) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 */
function imagecolorallocate ($im, $red, $green, $blue) {}

/**
 * @param dst
 * @param src
 */
function imagepalettecopy ($dst, $src) {}

/**
 * @param image
 */
function imagecreatefromstring ($image) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 */
function imagecolorclosest ($im, $red, $green, $blue) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 */
function imagecolorclosesthwb ($im, $red, $green, $blue) {}

/**
 * @param im
 * @param index
 */
function imagecolordeallocate ($im, $index) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 */
function imagecolorresolve ($im, $red, $green, $blue) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 */
function imagecolorexact ($im, $red, $green, $blue) {}

/**
 * @param im
 * @param color
 * @param red
 * @param green
 * @param blue
 * @param alpha[optional]
 */
function imagecolorset ($im, $color, $red, $green, $blue, $alpha) {}

/**
 * @param im
 * @param col[optional]
 */
function imagecolortransparent ($im, $col) {}

/**
 * @param im
 */
function imagecolorstotal ($im) {}

/**
 * @param im
 * @param index
 */
function imagecolorsforindex ($im, $index) {}

/**
 * @param dst_im
 * @param src_im
 * @param dst_x
 * @param dst_y
 * @param src_x
 * @param src_y
 * @param src_w
 * @param src_h
 */
function imagecopy ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h) {}

/**
 * @param src_im
 * @param dst_im
 * @param dst_x
 * @param dst_y
 * @param src_x
 * @param src_y
 * @param src_w
 * @param src_h
 * @param pct
 */
function imagecopymerge ($src_im, $dst_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h, $pct) {}

/**
 * @param src_im
 * @param dst_im
 * @param dst_x
 * @param dst_y
 * @param src_x
 * @param src_y
 * @param src_w
 * @param src_h
 * @param pct
 */
function imagecopymergegray ($src_im, $dst_im, $dst_x, $dst_y, $src_x, $src_y, $src_w, $src_h, $pct) {}

/**
 * @param dst_im
 * @param src_im
 * @param dst_x
 * @param dst_y
 * @param src_x
 * @param src_y
 * @param dst_w
 * @param dst_h
 * @param src_w
 * @param src_h
 */
function imagecopyresized ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h) {}

/**
 * @param x_size
 * @param y_size
 */
function imagecreate ($x_size, $y_size) {}

/**
 * @param x_size
 * @param y_size
 */
function imagecreatetruecolor ($x_size, $y_size) {}

/**
 * @param im
 */
function imageistruecolor ($im) {}

/**
 * @param im
 * @param ditherFlag
 * @param colorsWanted
 */
function imagetruecolortopalette ($im, $ditherFlag, $colorsWanted) {}

/**
 * @param im
 */
function imagepalettetotruecolor ($im) {}

/**
 * @param im
 * @param thickness
 */
function imagesetthickness ($im, $thickness) {}

/**
 * @param im
 * @param cx
 * @param cy
 * @param w
 * @param h
 * @param s
 * @param e
 * @param col
 * @param style
 */
function imagefilledarc ($im, $cx, $cy, $w, $h, $s, $e, $col, $style) {}

/**
 * @param im
 * @param cx
 * @param cy
 * @param w
 * @param h
 * @param color
 */
function imagefilledellipse ($im, $cx, $cy, $w, $h, $color) {}

/**
 * @param im
 * @param blend
 */
function imagealphablending ($im, $blend) {}

/**
 * @param im
 * @param save
 */
function imagesavealpha ($im, $save) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
function imagecolorallocatealpha ($im, $red, $green, $blue, $alpha) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
function imagecolorresolvealpha ($im, $red, $green, $blue, $alpha) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
function imagecolorclosestalpha ($im, $red, $green, $blue, $alpha) {}

/**
 * @param im
 * @param red
 * @param green
 * @param blue
 * @param alpha
 */
function imagecolorexactalpha ($im, $red, $green, $blue, $alpha) {}

/**
 * @param dst_im
 * @param src_im
 * @param dst_x
 * @param dst_y
 * @param src_x
 * @param src_y
 * @param dst_w
 * @param dst_h
 * @param src_w
 * @param src_h
 */
function imagecopyresampled ($dst_im, $src_im, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h) {}

/**
 * @param im
 * @param angle
 * @param bgdcolor
 * @param ignoretransparent[optional]
 */
function imagerotate ($im, $angle, $bgdcolor, $ignoretransparent) {}

/**
 * @param im
 * @param mode
 */
function imageflip ($im, $mode) {}

/**
 * @param im
 * @param on
 */
function imageantialias ($im, $on) {}

/**
 * @param im
 * @param rect
 */
function imagecrop ($im, $rect) {}

/**
 * @param im
 * @param mode[optional]
 * @param threshold[optional]
 * @param color[optional]
 */
function imagecropauto ($im, $mode, $threshold, $color) {}

/**
 * @param im
 * @param new_width
 * @param new_height[optional]
 * @param mode[optional]
 */
function imagescale ($im, $new_width, $new_height, $mode) {}

/**
 * @param im
 * @param affine
 * @param clip[optional]
 */
function imageaffine ($im, $affine, $clip) {}

/**
 * @param m1
 * @param m2
 */
function imageaffinematrixconcat ($m1, $m2) {}

/**
 * @param type
 * @param options[optional]
 */
function imageaffinematrixget ($type, $options) {}

/**
 * @param im
 * @param method
 */
function imagesetinterpolation ($im, $method) {}

/**
 * @param im
 * @param tile
 */
function imagesettile ($im, $tile) {}

/**
 * @param im
 * @param brush
 */
function imagesetbrush ($im, $brush) {}

/**
 * @param im
 * @param styles
 */
function imagesetstyle ($im, $styles) {}

/**
 * @param filename
 */
function imagecreatefrompng ($filename) {}

/**
 * @param filename
 */
function imagecreatefromgif ($filename) {}

/**
 * @param filename
 */
function imagecreatefromjpeg ($filename) {}

/**
 * @param filename
 */
function imagecreatefromwbmp ($filename) {}

/**
 * @param filename
 */
function imagecreatefromxbm ($filename) {}

/**
 * @param filename
 */
function imagecreatefromgd ($filename) {}

/**
 * @param filename
 */
function imagecreatefromgd2 ($filename) {}

/**
 * @param filename
 * @param srcX
 * @param srcY
 * @param width
 * @param height
 */
function imagecreatefromgd2part ($filename, $srcX, $srcY, $width, $height) {}

/**
 * @param im
 * @param filename[optional]
 */
function imagepng ($im, $filename) {}

/**
 * @param im
 * @param filename[optional]
 */
function imagegif ($im, $filename) {}

/**
 * @param im
 * @param filename[optional]
 * @param quality[optional]
 */
function imagejpeg ($im, $filename, $quality) {}

/**
 * @param im
 * @param filename[optional]
 * @param foreground[optional]
 */
function imagewbmp ($im, $filename, $foreground) {}

/**
 * @param im
 * @param filename[optional]
 */
function imagegd ($im, $filename) {}

/**
 * @param im
 * @param filename[optional]
 * @param chunk_size[optional]
 * @param type[optional]
 */
function imagegd2 ($im, $filename, $chunk_size, $type) {}

/**
 * @param im
 */
function imagedestroy ($im) {}

/**
 * @param im
 * @param inputgamma
 * @param outputgamma
 */
function imagegammacorrect ($im, $inputgamma, $outputgamma) {}

/**
 * @param im
 * @param x
 * @param y
 * @param col
 */
function imagefill ($im, $x, $y, $col) {}

/**
 * @param im
 * @param points
 * @param num_pos
 * @param col
 */
function imagefilledpolygon ($im, $points, $num_pos, $col) {}

/**
 * @param im
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param col
 */
function imagefilledrectangle ($im, $x1, $y1, $x2, $y2, $col) {}

/**
 * @param im
 * @param x
 * @param y
 * @param border
 * @param col
 */
function imagefilltoborder ($im, $x, $y, $border, $col) {}

/**
 * @param font
 */
function imagefontwidth ($font) {}

/**
 * @param font
 */
function imagefontheight ($font) {}

/**
 * @param im
 * @param interlace[optional]
 */
function imageinterlace ($im, $interlace) {}

/**
 * @param im
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param col
 */
function imageline ($im, $x1, $y1, $x2, $y2, $col) {}

/**
 * @param filename
 */
function imageloadfont ($filename) {}

/**
 * @param im
 * @param points
 * @param num_pos
 * @param col
 */
function imagepolygon ($im, $points, $num_pos, $col) {}

/**
 * @param im
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param col
 */
function imagerectangle ($im, $x1, $y1, $x2, $y2, $col) {}

/**
 * @param im
 * @param x
 * @param y
 * @param col
 */
function imagesetpixel ($im, $x, $y, $col) {}

/**
 * @param im
 * @param font
 * @param x
 * @param y
 * @param str
 * @param col
 */
function imagestring ($im, $font, $x, $y, $str, $col) {}

/**
 * @param im
 * @param font
 * @param x
 * @param y
 * @param str
 * @param col
 */
function imagestringup ($im, $font, $x, $y, $str, $col) {}

/**
 * @param im
 */
function imagesx ($im) {}

/**
 * @param im
 */
function imagesy ($im) {}

/**
 * @param im
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param col
 */
function imagedashedline ($im, $x1, $y1, $x2, $y2, $col) {}

/**
 * @param size
 * @param angle
 * @param font_file
 * @param text
 */
function imagettfbbox ($size, $angle, $font_file, $text) {}

/**
 * @param im
 * @param size
 * @param angle
 * @param x
 * @param y
 * @param col
 * @param font_file
 * @param text
 */
function imagettftext ($im, $size, $angle, $x, $y, $col, $font_file, $text) {}

/**
 * @param size
 * @param angle
 * @param font_file
 * @param text
 * @param extrainfo[optional]
 */
function imageftbbox ($size, $angle, $font_file, $text, $extrainfo) {}

/**
 * @param im
 * @param size
 * @param angle
 * @param x
 * @param y
 * @param col
 * @param font_file
 * @param text
 * @param extrainfo[optional]
 */
function imagefttext ($im, $size, $angle, $x, $y, $col, $font_file, $text, $extrainfo) {}

/**
 * @param pathname
 */
function imagepsloadfont ($pathname) {}

/**
 * @param font_index
 */
function imagepsfreefont ($font_index) {}

/**
 * @param font_index
 * @param filename
 */
function imagepsencodefont ($font_index, $filename) {}

/**
 * @param font_index
 * @param extend
 */
function imagepsextendfont ($font_index, $extend) {}

/**
 * @param font_index
 * @param slant
 */
function imagepsslantfont ($font_index, $slant) {}

/**
 * @param im
 * @param text
 * @param font
 * @param size
 * @param foreground
 * @param background
 * @param xcoord
 * @param ycoord
 * @param space[optional]
 * @param tightness[optional]
 * @param angle[optional]
 * @param antialias[optional]
 */
function imagepstext ($im, $text, $font, $size, $foreground, $background, $xcoord, $ycoord, $space, $tightness, $angle, $antialias) {}

/**
 * @param text
 * @param font
 * @param size
 * @param space[optional]
 * @param tightness[optional]
 * @param angle[optional]
 */
function imagepsbbox ($text, $font, $size, $space, $tightness, $angle) {}

function imagetypes () {}

/**
 * @param f_org
 * @param f_dest
 * @param d_height
 * @param d_width
 * @param d_threshold
 */
function jpeg2wbmp ($f_org, $f_dest, $d_height, $d_width, $d_threshold) {}

/**
 * @param f_org
 * @param f_dest
 * @param d_height
 * @param d_width
 * @param d_threshold
 */
function png2wbmp ($f_org, $f_dest, $d_height, $d_width, $d_threshold) {}

/**
 * @param im
 * @param filename[optional]
 * @param threshold[optional]
 */
function image2wbmp ($im, $filename, $threshold) {}

/**
 * @param im
 * @param effect
 */
function imagelayereffect ($im, $effect) {}

/**
 * @param im
 * @param filename
 * @param foreground[optional]
 */
function imagexbm ($im, $filename, $foreground) {}

/**
 * @param im1
 * @param im2
 */
function imagecolormatch ($im1, $im2) {}

/**
 * @param im
 * @param filtertype
 * @param arg1[optional]
 * @param arg2[optional]
 * @param arg3[optional]
 * @param arg4[optional]
 */
function imagefilter ($im, $filtertype, $arg1, $arg2, $arg3, $arg4) {}

/**
 * @param im
 * @param matrix3x3
 * @param div
 * @param offset
 */
function imageconvolution ($im, $matrix3x3, $div, $offset) {}


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
 * Used together with imageflip, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FLIP_HORIZONTAL', 1);

/**
 * Used together with imageflip, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FLIP_VERTICAL', 2);

/**
 * Used together with imageflip, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_FLIP_BOTH', 3);

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
define ('IMG_CROP_DEFAULT', 0);
define ('IMG_CROP_TRANSPARENT', 1);
define ('IMG_CROP_BLACK', 2);
define ('IMG_CROP_WHITE', 3);
define ('IMG_CROP_SIDES', 4);
define ('IMG_CROP_THRESHOLD', 5);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BELL', 1);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BESSEL', 2);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BILINEAR_FIXED', 3);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BICUBIC', 4);
define ('IMG_BICUBIC_FIXED', 5);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BLACKMAN', 6);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BOX', 7);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_BSPLINE', 8);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_CATMULLROM', 9);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_GAUSSIAN', 10);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_GENERALIZED_CUBIC', 11);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_HERMITE', 12);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_HAMMING', 13);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_HANNING', 14);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_MITCHELL', 15);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_POWER', 17);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_QUADRATIC', 18);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_SINC', 19);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_NEAREST_NEIGHBOUR', 16);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_WEIGHTED4', 21);

/**
 * Used together with imagesetinterpolation, available as of PHP 5.5.0.
 * @link http://www.php.net/manual/en/image.constants.php
 */
define ('IMG_TRIANGLE', 20);
define ('IMG_AFFINE_TRANSLATE', 0);
define ('IMG_AFFINE_SCALE', 1);
define ('IMG_AFFINE_ROTATE', 2);
define ('IMG_AFFINE_SHEAR_HORIZONTAL', 3);
define ('IMG_AFFINE_SHEAR_VERTICAL', 4);

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
