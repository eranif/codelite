<?php


class test_parent {
    public function foo() {
    }
    public function bar() {
    }
}

class test_parent_subclass extends test_parent {
    public function foo_subclass() {}
    public function baz() {
        parent:: // Code complete here 2 matches, ignore foo_subclass()
