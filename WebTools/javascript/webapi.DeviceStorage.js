
/**
 * @brief The DeviceStorage interface is used to access files on a specific storage area available on the device. A storage area is, in essence, a file system repository even if it hides the reality of the underlying file system.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage
 */
function DeviceStorage() {

	/**
	 * @brief The add method is used to add a file inside a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/add
	 * @param file - A Blob object representing the file to add (note that a File object is also a Blob object).
	 */
	this.add = function(file) {};

	/**
	 * @brief The addNamed method is used to add a file inside a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/addNamed
	 * @param file - A Blob object representing the file to add (note that a File object is also a Blob object).
	 * @param name - A string representing the full name (path + file name) of the file.
	 */
	this.addNamed = function(file, name) {};

	/**
	 * @brief The available() method is used to check whether the storage area is available; this is an asynchronous operation that returns a DOMRequest object that will receive a callback when the operation is complete.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/available
	 */
	this.available = function() {};

	/**
	 * @brief The delete method is used to remove a file from a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/delete
	 * @param fileName - A string representing the full name (path + file name) of the file to remove.
	 */
	this.delete = function(fileName) {};

	/**
	 * @brief The enumerate method is used to iterate through the files of a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/enumerate
	 * @param path Optional - A string representing a path directory within the storage area where to search for files.
	 * @param options Optional - An object with the property since which is a Date object. Files older than that date will be ignored during the iteration process.
	 */
	this.enumerate = function(path Optional, options Optional) {};

	/**
	 * @brief The enumerateEditable method is used to iterate through the files of a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/enumerateEditable
	 * @param path Optional - A string representing a path directory within the storage area where to search for files.
	 * @param options Optional - An object with the property since which is a Date object. Files older than that date will be ignored during the iteration process.
	 */
	this.enumerateEditable = function(path Optional, options Optional) {};

	/**
	 * @brief The freeSpace method is used to get the amount of free space usable by the storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/freeSpace
	 */
	this.freeSpace = function() {};

	/**
	 * @brief The get method is used to retrieve a read-only file from a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/get
	 * @param fileName - A string representing the full name (path + file name) of the file to retrieve.
	 */
	this.get = function(fileName) {};

	/**
	 * @brief The getEditable method is used to retrieve a file, which is editable, from a given storage area.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/getEditable
	 * @param fileName - A string representing the full name (path + file name) of the file to retrieve.
	 */
	this.getEditable = function(fileName) {};

	/**
	 * @brief The usedSpace method is used to get the total of space used by the storage (the amount of space filled by files).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/DeviceStorage/usedSpace
	 */
	this.usedSpace = function() {};

}

