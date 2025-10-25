#pragma once

#include "LSP/basic_types.h"
#include "codelite_exports.h"

struct WXDLLIMPEXP_SDK DiagnosticsData : public wxClientData {
    LSP::Diagnostic diagnostic;
    virtual ~DiagnosticsData() = default;
    DiagnosticsData(const DiagnosticsData&) = default;
    DiagnosticsData(const LSP::Diagnostic& diag) { diagnostic = diag; }
    DiagnosticsData(DiagnosticsData&&) = delete;
    DiagnosticsData& operator=(const DiagnosticsData&) = default;
    DiagnosticsData& operator=(DiagnosticsData&&) = delete;
};
