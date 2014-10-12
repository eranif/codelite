<?php

// Start of tidy v.2.0

class tidy  {

	public function getOpt () {}

	public function cleanRepair () {}

	public function parseFile () {}

	public function parseString () {}

	public function repairString () {}

	public function repairFile () {}

	public function diagnose () {}

	public function getRelease () {}

	public function getConfig () {}

	public function getStatus () {}

	public function getHtmlVer () {}

	public function getOptDoc () {}

	public function isXhtml () {}

	public function isXml () {}

	public function root () {}

	public function head () {}

	public function html () {}

	public function body () {}

	public function __construct () {}

}

final class tidyNode  {

	public function hasChildren () {}

	public function hasSiblings () {}

	public function isComment () {}

	public function isHtml () {}

	public function isText () {}

	public function isJste () {}

	public function isAsp () {}

	public function isPhp () {}

	public function getParent () {}

	private function __construct () {}

}

/**
 * @param option
 */
function tidy_getopt ($option) {}

/**
 * @param input
 * @param config_options[optional]
 * @param encoding[optional]
 */
function tidy_parse_string ($input, $config_options, $encoding) {}

/**
 * @param file
 * @param config_options[optional]
 * @param encoding[optional]
 * @param use_include_path[optional]
 */
function tidy_parse_file ($file, $config_options, $encoding, $use_include_path) {}

function tidy_get_output () {}

function tidy_get_error_buffer () {}

function tidy_clean_repair () {}

/**
 * @param data
 * @param config_file[optional]
 * @param encoding[optional]
 */
function tidy_repair_string ($data, $config_file, $encoding) {}

/**
 * @param filename
 * @param config_file[optional]
 * @param encoding[optional]
 * @param use_include_path[optional]
 */
function tidy_repair_file ($filename, $config_file, $encoding, $use_include_path) {}

function tidy_diagnose () {}

function tidy_get_release () {}

function tidy_get_config () {}

function tidy_get_status () {}

function tidy_get_html_ver () {}

function tidy_is_xhtml () {}

function tidy_is_xml () {}

function tidy_error_count () {}

function tidy_warning_count () {}

function tidy_access_count () {}

function tidy_config_count () {}

/**
 * @param resource
 * @param optname
 */
function tidy_get_opt_doc ($resource, $optname) {}

function tidy_get_root () {}

function tidy_get_head () {}

function tidy_get_html () {}

/**
 * @param tidy
 */
function tidy_get_body ($tidy) {}


