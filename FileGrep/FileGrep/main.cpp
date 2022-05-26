#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// templated version of my_equal so it could work with both char and wchar_t
template <typename charT> struct my_equal {
    my_equal(const std::locale& loc)
        : loc_(loc)
    {
    }
    bool operator()(charT ch1, charT ch2) { return std::toupper(ch1, loc_) == std::toupper(ch2, loc_); }

private:
    const std::locale& loc_;
};

// find substring (case insensitive)
int ci_find_substr(const std::string& str1, const std::string& str2, const std::locale& loc = std::locale())
{
    std::string::const_iterator it =
        std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), my_equal<std::string::value_type>(loc));
    if(it != str1.end()) {
        return it - str1.begin();
    } else {
        return -1; // not found
    }
}

int main(int argc, char** argv)
{
    FILE* fp = NULL;
    FILE* ofp = NULL;

    if(argc < 2) {
        printf("usage: %s <filename> <filter>\n", argv[0]);
        return 1;
    }

    if(argc < 3) {
        // no filter?
        // do nothing
        return 0;
    }

    fp = fopen(argv[1], "r");
    if(!fp) {
        return 2;
    }

    char out_file_name[1024];
    sprintf(out_file_name, "%s.tmp_result", argv[1]);
    ofp = fopen(out_file_name, "w");

    char line[2048];
    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), fp)) {
        if(ci_find_substr(line, argv[2]) != -1) {
            // we got a match
            fwrite(line, sizeof(char), strlen(line), ofp);
        }
        memset(line, 0, sizeof(line));
    }

    // read the file content and close the file
    fclose(fp);
    fclose(ofp);

    remove(argv[1]);
    rename(out_file_name, argv[1]);
    return 0;
}
