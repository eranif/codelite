<?php

class test_variable_1_return_value {

    public function return_value_1(){
    }

    public function return_value_2(){
    }

    public function return_value_3(){
    }
}

class test_variable_1 {
    /** @return test_variable_1_return_value */
    public function foo(){
    }

    public function bar(){
    }
}
$a = new test_variable_1();
$a->bar();
$a->foo()->