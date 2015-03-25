
/**
 * @brief The String global object is a constructor for strings, or a sequence of characters.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String
 */
function String() {

	/**
	 * @brief The length property represents the length of a string.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/length
	 */
	this.length = '';

	/**
	 * @brief The static String.fromCharCode() method returns a string created by using the specified sequence of Unicode values.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/fromCharCode
	 * @param num1, ..., numN - A sequence of numbers that are Unicode values.
	 */
	this., numN) {};

	/**
	 * @brief The static String.fromCodePoint() method returns a string created by using the specified sequence of code points.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/fromCodePoint
	 * @param num1, ..., numN - A sequence of code points.
	 */
	this., numN) {};

	/**
	 * @brief The static String.raw() method is a tag function of template strings, like the r prefix in Python or the @ prefix in C# for string literals, this function is used to get the raw string form of template strings.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/raw
	 * @param callSite - Well-formed template call site object, like { raw: 'string' }.
	 * @param ...substitutions - Contains substitution values.
	 * @param templateString - A template string, optionally with substitutions (${...}).
	 */
	this.substitutions, templateString) {};

}

/**
 * @brief The anchor() method creates an &lt;a> HTML anchor element that is used as a hypertext target.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/anchor
 * @param name - A string representing the name attribute of the a tag to be created.
 */
String.prototype.anchor = function(name) {};

/**
 * @brief The big() method creates a &lt;big> HTML element that causes a string to be displayed in a big font.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/big
 */
String.prototype.big = function() {};

/**
 * @brief The blink() method creates a &lt;blink> HTML element that causes a string to blink.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/blink
 */
String.prototype.blink = function() {};

/**
 * @brief The bold() method creates a &lt;b> HTML element that causes a string to be displayed as bold.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/bold
 */
String.prototype.bold = function() {};

/**
 * @brief The charAt() method returns the specified character from a string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/charAt
 * @param index - An integer between 0 and 1-less-than the length of the string.
 */
String.prototype.charAt = function(index) {};

/**
 * @brief The charCodeAt() method returns the numeric Unicode value of the character at the given index (except for unicode codepoints > 0x10000).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/charCodeAt
 * @param index - An integer greater than or equal to 0 and less than the length of the string; if it is not a number, it defaults to 0.
 */
String.prototype.charCodeAt = function(index) {};

/**
 * @brief The codePointAt() method returns a non-negative integer that is the UTF-16 encoded code point value.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/codePointAt
 * @param pos - Position of an element in the String to return the code point value from.
 */
String.prototype.codePointAt = function(pos) {};

/**
 * @brief The concat() method combines the text of two or more strings and returns a new string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/concat
 * @param string2...stringN - Strings to concatenate to this string.
 */
String.prototype.concat = function(string2...stringN) {};

/**
 * @brief The endsWith() method determines whether a string ends with the characters of another string, returning true or false as appropriate.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/endsWith
 * @param searchString - The characters to be searched for at the end of this string.
 * @param position - Optional. Search within this string as if this string were only this long; defaults to this string's actual length, clamped within the range established by this string's length.
 */
String.prototype.endsWith = function(searchString, position) {};

/**
 * @brief The fixed() method creates a &lt;tt> HTML element that causes a string to be displayed in fixed-pitch font.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/fixed
 */
String.prototype.fixed = function() {};

/**
 * @brief The fontcolor() method creates a &lt;font> HTML element that causes a string to be displayed in the specified font color.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/fontcolor
 * @param color - A string expressing the color as a hexadecimal RGB triplet or as a string literal. String literals for color names are listed in the CSS color reference.
 */
String.prototype.fontcolor = function(color) {};

/**
 * @brief The fontsize() method creates a &lt;font> HTML element that causes a string to be displayed in the specified font size.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/fontsize
 * @param size - An integer between 1 and 7, a string representing a signed integer between 1 and 7.
 */
String.prototype.fontsize = function(size) {};

