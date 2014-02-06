#include "clDTL.h"
#include <wx/tokenzr.h>
#include "dtl/dtl.hpp"
#include <wx/ffile.h>

clDTL::clDTL()
{
}

clDTL::~clDTL()
{
}

void clDTL::Diff(const wxFileName& fnLeft, const wxFileName& fnRight)
{
    wxString leftFile, rightFile;
    
    {
        wxFFile fp1(fnLeft.GetFullPath(), "rb");
        wxFFile fp2(fnRight.GetFullPath(), "rb");
        
        if ( !fp1.IsOpened() || !fp2.IsOpened() )
            return;

        // Read the file content
        fp1.ReadAll(&leftFile, wxConvUTF8);
        fp2.ReadAll(&rightFile, wxConvUTF8);
    }
    
    m_result.clear();
    
    typedef wxString elem;
    typedef std::pair<elem, dtl::elemInfo> sesElem;
    
    wxArrayString leftLines = wxStringTokenize(leftFile, "\n", wxTOKEN_RET_EMPTY_ALL);
    wxArrayString rightLines = wxStringTokenize(rightFile, "\n", wxTOKEN_RET_EMPTY_ALL);
    
    std::vector<elem> leftLinesVec;
    std::vector<elem> rightLinesVec;
    leftLinesVec.insert(leftLinesVec.end(), leftLines.begin(), leftLines.end());
    rightLinesVec.insert(rightLinesVec.end(), rightLines.begin(), rightLines.end());
    
    dtl::Diff<elem, std::vector<elem> > diff(leftLinesVec, rightLinesVec);
    diff.onHuge();
    diff.compose();
    
    if ( 0 == diff.getEditDistance() ) {
        // nothing to be done - files are identical
        return;
    }
    
    // Loop over the diff and check if it is a whitespace only diff
    std::vector<sesElem> seq = diff.getSes().getSequence();
    m_result.reserve( seq.size() );
    for(size_t i=0; i<seq.size(); ++i) {
        clDTL::LineInfo line(seq.at(i).first, seq.at(i).second.type);
        m_result.push_back( line );
    }
}
