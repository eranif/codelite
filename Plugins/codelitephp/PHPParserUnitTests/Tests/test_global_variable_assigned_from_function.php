<?php

class test_global_variable_assigned_from_function_return_value {
    public function foo() {}
}

/** @return test_global_variable_assigned_from_function_return_value */
function globalFunc() {}

$a = globalFunc();
$a-> // $a is of type test_global_variable_assigned_from_function_return_value, and suggest 1 member 'foo'