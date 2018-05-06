//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBMemoryView.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "LLDBMemoryView.h"
#include "LLDBPlugin.h"

#include <wx/base64.h>
#include <wx/msgdlg.h>

namespace
{

bool IsPrintable(uint8_t chr)
{
    // Note sure what rules GDB uses but this is close I imagine...
    return std::isprint(chr) || ((chr >= 128) && (chr != 255));
}

} // namespace

// ------------------------------------------------------------------------------

LLDBMemoryView::LLDBMemoryView(wxWindow* parent, LLDBPlugin& plugin, const size_t initialColumns)
    : MemoryView(parent)
    , m_plugin(plugin)
{
    const auto defaultColsStr = wxString() << initialColumns;
    for(size_t i = 0, numberOfChoices = m_choiceCols->GetCount(); i < numberOfChoices; ++i) {
        if(defaultColsStr == m_choiceCols->GetString(i)) {
            m_choiceCols->SetSelection(i);
            break;
        }
    }

    m_plugin.Bind(wxEVT_LLDB_MEMORY_VIEW_EXPRESSION, &LLDBMemoryView::OnViewExpression, this);
    m_plugin.GetLLDB()->Bind(wxEVT_LLDB_MEMORY_VIEW_RESPONSE, &LLDBMemoryView::OnMemoryViewResponse, this);
}

LLDBMemoryView::~LLDBMemoryView()
{
    m_plugin.Unbind(wxEVT_LLDB_MEMORY_VIEW_EXPRESSION, &LLDBMemoryView::OnViewExpression, this);
    m_plugin.GetLLDB()->Unbind(wxEVT_LLDB_MEMORY_VIEW_RESPONSE, &LLDBMemoryView::OnMemoryViewResponse, this);
}

void LLDBMemoryView::Refresh()
{
    m_plugin.GetLLDB()->GetMemoryView(GetExpression(), GetSize());
}

void LLDBMemoryView::OnViewExpression(LLDBEvent& lldbEvent)
{
    m_textCtrlExpression->ChangeValue(lldbEvent.GetExpression());
    Refresh();
}

void LLDBMemoryView::OnMemoryViewResponse(LLDBEvent& lldbEvent)
{
    // Format pretty printed like GDB so base MemoryView can parse edited text.
    const auto memoryView = ::wxBase64Decode(lldbEvent.GetExpression());
    m_targetProcessAddress = lldbEvent.GetAddress();
    auto lineStartAddress = m_targetProcessAddress;
    const uint8_t* pChar = reinterpret_cast<uint8_t*>(memoryView.GetData());
    const size_t bytesPerLine = GetColumns();
    const size_t hexWidth = bytesPerLine * 5; // 5 chars per byte - "0xab "
    constexpr wchar_t unprintableCharacter = '.';
    size_t bytesLeft = memoryView.GetDataLen();
    const size_t numberOfLines = (bytesLeft + bytesPerLine - 1) / bytesPerLine;
    wxString outputString;

    for(size_t i = 0; i < numberOfLines; ++i) {
        const size_t bytesThisLine = std::min<size_t>(bytesLeft, bytesPerLine);
        wxString hexBuff, textBuff;

        for(size_t j = 0; j < bytesThisLine; ++j, ++pChar, --bytesLeft) {
            hexBuff += wxString::Format("0x%02x ", *pChar);
            textBuff.Append(IsPrintable(*pChar) ? static_cast<wchar_t>(*pChar) : unprintableCharacter, 1);
        }

        outputString += wxString::Format("0x%016llx: %-*.*s : %s\n", lineStartAddress, static_cast<int>(hexWidth),
            static_cast<int>(hexWidth), hexBuff, textBuff);
        lineStartAddress += bytesThisLine;
    }

    SetViewString(outputString);
}

void LLDBMemoryView::OnMemorySize(wxCommandEvent& event)
{
    Refresh();
}

void LLDBMemoryView::OnNumberOfRows(wxCommandEvent& event)
{
    Refresh();
}

void LLDBMemoryView::OnEvaluate(wxCommandEvent& event)
{
    Refresh();
}

void LLDBMemoryView::OnUpdate(wxCommandEvent& event)
{
    const auto textMemory = m_textCtrlMemory->GetValue();
    uint8_t buff[textMemory.Length()]; // Length() / 5 would be sufficient...
    size_t dataLength = 0;

    auto lines = wxStringTokenize(textMemory, wxT("\n"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        auto line = lines.Item(i).AfterFirst(wxT(':')).BeforeFirst(wxT(':')).Trim().Trim(false);
        auto hexValues = wxStringTokenize(line, wxT(" "), wxTOKEN_STRTOK);

        for(size_t y = 0; y < hexValues.GetCount(); ++y) {
            const auto& hex = hexValues.Item(y);
            unsigned long val;
            if((!hex.ToULong(&val, 16)) || (val > 255)) {
                wxMessageBox(wxString::Format(_("Invalid memory value: %s"), textMemory), _("CodeLite"), wxICON_WARNING|wxOK);
                Refresh();
                return;
            }

            buff[dataLength++] = static_cast<uint8_t>(val);
        }
    }

    const auto base64Memory = ::wxBase64Encode(buff, dataLength);
    m_plugin.GetLLDB()->SetMemoryView(m_targetProcessAddress, base64Memory);
}
