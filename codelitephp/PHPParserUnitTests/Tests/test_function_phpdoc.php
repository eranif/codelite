<?php

class ArgType {
    public function foo() {}
    public function baz() {}
};

/**
 * @param ArgType $dbh
 */
function test_function_phpdoc($dbh) {
    $dbh->