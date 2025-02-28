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
#include <iostream>
//------------------------------------------------------------------------------
#include "buildtools.h"
#include "stlconvert.h"
#include "cbhelper.h"
#include "tinyxml.h"
//------------------------------------------------------------------------------

CBuildTool::CBuildTool(void)
{
 Clear();
}

CBuildTool::CBuildTool(const CBuildTool& BuildTool)
{
 m_Platform         = BuildTool.m_Platform;
 m_Alias            = BuildTool.m_Alias;
 m_Type             = BuildTool.m_Type;
 m_Description      = BuildTool.m_Description;
 m_Program          = BuildTool.m_Program;
 m_MakeVariable     = BuildTool.m_MakeVariable;
 m_CommandTemplate  = BuildTool.m_CommandTemplate;
 m_SourceExtensions = BuildTool.m_SourceExtensions;
 m_TargetExtension  = BuildTool.m_TargetExtension;
 m_NeedQuotedPath   = BuildTool.m_NeedQuotedPath;
 m_NeedFullPath     = BuildTool.m_NeedFullPath;
 m_NeedUnixPath     = BuildTool.m_NeedUnixPath;
}

CBuildTool::~CBuildTool(void)
{
 Clear();
}

CBuildTool *CBuildTool::CreateInstance(void)
{
 return new CBuildTool(*this);
}

void CBuildTool::Clear(void)
{
 m_Platform = CPlatform::OS_Other;
 m_Type = CBuildTool::btOther;
 m_Alias.Clear();
 m_Description.Clear();
 m_Program.Clear();
 m_MakeVariable.Clear();
 m_CommandTemplate.Clear();
 m_SourceExtensions.Clear();
 m_TargetExtension.Clear();
 m_NeedQuotedPath = false;
 m_NeedFullPath = false;
 m_NeedUnixPath = false;
}

CBuildTool::ToolType CBuildTool::Type(const CString& Name)
{
 for (int i = btOther; i < btCount; i++)
 {
  CBuildTool::ToolType tt = (CBuildTool::ToolType)i;
  if ((Name==XMLFriendly(TypeName(tt)))||(Name==TypeName(tt))||(Name==AbbrevTypeName(tt)))
  {
   return tt;
  }
 }
 return CBuildTool::btOther;
}

CString CBuildTool::TypeName(const CBuildTool::ToolType Type)
{
 switch (Type)
 {
  default:
  case CBuildTool::btCount:
  case CBuildTool::btOther:            { return "Other"; }
  case CBuildTool::btPreprocessor:     { return "Preprocessor"; }
  case CBuildTool::btAssembler:        { return "Assembler"; }
  case CBuildTool::btCompiler:         { return "Compiler"; }
  case CBuildTool::btResourceCompiler: { return "Resource compiler"; }
  case CBuildTool::btStaticLinker:     { return "Static library linker"; }
  case CBuildTool::btDynamicLinker:    { return "Dynamic library linker"; }
  case CBuildTool::btExecutableLinker: { return "Executable binary linker"; }
  case CBuildTool::btNativeLinker: { return "Native binary linker"; }

  //case CBuildTool::btBuildManager,
 }
 return "Other";
}

CString CBuildTool::AbbrevTypeName(const CBuildTool::ToolType Type)
{
 switch (Type)
 {
  default:
  case CBuildTool::btCount:
  case CBuildTool::btOther:            { return "bt"; }
  case CBuildTool::btPreprocessor:     { return "pp"; }
  case CBuildTool::btAssembler:        { return "as"; }
  case CBuildTool::btCompiler:         { return "cc"; }
  case CBuildTool::btResourceCompiler: { return "rc"; }
  case CBuildTool::btStaticLinker:     { return "sl"; }
  case CBuildTool::btDynamicLinker:    { return "dl"; }
  case CBuildTool::btExecutableLinker: { return "el"; }
  case CBuildTool::btNativeLinker:     { return "nl"; }

  //case CBuildTool::btBuildManager,
 }
 return "bt";
}

CString CBuildTool::TypeName(void) const
{
 return TypeName(m_Type);
}

