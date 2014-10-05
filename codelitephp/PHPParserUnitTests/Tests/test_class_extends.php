<?php

class test_class_extends_base {
    public function foo() {
        
    }
}

class test_class_extends extends  test_class_extends_base {
    public function child_function() {}
}

$a = new test_class_extends();
$a-> // should suggest 2 entries