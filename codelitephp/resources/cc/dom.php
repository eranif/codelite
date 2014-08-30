<?php

// Start of dom v.20031129

class DOMException extends Exception  {
	protected $message;
	public $code;
	protected $file;
	protected $line;


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

	/**
	 * Test if the DOM implementation implements a specific feature
	 * @link http://www.php.net/manual/en/domimplementation.hasfeature.php
	 * @param feature string <p>
	 *       The feature to test.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test. In 
	 *       level 2, this can be either 2.0 or
	 *       1.0.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasFeature ($feature, $version) {}

	/**
	 * Creates an empty DOMDocumentType object
	 * @link http://www.php.net/manual/en/domimplementation.createdocumenttype.php
	 * @param qualifiedName string[optional] <p>
	 *       The qualified name of the document type to create.
	 *      </p>
	 * @param publicId string[optional] <p>
	 *       The external subset public identifier.
	 *      </p>
	 * @param systemId string[optional] <p>
	 *       The external subset system identifier.
	 *      </p>
	 * @return DOMDocumentType A new DOMDocumentType node with its 
	 *   ownerDocument set to &null;.
	 */
	public function createDocumentType ($qualifiedName = null, $publicId = null, $systemId = null) {}

	/**
	 * Creates a DOMDocument object of the specified type with its document element
	 * @link http://www.php.net/manual/en/domimplementation.createdocument.php
	 * @param namespaceURI string[optional] <p>
	 *       The namespace URI of the document element to create.
	 *      </p>
	 * @param qualifiedName string[optional] <p>
	 *       The qualified name of the document element to create.
	 *      </p>
	 * @param doctype DOMDocumentType[optional] <p>
	 *       The type of document to create or &null;.
	 *      </p>
	 * @return DOMDocument A new DOMDocument object. If 
	 *   namespaceURI, qualifiedName, 
	 *   and doctype are null, the returned 
	 *   DOMDocument is empty with no document element
	 */
	public function createDocument ($namespaceURI = null, $qualifiedName = null, DOMDocumentType $doctype = null) {}

}

class DOMNode  {

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Append raw XML data
	 * @link http://www.php.net/manual/en/domdocumentfragment.appendxml.php
	 * @param data string <p>
	 *       XML to append.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function appendXML ($data) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Create new element node
	 * @link http://www.php.net/manual/en/domdocument.createelement.php
	 * @param name string <p>
	 *       The tag name of the element.
	 *      </p>
	 * @param value string[optional] <p>
	 *       The value of the element. By default, an empty element will be created.
	 *       The value can also be set later with DOMElement->nodeValue.
	 *      </p>
	 * @return DOMElement a new instance of class DOMElement or false
	 *   if an error occured.
	 */
	public function createElement ($name, $value = null) {}

	/**
	 * Create new document fragment
	 * @link http://www.php.net/manual/en/domdocument.createdocumentfragment.php
	 * @return DOMDocumentFragment The new DOMDocumentFragment or false if an error occured.
	 */
	public function createDocumentFragment () {}

	/**
	 * Create new text node
	 * @link http://www.php.net/manual/en/domdocument.createtextnode.php
	 * @param content string <p>
	 *       The content of the text.
	 *      </p>
	 * @return DOMText The new DOMText or false if an error occured.
	 */
	public function createTextNode ($content) {}

	/**
	 * Create new comment node
	 * @link http://www.php.net/manual/en/domdocument.createcomment.php
	 * @param data string <p>
	 *       The content of the comment.
	 *      </p>
	 * @return DOMComment The new DOMComment or false if an error occured.
	 */
	public function createComment ($data) {}

	/**
	 * Create new cdata node
	 * @link http://www.php.net/manual/en/domdocument.createcdatasection.php
	 * @param data string <p>
	 *       The content of the cdata.
	 *      </p>
	 * @return DOMCDATASection The new DOMCDATASection or false if an error occured.
	 */
	public function createCDATASection ($data) {}

	/**
	 * Creates new PI node
	 * @link http://www.php.net/manual/en/domdocument.createprocessinginstruction.php
	 * @param target string <p>
	 *       The target of the processing instruction.
	 *      </p>
	 * @param data string[optional] <p>
	 *       The content of the processing instruction.
	 *      </p>
	 * @return DOMProcessingInstruction The new DOMProcessingInstruction or false if an error occured.
	 */
	public function createProcessingInstruction ($target, $data = null) {}

	/**
	 * Create new attribute
	 * @link http://www.php.net/manual/en/domdocument.createattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return DOMAttr The new DOMAttr or false if an error occured.
	 */
	public function createAttribute ($name) {}

	/**
	 * Create new entity reference node
	 * @link http://www.php.net/manual/en/domdocument.createentityreference.php
	 * @param name string <p>
	 *       The content of the entity reference, e.g. the entity reference minus
	 *       the leading & and the trailing
	 *       ; characters.
	 *      </p>
	 * @return DOMEntityReference The new DOMEntityReference or false if an error
	 *   occured.
	 */
	public function createEntityReference ($name) {}