/**
 * @brief The includes() method determines whether one string may be found within another string, returning true or false as appropriate.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/includes
 * @param searchString - A string to be searched for within this string.
 * @param position - Optional. The position in this string at which to begin searching for searchString; defaults to 0.
 */
String.prototype.includes = function(searchString, position) {};

/**
 * @brief The indexOf() method returns the index within the calling String object of the first occurrence of the specified value, starting the search at fromIndex. Returns -1 if the value is not found.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/indexOf
 * @param searchValue - A string representing the value to search for.
 * @param fromIndex - Optional. The location within the calling string to start the search from. It can be any integer. The default value is 0. If fromIndex &lt; 0 the entire string is searched (same as passing 0). If fromIndex &gt;= str.length, the method will return -1 unless searchValue is an empty string in which case str.length is returned.
 */
String.prototype.indexOf = function(searchValue, fromIndex) {};

/**
 * @brief The italics() method creates an &lt;i> HTML element that causes a string to be italic.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/italics
 */
String.prototype.italics = function() {};

/**
 * @brief The lastIndexOf() method returns the index within the calling String object of the last occurrence of the specified value, or -1 if not found. The calling string is searched backward, starting at fromIndex.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/lastIndexOf
 * @param searchValue - A string representing the value to search for.
 * @param fromIndex - Optional. The location within the calling string to start the search at, indexed from left to right. It can be any integer. The default value is str.length. If it is negative, it is treated as 0. If fromIndex &gt; str.length, fromIndex is treated as str.length.
 */
String.prototype.lastIndexOf = function(searchValue, fromIndex) {};

/**
 * @brief The link() method creates an &lt;a> HTML element that causes a string to be displayed as a hypertext link to another URL.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/link
 * @param url - Any string that specifies the href attribute of the &lt;a&gt; tag; it should be a valid URL (relative or absolute), with any &amp; characters escaped as &amp;amp;, and any " characters escaped as &amp;quot;.
 */
String.prototype.link = function(url) {};

/**
 * @brief The localeCompare() method returns a number indicating whether a reference string comes before or after or is the same as the given string in sort order.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/localeCompare
 * @param compareString - The string against which the referring string is comparing
 * @param locales - Optional. A string with a BCP 47 language tag, or an array of such strings. For the general form and interpretation of the locales argument, see the Intl page. The following Unicode extension keys are allowed:  co   Variant collations for certain locales. Possible values include: "big5han", "dict", "direct", "ducet", "gb2312", "phonebk", "phonetic", "pinyin", "reformed", "searchjl", "stroke", "trad", "unihan". The "standard" and "search" values are ignored; they are replaced by the options property usage (see below).
 * @param kn - Whether numeric collation should be used, such that "1" &lt; "2" &lt; "10". Possible values are "true" and "false". This option can be set through an options property or through a Unicode extension key; if both are provided, the options property takes precedence.
 * @param kf - Whether upper case or lower case should sort first. Possible values are "upper", "lower", or "false" (use the locale's default). This option can be set through an options property or through a Unicode extension key; if both are provided, the options property takes precedence.
 * @param options - Optional. An object with some or all of the following properties:  localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
 * @param usage - Whether the comparison is for sorting or for searching for matching strings. Possible values are "sort" and "search"; the default is "sort".
 * @param sensitivity - Which differences in the strings should lead to non-zero result values. Possible values are:    "base": Only strings that differ in base letters compare as unequal. Examples: a ≠ b, a = á, a = A.     "accent": Only strings that differ in base letters or accents and other diacritic marks compare as unequal. Examples: a ≠ b, a ≠ á, a = A.     "case": Only strings that differ in base letters or case compare as unequal. Examples: a ≠ b, a = á, a ≠ A.     "variant": Strings that differ in base letters, accents and other diacritic marks, or case compare as unequal. Other differences may also be taken into consideration. Examples: a ≠ b, a ≠ á, a ≠ A.    The default is "variant" for usage "sort"; it's locale dependent for usage "search".
 * @param ignore­Punctua­tion - Whether punctuation should be ignored. Possible values are true and false; the default is false.
 * @param numeric - Whether numeric collation should be used, such that "1" &lt; "2" &lt; "10". Possible values are true and false; the default is false. This option can be set through an options property or through a Unicode extension key; if both are provided, the options property takes precedence. Implementations are not required to support this property.
 * @param caseFirst - Whether upper case or lower case should sort first. Possible values are "upper", "lower", or "false" (use the locale's default); the default is "false". This option can be set through an options property or through a Unicode extension key; if both are provided, the options property takes precedence. Implementations are not required to support this property.
 */
