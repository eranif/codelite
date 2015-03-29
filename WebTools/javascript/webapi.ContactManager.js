
/**
 * @brief The ContactManager interface is used to access and manage the contact available on the device.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager
 */
function ContactManager() {

	/**
	 * @brief Specifies an event listener to receive contactchange events. These events occur when a contact is added, updated, or removed from the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/oncontactchange
	 */
	this.oncontactchange = '';

	/**
	 * @brief The clear method is used to remove ALL the contacts from the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/clear
	 */
	this.clear = function() {};

	/**
	 * @brief The find method is used to retrieve a limited list of contacts from the device's contact database. This method is best suited to access a small data set, where getAll() is better suited for a large data set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/find
	 * @param options - The find method expects some options to filter the contacts in order to reduce the size of the resulting list. The possible options are the following:
 
  filterBy: An array of strings representing all the fields to filter by.
  filterValue: The value to match against.  Because of Bug 1093105 if you are using the equals filterOp you will need to manually call toLowerCase() on any string values you pass in because the Contacts API normalizes all strings in its index but fails to normalize the passed-in filterValue in the case of equals.  startsWith automatically normalizes correctly.
  filterOp: The filter comparison operator to use. Possible values are equals, startsWith, and match, the latter being specific to telephone numbers.
  filterLimit: The number of contacts to retrieve.
	 */
	this.find = function(options) {};

	/**
	 * @brief The getAll method is used to access a sorted list of contact from the device's contact database. This method is best suited to access a large data set, where find() is better suited for a small data set.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/getAll
	 * @param options - The getAll method expects some options to sort and filter the contacts. The possible options are the following:
 Sort options
 
  sortBy: A string representing the field by which the results of the search are sorted. Currently only givenName and familyName are supported.
  sortOrder: A string indicating the result's sort order. Possible values are descending or ascending.
 
 Filter options

 
  filterBy: An array of strings representing all the fields to filter by.
  filterValue: The value to match against.
  filterOp: The filter comparison operator to use. Possible values are equals, startsWith, and match, the latter being specific to telephone numbers.
  filterLimit: The number of contacts to retrieve.
	 */
	this.getAll = function(options) {};

	/**
	 * @brief The getCount method is used to retrieve the total number of contacts available inside the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/getCount
	 */
	this.getCount = function() {};

	/**
	 * @brief The getRevision method is used to retrieve the revision number of the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/getRevision
	 */
	this.getRevision = function() {};

	/**
	 * @brief The remove method is used to delete a contact from the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/remove
	 * @param contact - A mozContact object representing the contact to remove.
	 */
	this.remove = function(contact) {};

	/**
	 * @brief The save method is used to store a contact inside the device's contact database. If the contact is already in the database (based on its id), it is updated, otherwise it is created.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/ContactManager/save
	 * @param contact - A mozContact object representing the contact to add or update.
	 */
	this.save = function(contact) {};

}

