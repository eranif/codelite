#include "CompilerLocatorRustc.hpp"

#include "build_settings_config.h"
#include "file_logger.h"

CompilerLocatorRustc::CompilerLocatorRustc() {}

CompilerLocatorRustc::~CompilerLocatorRustc() {}

bool CompilerLocatorRustc::Locate()
{
    // This is a dummy compiler, we dont really need it
    // we only need the patterns

    wxString error_pattern = R"re1(^error\[.*?\]:(.*?)$)re1";
    wxString error_pattern2 = R"re2(^error:[ ]+(.*?))re2";
    wxString warn_pattern = R"re3(-->[ ]*([\\\w\./]+):([\d]+):([\d]+))re3";
    wxString warn_pattern1 = R"(:::[ ]*([\\\w\./]+):([\d]+):([\d]+))";
    wxString warn_pattern2 = R"re4(^warning:)re4";
    wxString warn_pattern3 = R"re5(^note:)re5";

    clDEBUG() << "Searching for rustc compiler..." << endl;

    CompilerPtr rustc(new Compiler(nullptr));
    // need to create it
    rustc->SetCompilerFamily("Other");
    rustc->SetName("rustc");
    Compiler::CmpListInfoPattern errPatterns;
    Compiler::CmpListInfoPattern warnPatterns;
    {
        // error[E1234]:
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = error_pattern;
        pattern.columnIndex = "-1";
        pattern.lineNumberIndex = "-1";
        pattern.fileNameIndex = "-1";
        errPatterns.push_back(pattern);
    }
    {
        // error:
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = error_pattern2;
        pattern.columnIndex = "-1";
        pattern.lineNumberIndex = "-1";
        pattern.fileNameIndex = "-1";
        errPatterns.push_back(pattern);
    }

    {
        // --> src/main.rs:1:23
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = warn_pattern;
        pattern.fileNameIndex = "1";
        pattern.lineNumberIndex = "2";
        pattern.columnIndex = "3";
        warnPatterns.push_back(pattern);
    }

    {
        // ::: src/main.rs:1:23
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = warn_pattern1;
        pattern.fileNameIndex = "1";
        pattern.lineNumberIndex = "2";
        pattern.columnIndex = "3";
        warnPatterns.push_back(pattern);
    }

    {
        // warning:
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = warn_pattern2;
        pattern.fileNameIndex = "-1";
        pattern.lineNumberIndex = "-1";
        pattern.columnIndex = "-1";
        warnPatterns.push_back(pattern);
    }
    {
        // note:
        Compiler::CmpInfoPattern pattern;
        pattern.pattern = warn_pattern3;
        pattern.fileNameIndex = "-1";
        pattern.lineNumberIndex = "-1";
        pattern.columnIndex = "-1";
        warnPatterns.push_back(pattern);
    }

    rustc->SetWarnPatterns(warnPatterns);
    rustc->SetErrPatterns(errPatterns);
    rustc->SetTool("CXX", "rustc");
    rustc->SetTool("CC", "rustc");
    m_compilers.push_back(rustc);
    return true;
}

CompilerPtr CompilerLocatorRustc::Locate(const wxString& folder) { return nullptr; }
