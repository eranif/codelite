//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clDTL.cpp
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

#include "clDTL.h"

#include "dtl/dtl.hpp"

#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

clDTL::clDTL() {}

clDTL::~clDTL() {}

void clDTL::Diff(const wxFileName& fnLeft, const wxFileName& fnRight, DiffMode mode)
{
    wxString leftFile, rightFile;

    {
        wxFFile fp1(fnLeft.GetFullPath(), "rb");
        wxFFile fp2(fnRight.GetFullPath(), "rb");

        if(!fp1.IsOpened() || !fp2.IsOpened())
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

    dtl::Diff<elem, std::vector<elem>> diff(leftLinesVec, rightLinesVec);
    diff.onHuge();
    diff.compose();

    if(0 == diff.getEditDistance()) {
        // nothing to be done - files are identical
        return;
    }

    if(mode & clDTL::kTwoPanes) {

        ///////////////////////////////////////////////////////////////////
        // Two panes diff
        // designed for displayed on a two panes view where on the left
        // pane all deletions while on the right pane all the new lines
        ///////////////////////////////////////////////////////////////////

        // Loop over the diff and check if it is a whitespace only diff
        std::vector<sesElem> seq = diff.getSes().getSequence();
        m_resultLeft.reserve(seq.size());
        m_resultRight.reserve(seq.size());

        const int STATE_NONE = 0;
        const int STATE_IN_SEQ = 1;

        int state = STATE_NONE;
        int seqStartLine = wxNOT_FOUND;
        size_t seqSize = 0;

        LineInfoVec_t tmpSeqLeft;
        LineInfoVec_t tmpSeqRight;

        for(size_t i = 0; i < seq.size(); ++i) {
            switch(seq.at(i).second.type) {
            case dtl::SES_COMMON: {
                if(state == STATE_IN_SEQ) {

                    // set the sequence size
                    seqSize = ::wxMax(tmpSeqLeft.size(), tmpSeqRight.size());

                    m_sequences.push_back(std::make_pair(seqStartLine, seqStartLine + seqSize));
                    seqStartLine = wxNOT_FOUND;
                    state = STATE_NONE;

                    // increase the buffer size
                    tmpSeqLeft.resize(seqSize);
                    tmpSeqRight.resize(seqSize);

                    m_resultLeft.insert(m_resultLeft.end(), tmpSeqLeft.begin(), tmpSeqLeft.end());
                    m_resultRight.insert(m_resultRight.end(), tmpSeqRight.begin(), tmpSeqRight.end());

                    tmpSeqLeft.clear();
                    tmpSeqRight.clear();
                    seqSize = 0;
                }
                clDTL::LineInfo line(seq.at(i).first, LINE_COMMON);
                m_resultLeft.push_back(line);
                m_resultRight.push_back(line);
                break;
            }
            case dtl::SES_ADD: {
                clDTL::LineInfo lineRight(seq.at(i).first, LINE_ADDED);
                tmpSeqRight.push_back(lineRight);

                if(state == STATE_NONE) {
                    seqStartLine = m_resultLeft.size();
                    state = STATE_IN_SEQ;
                }
                break;
            }
            case dtl::SES_DELETE: {
                clDTL::LineInfo lineLeft(seq.at(i).first, LINE_REMOVED);
                tmpSeqLeft.push_back(lineLeft);

                if(state == STATE_NONE) {
                    seqStartLine = m_resultLeft.size();
                    state = STATE_IN_SEQ;
                }
                break;
            }
            }
        }

        if(state == STATE_IN_SEQ) {
            // set the sequence size
            seqSize = ::wxMax(tmpSeqLeft.size(), tmpSeqRight.size());
            if(seqSize) {
                m_sequences.push_back(std::make_pair(seqStartLine, seqStartLine + seqSize));
                seqStartLine = wxNOT_FOUND;
                state = STATE_NONE;

                // increase the buffer size
                tmpSeqLeft.resize(seqSize);
                tmpSeqRight.resize(seqSize);

                m_resultLeft.insert(m_resultLeft.end(), tmpSeqLeft.begin(), tmpSeqLeft.end());
                m_resultRight.insert(m_resultRight.end(), tmpSeqRight.begin(), tmpSeqRight.end());

                tmpSeqLeft.clear();
                tmpSeqRight.clear();
                seqSize = 0;
            }
        }
    } else {
        ///////////////////////////////////////////////////////////////////
        // One pane diff view
        // designed for displayed on a single editor
        ///////////////////////////////////////////////////////////////////
        std::vector<sesElem> seq = diff.getSes().getSequence();
        m_resultLeft.reserve(seq.size());
        int seqStartLine = wxNOT_FOUND;
        for(size_t i = 0; i < seq.size(); ++i) {
            switch(seq.at(i).second.type) {
            case dtl::SES_COMMON: {
                if(seqStartLine != wxNOT_FOUND) {
                    m_sequences.push_back(std::make_pair(seqStartLine, m_resultLeft.size()));
                    seqStartLine = wxNOT_FOUND;
                }
                clDTL::LineInfo line(seq.at(i).first, LINE_COMMON);
                m_resultLeft.push_back(line);
                break;
            }
            case dtl::SES_ADD: {
                if(seqStartLine == wxNOT_FOUND) {
                    seqStartLine = m_resultLeft.size();
                }
                clDTL::LineInfo line(seq.at(i).first, LINE_ADDED);
                m_resultLeft.push_back(line);
                break;
            }
            case dtl::SES_DELETE: {
                if(seqStartLine == wxNOT_FOUND) {
                    seqStartLine = m_resultLeft.size();
                }
                clDTL::LineInfo line(seq.at(i).first, LINE_REMOVED);
                m_resultLeft.push_back(line);
                break;
            }
            }
        }

        if(seqStartLine != wxNOT_FOUND) {
            m_sequences.push_back(std::make_pair(seqStartLine, m_resultLeft.size()));
            seqStartLine = wxNOT_FOUND;
        }
    }
}
