<?php

// Start of tokenizer v.0.1

/**
 * Split given source into PHP tokens
 * @link http://www.php.net/manual/en/function.token-get-all.php
 * @param source string <p>
 *       The PHP source to parse.
 *      </p>
 * @return array An array of token identifiers. Each individual token identifier is either
 *   a single character (i.e.: ;, ., 
 *   >, !, etc...),
 *   or a three element array containing the token index in element 0, the string
 *   content of the original token in element 1 and the line number in element 2.
 */
function token_get_all ($source) {}

/**
 * Get the symbolic name of a given PHP token
 * @link http://www.php.net/manual/en/function.token-name.php
 * @param token int <p>
 *       The token value.
 *      </p>
 * @return string The symbolic name of the given token.
 */
function token_name ($token) {}

define ('T_REQUIRE_ONCE', 258);
define ('T_REQUIRE', 259);
define ('T_EVAL', 260);
define ('T_INCLUDE_ONCE', 261);
define ('T_INCLUDE', 262);
define ('T_LOGICAL_OR', 263);
define ('T_LOGICAL_XOR', 264);
define ('T_LOGICAL_AND', 265);
define ('T_PRINT', 266);
define ('T_YIELD', 267);
define ('T_POW_EQUAL', 268);
define ('T_SR_EQUAL', 269);
define ('T_SL_EQUAL', 270);
define ('T_XOR_EQUAL', 271);
define ('T_OR_EQUAL', 272);
define ('T_AND_EQUAL', 273);
define ('T_MOD_EQUAL', 274);
define ('T_CONCAT_EQUAL', 275);
define ('T_DIV_EQUAL', 276);
define ('T_MUL_EQUAL', 277);
define ('T_MINUS_EQUAL', 278);
define ('T_PLUS_EQUAL', 279);
define ('T_BOOLEAN_OR', 280);
define ('T_BOOLEAN_AND', 281);
define ('T_IS_NOT_IDENTICAL', 282);
define ('T_IS_IDENTICAL', 283);
define ('T_IS_NOT_EQUAL', 284);
define ('T_IS_EQUAL', 285);
define ('T_IS_GREATER_OR_EQUAL', 286);
define ('T_IS_SMALLER_OR_EQUAL', 287);
define ('T_SR', 288);
define ('T_SL', 289);
define ('T_INSTANCEOF', 290);
define ('T_UNSET_CAST', 291);
define ('T_BOOL_CAST', 292);
define ('T_OBJECT_CAST', 293);
define ('T_ARRAY_CAST', 294);
define ('T_STRING_CAST', 295);
define ('T_DOUBLE_CAST', 296);
define ('T_INT_CAST', 297);
define ('T_DEC', 298);
define ('T_INC', 299);
define ('T_POW', 300);
define ('T_CLONE', 301);
define ('T_NEW', 302);
define ('T_EXIT', 303);
define ('T_IF', 304);
define ('T_ELSEIF', 305);
define ('T_ELSE', 306);
define ('T_ENDIF', 307);
define ('T_LNUMBER', 308);
define ('T_DNUMBER', 309);
define ('T_STRING', 310);
define ('T_STRING_VARNAME', 311);
define ('T_VARIABLE', 312);
define ('T_NUM_STRING', 313);
define ('T_INLINE_HTML', 314);
define ('T_CHARACTER', 315);
define ('T_BAD_CHARACTER', 316);
define ('T_ENCAPSED_AND_WHITESPACE', 317);
define ('T_CONSTANT_ENCAPSED_STRING', 318);
define ('T_ECHO', 319);
define ('T_DO', 320);
define ('T_WHILE', 321);
define ('T_ENDWHILE', 322);
define ('T_FOR', 323);
define ('T_ENDFOR', 324);
define ('T_FOREACH', 325);
define ('T_ENDFOREACH', 326);
define ('T_DECLARE', 327);
define ('T_ENDDECLARE', 328);
define ('T_AS', 329);
define ('T_SWITCH', 330);
define ('T_ENDSWITCH', 331);
define ('T_CASE', 332);
define ('T_DEFAULT', 333);
define ('T_BREAK', 334);
define ('T_CONTINUE', 335);
define ('T_GOTO', 336);
define ('T_FUNCTION', 337);
define ('T_CONST', 338);
define ('T_RETURN', 339);
define ('T_TRY', 340);
define ('T_CATCH', 341);
define ('T_FINALLY', 342);
define ('T_THROW', 343);
define ('T_USE', 344);
define ('T_INSTEADOF', 345);
define ('T_GLOBAL', 346);
define ('T_PUBLIC', 347);
define ('T_PROTECTED', 348);
define ('T_PRIVATE', 349);
define ('T_FINAL', 350);
define ('T_ABSTRACT', 351);
define ('T_STATIC', 352);
define ('T_VAR', 353);
define ('T_UNSET', 354);
define ('T_ISSET', 355);
define ('T_EMPTY', 356);
define ('T_HALT_COMPILER', 357);
define ('T_CLASS', 358);
define ('T_TRAIT', 359);
define ('T_INTERFACE', 360);
define ('T_EXTENDS', 361);
define ('T_IMPLEMENTS', 362);
define ('T_OBJECT_OPERATOR', 363);
define ('T_DOUBLE_ARROW', 364);
define ('T_LIST', 365);
define ('T_ARRAY', 366);
define ('T_CALLABLE', 367);
define ('T_CLASS_C', 368);
define ('T_TRAIT_C', 369);
define ('T_METHOD_C', 370);
define ('T_FUNC_C', 371);
define ('T_LINE', 372);
define ('T_FILE', 373);
define ('T_COMMENT', 374);
define ('T_DOC_COMMENT', 375);
define ('T_OPEN_TAG', 376);
define ('T_OPEN_TAG_WITH_ECHO', 377);
define ('T_CLOSE_TAG', 378);
define ('T_WHITESPACE', 379);
define ('T_START_HEREDOC', 380);
define ('T_END_HEREDOC', 381);
define ('T_DOLLAR_OPEN_CURLY_BRACES', 382);
define ('T_CURLY_OPEN', 383);
define ('T_PAAMAYIM_NEKUDOTAYIM', 384);
define ('T_NAMESPACE', 385);
define ('T_NS_C', 386);
define ('T_DIR', 387);
define ('T_NS_SEPARATOR', 388);
define ('T_ELLIPSIS', 389);
define ('T_DOUBLE_COLON', 384);

// End of tokenizer v.0.1
?>
