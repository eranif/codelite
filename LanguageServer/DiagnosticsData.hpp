#ifndef DIAGNOSTICS_DATA_HPP
#define DIAGNOSTICS_DATA_HPP

#include "LSP/basic_types.h"

struct DiagnosticsData : public wxClientData {
    LSP::Diagnostic diagnostic;
    virtual ~DiagnosticsData() {}
    DiagnosticsData(const DiagnosticsData&) = default;
    DiagnosticsData(const LSP::Diagnostic& diag) { diagnostic = diag; }
    DiagnosticsData(DiagnosticsData&&) = delete;
    DiagnosticsData& operator=(const DiagnosticsData&) = default;
    DiagnosticsData& operator=(DiagnosticsData&&) = delete;
};
#endif // DIAGNOSTICS_DATA_HPP
