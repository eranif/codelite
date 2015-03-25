
/**
 * @brief Creates a JavaScript Date instance that represents a single moment in time. Date objects are based on a time value that is the number of milliseconds since 1 January, 1970 UTC.
 * @link https://developer.mozilla.orghttps://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date
 */
function Date() {

	/**
	 * @brief The Date.UTC() method accepts the same parameters as the longest form of the constructor, and returns the number of milliseconds in a Date object since January 1, 1970, 00:00:00, universal time.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/UTC
	 * @param year - A year after 1900.
	 * @param month - An integer between 0 and 11 representing the month.
	 * @param day - Optional. An integer between 1 and 31 representing the day of the month.
	 * @param hour - Optional. An integer between 0 and 23 representing the hours.
	 * @param minute - Optional. An integer between 0 and 59 representing the minutes.
	 * @param second - Optional. An integer between 0 and 59 representing the seconds.
	 * @param millisecond - Optional. An integer between 0 and 999 representing the milliseconds.
	 */
	this.UTC = function(year, month, day, hour, minute, second, millisecond) {};

	/**
	 * @brief The Date.now() method returns the number of milliseconds elapsed since 1 January 1970 00:00:00 UTC.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/now
	 */
	this.now = function() {};

	/**
	 * @brief The Date.parse() method parses a string representation of a date, and returns the number of milliseconds since January 1, 1970, 00:00:00 UTC.
	 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/parse
	 * @param dateString - A string representing an RFC2822 or ISO 8601 date (other formats may be used, but results may be unexpected).
	 */
	this.parse = function(dateString) {};

}

/**
 * @brief The getDate() method returns the day of the month for the specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getDate
 */
Date.prototype.getDate = function() {};

/**
 * @brief The getDay() method returns the day of the week for the specified date according to local time, where 0 represents Sunday.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getDay
 */
Date.prototype.getDay = function() {};

/**
 * @brief The getFullYear() method returns the year of the specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getFullYear
 */
Date.prototype.getFullYear = function() {};

/**
 * @brief The getHours() method returns the hour for the specified date, according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getHours
 */
Date.prototype.getHours = function() {};

/**
 * @brief The getMilliseconds() method returns the milliseconds in the specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getMilliseconds
 */
Date.prototype.getMilliseconds = function() {};

/**
 * @brief The getMinutes() method returns the minutes in the specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getMinutes
 */
Date.prototype.getMinutes = function() {};

/**
 * @brief The getMonth() method returns the month in the specified date according to local time, as a zero-based value (where zero indicates the first month of the year).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getMonth
 */
Date.prototype.getMonth = function() {};

/**
 * @brief The getSeconds() method returns the seconds in the specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getSeconds
 */
Date.prototype.getSeconds = function() {};

/**
 * @brief The getTime() method returns the numeric value corresponding to the time for the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getTime
 */
Date.prototype.getTime = function() {};

/**
 * @brief The getTimezoneOffset() method returns the time-zone offset from UTC, in minutes, for the current locale.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getTimezoneOffset
 */
Date.prototype.getTimezoneOffset = function() {};

/**
 * @brief The getUTCDate() method returns the day (date) of the month in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCDate
 */
Date.prototype.getUTCDate = function() {};

/**
 * @brief The getUTCDay() method returns the day of the week in the specified date according to universal time, where 0 represents Sunday.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCDay
 */
Date.prototype.getUTCDay = function() {};

/**
 * @brief The getUTCFullYear() method returns the year in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCFullYear
 */
Date.prototype.getUTCFullYear = function() {};

/**
 * @brief The getUTCHours() method returns the hours in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCHours
 */
Date.prototype.getUTCHours = function() {};

/**
 * @brief The getUTCMilliseconds() method returns the milliseconds in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCMilliseconds
 */
Date.prototype.getUTCMilliseconds = function() {};

/**
 * @brief The getUTCMinutes() method returns the minutes in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCMinutes
 */