CString CBuildTool::MakeCommand(const CString& CommandTemplate, CConfiguration& Arguments)
{
 CString result = CommandTemplate;
 for (int i = 0; i < Arguments.GetCount(); i++)
 {
  CVariable& v = Arguments.Variable(i);
  result = FindReplaceStr(result,v.GetName(),v.GetString());
 }
 return result;
}

CString CBuildTool::MakeCommand(CConfiguration& Arguments)
{
 return MakeCommand(m_CommandTemplate,Arguments);
}

bool CBuildTool::ExpectedSourceExtension(const CString& FileExtension)
{
 CString file_ext = LowerCase(FileExtension);
 return (m_SourceExtensions.FindString(file_ext) >= 0);
}

void CBuildTool::Reset(const CPlatform::OS_Type OS)
{
 if (Supports(OS)) m_Platform = OS;
 else m_Platform = CPlatform::OS_Other;
}

bool CBuildTool::Supports(const CPlatform::OS_Type OS)
{
 return ((CPlatform::OS_Unix==OS)||
          (CPlatform::OS_Windows==OS)||
          (CPlatform::OS_Mac==OS));
}

void CBuildTool::Read(const TiXmlElement *Root, const CString& Name, CString& Value)
{
 TiXmlNode *_option = (TiXmlNode *)Root->FirstChild("option");
 while (0!=_option)
 {
  TiXmlElement* option = _option->ToElement();
  //if (strcmp(option->Value(),"option")!=0) break;
  if (0!=option)
  {
   char *value = 0;
   if ((value = (char *)option->Attribute(Name.GetCString())))
   {
    Value = value;
    break;
   }
  }
  _option = (TiXmlNode *)Root->IterateChildren(_option);
 } // option
}

void CBuildTool::Read(const TiXmlElement *Root, const CString& Name, bool& Value)
{
 CString value;
 Read(Root,Name,value);
 Value = StringToBoolean(value);
}

void CBuildTool::Read(const TiXmlElement *BuildToolRoot)
{
 char *value = 0; CString type_name;
 if ((value = (char *)BuildToolRoot->Attribute("type")))
 {
  type_name = value;
  if (CBuildTool::btOther==m_Type) m_Type = Type(type_name);
 }
 if ((value = (char *)BuildToolRoot->Attribute("alias")))
 {
  m_Alias = XMLFriendly(value);
 }
 /*{
	 CString l_TypeName;
	 Read(BuildToolRoot, "type", l_TypeName);
 }*/
	Read(BuildToolRoot, "description", m_Description);
	Read(BuildToolRoot, "program", m_Program);
	Read(BuildToolRoot, "make_variable", m_MakeVariable);
	Read(BuildToolRoot, "command_template", m_CommandTemplate);
	{
	 CString l_SourceExtensions; m_SourceExtensions.Clear();
	 Read(BuildToolRoot, "source_extensions", l_SourceExtensions);
  ParseStr(l_SourceExtensions,' ',m_SourceExtensions);
	}
	Read(BuildToolRoot, "target_extension", m_TargetExtension);
	Read(BuildToolRoot, "need_quoted_path", m_NeedQuotedPath);
	Read(BuildToolRoot, "need_full_path", m_NeedFullPath);
	Read(BuildToolRoot, "need_unix_path", m_NeedUnixPath);
}

void CBuildTool::Write(TiXmlElement *Root, const CString& Name, const CString& Value)
{
	TiXmlElement *option = new TiXmlElement("option");
	option->SetAttribute(Name.GetCString(),Value.GetCString());
	Root->LinkEndChild(option);
}

void CBuildTool::Write(TiXmlElement *Root, const CString& Name, const bool Value)
{
	TiXmlElement *option = new TiXmlElement("option");
	option->SetAttribute(Name.GetCString(),Value);
	Root->LinkEndChild(option);
}

void CBuildTool::Write(TiXmlElement *BuildToolRoot)
{
 BuildToolRoot->SetAttribute("type",XMLFriendly(TypeName()).GetCString());
 BuildToolRoot->SetAttribute("alias",m_Alias.GetCString());
	Write(BuildToolRoot, "description", m_Description);
	Write(BuildToolRoot, "program", m_Program);
	Write(BuildToolRoot, "make_variable", m_MakeVariable);
	Write(BuildToolRoot, "command_template", m_CommandTemplate);
	Write(BuildToolRoot, "source_extensions", m_SourceExtensions.Join(" "));
	Write(BuildToolRoot, "target_extension", m_TargetExtension);
	Write(BuildToolRoot, "need_quoted_path", m_NeedQuotedPath);
	Write(BuildToolRoot, "need_full_path", m_NeedFullPath);
	Write(BuildToolRoot, "need_unix_path", m_NeedUnixPath);
	//Write(BuildToolRoot, "", m_);
}

