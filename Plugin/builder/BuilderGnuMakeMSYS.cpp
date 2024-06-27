#include "BuilderGnuMakeMSYS.hpp"

BuilderGnuMakeMSYS::BuilderGnuMakeMSYS()
{
    m_name = "CodeLite Makefile Generator - UNIX";
    m_isWindows = false;
    m_isMSYSEnv = true;
}

BuilderGnuMakeMSYS::BuilderGnuMakeMSYS(const wxString& name, const wxString& buildTool,
                                       const wxString& buildToolOptions)
    : BuilderGnuMake(name, buildTool, buildToolOptions)
{
    m_isWindows = false;
    m_isMSYSEnv = true;
}

BuilderGnuMakeMSYS::~BuilderGnuMakeMSYS() {}
