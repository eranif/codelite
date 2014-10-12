<?php

// Start of xmlwriter v.0.1

class XMLWriter  {

	/**
	 * @param uri
	 */
	public function openUri ($uri) {}

	public function openMemory () {}

	/**
	 * @param indent
	 */
	public function setIndent ($indent) {}

	/**
	 * @param indentString
	 */
	public function setIndentString ($indentString) {}

	public function startComment () {}

	public function endComment () {}

	/**
	 * @param name
	 */
	public function startAttribute ($name) {}

	public function endAttribute () {}

	/**
	 * @param name
	 * @param value
	 */
	public function writeAttribute ($name, $value) {}

	/**
	 * @param prefix
	 * @param name
	 * @param uri
	 */
	public function startAttributeNs ($prefix, $name, $uri) {}

	/**
	 * @param prefix
	 * @param name
	 * @param uri
	 * @param content
	 */
	public function writeAttributeNs ($prefix, $name, $uri, $content) {}

	/**
	 * @param name
	 */
	public function startElement ($name) {}

	public function endElement () {}

	public function fullEndElement () {}

	/**
	 * @param prefix
	 * @param name
	 * @param uri
	 */
	public function startElementNs ($prefix, $name, $uri) {}

	/**
	 * @param name
	 * @param content[optional]
	 */
	public function writeElement ($name, $content) {}

	/**
	 * @param prefix
	 * @param name
	 * @param uri
	 * @param content[optional]
	 */
	public function writeElementNs ($prefix, $name, $uri, $content) {}

	/**
	 * @param target
	 */
	public function startPi ($target) {}

	public function endPi () {}

	/**
	 * @param target
	 * @param content
	 */
	public function writePi ($target, $content) {}

	public function startCdata () {}

	public function endCdata () {}

	/**
	 * @param content
	 */
	public function writeCdata ($content) {}

	/**
	 * @param content
	 */
	public function text ($content) {}

	/**
	 * @param content
	 */
	public function writeRaw ($content) {}

	/**
	 * @param version[optional]
	 * @param encoding[optional]
	 * @param standalone[optional]
	 */
	public function startDocument ($version, $encoding, $standalone) {}

	public function endDocument () {}

	/**
	 * @param content
	 */
	public function writeComment ($content) {}

	/**
	 * @param qualifiedName
	 * @param publicId[optional]
	 * @param systemId[optional]
	 */
	public function startDtd ($qualifiedName, $publicId, $systemId) {}

	public function endDtd () {}

	/**
	 * @param name
	 * @param publicId[optional]
	 * @param systemId[optional]
	 * @param subset[optional]
	 */
	public function writeDtd ($name, $publicId, $systemId, $subset) {}

	/**
	 * @param qualifiedName
	 */
	public function startDtdElement ($qualifiedName) {}

	public function endDtdElement () {}

	/**
	 * @param name
	 * @param content
	 */
	public function writeDtdElement ($name, $content) {}

	/**
	 * @param name
	 */
	public function startDtdAttlist ($name) {}

	public function endDtdAttlist () {}

	/**
	 * @param name
	 * @param content
	 */
	public function writeDtdAttlist ($name, $content) {}

	/**
	 * @param name
	 * @param isparam
	 */
	public function startDtdEntity ($name, $isparam) {}

	public function endDtdEntity () {}

	/**
	 * @param name
	 * @param content
	 */
	public function writeDtdEntity ($name, $content) {}

	/**
	 * @param flush[optional]
	 */
	public function outputMemory ($flush) {}

	/**
	 * @param empty[optional]
	 */
	public function flush ($empty) {}

}

/**
 * @param uri
 */
function xmlwriter_open_uri ($uri) {}

function xmlwriter_open_memory () {}

/**
 * @param xmlwriter
 * @param indent
 */
function xmlwriter_set_indent ($xmlwriter, $indent) {}

/**
 * @param xmlwriter
 * @param indentString
 */
function xmlwriter_set_indent_string ($xmlwriter, $indentString) {}

/**
 * @param xmlwriter
 */
