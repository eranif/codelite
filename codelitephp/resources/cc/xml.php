<?php

// Start of xml v.

/**
 * @param encoding[optional]
 */
function xml_parser_create ($encoding) {}

/**
 * @param encoding[optional]
 * @param sep[optional]
 */
function xml_parser_create_ns ($encoding, $sep) {}

/**
 * @param parser
 * @param obj
 */
function xml_set_object ($parser, &$obj) {}

/**
 * @param parser
 * @param shdl
 * @param ehdl
 */
function xml_set_element_handler ($parser, $shdl, $ehdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_character_data_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_processing_instruction_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_default_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_unparsed_entity_decl_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_notation_decl_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_external_entity_ref_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_start_namespace_decl_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param hdl
 */
function xml_set_end_namespace_decl_handler ($parser, $hdl) {}

/**
 * @param parser
 * @param data
 * @param isfinal[optional]
 */
function xml_parse ($parser, $data, $isfinal) {}

/**
 * @param parser
 * @param data
 * @param values
 * @param index[optional]
 */
function xml_parse_into_struct ($parser, $data, &$values, &$index) {}

/**
 * @param parser
 */
function xml_get_error_code ($parser) {}

/**
 * @param code
 */
function xml_error_string ($code) {}

/**
 * @param parser
 */
function xml_get_current_line_number ($parser) {}

/**
 * @param parser
 */
function xml_get_current_column_number ($parser) {}

/**
 * @param parser
 */
function xml_get_current_byte_index ($parser) {}

/**
 * @param parser
 */
function xml_parser_free ($parser) {}

/**
 * @param parser
 * @param option
 * @param value
 */
function xml_parser_set_option ($parser, $option, $value) {}

/**
 * @param parser
 * @param option
 */
function xml_parser_get_option ($parser, $option) {}

/**
 * @param data
 */
function utf8_encode ($data) {}

/**
 * @param data
 */
function utf8_decode ($data) {}

define ('XML_ERROR_NONE', 0);
define ('XML_ERROR_NO_MEMORY', 1);
define ('XML_ERROR_SYNTAX', 2);
define ('XML_ERROR_NO_ELEMENTS', 3);
define ('XML_ERROR_INVALID_TOKEN', 4);
define ('XML_ERROR_UNCLOSED_TOKEN', 5);
define ('XML_ERROR_PARTIAL_CHAR', 6);
define ('XML_ERROR_TAG_MISMATCH', 7);
define ('XML_ERROR_DUPLICATE_ATTRIBUTE', 8);
define ('XML_ERROR_JUNK_AFTER_DOC_ELEMENT', 9);
define ('XML_ERROR_PARAM_ENTITY_REF', 10);
define ('XML_ERROR_UNDEFINED_ENTITY', 11);
define ('XML_ERROR_RECURSIVE_ENTITY_REF', 12);
define ('XML_ERROR_ASYNC_ENTITY', 13);
define ('XML_ERROR_BAD_CHAR_REF', 14);
define ('XML_ERROR_BINARY_ENTITY_REF', 15);
define ('XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF', 16);
define ('XML_ERROR_MISPLACED_XML_PI', 17);
define ('XML_ERROR_UNKNOWN_ENCODING', 18);
define ('XML_ERROR_INCORRECT_ENCODING', 19);
define ('XML_ERROR_UNCLOSED_CDATA_SECTION', 20);
define ('XML_ERROR_EXTERNAL_ENTITY_HANDLING', 21);
define ('XML_OPTION_CASE_FOLDING', 1);
define ('XML_OPTION_TARGET_ENCODING', 2);
define ('XML_OPTION_SKIP_TAGSTART', 3);
define ('XML_OPTION_SKIP_WHITE', 4);

/**
 * Holds the SAX implementation method.
 *     Can be libxml or expat.
 * @link http://www.php.net/manual/en/xml.constants.php
 */
define ('XML_SAX_IMPL', "libxml");

// End of xml v.
?>
