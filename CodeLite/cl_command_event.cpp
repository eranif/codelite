#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_answer(false)
{
}

clCommandEvent::clCommandEvent(const clCommandEvent& event)
    : wxCommandEvent(event)
    , m_answer(false)
{
    *this = event;
}

clCommandEvent& clCommandEvent::operator=(const clCommandEvent& src)
{
    m_strings.clear();
    m_ptr = src.m_ptr;
    m_strings.insert(m_strings.end(), src.m_strings.begin(), src.m_strings.end());
    m_fileName = src.m_fileName;
    m_answer = src.m_answer;
    
    // Copy wxCommandEvent members here
    m_eventType  = src.m_eventType;
    m_id         = src.m_id;
    m_cmdString  = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong  = src.m_extraLong;
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

// ------------------------------------------------
// clColourEvent
// ------------------------------------------------

clColourEvent::clColourEvent(const clColourEvent& event)
{
    *this = event;
}

clColourEvent::clColourEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_page(NULL)
    , m_isActiveTab(false)
{
}

clColourEvent::~clColourEvent()
{
}

clColourEvent& clColourEvent::operator=(const clColourEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_bgColour     = src.m_bgColour;
    m_fgColour     = src.m_fgColour;
    m_page         = src.m_page;
    m_isActiveTab  = src.m_isActiveTab;
    m_borderColour = src.m_borderColour;
    return *this;
}

// ------------------------------------------------
// clBuildEvent
// ------------------------------------------------

clBuildEvent::clBuildEvent(const clBuildEvent& event)
{
    *this = event;
}

clBuildEvent::clBuildEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_projectOnly(false)
{
}

clBuildEvent::~clBuildEvent()
{
}

clBuildEvent& clBuildEvent::operator=(const clBuildEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_projectName = src.m_projectName;
    m_configurationName = src.m_configurationName;
    m_command = src.m_command;
    m_projectOnly = src.m_projectOnly;
    return *this;
}

// ------------------------------------------------------------------
// clDebugEvent
// ------------------------------------------------------------------
clDebugEvent::clDebugEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clDebugEvent::~clDebugEvent()
{
}

clDebugEvent::clDebugEvent(const clDebugEvent& event)
{
    *this = event;
}

clDebugEvent& clDebugEvent::operator=(const clDebugEvent& src)
{
    m_projectName = src.m_projectName;
    m_configurationName = src.m_configurationName;
    return *this;
}