function xmlwriter_start_comment ($xmlwriter) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_comment ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 */
function xmlwriter_start_attribute ($xmlwriter, $name) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_attribute ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 * @param value
 */
function xmlwriter_write_attribute ($xmlwriter, $name, $value) {}

/**
 * @param xmlwriter
 * @param prefix
 * @param name
 * @param uri
 */
function xmlwriter_start_attribute_ns ($xmlwriter, $prefix, $name, $uri) {}

/**
 * @param xmlwriter
 * @param prefix
 * @param name
 * @param uri
 * @param content
 */
function xmlwriter_write_attribute_ns ($xmlwriter, $prefix, $name, $uri, $content) {}

/**
 * @param xmlwriter
 * @param name
 */
function xmlwriter_start_element ($xmlwriter, $name) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_element ($xmlwriter) {}

/**
 * @param xmlwriter
 */
function xmlwriter_full_end_element ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param prefix
 * @param name
 * @param uri
 */
function xmlwriter_start_element_ns ($xmlwriter, $prefix, $name, $uri) {}

/**
 * @param xmlwriter
 * @param name
 * @param content[optional]
 */
function xmlwriter_write_element ($xmlwriter, $name, $content) {}

/**
 * @param xmlwriter
 * @param prefix
 * @param name
 * @param uri
 * @param content[optional]
 */
function xmlwriter_write_element_ns ($xmlwriter, $prefix, $name, $uri, $content) {}

/**
 * @param xmlwriter
 * @param target
 */
function xmlwriter_start_pi ($xmlwriter, $target) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_pi ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param target
 * @param content
 */
function xmlwriter_write_pi ($xmlwriter, $target, $content) {}

/**
 * @param xmlwriter
 */
function xmlwriter_start_cdata ($xmlwriter) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_cdata ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param content
 */
function xmlwriter_write_cdata ($xmlwriter, $content) {}

/**
 * @param xmlwriter
 * @param content
 */
function xmlwriter_text ($xmlwriter, $content) {}

/**
 * @param xmlwriter
 * @param content
 */
function xmlwriter_write_raw ($xmlwriter, $content) {}

/**
 * @param xmlwriter
 * @param version[optional]
 * @param encoding[optional]
 * @param standalone[optional]
 */
function xmlwriter_start_document ($xmlwriter, $version, $encoding, $standalone) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_document ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param content
 */
function xmlwriter_write_comment ($xmlwriter, $content) {}

/**
 * @param xmlwriter
 * @param qualifiedName
 * @param publicId[optional]
 * @param systemId[optional]
 */
function xmlwriter_start_dtd ($xmlwriter, $qualifiedName, $publicId, $systemId) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_dtd ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 * @param publicId[optional]
 * @param systemId[optional]
 * @param subset[optional]
 */
function xmlwriter_write_dtd ($xmlwriter, $name, $publicId, $systemId, $subset) {}

/**
 * @param xmlwriter
 * @param qualifiedName
 */
function xmlwriter_start_dtd_element ($xmlwriter, $qualifiedName) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_dtd_element ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 * @param content
 */
function xmlwriter_write_dtd_element ($xmlwriter, $name, $content) {}

/**
 * @param xmlwriter
 * @param name
 */
function xmlwriter_start_dtd_attlist ($xmlwriter, $name) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_dtd_attlist ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 * @param content
 */
function xmlwriter_write_dtd_attlist ($xmlwriter, $name, $content) {}

/**
 * @param xmlwriter
 * @param name
 * @param isparam
 */
function xmlwriter_start_dtd_entity ($xmlwriter, $name, $isparam) {}

/**
 * @param xmlwriter
 */
function xmlwriter_end_dtd_entity ($xmlwriter) {}

/**
 * @param xmlwriter
 * @param name
 * @param content
 */
function xmlwriter_write_dtd_entity ($xmlwriter, $name, $content) {}

/**
 * @param xmlwriter
 * @param flush[optional]
 */
function xmlwriter_output_memory ($xmlwriter, $flush) {}

/**
 * @param xmlwriter
 * @param empty[optional]
 */
function xmlwriter_flush ($xmlwriter, $empty) {}

// End of xmlwriter v.0.1
?>
