/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include "file_logger.h"

#include "memcheckerror.h"

bool MemCheckErrorLocation::operator==(const MemCheckErrorLocation & other) const
{
    return func == other.func && file == other.file && line == other.line;
}

bool MemCheckErrorLocation::operator!=(const MemCheckErrorLocation & other) const
{
    return !(*this == other);
}

const wxString MemCheckErrorLocation::toString() const
{
    return wxString::Format(wxT("%s\t%s\t%i\t%s"), func, file, line, obj);
}

const wxString MemCheckErrorLocation::toText(const wxString & workspacePath) const
{
    return wxString::Format(wxT("%s   ( %s: %i )"), func, getFile(workspacePath), line);
}

const wxString MemCheckErrorLocation::getFile(const wxString & workspacePath) const
{
    wxString localPath;
    if (workspacePath.IsEmpty() || !file.StartsWith(workspacePath, &localPath)) {
        return file;
    } else {
        return localPath;
    }
}

const wxString MemCheckErrorLocation::getObj(const wxString & workspacePath) const
{
    wxString localPath;
    if (workspacePath.IsEmpty() || !obj.StartsWith(workspacePath, &localPath)) {
        return obj;
    } else {
        return localPath;
    }
}

const bool MemCheckErrorLocation::isOutOfWorkspace(const wxString & workspacePath) const
{
    return !file.StartsWith(workspacePath);
}



MemCheckError::MemCheckError(): suppressed(false) {}

const wxString MemCheckError::toString() const
{
    wxString string = wxString::Format(wxT("%s"), label);
    for (const auto& nestedError : nestedErrors)
        string.Append(wxString::Format("\n%s", nestedError.toString()));
    for (const auto& location : locations)
        string.Append(wxString::Format("\n%s", location.toString()));
    return string;
}

const wxString MemCheckError::toText(unsigned int indent) const
{
    wxString text = label;
    for (const auto& nestedError : nestedErrors)
        text.Append(wxString::Format("\n%s%s", wxString(' ', 2 * indent), nestedError.toText(indent + 1)));
    for (const auto& location : locations)
        text.Append(wxString::Format("\n%s%s", wxString(' ', 4 * indent), location.toText()));
    return text;
}

const wxString MemCheckError::getSuppression()
{
    wxString ruleName;
    if (suppression.Contains(wxT(SUPPRESSION_NAME_PLACEHOLDER))) {
        suppression.Trim(true);
        suppression.Trim(false);

        wxStringTokenizer tokenizer(suppression, "\n");
        int kind = 1;
        while ( tokenizer.HasMoreTokens() ) {
            wxString line = tokenizer.GetNextToken();
            wxString token = line.AfterFirst(':');
            if (!token.IsEmpty())
                switch (kind) {
                case 1:
                    ruleName.Append(wxString::Format("(%s =", token));
                    ++kind;
                    break;
                case 2:
                    ruleName.Append(wxString::Format(" %s", token));
                    ++kind;
                    break;
                default:
                    ruleName.Append(wxString::Format(" | %s", token));
                }
        }
        ruleName.Append(wxT(")"));
        suppression.Replace(wxT(SUPPRESSION_NAME_PLACEHOLDER), ruleName, false);
    }
    return suppression;
}

