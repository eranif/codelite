<?php

class test_class_extends_base___1 {
    public function test_class_extends_base_func1() {
    }
}

class test_class_extends extends  test_class_extends_base___1 {
    public function child_function() {}
}

$a = new test_class_extends();
$a-> // should suggest 2 entries