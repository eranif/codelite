<?php

namespace MyLovelyNS;
class MyClass1_In_NS
{
    public function foo1($a, $b) {}
    public function foo2() {}
    public function foo3() {}
}

$clsA = new \MyClass1_In_NS();            // expected res = 0
$clsB = new \MyLovelyNS\MyClass1_In_NS(); // expected res = 3
$clsC = new MyClass1_In_NS();             // expected res = 3
