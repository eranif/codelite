
/**
 * @brief The CharacterData abstract interface represents a Node object that contains characters. This is an abstract interface, meaning there aren't any object of type CharacterData: it is implemented by other interfaces, like Text, Comment, or ProcessingInstruction which aren't abstract.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData
 */
function CharacterData() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/data
	 */
	this.data = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/appendData
	 */
	this.appendData = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/deleteData
	 */
	this.deleteData = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/insertData
	 */
	this.insertData = function() {};

	/**
	 * @brief The ChildNode.remove() method removes the object from the tree it belongs to.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ChildNode/remove
	 */
	this.remove = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/replaceData
	 */
	this.replaceData = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CharacterData/substringData
	 */
	this.substringData = function() {};

}