void CBuildTool::Show(void)
{
 std::cout<<"Type: "<<TypeName().GetCString()<<std::endl;
 std::cout<<"Alias: "<<m_Alias.GetCString()<<std::endl;
 std::cout<<"Description: "<<m_Description.GetCString()<<std::endl;
 std::cout<<"Program: "<<m_Program.GetCString()<<std::endl;
 std::cout<<"Make variable: "<<m_MakeVariable.GetCString()<<std::endl;
 std::cout<<"Command template: "<<m_CommandTemplate.GetCString()<<std::endl;
 std::cout<<"Source extensions: "<<m_SourceExtensions.Join(" ").GetCString()<<std::endl;
 std::cout<<"Target extension: "<<m_TargetExtension.GetCString()<<std::endl;
 std::cout<<"Need quoted path: "<<BooleanToYesNoString(m_NeedQuotedPath).GetCString()<<std::endl;
 std::cout<<"Need full path: "<<BooleanToYesNoString(m_NeedFullPath).GetCString()<<std::endl;
 std::cout<<"Need Unix-style path: "<<BooleanToYesNoString(m_NeedUnixPath).GetCString()<<std::endl;
}

//------------------------------------------------------------------------------

CPreprocessor::CPreprocessor(void)
{
 m_Type = CBuildTool::btPreprocessor;
 m_IncludeDirSwitch = "-I";
 m_DefineSwitch     = "-D";
}

CPreprocessor::CPreprocessor(const CPreprocessor& Preprocessor): CBuildTool(Preprocessor)
{
 m_IncludeDirSwitch = Preprocessor.m_IncludeDirSwitch;
 m_DefineSwitch     = Preprocessor.m_DefineSwitch;
}

CPreprocessor::~CPreprocessor(void)
{
 //
}

CPreprocessor *CPreprocessor::CreateInstance(void)
{
 return new CPreprocessor(*this);
}

void CPreprocessor::Read(const TiXmlElement *BuildToolRoot)
{
 CBuildTool::Read(BuildToolRoot);
	CBuildTool::Read(BuildToolRoot, "include_dir_switch", m_IncludeDirSwitch);
	CBuildTool::Read(BuildToolRoot, "define_switch", m_DefineSwitch);
	//Read(BuildToolRoot, "", m_);
}

void CPreprocessor::Write(TiXmlElement *BuildToolRoot)
{
 CBuildTool::Write(BuildToolRoot);
	CBuildTool::Write(BuildToolRoot, "include_dir_switch", m_IncludeDirSwitch);
	CBuildTool::Write(BuildToolRoot, "define_switch", m_DefineSwitch);
	//Write(BuildToolRoot, "", m_);
}

void CPreprocessor::Show(void)
{
 CBuildTool::Show();
 std::cout<<"Include directory switch: "<<m_IncludeDirSwitch.GetCString()<<std::endl;
 std::cout<<"Define switch: "<<m_DefineSwitch.GetCString()<<std::endl;
}

//------------------------------------------------------------------------------

CCompiler::CCompiler(void)
{
 m_Type = CBuildTool::btCompiler;
 m_IncludeDirSwitch = "-I";
 m_DefineSwitch     = "-D";
 m_NeedDependencies = true;
}

CCompiler::CCompiler(const CCompiler& Compiler): CBuildTool(Compiler)
{
 m_IncludeDirSwitch = Compiler.m_IncludeDirSwitch;
 m_DefineSwitch     = Compiler.m_DefineSwitch;
 m_NeedDependencies = Compiler.m_NeedDependencies;
}

CCompiler::~CCompiler(void)
{
 //
}

CCompiler *CCompiler::CreateInstance(void)
{
 return new CCompiler(*this);
}

