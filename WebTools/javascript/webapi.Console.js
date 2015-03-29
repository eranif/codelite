
/**
 * @brief The console object provides access to the browser's debugging console (e.g., the Web Console in Firefox). The specifics of how it works vary from browser to browser, but there is a de facto set of features that are typically provided.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console
 */
function Console() {

	/**
	 * @brief Writes an error message to the console if the assertion is false. If the assertion is true, nothing will happen.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/assert
	 * @param assertion - Any boolean expression. If the assertion is false, the message will get written to the console.
	 * @param obj1 ... objN - A list of JavaScript objects to output. The string representations of each of these objects are appended together in the order listed and output.
	 * @param msg - A JavaScript string containing zero or more substitution strings.
	 * @param subst1 ... substN - JavaScript objects with which to replace substitution strings within msg. This gives you additional control over the format of the output.
	 */
	this.assert = function(assertion, obj1 ... objN, msg, subst1 ... substN) {};

	/**
	 * @brief Logs the number of times that this particular call to count() has been called. This function takes an optional argument label.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/count
	 */
	this.count = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/debug
	 */
	this.debug = function() {};

	/**
	 * @brief Displays an interactive list of the properties of the specified JavaScript object. The output is presented as a hierarchical listing with disclosure triangles that let you see the contents of child objects.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/dir
	 */
	this.dir = function() {};

	/**
	 * @brief Outputs an error message to the Web Console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/error
	 * @param obj1 ... objN - A list of JavaScript objects to output. The string representations of each of these objects are appended together in the order listed and output.
	 * @param msg - A JavaScript string containing zero or more substitution strings.
	 * @param subst1 ... substN - JavaScript objects with which to replace substitution strings within msg. This gives you additional control over the format of the output.
	 */
	this.error = function(obj1 ... objN, msg, subst1 ... substN) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/_exception
	 */
	this._exception = function() {};

	/**
	 * @brief Creates a new inline group in the Web Console log. This indents all following output by an additional level, until console.groupEnd() is called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/group
	 */
	this.group = function() {};

	/**
	 * @brief Creates a new inline group in the Web Console. Unlike console.group(), however, the new group is created collapsed. The user will need to use the disclosure button next to it to expand it, revealing the entries created in the group.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/groupCollapsed
	 */
	this.groupCollapsed = function() {};

	/**
	 * @brief Exits the current inline group in the Web Console. See Using groups in the console in the console documentation for details and examples.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/groupEnd
	 */
	this.groupEnd = function() {};

	/**
	 * @brief Outputs an informational message to the Web Console. In Firefox and Chrome, a small &quot;i&quot; icon is displayed next to these items in the Web Console's log.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/info
	 */
	this.info = function() {};

	/**
	 * @brief Outputs a message to the Web Console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/log
	 */
	this.log = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/profile
	 */
	this.profile = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/profileEnd
	 */
	this.profileEnd = function() {};

	/**
	 * @brief Displays tabular data as a table.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/table
	 * @param data - The data to display. This must be either an array or an object.
	 * @param columns - An array containing the names of columns to include in the output.
	 */
	this.table = function(data, columns) {};

	/**
	 * @brief Starts a timer you can use to track how long an operation takes. You give each timer a unique name, and may have up to 10,000 timers running on a given page. When you call console.timeEnd() with the same name, the browser will output the time, in milliseconds, that elapsed since the timer was started.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/time
	 */
	this.time = function() {};

	/**
	 * @brief Stops a timer that was previously started by calling console.time().
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/timeEnd
	 * @param timerName - The name of the timer to stop. Once stopped, the elapsed time is automatically displayed in the Web Console.
	 */
	this.timeEnd = function(timerName) {};

	/**
	 * @brief Outputs a stack trace to the Web Console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/trace
	 */
	this.trace = function() {};

	/**
	 * @brief Outputs a warning message to the Web Console.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/Console/warn
	 */
	this.warn = function() {};

}