String.prototype.localeCompare = function(compareString, locales, kn, kf, options, usage, sensitivity, ignore­Punctua­tion, numeric, caseFirst) {};

/**
 * @brief The match() method retrieves the matches when matching a string against a regular expression.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/match
 * @param regexp - A regular expression object. If a non-RegExp object obj is passed, it is implicitly converted to a RegExp by using new RegExp(obj).
 */
String.prototype.match = function(regexp) {};

/**
 * @brief The normalize() method returns the Unicode Normalization Form of a given string (if the value isn't a string, it will be converted to one first).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/normalize
 * @param form - One of "NFC", "NFD", "NFKC", or "NFKD", specifying the Unicode Normalization Form. If omitted or undefined, "NFC" is used.     NFC — Normalization Form Canonical Composition.   NFD — Normalization Form Canonical Decomposition.   NFKC — Normalization Form Compatibility Composition.   NFKD — Normalization Form Compatibility Decomposition.
 */
String.prototype.normalize = function(form) {};

/**
 * @brief This is an obsolete API and is no longer guaranteed to work.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/quote
 */
String.prototype.repeat = function() {};

/**
 * @brief The replace() method returns a new string with some or all matches of a pattern replaced by a replacement. The pattern can be a string or a RegExp, and the replacement can be a string or a function to be called for each match.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/replace
 * @param regexp - A RegExp object. The match is replaced by the return value of parameter #2.
 * @param substr - A String that is to be replaced by newSubStr.
 * @param newSubStr - The String that replaces the substring received from parameter #1. A number of special replacement patterns are supported; see the "Specifying a string as a parameter" section below.
 * @param function - A function to be invoked to create the new substring (to put in place of the substring received from parameter #1). The arguments supplied to this function are described in the "Specifying a function as a parameter" section below.
 * @param flags - Note: The flags argument does not work in v8 Core (Chrome and Node.js). A string specifying a combination of regular expression flags. The use of the flags parameter in the String.prototype.replace() method is non-standard. Instead of using this parameter, use a RegExp object with the corresponding flags. The value of this parameter if it is used should be a string consisting of one or more of the following characters to affect the operation as described:   g  global match
 * @param i - ignore case
 * @param m - match over multiple lines
 * @param y - sticky
 */
String.prototype.replace = function(regexp, substr, newSubStr, function, flags, i, m, y) {};

/**
 * @brief The search() method executes a search for a match between a regular expression and this String object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/search
 * @param regexp - Optional. A regular expression object. If a non-RegExp object obj is passed, it is implicitly converted to a RegExp by using new RegExp(obj).
 */
String.prototype.search = function(regexp) {};

/**
 * @brief The slice() method extracts a section of a string and returns a new string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/slice
 * @param beginSlice - The zero-based index at which to begin extraction. If negative, it is treated as sourceLength + beginSlice where sourceLength is the length of the string (for example, if beginSlice is -3 it is treated as sourceLength - 3).
 * @param endSlice - Optional. The zero-based index at which to end extraction. If omitted, slice() extracts to the end of the string. If negative, it is treated as sourceLength - endSlice where sourceLength is the length of the string.
 */
String.prototype.slice = function(beginSlice, endSlice) {};

/**
 * @brief The small() method creates a &lt;small> HTML element that causes a string to be displayed in a small font.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/small
 */
