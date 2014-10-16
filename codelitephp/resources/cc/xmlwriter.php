<?php

// Start of xmlwriter v.0.1

class XMLWriter  {

	/**
	 * Create new xmlwriter using source uri for output
	 * @link http://www.php.net/manual/en/function.xmlwriter-open-uri.php
	 * @param uri string <p>
	 *       The URI of the resource for the output.
	 *      </p>
	 * @return bool &style.oop;: Returns true on success, false on failure.
	 *  </p>
	 *  <p>
	 *   &style.procedural;: Returns a new xmlwriter &resource; for later use with the
	 *   xmlwriter functions on success, false on error.
	 */
	public function openUri ($uri) {}

	/**
	 * Create new xmlwriter using memory for string output
	 * @link http://www.php.net/manual/en/function.xmlwriter-open-memory.php
	 * @return bool &style.oop;: Returns true on success, false on failure.
	 *  </p>
	 *  <p>
	 *   &style.procedural;: Returns a new xmlwriter &resource; for later use with the
	 *   xmlwriter functions on success, false on error.
	 */
	public function openMemory () {}

	/**
	 * Toggle indentation on/off
	 * @link http://www.php.net/manual/en/function.xmlwriter-set-indent.php
	 * @param indent bool <p>
	 *       Whether indentation is enabled.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function setIndent ($indent) {}

	/**
	 * Set string used for indenting
	 * @link http://www.php.net/manual/en/function.xmlwriter-set-indent-string.php
	 * @param indentString string <p>
	 *       The indentation string.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function setIndentString ($indentString) {}

	/**
	 * Create start comment
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-comment.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function startComment () {}

	/**
	 * Create end comment
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-comment.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endComment () {}

	/**
	 * Create start attribute
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-attribute.php
	 * @param name string <p>
	 *       The attribute name.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startAttribute ($name) {}

	/**
	 * End attribute
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-attribute.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endAttribute () {}

	/**
	 * Write full attribute
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-attribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @param value string <p>
	 *       The value of the attribute.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeAttribute ($name, $value) {}

	/**
	 * Create start namespaced attribute
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-attribute-ns.php
	 * @param prefix string <p>
	 *       The namespace prefix.
	 *      </p>
	 * @param name string <p>
	 *       The attribute name.
	 *      </p>
	 * @param uri string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startAttributeNs ($prefix, $name, $uri) {}

	/**
	 * Write full namespaced attribute
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-attribute-ns.php
	 * @param prefix string <p>
	 *       The namespace prefix.
	 *      </p>
	 * @param name string <p>
	 *       The attribute name.
	 *      </p>
	 * @param uri string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param content string <p>
	 *       The attribute value.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeAttributeNs ($prefix, $name, $uri, $content) {}

	/**
	 * Create start element tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-element.php
	 * @param name string <p>
	 *       The element name.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startElement ($name) {}

	/**
	 * End current element
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-element.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endElement () {}

	/**
	 * End current element
	 * @link http://www.php.net/manual/en/function.xmlwriter-full-end-element.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function fullEndElement () {}

	/**
	 * Create start namespaced element tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-element-ns.php
	 * @param prefix string <p>
	 *       The namespace prefix.
	 *      </p>
	 * @param name string <p>
	 *       The element name.
	 *      </p>
	 * @param uri string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startElementNs ($prefix, $name, $uri) {}

	/**
	 * Write full element tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-element.php
	 * @param name string <p>
	 *       The element name.
	 *      </p>
	 * @param content string[optional] <p>
	 *       The element contents.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeElement ($name, $content = null) {}

	/**
	 * Write full namespaced element tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-element-ns.php
	 * @param prefix string <p>
	 *       The namespace prefix.
	 *      </p>
	 * @param name string <p>
	 *       The element name.
	 *      </p>
	 * @param uri string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param content string[optional] <p>
	 *       The element contents.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeElementNs ($prefix, $name, $uri, $content = null) {}

	/**
	 * Create start PI tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-pi.php
	 * @param target string <p>
	 *       The target of the processing instruction.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startPi ($target) {}

	/**
	 * End current PI
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-pi.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endPi () {}

	/**
	 * Writes a PI
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-pi.php
	 * @param target string <p>
	 *       The target of the processing instruction.
	 *      </p>
	 * @param content string <p>
	 *       The content of the processing instruction.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writePi ($target, $content) {}

	/**
	 * Create start CDATA tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-cdata.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function startCdata () {}

	/**
	 * End current CDATA
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-cdata.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endCdata () {}

	/**
	 * Write full CDATA tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-cdata.php
	 * @param content string <p>
	 *       The contents of the CDATA.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeCdata ($content) {}

	/**
	 * Write text
	 * @link http://www.php.net/manual/en/function.xmlwriter-text.php
	 * @param content string <p>
	 *       The contents of the text.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function text ($content) {}

	/**
	 * Write a raw XML text
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-raw.php
	 * @param content string <p>
	 *       The text string to write.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeRaw ($content) {}

	/**
	 * Create document tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-document.php
	 * @param version string[optional] <p>
	 *       The version number of the document as part of the XML declaration. 
	 *      </p>
	 * @param encoding string[optional] <p>
	 *       The encoding of the document as part of the XML declaration.
	 *      </p>
	 * @param standalone string[optional] <p>
	 *       yes or no.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startDocument ($version = null, $encoding = null, $standalone = null) {}

	/**
	 * End current document
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-document.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endDocument () {}

	/**
	 * Write full comment tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-comment.php
	 * @param content string <p>
	 *       The contents of the comment.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeComment ($content) {}

	/**
	 * Create start DTD tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-dtd.php
	 * @param qualifiedName string <p>
	 *       The qualified name of the document type to create.
	 *      </p>
	 * @param publicId string[optional] <p>
	 *       The external subset public identifier.
	 *      </p>
	 * @param systemId string[optional] <p>
	 *       The external subset system identifier.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startDtd ($qualifiedName, $publicId = null, $systemId = null) {}

	/**
	 * End current DTD
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-dtd.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endDtd () {}

	/**
	 * Write full DTD tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-dtd.php
	 * @param name string <p>
	 *       The DTD name.
	 *      </p>
	 * @param publicId string[optional] <p>
	 *       The external subset public identifier.
	 *      </p>
	 * @param systemId string[optional] <p>
	 *       The external subset system identifier.
	 *      </p>
	 * @param subset string[optional] <p>
	 *       The content of the DTD.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeDtd ($name, $publicId = null, $systemId = null, $subset = null) {}

	/**
	 * Create start DTD element
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-dtd-element.php
	 * @param qualifiedName string <p>
	 *       The qualified name of the document type to create.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startDtdElement ($qualifiedName) {}

	/**
	 * End current DTD element
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-dtd-element.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endDtdElement () {}

	/**
	 * Write full DTD element tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-dtd-element.php
	 * @param name string <p>
	 *       The name of the DTD element.
	 *      </p>
	 * @param content string <p>
	 *       The content of the element.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeDtdElement ($name, $content) {}

	/**
	 * Create start DTD AttList
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-dtd-attlist.php
	 * @param name string <p>
	 *       The attribute list name.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startDtdAttlist ($name) {}

	/**
	 * End current DTD AttList
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-dtd-attlist.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endDtdAttlist () {}

	/**
	 * Write full DTD AttList tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-dtd-attlist.php
	 * @param name string <p>
	 *       The name of the DTD attribute list.
	 *      </p>
	 * @param content string <p>
	 *       The content of the DTD attribute list.
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeDtdAttlist ($name, $content) {}

	/**
	 * Create start DTD Entity
	 * @link http://www.php.net/manual/en/function.xmlwriter-start-dtd-entity.php
	 * @param name string <p>
	 *       The name of the entity.
	 *      </p>
	 * @param isparam bool <p>
	 *      </p>
	 * @return bool Returns true on success, false on failure.
	 */
	public function startDtdEntity ($name, $isparam) {}

