// dayEntrySelectionDialog.cpp
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

#include"dayEntrySelectionDialog.h"

enum
  {
	 ID_CANCEL=wxID_CANCEL,
	 ID_NEWBUTTON
  };

BEGIN_EVENT_TABLE(DayEntrySelectionDialog, wxDialog)
  EVT_BUTTON(-1,      DayEntrySelectionDialog::OnButton)
END_EVENT_TABLE();

DayEntrySelectionDialog::DayEntrySelectionDialog(wxWindow *parent,
																 int numEntries)
  :wxDialog(parent, -1, "Select Day Entry",
			  wxDefaultPosition, wxDefaultSize,
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
{
  int ii;

  m_sessions=-1;					  // defaults to new entry

  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  for(ii=1;ii<=numEntries;ii++)
	 mainSizer->Add(new wxButton(this, 
										  ii+ID_NEWBUTTON,
										  wxString::Format("Entry %d", ii)),
						 1, wxGROW, 1);
  mainSizer->Add(new wxButton(this, ID_NEWBUTTON, "New Entry"),
					  1, wxGROW, 1);
  mainSizer->Add(new wxButton(this, ID_CANCEL, "Cancel"),
					  1, wxGROW, 1);
  SetSizer(mainSizer);
  mainSizer->Fit(this);
  Centre(wxBOTH);
}

void DayEntrySelectionDialog::OnButton(wxCommandEvent &event)
{
  switch(event.GetId()){
  case ID_CANCEL:
	 EndModal(wxID_CANCEL);
	 break;
  case ID_NEWBUTTON:
	 m_sessions=-1;
	 EndModal(ID_NEWBUTTON);
	 break;
  default:
	 m_sessions=event.GetId()-ID_NEWBUTTON;
	 EndModal(m_sessions);
	 break;
  }
}

int DayEntrySelectionDialog::GetSessionSelected()
{
  return(m_sessions);
}
