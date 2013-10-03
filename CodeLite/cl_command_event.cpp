#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_bool(false)
{
}

clCommandEvent::clCommandEvent(const clCommandEvent& event)
    : wxCommandEvent(event)
    , m_bool(false)
{
    *this = event;
}

clCommandEvent& clCommandEvent::operator=(const clCommandEvent& src)
{
    m_strings.clear();
    m_ptr = src.m_ptr;
    m_strings.insert(m_strings.end(), src.m_strings.begin(), src.m_strings.end());
    m_fileName = src.m_fileName;
    m_bool = src.m_bool;
    return *this;
}

clCommandEvent::~clCommandEvent()
{
    m_ptr.reset();
}

wxEvent* clCommandEvent::Clone() const
{
    clCommandEvent *new_event = new clCommandEvent(*this);
    return new_event;
}

void clCommandEvent::SetClientObject(wxClientData* clientObject)
{
    m_ptr = clientObject;
}

wxClientData* clCommandEvent::GetClientObject() const
{
    return m_ptr.get();
}

// ---------------------------------
// Code Completion event
// ---------------------------------
wxEvent* clCodeCompletionEvent::Clone() const
{
    clCodeCompletionEvent *new_event = new clCodeCompletionEvent(*this);
    return new_event;
}

clCodeCompletionEvent::clCodeCompletionEvent(const clCodeCompletionEvent& event)
    : clCommandEvent(event)
    , m_editor(NULL)
    , m_insideCommentOrString(false)
    , m_tagEntry(NULL)
{
    *this = event;
}

clCodeCompletionEvent::clCodeCompletionEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_editor(NULL)
    , m_insideCommentOrString(false)
    , m_tagEntry(NULL)
{
}

clCodeCompletionEvent::~clCodeCompletionEvent()
{
}

clCodeCompletionEvent& clCodeCompletionEvent::operator=(const clCodeCompletionEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    // Implemnt our copy ctor
    m_tags = src.m_tags;
    m_editor = src.m_editor;
    m_word = src.m_word;
    m_position = src.m_position;
    m_tooltip = src.m_tooltip;
    m_insideCommentOrString = src.m_insideCommentOrString;
    m_tagEntry = src.m_tagEntry;
    return *this;
}