void CCompiler::Read(const TiXmlElement *BuildToolRoot)
{
 CBuildTool::Read(BuildToolRoot);
	CBuildTool::Read(BuildToolRoot, "include_dir_switch", m_IncludeDirSwitch);
	CBuildTool::Read(BuildToolRoot, "define_switch", m_DefineSwitch);
	//Read(BuildToolRoot, "", m_);
}

void CCompiler::Write(TiXmlElement *BuildToolRoot)
{
 CBuildTool::Write(BuildToolRoot);
	CBuildTool::Write(BuildToolRoot, "include_dir_switch", m_IncludeDirSwitch);
	CBuildTool::Write(BuildToolRoot, "define_switch", m_DefineSwitch);
	//Write(BuildToolRoot, "", m_);
}

void CCompiler::Show(void)
{
 CBuildTool::Show();
 std::cout<<"Include directory switch: "<<m_IncludeDirSwitch.GetCString()<<std::endl;
 std::cout<<"Define switch: "<<m_DefineSwitch.GetCString()<<std::endl;
 std::cout<<"Need dependencies: "<<BooleanToYesNoString(m_NeedDependencies).GetCString()<<std::endl;
}

//------------------------------------------------------------------------------

CAssembler::CAssembler(void)
{
 m_Type = CBuildTool::btAssembler;
}

CAssembler::CAssembler(const CAssembler& Assembler): CCompiler(Assembler)
{
 //
}

CAssembler::~CAssembler(void)
{
 //
}

CAssembler *CAssembler::CreateInstance(void)
{
 return new CAssembler(*this);
}

//------------------------------------------------------------------------------

CResourceCompiler::CResourceCompiler(void)
{
 m_Type = CBuildTool::btResourceCompiler;
}

CResourceCompiler::CResourceCompiler(const CResourceCompiler& ResourceCompiler): CCompiler(ResourceCompiler)
{
 //
}

CResourceCompiler::~CResourceCompiler(void)
{
 //
}

CResourceCompiler *CResourceCompiler::CreateInstance(void)
{
 return new CResourceCompiler(*this);
}

//------------------------------------------------------------------------------

CLinker::CLinker(void)
{
 m_LibraryDirSwitch     = "-L";
 m_LinkLibrarySwitch    = "-l";
 m_ObjectExtension      = "o";
 m_LibraryPrefix        = "";
 m_LibraryExtension     = "";
 m_NeedLibraryPrefix    = false;
 m_NeedLibraryExtension = false;
 m_NeedFlatObjects      = false;
}

CLinker::CLinker(const CLinker& Linker): CBuildTool(Linker)
{
 m_LibraryDirSwitch     = Linker.m_LibraryDirSwitch;
 m_LinkLibrarySwitch    = Linker.m_LinkLibrarySwitch;
 m_ObjectExtension      = Linker.m_ObjectExtension;
 m_LibraryPrefix        = Linker.m_LibraryPrefix;
 m_LibraryExtension     = Linker.m_LibraryExtension;
 m_NeedLibraryPrefix    = Linker.m_NeedLibraryPrefix;
 m_NeedLibraryExtension = Linker.m_NeedLibraryExtension;
 m_NeedFlatObjects      = Linker.m_NeedFlatObjects;
}

CLinker::~CLinker(void)
{
 //
}

CLinker *CLinker::CreateInstance(void)
{
 return new CLinker(*this);
}

void CLinker::Read(const TiXmlElement *BuildToolRoot)
{
 CBuildTool::Read(BuildToolRoot);
 CBuildTool::Read(BuildToolRoot, "library_dir_switch", m_LibraryDirSwitch);
 CBuildTool::Read(BuildToolRoot, "link_library_switch", m_LinkLibrarySwitch);
 CBuildTool::Read(BuildToolRoot, "object_extension", m_ObjectExtension);
 CBuildTool::Read(BuildToolRoot, "library_prefix", m_LibraryPrefix);
 CBuildTool::Read(BuildToolRoot, "library_extension", m_LibraryExtension);
 CBuildTool::Read(BuildToolRoot, "need_library_prefix", m_NeedLibraryPrefix);
 CBuildTool::Read(BuildToolRoot, "need_library_extension", m_NeedLibraryExtension);
 CBuildTool::Read(BuildToolRoot, "need_flat_objects", m_NeedFlatObjects);
	//Read(BuildToolRoot, "", m_);
}

