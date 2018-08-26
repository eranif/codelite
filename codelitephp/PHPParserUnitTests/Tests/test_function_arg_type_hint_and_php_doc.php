<?php

class test_function_arg_type_hint_and_php_doc {
    public $name = "Hello";
    public $lastName = "World";
}

/**
 * @param mixed db
 */
function foo(\test_function_arg_type_hint_and_php_doc $db) {
    $db->// code complete based on \PDO and not "mixed"