Date.prototype.getUTCMinutes = function() {};

/**
 * @brief The getUTCMonth() returns the month of the specified date according to universal time, as a zero-based value (where zero indicates the first month of the year).
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCMonth
 */
Date.prototype.getUTCMonth = function() {};

/**
 * @brief The getUTCSeconds() method returns the seconds in the specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getUTCSeconds
 */
Date.prototype.getUTCSeconds = function() {};

/**
 * @brief This deprecated API should no longer be used, but will probably still work.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getYear
 */
Date.prototype.setDate = function() {};

/**
 * @brief The setFullYear() method sets the full year for a specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setFullYear
 * @param yearValue - An integer specifying the numeric value of the year, for example, 1995.
 * @param monthValue - Optional. An integer between 0 and 11 representing the months January through December.
 * @param dayValue - Optional. An integer between 1 and 31 representing the day of the month. If you specify the dayValue parameter, you must also specify the monthValue.
 */
Date.prototype.setFullYear = function(yearValue, monthValue, dayValue) {};

/**
 * @brief The setHours() method sets the hours for a specified date according to local time, and returns the number of milliseconds since 1 January 1970 00:00:00 UTC until the time represented by the updated Date instance.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setHours
 * @param hoursValue - An integer between 0 and 23, representing the hour.
 * @param minutesValue - Optional. An integer between 0 and 59, representing the minutes.
 * @param secondsValue - Optional. An integer between 0 and 59, representing the seconds. If you specify the secondsValue parameter, you must also specify the minutesValue.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds. If you specify the msValue parameter, you must also specify the minutesValue and secondsValue.
 */
Date.prototype.setHours = function(hoursValue, minutesValue, secondsValue, msValue) {};

/**
 * @brief The setMilliseconds() method sets the milliseconds for a specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setMilliseconds
 * @param millisecondsValue - A number between 0 and 999, representing the milliseconds.
 */
Date.prototype.setMilliseconds = function(millisecondsValue) {};

/**
 * @brief The setMinutes() method sets the minutes for a specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setMinutes
 * @param minutesValue - An integer between 0 and 59, representing the minutes.
 * @param secondsValue - Optional. An integer between 0 and 59, representing the seconds. If you specify the secondsValue parameter, you must also specify the minutesValue.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds. If you specify the msValue parameter, you must also specify the minutesValue and secondsValue.
 */
Date.prototype.setMinutes = function(minutesValue, secondsValue, msValue) {};

/**
 * @brief The setMonth() method sets the month for a specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setMonth
 */
Date.prototype.setMonth = function() {};

/**
 * @brief The setSeconds() method sets the seconds for a specified date according to local time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setSeconds
 * @param secondsValue - An integer between 0 and 59, representing the seconds.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds.
 */
Date.prototype.setSeconds = function(secondsValue, msValue) {};

/**
 * @brief The setTime() method sets the Date object to the time represented by a number of milliseconds since January 1, 1970, 00:00:00 UTC.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setTime
 * @param timeValue - An integer representing the number of milliseconds since 1 January 1970, 00:00:00 UTC.
 */
Date.prototype.setTime = function(timeValue) {};

/**
 * @brief The setUTCDate() method sets the day of the month for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCDate
 * @param dayValue - An integer from 1 to 31, representing the day of the month.
 */
Date.prototype.setUTCDate = function(dayValue) {};

/**
 * @brief The setUTCFullYear() method sets the full year for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCFullYear
 * @param yearValue - An integer specifying the numeric value of the year, for example, 1995.
 * @param monthValue - Optional. An integer between 0 and 11 representing the months January through December.
 * @param dayValue - Optional. An integer between 1 and 31 representing the day of the month. If you specify the dayValue parameter, you must also specify the monthValue.
 */
Date.prototype.setUTCFullYear = function(yearValue, monthValue, dayValue) {};

