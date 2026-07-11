<?php

abstract class AbstractFoo 
{
    static public $myStaticVar;
    abstract public function foo();
    abstract public function bar();
    abstract public function baz();
}

class test_parsing_abstract_class_impl extends AbstractFoo
{
    public function foo(){}
    public function bar(){}
    public function baz(){}
}

$a = new test_parsing_abstract_class_impl();
$a->