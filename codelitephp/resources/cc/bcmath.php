<?php

// Start of bcmath v.

/**
 * Add two arbitrary precision numbers
 * @link http://www.php.net/manual/en/function.bcadd.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string The sum of the two operands, as a string.
 */
function bcadd ($left_operand, $right_operand, $scale = null) {}

/**
 * Subtract one arbitrary precision number from another
 * @link http://www.php.net/manual/en/function.bcsub.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string The result of the subtraction, as a string.
 */
function bcsub ($left_operand, $right_operand, $scale = null) {}

/**
 * Multiply two arbitrary precision number
 * @link http://www.php.net/manual/en/function.bcmul.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string the result as a string.
 */
function bcmul ($left_operand, $right_operand, $scale = null) {}

/**
 * Divide two arbitrary precision numbers
 * @link http://www.php.net/manual/en/function.bcdiv.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string the result of the division as a string, or &null; if 
 *   right_operand is 0.
 */
function bcdiv ($left_operand, $right_operand, $scale = null) {}

/**
 * Get modulus of an arbitrary precision number
 * @link http://www.php.net/manual/en/function.bcmod.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param modulus string <p>
 *       The modulus, as a string.
 *      </p>
 * @return string the modulus as a string, or &null; if 
 *   modulus is 0.
 */
function bcmod ($left_operand, $modulus) {}

/**
 * Raise an arbitrary precision number to another
 * @link http://www.php.net/manual/en/function.bcpow.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string the result as a string.
 */
function bcpow ($left_operand, $right_operand, $scale = null) {}

/**
 * Get the square root of an arbitrary precision number
 * @link http://www.php.net/manual/en/function.bcsqrt.php
 * @param operand string <p>
 *       The operand, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string the square root as a string, or &null; if 
 *   operand is negative.
 */
function bcsqrt ($operand, $scale = null) {}

/**
 * Set default scale parameter for all bc math functions
 * @link http://www.php.net/manual/en/function.bcscale.php
 * @param scale int <p>
 *       The scale factor.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function bcscale ($scale) {}

/**
 * Compare two arbitrary precision numbers
 * @link http://www.php.net/manual/en/function.bccomp.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param scale int[optional] <p>
 *       The optional scale parameter is used to set the
 *       number of digits after the decimal place which will be used in the
 *       comparison.  
 *      </p>
 * @return int 0 if the two operands are equal, 1 if the
 *   left_operand is larger than the 
 *   right_operand, -1 otherwise.
 */
function bccomp ($left_operand, $right_operand, $scale = null) {}

/**
 * Raise an arbitrary precision number to another, reduced by a specified modulus
 * @link http://www.php.net/manual/en/function.bcpowmod.php
 * @param left_operand string <p>
 *       The left operand, as a string.
 *      </p>
 * @param right_operand string <p>
 *       The right operand, as a string.
 *      </p>
 * @param modulus string <p>
 *       The modulus, as a string.
 *      </p>
 * @param scale int[optional] 
 * @return string the result as a string, or &null; if modulus
 *   is 0.
 */
function bcpowmod ($left_operand, $right_operand, $modulus, $scale = null) {}

// End of bcmath v.
?>
