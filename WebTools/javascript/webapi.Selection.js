
/**
 * @brief Selection is the class of the object returned by window.getSelection() and other methods. It represents the text selection in the greater page, possibly spanning multiple elements, when the user drags over static text and other parts of the page. For information about text selection in an individual text editing element, see Input, TextArea and document.activeElement which typically return the parent object returned from window.getSelection().
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection
 */
function Selection() {

	/**
	 * @brief The Selection.anchorNode read-only property returns the Node in which the selection begins.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/anchorNode
	 */
	this.anchorNode = '';

	/**
	 * @brief The Selection.anchorOffset read-only property returns the number of characters that the selection's anchor is offset within the Selection.anchorNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/anchorOffset
	 */
	this.anchorOffset = '';

	/**
	 * @brief The Selection.focusNode read-only property returns the Node in which the selection ends.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/focusNode
	 */
	this.focusNode = '';

	/**
	 * @brief The Selection.focusOffset read-only property returns the number of characters that the selection's focus is offset within the Selection.focusNode.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/focusOffset
	 */
	this.focusOffset = '';

	/**
	 * @brief The Selection.isCollapsed read-only property returns a Boolean which indicates whether or not there is currently any text selected; That is to say that the selection's start and end points are at the same position in the content.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/isCollapsed
	 */
	this.isCollapsed = '';

	/**
	 * @brief The Selection.rangeCount read-only property returns the number of ranges in the selection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/rangeCount
	 */
	this.rangeCount = '';

	/**
	 * @brief The Selection.getRangeAt() method returns a range object representing one of the ranges currently selected.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/getRangeAt
	 * @param range - The Range object that will be returned.
	 * @param index - The zero-based index of the range to return. A negative number or a number greater than or equal to Selection.rangeCount will result in an error.
	 */
	this.getRangeAt = function(range, index) {};

	/**
	 * @brief The Selection.collapse() method collapses the current selection to a single point. The document is not modified. If the content is focused and editable, the caret will blink there.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/collapse
	 * @param parentNode - The caret location will be within this node.
	 * @param offset - The offset in parentNode to which the selection will be collapsed.
	 */
	this.collapse = function(parentNode, offset) {};

	/**
	 * @brief The Selection.extend() method moves the focus of the selection to a specified point. The anchor of the selection does not move. The selection will be from the anchor to the new focus regardless of direction.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/extend
	 * @param parentNode - The node within which the focus will be moved.
	 * @param offset - The offset position within parentNode where the focus will be moved to.
	 */
	this.extend = function(parentNode, offset) {};

	/**
	 * @brief The Selection.modify() method applies a change to the current selection or cursor position, using simple textual commands.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/modify
	 * @param alter - The type of change to apply. Specify "move" to move the current cursor position or "extend" to extend the current selection.
	 * @param direction - The direction in which to adjust the current selection. You can specify "forward" or "backward" to adjust in the appropriate direction based on the language at the selection point. If you want to adjust in a specific direction, you can specify "left" or "right".
	 * @param granularity - The distance to adjust the current selection or cursor position. You can move by "character", "word", "sentence", "line", "paragraph", "lineboundary", "sentenceboundary", "paragraphboundary", or "documentboundary".
	 */
	this.modify = function(alter, direction, granularity) {};

	/**
	 * @brief The Selection.collapseToStart() method collapses the selection to the start of the first range in the selection.  If the content of the selection is focused and editable, the caret will blink there.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/collapseToStart
	 */
	this.collapseToStart = function() {};

	/**
	 * @brief The Selection.collapseToEnd() method collapses the selection to the end of the last range in the selection.  If the content the selection is in is focused and editable, the caret will blink there.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/collapseToEnd
	 */
	this.collapseToEnd = function() {};

	/**
	 * @brief The Selection.selectAllChildren() method adds all the children of the specified node to the selection. Previous selection is lost.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/selectAllChildren
	 * @param parentNode - All children of
 parentNode
 will be selected.

 parentNode
 itself is not part of the selection.
	 */
	this.selectAllChildren = function(parentNode) {};

	/**
	 * @brief The Selection.addRange() method adds a Range to a Selection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/addRange
	 * @param range - A Range object that will be added to the Selection.
	 */
	this.addRange = function(range) {};

	/**
	 * @brief The Selection.removeRange() method removes a range from the selection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/removeRange
	 * @param range - A range object that will be removed to the selection.
	 */
	this.removeRange = function(range) {};

	/**
	 * @brief The Selection.removeAllRanges() method removes all ranges from the selection, leaving the anchorNode and focusNode properties equal to null and leaving nothing selected.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/removeAllRanges
	 */
	this.removeAllRanges = function() {};

	/**
	 * @brief The Selection.deleteFromDocument() method deletes the actual text being represented by a selection object from the document's DOM.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/deleteFromDocument
	 */
	this.deleteFromDocument = function() {};

	/**
	 * @brief The Selection.selectionLanguageChange() method modifies the cursor Bidi level after a change in keyboard direction.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/selectionLanguageChange
	 * @param langRTL - true if the new language is right-to-left or false if the new language is left-to-right.
	 */
	this.selectionLanguageChange = function(langRTL) {};

	/**
	 * @brief The Selection.toString() method returns a string currently being represented by the selection object, i.e. the currently selected text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/toString
	 */
	this.toString = function() {};

	/**
	 * @brief The Selection.containsNode() method indicates if the node is part of the selection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Selection/containsNode
	 * @param aNode - The node that is being looked for whether it is part of the selection
	 * @param aPartlyContained - When
 
 true, containsNode() returns true when a part of the node is part of the selection.
 When false, containsNode() only returns true when the entire node is part of the selection.
	 */
	this.containsNode = function(aNode, aPartlyContained) {};

}

