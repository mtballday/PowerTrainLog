// planDisplay.cpp

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

#include"userProfile.h"
#include"timePeriod.h"
#include"planDisplay.h"
#include"workouts.h"

enum
  {
	 ID_OK=1,
	 ID_WORKOUTS,
	 ID_CANCEL,
	 ID_WORKOUTSBUTTON,
	 ID_PRIORITY,
	 ID_INTENSITY
  };

BEGIN_EVENT_TABLE(PlanDisplayDialog, wxDialog)
  EVT_COMBOBOX(ID_WORKOUTS,    PlanDisplayDialog::OnSelectWorkout)
  EVT_TEXT(ID_WORKOUTS,      PlanDisplayDialog::OnSetWorkout)
  EVT_BUTTON(ID_OK,          PlanDisplayDialog::OnOK)
  EVT_BUTTON(ID_CANCEL,      PlanDisplayDialog::OnCancel)

  END_EVENT_TABLE()
  ;

PlanDisplayDialog::PlanDisplayDialog(wxWindow *parent, UserProfile *pf, BikeEntryData thedata)
  :wxDialog(parent, -1, "Day Entry",
			  wxDefaultPosition, wxDefaultSize,
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxRESIZE_BORDER|wxSYSTEM_MENU)
{
  wxString priorityChoices[]=
	 {
		_T("A"),
		_T("B"),
		_T("C"),
		_T("None")
	 };
  wxString intensityChoices[]=
	 {
		_T("None"),
		_T("Rec"),
		_T("Low"),
		_T("Med"),
		_T("High")
	 };

  theData=thedata;
  SetTitle(wxString::Format("Entry for %s - %s: Session #%d",
									 theData.theDate.FormatISODate().GetData(),
									 theData.theDate.GetWeekDayName(theData.theDate.GetWeekDay()
																			  ,wxDateTime::Name_Full).GetData(),
									 theData.session));
  theDate=theData.theDate;
  session=theData.session;
  // ------- Variable Init ------
  userProfile=pf;

  m_planDescription=theData.m_planDescription;
  m_planNotes=theData.m_planNotes;
  m_planMesoCycleWeek=theData.m_mesoCycleWeek;
  m_planPhase=theData.m_planPhase;
  m_planTime =theData.m_planTime;

  // --GUI Display--
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  phasesCtrl=new wxComboBox(this, -1, m_planPhase, wxDefaultPosition, wxDefaultSize);
  priorityCtrl=new wxChoice(this, ID_PRIORITY, 
									  wxDefaultPosition, wxDefaultSize, 4, priorityChoices);
  priorityCtrl->SetSelection(3-theData.m_priority);
  intensityCtrl=new wxChoice(this, ID_INTENSITY,
									  wxDefaultPosition, wxDefaultSize, 5, intensityChoices);
  intensityCtrl->SetSelection(theData.m_planIntensity);
  timeTextCtrl=new wxTextCtrl(this, -1, theData.m_planTime.HHMM(),
										wxDefaultPosition, wxDefaultSize);
  notesTextCtrl=new wxTextCtrl(this, -1, theData.m_planNotes, 
										 wxDefaultPosition, wxDefaultSize, 
										 wxTE_MULTILINE);
  workoutsButton=new wxButton(this, ID_WORKOUTSBUTTON, "Details...");
  workoutsCtrl=new wxComboBox(this, ID_WORKOUTS, wxEmptyString, 
										wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
  FillPhases();

  workoutsSelected=new wxTextCtrl(this, -1, m_planDescription, 
											 wxDefaultPosition, wxDefaultSize);

  wxBoxSizer *phasesLine=new wxBoxSizer(wxHORIZONTAL);
  phasesLine->Add(new wxStaticText(this, -1, "&Phase:"));
  phasesLine->Add(phasesCtrl, 0, wxGROW, 1);
  wxBoxSizer *workoutsLine=new wxBoxSizer(wxHORIZONTAL);
  workoutsLine->Add(new wxStaticText(this, -1, "&Workouts:"));
  workoutsLine->Add(workoutsCtrl, 0, wxGROW, 1);
  workoutsLine->Add(workoutsButton, 0, 0, 1);

  wxBoxSizer *priorityRow=new wxBoxSizer(wxHORIZONTAL);
  priorityRow->Add(new wxStaticText(this, -1, "P&riority:"));
  priorityRow->Add(priorityCtrl, 0, wxGROW, 1);
  wxBoxSizer *intensityRow=new wxBoxSizer(wxHORIZONTAL);
  intensityRow->Add(new wxStaticText(this, -1, "&Intensity:"));
  intensityRow->Add(intensityCtrl, 0, wxGROW, 1);
  wxBoxSizer *okcancelrow=new wxBoxSizer(wxHORIZONTAL);
  okcancelrow->Add(new wxButton(this, ID_CANCEL, "&Cancel"), 1, wxGROW, 1);
  okcancelrow->Add(new wxButton(this, ID_OK,     "&Ok"), 1, wxGROW, 1);
  wxBoxSizer *timeSizer=new wxBoxSizer(wxHORIZONTAL);
  timeSizer->Add(new wxStaticText(this, -1, "&Time"), 0, wxGROW, 1);
  timeSizer->Add(timeTextCtrl, 1, wxGROW, 1);

  mainSizer->Add(phasesLine);
  
  mainSizer->Add(workoutsSelected, 0, wxGROW, 1);
  mainSizer->Add(workoutsLine);
  mainSizer->Add(priorityRow);
  mainSizer->Add(intensityRow);
  mainSizer->Add(timeSizer, 0, wxGROW, 1);
  mainSizer->Add(notesTextCtrl, 1, wxGROW, 1);
  mainSizer->Add(okcancelrow, 0, wxGROW, 1);

  SetSizer(mainSizer);
  mainSizer->Fit(this);
}

void PlanDisplayDialog::FillPhases()
{
  //  phasesCtrl->Clear();
  PhaseList::Node *currentLine;
  currentLine=userProfile->workoutMenu.phaseList->GetFirst();
  while(currentLine!=NULL){
	 phasesCtrl->Append(currentLine->GetData()->m_phase);
	 currentLine=currentLine->GetNext();
  }

  WorkoutList::Node *workoutNode;
  workoutNode=userProfile->workoutMenu.workoutList->GetFirst();
  while(workoutNode!=NULL){
	 workoutsCtrl->Append(wxString::Format("%s: %s", 
														workoutNode->GetData()->m_abbreviation.GetData(),
														workoutNode->GetData()->m_title.GetData()));
	 workoutNode=workoutNode->GetNext();
  }
}

// `OnOK
void PlanDisplayDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_OK);
}

