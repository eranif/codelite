<?php

// Start of dom v.20031129

class DOMException extends Exception  {
	protected $message;
	protected $file;
	protected $line;
	public $code;


	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

class DOMStringList  {

	/**
	 * @param index
	 */
	public function item ($index) {}

}

class DOMNameList  {

	/**
	 * @param index
	 */
	public function getName ($index) {}

	/**
	 * @param index
	 */
	public function getNamespaceURI ($index) {}

}

class DOMImplementationList  {

	/**
	 * @param index
	 */
	public function item ($index) {}

}

class DOMImplementationSource  {

	/**
	 * @param features
	 */
	public function getDomimplementation ($features) {}

	/**
	 * @param features
	 */
	public function getDomimplementations ($features) {}

}

class DOMImplementation  {

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	public function hasFeature () {}

	/**
	 * @param qualifiedName
	 * @param publicId
	 * @param systemId
	 */
	public function createDocumentType ($qualifiedName, $publicId, $systemId) {}

	/**
	 * @param namespaceURI
	 * @param qualifiedName
	 * @param docType DOMDocumentType
	 */
	public function createDocument ($namespaceURI, $qualifiedNameDOMDocumentType , $docType) {}

}

class DOMNode  {

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMNameSpaceNode  {
}

class DOMDocumentFragment extends DOMNode  {

	public function __construct () {}

