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


	public function close () {}

	/**
	 * @param name
	 */
	public function getAttribute ($name) {}

	/**
	 * @param index
	 */
	public function getAttributeNo ($index) {}

	/**
	 * @param name
	 * @param namespaceURI
	 */
	public function getAttributeNs ($name, $namespaceURI) {}

	/**
	 * @param property
	 */
	public function getParserProperty ($property) {}

	public function isValid () {}

	/**
	 * @param prefix
	 */
	public function lookupNamespace ($prefix) {}

	/**
	 * @param index
	 */
	public function moveToAttributeNo ($index) {}

	/**
	 * @param name
	 */
	public function moveToAttribute ($name) {}

	/**
	 * @param name
	 * @param namespaceURI
	 */
	public function moveToAttributeNs ($name, $namespaceURI) {}

	public function moveToElement () {}

	public function moveToFirstAttribute () {}

	public function moveToNextAttribute () {}

	/**
	 * @param URI
	 * @param encoding[optional]
	 * @param options[optional]
	 */
	public function open ($URI, $encoding, $options) {}

	public function read () {}

	/**
	 * @param localname[optional]
	 */
	public function next ($localname) {}

	public function readInnerXml () {}

	public function readOuterXml () {}

	public function readString () {}

	/**
	 * @param filename
	 */
	public function setSchema ($filename) {}

	/**
	 * @param property
	 * @param value
	 */
	public function setParserProperty ($property, $value) {}

	/**
	 * @param filename
	 */
	public function setRelaxNGSchema ($filename) {}

	/**
	 * @param source
	 */
	public function setRelaxNGSchemaSource ($source) {}

	/**
	 * @param source
	 * @param encoding[optional]
	 * @param options[optional]
	 */
	public function XML ($source, $encoding, $options) {}

	public function expand () {}

}
// End of xmlreader v.0.1
?>
