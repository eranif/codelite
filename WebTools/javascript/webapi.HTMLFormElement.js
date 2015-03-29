
/**
 * @brief The HTMLFormElement interface provides methods to create and modify &lt;form> elements; it inherits from properties and methods of the HTMLElement interface.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement
 */
function HTMLFormElement() {

	/**
	 * @brief The HTMLFormElement.acceptCharset property represents a list of the supported character encodings for the given FORM element. This list can be comma- or space-separated.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/acceptCharset
	 */
	this.acceptCharset = '';

	/**
	 * @brief The HTMLFormElement.action property represents the action of the &lt;form> element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/action
	 */
	this.action = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/autocomplete
	 */
	this.autocomplete = '';

	/**
	 * @brief The HTMLFormElement.elements property returns an HTMLFormControlsCollection (HTML 4 HTMLCollection) of all the form controls contained in the FORM element, with the exception of input elements which have a type attribute of image.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/elements
	 */
	this.encoding = '';

	/**
	 * @brief The HTMLFormElement.enctype property represents the content type of the &lt;form> element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/enctype
	 */
	this.enctype = '';

	/**
	 * @brief The HTMLFormElement.length read-only property returns the number of controls in the &lt;form> element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/length
	 */
	this.method = '';

	/**
	 * @brief The HTMLFormElement.name property represents the name of the current form element as a string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/name
	 */
	this.name = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/noValidate
	 */
	this.noValidate = '';

	/**
	 * @brief The HTMLFormElement.target property represents the target of the action (i.e., the frame to render its output in).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/target
	 */
	this.target = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/checkValidity
	 */
	this.checkValidity = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/item
	 */
	this.item = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/namedItem
	 */
	this.namedItem = function() {};

	/**
	 * @brief The HTMLFormElement.submit() method submits the form. This method does something similar to activating a submit button of the form.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/submit
	 */
	this.submit = function() {};

	/**
	 * @brief The HTMLFormElement.reset() method restores a form element's default values. This method does the same thing as clicking the form's reset button.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLFormElement/reset
	 */
	this.reset = function() {};

}

