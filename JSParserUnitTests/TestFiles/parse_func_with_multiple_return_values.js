
function Boolean() { this.isTrue = false; }
function String() { this.str = "value"; }
function Number() { this.number = 1; }

// a function with multiple return statements
// which results in type: Boolean|Number|String
function Foo() {
    if(one) {
        return false;
    } else if(two) {
        return 10;
    } else {
        return "string";
    }
}

Foo(). // type should be "Boolean|Number|String" and CC should offer CC for all three classes