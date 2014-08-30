<?php

// Start of SimpleXML v.0.1

class SimpleXMLElement implements Traversable {

	/**
	 * Creates a new SimpleXMLElement object
	 * @link http://www.php.net/manual/en/simplexmlelement.construct.php
	 * @param data
	 * @param options[optional]
	 * @param data_is_url[optional]
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	final public function __construct ($data, $options, $data_is_url, $ns, $is_prefix) {}

	/**
	 * Return a well-formed XML string based on SimpleXML element
	 * @link http://www.php.net/manual/en/simplexmlelement.asxml.php
	 * @param filename string[optional] <p>
	 *       If specified, the function writes the data to the file rather than
	 *       returning it.
	 *      </p>
	 * @return mixed If the filename isn't specified, this function
	 *   returns a string on success and false on error. If the
	 *   parameter is specified, it returns true if the file was written
	 *   successfully and false otherwise.
	 */
	public function asXML ($filename = null) {}

	/**
	 * &Alias; <methodname>SimpleXMLElement::asXML</methodname>
	 * @link http://www.php.net/manual/en/simplexmlelement.savexml.php
	 * @param filename[optional]
	 */
	public function saveXML ($filename) {}

	/**
	 * Runs XPath query on XML data
	 * @link http://www.php.net/manual/en/simplexmlelement.xpath.php
	 * @param path string <p>
	 *       An XPath path
	 *      </p>
	 * @return array an array of SimpleXMLElement objects or false in
	 *   case of an error.
	 */
	public function xpath ($path) {}

	/**
	 * Creates a prefix/ns context for the next XPath query
	 * @link http://www.php.net/manual/en/simplexmlelement.registerxpathnamespace.php
	 * @param prefix string <p>
	 *       The namespace prefix to use in the XPath query for the namespace given in 
	 *       ns.
	 *      </p>
	 * @param ns string <p>
	 *       The namespace to use for the XPath query. This must match a namespace in
	 *       use by the XML document or the XPath query using 
	 *       prefix will not return any results.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function registerXPathNamespace ($prefix, $ns) {}

	/**
	 * Identifies an element's attributes
	 * @link http://www.php.net/manual/en/simplexmlelement.attributes.php
	 * @param ns string[optional] <p>
	 *       An optional namespace for the retrieved attributes
	 *      </p>
	 * @param is_prefix bool[optional] <p>
	 *       Default to false
	 *      </p>
	 * @return SimpleXMLElement a SimpleXMLElement object that can be
	 *   iterated over to loop through the attributes on the tag.
	 *  </p>
	 *  <p>
	 *   Returns &null; if called on a SimpleXMLElement
	 *   object that already represents an attribute and not a tag.
	 */
	public function attributes ($ns = null, $is_prefix = null) {}

	/**
	 * Finds children of given node
	 * @link http://www.php.net/manual/en/simplexmlelement.children.php
	 * @param ns string[optional] <p>
	 *       An XML namespace.
	 *      </p>
	 * @param is_prefix bool[optional] <p>
	 *       If is_prefix is true,
	 *       ns will be regarded as a prefix. If false,
	 *       ns will be regarded as a namespace
	 *       URL.
	 *      </p>
	 * @return SimpleXMLElement a SimpleXMLElement element, whether the node 
	 *   has children or not.
	 */
	public function children ($ns = null, $is_prefix = null) {}

	/**
	 * Returns namespaces used in document
	 * @link http://www.php.net/manual/en/simplexmlelement.getnamespaces.php
	 * @param recursive bool[optional] <p>
	 *       If specified, returns all namespaces used in parent and child nodes. 
	 *       Otherwise, returns only namespaces used in root node.
	 *      </p>
	 * @return array The getNamespaces method returns an array of 
	 *   namespace names with their associated URIs.
	 */
	public function getNamespaces ($recursive = null) {}

	/**
	 * Returns namespaces declared in document
	 * @link http://www.php.net/manual/en/simplexmlelement.getdocnamespaces.php
	 * @param recursive bool[optional] <p>
	 *       If specified, returns all namespaces declared in parent and child nodes. 
	 *       Otherwise, returns only namespaces declared in root node.
	 *      </p>
	 * @return array The getDocNamespaces method returns an array 
	 *   of namespace names with their associated URIs.
	 */
	public function getDocNamespaces ($recursive = null) {}

