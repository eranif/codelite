<?php

// Start of calendar v.

/**
 * Converts Julian Day Count to Gregorian date
 * @link http://www.php.net/manual/en/function.jdtogregorian.php
 * @param julianday int <p>
 *       A julian day number as integer
 *      </p>
 * @return string The gregorian date as a string in the form "month/day/year"
 */
function jdtogregorian ($julianday) {}

/**
 * Converts a Gregorian date to Julian Day Count
 * @link http://www.php.net/manual/en/function.gregoriantojd.php
 * @param month int <p>
 *       The month as a number from 1 (for January) to 12 (for December)
 *      </p>
 * @param day int <p>
 *       The day as a number from 1 to 31
 *      </p>
 * @param year int <p>
 *       The year as a number between -4714 and 9999
 *      </p>
 * @return int The julian day for the given gregorian date as an integer.
 */
function gregoriantojd ($month, $day, $year) {}

/**
 * Converts a Julian Day Count to a Julian Calendar Date
 * @link http://www.php.net/manual/en/function.jdtojulian.php
 * @param julianday int <p>
 *       A julian day number as integer
 *      </p>
 * @return string The julian date as a string in the form "month/day/year"
 */
function jdtojulian ($julianday) {}

/**
 * Converts a Julian Calendar date to Julian Day Count
 * @link http://www.php.net/manual/en/function.juliantojd.php
 * @param month int <p>
 *       The month as a number from 1 (for January) to 12 (for December)
 *      </p>
 * @param day int <p>
 *       The day as a number from 1 to 31
 *      </p>
 * @param year int <p>
 *       The year as a number between -4713 and 9999
 *      </p>
 * @return int The julian day for the given julian date as an integer.
 */
function juliantojd ($month, $day, $year) {}

/**
 * Converts a Julian day count to a Jewish calendar date
 * @link http://www.php.net/manual/en/function.jdtojewish.php
 * @param juliandaycount int 
 * @param hebrew bool[optional] <p>
 *       If the hebrew parameter is set to true, the
 *       fl parameter is used for Hebrew, string based,
 *       output format.  
 *      </p>
 * @param fl int[optional] <p>
 *       The available formats are:  
 *       CAL_JEWISH_ADD_ALAFIM_GERESH,
 *       CAL_JEWISH_ADD_ALAFIM,
 *       CAL_JEWISH_ADD_GERESHAYIM.
 *      </p>
 * @return string The jewish date as a string in the form "month/day/year"
 */
function jdtojewish ($juliandaycount, $hebrew = null, $fl = null) {}

/**
 * Converts a date in the Jewish Calendar to Julian Day Count
 * @link http://www.php.net/manual/en/function.jewishtojd.php
 * @param month int <p>
 *       The month as a number from 1 to 13
 *      </p>
 * @param day int <p>
 *       The day as a number from 1 to 30
 *      </p>
 * @param year int <p>
 *       The year as a number between 1 and 9999
 *      </p>
 * @return int The julian day for the given jewish date as an integer.
 */
function jewishtojd ($month, $day, $year) {}

/**
 * Converts a Julian Day Count to the French Republican Calendar
 * @link http://www.php.net/manual/en/function.jdtofrench.php
 * @param juliandaycount int 
 * @return string The french revolution date as a string in the form "month/day/year"
 */
function jdtofrench ($juliandaycount) {}

/**
 * Converts a date from the French Republican Calendar to a Julian Day Count
 * @link http://www.php.net/manual/en/function.frenchtojd.php
 * @param month int <p>
 *       The month as a number from 1 (for Vendémiaire) to 13 (for the period of 5-6 days at the end of each year)
 *      </p>
 * @param day int <p>
 *       The day as a number from 1 to 30
 *      </p>
 * @param year int <p>
 *       The year as a number between 1 and 14
 *      </p>
 * @return int The julian day for the given french revolution date as an integer.
 */
function frenchtojd ($month, $day, $year) {}

/**
 * Returns the day of the week
 * @link http://www.php.net/manual/en/function.jddayofweek.php
 * @param julianday int <p>
 *       A julian day number as integer
 *      </p>
 * @param mode int[optional] <table>
 *       Calendar week modes
 *       
 *        
 *         <tr valign="top">
 *          <td>Mode</td>
 *          <td>Meaning</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>0 (Default)</td> 
 *          <td>
 *           Return the day number as an int (0=Sunday, 1=Monday, etc)
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>1</td> 
 *          <td>
 *           Returns string containing the day of week
 *           (English-Gregorian)
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>2</td> 
 *          <td>
 *           Return a string containing the abbreviated day of week
 *           (English-Gregorian)
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return mixed The gregorian weekday as either an integer or string.
 */
function jddayofweek ($julianday, $mode = null) {}

/**
 * Returns a month name
 * @link http://www.php.net/manual/en/function.jdmonthname.php
 * @param julianday int 
 * @param mode int 
 * @return string The month name for the given Julian Day and calendar.
 */
