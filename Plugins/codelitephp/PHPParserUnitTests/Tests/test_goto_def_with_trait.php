<?php

trait test_goto_def_with_trait_impl {
    public function _prepareFixtures() {}
}

class test_goto_def_with_trait extends test_goto_def_with_trait_base_class
{
    use test_goto_def_with_trait_impl;

    public static function setUpBeforeClass() {
        self::