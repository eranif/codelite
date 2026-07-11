<?php

// Prepare the return type classes

class test_php7_function_arg_hinting_type1 {
    protected $member1 = "";
    protected $member2 = "";
    protected $member3 = "";
}

class test_php7_function_arg_hinting_type2 {
    public $member1 = "";
    public $member2 = "";
}

function test_php7_function_arg_hinting_foo(test_php7_function_arg_hinting_type1 $one, 
                                            test_php7_function_arg_hinting_type2 $two) {
    $two-> // resolved type is test_php7_function_arg_hinting_type2, suugest 2 members