	/**
	 * Searches for all elements with given tag name
	 * @link http://www.php.net/manual/en/domdocument.getelementsbytagname.php
	 * @param name string <p>
	 *       The name of the tag to match on. The special value *
	 *       matches all tags.
	 *      </p>
	 * @return DOMNodeList A new DOMNodeList object containing all the matched 
	 *   elements.
	 */
	public function getElementsByTagName ($name) {}

	/**
	 * Import node into current document
	 * @link http://www.php.net/manual/en/domdocument.importnode.php
	 * @param importedNode DOMNode <p>
	 *       The node to import.
	 *      </p>
	 * @param deep bool[optional] <p>
	 *       If set to true, this method will recursively import the subtree under
	 *       the importedNode.
	 *      </p>
	 *      
	 *       <p>
	 *        To copy the nodes attributes deep needs to be set to true
	 *       </p>
	 * @return DOMNode The copied node or false, if it cannot be copied.
	 */
	public function importNode (DOMNode $importedNode, $deep = null) {}

	/**
	 * Create new element node with an associated namespace
	 * @link http://www.php.net/manual/en/domdocument.createelementns.php
	 * @param namespaceURI string <p>
	 *       The URI of the namespace.
	 *      </p>
	 * @param qualifiedName string <p>
	 *       The qualified name of the element, as prefix:tagname.
	 *      </p>
	 * @param value string[optional] <p>
	 *       The value of the element. By default, an empty element will be created.
	 *       You can also set the value later with DOMElement->nodeValue.
	 *      </p>
	 * @return DOMElement The new DOMElement or false if an error occured.
	 */
	public function createElementNS ($namespaceURI, $qualifiedName, $value = null) {}

	/**
	 * Create new attribute node with an associated namespace
	 * @link http://www.php.net/manual/en/domdocument.createattributens.php
	 * @param namespaceURI string <p>
	 *       The URI of the namespace.
	 *      </p>
	 * @param qualifiedName string <p>
	 *       The tag name and prefix of the attribute, as prefix:tagname.
	 *      </p>
	 * @return DOMAttr The new DOMAttr or false if an error occured.
	 */
	public function createAttributeNS ($namespaceURI, $qualifiedName) {}

	/**
	 * Searches for all elements with given tag name in specified namespace
	 * @link http://www.php.net/manual/en/domdocument.getelementsbytagnamens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI of the elements to match on. 
	 *       The special value * matches all namespaces.
	 *      </p>
	 * @param localName string <p>
	 *       The local name of the elements to match on. 
	 *       The special value * matches all local names.
	 *      </p>
	 * @return DOMNodeList A new DOMNodeList object containing all the matched 
	 *   elements.
	 */
	public function getElementsByTagNameNS ($namespaceURI, $localName) {}

	/**
	 * Searches for an element with a certain id
	 * @link http://www.php.net/manual/en/domdocument.getelementbyid.php
	 * @param elementId string <p>
	 *       The unique id value for an element.
	 *      </p>
	 * @return DOMElement the DOMElement or &null; if the element is
	 *   not found.
	 */
	public function getElementById ($elementId) {}

	/**
	 * @param source DOMNode
	 */
	public function adoptNode (DOMNode $source) {}

	/**
	 * Normalizes the document
	 * @link http://www.php.net/manual/en/domdocument.normalizedocument.php
	 * @return void 
	 */
	public function normalizeDocument () {}

	/**
	 * @param node DOMNode
	 * @param namespaceURI
	 * @param qualifiedName
	 */
	public function renameNode (DOMNode $node, $namespaceURI, $qualifiedName) {}

	/**
	 * Load XML from a file
	 * @link http://www.php.net/manual/en/domdocument.load.php
	 * @param filename string <p>
	 *       The path to the XML document.
	 *      </p>
	 * @param options int[optional] <p>
	 *       Bitwise OR
	 *       of the libxml option constants.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.  If called statically, returns a
	 *   DOMDocument&return.falseforfailure;.
	 */
	public function load ($filename, $options = null) {}

	/**
	 * Dumps the internal XML tree back into a file
	 * @link http://www.php.net/manual/en/domdocument.save.php
	 * @param filename string <p>
	 *       The path to the saved XML document.
	 *      </p>
	 * @param options int[optional] <p>
	 *       Additional Options. Currently only LIBXML_NOEMPTYTAG is supported.
	 *      </p>
	 * @return int the number of bytes written or false if an error occurred.
	 */
	public function save ($filename, $options = null) {}

	/**
	 * Load XML from a string
	 * @link http://www.php.net/manual/en/domdocument.loadxml.php
	 * @param source string <p>
	 *       The string containing the XML.
	 *      </p>
	 * @param options int[optional] <p>
	 *       Bitwise OR
	 *       of the libxml option constants.
	 *      </p>
	 * @return mixed Returns true on success or false on failure. If called statically, returns a
	 *   DOMDocument&return.falseforfailure;.
	 */
	public function loadXML ($source, $options = null) {}

