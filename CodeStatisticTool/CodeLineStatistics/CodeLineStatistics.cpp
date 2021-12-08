
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define SEARCH_PATH "C:/"

vector<string> _suffix =
{
    //"cpp",
    //"hpp",
    //"c",
    //"h",
    //"inl",
    //"txt",
};

struct INFO
{
    unsigned int lines;
    unsigned int lines_valid;
};

bool __start_slash_slash(const string&);
bool __start_slash_star(const string&);
bool __end_star_slash(const string&);

void __iterator(const string &filePath, unsigned int prefixLen, INFO *pInfo)
{
    static char buf[1000];
    bool HeaderContext = false;
    unsigned int lines = 0;
    unsigned int valid_lines = 0;
    cout << ">>> " << filePath.substr(prefixLen);
    FILE *f = fopen(filePath.c_str(), "r");
    while (!feof(f))
    {
        ++lines;
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf) - 1, f);
        string s(buf);

        if (lines < 3 && __start_slash_star(s) && HeaderContext == false)
        {
            HeaderContext = true;
        }

        if (HeaderContext)
        {
            if (__end_star_slash(s))
            {
                HeaderContext = false;
            }
            continue;
        }

        if (__start_slash_slash(s))
            continue;

        for (auto ch : s)
        {
            if (ch >= 'a' && ch <= 'z')
            {
                ++valid_lines;
                break;
            }
            else if (ch >= 'A' && ch <= 'Z')
            {
                ++valid_lines;
                break;
            }
            else if (ch >= '0' && ch <= '9')
            {
                ++valid_lines;
                break;
            }
        }
        //cout << "   --- " << buf << endl;
    }
    cout << " : " << valid_lines << '/' << lines << '\n';
    pInfo->lines += lines;
    pInfo->lines_valid += valid_lines;
    fclose(f);
}

unsigned int __find(const string &path, const string &suffix, unsigned int prefixLen, INFO *pInfo)
{
    HANDLE hFind;
    WIN32_FIND_DATA wfd;
    unsigned int cnt = 0;

    hFind = FindFirstFile((path + "*." + suffix).c_str(), &wfd);
    while (hFind != INVALID_HANDLE_VALUE)
    {
        ++cnt;
        string file_full_path = path + wfd.cFileName;
        __iterator(file_full_path, prefixLen, pInfo);
        if (FindNextFile(hFind, &wfd) == FALSE)
        {
            break;
        }
    }
    FindClose(hFind);

    hFind = FindFirstFile((path + "*").c_str(), &wfd);
    while (hFind != INVALID_HANDLE_VALUE)
    {
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(wfd.cFileName, ".") == 0)
            {
                goto DONE;
            }
            if (strcmp(wfd.cFileName, "..") == 0)
            {
                goto DONE;
            }
            cnt += __find(path + wfd.cFileName + "/", suffix, prefixLen, pInfo);
        }
        DONE:
        if (FindNextFile(hFind, &wfd) == FALSE)
        {
            break;
        }
    }
    FindClose(hFind);

    return cnt;
}

bool __start_slash_slash(const string &s)
{
    for (int i = 0; i < s.size() - 1; ++i)
    {
        if (s[i] == ' ')
            continue;
        if (s[i] == '/' && s[i + 1] == '/')
            return true;
        return false;
    }
    return false;
}

bool __start_slash_star(const string &s)
{
    for (int i = 0; i < s.size() - 1; ++i)
    {
        if (s[i] == ' ')
            continue;
        if (s[i] == '/' && s[i + 1] == '*')
            return true;
        return false;
    }
    return false;
}

bool __end_star_slash(const string &s)
{
    for (int i = s.size() - 1; i > 0; --i)
    {
        if (s[i] == ' ' || s[i] == '\n')
            continue;
        if (s[i - 1] == '*' && s[i] == '/')
            return true;
        return false;
    }
    return false;
}

int main(int argc, const char *args[])
{
    //string path = SEARCH_PATH;
    string path = args[0];
    long long int i = path.size();
    --i;
    while (path[i] != '\\')
    {
        --i;
    }
    path = path.substr(0, i + 1);

    string input;
    cout << "Please input file suffix, separated by space :" << endl;
    getline(cin, input);

    int m = 0;
    int len = 0;

    while (m < input.size())
    {
        if (
            (input[m] >= 'a' && input[m] <= 'z') ||
            (input[m] >= 'A' && input[m] <= 'Z') ||
            (input[m] >= '0' && input[m] <= '9') ||
            (input[m] == '_')
           )
        {
            ++len;
            ++m;
            continue;
        }
        else
        {
            if (len)
            {
                _suffix.push_back(input.substr(m - len, len));
                len = 0;
            }
            ++m;
            continue;
        }
    }
    if (len)
    {
        _suffix.push_back(input.substr(m - len, len));
    }

    if (_suffix.size() == 0)
    {
        _suffix.push_back("hpp");
        _suffix.push_back("cpp");
        _suffix.push_back("h");
        _suffix.push_back("c");
        _suffix.push_back("inl");
    }

    vector<INFO> infos;
    for (auto s : _suffix)
    {
        INFO info = {};
        __find(path, s, (unsigned int)path.size(), &info);
        infos.push_back(info);
    }

    cout << "=========================================================" << endl;
    for (int i = 0; i != infos.size(); ++i)
    {
        cout << "For ." << _suffix[i] << "" << endl;
        cout << "   Valid Lines : " << infos[i].lines_valid << endl;
        cout << "   Total Lines : " << infos[i].lines << endl;
        if (infos[i].lines_valid == 0 || infos[i].lines == 0)
        {
            cout << "   Valid Rate  : 0.0%" << endl;
            cout << "=========================================================" << endl;
            continue;
        }
        float rate = float(infos[i].lines_valid) / float(infos[i].lines);
        int decade, unit, fraction;
        cout << "   Valid Rate  : ";
        decade = (int)(rate / 0.1f);
        rate -= decade * 0.1f;
        unit = (int)(rate / 0.01f);
        rate -= unit * 0.01f;
        fraction = (int)(rate / 0.001f);
        cout << decade << unit << "." << fraction << "%" << endl;
        cout << "=========================================================" << endl;
    }

    system("pause");
    return 0;
}