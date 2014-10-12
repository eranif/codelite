<?php

// Start of SimpleXML v.0.1

class SimpleXMLElement implements Traversable {

	/**
	 * @param data
	 * @param options[optional]
	 * @param data_is_url[optional]
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	final public function __construct ($data, $options, $data_is_url, $ns, $is_prefix) {}

	/**
	 * @param filename[optional]
	 */
	public function asXML ($filename) {}

	/**
	 * @param filename[optional]
	 */
	public function saveXML ($filename) {}

	/**
	 * @param path
	 */
	public function xpath ($path) {}

	/**
	 * @param prefix
	 * @param ns
	 */
	public function registerXPathNamespace ($prefix, $ns) {}

	/**
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	public function attributes ($ns, $is_prefix) {}

	/**
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	public function children ($ns, $is_prefix) {}

	/**
	 * @param recursve[optional]
	 */
	public function getNamespaces ($recursve) {}

	/**
	 * @param recursve[optional]
	 * @param from_root[optional]
	 */
	public function getDocNamespaces ($recursve, $from_root) {}

	public function getName () {}

	/**
	 * @param name
	 * @param value[optional]
	 * @param ns[optional]
	 */
	public function addChild ($name, $value, $ns) {}

	/**
	 * @param name
	 * @param value[optional]
	 * @param ns[optional]
	 */
	public function addAttribute ($name, $value, $ns) {}

	public function __toString () {}

	public function count () {}

}

class SimpleXMLIterator extends SimpleXMLElement implements Traversable, RecursiveIterator, Iterator, Countable {

	public function rewind () {}

	public function valid () {}

	public function current () {}

	public function key () {}

	public function next () {}

	public function hasChildren () {}

	public function getChildren () {}

	/**
	 * @param data
	 * @param options[optional]
	 * @param data_is_url[optional]
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	final public function __construct ($data, $options, $data_is_url, $ns, $is_prefix) {}

	/**
	 * @param filename[optional]
	 */
	public function asXML ($filename) {}

	/**
	 * @param filename[optional]
	 */
	public function saveXML ($filename) {}

	/**
	 * @param path
	 */
	public function xpath ($path) {}

	/**
	 * @param prefix
	 * @param ns
	 */
	public function registerXPathNamespace ($prefix, $ns) {}

	/**
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	public function attributes ($ns, $is_prefix) {}

	/**
	 * @param ns[optional]
	 * @param is_prefix[optional]
	 */
	public function children ($ns, $is_prefix) {}

	/**
	 * @param recursve[optional]
	 */
	public function getNamespaces ($recursve) {}

	/**
	 * @param recursve[optional]
	 * @param from_root[optional]
	 */
	public function getDocNamespaces ($recursve, $from_root) {}

	public function getName () {}

	/**
	 * @param name
	 * @param value[optional]
	 * @param ns[optional]
	 */
	public function addChild ($name, $value, $ns) {}

	/**
	 * @param name
	 * @param value[optional]
	 * @param ns[optional]
	 */
	public function addAttribute ($name, $value, $ns) {}

	public function __toString () {}

	public function count () {}

}

/**
 * @param filename
 * @param class_name[optional]
 * @param options[optional]
 * @param ns[optional]
 * @param is_prefix[optional]
 */
function simplexml_load_file ($filename, $class_name, $options, $ns, $is_prefix) {}

/**
 * @param data
 * @param class_name[optional]
 * @param options[optional]
 * @param ns[optional]
 * @param is_prefix[optional]
 */
function simplexml_load_string ($data, $class_name, $options, $ns, $is_prefix) {}

/**
 * @param node
 * @param class_name[optional]
 */
function simplexml_import_dom ($node, $class_name) {}

// End of SimpleXML v.0.1
?>