function jdmonthname ($julianday, $mode) {}

/**
 * Get Unix timestamp for midnight on Easter of a given year
 * @link http://www.php.net/manual/en/function.easter-date.php
 * @param year int[optional] <p>
 *       The year as a number between 1970 an 2037
 *      </p>
 * @return int The easter date as a unix timestamp.
 */
function easter_date ($year = null) {}

/**
 * Get number of days after March 21 on which Easter falls for a given year
 * @link http://www.php.net/manual/en/function.easter-days.php
 * @param year int[optional] <p>
 *       The year as a positive number
 *      </p>
 * @param method int[optional] <p>
 *       Allows to calculate easter dates based
 *       on the Gregorian calendar during the years 1582 - 1752 when set to
 *       CAL_EASTER_ROMAN. See the calendar constants for more valid
 *       constants.      
 *       </p>
 * @return int The number of days after March 21st that the Easter Sunday
 *   is in the given year.
 */
function easter_days ($year = null, $method = null) {}

/**
 * Convert Unix timestamp to Julian Day
 * @link http://www.php.net/manual/en/function.unixtojd.php
 * @param timestamp int[optional] <p>
 *       A unix timestamp to convert.
 *      </p>
 * @return int A julian day number as integer.
 */
function unixtojd ($timestamp = null) {}

/**
 * Convert Julian Day to Unix timestamp
 * @link http://www.php.net/manual/en/function.jdtounix.php
 * @param jday int <p>
 *       A julian day number between 2440588 and 2465342.
 *      </p>
 * @return int The unix timestamp for the start of the given julian day.
 */
function jdtounix ($jday) {}

/**
 * Converts from a supported calendar to Julian Day Count
 * @link http://www.php.net/manual/en/function.cal-to-jd.php
 * @param calendar int <p>
 *       Calendar to convert from, one of 
 *       CAL_GREGORIAN,
 *       CAL_JULIAN,
 *       CAL_JEWISH or
 *       CAL_FRENCH.
 *      </p>
 * @param month int <p>
 *       The month as a number, the valid range depends 
 *       on the calendar
 *      </p>
 * @param day int <p>
 *       The day as a number, the valid range depends 
 *       on the calendar
 *      </p>
 * @param year int <p>
 *       The year as a number, the valid range depends 
 *       on the calendar
 *      </p>
 * @return int A Julian Day number.
 */
function cal_to_jd ($calendar, $month, $day, $year) {}

/**
 * Converts from Julian Day Count to a supported calendar
 * @link http://www.php.net/manual/en/function.cal-from-jd.php
 * @param jd int <p>
 *       Julian day as integer
 *      </p>
 * @param calendar int <p>
 *       Calendar to convert to
 *      </p>
 * @return array an array containing calendar information like month, day, year,
 *   day of week, abbreviated and full names of weekday and month and the
 *   date in string form "month/day/year".
 */
function cal_from_jd ($jd, $calendar) {}

/**
 * Return the number of days in a month for a given year and calendar
 * @link http://www.php.net/manual/en/function.cal-days-in-month.php
 * @param calendar int <p>
 *       Calendar to use for calculation
 *      </p>
 * @param month int <p>
 *       Month in the selected calendar
 *      </p>
 * @param year int <p>
 *       Year in the selected calendar
 *      </p>
 * @return int The length in days of the selected month in the given calendar
 */
function cal_days_in_month ($calendar, $month, $year) {}

/**
 * Returns information about a particular calendar
 * @link http://www.php.net/manual/en/function.cal-info.php
 * @param calendar int[optional] <p>
 *       Calendar to return information for. If no calendar is specified
 *       information about all calendars is returned.
 *      </p>
 * @return array 
 */
function cal_info ($calendar = null) {}

define ('CAL_GREGORIAN', 0);
define ('CAL_JULIAN', 1);
define ('CAL_JEWISH', 2);
define ('CAL_FRENCH', 3);
define ('CAL_NUM_CALS', 4);
define ('CAL_DOW_DAYNO', 0);
define ('CAL_DOW_SHORT', 1);
define ('CAL_DOW_LONG', 2);
define ('CAL_MONTH_GREGORIAN_SHORT', 0);
define ('CAL_MONTH_GREGORIAN_LONG', 1);
define ('CAL_MONTH_JULIAN_SHORT', 2);
define ('CAL_MONTH_JULIAN_LONG', 3);
define ('CAL_MONTH_JEWISH', 4);
define ('CAL_MONTH_FRENCH', 5);
define ('CAL_EASTER_DEFAULT', 0);
define ('CAL_EASTER_ROMAN', 1);
define ('CAL_EASTER_ALWAYS_GREGORIAN', 2);
define ('CAL_EASTER_ALWAYS_JULIAN', 3);
define ('CAL_JEWISH_ADD_ALAFIM_GERESH', 2);
define ('CAL_JEWISH_ADD_ALAFIM', 4);
define ('CAL_JEWISH_ADD_GERESHAYIM', 8);

// End of calendar v.
?>