/**
 * description
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_TAG_UNKNOWN', 0);
define ('TIDY_TAG_A', 1);
define ('TIDY_TAG_ABBR', 2);
define ('TIDY_TAG_ACRONYM', 3);
define ('TIDY_TAG_ADDRESS', 4);
define ('TIDY_TAG_ALIGN', 5);
define ('TIDY_TAG_APPLET', 6);
define ('TIDY_TAG_AREA', 7);
define ('TIDY_TAG_B', 8);
define ('TIDY_TAG_BASE', 9);
define ('TIDY_TAG_BASEFONT', 10);
define ('TIDY_TAG_BDO', 11);
define ('TIDY_TAG_BGSOUND', 12);
define ('TIDY_TAG_BIG', 13);
define ('TIDY_TAG_BLINK', 14);
define ('TIDY_TAG_BLOCKQUOTE', 15);
define ('TIDY_TAG_BODY', 16);
define ('TIDY_TAG_BR', 17);
define ('TIDY_TAG_BUTTON', 18);
define ('TIDY_TAG_CAPTION', 19);
define ('TIDY_TAG_CENTER', 20);
define ('TIDY_TAG_CITE', 21);
define ('TIDY_TAG_CODE', 22);
define ('TIDY_TAG_COL', 23);
define ('TIDY_TAG_COLGROUP', 24);
define ('TIDY_TAG_COMMENT', 25);
define ('TIDY_TAG_DD', 26);
define ('TIDY_TAG_DEL', 27);
define ('TIDY_TAG_DFN', 28);
define ('TIDY_TAG_DIR', 29);
define ('TIDY_TAG_DIV', 30);
define ('TIDY_TAG_DL', 31);
define ('TIDY_TAG_DT', 32);
define ('TIDY_TAG_EM', 33);
define ('TIDY_TAG_EMBED', 34);
define ('TIDY_TAG_FIELDSET', 35);
define ('TIDY_TAG_FONT', 36);
define ('TIDY_TAG_FORM', 37);
define ('TIDY_TAG_FRAME', 38);
define ('TIDY_TAG_FRAMESET', 39);
define ('TIDY_TAG_H1', 40);
define ('TIDY_TAG_H2', 41);
define ('TIDY_TAG_H3', 42);
define ('TIDY_TAG_H4', 43);
define ('TIDY_TAG_H5', 44);
define ('TIDY_TAG_H6', 45);
define ('TIDY_TAG_HEAD', 46);
define ('TIDY_TAG_HR', 47);
define ('TIDY_TAG_HTML', 48);
define ('TIDY_TAG_I', 49);
define ('TIDY_TAG_IFRAME', 50);
define ('TIDY_TAG_ILAYER', 51);
define ('TIDY_TAG_IMG', 52);
define ('TIDY_TAG_INPUT', 53);
define ('TIDY_TAG_INS', 54);
define ('TIDY_TAG_ISINDEX', 55);
define ('TIDY_TAG_KBD', 56);
define ('TIDY_TAG_KEYGEN', 57);
define ('TIDY_TAG_LABEL', 58);
define ('TIDY_TAG_LAYER', 59);
define ('TIDY_TAG_LEGEND', 60);
define ('TIDY_TAG_LI', 61);
define ('TIDY_TAG_LINK', 62);
define ('TIDY_TAG_LISTING', 63);
define ('TIDY_TAG_MAP', 64);
define ('TIDY_TAG_MARQUEE', 65);
define ('TIDY_TAG_MENU', 66);
define ('TIDY_TAG_META', 67);
define ('TIDY_TAG_MULTICOL', 68);
define ('TIDY_TAG_NOBR', 69);
define ('TIDY_TAG_NOEMBED', 70);
define ('TIDY_TAG_NOFRAMES', 71);
define ('TIDY_TAG_NOLAYER', 72);
define ('TIDY_TAG_NOSAVE', 73);
define ('TIDY_TAG_NOSCRIPT', 74);
define ('TIDY_TAG_OBJECT', 75);
define ('TIDY_TAG_OL', 76);
define ('TIDY_TAG_OPTGROUP', 77);
define ('TIDY_TAG_OPTION', 78);
define ('TIDY_TAG_P', 79);
define ('TIDY_TAG_PARAM', 80);
define ('TIDY_TAG_PLAINTEXT', 81);
define ('TIDY_TAG_PRE', 82);
define ('TIDY_TAG_Q', 83);
define ('TIDY_TAG_RB', 84);
define ('TIDY_TAG_RBC', 85);
define ('TIDY_TAG_RP', 86);
define ('TIDY_TAG_RT', 87);
define ('TIDY_TAG_RTC', 88);
define ('TIDY_TAG_RUBY', 89);
define ('TIDY_TAG_S', 90);
define ('TIDY_TAG_SAMP', 91);
define ('TIDY_TAG_SCRIPT', 92);
define ('TIDY_TAG_SELECT', 93);
define ('TIDY_TAG_SERVER', 94);
define ('TIDY_TAG_SERVLET', 95);
define ('TIDY_TAG_SMALL', 96);
define ('TIDY_TAG_SPACER', 97);
define ('TIDY_TAG_SPAN', 98);
define ('TIDY_TAG_STRIKE', 99);
define ('TIDY_TAG_STRONG', 100);
define ('TIDY_TAG_STYLE', 101);
define ('TIDY_TAG_SUB', 102);
define ('TIDY_TAG_SUP', 103);
define ('TIDY_TAG_TABLE', 104);
define ('TIDY_TAG_TBODY', 105);
define ('TIDY_TAG_TD', 106);
define ('TIDY_TAG_TEXTAREA', 107);
define ('TIDY_TAG_TFOOT', 108);
define ('TIDY_TAG_TH', 109);
define ('TIDY_TAG_THEAD', 110);
define ('TIDY_TAG_TITLE', 111);
define ('TIDY_TAG_TR', 112);
define ('TIDY_TAG_TT', 113);
define ('TIDY_TAG_U', 114);
define ('TIDY_TAG_UL', 115);
define ('TIDY_TAG_VAR', 116);
define ('TIDY_TAG_WBR', 117);
define ('TIDY_TAG_XMP', 118);

/**
 * root node
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_ROOT', 0);

/**
 * doctype
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_DOCTYPE', 1);

/**
 * HTML comment
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_COMMENT', 2);

/**
 * Processing Instruction
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_PROCINS', 3);

/**
 * Text
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_TEXT', 4);

/**
 * start tag
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_START', 5);

/**
 * end tag
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_END', 6);

/**
 * empty tag
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_STARTEND', 7);

/**
 * CDATA
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_CDATA', 8);

/**
 * XML section
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_SECTION', 9);

/**
 * ASP code
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_ASP', 10);

/**
 * JSTE code
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_JSTE', 11);

/**
 * PHP code
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_PHP', 12);

/**
 * XML declaration
 * @link http://www.php.net/manual/en/tidy.constants.php
 */
define ('TIDY_NODETYPE_XMLDECL', 13);

// End of tidy v.2.0
?>
