<?php

// Start of bz2 v.

/**
 * @param file
 * @param mode
 */
function bzopen ($file, $mode) {}

/**
 * @param bz
 * @param length[optional]
 */
function bzread ($bz, $length) {}

/**
 * @param fp
 * @param str
 * @param length[optional]
 */
function bzwrite ($fp, $str, $length) {}

/**
 * @param fp
 */
function bzflush ($fp) {}

/**
 * @param fp
 */
function bzclose ($fp) {}

/**
 * @param bz
 */
function bzerrno ($bz) {}

/**
 * @param bz
 */
function bzerrstr ($bz) {}

/**
 * @param bz
 */
function bzerror ($bz) {}

/**
 * @param source
 * @param blocksize
 * @param workfactor[optional]
 */
function bzcompress ($source, $blocksize, $workfactor) {}

/**
 * @param source
 * @param small[optional]
 */
function bzdecompress ($source, $small) {}

// End of bz2 v.
?>