void CLinker::Write(TiXmlElement *BuildToolRoot)
{
 CBuildTool::Write(BuildToolRoot);
 CBuildTool::Write(BuildToolRoot, "library_dir_switch", m_LibraryDirSwitch);
 CBuildTool::Write(BuildToolRoot, "link_library_switch", m_LinkLibrarySwitch);
 CBuildTool::Write(BuildToolRoot, "object_extension", m_ObjectExtension);
 CBuildTool::Write(BuildToolRoot, "library_prefix", m_LibraryPrefix);
 CBuildTool::Write(BuildToolRoot, "library_extension", m_LibraryExtension);
 CBuildTool::Write(BuildToolRoot, "need_library_prefix", m_NeedLibraryPrefix);
 CBuildTool::Write(BuildToolRoot, "need_library_extension", m_NeedLibraryExtension);
 CBuildTool::Write(BuildToolRoot, "need_flat_objects", m_NeedFlatObjects);
	//Write(BuildToolRoot, "", m_);
}

void CLinker::Show(void)
{
 CBuildTool::Show();
 std::cout<<"Library directory switch: "<<m_LibraryDirSwitch.GetCString()<<std::endl;
 std::cout<<"Link library switch: "<<m_LinkLibrarySwitch.GetCString()<<std::endl;
 std::cout<<"Object extension: "<<m_ObjectExtension.GetCString()<<std::endl;
 std::cout<<"Library prefix: "<<m_LibraryPrefix.GetCString()<<std::endl;
 std::cout<<"Library extension: "<<m_LibraryExtension.GetCString()<<std::endl;
 std::cout<<"Need library prefix: "<<BooleanToYesNoString(m_NeedLibraryPrefix).GetCString()<<std::endl;
 std::cout<<"Need library extension: "<<BooleanToYesNoString(m_NeedLibraryExtension).GetCString()<<std::endl;
 std::cout<<"Need flat objects: "<<BooleanToYesNoString(m_NeedFlatObjects).GetCString()<<std::endl;
}

//------------------------------------------------------------------------------

CLibraryLinker::CLibraryLinker(void)
{
 m_LibraryPrefix = "lib";
}

CLibraryLinker::CLibraryLinker(const CLibraryLinker& LibraryLinker): CLinker(LibraryLinker)
{
 //
}

CLibraryLinker::~CLibraryLinker(void)
{
 //
}

CLibraryLinker *CLibraryLinker::CreateInstance(void)
{
 return new CLibraryLinker(*this);
}

CStaticLinker::CStaticLinker(void)
{
 m_Type = CBuildTool::btStaticLinker;
 m_LibraryExtension = "a";
}

CStaticLinker::CStaticLinker(const CStaticLinker& StaticLinker): CLibraryLinker(StaticLinker)
{
 //
}

CStaticLinker::~CStaticLinker(void)
{
 //
}

CStaticLinker *CStaticLinker::CreateInstance(void)
{
 return new CStaticLinker(*this);
}

CDynamicLinker::CDynamicLinker(void)
{
 m_Type = CBuildTool::btDynamicLinker;
 Reset(CPlatform::OS_Other);
}

void CDynamicLinker::Reset(const CPlatform::OS_Type OS)
{
 CLinker::Reset(OS);
 switch (OS)
 {
  default:
  case CPlatform::OS_Unix: { m_LibraryExtension = "so"; break; }
  case CPlatform::OS_Windows: { m_LibraryExtension = "dll"; break; }
  case CPlatform::OS_Mac: { m_LibraryExtension = "dylib"; break; }
 }
}

CDynamicLinker::CDynamicLinker(const CDynamicLinker& DynamicLinker): CLibraryLinker(DynamicLinker)
{
 //
}

CDynamicLinker::~CDynamicLinker(void)
{
 //
}

CDynamicLinker *CDynamicLinker::CreateInstance(void)
{
 return new CDynamicLinker(*this);
}

CExecutableLinker::CExecutableLinker(void)
{
 m_Type = CBuildTool::btExecutableLinker;
}

CExecutableLinker::CExecutableLinker(const CExecutableLinker& ExecutableLinker): CLinker(ExecutableLinker)
{
 //
}