	/**
	 * Dumps the internal XML tree back into a string
	 * @link http://www.php.net/manual/en/domdocument.savexml.php
	 * @param node DOMNode[optional] <p>
	 *       Use this parameter to output only a specific node without XML declaration
	 *       rather than the entire document.
	 *      </p>
	 * @param options int[optional] <p>
	 *       Additional Options. Currently only LIBXML_NOEMPTYTAG is supported.
	 *      </p>
	 * @return string the XML, or false if an error occurred.
	 */
	public function saveXML (DOMNode $node = null, $options = null) {}

	/**
	 * Creates a new DOMDocument object
	 * @link http://www.php.net/manual/en/domdocument.construct.php
	 * @param version[optional]
	 * @param encoding[optional]
	 */
	public function __construct ($version, $encoding) {}

	/**
	 * Validates the document based on its DTD
	 * @link http://www.php.net/manual/en/domdocument.validate.php
	 * @return bool Returns true on success or false on failure.
	 *   If the document have no DTD attached, this method will return false.
	 */
	public function validate () {}

	/**
	 * Substitutes XIncludes in a DomDocument Object
	 * @link http://www.php.net/manual/en/function.domdocument-xinclude.php
	 * @param options int[optional] <p>
	 *       libxml parameters. Available
	 *       since PHP 5.1.0 and Libxml 2.6.7.
	 *      </p>
	 * @return int the number of XIncludes in the document, -1 if some processing failed,
	 *   or false if there were no substitutions.
	 */
	public function xinclude ($options = null) {}

	/**
	 * Load HTML from a string
	 * @link http://www.php.net/manual/en/domdocument.loadhtml.php
	 * @param source string <p>
	 *       The HTML string.
	 *      </p>
	 * @return bool Returns true on success or false on failure. If called statically, returns a
	 *   DOMDocument&return.falseforfailure;.
	 */
	public function loadHTML ($source) {}

	/**
	 * Load HTML from a file
	 * @link http://www.php.net/manual/en/domdocument.loadhtmlfile.php
	 * @param filename string <p>
	 *       The path to the HTML file.
	 *      </p>
	 * @return bool Returns true on success or false on failure. If called statically, returns a
	 *   DOMDocument&return.falseforfailure;.
	 */
	public function loadHTMLFile ($filename) {}

	/**
	 * Dumps the internal document into a string using HTML formatting
	 * @link http://www.php.net/manual/en/domdocument.savehtml.php
	 * @param node DOMNode[optional] <p>
	 *       Optional parameter to output a subset of the document.
	 *      </p>
	 * @return string the HTML, or false if an error occurred.
	 */
	public function saveHTML (DOMNode $node = null) {}

	/**
	 * Dumps the internal document into a file using HTML formatting
	 * @link http://www.php.net/manual/en/domdocument.savehtmlfile.php
	 * @param filename string <p>
	 *       The path to the saved HTML document.
	 *      </p>
	 * @return int the number of bytes written or false if an error occurred.
	 */
	public function saveHTMLFile ($filename) {}

	/**
	 * Validates a document based on a schema
	 * @link http://www.php.net/manual/en/domdocument.schemavalidate.php
	 * @param filename string <p>
	 *       The path to the schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function schemaValidate ($filename) {}

	/**
	 * Validates a document based on a schema
	 * @link http://www.php.net/manual/en/domdocument.schemavalidatesource.php
	 * @param source string <p>
	 *       A string containing the schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function schemaValidateSource ($source) {}

	/**
	 * Performs relaxNG validation on the document
	 * @link http://www.php.net/manual/en/domdocument.relaxngvalidate.php
	 * @param filename string <p>
	 *       The RNG file.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function relaxNGValidate ($filename) {}

	/**
	 * Performs relaxNG validation on the document
	 * @link http://www.php.net/manual/en/domdocument.relaxngvalidatesource.php
	 * @param source string <p>
	 *       A string containing the RNG schema.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function relaxNGValidateSource ($source) {}

	/**
	 * Register extended class used to create base node type
	 * @link http://www.php.net/manual/en/domdocument.registernodeclass.php
	 * @param baseclass string <p>
	 *       The DOM class that you want to extend. You can find a list of these 
	 *       classes in the chapter introduction.
	 *      </p>
	 * @param extendedclass string <p>
	 *       Your extended class name. If &null; is provided, any previously 
	 *       registered class extending baseclass will
	 *       be removed.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function registerNodeClass ($baseclass, $extendedclass) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Retrieves a node specified by index
	 * @link http://www.php.net/manual/en/domnodelist.item.php
	 * @param index int <p>
	 *       Index of the node into the collection.
	 *      </p>
	 * @return DOMNode The node at the indexth position in the 
	 *   DOMNodeList, or &null; if that is not a valid
	 *   index.
	 */
	public function item ($index) {}

}

class DOMNamedNodeMap implements Traversable {

