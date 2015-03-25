
/**
 * @brief The Text interface represents the textual content of Element or Attr.  If an element has no markup within its content, it has a single child implementing Text that contains the element's text.  However, if the element contains markup, it is parsed into information items and Text nodes that form its children.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/Text
 */
function Text() {

	/**
	 * @brief The Text.isElementContentWhitespace read-only property returns a Boolean flag indicating whether or not the text node's content consists solely of whitespace.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text/isElementContentWhitespace
	 */
	this.isElementContentWhitespace = '';

	/**
	 * @brief The Text.wholeText read-only property returns all text of all Text nodes logically adjacent to the node. The text is concatenated in document order.  This allows to specify any text node and obtain all adjacent text as a single string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text/wholeText
	 */
	this.wholeText = '';

	/**
	 * @brief The Text.replaceWholeText() method replaces the text of the node and all of its logically adjacent text nodes with the specified text. The replaced nodes are removed, including the current node, unless it was the recipient of the replacement text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text/replaceWholeText
	 */
	this.replaceWholeText = function() {};

	/**
	 * @brief The Text.splitText() method breaks the Textnode into two nodes at the specified offset, keeping both nodes in the tree as siblings.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text/splitText
	 */
	this.splitText = function() {};

}

