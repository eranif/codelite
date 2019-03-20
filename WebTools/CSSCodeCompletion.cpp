#include "CSSCodeCompletion.h"
#include "ieditor.h"
#include "css.json"
#include <wx/stc/stc.h>
#include "wxCodeCompletionBox.h"
#include <wx/xrc/xmlres.h>
#include "wxCodeCompletionBoxEntry.h"
#include "wxCodeCompletionBoxManager.h"
#include "JSON.h"
#include <wx/tokenzr.h>
#include <set>
#include <algorithm>
#include "codelite_events.h"
#include "webtools.h"

CSSCodeCompletion::CSSCodeCompletion(WebTools* plugin)
    : ServiceProvider("WebTools: CSS", eServiceType::kCodeCompletion)
    , m_isEnabled(true)
    , m_plugin(plugin)
{
    JSON root(CSS_JSON);
    JSONItem arr = root.toElement();
    int count = arr.arraySize();
    std::set<wxString> valuesSet;
    for(int i = 0; i < count; ++i) {
        JSONItem entry = arr.arrayItem(i);
        if(!entry.hasNamedObject("name")) continue;

        Entry e;
        e.property = entry.namedObject("name").toString();
        e.values = entry.namedObject("values").toArrayString();
        m_entries.push_back(e);

        // collect the values and make them a unique set
        for(size_t i = 0; i < e.values.size(); ++i) {
            valuesSet.insert(e.values.Item(i));
        }
    }

    std::for_each(valuesSet.begin(), valuesSet.end(), [&](const wxString& v) {
        Entry e;
        e.property = v;
        m_entries.push_back(e);
    });

    Bind(wxEVT_CC_CODE_COMPLETE, &CSSCodeCompletion::OnCodeComplete, this);
}

CSSCodeCompletion::~CSSCodeCompletion() { Unbind(wxEVT_CC_CODE_COMPLETE, &CSSCodeCompletion::OnCodeComplete, this); }

void CSSCodeCompletion::CssCodeComplete(IEditor* editor)
{
    if(!m_isEnabled) return;

    // Perform HTML code completion
    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    int currentLine = ctrl->GetCurrentLine();
    int minPos = ctrl->PositionFromLine(currentLine);

    wxChar nonWhitespaceChar = 0;
    int nonWhitespaceCharPos = wxNOT_FOUND;
    int curpos = ctrl->GetCurrentPos();

    while((curpos > minPos)) {
        nonWhitespaceChar = ctrl->GetCharAt(curpos);
        switch(nonWhitespaceChar) {
        case '\n':
        case '\r':
        case '\t':
        case ' ':
            break;
        default:
            nonWhitespaceCharPos = curpos;
            break;
        }
        if(nonWhitespaceCharPos != wxNOT_FOUND) break;
        curpos = ctrl->PositionBefore(curpos);
    }

    if(nonWhitespaceCharPos != wxNOT_FOUND && nonWhitespaceChar == ':') {
        // Suggest values of the given properties
        wxString word = GetPreviousWord(editor, nonWhitespaceCharPos);
        if(word.IsEmpty()) return;
        Entry::Vec_t::const_iterator iter =
            std::find_if(m_entries.begin(), m_entries.end(), [&](const Entry& e) { return (e.property == word); });
        if(iter != m_entries.end()) {
            wxCodeCompletionBox::BmpVec_t bitmaps;
            bitmaps.push_back(wxXmlResource::Get()->LoadBitmap("code-tags"));

            wxCodeCompletionBoxEntry::Vec_t entries;
            const wxArrayString& values = (*iter).values;

            for(size_t i = 0; i < values.size(); ++i) {
                wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(values.Item(i), 0);
                entries.push_back(entry);
            }
            wxCodeCompletionBoxManager::Get().ShowCompletionBox(editor->GetCtrl(), entries, bitmaps, 0,
                                                                editor->GetCurrentPosition(), this);
        }
    } else {
        wxCodeCompletionBox::BmpVec_t bitmaps;
        bitmaps.push_back(wxXmlResource::Get()->LoadBitmap("code-tags"));

        wxCodeCompletionBoxEntry::Vec_t entries;
        for(size_t i = 0; i < m_entries.size(); ++i) {
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(m_entries.at(i).property, 0);
            // entry->SetComment(m_entries.at(i).m_comment);
            entries.push_back(entry);
        }
        wxCodeCompletionBoxManager::Get().ShowCompletionBox(
            editor->GetCtrl(), entries, bitmaps, 0, editor->WordStartPos(editor->GetCurrentPosition(), false), this);
    }
}

wxString CSSCodeCompletion::GetPreviousWord(IEditor* editor, int pos)
{
    int lineStartPos = editor->GetCtrl()->PositionFromLine(editor->GetCtrl()->LineFromPosition(pos));

    // get the line from the start up until the ":"
    wxString line = editor->GetCtrl()->GetTextRange(lineStartPos, pos);
    if(line.IsEmpty()) return "";

    wxArrayString words = ::wxStringTokenize(line, "\r\n \t", wxTOKEN_STRTOK);
    if(words.IsEmpty()) return "";
    return words.Last();
}

void CSSCodeCompletion::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    if(editor && m_plugin->IsCSSFile(editor)) {
        // CSS code completion
        event.Skip(false);
        CssCodeComplete(editor);
    }
}
