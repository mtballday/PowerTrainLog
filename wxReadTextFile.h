#ifndef __READTEXTFILE__
#define __READTEXTFILE__

#include"wx/file.h"
#include<wx/wfstream.h>

class wxReadTextFile:public wxFileInputStream
{
private:
  bool ready;
  wxString filename;
  wxFileInputStream *fi;
  wxBufferedInputStream *inFileBuf;

public:
  wxReadTextFile();
  void Close();
  wxReadTextFile(wxString);
  void UnreadChar(char ch);
  char lastChar;
  char ReadChar();
  char Peek();
  bool ReadWord(wxString*);
  bool ReadLine(wxString*);
  bool ReadDouble(double*);
  bool ReadInt(int*);
  bool Ready();
  ~wxReadTextFile(); // simple way to open file
};
#endif
