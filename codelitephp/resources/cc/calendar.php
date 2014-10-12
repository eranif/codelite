<?php

// Start of calendar v.

/**
 * @param juliandaycount
 */
function jdtogregorian ($juliandaycount) {}

/**
 * @param month
 * @param day
 * @param year
 */
function gregoriantojd ($month, $day, $year) {}

/**
 * @param juliandaycount
 */
function jdtojulian ($juliandaycount) {}

/**
 * @param month
 * @param day
 * @param year
 */
function juliantojd ($month, $day, $year) {}

/**
 * @param juliandaycount
 * @param hebrew[optional]
 * @param fl[optional]
 */
function jdtojewish ($juliandaycount, $hebrew, $fl) {}

/**
 * @param month
 * @param day
 * @param year
 */
function jewishtojd ($month, $day, $year) {}

/**
 * @param juliandaycount
 */
function jdtofrench ($juliandaycount) {}

/**
 * @param month
 * @param day
 * @param year
 */
function frenchtojd ($month, $day, $year) {}

/**
 * @param juliandaycount
 * @param mode[optional]
 */
function jddayofweek ($juliandaycount, $mode) {}

/**
 * @param juliandaycount
 * @param mode
 */
function jdmonthname ($juliandaycount, $mode) {}

/**
 * @param year[optional]
 */
function easter_date ($year) {}

/**
 * @param year[optional]
 * @param method[optional]
 */
function easter_days ($year, $method) {}

/**
 * @param timestamp[optional]
 */
function unixtojd ($timestamp) {}

/**
 * @param jday
 */
function jdtounix ($jday) {}

/**
 * @param calendar
 * @param month
 * @param day
 * @param year
 */
function cal_to_jd ($calendar, $month, $day, $year) {}

/**
 * @param jd
 * @param calendar
 */
function cal_from_jd ($jd, $calendar) {}

/**
 * @param calendar
 * @param month
 * @param year
 */
function cal_days_in_month ($calendar, $month, $year) {}

/**
 * @param calendar[optional]
 */
function cal_info ($calendar) {}

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
