
/**
 * @brief The HTMLElement interface represents any HTML element. Some elements directly implement this interface, others implement it via an interface that inherits it.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement
 */
function HTMLElement() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/accessKey
	 */
	this.accessKey = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/accessKeyLabel
	 */
	this.contentEditable = '';

	/**
	 * @brief The HTMLElement.isContentEditable read-only property returns a Boolean that is true if the contents of the element are editable; otherwise it returns false.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/isContentEditable
	 */
	this.contextMenu = '';

	/**
	 * @brief The HTMLElement.dataset read-only property allows access, both in reading and writing mode, to all the custom data attributes (data-*) set on the element. It is a map of DOMString, one entry for each custom data attribute.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/dataset
	 */
	this.dir = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/draggable
	 */
	this.draggable = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/dropzone
	 */
	this.hidden = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/itemScope
	 */
	this.lang = '';

	/**
	 * @brief The HTMLElement.offsetHeight read-only property is the height of the element including vertical padding and borders, in pixels, as an integer.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/offsetHeight
	 */
	this.spellcheck = '';

	/**
	 * @brief The HTMLElement.style property returns a CSSStyleDeclaration object that represents the element's style attribute. See the CSS Properties Reference for a list of the CSS properties accessible via style.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/style
	 */
	this.style = '';

	/**
	 * @brief The HTMLElement.tabIndex property represents the tab order of the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/tabIndex
	 */
	this.tabIndex = '';

	/**
	 * @brief The HTMLElement.title property represents the title of the element, the text usually displayed in a 'tool tip' popup when the mouse is over the displayed node.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/title
	 */
	this.title = '';

	/**
	 * @brief The HTMLElement.blur() method removes keyboard focus from the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/blur
	 */
	this.blur = function() {};

	/**
	 * @brief The HTMLElement.click() method simulates a mouse click on an element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/click
	 */
	this.click = function() {};

	/**
	 * @brief The HTMLElement.focus() method sets focus on the specified element, if it can be focused.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/focus
	 */
	this.focus = function() {};

	/**
	 * @brief Forces a spelling and grammar check on HTML elements, even if the user has not focused on the elements. This method overrides user agent behavior. The user interface of the check, for example whether red underlining appears, is determined by the user agent.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/forceSpellCheck
	 */
	this.forceSpellCheck = function() {};

}

