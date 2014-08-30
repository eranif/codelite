<?php

class BaseClass
{
    public $memberVariable = null;
    public function baseFunction1() {}
    public function baseFunction2() {}
}

class ChildClass extends BaseClass
{
    public function childFunction() {
    }
}

class ChildClass extends BaseClass
{
    public function childFunction() {
// test: parent:: -> should result in BaseClass functions (2) only (without the member variable)