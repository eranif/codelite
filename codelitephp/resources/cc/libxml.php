<?php

// Start of libxml v.

class LibXMLError  {
}

/**
 * Set the streams context for the next libxml document load or write
 * @link http://www.php.net/manual/en/function.libxml-set-streams-context.php
 * @param streams_context resource <p>
 *        The stream context resource (created with
 *        stream_context_create)
 *       </p>
 * @return void 
 */
function libxml_set_streams_context ($streams_context) {}

/**
 * Disable libxml errors and allow user to fetch error information as needed
 * @link http://www.php.net/manual/en/function.libxml-use-internal-errors.php
 * @param use_errors bool[optional] <p>
 *        Whether to enable user error handling.
 *       </p>
 * @return bool This function returns the previous value of
 *   use_errors.
 */
function libxml_use_internal_errors ($use_errors = null) {}

/**
 * Retrieve last error from libxml
 * @link http://www.php.net/manual/en/function.libxml-get-last-error.php
 * @return LibXMLError a LibXMLError object if there is any error in the
 *   buffer, false otherwise.
 */
function libxml_get_last_error () {}

/**
 * Clear libxml error buffer
 * @link http://www.php.net/manual/en/function.libxml-clear-errors.php
 * @return void 
 */
function libxml_clear_errors () {}

/**
 * Retrieve array of errors
 * @link http://www.php.net/manual/en/function.libxml-get-errors.php
 * @return array an array with LibXMLError objects if there are any
 *   errors in the buffer, or an empty array otherwise.
 */
function libxml_get_errors () {}

/**
 * Disable the ability to load external entities
 * @link http://www.php.net/manual/en/function.libxml-disable-entity-loader.php
 * @param disable bool[optional] <p>
 *       Disable (true) or enable (false) libxml extensions (such as
 *       , 
 *       and ) to load external entities.
 *      </p>
 * @return bool the previous value.
 */
function libxml_disable_entity_loader ($disable = null) {}


/**
 * libxml version like 20605 or 20617
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_VERSION', 20707);

/**
 * libxml version like 2.6.5 or 2.6.17
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_DOTTED_VERSION', "2.7.7");
define ('LIBXML_LOADED_VERSION', 20707);

/**
 * Substitute entities
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOENT', 2);

/**
 * Load the external subset
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_DTDLOAD', 4);

/**
 * Default DTD attributes
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_DTDATTR', 8);

/**
 * Validate with the DTD
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_DTDVALID', 16);

/**
 * Suppress error reports
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOERROR', 32);

/**
 * Suppress warning reports
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOWARNING', 64);

/**
 * Remove blank nodes
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOBLANKS', 256);

/**
 * Implement XInclude substitution
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_XINCLUDE', 1024);

/**
 * Remove redundant namespaces declarations
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NSCLEAN', 8192);

/**
 * Merge CDATA as text nodes
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOCDATA', 16384);

/**
 * Disable network access when loading documents
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NONET', 2048);

/**
 * Activate small nodes allocation optimization. This may speed up your
 *     application without needing to change the code.
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.6.21
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_COMPACT', 65536);

/**
 * Drop the XML declaration when saving a document
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.6.21
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOXMLDECL', 2);

/**
 * Sets XML_PARSE_HUGE flag, which relaxes any hardcoded limit from the parser.
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_PARSEHUGE', 524288);

/**
 * Expand empty tags (e.g.  to
 *     )
 *    
 *    
 *     <p>
 *      This option is currently just available in the
 *       and
 *       functions.
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_NOEMPTYTAG', 4);

/**
 * No errors
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_ERR_NONE', 0);

/**
 * A simple warning
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_ERR_WARNING', 1);

/**
 * A recoverable error
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_ERR_ERROR', 2);

/**
 * A fatal error
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_ERR_FATAL', 3);

// End of libxml v.
?>