	/**
	 * @param data
	 */
	public function appendXML ($data) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMDocument extends DOMNode  {

	/**
	 * @param tagName
	 * @param value[optional]
	 */
	public function createElement ($tagName, $value) {}

	public function createDocumentFragment () {}

	/**
	 * @param data
	 */
	public function createTextNode ($data) {}

	/**
	 * @param data
	 */
	public function createComment ($data) {}

	/**
	 * @param data
	 */
	public function createCDATASection ($data) {}

	/**
	 * @param target
	 * @param data
	 */
	public function createProcessingInstruction ($target, $data) {}

	/**
	 * @param name
	 */
	public function createAttribute ($name) {}

	/**
	 * @param name
	 */
	public function createEntityReference ($name) {}

	/**
	 * @param tagName
	 */
	public function getElementsByTagName ($tagName) {}

	/**
	 * @param importedNode DOMNode
	 * @param deep
	 */
	public function importNode (DOMNode $importedNode, $deep) {}

	/**
	 * @param namespaceURI
	 * @param qualifiedName
	 * @param value[optional]
	 */
	public function createElementNS ($namespaceURI, $qualifiedName, $value) {}

	/**
	 * @param namespaceURI
	 * @param qualifiedName
	 */
	public function createAttributeNS ($namespaceURI, $qualifiedName) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function getElementsByTagNameNS ($namespaceURI, $localName) {}

	/**
	 * @param elementId
	 */
	public function getElementById ($elementId) {}

	/**
	 * @param source DOMNode
	 */
	public function adoptNode (DOMNode $source) {}

	public function normalizeDocument () {}

	/**
	 * @param node DOMNode
	 * @param namespaceURI
	 * @param qualifiedName
	 */
	public function renameNode (DOMNode $node, $namespaceURI, $qualifiedName) {}

	/**
	 * @param source
	 * @param options[optional]
	 */
	public function load ($source, $options) {}

	/**
	 * @param file
	 */
	public function save ($file) {}

	/**
	 * @param source
	 * @param options[optional]
	 */
	public function loadXML ($source, $options) {}

	/**
	 * @param node DOMNode[optional]
	 */
	public function saveXML (DOMNode $node = null) {}

	/**
	 * @param version[optional]
	 * @param encoding[optional]
	 */
	public function __construct ($version, $encoding) {}

	public function validate () {}

	/**
	 * @param options[optional]
	 */
	public function xinclude ($options) {}

	/**
	 * @param source
	 * @param options[optional]
	 */
	public function loadHTML ($source, $options) {}

	/**
	 * @param source
	 * @param options[optional]
	 */
	public function loadHTMLFile ($source, $options) {}

	public function saveHTML () {}

	/**
	 * @param file
	 */
	public function saveHTMLFile ($file) {}

	/**
	 * @param filename
	 */
	public function schemaValidate ($filename) {}

	/**
	 * @param source
	 */
	public function schemaValidateSource ($source) {}

	/**
	 * @param filename
	 */
	public function relaxNGValidate ($filename) {}

	/**
	 * @param source
	 */
	public function relaxNGValidateSource ($source) {}

	/**
	 * @param baseClass
	 * @param extendedClass
	 */
	public function registerNodeClass ($baseClass, $extendedClass) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMNodeList implements Traversable {

	/**
	 * @param index
	 */
	public function item ($index) {}

}

class DOMNamedNodeMap implements Traversable {

	/**
	 * @param name
	 */
	public function getNamedItem ($name) {}

	/**
	 * @param arg DOMNode
	 */
	public function setNamedItem (DOMNode $arg) {}

	/**
	 * @param name[optional]
	 */
	public function removeNamedItem ($name) {}

	/**
	 * @param index[optional]
	 */
	public function item ($index) {}

	/**
	 * @param namespaceURI[optional]
	 * @param localName[optional]
	 */
	public function getNamedItemNS ($namespaceURI, $localName) {}

	/**
	 * @param arg DOMNode[optional]
	 */
	public function setNamedItemNS (DOMNode $arg) {}

	/**
	 * @param namespaceURI[optional]
	 * @param localName[optional]
	 */
	public function removeNamedItemNS ($namespaceURI, $localName) {}

}

class DOMCharacterData extends DOMNode  {

	/**
	 * @param offset
	 * @param count
	 */
	public function substringData ($offset, $count) {}

	/**
	 * @param arg
	 */
	public function appendData ($arg) {}

	/**
	 * @param offset
	 * @param arg
	 */
	public function insertData ($offset, $arg) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * @param offset
	 * @param count
	 * @param arg
	 */
	public function replaceData ($offset, $count, $arg) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMAttr extends DOMNode  {

	public function isId () {}

	/**
	 * @param name
	 * @param value[optional]
	 */
	public function __construct ($name, $value) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMElement extends DOMNode  {

	/**
	 * @param name
	 */
	public function getAttribute ($name) {}

	/**
	 * @param name
	 * @param value
	 */
	public function setAttribute ($name, $value) {}

	/**
	 * @param name
	 */
	public function removeAttribute ($name) {}

	/**
	 * @param name
	 */
	public function getAttributeNode ($name) {}

	/**
	 * @param newAttr DOMAttr
	 */
	public function setAttributeNode (DOMAttr $newAttr) {}

	/**
	 * @param oldAttr DOMAttr
	 */
	public function removeAttributeNode (DOMAttr $oldAttr) {}

	/**
	 * @param name
	 */
	public function getElementsByTagName ($name) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function getAttributeNS ($namespaceURI, $localName) {}

	/**
	 * @param namespaceURI
	 * @param qualifiedName
	 * @param value
	 */
	public function setAttributeNS ($namespaceURI, $qualifiedName, $value) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function removeAttributeNS ($namespaceURI, $localName) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function getAttributeNodeNS ($namespaceURI, $localName) {}

	/**
	 * @param newAttr DOMAttr
	 */
	public function setAttributeNodeNS (DOMAttr $newAttr) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function getElementsByTagNameNS ($namespaceURI, $localName) {}

	/**
	 * @param name
	 */
	public function hasAttribute ($name) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 */
	public function hasAttributeNS ($namespaceURI, $localName) {}

	/**
	 * @param name
	 * @param isId
	 */
	public function setIdAttribute ($name, $isId) {}

	/**
	 * @param namespaceURI
	 * @param localName
	 * @param isId
	 */
	public function setIdAttributeNS ($namespaceURI, $localName, $isId) {}

	/**
	 * @param attr DOMAttr
	 * @param isId
	 */
	public function setIdAttributeNode (DOMAttr $attr, $isId) {}

	/**
	 * @param name
	 * @param value[optional]
	 * @param uri[optional]
	 */
	public function __construct ($name, $value, $uri) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMText extends DOMCharacterData  {

	/**
	 * @param offset
	 */
	public function splitText ($offset) {}

	public function isWhitespaceInElementContent () {}

	public function isElementContentWhitespace () {}

	/**
	 * @param content
	 */
	public function replaceWholeText ($content) {}

	/**
	 * @param value[optional]
	 */
	public function __construct ($value) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function substringData ($offset, $count) {}

	/**
	 * @param arg
	 */
	public function appendData ($arg) {}

	/**
	 * @param offset
	 * @param arg
	 */
	public function insertData ($offset, $arg) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * @param offset
	 * @param count
	 * @param arg
	 */
	public function replaceData ($offset, $count, $arg) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMComment extends DOMCharacterData  {

	/**
	 * @param value[optional]
	 */
	public function __construct ($value) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function substringData ($offset, $count) {}

	/**
	 * @param arg
	 */
	public function appendData ($arg) {}

	/**
	 * @param offset
	 * @param arg
	 */
	public function insertData ($offset, $arg) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * @param offset
	 * @param count
	 * @param arg
	 */
	public function replaceData ($offset, $count, $arg) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMTypeinfo  {
}

class DOMUserDataHandler  {

	public function handle () {}

}

class DOMDomError  {
}

class DOMErrorHandler  {

	/**
	 * @param error DOMDomError
	 */
	public function handleError (DOMDomError $error) {}

}

class DOMLocator  {
}

class DOMConfiguration  {

	/**
	 * @param name
	 * @param value
	 */
	public function setParameter ($name, $value) {}

	/**
	 * @param name[optional]
	 */
	public function getParameter ($name) {}

	/**
	 * @param name[optional]
	 * @param value[optional]
	 */
	public function canSetParameter ($name, $value) {}

}

class DOMCdataSection extends DOMText  {

	/**
	 * @param value
	 */
	public function __construct ($value) {}

	/**
	 * @param offset
	 */
	public function splitText ($offset) {}

	public function isWhitespaceInElementContent () {}

	public function isElementContentWhitespace () {}

	/**
	 * @param content
	 */
	public function replaceWholeText ($content) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function substringData ($offset, $count) {}

	/**
	 * @param arg
	 */
	public function appendData ($arg) {}

	/**
	 * @param offset
	 * @param arg
	 */
	public function insertData ($offset, $arg) {}

	/**
	 * @param offset
	 * @param count
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * @param offset
	 * @param count
	 * @param arg
	 */
	public function replaceData ($offset, $count, $arg) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMDocumentType extends DOMNode  {

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMNotation extends DOMNode  {

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMEntity extends DOMNode  {

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMEntityReference extends DOMNode  {

	/**
	 * @param name
	 */
	public function __construct ($name) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMProcessingInstruction extends DOMNode  {

	/**
	 * @param name
	 * @param value[optional]
	 */
	public function __construct ($name, $value) {}

	/**
	 * @param newChild DOMNode
	 * @param refChild DOMNode[optional]
	 */
	public function insertBefore (DOMNode $newChildDOMNode , $refChild = null) {}

	/**
	 * @param newChild DOMNode
	 * @param oldChild DOMNode
	 */
	public function replaceChild (DOMNode $newChildDOMNode , $oldChild) {}

	/**
	 * @param oldChild DOMNode
	 */
	public function removeChild (DOMNode $oldChild) {}

	/**
	 * @param newChild DOMNode
	 */
	public function appendChild (DOMNode $newChild) {}

	public function hasChildNodes () {}

	/**
	 * @param deep
	 */
	public function cloneNode ($deep) {}

	public function normalize () {}

	/**
	 * @param feature
	 * @param version
	 */
	public function isSupported ($feature, $version) {}

	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * @param other DOMNode
	 */
	public function isSameNode (DOMNode $other) {}

	/**
	 * @param namespaceURI
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * @param namespaceURI
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * @param prefix
	 */
	public function lookupNamespaceUri ($prefix) {}

	/**
	 * @param arg DOMNode
	 */
	public function isEqualNode (DOMNode $arg) {}

	/**
	 * @param feature
	 * @param version
	 */
	public function getFeature ($feature, $version) {}

	/**
	 * @param key
	 * @param data
	 * @param handler
	 */
	public function setUserData ($key, $data, $handler) {}

	/**
	 * @param key
	 */
	public function getUserData ($key) {}

	public function getNodePath () {}

	public function getLineNo () {}

	/**
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14N ($exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

	/**
	 * @param uri
	 * @param exclusive[optional]
	 * @param with_comments[optional]
	 * @param xpath[optional]
	 * @param ns_prefixes[optional]
	 */
	public function C14NFile ($uri, $exclusive, $with_commentsarray , $xpath = nullarray , $ns_prefixes = null) {}

}

class DOMStringExtend  {

	/**
	 * @param offset32
	 */
	public function findOffset16 ($offset32) {}

	/**
	 * @param offset16
	 */
	public function findOffset32 ($offset16) {}

}

class DOMXPath  {

	/**
	 * @param doc DOMDocument
	 */
	public function __construct (DOMDocument $doc) {}

	/**
	 * @param prefix
	 * @param uri
	 */
	public function registerNamespace ($prefix, $uri) {}

	/**
	 * @param expr
	 * @param context DOMNode[optional]
	 * @param registerNodeNS[optional]
	 */
	public function query ($exprDOMNode , $context = null, $registerNodeNS) {}

	/**
	 * @param expr
	 * @param context DOMNode[optional]
	 * @param registerNodeNS[optional]
	 */
	public function evaluate ($exprDOMNode , $context = null, $registerNodeNS) {}

	public function registerPhpFunctions () {}

}

/**
 * @param node
 */
function dom_import_simplexml ($node) {}


/**
 * Node is a DOMElement
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_ELEMENT_NODE', 1);

/**
 * Node is a DOMAttr
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_ATTRIBUTE_NODE', 2);

/**
 * Node is a DOMText
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_TEXT_NODE', 3);

/**
 * Node is a DOMCharacterData
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_CDATA_SECTION_NODE', 4);

/**
 * Node is a DOMEntityReference
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_ENTITY_REF_NODE', 5);

/**
 * Node is a DOMEntity
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_ENTITY_NODE', 6);

/**
 * Node is a DOMProcessingInstruction
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_PI_NODE', 7);

/**
 * Node is a DOMComment
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_COMMENT_NODE', 8);

/**
 * Node is a DOMDocument
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_DOCUMENT_NODE', 9);

/**
 * Node is a DOMDocumentType
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_DOCUMENT_TYPE_NODE', 10);

/**
 * Node is a DOMDocumentFragment
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_DOCUMENT_FRAG_NODE', 11);

/**
 * Node is a DOMNotation
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('XML_NOTATION_NODE', 12);
define ('XML_HTML_DOCUMENT_NODE', 13);
define ('XML_DTD_NODE', 14);
define ('XML_ELEMENT_DECL_NODE', 15);
define ('XML_ATTRIBUTE_DECL_NODE', 16);
define ('XML_ENTITY_DECL_NODE', 17);
define ('XML_NAMESPACE_DECL_NODE', 18);
define ('XML_LOCAL_NAMESPACE', 18);
define ('XML_ATTRIBUTE_CDATA', 1);
define ('XML_ATTRIBUTE_ID', 2);
define ('XML_ATTRIBUTE_IDREF', 3);
define ('XML_ATTRIBUTE_IDREFS', 4);
define ('XML_ATTRIBUTE_ENTITY', 6);
define ('XML_ATTRIBUTE_NMTOKEN', 7);
define ('XML_ATTRIBUTE_NMTOKENS', 8);
define ('XML_ATTRIBUTE_ENUMERATION', 9);
define ('XML_ATTRIBUTE_NOTATION', 10);

/**
 * Error code not part of the DOM specification. Meant for PHP errors.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_PHP_ERR', 0);

/**
 * If index or size is negative, or greater than the allowed value.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INDEX_SIZE_ERR', 1);

/**
 * If the specified range of text does not fit into a 
 *       DOMString.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOMSTRING_SIZE_ERR', 2);

/**
 * If any node is inserted somewhere it doesn't belong
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_HIERARCHY_REQUEST_ERR', 3);

/**
 * If a node is used in a different document than the one that created it.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_WRONG_DOCUMENT_ERR', 4);

/**
 * If an invalid or illegal character is specified, such as in a name.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INVALID_CHARACTER_ERR', 5);

/**
 * If data is specified for a node which does not support data.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_NO_DATA_ALLOWED_ERR', 6);

/**
 * If an attempt is made to modify an object where modifications are not allowed.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_NO_MODIFICATION_ALLOWED_ERR', 7);

/**
 * If an attempt is made to reference a node in a context where it does not exist.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_NOT_FOUND_ERR', 8);

/**
 * If the implementation does not support the requested type of object or operation.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_NOT_SUPPORTED_ERR', 9);

/**
 * If an attempt is made to add an attribute that is already in use elsewhere.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INUSE_ATTRIBUTE_ERR', 10);

/**
 * If an attempt is made to use an object that is not, or is no longer, usable.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INVALID_STATE_ERR', 11);

/**
 * If an invalid or illegal string is specified.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_SYNTAX_ERR', 12);

/**
 * If an attempt is made to modify the type of the underlying object.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INVALID_MODIFICATION_ERR', 13);

/**
 * If an attempt is made to create or change an object in a way which is 
 *       incorrect with regard to namespaces.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_NAMESPACE_ERR', 14);

/**
 * If a parameter or an operation is not supported by the underlying object.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_INVALID_ACCESS_ERR', 15);

/**
 * If a call to a method such as insertBefore or removeChild would make the Node
 *       invalid with respect to "partial validity", this exception would be raised and 
 *       the operation would not be done.
 * @link http://www.php.net/manual/en/dom.constants.php
 */
define ('DOM_VALIDATION_ERR', 16);

// End of dom v.20031129
?>