	/**
	 * Retrieves a node specified by name
	 * @link http://www.php.net/manual/en/domnamednodemap.getnameditem.php
	 * @param name string <p>
	 *       The nodeName of the node to retrieve.
	 *      </p>
	 * @return DOMNode A node (of any type) with the specified nodeName, or 
	 *   &null; if no node is found.
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
	 * Retrieves a node specified by index
	 * @link http://www.php.net/manual/en/domnamednodemap.item.php
	 * @param index int <p>
	 *       Index into this map.
	 *      </p>
	 * @return DOMNode The node at the indexth position in the map, or &null;
	 *   if that is not a valid index (greater than or equal to the number of nodes 
	 *   in this map).
	 */
	public function item ($index) {}

	/**
	 * Retrieves a node specified by local name and namespace URI
	 * @link http://www.php.net/manual/en/domnamednodemap.getnameditemns.php
	 * @param namespaceURI string <p>
	 *       The namespace URI of the node to retrieve.
	 *      </p>
	 * @param localName string <p>
	 *       The local name of the node to retrieve.
	 *      </p>
	 * @return DOMNode A node (of any type) with the specified local name and namespace URI, or 
	 *   &null; if no node is found.
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
	 * Extracts a range of data from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.substringdata.php
	 * @param offset int <p>
	 *       Start offset of substring to extract.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to extract.
	 *      </p>
	 * @return string The specified substring. If the sum of offset 
	 *   and count exceeds the length, then all 16-bit units 
	 *   to the end of the data are returned.
	 */
	public function substringData ($offset, $count) {}

	/**
	 * Append the string to the end of the character data of the node
	 * @link http://www.php.net/manual/en/domcharacterdata.appenddata.php
	 * @param data string <p>
	 *       The string to append.
	 *      </p>
	 * @return void 
	 */
	public function appendData ($data) {}

	/**
	 * Insert a string at the specified 16-bit unit offset
	 * @link http://www.php.net/manual/en/domcharacterdata.insertdata.php
	 * @param offset int <p>
	 *       The character offset at which to insert.
	 *      </p>
	 * @param data string <p>
	 *       The string to insert.
	 *      </p>
	 * @return void 
	 */
	public function insertData ($offset, $data) {}

	/**
	 * Remove a range of characters from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.deletedata.php
	 * @param offset int <p>
	 *       The offset from which to start removing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to delete. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are deleted.
	 *      </p>
	 * @return void 
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * Replace a substring within the DOMCharacterData node
	 * @link http://www.php.net/manual/en/domcharacterdata.replacedata.php
	 * @param offset int <p>
	 *       The offset from which to start replacing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to replace. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are replaced.
	 *      </p>
	 * @param data string <p>
	 *       The string with which the range must be replaced.
	 *      </p>
	 * @return void 
	 */
	public function replaceData ($offset, $count, $data) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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

	/**
	 * Checks if attribute is a defined ID
	 * @link http://www.php.net/manual/en/domattr.isid.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function isId () {}

	/**
	 * Creates a new <classname>DOMAttr</classname> object
	 * @link http://www.php.net/manual/en/domattr.construct.php
	 * @param name
	 * @param value[optional]
	 */
	public function __construct ($name, $value) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Returns value of attribute
	 * @link http://www.php.net/manual/en/domelement.getattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return string The value of the attribute, or an empty string if no attribute with the
	 *   given name is found.
	 */
	public function getAttribute ($name) {}

	/**
	 * Adds new attribute
	 * @link http://www.php.net/manual/en/domelement.setattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @param value string <p>
	 *       The value of the attribute.
	 *      </p>
	 * @return DOMAttr The new DOMAttr or false if an error occured.
	 */
	public function setAttribute ($name, $value) {}

	/**
	 * Removes attribute
	 * @link http://www.php.net/manual/en/domelement.removeattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function removeAttribute ($name) {}

	/**
	 * Returns attribute node
	 * @link http://www.php.net/manual/en/domelement.getattributenode.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @return DOMAttr The attribute node.
	 */
	public function getAttributeNode ($name) {}

	/**
	 * Adds new attribute node to element
	 * @link http://www.php.net/manual/en/domelement.setattributenode.php
	 * @param attr DOMAttr <p>
	 *       The attribute node.
	 *      </p>
	 * @return DOMAttr old node if the attribute has been replaced or &null;.
	 */
	public function setAttributeNode (DOMAttr $attr) {}

	/**
	 * Removes attribute
	 * @link http://www.php.net/manual/en/domelement.removeattributenode.php
	 * @param oldnode DOMAttr <p>
	 *       The attribute node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function removeAttributeNode (DOMAttr $oldnode) {}

	/**
	 * Gets elements by tagname
	 * @link http://www.php.net/manual/en/domelement.getelementsbytagname.php
	 * @param name string <p>
	 *       The tag name. Use * to return all elements within 
	 *       the element tree.
	 *      </p>
	 * @return DOMNodeList This function returns a new instance of the class
	 *   DOMNodeList of all matched elements.
	 */
	public function getElementsByTagName ($name) {}

	/**
	 * Returns value of attribute
	 * @link http://www.php.net/manual/en/domelement.getattributens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @return string The value of the attribute, or an empty string if no attribute with the
	 *   given localName and namespaceURI 
	 *   is found.
	 */
	public function getAttributeNS ($namespaceURI, $localName) {}

