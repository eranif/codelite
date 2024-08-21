#include <wx/filename.h>
#include <wx/ffile.h>
#include <wx/init.h>	//wxInitializer
#include <wx/string.h>	//wxString
#include <wx/wxcrtvararg.h>

wxString Bin2C(wxString filename)
{
	wxString res = wxEmptyString;
	FILE            *fp;
	long            size, i, fsize;
	unsigned char   data;
	wxFileName 		fn(filename);
	
	if ((fp = fopen(filename.mb_str(), "rb")) != NULL) {

		fseek(fp, 0, SEEK_END);
		fsize = size = ftell(fp);
		rewind(fp);

		wxString validFilename = fn.GetName().Lower();
		validFilename.Replace(_T(">"), _T("_"));
		validFilename.Replace(_T("<"), _T("_"));
		validFilename.Replace(_T("="), _T("_"));
		validFilename.Replace(_T("!"), _T("_"));
		validFilename.Replace(_T("&"), _T("_"));
		validFilename.Replace(_T("|"), _T("_"));
		validFilename.Replace(_T("-"), _T("_"));
		validFilename.Replace(_T("["), _T("_"));
		validFilename.Replace(_T("]"), _T("_"));
		validFilename.Replace(_T("^"), _T("_"));
		validFilename.Replace(_T(":"), _T("_"));
		validFilename.Replace(_T(","), _T("_"));
		validFilename.Replace(_T("{"), _T("_"));
		validFilename.Replace(_T("}"), _T("_"));
		validFilename.Replace(_T("."), _T("_"));
		validFilename.Replace(_T("*"), _T("_"));
		validFilename.Replace(_T("("), _T("_"));
		validFilename.Replace(_T(")"), _T("_"));
		validFilename.Replace(_T("+"), _T("_"));
		validFilename.Replace(_T("%"), _T("_"));
		validFilename.Replace(_T("#"), _T("_"));
		validFilename.Replace(_T("?"), _T("_"));
		validFilename.Replace(_T("/"), _T("_"));
		validFilename.Replace(_T("*"), _T("_"));
		validFilename.Replace(_T("~"), _T("_"));
		validFilename.Replace(_T("\\"), _T("_"));
		validFilename.Replace(_T("."), _T("_"));
		validFilename.Replace(_T(" "), _T("_"));
		res += wxString::Format(wxT("const char %s_hex [] ="), validFilename.GetData());
		res += wxT("{\n");

		i = 0;
		while (size--) {
			fread(&data, sizeof(unsigned char), 1, fp);

			if (!i)
				res += wxT("\t");
			if (size == 0) {
				res += wxString::Format(wxT("0x%02X,\n"), data);
				res += wxT("\t");
				res += wxString::Format(wxT("0x00};\n"));
			} else {
				res += wxString::Format(wxT("0x%02X,"), data);
			}
			i++;
			if (i == 14) {
				res += wxT("\n");
				i = 0;
			}
		}
		fclose(fp);
	}
	return res;
}


int main(int argc, char **argv) 
{
	//Initialize the wxWidgets library
	wxInitializer initializer;
	
	if(argc < 3) {
		wxPrintf(wxT("Usage: file2hex <input file> <output file>\n"));
		return 1;
	}
	
	wxString inputFile(argv[1], wxConvUTF8);
	wxString outputFile(argv[2],wxConvUTF8);
	
	wxString hexStr = Bin2C(inputFile);
	
	wxFFile o(outputFile.c_str(), wxT("w+b"));
	if(o.IsOpened()){
		o.Write(hexStr);
		o.Close();
	}
	
	return 0;
}

