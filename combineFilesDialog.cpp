// combineFilesDialog.cpp
// Class to show a window that selects entry

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include<wx/file.h>

#include"combineFilesDialog.h"
#include"powerfile.h"

enum
  {
	 ID_CANCEL=wxID_CANCEL,
	 ID_NEWBUTTON,
	 ID_COMBINEFILES
  };

BEGIN_EVENT_TABLE(CombineFilesDialog, wxDialog)
  EVT_BUTTON(-1,      CombineFilesDialog::OnButton)
END_EVENT_TABLE();

CombineFilesDialog::CombineFilesDialog(wxWindow *parent, UserProfile *up)
  :wxDialog(parent, -1, "Select Day Entry",
			  wxDefaultPosition, wxDefaultSize,
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU/*|wxTHICK_FRAME*/)
{
  m_userProfile=up;
  m_fileCount=0;

  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  mainSizer->Add(m_filesListbox=new wxListBox(this, -1, 
															 wxDefaultPosition,
															 wxDefaultSize,
															 0, NULL,wxLB_SINGLE),
					  1, wxGROW, 1);
  mainSizer->Add(new wxButton(this, ID_NEWBUTTON, "Add Files..."),
					  0, wxGROW, 1);
  mainSizer->Add(new wxButton(this, ID_COMBINEFILES, "Combine files"),
					  0, wxGROW, 1);
  mainSizer->Add(new wxButton(this, ID_CANCEL, "Cancel"),
					  0, wxGROW, 1);
  mainSizer->Add(m_statusLine=new wxStaticText(this, -1, ""));
  SetSizer(mainSizer);
  mainSizer->Fit(this);
  Centre(wxBOTH);
}

void CombineFilesDialog::OnButton(wxCommandEvent &event)
{
  switch(event.GetId()){
  case ID_CANCEL:
	 EndModal(wxID_CANCEL);
	 break;
  case ID_NEWBUTTON:
	 AddFile();
	 break;
  case ID_COMBINEFILES:
	 CombineFiles();
	 break;
  default:
	 break;
  }
}

void CombineFilesDialog::AddFile()
{
  wxString filePath;
  int count;
  int ii;

  wxFileDialog fd(this, "Select File", m_userProfile->ppDefDir,
						"", "Powerplan files (*.csv)|*.csv|All Files (*.*)|*.*",
						wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
  if(fd.ShowModal()==wxID_OK){
	 wxArrayString paths;

	 fd.GetPaths(paths);
	 //	 dialog.GetFilenames(filenames);

	 wxString msg, s;
	 
	 count=paths.GetCount()-1;
	 for(ii=count;ii>=0;ii--){
		m_fileCount=m_filesListbox->GetCount();
		m_filesListbox->InsertItems(1, &paths[ii], m_fileCount);
	 }
  }
}

void CombineFilesDialog::CombineFiles()
{
  long ii;
  long timeoffset;
  long t1;
  long linesCounted;

  double meters1, metersOffset;

  wxString filename;
  Powerfile *file1;
  Powerfile *mainFile;

  wxFileDialog fd(this, "Enter new filename", m_userProfile->ppDefDir,
						 "", "Powerplan files (*.csv)|*.csv|All Files (*.*)|*.*",
						 wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR);
  if(fd.ShowModal()==wxID_CANCEL){
	 return;							  // user canceled
  }

  timeoffset=0;
  t1=0;

  metersOffset=meters1=0;

  m_fileCount=m_filesListbox->GetCount();
  mainFile=new Powerfile();	  // this will be the new file

  for(ii=0;ii<m_fileCount;ii++){
	 linesCounted=0;
	 file1=new Powerfile();
	 filename=m_filesListbox->GetString(ii);
	 file1->Load(filename);
	 if(ii==0){
		mainFile->startTime=file1->startTime;
	 }
	 file1->Firstline();
	 file1->FirstInterval();
	 while(file1->OnAInterval()==true){						  // copy the intervals
		mainFile->AppendInterval(file1->CurrentInterval());
		mainFile->SetIntervalStart(mainFile->IntervalStartSeconds()+timeoffset);
		mainFile->SetIntervalEnd(mainFile->IntervalEndSeconds()+timeoffset);
		file1->NextInterval();
	 }
	 while(file1->OnALine()==true){
		mainFile->Appendline(file1->Currentline());
		mainFile->SetDistanceMeters(file1->CurrentDistance(UNITS_METERS)+metersOffset);
		mainFile->SetSeconds(file1->Seconds()+timeoffset);
		t1=mainFile->Seconds();
		meters1=mainFile->CurrentDistance(UNITS_METERS);
		file1->Nextline();
		linesCounted++;
		if(linesCounted%1000==0){
		  m_statusLine->SetLabel(wxString::Format("Lines: %ld", linesCounted));
		  Update();
		}
	 }
	 timeoffset=t1;
	 metersOffset=meters1;
	 delete(file1);
  }
  mainFile->Save(fd.GetPath());
  EndModal(true);
}