	/**
	 * Adds new attribute
	 * @link http://www.php.net/manual/en/domelement.setattributens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param qualifiedName string <p>
	 *       The qualified name of the attribute, as prefix:tagname.
	 *      </p>
	 * @param value string <p>
	 *       The value of the attribute.
	 *      </p>
	 * @return void 
	 */
	public function setAttributeNS ($namespaceURI, $qualifiedName, $value) {}

	/**
	 * Removes attribute
	 * @link http://www.php.net/manual/en/domelement.removeattributens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function removeAttributeNS ($namespaceURI, $localName) {}

	/**
	 * Returns attribute node
	 * @link http://www.php.net/manual/en/domelement.getattributenodens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @return DOMAttr The attribute node.
	 */
	public function getAttributeNodeNS ($namespaceURI, $localName) {}

	/**
	 * Adds new attribute node to element
	 * @link http://www.php.net/manual/en/domelement.setattributenodens.php
	 * @param attr DOMAttr <p>
	 *       The attribute node.
	 *      </p>
	 * @return DOMAttr the old node if the attribute has been replaced.
	 */
	public function setAttributeNodeNS (DOMAttr $attr) {}

	/**
	 * Get elements by namespaceURI and localName
	 * @link http://www.php.net/manual/en/domelement.getelementsbytagnamens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param localName string <p>
	 *       The local name. Use * to return all elements within 
	 *       the element tree.
	 *      </p>
	 * @return DOMNodeList This function returns a new instance of the class
	 *   DOMNodeList of all matched elements in the order in
	 *   which they are encountered in a preorder traversal of this element tree.
	 */
	public function getElementsByTagNameNS ($namespaceURI, $localName) {}

	/**
	 * Checks to see if attribute exists
	 * @link http://www.php.net/manual/en/domelement.hasattribute.php
	 * @param name string <p>
	 *       The attribute name.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttribute ($name) {}

	/**
	 * Checks to see if attribute exists
	 * @link http://www.php.net/manual/en/domelement.hasattributens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @param localName string <p>
	 *       The local name.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributeNS ($namespaceURI, $localName) {}

	/**
	 * Declares the attribute specified by name to be of type ID
	 * @link http://www.php.net/manual/en/domelement.setidattribute.php
	 * @param name string <p>
	 *       The name of the attribute.
	 *      </p>
	 * @param isId bool <p>
	 *       Set it to true if you want name to be of type
	 *       ID, false otherwise.
	 *      </p>
	 * @return void 
	 */
	public function setIdAttribute ($name, $isId) {}

	/**
	 * Declares the attribute specified by local name and namespace URI to be of type ID
	 * @link http://www.php.net/manual/en/domelement.setidattributens.php
	 * @param namespaceURI string <p>
	 *       The namespace URI of the attribute.
	 *      </p>
	 * @param localName string <p>
	 *       The local name of the attribute, as prefix:tagname.
	 *      </p>
	 * @param isId bool <p>
	 *       Set it to true if you want name to be of type
	 *       ID, false otherwise.
	 *      </p>
	 * @return void 
	 */
	public function setIdAttributeNS ($namespaceURI, $localName, $isId) {}

	/**
	 * Declares the attribute specified by node to be of type ID
	 * @link http://www.php.net/manual/en/domelement.setidattributenode.php
	 * @param attr DOMAttr <p>
	 *       The attribute node.
	 *      </p>
	 * @param isId bool <p>
	 *       Set it to true if you want name to be of type
	 *       ID, false otherwise.
	 *      </p>
	 * @return void 
	 */
	public function setIdAttributeNode (DOMAttr $attr, $isId) {}

	/**
	 * Creates a new DOMElement object
	 * @link http://www.php.net/manual/en/domelement.construct.php
	 * @param name
	 * @param value[optional]
	 * @param uri[optional]
	 */
	public function __construct ($name, $value, $uri) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Breaks this node into two nodes at the specified offset
	 * @link http://www.php.net/manual/en/domtext.splittext.php
	 * @param offset int <p>
	 *       The offset at which to split, starting from 0.
	 *      </p>
	 * @return DOMText The new node of the same type, which contains all the content at and after the 
	 *   offset.
	 */
	public function splitText ($offset) {}

	/**
	 * Indicates whether this text node contains whitespace
	 * @link http://www.php.net/manual/en/domtext.iswhitespaceinelementcontent.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function isWhitespaceInElementContent () {}

	public function isElementContentWhitespace () {}

	/**
	 * @param content
	 */
	public function replaceWholeText ($content) {}

	/**
	 * Creates a new <classname>DOMText</classname> object
	 * @link http://www.php.net/manual/en/domtext.construct.php
	 * @param value[optional]
	 */
	public function __construct ($value) {}

	/**
	 * Extracts a range of data from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.substringdata.php
	 * @param offset int <p>
	 *       Start offset of substring to extract.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to extract.
	 *      </p>
	 * @return string The specified substring. If the sum of offset 
	 *   and count exceeds the length, then all 16-bit units 
	 *   to the end of the data are returned.
	 */
	public function substringData ($offset, $count) {}

