<?php

abstract class AbstractFoo1 
{
    static public $myStaticVar;
    abstract public function foo();
    abstract public function bar();
    abstract public function baz();
}

class test_parsing_abstract_class_impl1 extends AbstractFoo1
{
    public function foo(){}
    public function bar(){}
    public function baz(){
    self:: // Code complete here only 1 entry