
/**
 * @brief A TouchList represents a list of all of the points of contact with a touch surface; for example, if the user has three fingers on the screen (or trackpad), the corresponding TouchList would have one Touch object for each finger, for a total of three entries.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TouchList
 */
function TouchList() {

	/**
	 * @brief This method returns the first Touch item in the TouchList matching the specified identifier.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TouchList/identifiedTouch
	 * @param id - An integer value identifying the Touch object to retrieve from the list.
	 */
	this.identifiedTouch = function(id) {};

	/**
	 * @brief Returns the Touch object at the specified index into the TouchList. You may also use standard array syntax to access items in the list.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TouchList/item
	 * @param index - The index into the TouchList of the object to retrieve.
	 */
	this.item = function(index) {};

}

