#ifndef IO_HELPER
#define IO_HELPER

#include <string>
#include <string.h>
#include <windows.h>
#include <strsafe.h>
#include "Utility.h"

using namespace std;


namespace Utility{
	class IOHelper{
	private:
	public:

		static const char* IO_CHAR_DOT;
		static const char* IO_CHAR_SLASH;
		static const char* IO_CHAR_SLASH_2 ;

		static char* Strrstr(char* str1,const char* str2){
			char* ret = NULL;
			int len1 = strlen(str1);
			int len2 = strlen(str2);
			if (len2 == 0)
				return ret;
			for (char* i = str1 + len1 - len2; i >= str1; i--){
				char buffer[_MAX_PATH];
				strncpy_s(buffer, i, len2);
				if (strcmp(buffer, str2) == 0){
					ret = i;
					break;
				}
			}
			return ret;
		}

		static string SplitSuffix(char* fullname){
			char buffer[_MAX_PATH];
			char* token, *nextToken;
			strcpy_s(buffer, fullname);
			token = strtok_s(buffer, IO_CHAR_DOT, &nextToken);
			return string(nextToken);
		}
		static string SplifPrefix(char* fullname){
			string filename = SplitFilename(fullname);
			char buffer[_MAX_PATH];
			char* token, *nextToken;
			strcpy_s(buffer, filename.c_str());
			token = strtok_s(buffer, IO_CHAR_DOT, &nextToken);
			return string(token);
		}
		static string SplitDirectory(char* fullname){
			char buffer[_MAX_PATH];
			char* pos1 = Strrstr(fullname, IO_CHAR_SLASH);
			char* pos2 = Strrstr(fullname, IO_CHAR_SLASH_2);
			if (pos1 != NULL){
				strncpy_s(buffer, fullname, pos1 - fullname);
				strcat_s(buffer, IO_CHAR_SLASH);
			}
			else if (pos2 != NULL){
				strncpy_s(buffer, fullname, pos2 - fullname);
				strcat_s(buffer, IO_CHAR_SLASH_2);
			}
			return string(buffer);
		}
		static string SplitFilename(char* fullname){
			char buffer[_MAX_PATH];
			char* pos1 = Strrstr(fullname, IO_CHAR_SLASH);
			char* pos2 = Strrstr(fullname, IO_CHAR_SLASH_2);
			if (pos1 != NULL)
				strcpy_s(buffer, pos1 + 1);
			else if (pos2 != NULL)
				strcpy_s(buffer, pos2 + 1);
			return string(buffer);
		}

		static void CurrentDirectory(TCHAR* buffer){
			GetCurrentDirectory(MAX_PATH, buffer);
		}
		static List<TCHAR*> ListDirectoryFiles(TCHAR* dir){
			TCHAR folder[MAX_PATH];
			StringCchCopy(folder, MAX_PATH, dir);
			StringCchCat(folder, MAX_PATH, TEXT("*"));
			List<TCHAR*> files;
			WIN32_FIND_DATA fd;
			HANDLE hFind = ::FindFirstFile(folder, &fd);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						TCHAR* file = new TCHAR[MAX_PATH];
						StringCchCopy(file, MAX_PATH, fd.cFileName);
						files.Add(LPTSTR(file));
					}
				} while (::FindNextFile(hFind, &fd));
				::FindClose(hFind);
			}
			return files;
		}
		static void ReleaseDirectoryList(List<TCHAR*>& list){
			for (int i = 0; i < list.Count(); i++)
				delete[] list[i];
		}
	};
}

#endif