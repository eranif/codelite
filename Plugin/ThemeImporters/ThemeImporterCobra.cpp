#include "ThemeImporterCobra.hpp"

ThemeImporterCobra::ThemeImporterCobra()
{
    SetKeywords0("abstract adds all and any as assert base body bool branch break callable catch char class const "
                 "continue cue decimal def do dynamic each else end ensure enum event every except expect extend "
                 "extern fake false finally float for from get has if ignore implements implies import in inherits "
                 "inlined inout int interface internal invariant is listen lock mixin must namespace new nil "
                 "nonvirtual not number objc of old or out override partial pass passthrough post print private pro "
                 "protected public raise ref require return same set shared sig stop struct success test this throw to "
                 "to? trace true try uint use using var vari virtual where while yield");
    SetFileExtensions("*.cobra;cob");
    m_langName = "cobra";
}

ThemeImporterCobra::~ThemeImporterCobra() {}