	/**
	 * Gets the name of the XML element
	 * @link http://www.php.net/manual/en/simplexmlelement.getname.php
	 * @return string The getName method returns as a string the 
	 *   name of the XML tag referenced by the SimpleXMLElement object.
	 */
	public function getName () {}

	/**
	 * Adds a child element to the XML node
	 * @link http://www.php.net/manual/en/simplexmlelement.addchild.php
	 * @param name string <p>
	 *       The name of the child element to add.
	 *      </p>
	 * @param value string[optional] <p>
	 *       If specified, the value of the child element.
	 *      </p>
	 * @param namespace string[optional] <p>
	 *       If specified, the namespace to which the child element belongs.
	 *      </p>
	 * @return SimpleXMLElement The addChild method returns a SimpleXMLElement
	 *   object representing the child added to the XML node.
	 */
	public function addChild ($name, $value = null, $namespace = null) {}

	/**
	 * Adds an attribute to the SimpleXML element
	 * @link http://www.php.net/manual/en/simplexmlelement.addattribute.php
	 * @param name string <p>
	 *       The name of the attribute to add.
	 *      </p>
	 * @param value string <p>
	 *       The value of the attribute.
	 *      </p>
	 * @param namespace string[optional] <p>
	 *       If specified, the namespace to which the attribute belongs.
	 *      </p>
	 * @return void 
	 */
	public function addAttribute ($name, $value, $namespace = null) {}

	public function __toString () {}

	/**
	 * Counts the children of an element
	 * @link http://www.php.net/manual/en/simplexmlelement.count.php
	 * @return int the number of elements of an element.
	 */
	public function count () {}

}

class SimpleXMLIterator extends SimpleXMLElement implements Traversable, RecursiveIterator, Iterator, Countable {

	/**
	 * Rewind to the first element
	 * @link http://www.php.net/manual/en/simplexmliterator.rewind.php
	 * @return void 
	 */
	public function rewind () {}

	/**
	 * Check whether the current element is valid
	 * @link http://www.php.net/manual/en/simplexmliterator.valid.php
	 * @return bool true if the current element is valid, otherwise false
	 */
	public function valid () {}

	/**
	 * Returns the current element
	 * @link http://www.php.net/manual/en/simplexmliterator.current.php
	 * @return mixed the current element as a SimpleXMLIterator object or &null; on failure.
	 */
	public function current () {}

	/**
	 * Return current key
	 * @link http://www.php.net/manual/en/simplexmliterator.key.php
	 * @return mixed the XML tag name of the element referenced by the current SimpleXMLIterator object or false
	 */
	public function key () {}

	/**
	 * Move to next element
	 * @link http://www.php.net/manual/en/simplexmliterator.next.php
	 * @return void 
	 */
	public function next () {}

	/**
	 * Checks whether the current element has sub elements.
	 * @link http://www.php.net/manual/en/simplexmliterator.haschildren.php
	 * @return bool true if the current element has sub-elements, otherwise false
	 */
	public function hasChildren () {}

	/**
	 * Returns the sub-elements of the current element
	 * @link http://www.php.net/manual/en/simplexmliterator.getchildren.php
	 * @return object a SimpleXMLIterator object containing
	 *   the sub-elements of the current element.
	 */
	public function getChildren () {}

	/**
	 * Creates a new SimpleXMLElement object
	 * @link http://www.php.net/manual/en/simplexmlelement.construct.php
	 * @param data
	 * @param options[optional]
	 * @param data_is_url[optional]
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	final public function __construct ($data, $options, $data_is_url, $ns, $is_prefix) {}

	/**
	 * Return a well-formed XML string based on SimpleXML element
	 * @link http://www.php.net/manual/en/simplexmlelement.asxml.php
	 * @param filename string[optional] <p>
	 *       If specified, the function writes the data to the file rather than
	 *       returning it.
	 *      </p>
	 * @return mixed If the filename isn't specified, this function
	 *   returns a string on success and false on error. If the
	 *   parameter is specified, it returns true if the file was written
	 *   successfully and false otherwise.
	 */
	public function asXML ($filename = null) {}

