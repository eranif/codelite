<?php

interface test_interface {

    public function foo();

    public function bar();
}

class test_interface_impl implements test_interface {

    public function foo(){
    }

    public function bar(){
    }
}

$a = new test_interface_impl();
$a-> // suggest 2 functions here