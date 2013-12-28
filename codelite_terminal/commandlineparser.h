#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <wx/string.h>
#include <map>
#include <list>

class CommandLineParser
{
public:
    enum {
        kOptionWithValue = 0x00000001,
        kOptional        = 0x00000002,
        kDefault         = kOptional, // Optional without value
    };

    class OptionInfo
    {
        size_t   m_flags;
        wxString m_longName;
        wxString m_shortName;
        wxString m_predicateName;

    public:
        typedef std::list<OptionInfo> List_t;

    public:
        OptionInfo() : m_flags(0) {}
        OptionInfo(const wxString& lname, const wxString &sname)
            : m_flags(CommandLineParser::kDefault)
            , m_longName(lname)
            , m_shortName(sname)
        {}

        OptionInfo(const wxString& predicateName)
            : m_flags(0)
        {
            wxString noPrefixName = predicateName;
            noPrefixName.StartsWith("--", &noPrefixName);
            noPrefixName.StartsWith("-", &noPrefixName);
            noPrefixName.StartsWith("/", &noPrefixName);
            m_predicateName = noPrefixName;
        }

        ~OptionInfo() {}

        bool IsOk() const {
            return !m_shortName.IsEmpty() && !m_longName.IsEmpty();
        }

        bool operator()(const OptionInfo& other) const {

            return m_predicateName == other.m_shortName || m_predicateName == other.m_longName;
        }

        void SetFlags(size_t flags) {
            this->m_flags = flags;
        }
        void SetLongName(const wxString& longName) {
            this->m_longName = longName;
        }
        void SetShortName(const wxString& shortName) {
            this->m_shortName = shortName;
        }
        size_t GetFlags() const {
            return m_flags;
        }
        const wxString& GetLongName() const {
            return m_longName;
        }
        const wxString& GetShortName() const {
            return m_shortName;
        }
    };

protected:
    typedef std::map<wxString, wxString> Map_t;

    CommandLineParser::Map_t m_argsMap;
    OptionInfo::List_t       m_optionsInfo;
    wxString                 m_command;
    int                      m_argc;
    char**                   m_argv;
    int                      m_curIndex;
    bool                     m_used;

protected:
    void DoParse();
    wxString NextToken();
    wxString PeekNextToken();
    bool HasMore();

    CommandLineParser::OptionInfo GetOptionInfo(const wxString &name) const;
    wxString StripDash(const wxString &opt) const;
    
public:
    CommandLineParser(int argc, char **argv);
    virtual ~CommandLineParser();

    wxString GetArg(const wxString& shortOpt, const wxString &longOption) const;

    void SetCommand(const wxString& command) {
        this->m_command = command;
    }
    const wxString& GetCommand() const {
        return m_command;
    }

    bool HasOption(const wxString& shortOpt, const wxString &longOption) const;

    void AddOption(const wxString &shortOpt, const wxString &longOpt, size_t flags = CommandLineParser::kDefault);
    void Parse() {
        DoParse();
    }

};

#endif // COMMANDLINEPARSER_H
