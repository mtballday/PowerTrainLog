#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"wx/filename.h"
#include"profileEditor.h"
#include"bikeDayEntry.h"

enum
  {
	 ID_OK=wxID_OK,
	 ID_CANCEL=wxID_CANCEL,

	 INSERTBUTTON=1,
	 DELETEBUTTON,
	 LISTBOX,
	 CALENDAR,
	 ID_DATABASEBROWSE,
	 ID_WORKOUTSBROWSE,
	 ID_CTBROWSE,
	 ID_PPBROWSE,
	 ID_PTBROWSE,
	 ID_POLARBROWSE,
    ID_GARMINBROWSE,
	 ID_RESTHR,
	 ID_Z0,
	 ID_Z1,
	 ID_Z2,
	 ID_Z3,
	 ID_Z4,
	 ID_Z5,
	 ID_Z6,
	 ID_Z7,
	 ID_Z8,
	 ID_Z9,
	 ID_Z10,
	 ID_MAXHR
  }
;

BEGIN_EVENT_TABLE(ProfileEditor, wxDialog)
  EVT_BUTTON(wxID_OK, ProfileEditor::OnOk)
  EVT_BUTTON(wxID_CANCEL, ProfileEditor::OnCancel)
  EVT_BUTTON(ID_DATABASEBROWSE, ProfileEditor::OnDatabaseBrowse)
  EVT_BUTTON(ID_WORKOUTSBROWSE, ProfileEditor::OnBrowseWorkoutMenu)
  EVT_BUTTON(ID_PTBROWSE, ProfileEditor::OnBrowsePT)
  EVT_BUTTON(ID_CTBROWSE, ProfileEditor::OnBrowseCT)
  EVT_BUTTON(ID_PPBROWSE, ProfileEditor::OnBrowsePP)
  EVT_BUTTON(ID_POLARBROWSE, ProfileEditor::OnBrowsePolar)
  EVT_BUTTON(ID_GARMINBROWSE, ProfileEditor::OnBrowseGarmin)
  END_EVENT_TABLE()
  ;


