<?php

trait MyTrait {

    public function trait_method() {
    }
}

class ClassWithTrait {
    use MyTrait;

    public function a_method() {
    }
}

$test = new ClassWithTrait();
$test-> // 2 methods should be here