/**
 * @brief The setUTCHours() method sets the hour for a specified date according to universal time, and returns the number of milliseconds since 1 January 1970 00:00:00 UTC until the time represented by the updated Date instance.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCHours
 * @param hoursValue - An integer between 0 and 23, representing the hour.
 * @param minutesValue - Optional. An integer between 0 and 59, representing the minutes.
 * @param secondsValue - Optional. An integer between 0 and 59, representing the seconds. If you specify the secondsValue parameter, you must also specify the minutesValue.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds. If you specify the msValue parameter, you must also specify the minutesValue and secondsValue.
 */
Date.prototype.setUTCHours = function(hoursValue, minutesValue, secondsValue, msValue) {};

/**
 * @brief The setUTCMilliseconds() method sets the milliseconds for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCMilliseconds
 * @param millisecondsValue - A number between 0 and 999, representing the milliseconds.
 */
Date.prototype.setUTCMilliseconds = function(millisecondsValue) {};

/**
 * @brief The setUTCMinutes() method sets the minutes for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCMinutes
 * @param minutesValue - An integer between 0 and 59, representing the minutes.
 * @param secondsValue - Optional. An integer between 0 and 59, representing the seconds. If you specify the secondsValue parameter, you must also specify the minutesValue.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds. If you specify the msValue parameter, you must also specify the minutesValue and secondsValue.
 */
Date.prototype.setUTCMinutes = function(minutesValue, secondsValue, msValue) {};

/**
 * @brief The setUTCMonth() method sets the month for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCMonth
 * @param monthValue - An integer between 0 and 11, representing the months January through December.
 * @param dayValue - Optional. An integer from 1 to 31, representing the day of the month.
 */
Date.prototype.setUTCMonth = function(monthValue, dayValue) {};

/**
 * @brief The setUTCSeconds() method sets the seconds for a specified date according to universal time.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setUTCSeconds
 * @param secondsValue - An integer between 0 and 59, representing the seconds.
 * @param msValue - Optional. A number between 0 and 999, representing the milliseconds.
 */
Date.prototype.setUTCSeconds = function(secondsValue, msValue) {};

/**
 * @brief This deprecated API should no longer be used, but will probably still work.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/setYear
 * @param yearValue - An integer.
 */
Date.prototype.toDateString = function(yearValue) {};

/**
 * @brief The toGMTString() method converts a date to a string, using Internet Greenwich Mean Time (GMT) conventions. The exact format of the value returned by toGMTString() varies according to the platform and browser, in general it should represent a human readable date string.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toGMTString
 */
Date.prototype.toGMTString = function() {};

/**
 * @brief The toJSON() method returns a string representation of the Date object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toISOString
 */
Date.prototype.toJSON = function() {};

