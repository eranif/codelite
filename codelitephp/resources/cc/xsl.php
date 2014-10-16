<?php

// Start of xsl v.0.1

class XSLTProcessor  {

	/**
	 * Import stylesheet
	 * @link http://www.php.net/manual/en/xsltprocessor.importstylesheet.php
	 * @param stylesheet object <p>
	 *       The imported style sheet as a DOMDocument or
	 *       SimpleXMLElement object.
	 *      </p>
	 * @return void 
	 */
	public function importStylesheet ($stylesheet) {}

	/**
	 * Transform to a DOMDocument
	 * @link http://www.php.net/manual/en/xsltprocessor.transformtodoc.php
	 * @param doc DOMNode <p>
	 *       The node to be transformed.
	 *      </p>
	 * @return DOMDocument The resulting DOMDocument or false on error.
	 */
	public function transformToDoc (DOMNode $doc) {}

	/**
	 * Transform to URI
	 * @link http://www.php.net/manual/en/xsltprocessor.transformtouri.php
	 * @param doc DOMDocument <p>
	 *       The document to transform.
	 *      </p>
	 * @param uri string <p>
	 *        The target URI for the transformation.
	 *      </p>
	 * @return int the number of bytes written or false if an error occurred.
	 */
	public function transformToUri (DOMDocument $doc, $uri) {}

	/**
	 * Transform to XML
	 * @link http://www.php.net/manual/en/xsltprocessor.transformtoxml.php
	 * @param doc DOMDocument <p>
	 *       The transformed document.
	 *      </p>
	 * @return string The result of the transformation as a string or false on error.
	 */
	public function transformToXml (DOMDocument $doc) {}

	/**
	 * Set value for a parameter
	 * @link http://www.php.net/manual/en/xsltprocessor.setparameter.php
	 * @param namespace string <p>
	 *       The namespace URI of the XSLT parameter.
	 *      </p>
	 * @param name string <p>
	 *       The local name of the XSLT parameter.
	 *      </p>
	 * @param value string <p>
	 *       The new value of the XSLT parameter.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function setParameter ($namespace, $name, $value) {}

	/**
	 * Get value of a parameter
	 * @link http://www.php.net/manual/en/xsltprocessor.getparameter.php
	 * @param namespaceURI string <p>
	 *       The namespace URI of the XSLT parameter.
	 *      </p>
	 * @param localName string <p>
	 *       The local name of the XSLT parameter.
	 *      </p>
	 * @return string The value of the parameter (as a string), or false if it's not set.
	 */
	public function getParameter ($namespaceURI, $localName) {}

	/**
	 * Remove parameter
	 * @link http://www.php.net/manual/en/xsltprocessor.removeparameter.php
	 * @param namespaceURI string <p>
	 *       The namespace URI of the XSLT parameter.
	 *      </p>
	 * @param localName string <p>
	 *       The local name of the XSLT parameter.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function removeParameter ($namespaceURI, $localName) {}

	/**
	 * Determine if PHP has EXSLT support
	 * @link http://www.php.net/manual/en/xsltprocessor.hasexsltsupport.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function hasExsltSupport () {}

	/**
	 * Enables the ability to use PHP functions as XSLT functions
	 * @link http://www.php.net/manual/en/xsltprocessor.registerphpfunctions.php
	 * @param restrict mixed[optional] <p>
	 *       Use this parameter to only allow certain functions to be called from 
	 *       XSLT.
	 *      </p>
	 *      <p>
	 *       This parameter can be either a string (a function name) or an array of
	 *       functions.
	 *      </p>
	 * @return void 
	 */
	public function registerPHPFunctions ($restrict = null) {}

	/**
	 * Sets profiling output file
	 * @link http://www.php.net/manual/en/xsltprocessor.setprofiling.php
	 * @param filename string <p>
	 *       Path to the file to dump profiling information.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function setProfiling ($filename) {}

	/**
	 * Set security preferences
	 * @link http://www.php.net/manual/en/xsltprocessor.setsecurityprefs.php
	 * @param securityPrefs int 
	 * @return int 
	 */
	public function setSecurityPrefs ($securityPrefs) {}

	/**
	 * Get security preferences
	 * @link http://www.php.net/manual/en/xsltprocessor.getsecurityprefs.php
	 * @return int 
	 */
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