CExecutableLinker::~CExecutableLinker(void)
{
 //
}

CExecutableLinker *CExecutableLinker::CreateInstance(void)
{
 return new CExecutableLinker(*this);
}

//------------------------------------------------------------------------------

CGNUCCompiler::CGNUCCompiler(void)
{
 Reset(CPlatform::OS_Other);
}

CIncludeSearchFilter *CGNUCCompiler::IncludeSearchFilter(void) const
{
 return (CIncludeSearchFilter *)&m_IncludeSearchFilter;
}

CGNUCCompiler *CGNUCCompiler::CreateInstance(void)
{
 return new CGNUCCompiler(*this);
}

void CGNUCCompiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "gnu_c_compiler";
 m_Description = "GNU C Compiler";
 m_MakeVariable = "CC";
 m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
 m_SourceExtensions.Clear()<<"c"<<"cc";
 m_TargetExtension = "o";
 if (CPlatform::OS_Windows==OS) m_Program = "gcc.exe";
 else m_Program = "gcc";
}

//------------------------------------------------------------------------------

CGNUCppCompiler::CGNUCppCompiler(void)
{
 Reset(CPlatform::OS_Other);
}

CIncludeSearchFilter *CGNUCppCompiler::IncludeSearchFilter(void) const
{
 return (CIncludeSearchFilter *)&m_IncludeSearchFilter;
}

CGNUCppCompiler *CGNUCppCompiler::CreateInstance(void)
{
 return new CGNUCppCompiler(*this);
}

void CGNUCppCompiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "gnu_cpp_compiler";
 m_Description = "GNU C++ Compiler";
 m_MakeVariable = "CXX";
 m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
 m_SourceExtensions.Clear()<<"cpp"<<"cxx";
 m_TargetExtension = "o";
 if (CPlatform::OS_Windows==OS) m_Program = "g++.exe";
 else m_Program = "g++";
}

//------------------------------------------------------------------------------

CGNUFortran77Compiler::CGNUFortran77Compiler(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUFortran77Compiler *CGNUFortran77Compiler::CreateInstance(void)
{
 return new CGNUFortran77Compiler(*this);
}

void CGNUFortran77Compiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "gnu_f77_compiler";
 m_Description = "GNU Fortran-77 Compiler";
 m_MakeVariable = "F77";
 m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
 m_SourceExtensions.Clear()<<"f"<<"f77";
 m_TargetExtension = "o";
 if (CPlatform::OS_Windows==OS) m_Program = "g77.exe";
 else m_Program = "g77";
}

//------------------------------------------------------------------------------

CGNUFortran90Compiler::CGNUFortran90Compiler(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUFortran90Compiler *CGNUFortran90Compiler::CreateInstance(void)
{
 return new CGNUFortran90Compiler(*this);
}

void CGNUFortran90Compiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "gnu_f90_compiler";
 m_Description = "GNU Fortran-90 Compiler";
 m_MakeVariable = "F90";
 m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
 m_SourceExtensions.Clear()<<"f90";
 m_TargetExtension = "o";
 if (CPlatform::OS_Windows==OS) m_Program = "gfortran.exe";
 else m_Program = "gfortran";
}

//------------------------------------------------------------------------------

CGNUWindowsResourceCompiler::CGNUWindowsResourceCompiler(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUWindowsResourceCompiler *CGNUWindowsResourceCompiler::CreateInstance(void)
{
 return new CGNUWindowsResourceCompiler(*this);
}

void CGNUWindowsResourceCompiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btResourceCompiler;
 m_Alias = "gnu_windres_compiler";
 m_Description = "GNU Windows Resource Compiler";
 m_MakeVariable = "WINDRES";
 m_CommandTemplate = "$rescomp -i $file -J rc -o $resource_output -O coff $includes";
 m_SourceExtensions.Clear()<<"rc"<<"res"<<"coff";
 m_TargetExtension = "o";
 if (CPlatform::OS_Windows==OS) m_Program = "windres.exe";
 else m_Program = "windres";
}

bool CGNUWindowsResourceCompiler::Supports(const CPlatform::OS_Type OS)
{
 return (CPlatform::OS_Windows==OS);
}