	/**
	 * Append the string to the end of the character data of the node
	 * @link http://www.php.net/manual/en/domcharacterdata.appenddata.php
	 * @param data string <p>
	 *       The string to append.
	 *      </p>
	 * @return void 
	 */
	public function appendData ($data) {}

	/**
	 * Insert a string at the specified 16-bit unit offset
	 * @link http://www.php.net/manual/en/domcharacterdata.insertdata.php
	 * @param offset int <p>
	 *       The character offset at which to insert.
	 *      </p>
	 * @param data string <p>
	 *       The string to insert.
	 *      </p>
	 * @return void 
	 */
	public function insertData ($offset, $data) {}

	/**
	 * Remove a range of characters from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.deletedata.php
	 * @param offset int <p>
	 *       The offset from which to start removing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to delete. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are deleted.
	 *      </p>
	 * @return void 
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * Replace a substring within the DOMCharacterData node
	 * @link http://www.php.net/manual/en/domcharacterdata.replacedata.php
	 * @param offset int <p>
	 *       The offset from which to start replacing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to replace. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are replaced.
	 *      </p>
	 * @param data string <p>
	 *       The string with which the range must be replaced.
	 *      </p>
	 * @return void 
	 */
	public function replaceData ($offset, $count, $data) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Creates a new DOMComment object
	 * @link http://www.php.net/manual/en/domcomment.construct.php
	 * @param value[optional]
	 */
	public function __construct ($value) {}

	/**
	 * Extracts a range of data from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.substringdata.php
	 * @param offset int <p>
	 *       Start offset of substring to extract.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to extract.
	 *      </p>
	 * @return string The specified substring. If the sum of offset 
	 *   and count exceeds the length, then all 16-bit units 
	 *   to the end of the data are returned.
	 */
	public function substringData ($offset, $count) {}

	/**
	 * Append the string to the end of the character data of the node
	 * @link http://www.php.net/manual/en/domcharacterdata.appenddata.php
	 * @param data string <p>
	 *       The string to append.
	 *      </p>
	 * @return void 
	 */
	public function appendData ($data) {}

	/**
	 * Insert a string at the specified 16-bit unit offset
	 * @link http://www.php.net/manual/en/domcharacterdata.insertdata.php
	 * @param offset int <p>
	 *       The character offset at which to insert.
	 *      </p>
	 * @param data string <p>
	 *       The string to insert.
	 *      </p>
	 * @return void 
	 */
	public function insertData ($offset, $data) {}

	/**
	 * Remove a range of characters from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.deletedata.php
	 * @param offset int <p>
	 *       The offset from which to start removing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to delete. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are deleted.
	 *      </p>
	 * @return void 
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * Replace a substring within the DOMCharacterData node
	 * @link http://www.php.net/manual/en/domcharacterdata.replacedata.php
	 * @param offset int <p>
	 *       The offset from which to start replacing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to replace. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are replaced.
	 *      </p>
	 * @param data string <p>
	 *       The string with which the range must be replaced.
	 *      </p>
	 * @return void 
	 */
	public function replaceData ($offset, $count, $data) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * @param error DOMError
	 */
	public function handleError (DOMError $error) {}

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
	 * Constructs a new DOMCdataSection object
	 * @link http://www.php.net/manual/en/domcdatasection.construct.php
	 * @param value
	 */
	public function __construct ($value) {}

	/**
	 * Breaks this node into two nodes at the specified offset
	 * @link http://www.php.net/manual/en/domtext.splittext.php
	 * @param offset int <p>
	 *       The offset at which to split, starting from 0.
	 *      </p>
	 * @return DOMText The new node of the same type, which contains all the content at and after the 
	 *   offset.
	 */
	public function splitText ($offset) {}

	/**
	 * Indicates whether this text node contains whitespace
	 * @link http://www.php.net/manual/en/domtext.iswhitespaceinelementcontent.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function isWhitespaceInElementContent () {}

	public function isElementContentWhitespace () {}

	/**
	 * @param content
	 */
	public function replaceWholeText ($content) {}

	/**
	 * Extracts a range of data from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.substringdata.php
	 * @param offset int <p>
	 *       Start offset of substring to extract.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to extract.
	 *      </p>
	 * @return string The specified substring. If the sum of offset 
	 *   and count exceeds the length, then all 16-bit units 
	 *   to the end of the data are returned.
	 */
	public function substringData ($offset, $count) {}

	/**
	 * Append the string to the end of the character data of the node
	 * @link http://www.php.net/manual/en/domcharacterdata.appenddata.php
	 * @param data string <p>
	 *       The string to append.
	 *      </p>
	 * @return void 
	 */
	public function appendData ($data) {}

	/**
	 * Insert a string at the specified 16-bit unit offset
	 * @link http://www.php.net/manual/en/domcharacterdata.insertdata.php
	 * @param offset int <p>
	 *       The character offset at which to insert.
	 *      </p>
	 * @param data string <p>
	 *       The string to insert.
	 *      </p>
	 * @return void 
	 */
	public function insertData ($offset, $data) {}