	/**
	 * End current DTD Entity
	 * @link http://www.php.net/manual/en/function.xmlwriter-end-dtd-entity.php
	 * @return bool Returns true on success, false on failure.
	 */
	public function endDtdEntity () {}

	/**
	 * Write full DTD Entity tag
	 * @link http://www.php.net/manual/en/function.xmlwriter-write-dtd-entity.php
	 * @param name string <p>
	 *       The name of the entity.
	 *      </p>
	 * @param content string <p>
	 *       The content of the entity.
	 *      </p>
	 * @param pe bool 
	 * @param pubid string 
	 * @param sysid string 
	 * @param ndataid string 
	 * @return bool Returns true on success, false on failure.
	 */
	public function writeDtdEntity ($name, $content, $pe, $pubid, $sysid, $ndataid) {}

	/**
	 * Returns current buffer
	 * @link http://www.php.net/manual/en/function.xmlwriter-output-memory.php
	 * @param flush bool[optional] <p>
	 *       Whether to flush the output buffer or not. Default is true.
	 *      </p>
	 * @return string the current buffer as a string.
	 */
	public function outputMemory ($flush = null) {}

	/**
	 * Flush current buffer
	 * @link http://www.php.net/manual/en/function.xmlwriter-flush.php
	 * @param empty bool[optional] <p>
	 *       Whether to empty the buffer or not. Default is true.
	 *      </p>
	 * @return mixed If you opened the writer in memory, this function returns the generated XML buffer,
	 *   Else, if using URI, this function will write the buffer and return the number of 
	 *   written bytes.
	 */
	public function flush ($empty = null) {}

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