/**
 * @brief The toLocaleDateString() method returns a string with a language sensitive representation of the date portion of this date. The new locales and options arguments let applications specify the language whose formatting conventions should be used and allow to customize the behavior of the function. In older implementations, which ignore the locales and options arguments, the locale used and the form of the string returned are entirely implementation dependent.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toLocaleDateString
 * @param locales - Optional. A string with a BCP 47 language tag, or an array of such strings. For the general form and interpretation of the locales argument, see the Intl page. The following Unicode extension keys are allowed:  nu   Numbering system. Possible values include: "arab", "arabext", "bali", "beng", "deva", "fullwide", "gujr", "guru", "hanidec", "khmr", "knda", "laoo", "latn", "limb", "mlym", "mong", "mymr", "orya", "tamldec", "telu", "thai", "tibt".
 * @param ca - Calendar. Possible values include: "buddhist", "chinese", "coptic", "ethioaa", "ethiopic", "gregory", "hebrew", "indian", "islamic", "islamicc", "iso8601", "japanese", "persian", "roc".
 * @param options - Optional. An object with some or all of the following properties:  localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
 * @param timeZone - The time zone to use. The only value implementations must recognize is "UTC"; the default is the runtime's default time zone. Implementations may also recognize the time zone names of the IANA time zone database, such as "Asia/Shanghai", "Asia/Kolkata", "America/New_York".
 * @param hour12 - Whether to use 12-hour time (as opposed to 24-hour time). Possible values are true and false; the default is locale dependent.
 * @param formatMatcher - The format matching algorithm to use. Possible values are "basic" and "best fit"; the default is "best fit". See the following paragraphs for information about the use of this property.
 * @param weekday - The representation of the weekday. Possible values are "narrow", "short", "long".
 * @param era - The representation of the era. Possible values are "narrow", "short", "long".
 * @param year - The representation of the year. Possible values are "numeric", "2-digit".
 * @param month - The representation of the month. Possible values are "numeric", "2-digit", "narrow", "short", "long".
 * @param day - The representation of the day. Possible values are "numeric", "2-digit".
 * @param hour - The representation of the hour. Possible values are "numeric", "2-digit".
 * @param minute - The representation of the minute. Possible values are "numeric", "2-digit".
 * @param second - The representation of the second. Possible values are "numeric", "2-digit".
 * @param timeZoneName - The representation of the time zone name. Possible values are "short", "long".
 */
Date.prototype.toLocaleDateString = function(locales, ca, options, timeZone, hour12, formatMatcher, weekday, era, year, month, day, hour, minute, second, timeZoneName) {};

/**
 * @brief The non-standard toLocaleFormat() method converts a date to a string using the specified formatting. Intl.DateTimeFormat is an alternative to format dates in a standards-compliant way. See also the newer version of Date.prototype.toLocaleDateString().
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toLocaleFormat
 * @param formatString - A format string in the same format expected by the strftime() function in C.
 */
Date.prototype.toLocaleFormat = function(formatString) {};

/**
 * @brief The toLocaleString() method returns a string with a language sensitive representation of this date. The new locales and options arguments let applications specify the language whose formatting conventions should be used and customize the behavior of the function. In older implementations, which ignore the locales and options arguments, the locale used and the form of the string returned are entirely implementation dependent.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toLocaleString
 * @param locales - Optional. A string with a BCP 47 language tag, or an array of such strings. For the general form and interpretation of the locales argument, see the Intl page. The following Unicode extension keys are allowed:  nu   Numbering system. Possible values include: "arab", "arabext", "bali", "beng", "deva", "fullwide", "gujr", "guru", "hanidec", "khmr", "knda", "laoo", "latn", "limb", "mlym", "mong", "mymr", "orya", "tamldec", "telu", "thai", "tibt".
 * @param ca - Calendar. Possible values include: "buddhist", "chinese", "coptic", "ethioaa", "ethiopic", "gregory", "hebrew", "indian", "islamic", "islamicc", "iso8601", "japanese", "persian", "roc".
 * @param options - Optional. An object with some or all of the following properties:  localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
 * @param timeZone - The time zone to use. The only value implementations must recognize is "UTC"; the default is the runtime's default time zone. Implementations may also recognize the time zone names of the IANA time zone database, such as "Asia/Shanghai", "Asia/Kolkata", "America/New_York".
 * @param hour12 - Whether to use 12-hour time (as opposed to 24-hour time). Possible values are true and false; the default is locale dependent.
 * @param formatMatcher - The format matching algorithm to use. Possible values are "basic" and "best fit"; the default is "best fit". See the following paragraphs for information about the use of this property.
 * @param weekday - The representation of the weekday. Possible values are "narrow", "short", "long".
 * @param era - The representation of the era. Possible values are "narrow", "short", "long".
 * @param year - The representation of the year. Possible values are "numeric", "2-digit".
 * @param month - The representation of the month. Possible values are "numeric", "2-digit", "narrow", "short", "long".
 * @param day - The representation of the day. Possible values are "numeric", "2-digit".
 * @param hour - The representation of the hour. Possible values are "numeric", "2-digit".
 * @param minute - The representation of the minute. Possible values are "numeric", "2-digit".
 * @param second - The representation of the second. Possible values are "numeric", "2-digit".
 * @param timeZoneName - The representation of the time zone name. Possible values are "short", "long".
 */
