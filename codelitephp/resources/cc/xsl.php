<?php

// Start of xsl v.0.1

class XSLTProcessor  {

	/**
	 * @param doc
	 */
	public function importStylesheet ($doc) {}

	/**
	 * @param doc
	 */
	public function transformToDoc ($doc) {}

	/**
	 * @param doc
	 * @param uri
	 */
	public function transformToUri ($doc, $uri) {}

	/**
	 * @param doc
	 */
	public function transformToXml ($doc) {}

	/**
	 * @param namespace
	 * @param name
	 * @param value[optional]
	 */
	public function setParameter ($namespace, $name, $value) {}

	/**
	 * @param namespace
	 * @param name
	 */
	public function getParameter ($namespace, $name) {}

	/**
	 * @param namespace
	 * @param name
	 */
	public function removeParameter ($namespace, $name) {}

	public function hasExsltSupport () {}

	/**
	 * @param restrict[optional]
	 */
	public function registerPHPFunctions ($restrict) {}

	/**
	 * @param filename
	 */
	public function setProfiling ($filename) {}

	/**
	 * @param securityPrefs
	 */
	public function setSecurityPrefs ($securityPrefs) {}

	public function getSecurityPrefs () {}

}
define ('XSL_CLONE_AUTO', 0);
define ('XSL_CLONE_NEVER', -1);
define ('XSL_CLONE_ALWAYS', 1);
define ('XSL_SECPREF_NONE', 0);
define ('XSL_SECPREF_READ_FILE', 2);
define ('XSL_SECPREF_WRITE_FILE', 4);
define ('XSL_SECPREF_CREATE_DIRECTORY', 8);
define ('XSL_SECPREF_READ_NETWORK', 16);
define ('XSL_SECPREF_WRITE_NETWORK', 32);
define ('XSL_SECPREF_DEFAULT', 44);

/**
 * libxslt version like 10117. Available as of PHP 5.1.2.
 * @link http://www.php.net/manual/en/xsl.constants.php
 */
define ('LIBXSLT_VERSION', 10127);

/**
 * libxslt version like 1.1.17. Available as of PHP 5.1.2.
 * @link http://www.php.net/manual/en/xsl.constants.php
 */
define ('LIBXSLT_DOTTED_VERSION', "1.1.27");

/**
 * libexslt version like 813. Available as of PHP 5.1.2.
 * @link http://www.php.net/manual/en/xsl.constants.php
 */
define ('LIBEXSLT_VERSION', 816);

/**
 * libexslt version like 1.1.17. Available as of PHP 5.1.2.
 * @link http://www.php.net/manual/en/xsl.constants.php
 */
define ('LIBEXSLT_DOTTED_VERSION', "1.1.27");

// End of xsl v.0.1
?>
