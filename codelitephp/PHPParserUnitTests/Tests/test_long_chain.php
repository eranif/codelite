<?php

class ClassRetVal1() {
    public function foo_1(){}
    public function foo_2(){}
    public function foo_3(){}
}


class ClassRetVal2() {
    /** @return ClassRetVal1 */
    public function bar_1(){}
    public function bar_2(){}
    public function bar_3(){}
}

class ClassRetVal3() {
    /** @return ClassRetVal2 */
    public function baz_1(){}
}
$a = new ClassRetVal3();
$a->baz_1()->bar_1()-> // suggest 3 members (foo_*)