ProfileEditor::ProfileEditor(wxWindow *parent, wxWindowID id, const wxPoint pos,
									  const wxSize size, UserProfile *up)
  : wxDialog(parent, id, "Select Day Entry",
	  pos, size,
	  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU/*|wxTHICK_FRAME*/)
{
  int ii;

  wxTextCtrl *ctl;              // temporary holder

  currentProfile=up;

  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *fnSizer=new wxFlexGridSizer(3,1,1);
  wxFlexGridSizer *thresholdSizer=new wxFlexGridSizer(6,2,1,1);
  wxFlexGridSizer *dirSizer =new wxFlexGridSizer(5,1,1);
  wxSizer *hrSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *buttonsSizer=new wxBoxSizer(wxHORIZONTAL);


  fnSizer->AddGrowableCol(1,1);
  dirSizer->AddGrowableCol(1,1);

  thresholdSizer->Add(new wxStaticText(this, -1, "&Name"), 0, wxGROW, 1);
  thresholdSizer->Add(m_nameCtrl=new wxTextCtrl(this, -1, up->name),
							 1, wxGROW, 1);
  

  thresholdSizer->Add(new wxStaticText(this, -1, "Threshold &Power"),
							 0, wxGROW, 1);
  thresholdSizer->Add(m_thresholdWattsCtrl=new wxTextCtrl(this, -1, wxString::Format("%ld", up->thresholdPower)),
							 1, wxGROW, 1);

  thresholdSizer->Add(new wxStaticText(this, -1, "&MTB Threshold Power"),
							 0, wxGROW, 1);
  thresholdSizer->Add(m_mtbThresholdWattsCtrl=new wxTextCtrl(this, -1, wxString::Format("%ld", up->mtbThresholdPower)),
							 1, wxGROW, 1);

  thresholdSizer->Add(new wxStaticText(this, -1, "Threshold &Hr"),
							 0, wxGROW, 1);
  thresholdSizer->Add(m_thresholdHrCtrl=new wxTextCtrl(this, -1, wxString::Format("%ld", up->thresholdHR)),
							 1, wxGROW, 1);

  thresholdSizer->Add(new wxStaticText(this, -1, "Hr &Cutoff"),
							 0, wxGROW, 1);
  thresholdSizer->Add(m_hrCutOffCtrl=new wxTextCtrl(this, -1, wxString::Format("%ld", up->hrCutoff)),
							 1, wxGROW, 1);


  thresholdSizer->Add(new wxStaticText(this, -1, "Acute TC"),
                      0, wxGROW, 1);
  thresholdSizer->Add(m_acuteTc=new wxTextCtrl(this, -1, wxString::Format("%d", up->acuteTc)),
                      1, wxGROW, 1);

  thresholdSizer->Add(new wxStaticText(this, -1, "Chronic TC"),
                      0, wxGROW, 1);
  thresholdSizer->Add(m_chronicTc=new wxTextCtrl(this, -1, wxString::Format("%d", up->chronicTc)),
                      1, wxGROW, 1);

  fnSizer->Add(new wxStaticText(this, -1, "&Database"), 0, wxGROW, 1);
  fnSizer->Add(m_databaseCtrl=new wxTextCtrl(this, -1, up->DatabaseFilename()), 1, wxGROW, 1);
  fnSizer->Add(new wxButton(this, ID_DATABASEBROWSE, "Browse"),
					 0, wxGROW, 1);
  
  fnSizer->Add(new wxStaticText(this, -1, "&Workout Menu"), 0, wxGROW, 1);
  fnSizer->Add(m_workoutsCtrl=new wxTextCtrl(this, -1, up->WorkoutFilename()), 1, wxGROW, 1);
  fnSizer->Add(new wxButton(this, ID_WORKOUTSBROWSE, "Browse"),
					 0, wxGROW, 1);
  
  dirSizer->Add(new wxStaticText(this, -1, "&Powerplan Dir"),
					 0, wxGROW, 1);
  dirSizer->Add(m_powerplanCtrl=new wxTextCtrl(this, -1, up->ppDefDir),
					 1, wxGROW, 1);
  m_powerplanCtrl->SetSizeHints(wxSize(100,20));
  dirSizer->Add(new wxButton(this, ID_PPBROWSE, "Browse"),
					 0, wxGROW, 1);
  dirSizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  dirSizer->Add(m_powerplanScaleCtrl=new wxTextCtrl(this, -1, ""), 0, wxGROW,1);
  m_powerplanScaleCtrl->Enable(false);
  
  //--  
  dirSizer->Add(new wxStaticText(this, -1, "Power&tap Dir"),
					 0, wxGROW, 1);
  dirSizer->Add(m_powertapCtrl=new wxTextCtrl(this, -1, up->ptDefDir),
					 1, wxGROW, 1);
  dirSizer->Add(new wxButton(this, ID_PTBROWSE, "Browse"),
					 0, wxGROW, 1);
  dirSizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  dirSizer->Add(m_powertapScaleCtrl=new wxTextCtrl(this, -1, wxString::Format("%3.3lf", up->ptCorrection)), 0, wxGROW,1);
  //--
  dirSizer->Add(new wxStaticText(this, -1, "Co&mputrainer Dir"),
					 0, wxGROW, 1);
  dirSizer->Add(m_computrainerCtrl=new wxTextCtrl(this, -1, up->ctDefDir),
					 1, wxGROW, 1);
  dirSizer->Add(new wxButton(this, ID_CTBROWSE, "Browse"),
					 0, wxGROW, 1);
  dirSizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  dirSizer->Add(m_computrainerScaleCtrl=new wxTextCtrl(this, -1, wxString::Format("%3.3lf", up->ctCorrection)), 0, wxGROW,1);
  //--
  dirSizer->Add(new wxStaticText(this, -1, "Po&lar Dir"),
					 0, wxGROW, 1);
  dirSizer->Add(m_polarCtrl=new wxTextCtrl(this, -1, up->polarDefDir),
					 1, wxGROW, 1);
  dirSizer->Add(new wxButton(this, ID_POLARBROWSE, "Browse"),
					 0, wxGROW, 1);
  dirSizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  dirSizer->Add(m_polarScaleCtrl=new wxTextCtrl(this, -1, wxString::Format("%3.3lf", up->polarCorrection)), 0, wxGROW,1);
  //--
  dirSizer->Add(new wxStaticText(this, -1, "&Garmin File"),
                0, wxGROW, 1);
  dirSizer->Add(m_garminCtrl=new wxTextCtrl(this, -1, up->garminDefFile),
					 1, wxGROW, 1);
  dirSizer->Add(new wxButton(this, ID_GARMINBROWSE, "Browse"),
					 0, wxGROW, 1);

  dirSizer->Add(new wxStaticText(this, -1, "Scale"), 0, wxGROW, 1);
  dirSizer->Add(ctl=new wxTextCtrl(this, -1, ""), 0, wxGROW,1);
  ctl->Enable(false);
  //--

  hrSizer->Add(new wxStaticText(this, -1, "HR Zones>> Rest:"), 0, wxGROW, 1);

  hrSizer->Add(m_restHrCtrl=new wxTextCtrl(this, ID_RESTHR, wxString::Format("%d", up->restHr)),
					1, wxGROW, 1);


  for(ii=1;ii<NUMZONES;ii++){
	 hrSizer->Add(new wxStaticText(this, -1, wxString::Format("Z%d:", ii)), 0, wxGROW, 1);
	 hrSizer->Add(m_hrzones[ii]=new wxTextCtrl(this, ID_Z0+ii, wxString::Format("%d", up->zones[ii])),
					  1, wxGROW, 1);
  }
  hrSizer->Add(new wxStaticText(this, -1, "Max:"), 0, wxGROW, 1);
  hrSizer->Add(m_maxHrCtrl=new wxTextCtrl(this, ID_MAXHR, wxString::Format("%d", up->hrlimit)),
					1, wxGROW, 1);
  //--
  buttonsSizer->Add(new wxButton(this, wxID_CANCEL, "&Cancel"),
						  1, wxGROW, 1);
  buttonsSizer->Add(new wxButton(this, wxID_OK, "&Ok"),
						  1, wxGROW, 1);

  mainSizer->Add(thresholdSizer, 0, wxGROW, 1);
  mainSizer->Add(fnSizer,0,wxGROW,1);
  mainSizer->Add(dirSizer, 0, wxGROW, 1);
  mainSizer->Add(hrSizer,0, wxGROW, 1);
  mainSizer->Add(buttonsSizer, 0, wxGROW, 1);
					 
  SetSizer(mainSizer);
  Fit();
  Center(wxBOTH);
}