//------------------------------------------------------------------------------

CGNUStaticLinker::CGNUStaticLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUStaticLinker *CGNUStaticLinker::CreateInstance(void)
{
 return new CGNUStaticLinker(*this);
}

void CGNUStaticLinker::Reset(const CPlatform::OS_Type OS)
{
 CStaticLinker::Reset(OS);
 m_Type = btStaticLinker;
 m_Alias = "gnu_static_linker";
 m_Description = "GNU Static Library Linker";
 m_MakeVariable = "AR";
 m_CommandTemplate = "$lib_linker rcs $static_output $link_objects";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 m_TargetExtension = "a";
 if (CPlatform::OS_Windows==OS) m_Program = "ar.exe";
 else m_Program = "ar";
}

//------------------------------------------------------------------------------

CGNUDynamicLinker::CGNUDynamicLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUDynamicLinker *CGNUDynamicLinker::CreateInstance(void)
{
 return new CGNUDynamicLinker(*this);
}

void CGNUDynamicLinker::Reset(const CPlatform::OS_Type OS)
{
 CDynamicLinker::Reset(OS);
 m_Type = btDynamicLinker;
 m_Alias = "gnu_dynamic_linker";
 m_Description = "GNU Dynamic Library Linker";
 m_MakeVariable = "LD";
 m_CommandTemplate = "$linker -shared $link_options $libdirs $link_objects $libs -o $exe_output";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "g++.exe";
  m_TargetExtension = "dll";
 }
 else
 {
  m_Program = "g++";
  m_TargetExtension = "so";
 }
}

//------------------------------------------------------------------------------

CGNUExecutableLinker::CGNUExecutableLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CGNUExecutableLinker *CGNUExecutableLinker::CreateInstance(void)
{
 return new CGNUExecutableLinker(*this);
}

void CGNUExecutableLinker::Reset(const CPlatform::OS_Type OS)
{
 CExecutableLinker::Reset(OS);
 m_Type = btExecutableLinker;
 m_Alias = "gnu_executable_linker";
 m_Description = "GNU Executable Binary Linker";
 m_MakeVariable = "LD";
 m_CommandTemplate = "$linker $link_options $libdirs $link_objects $libs -o $exe_output";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "g++.exe";
  m_TargetExtension = "exe";
 }
 else
 {
  m_Program = "g++";
  //m_TargetExtension = "";
 }
}

//------------------------------------------------------------------------------

CIntelCCompiler::CIntelCCompiler(void)
{
 Reset(CPlatform::OS_Other);
}

CIncludeSearchFilter *CIntelCCompiler::IncludeSearchFilter(void) const
{
 return (CIncludeSearchFilter *)&m_IncludeSearchFilter;
}

CIntelCCompiler *CIntelCCompiler::CreateInstance(void)
{
 return new CIntelCCompiler(*this);
}

void CIntelCCompiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "intel_c_compiler";
 m_Description = "Intel C Compiler";
 m_MakeVariable = "CC";
 m_SourceExtensions.Clear()<<"c"<<"cc";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "icl.exe";
  m_CommandTemplate = "$compiler /nologo $options $includes /c $file /Fo$object";
  m_IncludeDirSwitch = "/I";
  m_DefineSwitch = "/D";
  m_TargetExtension = "obj";
  m_NeedDependencies = false;
 }
 else
 {
  m_Program = "icc";
  m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
  m_IncludeDirSwitch = "-I";
  m_DefineSwitch = "-D";
  m_TargetExtension = "o";
  m_NeedDependencies = true;
 }
}

//------------------------------------------------------------------------------

CIntelCppCompiler::CIntelCppCompiler(void)
{
 Reset(CPlatform::OS_Other);
}

CIncludeSearchFilter *CIntelCppCompiler::IncludeSearchFilter(void) const
{
 return (CIncludeSearchFilter *)&m_IncludeSearchFilter;
}

CIntelCppCompiler *CIntelCppCompiler::CreateInstance(void)
{
 return new CIntelCppCompiler(*this);
}