Date.prototype.toLocaleString = function(locales, ca, options, timeZone, hour12, formatMatcher, weekday, era, year, month, day, hour, minute, second, timeZoneName) {};

/**
 * @brief The toLocaleTimeString() method returns a string with a language sensitive representation of the time portion of this date. The new locales and options arguments let applications specify the language whose formatting conventions should be used and customize the behavior of the function. In older implementations, which ignore the locales and options arguments, the locale used and the form of the string returned are entirely implementation dependent.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toLocaleTimeString
 * @param locales - Optional. A string with a BCP 47 language tag, or an array of such strings. For the general form and interpretation of the locales argument, see the Intl page. The following Unicode extension keys are allowed:  nu   Numbering system. Possible values include: "arab", "arabext", "bali", "beng", "deva", "fullwide", "gujr", "guru", "hanidec", "khmr", "knda", "laoo", "latn", "limb", "mlym", "mong", "mymr", "orya", "tamldec", "telu", "thai", "tibt".
 * @param ca - Calendar. Possible values include: "buddhist", "chinese", "coptic", "ethioaa", "ethiopic", "gregory", "hebrew", "indian", "islamic", "islamicc", "iso8601", "japanese", "persian", "roc".
 * @param options - Optional. An object with some or all of the following properties:  localeMatcher   The locale matching algorithm to use. Possible values are "lookup" and "best fit"; the default is "best fit". For information about this option, see the Intl page.
 * @param timeZone - The time zone to use. The only value implementations must recognize is "UTC"; the default is the runtime's default time zone. Implementations may also recognize the time zone names of the IANA time zone database, such as "Asia/Shanghai", "Asia/Kolkata", "America/New_York".
 * @param hour12 - Whether to use 12-hour time (as opposed to 24-hour time). Possible values are true and false; the default is locale dependent.
 * @param formatMatcher - The format matching algorithm to use. Possible values are "basic" and "best fit"; the default is "best fit". See the following paragraphs for information about the use of this property.
 * @param weekday - The representation of the weekday. Possible values are "narrow", "short", "long".
 * @param era - The representation of the era. Possible values are "narrow", "short", "long".
 * @param year - The representation of the year. Possible values are "numeric", "2-digit".
 * @param month - The representation of the month. Possible values are "numeric", "2-digit", "narrow", "short", "long".
 * @param day - The representation of the day. Possible values are "numeric", "2-digit".
 * @param hour - The representation of the hour. Possible values are "numeric", "2-digit".
 * @param minute - The representation of the minute. Possible values are "numeric", "2-digit".
 * @param second - The representation of the second. Possible values are "numeric", "2-digit".
 * @param timeZoneName - The representation of the time zone name. Possible values are "short", "long".
 */
Date.prototype.toLocaleTimeString = function(locales, ca, options, timeZone, hour12, formatMatcher, weekday, era, year, month, day, hour, minute, second, timeZoneName) {};

/**
 * @brief The toSource() method returns a string representing the source code of the object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toSource
 */
Date.prototype.toSource = function() {};

/**
 * @brief The toString() method returns a string representing the specified Date object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toString
 */
Date.prototype.toString = function() {};

/**
 * @brief The toTimeString() method returns the time portion of a Date object in human readable form in American English.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toTimeString
 */
Date.prototype.toTimeString = function() {};

/**
 * @brief The toUTCString() method converts a date to a string, using the UTC time zone.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/toUTCString
 */
Date.prototype.toUTCString = function() {};

/**
 * @brief The valueOf() method returns the primitive value of a Date object.
 * @link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/valueOf
 */
Date.prototype.valueOf = function() {};

