
/**
 * @brief The HTMLSelectElement interface represents a &lt;select> HTML Element. These elements also share all of the properties and methods of other HTML elements via the HTMLElement interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement
 */
function HTMLSelectElement() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/autofocus
	 */
	this.autofocus = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/disabled
	 */
	this.disabled = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/form
	 */
	this.length = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/multiple
	 */
	this.multiple = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/name
	 */
	this.name = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/options
	 */
	this.required = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/selectedIndex
	 */
	this.size = '';

	/**
	 * @brief The HTMLSelectElement.type read-only property returns the form control's type. The possible values are:
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/type
	 */
	this.value = '';

	/**
	 * @brief The HTMLSelectElement.add() method adds an element to the collection of option elements for this select element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/add
	 */
	this.add = function() {};

	/**
	 * @brief The HTMLElement.blur() method removes keyboard focus from the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/blur
	 */
	this.blur = function() {};

	/**
	 * @brief The HTMLSelectElement.checkValidity() method checks whether the element has any constraints and whether it satisfies them. If the element fails its constraints, the browser fires a cancelable invalid event at the element, and the returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/checkValidity
	 */
	this.checkValidity = function() {};

	/**
	 * @brief The HTMLElement.focus() method sets focus on the specified element, if it can be focused.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/focus
	 */
	this.focus = function() {};

	/**
	 * @brief The HTMLSelectElement.item() method returns the Element corresponding to the HTMLOptionElement whose position in the options list corresponds to the index given in parameter, or null if there are none.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/item
	 */
	this.item = function() {};

	/**
	 * @brief The HTMLSelectElement.namedItem() method returns the HTMLOptionElement corresponding to the HTMLOptionElement whose name or id match the specified name, or null if no option matches.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/namedItem
	 */
	this.namedItem = function() {};

	/**
	 * @brief The HTMLSelectElement.remove() method removes the element at the specified index from the options collection for this select element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/remove
	 */
	this.remove = function() {};

	/**
	 * @brief The HTMLSelectElement.setCustomValidity() method sets the custom validity message for the selection element to the specified message. Use the empty string to indicate that the element does not have a custom validity error.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLSelectElement/setCustomValidity
	 */
	this.setCustomValidity = function() {};

}

