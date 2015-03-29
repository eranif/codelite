
/**
 * @brief The Text interface represents the textual content of Element or Attr.  If an element has no markup within its content, it has a single child implementing Text that contains the element's text.  However, if the element contains markup, it is parsed into information items and Text nodes that form its children.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text
 */
function Text() {

	/**
	 * @brief The Text.replaceWholeText() method replaces the text of the node and all of its logically adjacent text nodes with the specified text. The replaced nodes are removed, including the current node, unless it was the recipient of the replacement text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Text/replaceWholeText
	 */
	this.ext.replaceWholeText

}

