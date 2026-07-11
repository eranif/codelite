<?php


class test_var_assigned_from_require {
    public function foo() {
    }
}

/** @var test_var_assigned_from_require */
$app = require_once __DIR__ . '/../bootstrap/start.php';
$app-> //foo