	/**
	 * &Alias; <methodname>SimpleXMLElement::asXML</methodname>
	 * @link http://www.php.net/manual/en/simplexmlelement.savexml.php
	 * @param filename[optional]
	 */
	public function saveXML ($filename) {}

	/**
	 * Runs XPath query on XML data
	 * @link http://www.php.net/manual/en/simplexmlelement.xpath.php
	 * @param path string <p>
	 *       An XPath path
	 *      </p>
	 * @return array an array of SimpleXMLElement objects or false in
	 *   case of an error.
	 */
	public function xpath ($path) {}

	/**
	 * Creates a prefix/ns context for the next XPath query
	 * @link http://www.php.net/manual/en/simplexmlelement.registerxpathnamespace.php
	 * @param prefix string <p>
	 *       The namespace prefix to use in the XPath query for the namespace given in 
	 *       ns.
	 *      </p>
	 * @param ns string <p>
	 *       The namespace to use for the XPath query. This must match a namespace in
	 *       use by the XML document or the XPath query using 
	 *       prefix will not return any results.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function registerXPathNamespace ($prefix, $ns) {}

	/**
	 * Identifies an element's attributes
	 * @link http://www.php.net/manual/en/simplexmlelement.attributes.php
	 * @param ns string[optional] <p>
	 *       An optional namespace for the retrieved attributes
	 *      </p>
	 * @param is_prefix bool[optional] <p>
	 *       Default to false
	 *      </p>
	 * @return SimpleXMLElement a SimpleXMLElement object that can be
	 *   iterated over to loop through the attributes on the tag.
	 *  </p>
	 *  <p>
	 *   Returns &null; if called on a SimpleXMLElement
	 *   object that already represents an attribute and not a tag.
	 */
	public function attributes ($ns = null, $is_prefix = null) {}

	/**
	 * Finds children of given node
	 * @link http://www.php.net/manual/en/simplexmlelement.children.php
	 * @param ns string[optional] <p>
	 *       An XML namespace.
	 *      </p>
	 * @param is_prefix bool[optional] <p>
	 *       If is_prefix is true,
	 *       ns will be regarded as a prefix. If false,
	 *       ns will be regarded as a namespace
	 *       URL.
	 *      </p>
	 * @return SimpleXMLElement a SimpleXMLElement element, whether the node 
	 *   has children or not.
	 */
	public function children ($ns = null, $is_prefix = null) {}

	/**
	 * Returns namespaces used in document
	 * @link http://www.php.net/manual/en/simplexmlelement.getnamespaces.php
	 * @param recursive bool[optional] <p>
	 *       If specified, returns all namespaces used in parent and child nodes. 
	 *       Otherwise, returns only namespaces used in root node.
	 *      </p>
	 * @return array The getNamespaces method returns an array of 
	 *   namespace names with their associated URIs.
	 */
	public function getNamespaces ($recursive = null) {}

	/**
	 * Returns namespaces declared in document
	 * @link http://www.php.net/manual/en/simplexmlelement.getdocnamespaces.php
	 * @param recursive bool[optional] <p>
	 *       If specified, returns all namespaces declared in parent and child nodes. 
	 *       Otherwise, returns only namespaces declared in root node.
	 *      </p>
	 * @return array The getDocNamespaces method returns an array 
	 *   of namespace names with their associated URIs.
	 */
	public function getDocNamespaces ($recursive = null) {}

	/**
	 * Gets the name of the XML element
	 * @link http://www.php.net/manual/en/simplexmlelement.getname.php
	 * @return string The getName method returns as a string the 
	 *   name of the XML tag referenced by the SimpleXMLElement object.
	 */
	public function getName () {}

