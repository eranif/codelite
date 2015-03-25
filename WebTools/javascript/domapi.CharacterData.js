
/**
 * @brief The CharacterData abstract interface represents a Node object that contains characters. This is an abstract interface, meaning there aren't any object of type CharacterData: it is implemented by other interfaces, like Text, Comment, or ProcessingInstruction which aren't abstract.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/API/CharacterData
 */
function CharacterData() {

	/**
	 * @brief The NonDocumentTypeChildNode.nextElementSibling read-only property returns the element immediately following the specified one in its parent's children list, or null if the specified element is the last one in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NonDocumentTypeChildNode/nextElementSibling
	 */
	this.nextElementSibling = '';

	/**
	 * @brief The NonDocumentTypeChildNode.previousElementSibling read-only property returns the Element immediately prior to the specified one in its parent's children list, or null if the specified element is the first one in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/NonDocumentTypeChildNode/previousElementSibling
	 */
	this.previousElementSibling = '';

	/**
	 * @brief The ChildNode.remove() method removes the object from the tree it belongs to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ChildNode/remove
	 */
	this.remove = function() {};

}