void ProfileEditor::OnDatabaseBrowse(wxCommandEvent &WXUNUSED(event))
{
  wxFileName fn(m_databaseCtrl->GetValue());

  wxFileDialog fd(this, "Choose a database file", fn.GetPath(wxPATH_GET_VOLUME)/*default dir*/,
					fn.GetName(), /*default file*/
					"Powerplan Database Files (*.pdt)|*.pdt",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_OK){
	 m_databaseCtrl->SetValue(fd.GetPath());
  }
}

void ProfileEditor::OnBrowseWorkoutMenu(wxCommandEvent &WXUNUSED(event))
{
  wxFileName fn(m_workoutsCtrl->GetValue());

  wxFileDialog fd(this, "Choose a Workout Menu File", fn.GetPath(wxPATH_GET_VOLUME)/*default dir*/,
					fn.GetName(), /*default file*/
					"Powerplan Workout Menu (*.dat)|*.dat",
					wxFD_OPEN);
  if(fd.ShowModal()==wxID_OK){
	 m_workoutsCtrl->SetValue(fd.GetPath());
  }
}

void ProfileEditor::OnBrowsePP(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog wd(this, "Choose Powerplan Directory",
					  m_powerplanCtrl->GetValue(),
					  wxDD_NEW_DIR_BUTTON);
  if(wd.ShowModal()==wxID_OK){
	 m_powerplanCtrl->SetValue(wd.GetPath());
  }
}

