
/**
 * @brief The HTMLTableElement interface provides special properties and methods (beyond the regular HTMLElement object interface it also has available to it by inheritance) for manipulating the layout and presentation of tables in an HTML document.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement
 */
function HTMLTableElement() {

	/**
	 * @brief The HTMLTableElement.caption property represents the table caption. If no caption element is associated with the table, it can be null.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/caption
	 */
	this.caption = '';

	/**
	 * @brief The HTMLTableElement.tHead represents  the table's &lt;thead> element. Its value can be null if there is no such element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/tHead
	 */
	this.tHead = '';

	/**
	 * @brief The HTMLTableElement.tFoot property represents the table's &lt;tfoot> element. Its value can be null if there is no such element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/tFoot
	 */
	this.tFoot = '';

	/**
	 * @brief The HTMLTableElement.createTHead()method returns the &lt;thead> element association with the table, of type HTMLTableSectionElement. If there is no such element associated to the thable, this method creates it, then returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/createTHead
	 */
	this.createTHead = function() {};

	/**
	 * @brief The HTMLTableElement.deleteTHead() removes a &lt;thead> element from the table.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/deleteTHead
	 */
	this.deleteTHead = function() {};

	/**
	 * @brief The HTMLTableElement.createTFoot() method returns the &lt;tfoot> element associated with the table, of type HTMLTableSectionElement. If there is no footer for this table, this methods creates it, then returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/createTFoot
	 */
	this.createTFoot = function() {};

	/**
	 * @brief The HTMLTableElement.deleteTFoot() method removes a &lt;tfoot> element from the table.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/deleteTFoot
	 */
	this.deleteTFoot = function() {};

	/**
	 * @brief The HTMLTableElement.createCaption() method returns the caption for the table. If no caption element exists on the table, this method creates it, then returns it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/createCaption
	 */
	this.createCaption = function() {};

	/**
	 * @brief The HTMLTableElement.deleteCaption() method removes the caption from the table. If there is no caption associated with the table, this method does nothing.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/deleteCaption
	 */
	this.deleteCaption = function() {};

	/**
	 * @brief The HTMLTableElement.insertRow() method inserts a new row in the table.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/insertRow
	 */
	this.insertRow = function() {};

	/**
	 * @brief The HTMLTableElement.deleteRow() method removes a row from the table. If the amount of rows to delete, specified by the parameter, is greater or equal to the number of available rows, or if it is negative and not equal to the special index -1, representing the last row of the table, the exception INDEX_SIZE_ERR is thrown.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/deleteRow
	 * @param index - index is an integer representing the row that should be deleted.
 However, the special index -1 can be used to remove the very last row of a table.
	 */
	this.deleteRow = function(index) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/HTMLTableElement/stopSorting
	 */
	this.stopSorting = function() {};

}

