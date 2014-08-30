<?php

// Start of xmlreader v.0.1

class XMLReader  {
	const NONE = 0;
	const ELEMENT = 1;
	const ATTRIBUTE = 2;
	const TEXT = 3;
	const CDATA = 4;
	const ENTITY_REF = 5;
	const ENTITY = 6;
	const PI = 7;
	const COMMENT = 8;
	const DOC = 9;
	const DOC_TYPE = 10;
	const DOC_FRAGMENT = 11;
	const NOTATION = 12;
	const WHITESPACE = 13;
	const SIGNIFICANT_WHITESPACE = 14;
	const END_ELEMENT = 15;
	const END_ENTITY = 16;
	const XML_DECLARATION = 17;
	const LOADDTD = 1;
	const DEFAULTATTRS = 2;
	const VALIDATE = 3;
	const SUBST_ENTITIES = 4;


	/**
	 * Close the XMLReader input
	 * @link http://www.php.net/manual/en/xmlreader.close.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function close () {}

	/**
	 * Get the value of a named attribute
	 * @link http://www.php.net/manual/en/xmlreader.getattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return string The value of the attribute, or &null; if no attribute with the
	 *   given name is found or not positioned of element.
	 */
	public function getAttribute ($name) {}

	/**
	 * Get the value of an attribute by index
	 * @link http://www.php.net/manual/en/xmlreader.getattributeno.php
	 * @param index int <p>
	 *       The position of the attribute.
	 *      </p>
	 * @return string The value of the attribute, or an empty string if no attribute exists 
	 *   at index or not positioned of element.
	 */
	public function getAttributeNo ($index) {}

	/**
	 * Get the value of an attribute by localname and URI
	 * @link http://www.php.net/manual/en/xmlreader.getattributens.php
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The value of the attribute, or an empty string if no attribute with the
	 *   given localName and 
	 *   namespaceURI is found or not positioned of element.
	 */
	public function getAttributeNs ($localName, $namespaceURI) {}

	/**
	 * Indicates if specified property has been set
	 * @link http://www.php.net/manual/en/xmlreader.getparserproperty.php
	 * @param property int <p>
	 *       One of the parser option 
	 *       constants.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function getParserProperty ($property) {}

	/**
	 * Indicates if the parsed document is valid
	 * @link http://www.php.net/manual/en/xmlreader.isvalid.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function isValid () {}

	/**
	 * Lookup namespace for a prefix
	 * @link http://www.php.net/manual/en/xmlreader.lookupnamespace.php
	 * @param prefix string <p>
	 *       String containing the prefix.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function lookupNamespace ($prefix) {}

	/**
	 * Move cursor to an attribute by index
	 * @link http://www.php.net/manual/en/xmlreader.movetoattributeno.php
	 * @param index int <p>
	 *       The position of the attribute.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function moveToAttributeNo ($index) {}

	/**
	 * Move cursor to a named attribute
	 * @link http://www.php.net/manual/en/xmlreader.movetoattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function moveToAttribute ($name) {}

	/**
	 * Move cursor to a named attribute
	 * @link http://www.php.net/manual/en/xmlreader.movetoattributens.php
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function moveToAttributeNs ($localName, $namespaceURI) {}

	/**
	 * Position cursor on the parent Element of current Attribute
	 * @link http://www.php.net/manual/en/xmlreader.movetoelement.php
	 * @return bool true if successful and false if it fails or not positioned on 
	 *   Attribute when this method is called.
	 */
	public function moveToElement () {}

	/**
	 * Position cursor on the first Attribute
	 * @link http://www.php.net/manual/en/xmlreader.movetofirstattribute.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function moveToFirstAttribute () {}

	/**
	 * Position cursor on the next Attribute
	 * @link http://www.php.net/manual/en/xmlreader.movetonextattribute.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function moveToNextAttribute () {}

	/**
	 * Set the URI containing the XML to parse
	 * @link http://www.php.net/manual/en/xmlreader.open.php
	 * @param URI string <p>
	 *       URI pointing to the document.
	 *      </p>
	 * @param encoding string[optional] <p>
	 *       The document encoding or &null;.
	 *      </p>
	 * @param options int[optional] <p>
	 *       A bitmask of the LIBXML_* 
	 *       constants.
	 *      </p>
	 * @return bool Returns true on success or false on failure. If called statically, returns an
	 *   XMLReader&return.falseforfailure;.
	 */
	public function open ($URI, $encoding = null, $options = null) {}

	/**
	 * Move to next node in document
	 * @link http://www.php.net/manual/en/xmlreader.read.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function read () {}

	/**
	 * Move cursor to next node skipping all subtrees
	 * @link http://www.php.net/manual/en/xmlreader.next.php
	 * @param localname string[optional] <p>
	 *       The name of the next node to move to.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function next ($localname = null) {}

	/**
	 * Retrieve XML from current node
	 * @link http://www.php.net/manual/en/xmlreader.readinnerxml.php
	 * @return string the contents of the current node as a string. Empty string on failure.
	 */
	public function readInnerXml () {}

	/**
	 * Retrieve XML from current node, including it self
	 * @link http://www.php.net/manual/en/xmlreader.readouterxml.php
	 * @return string the contents of current node, including itself, as a string. Empty string on failure.
	 */
	public function readOuterXml () {}

	/**
	 * Reads the contents of the current node as a string
	 * @link http://www.php.net/manual/en/xmlreader.readstring.php
	 * @return string the content of the current node as a string. Empty string on
	 *   failure.
	 */
	public function readString () {}

	/**
	 * Validate document against XSD
	 * @link http://www.php.net/manual/en/xmlreader.setschema.php
	 * @param filename string <p>
	 *       The filename of the XSD schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setSchema ($filename) {}

	/**
	 * Set or Unset parser options
	 * @link http://www.php.net/manual/en/xmlreader.setparserproperty.php
	 * @param property int <p>
	 *       One of the parser option 
	 *       constants.
	 *      </p>
	 * @param value bool <p>
	 *       If set to true the option will be enabled otherwise will 
	 *       be disabled.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setParserProperty ($property, $value) {}

	/**
	 * Set the filename or URI for a RelaxNG Schema
	 * @link http://www.php.net/manual/en/xmlreader.setrelaxngschema.php
	 * @param filename string <p>
	 *       filename or URI pointing to a RelaxNG Schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setRelaxNGSchema ($filename) {}

	/**
	 * Set the data containing a RelaxNG Schema
	 * @link http://www.php.net/manual/en/xmlreader.setrelaxngschemasource.php
	 * @param source string <p>
	 *       String containing the RelaxNG Schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setRelaxNGSchemaSource ($source) {}

	/**
	 * Set the data containing the XML to parse
	 * @link http://www.php.net/manual/en/xmlreader.xml.php
	 * @param source string <p>
	 *       String containing the XML to be parsed.
	 *      </p>
	 * @param encoding string[optional] <p>
	 *       The document encoding or &null;.
	 *      </p>
	 * @param options int[optional] <p>
	 *       A bitmask of the LIBXML_* 
	 *       constants.
	 *      </p>
	 * @return bool Returns true on success or false on failure. If called statically, returns an
	 *   XMLReader&return.falseforfailure;.
	 */
	public function XML ($source, $encoding = null, $options = null) {}

	/**
	 * Returns a copy of the current node as a DOM object
	 * @link http://www.php.net/manual/en/xmlreader.expand.php
	 * @param basenode DOMNode[optional] 
	 * @return DOMNode The resulting DOMNode or false on error.
	 */
	public function expand (DOMNode $basenode = null) {}

}
// End of xmlreader v.0.1
?>