// `OnCancel
void PlanDisplayDialog::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

BikeEntryData PlanDisplayDialog::GetData()
{
  m_planPhase=phasesCtrl->GetValue();
  m_planNotes=notesTextCtrl->GetValue();
  m_planTime.SetByHHMM(timeTextCtrl->GetValue());

  theData.m_planNotes=m_planNotes;
  theData.m_planTime=m_planTime;
  theData.m_planPhase=m_planPhase;
  theData.m_planDescription=workoutsSelected->GetValue();
  theData.m_priority=3-priorityCtrl->GetSelection();
  theData.m_planIntensity=intensityCtrl->GetSelection();
  return(theData);
}

void PlanDisplayDialog::OnSelectWorkout(wxCommandEvent &WXUNUSED(event))
{
  ;
}

void PlanDisplayDialog::OnSetWorkout(wxCommandEvent &event)
{
  wxString selected;
  wxString old;
  wxString left, right;
  wxString tStr;

  WorkoutList::Node *workoutNode;

  int ii;
  int selindex;
  wxString description;

  selected=event.GetString();
  selindex=workoutsCtrl->GetSelection();

  workoutNode=userProfile->workoutMenu.workoutList->GetFirst();
  while(workoutNode!=NULL && selindex>0){
	 selindex--;
	 workoutNode=workoutNode->GetNext();
  }
  if(workoutNode!=NULL) description=selected.Append("-").Append(workoutNode->GetData()->m_description.GetData()); // assemble description
  
  ii=selected.Find(":");
  selected=","+selected.Left(ii)+",";

  old=","+workoutsSelected->GetValue()+",";

  ii=old.Find(selected);
  if(ii>-1){						  // delete instead of add
	 left=old.Left(ii);
	 if(left.Right(1)==",") left=left.Left(left.Length()-1);
	 
	 right=old.Mid(ii);
	 if(right.Left(1)==",") right=right.Mid(1); // prune any left commas

	 ii=right.Find(",");
	 if(ii>-1)right=right.Mid(ii); else right.Empty();
	 if(right.Left(1)==",") right=right.Mid(1);
	 
	 if(left.IsEmpty()==false && right.IsEmpty()==false){
		selected=left+","+right;
	 }else{
		selected=left+right;
	 }
	 tStr=notesTextCtrl->GetValue();	// remove the description if it was unchanged
	 if(tStr.Replace(description+"\n", "", false)==0){	// try to replace with and
																		// without lf
		tStr.Replace(description, "");
	 }
	 notesTextCtrl->SetValue(tStr);
  }else{								  // add
	 old=workoutsSelected->GetValue(); // don't want the ,, anymore
	 if(old.IsEmpty()==false){	  // not empty so add the new thing with a ,
		left=selected;
		selected=old+selected;
	 }else{							  // was empty anyway
		;								  // selected is already ok
	 }
	 if(notesTextCtrl->GetValue().IsEmpty()!=true){	// add the description to the
																	// notes field
		notesTextCtrl->SetValue(notesTextCtrl->GetValue()+"\n"+description);
	 }else{
		notesTextCtrl->SetValue(description);
	 }
  }

  while(selected.Left(1)==",") selected=selected.Mid(1);
  while(selected.Right(1)==",") selected=selected.Left(selected.Length()-1);
  workoutsSelected->SetValue(selected);
}
