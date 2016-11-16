#ifndef TAIL_DATA_H
#define TAIL_DATA_H

#include <wx/filename.h>
#include <wx/string.h>

class TailData
{
public:
    wxFileName filename;
    size_t lastPos;
    wxString displayedText;

public:
    TailData()
        : lastPos(0)
    {
    }
};

#endif
