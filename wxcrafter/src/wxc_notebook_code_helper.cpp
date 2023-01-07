#include "wxc_notebook_code_helper.h"

wxcNotebookCodeHelper::wxcNotebookCodeHelper() {}

wxcNotebookCodeHelper::~wxcNotebookCodeHelper() {}

wxcNotebookCodeHelper& wxcNotebookCodeHelper::Get()
{
    static wxcNotebookCodeHelper helper;
    return helper;
}

void wxcNotebookCodeHelper::Clear() { m_code.Clear(); }

wxString& wxcNotebookCodeHelper::Code() { return m_code; }