const bool MemCheckError::hasPath(const wxString& path) const
{
    for (const auto& location : locations)
        if (location.file.StartsWith(path))
            return true;
    for (const auto& nestedError : nestedErrors)
        if (nestedError.hasPath(path))
            return true;
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool MemCheckIterTools::IterTool::isEqual(MemCheckError & lhs, MemCheckError & rhs) const
{
    if (!(lhs.type == rhs.type && lhs.label == rhs.label))
        return false;

    // locations == other.locations
    LocationListIterator lhsItL = LocationListIterator(lhs.locations, *this);
    LocationListIterator rhsItL = LocationListIterator(rhs.locations, *this);
    LocationList::iterator lhsItEndL = lhs.locations.end();
    LocationList::iterator rhsItEndL = rhs.locations.end();

    while (!(lhsItL == lhsItEndL && rhsItL == rhsItEndL)) { // both on end => ok
        if ( lhsItL == lhsItEndL || rhsItL == rhsItEndL || *lhsItL != *rhsItL)
            return false; // only one on end or not equal => fail

        ++lhsItL;
        ++rhsItL;
    }

    // nestedErrors == other.nestedErrors
    ErrorListIterator lhsItE = ErrorListIterator(lhs.nestedErrors, *this);
    ErrorListIterator rhsItE = ErrorListIterator(rhs.nestedErrors, *this);
    ErrorList::iterator lhsItEndE = lhs.nestedErrors.end();
    ErrorList::iterator rhsItEndE = rhs.nestedErrors.end();

    while (!(lhsItE == lhsItEndE && rhsItE == rhsItEndE)) { // both on end => ok
        if ( lhsItE == lhsItEndE || rhsItE == rhsItEndE || ! isEqual(*lhsItE, *rhsItE))
            return false; // only one on end or not equal => fail

        ++lhsItE;
        ++rhsItE;
    }

    return true;
}


MemCheckIterTools::LocationListIterator::LocationListIterator(LocationList & l,
        const IterTool &iterTool) : p(l.begin()), m_end(l.end()), m_iterTool(iterTool)
{
    while (p != m_end && m_iterTool.omitNonWorkspace && p->isOutOfWorkspace(m_iterTool.workspacePath))
        ++p;
}

MemCheckIterTools::LocationListIterator::~LocationListIterator() {}

LocationList::iterator& MemCheckIterTools::LocationListIterator::operator++()
{
    ++p;
    while (p != m_end && m_iterTool.omitNonWorkspace && p->isOutOfWorkspace(m_iterTool.workspacePath))
        ++p;
    return p;
}

LocationList::iterator MemCheckIterTools::LocationListIterator::operator++(int)
{
    LocationList::iterator tmp(p);
    operator++();
    return tmp;
}

bool MemCheckIterTools::LocationListIterator::operator==(const LocationList::iterator& rhs)
{
    return p == rhs;
}

bool MemCheckIterTools::LocationListIterator::operator!=(const LocationList::iterator& rhs)
{
    return p != rhs;
}

MemCheckErrorLocation & MemCheckIterTools::LocationListIterator::operator*()
{
    return *p;
}



MemCheckIterTools::ErrorListIterator::ErrorListIterator(ErrorList & l, const IterTool & iterTool)
    : p(l.begin()), m_end(l.end()), m_iterTool(iterTool)
{
    while (p != m_end && m_iterTool.omitSuppressed && p->suppressed)
        ++p;
}

MemCheckIterTools::ErrorListIterator::~ErrorListIterator() {}

ErrorList::iterator& MemCheckIterTools::ErrorListIterator::operator++()
{
    ErrorList::iterator prev(p);
    ++p;

    while (p != m_end && ( (m_iterTool.omitDuplications && m_iterTool.isEqual(*prev, *p))
                           || (m_iterTool.omitSuppressed && p->suppressed) ))
        ++p;

    return p;
}

ErrorList::iterator MemCheckIterTools::ErrorListIterator::operator++(int)
{
    ErrorList::iterator tmp(p);
    operator++();
    return tmp;
}

bool MemCheckIterTools::ErrorListIterator::operator==(const ErrorList::iterator& rhs)
{
    return p == rhs;
}

bool MemCheckIterTools::ErrorListIterator::operator!=(const ErrorList::iterator& rhs)
{
    return p != rhs;
}

MemCheckError & MemCheckIterTools::ErrorListIterator::operator*()
{
    return *p;
}


MemCheckIterTools::MemCheckIterTools(const wxString & workspacePath, unsigned int flags)
{
    m_iterTool.omitNonWorkspace = flags & MC_IT_OMIT_NONWORKSPACE;
    m_iterTool.omitDuplications = flags & MC_IT_OMIT_DUPLICATIONS;
    m_iterTool.omitSuppressed   = flags & MC_IT_OMIT_SUPPRESSED;
    m_iterTool.workspacePath    = workspacePath;
}

MemCheckIterTools::ErrorListIterator MemCheckIterTools::GetIterator(ErrorList & l)
{
    return ErrorListIterator(l, m_iterTool);
}

MemCheckIterTools::LocationListIterator MemCheckIterTools::GetIterator(LocationList & l)
{
    return LocationListIterator(l, m_iterTool);
}

MemCheckIterTools::ErrorListIterator MemCheckIterTools::Factory(ErrorList & l,
        const wxString & workspacePath, unsigned int flags)
{
    return MemCheckIterTools(workspacePath, flags).GetIterator(l);
}

MemCheckIterTools::LocationListIterator MemCheckIterTools::Factory(LocationList & l,
        const wxString & workspacePath, unsigned int flags)
{
    return MemCheckIterTools(workspacePath, flags).GetIterator(l);
}
