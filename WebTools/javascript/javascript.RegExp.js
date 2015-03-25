
/**
 * @brief The RegExp constructor creates a regular expression object for matching text with a pattern.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp
 */
function RegExp() {

	/**
	 * @brief The non-standard $1, $2, $3, $4, $5, $6, $7, $8, $9 properties are static and read-only properties of regular expressions that contain parenthesized substring matches.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/n
	 */
	this.$1-$9 = '';

	/**
	 * @brief The non-standard input property is a static property of regular expressions that contains the string against which a regular expression is matched. RegExp.$_ is an alias for this property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/input
	 */
	this.input ($_) = '';

	/**
	 * @brief The lastIndex is a read/write integer property of regular expressions that specifies the index at which to start the next match.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/lastIndex
	 */
	this.lastIndex = '';

	/**
	 * @brief The non-standard lastMatch property is a static and read-only property of regular expressions that contains the last matched characters. RegExp.$&amp; is an alias for this property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/lastMatch
	 */
	this.lastMatch ($&amp;) = '';

	/**
	 * @brief The non-standard lastParen property is a static and read-only property of regular expressions that contains the last parenthesized substring match, if any. RegExp.$+ is an alias for this property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/lastParen
	 */
	this.lastParen ($+) = '';

	/**
	 * @brief The non-standard leftContext property is a static and read-only property of regular expressions that contains the substring preceding the most recent match. RegExp.$` is an alias for this property.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/leftContext
	 */
	this.leftContext ($`) = '';

	/**
	 * @brief The flags property returns a string consisting of the flags of the current regular expression object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/flags
	 */
	this.flags = '';

	/**
	 * @brief The source property returns a String containing the source text of the regexp object, and it doesn't contain the two forward slashes on both sides and any flags.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/global
	 */
	this.source = '';

	/**
	 * @brief The sticky property reflects whether or not the search is sticky (searches in strings only from the index indicated by the lastIndex property of this regular expression). sticky is a read-only property of an individual regular expression object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/sticky
	 */
	this.sticky = '';

	/**
	 * @brief This is an experimental API that should not be used in production code.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/unicode
	 */
	this.rightContext ($') = '';

}

/**
 * @brief The deprecated compile() method is used to (re-)compile a regular expression during execution of a script. It is basically the same as the RegExp constructor.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/compile
 * @param pattern - The text of the regular expression.
 * @param flags - If specified, flags can have any combination of the following values:   g  global match
 * @param i - ignore case
 * @param m - multiline; treat beginning and end characters (^ and $) as working over multiple lines (i.e., match the beginning or end of each line (delimited by \n or \r), not only the very beginning or end of the whole input string)
 * @param y - sticky; matches only from the index indicated by the lastIndex property of this regular expression in the target string (and does not attempt to match from any later indexes).
 */
RegExp.prototype.compile = function(pattern, flags, i, m, y) {};

/**
 * @brief The exec() method executes a search for a match in a specified string. Returns a result array, or null.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/exec
 * @param str - The string against which to match the regular expression.
 */
RegExp.prototype.exec = function(str) {};

/**
 * @brief The test() method executes a search for a match between a regular expression and a specified string. Returns true or false.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/test
 * @param str - The string against which to match the regular expression.
 */
RegExp.prototype.test = function(str) {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/toSource
 */
RegExp.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the regular expression.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp/toString
 */
RegExp.prototype.toString = function() {};

