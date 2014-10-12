<?php

// Start of exif v.1.4 $Id: 0b28f1c2604848245c00549a8c8055a1c0d22a49 $

/**
 * @param filename
 * @param sections_needed[optional]
 * @param sub_arrays[optional]
 * @param read_thumbnail[optional]
 */
function exif_read_data ($filename, $sections_needed, $sub_arrays, $read_thumbnail) {}

/**
 * @param filename
 * @param sections_needed[optional]
 * @param sub_arrays[optional]
 * @param read_thumbnail[optional]
 */
function read_exif_data ($filename, $sections_needed, $sub_arrays, $read_thumbnail) {}

/**
 * @param index
 */
function exif_tagname ($index) {}

/**
 * @param filename
 * @param width[optional]
 * @param height[optional]
 * @param imagetype[optional]
 */
function exif_thumbnail ($filename, &$width, &$height, &$imagetype) {}

/**
 * @param imagefile
 */
function exif_imagetype ($imagefile) {}

define ('EXIF_USE_MBSTRING', 1);

// End of exif v.1.4 $Id: 0b28f1c2604848245c00549a8c8055a1c0d22a49 $
?>
