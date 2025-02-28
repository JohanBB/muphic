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
#ifndef BUILDTOOLS_H
#define BUILDTOOLS_H
//------------------------------------------------------------------------------
#include "stlconfig.h"
#include "stlstrings.h"
#include "platforms.h"
#include "depsearch.h"
//------------------------------------------------------------------------------

class TiXmlElement;

class CBuildTool
{
 public:
  enum ToolType
  {
   btOther,
   btPreprocessor,
   btAssembler,
   btCompiler,
   btResourceCompiler,
   btStaticLinker,
   btDynamicLinker,
   btExecutableLinker,
   btNativeLinker,
   //btBuildManager,
   btCount
  };
 protected:
  CPlatform::OS_Type m_Platform;
  CBuildTool::ToolType m_Type;
  CString m_Alias;
  CString m_Description;
  CString m_Program;
  CString m_MakeVariable;
  CString m_CommandTemplate;
  CStringList m_SourceExtensions;
  CString m_TargetExtension;
  bool m_NeedQuotedPath;
  bool m_NeedFullPath;
  bool m_NeedUnixPath;
 protected:
  void Read(const TiXmlElement *Root, const CString& Name, CString& Value);
  void Read(const TiXmlElement *Root, const CString& Name, bool& Value);
  void Write(TiXmlElement *Root, const CString& Name, const CString& Value);
  void Write(TiXmlElement *Root, const CString& Name, const bool Value);
 public:
  static ToolType Type(const CString& Name);
  static CString TypeName(const ToolType Type);
  static CString AbbrevTypeName(const ToolType Type);
  CBuildTool::ToolType Type(void) const { return m_Type; }
  CString TypeName(void) const;
  CString& Alias(void) { return m_Alias; }
  CString& Description(void) { return m_Description; }
  CString& Program(void) { return m_Program; }
  CString& MakeVariable(void) { return m_MakeVariable; }
  CString& CommandTemplate(void) { return m_CommandTemplate; }
  CStringList& SourceExtensions(void) { return m_SourceExtensions; }
  CString& TargetExtension(void) { return m_TargetExtension; }
  bool& NeedQuotedPath(void) { return m_NeedQuotedPath; }
  bool& NeedFullPath(void) { return m_NeedFullPath; }
  bool& NeedUnixPath(void) { return m_NeedUnixPath; }
  CString MakeCommand(const CString& CommandTemplate, CConfiguration& Arguments);
  CString MakeCommand(CConfiguration& Arguments);
  virtual bool ExpectedSourceExtension(const CString& FileExtension);
  virtual CBuildTool *CreateInstance(void);
  virtual void Clear(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  virtual bool Supports(const CPlatform::OS_Type OS);
  virtual void Read(const TiXmlElement *BuildToolRoot);
  virtual void Write(TiXmlElement *BuildToolRoot);
  virtual void Show(void);
 public:
  CBuildTool(void);
  CBuildTool(const CBuildTool& BuildTool);
  virtual ~CBuildTool(void);
};

//------------------------------------------------------------------------------

class CPreprocessor: public CBuildTool
{
 protected:
  CString m_IncludeDirSwitch;
  CString m_DefineSwitch;
 public:
  virtual CPreprocessor *CreateInstance(void);
  virtual void Read(const TiXmlElement *BuildToolRoot);
  virtual void Write(TiXmlElement *BuildToolRoot);
  virtual void Show(void);
 public:
  CPreprocessor(void);
  CPreprocessor(const CPreprocessor& Preprocessor);
  virtual ~CPreprocessor(void);
};

class CCompiler: public CBuildTool
{
 protected:
  CString m_IncludeDirSwitch;
  CString m_DefineSwitch;
  bool m_NeedDependencies;
 public:
  CString& IncludeDirSwitch(void) { return m_IncludeDirSwitch; }
  CString& DefineSwitch(void) { return m_DefineSwitch; }
  bool& NeedDependencies(void) { return m_NeedDependencies; }
 public:
  virtual CIncludeSearchFilter *IncludeSearchFilter(void) const { return 0; }
  virtual CCompiler *CreateInstance(void);
  virtual void Read(const TiXmlElement *BuildToolRoot);
  virtual void Write(TiXmlElement *BuildToolRoot);
  virtual void Show(void);
 public:
  CCompiler(void);
  CCompiler(const CCompiler& Compiler);
  virtual ~CCompiler(void);
};

class CAssembler: public CCompiler
{
 protected:
 public:
  virtual CAssembler *CreateInstance(void);
 public:
  CAssembler(void);
  CAssembler(const CAssembler& Assembler);
  virtual ~CAssembler(void);
};

class CResourceCompiler: public CCompiler
{
 protected:
 public:
  virtual CResourceCompiler *CreateInstance(void);
 public:
  CResourceCompiler(void);
  CResourceCompiler(const CResourceCompiler& ResourceCompiler);
  virtual ~CResourceCompiler(void);
};

class CLinker: public CBuildTool
{
 protected:
  CString m_LibraryDirSwitch;
  CString m_LinkLibrarySwitch;
  CString m_ObjectExtension;
  CString m_LibraryPrefix;
  CString m_LibraryExtension;
  bool m_NeedLibraryPrefix;
  bool m_NeedLibraryExtension;
  bool m_NeedFlatObjects;
 public:
  CString& LibraryDirSwitch(void) { return m_LibraryDirSwitch; }
  CString& LinkLibrarySwitch(void) { return m_LinkLibrarySwitch; }
  CString& ObjectExtension(void) { return m_ObjectExtension; }
  CString& LibraryPrefix(void) { return m_LibraryPrefix; }
  CString& LibraryExtension(void) { return m_LibraryExtension; }
  bool& NeedLibraryPrefix(void) { return m_NeedLibraryPrefix; }
  bool& NeedLibraryExtension(void) { return m_NeedLibraryExtension; }
  bool& NeedFlatObjects(void) { return m_NeedFlatObjects; }
 public:
  virtual CLinker *CreateInstance(void);
  virtual void Read(const TiXmlElement *BuildToolRoot);
  virtual void Write(TiXmlElement *BuildToolRoot);
  virtual void Show(void);
 public:
  CLinker(void);
  CLinker(const CLinker& Linker);
  virtual ~CLinker(void);
};

class CLibraryLinker: public CLinker
{
 protected:
 public:
  virtual CLibraryLinker *CreateInstance(void);
 public:
  CLibraryLinker(void);
  CLibraryLinker(const CLibraryLinker& LibraryLinker);
  virtual ~CLibraryLinker(void);
};

class CStaticLinker: public CLibraryLinker
{
 protected:
 public:
  virtual CStaticLinker *CreateInstance(void);
 public:
  CStaticLinker(void);
  CStaticLinker(const CStaticLinker& StaticLinker);
  virtual ~CStaticLinker(void);
};

class CDynamicLinker: public CLibraryLinker
{
 protected:
 public:
  virtual CDynamicLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
 public:
  CDynamicLinker(void);
  CDynamicLinker(const CDynamicLinker& DynamicLinker);
  virtual ~CDynamicLinker(void);
};

class CExecutableLinker: public CLinker
{
 protected:
 public:
  virtual CExecutableLinker *CreateInstance(void);
 public:
  CExecutableLinker(void);
  CExecutableLinker(const CExecutableLinker& ExecutableLinker);
  virtual ~CExecutableLinker(void);
};

//------------------------------------------------------------------------------

class CGNUCCompiler: public CCompiler
{
 private:
  CCppIncludeSearchFilter m_IncludeSearchFilter;
 public:
  virtual CIncludeSearchFilter *IncludeSearchFilter(void) const;
  virtual CGNUCCompiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUCCompiler(void);
};

class CGNUCppCompiler: public CCompiler
{
 private:
  CCppIncludeSearchFilter m_IncludeSearchFilter;
 public:
  virtual CIncludeSearchFilter *IncludeSearchFilter(void) const;
  virtual CGNUCppCompiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUCppCompiler(void);
};

class CGNUFortran77Compiler: public CCompiler
{
 public:
  virtual CGNUFortran77Compiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUFortran77Compiler(void);
};

class CGNUFortran90Compiler: public CCompiler
{
 public:
  virtual CGNUFortran90Compiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUFortran90Compiler(void);
};

class CGNUWindowsResourceCompiler: public CResourceCompiler
{
 public:
  virtual CGNUWindowsResourceCompiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  virtual bool Supports(const CPlatform::OS_Type OS);
  CGNUWindowsResourceCompiler(void);
};

class CGNUStaticLinker: public CStaticLinker
{
 public:
  virtual CGNUStaticLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUStaticLinker(void);
};

class CGNUDynamicLinker: public CDynamicLinker
{
 public:
  virtual CGNUDynamicLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUDynamicLinker(void);
};

class CGNUExecutableLinker: public CExecutableLinker
{
 public:
  virtual CGNUExecutableLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CGNUExecutableLinker(void);
};

//------------------------------------------------------------------------------

class CIntelCCompiler: public CCompiler
{
 private:
  CCppIncludeSearchFilter m_IncludeSearchFilter;
 public:
  virtual CIncludeSearchFilter *IncludeSearchFilter(void) const;
  virtual CIntelCCompiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CIntelCCompiler(void);
};

class CIntelCppCompiler: public CCompiler
{
 private:
  CCppIncludeSearchFilter m_IncludeSearchFilter;
 public:
  virtual CIncludeSearchFilter *IncludeSearchFilter(void) const;
  virtual CIntelCppCompiler *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CIntelCppCompiler(void);
};

class CIntelStaticLinker: public CStaticLinker
{
 public:
  virtual CIntelStaticLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CIntelStaticLinker(void);
};

class CIntelDynamicLinker: public CDynamicLinker
{
 public:
  virtual CIntelDynamicLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CIntelDynamicLinker(void);
};

class CIntelExecutableLinker: public CExecutableLinker
{
 public:
  virtual CIntelExecutableLinker *CreateInstance(void);
  virtual void Reset(const CPlatform::OS_Type OS);
  CIntelExecutableLinker(void);
};

//------------------------------------------------------------------------------

static const CString TPL_COMPILER        = "$compiler";
static const CString TPL_RES_COMPILER    = "$rescomp";
static const CString TPL_LINKER          = "$linker";
static const CString TPL_LIB_LINKER      = "$lib_linker";
static const CString TPL_OPTIONS         = "$options";
static const CString TPL_RES_INCLUDES    = "$res_includes";
static const CString TPL_INCLUDES        = "$includes";
static const CString TPL_FILE            = "$file";
static const CString TPL_OBJECT          = "$object";
static const CString TPL_RES_OUTPUT      = "$resource_output";
static const CString TPL_OUTPUT          = "$output";
static const CString TPL_STL_OUTPUT      = "$static_output";
static const CString TPL_EXE_OUTPUT      = "$exe_output";
static const CString TPL_LNK_OPTIONS     = "$link_options";
static const CString TPL_LNK_OBJECTS     = "$link_objects";
static const CString TPL_LNK_RES_OBJECTS = "$link_resobjects";
static const CString TPL_LIB_DIRS        = "$libdirs";
static const CString TPL_LIBS            = "$libs";
//static const CString TPL_ = "$";

#endif
//------------------------------------------------------------------------------