	/**
	 * Remove a range of characters from the node
	 * @link http://www.php.net/manual/en/domcharacterdata.deletedata.php
	 * @param offset int <p>
	 *       The offset from which to start removing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to delete. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are deleted.
	 *      </p>
	 * @return void 
	 */
	public function deleteData ($offset, $count) {}

	/**
	 * Replace a substring within the DOMCharacterData node
	 * @link http://www.php.net/manual/en/domcharacterdata.replacedata.php
	 * @param offset int <p>
	 *       The offset from which to start replacing.
	 *      </p>
	 * @param count int <p>
	 *       The number of characters to replace. If the sum of 
	 *       offset and count exceeds
	 *       the length, then all characters to the end of the data are replaced.
	 *      </p>
	 * @param data string <p>
	 *       The string with which the range must be replaced.
	 *      </p>
	 * @return void 
	 */
	public function replaceData ($offset, $count, $data) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Creates a new DOMEntityReference object
	 * @link http://www.php.net/manual/en/domentityreference.construct.php
	 * @param name
	 */
	public function __construct ($name) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Creates a new <classname>DOMProcessingInstruction</classname> object
	 * @link http://www.php.net/manual/en/domprocessinginstruction.construct.php
	 * @param name
	 * @param value[optional]
	 */
	public function __construct ($name, $value) {}

	/**
	 * Adds a new child before a reference node
	 * @link http://www.php.net/manual/en/domnode.insertbefore.php
	 * @param newnode DOMNode <p>
	 *       The new node.
	 *      </p>
	 * @param refnode DOMNode[optional] <p>
	 *       The reference node. If not supplied, newnode is
	 *       appended to the children.
	 *      </p>
	 * @return DOMNode The inserted node.
	 */
	public function insertBefore (DOMNode $newnode, DOMNode $refnode = null) {}

	/**
	 * Replaces a child
	 * @link http://www.php.net/manual/en/domnode.replacechild.php
	 * @param newnode DOMNode <p>
	 *       The new node. It must be a member of the target document, i.e.
	 *       created by one of the DOMDocument->createXXX() methods or imported in
	 *       the document by .
	 *      </p>
	 * @param oldnode DOMNode <p>
	 *       The old node.
	 *      </p>
	 * @return DOMNode The old node or false if an error occur.
	 */
	public function replaceChild (DOMNode $newnode, DOMNode $oldnode) {}

	/**
	 * Removes child from list of children
	 * @link http://www.php.net/manual/en/domnode.removechild.php
	 * @param oldnode DOMNode <p>
	 *       The removed child.
	 *      </p>
	 * @return DOMNode If the child could be removed the function returns the old child.
	 */
	public function removeChild (DOMNode $oldnode) {}

	/**
	 * Adds new child at the end of the children
	 * @link http://www.php.net/manual/en/domnode.appendchild.php
	 * @param newnode DOMNode <p>
	 *       The appended child.
	 *      </p>
	 * @return DOMNode The node added.
	 */
	public function appendChild (DOMNode $newnode) {}

	/**
	 * Checks if node has children
	 * @link http://www.php.net/manual/en/domnode.haschildnodes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasChildNodes () {}

	/**
	 * Clones a node
	 * @link http://www.php.net/manual/en/domnode.clonenode.php
	 * @param deep bool[optional] <p>
	 *       Indicates whether to copy all descendant nodes. This parameter is 
	 *       defaulted to false.
	 *      </p>
	 * @return DOMNode The cloned node.
	 */
	public function cloneNode ($deep = null) {}

	/**
	 * Normalizes the node
	 * @link http://www.php.net/manual/en/domnode.normalize.php
	 * @return void 
	 */
	public function normalize () {}

	/**
	 * Checks if feature is supported for specified version
	 * @link http://www.php.net/manual/en/domnode.issupported.php
	 * @param feature string <p>
	 *       The feature to test. See the example of 
	 *       DOMImplementation::hasFeature for a
	 *       list of features.
	 *      </p>
	 * @param version string <p>
	 *       The version number of the feature to test.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSupported ($feature, $version) {}

	/**
	 * Checks if node has attributes
	 * @link http://www.php.net/manual/en/domnode.hasattributes.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function hasAttributes () {}

	/**
	 * @param other DOMNode
	 */
	public function compareDocumentPosition (DOMNode $other) {}

	/**
	 * Indicates if two nodes are the same node
	 * @link http://www.php.net/manual/en/domnode.issamenode.php
	 * @param node DOMNode <p>
	 *       The compared node.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function isSameNode (DOMNode $node) {}

	/**
	 * Gets the namespace prefix of the node based on the namespace URI
	 * @link http://www.php.net/manual/en/domnode.lookupprefix.php
	 * @param namespaceURI string <p>
	 *       The namespace URI.
	 *      </p>
	 * @return string The prefix of the namespace.
	 */
	public function lookupPrefix ($namespaceURI) {}

	/**
	 * Checks if the specified namespaceURI is the default namespace or not
	 * @link http://www.php.net/manual/en/domnode.isdefaultnamespace.php
	 * @param namespaceURI string <p>
	 *       The namespace URI to look for.
	 *      </p>
	 * @return bool Return true if namespaceURI is the default
	 *   namespace, false otherwise.
	 */
	public function isDefaultNamespace ($namespaceURI) {}

