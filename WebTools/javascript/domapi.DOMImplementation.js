
/**
 * @brief The DOMImplementation interface represent an object providing methods which are not dependent on any particular document. Such an object is returned by the Document.implementation property.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMImplementation
 */
function DOMImplementation() {

	/**
	 * @brief The DOMImplementation.createDocument() method creates and returns an XMLDocument.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMImplementation/createDocument
	 * @param namespaceURI - Is a DOMString containing the namespace URI of the document to be created, or null if the document doesn't belong to one.
	 * @param qualifiedNameStr - Is a DOMString containing the qualified name, that is an optional prefix and colon plus the local root element name, of the document to be created.
	 * @param documentType Optional - Is the DocumentType of the document to be created. It defaults to null.
	 */
	this.OMImplementation.createDocument = function(namespaceURI, qualifiedNameStr, documentType Optional) {};

	/**
	 * @brief The DOMImplementation.createDocumentType() method returns a DocumentType object which can either be used with DOMImplementation.createDocument upon document creation or can be put into the document via methods like Node.insertBefore() or Node.replaceChild().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMImplementation/createDocumentType
	 * @param qualifiedNameStr - Is a DOMString containing the qualified name, like svg:svg.
	 * @param publicId - Is a DOMString containing the PUBLIC identifier.
	 * @param systemId - Is a DOMString containing the SYSTEM identifiers.
	 */
	this.OMImplementation.createDocumentType = function(qualifiedNameStr, publicId, systemId) {};

	/**
	 * @brief The DOMImplementation.createHTMLDocument() method creates a new HTML Document.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMImplementation/createHTMLDocument
	 * @param title  Optional - Is a DOMString containing the title to give the new HTML document.
	 */
	this.OMImplementation.createHTMLDocument = function(title  Optional) {};

	/**
	 * @brief The DOMImplementation.hasFeature() method returns a Boolean flag indicating if a given feature is supported.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DOMImplementation/hasFeature
	 * @param feature - Is a DOMString representing the feature name.
	 * @param version - Is a DOMString representing the version of the specification defining the feature.
	 */
	this.OMImplementation.hasFeature = function(feature, version) {};

}

