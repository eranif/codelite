function Array() {}

function Foo() {
    var a = new Array();
    var b = new Foo();
    var c;
    
    function Baz() {
        var innerVariable;
        var innerVariable2;
    
        // Expecting 5 variables at this point
        