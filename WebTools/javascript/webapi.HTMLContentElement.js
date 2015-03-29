
/**
 * @brief The HTMLContentElement interface represents a &lt;content> HTML Element, which is used in Shadow DOM.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLContentElement
 */
function HTMLContentElement() {

	/**
	 * @brief The HTMLContentElement.select property reflects the select attribute. It is a DOMString containing a space-separated list of CSS selectors that select the content to insert in place of the &lt;content> element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLContentElement/select
	 */
	this.select = '';

	/**
	 * @brief The HTMLContentElement.getDistributedNodes() method returns a static NodeList of the distributed nodes associated with this &lt;content> element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLContentElement/getDistributedNodes
	 */
	this.getDistributedNodes = function() {};

}

