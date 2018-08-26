<?php

/**
 * @property MyType1 $prop1
 * @property-read MyType2 $prop2
 * @property-write MyType3 $prop3
 */
class ClassWithProps {
    public $real_prop;
}

$foo = new ClassWithProps();
$foo-> // 4 options

