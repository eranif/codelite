<?php

// Start of fileinfo v.1.0.5

class finfo  {

	/**
	 * @param options[optional]
	 * @param arg[optional]
	 */
	public function finfo ($options, $arg) {}

	/**
	 * @param options
	 */
	public function set_flags ($options) {}

	/**
	 * @param filename
	 * @param options[optional]
	 * @param context[optional]
	 */
	public function file ($filename, $options, $context) {}

	/**
	 * @param string
	 * @param options[optional]
	 * @param context[optional]
	 */
	public function buffer ($string, $options, $context) {}

}

/**
 * @param options[optional]
 * @param arg[optional]
 */
function finfo_open ($options, $arg) {}

/**
 * @param finfo
 */
function finfo_close ($finfo) {}

/**
 * @param finfo
 * @param options
 */
function finfo_set_flags ($finfo, $options) {}

/**
 * @param finfo
 * @param filename
 * @param options[optional]
 * @param context[optional]
 */
function finfo_file ($finfo, $filename, $options, $context) {}

/**
 * @param finfo
 * @param string
 * @param options[optional]
 * @param context[optional]
 */
function finfo_buffer ($finfo, $string, $options, $context) {}

/**
 * @param string
 */
function mime_content_type ($string) {}


/**
 * No special handling.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_NONE', 0);

/**
 * Follow symlinks.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_SYMLINK', 2);

/**
 * Return the mime type and mime encoding as defined by RFC 2045.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_MIME', 1040);

/**
 * Return the mime type.
 *    
 *    
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_MIME_TYPE', 16);

/**
 * Return the mime encoding of the file.
 *    
 *    
 *     Available since PHP 5.3.0.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_MIME_ENCODING', 1024);

/**
 * Look at the contents of blocks or character special devices.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_DEVICES', 8);

/**
 * Return all matches, not just the first.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_CONTINUE', 32);

/**
 * If possible preserve the original access time.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_PRESERVE_ATIME', 128);

/**
 * Don't translate unprintable characters to a \ooo octal
 *     representation.
 * @link http://www.php.net/manual/en/fileinfo.constants.php
 */
define ('FILEINFO_RAW', 256);

// End of fileinfo v.1.0.5
?>
