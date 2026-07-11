<?php

class test_variable_assigned_from_function_return_value {
    public function foo() {}
}

/** @return test_variable_assigned_from_function_return_value */
function globalFunc() {}

class test_variable_assigned_from_function {
    public function baz() {
        $a = globalFunc();
        $a-> // $a is of type test_variable_assigned_from_function_return_value, and suggest 1 member 'foo'