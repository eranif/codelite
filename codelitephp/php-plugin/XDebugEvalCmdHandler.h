//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugEvalCmdHandler.h
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

#ifndef XDEBUGEVALCMDHANDLER_H
#define XDEBUGEVALCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler
#include <wx/string.h>

class XDebugManager;
class XDebugEvalCmdHandler : public XDebugCommandHandler
{
public:
    enum {
        kEvalForTooltip  = 1,
        kEvalForEvalPane = 2,
    };
protected:
    wxString m_expression;
    int      m_evalReason;
    
public:
    XDebugEvalCmdHandler(const wxString &expression, int evalReason, XDebugManager* mgr, int transcationId);
    virtual ~XDebugEvalCmdHandler() = default;

    void SetExpression(const wxString& expression) {
        this->m_expression = expression;
    }
    const wxString& GetExpression() const {
        return m_expression;
    }
public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGEVALCMDHANDLER_H
