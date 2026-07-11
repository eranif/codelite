<?php


class StructA {
    public $a;
}

class StructB extends StructA {
    public $b;
}

class ClassA {
    protected $struct;

    public function __construct(StructA $a) {
        $this->struct = $a;
    }
}

class ClassB extends ClassA {
    /** @var StructB $struct */
    public function __construct(StructB $b) {
        $this->struct = $b;
    }
}
$clsb = new ClassB();
$clsb->struct-> // 2 options $a and $b

