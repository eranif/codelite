#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif

struct WXDLLIMPEXP_CL PPToken {
	enum {
		IsFunctionLike = 0x00000001,
		IsValid        = 0x00000002,
		IsOverridable  = 0x00000004
	};

	int             line;           // line where found
	wxString        name;           // preprocessor name
	wxString        replacement;    // un processed replacement
	wxArrayString   args;           // for function like macros, contains the argument's names
	size_t          flags;          // PP token flags

	PPToken() : line(0), flags(IsOverridable)
	{}

	~PPToken()
	{}

	void expandOnce(const wxArrayString& initList);
	void processArgs(const wxString &argsList);
	wxString signature() const;
	void print(wxFFile &fp);
	static bool readInitList(const wxString &in, int from, wxString& initList, wxArrayString &initListArr);
	static bool readInitList(const std::string &in, size_t from, std::string& initList, std::vector<std::string> &initListArr);
	wxString fullname();
	void squeeze();
};

class WXDLLIMPEXP_CL PPTable
{
	std::map<wxString, PPToken> m_table;
	std::set<wxString>          m_namesUsed;

public:
	static PPTable* Instance();
	static void Release();

private:
	PPTable();
	~PPTable();

public:
	PPToken   Token(const wxString &name);
	bool      Contains(const wxString &name);
	void      Add  (const PPToken& token);
	void      AddUsed(const wxString& name);
