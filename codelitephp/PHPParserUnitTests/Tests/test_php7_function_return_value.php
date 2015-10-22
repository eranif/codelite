<?php

class test_php7_function_return_value_class {
    $name = "";
    $value = "";
}

function foo() : test_php7_function_return_value_class {
    return $somthing;
}

foo()-> // the resolved type here should be \test_php7_function_return_value_class
