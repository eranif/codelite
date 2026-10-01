#include "ThreadStack.hpp"

#include <sstream>
#ifdef __WXMSW__
// clang-format off
#include <windows.h>
#include <dbghelp.h>
#include <iostream>
// clang-format on
#elif defined(__linux__)
#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <memory>
#endif

#ifdef __WXMSW__
wxString DumpCurrentThreadStack()
{
    std::stringstream ss;
#ifndef _M_X64
    // Only supported on x86_64 Windows
    ss << "Stack trace is only supported on x86_64 Windows (not ARM64 or x86)" << std::endl;
#else
    // x86_64 implementation

    ss << "=== Stack Trace ===" << std::endl;

    // Initialize symbol handler for the current process
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    // Set symbol options
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    if (!SymInitialize(process, nullptr, TRUE)) {
        ss << "Failed to initialize symbols (error: " << GetLastError() << ")" << std::endl;
        return wxString::FromUTF8(ss.str());
    }

    // Setup stack frame for walking
    CONTEXT context;
    RtlCaptureContext(&context);

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

    DWORD machineType;
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrStack.Offset = context.Rsp;

    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    // Walk the stack
    int frameNum = 0;
    while (StackWalk64(machineType,
                       process,
                       thread,
                       &stackFrame,
                       &context,
                       nullptr,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       nullptr)) {

        DWORD64 address = stackFrame.AddrPC.Offset;

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        ss << "#" << frameNum++ << " 0x" << std::hex << address << std::dec << " ";

        DWORD64 displacement = 0;
        if (SymFromAddr(process, address, &displacement, symbol)) {
            ss << symbol->Name;
            if (displacement != 0) {
                ss << " + 0x" << std::hex << displacement << std::dec;
            }

            // Try to get line information
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(process, address, &lineDisplacement, &line)) {
                ss << " at " << line.FileName << ":" << line.LineNumber;
            }
        } else {
            ss << "<unknown>";
        }

        ss << std::endl;
    }

    ss << "===================" << std::endl;

    // Cleanup
    SymCleanup(process);
#endif // _M_X64
    return wxString::FromUTF8(ss.str());
}
#elif defined(__linux__)
wxString DumpCurrentThreadStack()
{
    std::stringstream ss;
    ss << "=== Stack Trace ===" << std::endl;

    constexpr int MAX_FRAMES = 64;
    void* addresses[MAX_FRAMES];
    int count = backtrace(addresses, MAX_FRAMES);
    if (count <= 0) {
        ss << "Failed to capture backtrace" << std::endl;
        return wxString::FromUTF8(ss.str());
    }

    // backtrace_symbols() allocates a single malloc'd block holding both the array
    // and the strings it points into; it must be free()d (not delete[]d).
    std::unique_ptr<char*, decltype(&free)> symbols(backtrace_symbols(addresses, count), &free);
    if (!symbols) {
        ss << "Failed to resolve backtrace symbols" << std::endl;
        return wxString::FromUTF8(ss.str());
    }

    for (int i = 0; i < count; ++i) {
        const std::string line{symbols.get()[i]};

        // Typical glibc format: "binary(mangled_name+0xoffset) [0xaddress]". Extract the
        // mangled name between '(' and '+' so it can be demangled into a readable form.
        std::string demangled_name;
        size_t open_paren = line.find('(');
        size_t plus = (open_paren == std::string::npos) ? std::string::npos : line.find('+', open_paren);
        if (open_paren != std::string::npos && plus != std::string::npos && plus > open_paren + 1) {
            std::string mangled = line.substr(open_paren + 1, plus - open_paren - 1);
            int status = 0;
            std::unique_ptr<char, decltype(&free)> demangled(
                abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status), &free);
            if (status == 0 && demangled) {
                demangled_name = demangled.get();
            }
        }

        ss << "#" << i << " ";
        if (!demangled_name.empty()) {
            // Keep the raw line too: it still carries the module name, offset and address.
            ss << demangled_name << "  (" << line << ")";
        } else {
            ss << line;
        }
        ss << std::endl;
    }

    ss << "===================" << std::endl;
    return wxString::FromUTF8(ss.str());
}
#else
wxString DumpCurrentThreadStack() { return {}; }
#endif
