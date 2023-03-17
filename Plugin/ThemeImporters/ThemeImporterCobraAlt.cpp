#include "ThemeImporterCobraAlt.hpp"

ThemeImporterCobraAlt::ThemeImporterCobraAlt()
{
    SetKeywords0("adds all and any as assert base body bool branch break callable catch char class const continue cue "
                 "decimal def do dynamic each else end ensure enum event every except expect extend false finally "
                 "float for from get has if ignore implements implies import in inherits inlined int interface "
                 "invariant is listen lock mixin must namespace new nil not number objc of old or pass passthrough "
                 "post print pro raise ref require return same set sig stop struct success test this throw to trace "
                 "true try uint use using var where while yield");
    SetKeywords1(
        "abstract fake internal nonvirtual private protected public shared vari inout out override partial virtual ");
    SetFileExtensions("*.cobra-A");
    m_langName = "cobraalt";
}

ThemeImporterCobraAlt::~ThemeImporterCobraAlt() {}