	/**
	 * Gets the namespace URI of the node based on the prefix
	 * @link http://www.php.net/manual/en/domnode.lookupnamespaceuri.php
	 * @param prefix string <p>
	 *       The prefix of the namespace.
	 *      </p>
	 * @return string The namespace URI of the node.
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

	/**
	 * Get line number for a node
	 * @link http://www.php.net/manual/en/domnode.getlineno.php
	 * @return int Always returns the line number where the node was defined in.
	 */
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
	 * Creates a new <classname>DOMXPath</classname> object
	 * @link http://www.php.net/manual/en/domxpath.construct.php
	 * @param doc DOMDocument
	 */
	public function __construct (DOMDocument $doc) {}

	/**
	 * Registers the namespace with the <classname>DOMXPath</classname> object
	 * @link http://www.php.net/manual/en/domxpath.registernamespace.php
	 * @param prefix string <p>
	 *       The prefix.
	 *      </p>
	 * @param namespaceURI string <p>
	 *       The URI of the namespace.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function registerNamespace ($prefix, $namespaceURI) {}

	/**
	 * Evaluates the given XPath expression
	 * @link http://www.php.net/manual/en/domxpath.query.php
	 * @param expression string <p>
	 *       The XPath expression to execute.
	 *      </p>
	 * @param contextnode DOMNode[optional] <p>
	 *       The optional contextnode can be specified for
	 *       doing relative XPath queries. By default, the queries are relative to 
	 *       the root element.
	 *      </p>
	 * @param registerNodeNS bool[optional] <p>
	 *       The optional registerNodeNS can be specified to 
	 *       disable automatic registration of the context node.
	 *      </p>
	 * @return DOMNodeList a DOMNodeList containing all nodes matching
	 *   the given XPath expression. Any expression which
	 *   does not return nodes will return an empty
	 *   DOMNodeList.
	 *  </p>
	 *  <p>
	 *   If the expression is malformed or the
	 *   contextnode is invalid,
	 *   DOMXPath::query returns false.
	 */
	public function query ($expression, DOMNode $contextnode = null, $registerNodeNS = null) {}

	/**
	 * Evaluates the given XPath expression and returns a typed result if possible
	 * @link http://www.php.net/manual/en/domxpath.evaluate.php
	 * @param expression string <p>
	 *       The XPath expression to execute.
	 *      </p>
	 * @param contextnode DOMNode[optional] <p>
	 *       The optional contextnode can be specified for
	 *       doing relative XPath queries. By default, the queries are relative to 
	 *       the root element.
	 *      </p>
	 * @param registerNodeNS bool[optional] <p>
	 *       The optional registerNodeNS can be specified to 
	 *       disable automatic registration of the context node.
	 *      </p>
	 * @return mixed a typed result if possible or a DOMNodeList 
	 *   containing all nodes matching the given XPath expression. 
	 *  </p>
	 *  <p>
	 *   If the expression is malformed or the
	 *   contextnode is invalid,
	 *   DOMXPath::evaluate returns false.
	 */
	public function evaluate ($expression, DOMNode $contextnode = null, $registerNodeNS = null) {}

	/**
	 * Register PHP functions as XPath functions
	 * @link http://www.php.net/manual/en/domxpath.registerphpfunctions.php
	 * @param restrict mixed[optional] <p>
	 *       Use this parameter to only allow certain functions to be called from XPath.
	 *      </p>
	 *      <p>
	 *       This parameter can be either a string (a function name) or 
	 *       an array of function names.
	 *      </p>
	 * @return void 
	 */
	public function registerPhpFunctions ($restrict = null) {}

}

/**
 * Gets a <classname>DOMElement</classname> object from a
   <classname>SimpleXMLElement</classname> object
 * @link http://www.php.net/manual/en/function.dom-import-simplexml.php
 * @param node SimpleXMLElement <p>
 *       The SimpleXMLElement node.
 *      </p>
 * @return DOMElement The DOMElement node added or false if any errors occur.
 */
function dom_import_simplexml (SimpleXMLElement $node) {}


/**
 * Node is an element
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_ELEMENT_NODE', 1);

/**
 * Node is an attribute
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_ATTRIBUTE_NODE', 2);

/**
 * Node is a piece of text
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_TEXT_NODE', 3);
define ('XML_CDATA_SECTION_NODE', 4);
define ('XML_ENTITY_REF_NODE', 5);

/**
 * Node is an entity like &nbsp;
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_ENTITY_NODE', 6);

/**
 * Node is a processing instruction
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_PI_NODE', 7);

/**
 * Node is a comment
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_COMMENT_NODE', 8);

/**
 * Node is a document
 * @link http://www.php.net/manual/en/domxml.constants.php
 */
define ('XML_DOCUMENT_NODE', 9);
define ('XML_DOCUMENT_TYPE_NODE', 10);
define ('XML_DOCUMENT_FRAG_NODE', 11);
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