void CIntelCppCompiler::Reset(const CPlatform::OS_Type OS)
{
 CCompiler::Reset(OS);
 m_Type = btCompiler;
 m_Alias = "intel_cpp_compiler";
 m_Description = "Intel C++ Compiler";
 m_MakeVariable = "CXX";
 m_SourceExtensions.Clear()<<"cpp"<<"cxx";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "icl.exe";
  m_CommandTemplate = "$compiler /nologo $options $includes /c $file /Fo$object";
  m_IncludeDirSwitch = "/I";
  m_DefineSwitch = "/D";
  m_TargetExtension = "obj";
  m_NeedDependencies = false;
 }
 else
 {
  m_Program = "icpc";
  m_CommandTemplate = "$compiler $options $includes -c $file -o $object";
  m_IncludeDirSwitch = "-I";
  m_DefineSwitch = "-D";
  m_TargetExtension = "o";
  m_NeedDependencies = true;
 }
}

//------------------------------------------------------------------------------

CIntelStaticLinker::CIntelStaticLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CIntelStaticLinker *CIntelStaticLinker::CreateInstance(void)
{
 return new CIntelStaticLinker(*this);
}

void CIntelStaticLinker::Reset(const CPlatform::OS_Type OS)
{
 CStaticLinker::Reset(OS);
 m_Type = btStaticLinker;
 m_Alias = "intel_static_linker";
 m_Description = "Intel Static Library Linker";
 m_MakeVariable = "AR";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 m_TargetExtension = "a";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "xilink.exe";
  m_CommandTemplate = "$lib_linker /lib /nologo $libdirs /output:$static_output $libs $link_objects $link_resobjects $link_options";
  m_LibraryPrefix = "";
  m_LibraryExtension = "lib";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = true;
 }
 else
 {
  m_Program = "ar";
  m_CommandTemplate = "$lib_linker rcs $static_output $link_objects";
  m_LibraryPrefix = "lib";
  m_LibraryExtension = "a";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = false;
 }
}

//------------------------------------------------------------------------------

CIntelDynamicLinker::CIntelDynamicLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CIntelDynamicLinker *CIntelDynamicLinker::CreateInstance(void)
{
 return new CIntelDynamicLinker(*this);
}

void CIntelDynamicLinker::Reset(const CPlatform::OS_Type OS)
{
 CDynamicLinker::Reset(OS);
 m_Type = btDynamicLinker;
 m_Alias = "intel_dynamic_linker";
 m_Description = "Intel Dynamic Library Linker";
 m_MakeVariable = "LD";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "xilink.exe";
  m_CommandTemplate = "$linker /dll /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options";
  m_LibraryPrefix = "";
  m_LibraryExtension = "lib";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = true;
  m_TargetExtension = "dll";
 }
 else
 {
  m_Program = "icpc";
  m_CommandTemplate = "$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs";
  m_LibraryPrefix = "lib";
  m_LibraryExtension = "a";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = false;
  m_TargetExtension = "so";
 }
}

//------------------------------------------------------------------------------

CIntelExecutableLinker::CIntelExecutableLinker(void)
{
 Reset(CPlatform::OS_Other);
}

CIntelExecutableLinker *CIntelExecutableLinker::CreateInstance(void)
{
 return new CIntelExecutableLinker(*this);
}

void CIntelExecutableLinker::Reset(const CPlatform::OS_Type OS)
{
 CExecutableLinker::Reset(OS);
 m_Type = btExecutableLinker;
 m_Alias = "intel_executable_linker";
 m_Description = "Intel Executable Binary Linker";
 m_MakeVariable = "LD";
 m_CommandTemplate = "$linker $link_options $libdirs $link_objects $libs -o $exe_output";
 m_SourceExtensions.Clear()<<"o"<<"obj";
 if (CPlatform::OS_Windows==OS)
 {
  m_Program = "xilink.exe";
  m_CommandTemplate = "$linker /nologo /subsystem:windows $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options";
  m_LibraryPrefix = "";
  m_LibraryExtension = "lib";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = true;
  m_TargetExtension = "exe";
 }
 else
 {
  m_Program = "icpc";
  m_CommandTemplate = "$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs";
  m_LibraryPrefix = "";
  m_LibraryExtension = "lib";
  m_NeedLibraryPrefix = false;
  m_NeedLibraryExtension = true;
  m_TargetExtension = "";
 }
}

//------------------------------------------------------------------------------
