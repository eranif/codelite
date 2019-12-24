#ifndef CONTEXTPYTHON_HPP
#define CONTEXTPYTHON_HPP

#include "generic_context.h"

class clEditor;
class ContextPython : public ContextGeneric
{
protected:
    bool m_eventsBound = false;

protected:
    bool IsComment(int pos) const;
    bool IsCommentBlock(int pos) const;

protected:
    void OnCommentSelection(wxCommandEvent& event);
    void OnCommentLine(wxCommandEvent& event);
    void AutoIndent(const wxChar& ch);

public:
    ContextPython();
    ContextPython(clEditor* container);
    virtual ~ContextPython();
    virtual void ApplySettings();
    virtual ContextBase* NewInstance(clEditor* container);
};

#endif // CONTEXTPYTHON_HPP