String.prototype.small = function() {};

/**
 * @brief The split() method splits a String object into an array of strings by separating the string into substrings.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/split
 * @param separator - Optional. Specifies the character(s) to use for separating the string. The separator is treated as a string or a regular expression. If separator is omitted, the array returned contains one element consisting of the entire string. If separator is an empty string, str is converted to an array of characters.
 * @param limit - Optional. Integer specifying a limit on the number of splits to be found. The split() method still splits on every match of separator, but it truncates the returned array to at most limit elements.
 */
String.prototype.split = function(separator, limit) {};

/**
 * @brief The startsWith() method determines whether a string begins with the characters of another string, returning true or false as appropriate.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/startsWith
 * @param searchString - The characters to be searched for at the start of this string.
 * @param position - Optional. The position in this string at which to begin searching for searchString; defaults to 0.
 */
String.prototype.startsWith = function(searchString, position) {};

/**
 * @brief The strike() method creates a &lt;strike> HTML element that causes a string to be displayed as struck-out text.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/strike
 */
String.prototype.strike = function() {};

/**
 * @brief The sub() method creates a &lt;sub> HTML element that causes a string to be displayed as subscript.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/sub
 */
String.prototype.sub = function() {};

/**
 * @brief The substr() method returns the characters in a string beginning at the specified location through the specified number of characters.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/substr
 * @param start - Location at which to begin extracting characters. If a negative number is given, it is treated as strLength + start where strLength = to the length of the string (for example, if start is -3 it is treated as strLength - 3.)
 * @param length - Optional. The number of characters to extract.
 */
String.prototype.substr = function(start, length) {};

/**
 * @brief The substring() method returns a subset of a string between one index and another, or through the end of the string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/substring
 * @param indexA - An integer between 0 and the length of the string, specifying the offset into the string of the first character to include in the returned substring.
 * @param indexB - Optional. An integer between 0 and the length of the string, which specifies the offset into the string of the first character not to include in the returned substring.
 */
String.prototype.substring = function(indexA, indexB) {};

/**
 * @brief The sup() method creates a &lt;sup> HTML element that causes a string to be displayed as superscript.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/sup
 */
String.prototype.sup = function() {};

/**
 * @brief The toLocaleLowerCase() method returns the calling string value converted to lower case, according to any locale-specific case mappings.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toLocaleLowerCase
 */
String.prototype.toLocaleLowerCase = function() {};

/**
 * @brief The toLocaleUpperCase() method returns the calling string value converted to upper case, according to any locale-specific case mappings.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toLocaleUpperCase
 */
String.prototype.toLocaleUpperCase = function() {};

/**
 * @brief The toLowerCase() method returns the calling string value converted to lowercase.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toLowerCase
 */
String.prototype.toLowerCase = function() {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toSource
 */
String.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the specified object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toString
 */
String.prototype.toString = function() {};

/**
 * @brief The toUpperCase() method returns the calling string value converted to uppercase.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/toUpperCase
 */
String.prototype.toUpperCase = function() {};

/**
 * @brief The trim() method removes whitespace from both ends of a string. Whitespace in this context is all the whitespace characters (space, tab, no-break space, etc.) and all the line terminator characters (LF, CR, etc.).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/Trim
 */
String.prototype.trim = function() {};

/**
 * @brief The trimLeft() removes whitespace from the left end of the string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/TrimLeft
 */
String.prototype.trimLeft = function() {};

/**
 * @brief The trimRight() method removes whitespace from the right end of the string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/TrimRight
 */
String.prototype.trimRight = function() {};

/**
 * @brief The valueOf() method returns the primitive value of a String object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/valueOf
 */
String.prototype.valueOf = function() {};

/**
 * @brief The [@@iterator]() method returns a new Iterator object that iterates over the code points of a String value, returning each code point as a String value.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/@@iterator
 */
String.prototype['@@iterator'] = function() {};