	/**
	 * Adds a child element to the XML node
	 * @link http://www.php.net/manual/en/simplexmlelement.addchild.php
	 * @param name string <p>
	 *       The name of the child element to add.
	 *      </p>
	 * @param value string[optional] <p>
	 *       If specified, the value of the child element.
	 *      </p>
	 * @param namespace string[optional] <p>
	 *       If specified, the namespace to which the child element belongs.
	 *      </p>
	 * @return SimpleXMLElement The addChild method returns a SimpleXMLElement
	 *   object representing the child added to the XML node.
	 */
	public function addChild ($name, $value = null, $namespace = null) {}

	/**
	 * Adds an attribute to the SimpleXML element
	 * @link http://www.php.net/manual/en/simplexmlelement.addattribute.php
	 * @param name string <p>
	 *       The name of the attribute to add.
	 *      </p>
	 * @param value string <p>
	 *       The value of the attribute.
	 *      </p>
	 * @param namespace string[optional] <p>
	 *       If specified, the namespace to which the attribute belongs.
	 *      </p>
	 * @return void 
	 */
	public function addAttribute ($name, $value, $namespace = null) {}

	public function __toString () {}

	/**
	 * Counts the children of an element
	 * @link http://www.php.net/manual/en/simplexmlelement.count.php
	 * @return int the number of elements of an element.
	 */
	public function count () {}

}

/**
 * Interprets an XML file into an object
 * @link http://www.php.net/manual/en/function.simplexml-load-file.php
 * @param filename string <p>
 *       Path to the XML file
 *      </p>
 *      
 *       <p>
 *        Libxml 2 unescapes the URI, so if you want to pass e.g.
 *        b&c as the URI parameter a,
 *        you have to call
 *        simplexml_load_file(rawurlencode('http://example.com/?a=' .
 *        urlencode('b&c'))). Since PHP 5.1.0 you don't need to do
 *        this because PHP will do it for you.
 *       </p>
 * @param class_name string[optional] <p>
 *       You may use this optional parameter so that
 *       simplexml_load_file will return an object of 
 *       the specified class. That class should extend the 
 *       SimpleXMLElement class.
 *      </p>
 * @param options int[optional] <p>
 *       Since PHP 5.1.0 and Libxml 2.6.0, you may also use the
 *       options parameter to specify additional Libxml parameters.
 *      </p>
 * @param ns string[optional] <p>
 *      </p>
 * @param is_prefix bool[optional] <p>
 *      </p>
 * @return SimpleXMLElement an object of class SimpleXMLElement with
 *   properties containing the data held within the XML document. On errors, it
 *   will return false.
 */
function simplexml_load_file ($filename, $class_name = null, $options = null, $ns = null, $is_prefix = null) {}

/**
 * Interprets a string of XML into an object
 * @link http://www.php.net/manual/en/function.simplexml-load-string.php
 * @param data string <p>
 *       A well-formed XML string
 *      </p>
 * @param class_name string[optional] <p>
 *       You may use this optional parameter so that
 *       simplexml_load_string will return an object of 
 *       the specified class. That class should extend the 
 *       SimpleXMLElement class.
 *      </p>
 * @param options int[optional] <p>
 *       Since PHP 5.1.0 and Libxml 2.6.0, you may also use the
 *       options parameter to specify additional Libxml parameters.
 *      </p>
 * @param ns string[optional] <p>
 *      </p>
 * @param is_prefix bool[optional] <p>
 *      </p>
 * @return SimpleXMLElement an object of class SimpleXMLElement with
 *   properties containing the data held within the xml document. On errors, it
 *   will return false.
 */
function simplexml_load_string ($data, $class_name = null, $options = null, $ns = null, $is_prefix = null) {}

/**
 * Get a <literal>SimpleXMLElement</literal> object from a DOM node.
 * @link http://www.php.net/manual/en/function.simplexml-import-dom.php
 * @param node DOMNode <p>
 *       A DOM Element node
 *      </p>
 * @param class_name string[optional] <p>
 *       You may use this optional parameter so that
 *       simplexml_import_dom will return an object of 
 *       the specified class. That class should extend the 
 *       SimpleXMLElement class.
 *      </p>
 * @return SimpleXMLElement a SimpleXMLElement&return.falseforfailure;.
 */
function simplexml_import_dom (DOMNode $node, $class_name = null) {}

// End of SimpleXML v.0.1
?>
