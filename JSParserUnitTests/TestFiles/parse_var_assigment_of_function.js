function Boolean()
{
    this.isTrue = 1;
    this.name = "";
}


function String() {
    this.asString = "";
    this.prototype.toString = function() { return this.asString; }
}

var t = function() {
    if(1) {
        return false;
    } else {
        return "";
    }
}

t. // code complete entries form 'Boolean'