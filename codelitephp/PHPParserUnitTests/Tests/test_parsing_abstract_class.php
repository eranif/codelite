<?php

abstract class AbstractFoo 
{
    abstract public function foo();
    abstract public function bar();
    abstract public function baz();
}

$a = new AbstractFoo();
$a-> // 3 entries