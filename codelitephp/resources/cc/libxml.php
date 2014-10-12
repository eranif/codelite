<?php

// Start of libxml v.

class LibXMLError  {
}

/**
 * @param context
 */
function libxml_set_streams_context ($context) {}

/**
 * @param use_errors[optional]
 */
function libxml_use_internal_errors ($use_errors) {}

function libxml_get_last_error () {}

function libxml_clear_errors () {}

function libxml_get_errors () {}

/**
 * @param disable[optional]
 */
function libxml_disable_entity_loader ($disable) {}

/**
 * @param resolver_function
 */
function libxml_set_external_entity_loader ($resolver_function) {}


/**
 * libxml version like 20605 or 20617
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_VERSION', 20901);

/**
 * libxml version like 2.6.5 or 2.6.17
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_DOTTED_VERSION', "2.9.1");
define ('LIBXML_LOADED_VERSION', 20901);

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
 * Sets XML_PARSE_PEDANTIC flag, which enables pedentic error reporting.
 *    
 *    
 *     <p>
 *      Available as of PHP >= 5.4.0
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_PEDANTIC', 128);

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
 * Sets XML_PARSE_HUGE flag, which relaxes any hardcoded limit from the parser. This affects 
 *     limits like maximum depth of a document or the entity recursion, as well as limits of the 
 *     size of text nodes.
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.7.0 (as of PHP >= 5.3.2 and PHP >= 5.2.12)
 *     </p>
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
 * Create default/fixed value nodes during XSD schema validation
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.6.14 (as of PHP >= 5.5.2)
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_SCHEMA_CREATE', 1);

/**
 * Sets HTML_PARSE_NOIMPLIED flag, which turns off the
 *     automatic adding of implied html/body... elements.
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.7.7 (as of PHP >= 5.4.0)
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_HTML_NOIMPLIED', 8192);

/**
 * Sets HTML_PARSE_NODEFDTD flag, which prevents a default doctype
 *     being added when one is not found.
 *    
 *    
 *     <p>
 *      Only available in Libxml >= 2.7.8 (as of PHP >= 5.4.0)
 *     </p>
 * @link http://www.php.net/manual/en/libxml.constants.php
 */
define ('LIBXML_HTML_NODEFDTD', 4);

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
