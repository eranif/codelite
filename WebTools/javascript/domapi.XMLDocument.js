
/**
 * @brief The XMLDocument interface represent an XML document. It inherits from the generic Document and do not add any specific methods or properties to it: nevertheless, several algorithms behave differently with the two types of documents.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/XMLDocument
 */
function XMLDocument() {

	/**
	 * @brief document.load() is a part of an old version of the W3C DOM Level 3 Load &amp; Save module. Can be used with document.async to indicate whether the request is synchronous or asynchronous (the default). As of at least Gecko 1.9, this no longer supports cross-site loading of documents (Use XMLHttpRequest instead).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/XMLDocument/load
	 */
	this.load = function() {};

}

