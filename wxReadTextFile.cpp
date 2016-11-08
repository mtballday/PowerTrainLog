#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include<wx/stream.h>
#include"wxReadTextFile.h"

wxReadTextFile::wxReadTextFile()	// default constructor
{
 inFileBuf=NULL; 
  ready=false;
}

void wxReadTextFile::Close()
{
  ready=false;
  if(inFileBuf!=NULL) delete(inFileBuf);
  if(fi!=NULL) delete(fi);
  inFileBuf=NULL;
  fi=NULL;
}

wxReadTextFile::wxReadTextFile(wxString fn) // simple way to open file
  //  : wxFileInputStream(fn)
{
 inFileBuf=NULL; 
  fi=new wxFileInputStream(fn);
  if(fi->IsOk()){
	 inFileBuf=new wxBufferedInputStream(*fi);
	 ready=true;
	 return;
  }else{
	 ready=false;
	 return;
  }
}

void wxReadTextFile::UnreadChar(char ch)
{
  if(ready==false) return;
  inFileBuf->Ungetch(ch);
}


char wxReadTextFile::Peek()
{
  char oneChar;

  if(ready==false) return(0);
  oneChar=inFileBuf->Peek();
  return(oneChar);
}

char wxReadTextFile::ReadChar()
{
  char oneChar;

  if(ready==false) return(0);
  inFileBuf->Read(&oneChar, 1);
  if(inFileBuf->Eof()) ready=false;
  lastChar=oneChar;
  return(oneChar);
}

bool wxReadTextFile::ReadWord(wxString *instr)
{
  bool complete=false;
  bool realCharFound=false;
  char oneChar;

  if(ready==false)return(false);
  wxString inStr;
  do{
	 inFileBuf->Read(&oneChar, 1);
	 if(inFileBuf->LastRead()>0){
		switch(oneChar){
		case '\n':
		case '\t':
		case ',':
		case ';':
		  complete=true;
		  break;
		case ' ':					  // ignore leading spaces
		  if(realCharFound==true)
			 complete=true;
		  break;
		case '\r':
		  break;						  // ignore these
		default:
		  realCharFound=true;
		  inStr.Append(oneChar);		  
		}
	 }
  }while(!complete && inFileBuf->Eof()==false);
  if(inFileBuf->Eof()==true)ready=false;
  *instr=inStr;
  if(inStr.Length()>0)return(true);
  return(false);
}

bool wxReadTextFile::ReadLine(wxString *instr)
{
  bool complete=false;
  char oneChar;

  wxString inStr;
  if(ready==false)return(false);
  do{
	 inFileBuf->Read(&oneChar, 1);
	 if(inFileBuf->LastRead()>0){
		switch(oneChar){
		case '\n':
		  complete=true;
		  break;
		case '\r':
		  break;
		default:
		  inStr.Append(oneChar);		  
		}
	 }
  }while(!complete && inFileBuf->Eof()==false);
  if(inFileBuf->Eof()==true)ready=false;
  *instr=inStr;
  if(ready==true)return(true);
  return(false);
}

bool wxReadTextFile::ReadDouble(double*retDouble){
  wxString aWord;
  double dTmp;
  bool   ok;
  int    i1;

  if(!ready) return(false);
  if(ReadWord(&aWord)==false)return(false);
  i1=aWord.Find("\"");
  if(i1>-1){						  // enclosed in quotes
	 aWord=aWord.Mid(i1+1);
	 i1=aWord.Find("\"");
	 if(i1>-1)aWord=aWord.Left(i1);
  }
  ok=aWord.ToDouble(&dTmp);
  if(ok)*retDouble=dTmp;
  return(ok);
}

bool wxReadTextFile::ReadInt(int *retInt){
  wxString aWord;
  long     lTmp;
  int      iTmp;
  bool     ok;
  int      i1;

  if(!ready) return(false);
  if(ReadWord(&aWord)==false)return(false);
  i1=aWord.Find("\"");
  if(i1>-1){						  // enclosed in quotes
	 aWord=aWord.Mid(i1+1);
	 i1=aWord.Find("\"");
	 if(i1>-1)aWord=aWord.Left(i1);
  }

  ok=aWord.ToLong(&lTmp);
  iTmp=lTmp;
  if(ok)*retInt=iTmp;
  return(ok);
}

bool wxReadTextFile::Ready()
{
  return(ready);
}

wxReadTextFile::~wxReadTextFile() // simple way to open file
{
 if(inFileBuf!=NULL) delete(inFileBuf);
  delete(fi);
}
