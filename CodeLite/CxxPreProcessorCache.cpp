#include "CxxPreProcessorCache.h"
#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"

CxxPreProcessorCache::CxxPreProcessorCache() {}

CxxPreProcessorCache::~CxxPreProcessorCache() {}

bool CxxPreProcessorCache::Find(const wxString& filename,
                                const wxString& project,
                                const wxString& config,
                                wxArrayString& definitions)
{
    wxString key;
    key << project << "|" << config << "|" << filename;
    CxxPreProcessorCache::Map_t::iterator iter = m_impl.find(key);
    if(iter == m_impl.end()) return false;

    if(wxFileName(filename).GetModificationTime().GetTicks() > iter->second.lastUpdated) {
        // the file file was saved since we last cached it
        // Next:
        // Check if the cached entry preabmle is the same as the current file
        wxString currentPreamble = GetPreamble(filename);
        wxString cachedPreamble = iter->second.preamble;
        if(cachedPreamble == currentPreamble) {
            // no change was done to the preamble
            definitions = iter->second.definitions;
            return true;
        } else {
            // preamble was modified - we need to re-cache it
            // remove the current cache and return false
            m_impl.erase(iter);
            return false;
        }
    } else {
        // the file was not saved since we cached it
        definitions = iter->second.definitions;
        return false;
    }
}

void CxxPreProcessorCache::Clear() { m_impl.clear(); }

wxString CxxPreProcessorCache::GetPreamble(const wxString& filename) const
{
    Scanner_t scanner = ::LexerNew(filename, 0);
    if(!scanner) return { "" };

    CxxLexerToken token;
    wxString preamble;
    while(::LexerNext(scanner, token)) {
        if(token.GetType() == T_PP_INCLUDE_FILENAME) {
            preamble << token.GetText() << "\n";
        }
    }
    ::LexerDestroy(&scanner);

    preamble.Trim();
    return preamble;
}

bool CxxPreProcessorCache::Insert(const wxString& filename,
                                  const wxString& project,
                                  const wxString& config,
                                  const wxArrayString& definitions)
{
    wxString key;
    key << project << "|" << config << "|" << filename;

    // remove old entry
    CxxPreProcessorCache::Map_t::iterator iter = m_impl.find(key);
    if(iter != m_impl.end()) m_impl.erase(iter);
    
    CacheEntry entry;
    entry.config = config;
    entry.definitions = definitions;
    entry.lastUpdated = time(NULL); // Now
    entry.filename = filename;
    entry.project = project;
    entry.preamble = GetPreamble(filename);
    m_impl.insert(std::make_pair(key, entry));
    return true;
}
