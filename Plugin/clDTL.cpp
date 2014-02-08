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
        fp1.ReadAll(&leftFile);
        fp2.ReadAll(&rightFile);
    }
    
    m_resultLeft.clear();
    m_resultRight.clear();
    m_sequences.clear();
    
    typedef wxString elem;
    typedef std::pair<elem, dtl::elemInfo> sesElem;
    
    wxArrayString leftLines = wxStringTokenize(leftFile, "\n", wxTOKEN_RET_DELIMS);
    wxArrayString rightLines = wxStringTokenize(rightFile, "\n", wxTOKEN_RET_DELIMS);
    
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
    m_resultLeft.reserve( seq.size() );
    m_resultRight.reserve( seq.size() );
    
    const int STATE_NONE   = 0;
    const int STATE_IN_SEQ = 1;
    
    int state = STATE_NONE;
    int seqStartLine = wxNOT_FOUND;
    
    for(size_t i=0; i<seq.size(); ++i) {
        switch(seq.at(i).second.type) {
        case dtl::SES_COMMON: {
            clDTL::LineInfo line(seq.at(i).first, LINE_COMMON);
            m_resultLeft.push_back( line );
            m_resultRight.push_back( line );
            
            if ( state == STATE_IN_SEQ ) {
                m_sequences.push_back( std::make_pair(seqStartLine, i) );
                seqStartLine = wxNOT_FOUND;
                state = STATE_NONE;
            }
            break;
            
        }
        case dtl::SES_ADD: {
            clDTL::LineInfo lineLeft("\n", LINE_PLACEHOLDER);
            m_resultLeft.push_back( lineLeft );
            
            clDTL::LineInfo lineRight(seq.at(i).first, LINE_ADDED);
            m_resultRight.push_back( lineRight );
            
            if ( state == STATE_NONE ) {
                seqStartLine = i;
                state = STATE_IN_SEQ;
            }
            break;
            
        }
        case dtl::SES_DELETE: {
            clDTL::LineInfo lineLeft(seq.at(i).first, LINE_REMOVED);
            m_resultLeft.push_back( lineLeft );
            
            clDTL::LineInfo lineRight("\n", LINE_PLACEHOLDER);
            m_resultRight.push_back( lineRight );
            
            if ( state == STATE_NONE ) {
                seqStartLine = i;
                state = STATE_IN_SEQ;
            }
            break;
        }
        }
    }
    
    if ( state == STATE_IN_SEQ ) {
        m_sequences.push_back( std::make_pair(seqStartLine, seq.size()-1) );
        seqStartLine = wxNOT_FOUND;
        state = STATE_NONE;
    }
}
