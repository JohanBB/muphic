/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2011 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>
//#include <iostream>
//------------------------------------------------------------------------------
#include "stlfutils.h"
//------------------------------------------------------------------------------
#ifdef OS_UNIX
#include "sys/stat.h"
#endif
#ifdef OS_WIN
#include "windows.h"
#endif
//------------------------------------------------------------------------------

const int MAX_PATH_LENGTH = 0x10000;

char PathDelimiter(void)
{
#ifdef OS_WIN
 return '\\';
#else
 return '/';
#endif
}

CString NativePathDelimiter(void)
{
#ifdef OS_WIN
 return "\\";
#else
 return "/";
#endif
}

CString ChangeFileExt(const CString& FileName, const CString& FileExt)
{
 CString result(FileName);
 int pos = LastCharPos(FileName,'.');
 if (pos>=0) result.SetLength(pos);
 result.Append(FileExt);
 return result;
}

CString ExtractFilePath(const CString& FilePathName)
{
 CString result;
 int pos = std::max(LastCharPos(FilePathName,'/'),LastCharPos(FilePathName,'\\'));
 if (pos >= 1)
 {
  result = LeftStr(FilePathName,pos-1);
 }
 return result;
}

CString ExtractFileName(const CString& FilePathName)
{
 CString result = FilePathName;
 int pos = std::max(LastCharPos(FilePathName,'/'),LastCharPos(FilePathName,'\\'));
 if (pos >= 0)
 {
  result = RightStr(FilePathName,pos+1);
 }
 return result;
}

CString ExtractFileExt(const CString& FilePathName)
{
 CString result;
 int pos = LastCharPos(FilePathName,'.');
 if (pos >= 0)
 {
  result = RightStr(FilePathName,pos+1);
 }
 return result;
}

CString MakeUnixPath(const CString& FilePathName)
{
 return FindReplaceChar(FilePathName,'\\','/');
}

CString MakeWindowsPath(const CString& FilePathName)
{
 return FindReplaceChar(FilePathName,'/','\\');
}

CString MakeNativePath(const CString& FilePathName)
{
#ifdef OS_WIN
 return MakeWindowsPath(FilePathName);
#else
 return MakeUnixPath(FilePathName);
#endif
}

CString RemoveTrailingPathDelimiter(const CString& FilePathName)
{
 CString result = FilePathName;
 if (!FilePathName.IsEmpty())
 {
  if ((FilePathName.GetLastChar()=='/')||(FilePathName.GetLastChar()=='\\'))
  {
   result.SetLength(result.GetLength()-1);
  }
 }
 return result;
}

CString IncludeTrailingPathDelimiter(const CString& FilePathName)
{
 return CheckLastChar(FilePathName,PathDelimiter());
}

CString QuoteSpaces(const CString& FilePathName)
{
 CString result = FilePathName;
 if (CountChars(FilePathName,' '))
 {
  result = "\""+FilePathName+"\"";
 }
 return result;
}

CString JoinPaths(const CString& HeadPath, const CString& TailPath)
{
 CString result = HeadPath;
 if (!HeadPath.IsEmpty() && !TailPath.IsEmpty())
 {
  result = CheckLastChar(HeadPath,PathDelimiter());
 }
 result += TailPath;
 return result;
}

bool FileExists(const CString& FileName)
{
 bool result = false;
#ifdef OS_UNIX
 struct stat st;
 if (stat(FileName.GetCString(),&st)==0)
 {
  result = S_ISREG(st.st_mode);
 }
#else
 FILE* stream = fopen(FileName.GetCString(),"r");
 result = (NULL!=stream);
 if (result)
 {
  fclose(stream);
 }
#endif
 return result;
}

bool DirExists(const CString& DirPath)
{
 bool result = false;
#ifdef OS_WIN
 /*
 WIN32_FIND_DATA w32fd;
 HANDLE h = FindFirstFile(DirPath.GetCString(),&w32fd);
 result = (INVALID_HANDLE_VALUE!=h)&&(ERROR_FILE_NOT_FOUND!=h);
 */
 DWORD attr = GetFileAttributes(DirPath.GetCString());
 result = (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
 struct stat st;
 if (stat(DirPath.GetCString(),&st) == 0)
 {
  result = S_ISDIR(st.st_mode);
 }
#endif
 return result;
}

bool MakeDir(const CString& DirPath, int DirMode)
{
#ifdef OS_WIN
 return CreateDirectory(DirPath.GetCString(),0);
#else
 return (mkdir(DirPath.GetCString(),DirMode)==0);
#endif
}

CString RootDirPath(void)
{
 CString result;
#ifdef OS_WIN
 result = getenv("SystemDrive");
 if (result.IsEmpty())
 {
  result = "C:";
 }
 result = CheckLastChar(result,'\\');
#else
 result = "/";
#endif
 return result;
}

CString HomeDirPath(void)
{
 CString result;
#ifdef OS_WIN
 result = getenv("USERPROFILE");
 if (result.IsEmpty() || !DirExists(result))
 {
  result = CString(getenv("HOMEDRIVE"))+CString(getenv("HOMEPATH"));
  if (result.IsEmpty() || !DirExists(result))
  {
   result = getenv("HOME");
   if (result.IsEmpty() || !DirExists(result))
   {
    result = RootDirPath();
 }}}
#else
 result = getenv("HOME");
 if (result.IsEmpty())
 {
  result = RootDirPath();
 }
#endif
 return result;
}

CString TempDirPath(void)
{
 CString result;
#ifdef OS_WIN
 char tmp[MAX_PATH_LENGTH];
 if (GetTempPath(MAX_PATH_LENGTH,tmp))
 {
  result.Assign((char*)&tmp);
 }
 if (result.IsEmpty())
 {
  result = RootDirPath()+"Temp\\";
 }
#else
 result = getenv("TMPDIR");
 if (result.IsEmpty())
 {
  result = "/tmp/";
 }
#endif
 return result;
}

CString GetCurrentDir(void)
{
 CString result; result.SetLength(MAX_PATH_LENGTH);
 if (0!=getcwd(result.GetCString(),MAX_PATH_LENGTH))
 {
  result.SetLength();
  return result;
 }
 return "";
}

bool ChangeDir(const CString& DirPath)
{
 return (0==chdir(DirPath.GetCString()));
}

//------------------------------------------------------------------------------