void ProfileEditor::OnBrowsePT(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog wd(this, "Choose Powertap Directory",
					  m_powertapCtrl->GetValue(),
					  wxDD_NEW_DIR_BUTTON);
  if(wd.ShowModal()==wxID_OK){
	 m_powertapCtrl->SetValue(wd.GetPath());
  }
}
void ProfileEditor::OnBrowseCT(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog wd(this, "Choose Computrainer Directory",
					  m_computrainerCtrl->GetValue(),
					  wxDD_NEW_DIR_BUTTON);
  if(wd.ShowModal()==wxID_OK){
	 m_computrainerCtrl->SetValue(wd.GetPath());
  }
}

void ProfileEditor::OnBrowsePolar(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog wd(this, "Choose Polar Directory",
					  m_polarCtrl->GetValue(),
					  wxDD_NEW_DIR_BUTTON);
  if(wd.ShowModal()==wxID_OK){
	 m_polarCtrl->SetValue(wd.GetPath());
  }
}

void ProfileEditor::OnBrowseGarmin(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog wd(this, "Choose Garmin File", "", /* last is the dir*/
					  m_garminCtrl->GetValue(),
					  "Garmin History files (*.hst)|*.hst|All files (*.*)|*.*");
  if(wd.ShowModal()==wxID_OK){
	 m_garminCtrl->SetValue(wd.GetPath());
  }
}

void ProfileEditor::OnOk(wxCommandEvent &event)
{
  wxString text;
  long tLong;
  int ii;
  
  text=m_nameCtrl->GetValue();
  if(text.IsEmpty()){
	 wxMessageBox("Notice", "Must enter a name", wxOK);
	 return;
  }else{
	 currentProfile->name=text;
  }

  if(m_databaseCtrl->GetValue().IsEmpty()==true){ // didn't enter a database
																  // name
	 currentProfile->SetDatabaseFilename(currentProfile->name);
	 m_databaseCtrl->SetValue(currentProfile->DatabaseFilename());
  }else{
	 currentProfile->SetDatabaseFilename(m_databaseCtrl->GetValue());
  }

  currentProfile->SetWorkoutsFilename(m_workoutsCtrl->GetValue());

  text=m_thresholdWattsCtrl->GetValue();
  text.ToLong(&currentProfile->thresholdPower);
  
  text=m_thresholdHrCtrl->GetValue();
  text.ToLong(&currentProfile->thresholdHR);

  text=m_hrCutOffCtrl->GetValue();
  text.ToLong(&currentProfile->hrCutoff);

  text=m_mtbThresholdWattsCtrl->GetValue();
  text.ToLong(&currentProfile->mtbThresholdPower);

  currentProfile->ppDefDir=m_powerplanCtrl->GetValue();
  currentProfile->ptDefDir=m_powertapCtrl->GetValue();
  currentProfile->ctDefDir=m_computrainerCtrl->GetValue();
  currentProfile->polarDefDir=m_polarCtrl->GetValue();
  currentProfile->garminDefFile=m_garminCtrl->GetValue();

  for(ii=1;ii<NUMZONES;ii++){
	 m_hrzones[ii]->GetValue().ToLong(&tLong);
	 currentProfile->zones[ii]=tLong;
  }
  m_restHrCtrl->GetValue().ToLong(&tLong);
  currentProfile->restHr=tLong;

  m_maxHrCtrl->GetValue().ToLong(&tLong);
  currentProfile->hrlimit=tLong;

  m_acuteTc->GetValue().ToLong(&currentProfile->acuteTc);
  m_chronicTc->GetValue().ToLong(&currentProfile->chronicTc);

  event.Skip();
}

void ProfileEditor::OnCancel(wxCommandEvent &event)
{
  event.Skip();
}

