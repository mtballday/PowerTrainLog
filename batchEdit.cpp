// batchEdit.cpp

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

#include"timePeriod.h"
#include"workouts.h"
#include"weekViewPanel.h"
#include"batchEdit.h"

enum
  {
	 ID_OK=1,
	 ID_CANCEL,
	 ID_WORKOUTS,
	 ID_WORKOUTSBUTTON,
	 ID_PRIORITY,
	 ID_STARTCALENDAR,
	 ID_ENDCALENDAR,
	 ID_DAYSTOENTERCTRL,
	 ID_MOVEUP,
	 ID_MOVEBACK,
	 ID_DELETE
  };

BEGIN_EVENT_TABLE(BatchEditDialog, wxDialog)
  //  EVT_COMBOBOX(ID_WORKOUTS,    BatchEditDialog::OnSelectWorkout)
  //  EVT_TEXT(ID_WORKOUTS,      BatchEditDialog::OnSetWorkout)
  EVT_BUTTON(ID_MOVEUP, BatchEditDialog::OnMoveUp)
  EVT_BUTTON(ID_MOVEBACK, BatchEditDialog::OnMoveBack)

  EVT_BUTTON(ID_OK,          BatchEditDialog::OnFill)
  EVT_BUTTON(ID_CANCEL,      BatchEditDialog::OnCancel)

  EVT_CALENDAR_SEL_CHANGED(ID_STARTCALENDAR, BatchEditDialog::AdjustEnd)
  EVT_CALENDAR_SEL_CHANGED(ID_ENDCALENDAR, BatchEditDialog::AdjustStart)

  END_EVENT_TABLE()
  ;

BatchEditDialog::BatchEditDialog(wxWindow *parent, UserProfile *pf)
  :wxDialog(parent, -1, "Batch Edit",
			  wxDefaultPosition, wxDefaultSize,
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
{
  // -- Var init
  userProfile=pf;
  m_parent=parent;
  weekViewParent=NULL;
  
  // -- Create the controls
  startDateCalendar=new wxCalendarCtrl(this,
													ID_STARTCALENDAR,
													wxDateTime::Now(), wxDefaultPosition, 
													wxDefaultSize,
													wxCAL_MONDAY_FIRST|wxCAL_SHOW_SURROUNDING_WEEKS);
  endDateCalendar=new wxCalendarCtrl(this,
												 ID_ENDCALENDAR,
												 wxDateTime::Now(), wxDefaultPosition, 
												 wxDefaultSize,
												 wxCAL_MONDAY_FIRST|wxCAL_SHOW_SURROUNDING_WEEKS);
  phasesCtrl=new wxComboBox(this, -1, m_planPhase, wxDefaultPosition, wxDefaultSize);

  m_daysToInsertCtrl=new wxTextCtrl(this, ID_DAYSTOENTERCTRL, "7");

  // -- Set up the sizers
  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *phaseSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *toFromRowSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *calendarRowSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *insertSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *okcancelrow=new wxBoxSizer(wxHORIZONTAL);

  phaseSizer->Add(new wxStaticText(this, -1, "&Phase"), 0, wxGROW, 1);
  phaseSizer->Add(phasesCtrl, 1, 0, 1);

  toFromRowSizer->Add(new wxStaticText(this, -1, "&From"), 1, 0, 1);
  toFromRowSizer->Add(new wxStaticText(this, -1, "&To"), 1, 0, 1);

  calendarRowSizer->Add(startDateCalendar, 0, wxGROW, 1);
  calendarRowSizer->Add(endDateCalendar, 0, wxGROW, 1);

  insertSizer->Add(new wxButton(this, ID_MOVEUP, "Move &Up"), 1, wxGROW, 1);
  insertSizer->Add(m_daysToInsertCtrl, 1, wxGROW, 1);
  insertSizer->Add(new wxButton(this, ID_MOVEBACK, "Move &Back"), 1, wxGROW, 1);

  okcancelrow->Add(new wxButton(this, ID_CANCEL, "&Close"), 1, wxGROW, 1);
  okcancelrow->Add(new wxButton(this, ID_OK,     "&Fill"), 1, wxGROW, 1);

  mainSizer->Add(phaseSizer, 0, wxGROW, 1);

  mainSizer->Add(toFromRowSizer, 0, wxGROW, 1);
  mainSizer->Add(calendarRowSizer, 0, wxGROW, 1);
  mainSizer->Add(insertSizer, 0, wxGROW, 1);
  mainSizer->Add(okcancelrow, 0, wxGROW, 1);
  SetSizer(mainSizer);
  mainSizer->Fit(this);
  FillPhases();
}

void BatchEditDialog::OnMoveBack(wxCommandEvent& WXUNUSED(event))
{
  long days;
  m_daysToInsertCtrl->GetValue().ToLong(&days);
  if(days>0){
	 MoveBack(days, startDateCalendar->GetDate(), endDateCalendar->GetDate());
  }
}

void BatchEditDialog::OnMoveUp(wxCommandEvent& WXUNUSED(event))
{
  long days;
  m_daysToInsertCtrl->GetValue().ToLong(&days);
  if(days>0){
	 MoveUp(days, startDateCalendar->GetDate(), endDateCalendar->GetDate());
  }
}

// `OnFill
void BatchEditDialog::OnFill(wxCommandEvent& WXUNUSED(event))
{
  wxDateTime currentDay;

  wxDateTime startDate=startDateCalendar->GetDate();
  wxDateTime endDate=endDateCalendar->GetDate();

  EntryList::Node *node1;
  BikeEntryData bd;

  node1=userProfile->mainList->GetFirst();
  currentDay=startDate;

  while(!currentDay.IsLaterThan(endDate)){
	 while(node1!=NULL && currentDay.IsLaterThan(node1->GetData()->theDate)){
		node1=node1->GetNext();
	 }
	 if(node1!=NULL && currentDay.IsSameDate(node1->GetData()->theDate)){
		// have an entry, and it is right
		node1->GetData()->m_planPhase=phasesCtrl->GetValue();
	 }else if(node1!=NULL){
		// have an entry, but it is for later, so right one doesn't exist
		node1=userProfile->mainList->Insert(node1, new BikeEntryData());
		node1->GetData()->theDate=currentDay;
		node1->GetData()->m_planPhase=phasesCtrl->GetValue();
		node1->GetData()->session=1;
	 }else{
		node1=userProfile->mainList->Append(new BikeEntryData());
		node1->GetData()->theDate=currentDay;
		node1->GetData()->m_planPhase=phasesCtrl->GetValue();
		node1->GetData()->session=1;
	 }
	 currentDay+=wxTimeSpan::Day()+wxTimeSpan::Hour();
	 currentDay.ResetTime();
  }
  startDate=endDate+wxTimeSpan::Day()+wxTimeSpan::Hour();
  startDate.ResetTime();
  endDate=startDate+wxTimeSpan::Days(6)+wxTimeSpan::Hour();
  endDate.ResetTime();
  
  startDateCalendar->SetDate(startDate);
  endDateCalendar->SetDate(endDate);

  if(weekViewParent!=NULL) weekViewParent->Repaint();		  // this doesn't work
}

// `OnCancel
void BatchEditDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

void BatchEditDialog::FillPhases()
{
  //  phasesCtrl->Clear();
  PhaseList::Node *currentLine;
  currentLine=userProfile->workoutMenu.phaseList->GetFirst();
  while(currentLine!=NULL){
	 phasesCtrl->Append(currentLine->GetData()->m_phase);
	 currentLine=currentLine->GetNext();
  }
}

void BatchEditDialog::AdjustEnd(wxCalendarEvent& WXUNUSED(event))
{
  wxDateTime newDay;
  wxDateTime newEnd;

  newDay=startDateCalendar->GetDate();
  newEnd=endDateCalendar->GetDate();
  if(newEnd-newDay>wxTimeSpan::Weeks(8) || newEnd<newDay){				  // if they choose a date in the future, move end
										  // date after that date
	 endDateCalendar->SetDate(newDay);
  }
}

void BatchEditDialog::AdjustStart(wxCalendarEvent& WXUNUSED(event))
{
  wxDateTime newDay;
  wxDateTime newEnd;

  newDay=startDateCalendar->GetDate();
  newEnd=endDateCalendar->GetDate();
  if(newEnd<newDay){				  // if they choose a date in the future, move end
										  // date after that date
	 startDateCalendar->SetDate(newEnd);
  }
}

void BatchEditDialog::MoveUp(int days, wxDateTime start, wxDateTime end)
{
  wxString astring;

  wxDateTime currentDaySource;
  wxDateTime currentDayTarget;

  wxDateTime startDate=start;
  wxDateTime endDate=end;

  EntryList::Node *node1;
  EntryList::Node *node2;
  BikeEntryData bd;

  currentDaySource=startDate;
  currentDaySource.ResetTime();
  currentDayTarget=startDate;
  currentDayTarget-=wxTimeSpan::Days(days);
  currentDayTarget.ResetTime();

  node1=userProfile->mainList->GetFirst(); // find pointers to the start source
														 // and start target

  node2=node1;
  while(!currentDaySource.IsLaterThan(endDate)){ // this is the main copy loop
	 while(node1!=NULL &&			  // find target start node
			 currentDayTarget.IsLaterThan(node1->GetData()->theDate)){ 
		node1=node1->GetNext();
	 }

	 // if target node exists, but it isn't the right day
	 if(node1!=NULL && !currentDayTarget.IsSameDate(node1->GetData()->theDate)){
		node1=userProfile->mainList->Insert(node1, new BikeEntryData());
		node1->GetData()->theDate=currentDayTarget;
		node1->GetData()->session=1;
	 }else if(node1==NULL){		  // didn't exist at all
		node1=userProfile->mainList->Append(new BikeEntryData());
		node1->GetData()->theDate=currentDayTarget;
		node1->GetData()->session=1;
	 }
		
	 while(node2!=NULL &&			  // find source start node
			 currentDaySource.IsLaterThan(node2->GetData()->theDate)){
		node2=node2->GetNext();
	 }
	 // source if a node exists, but it isn't the right day
	 if(node2!=NULL &&
		 !currentDaySource.IsSameDate(node2->GetData()->theDate)){
		node2=userProfile->mainList->Insert(node2, new BikeEntryData());
		node2->GetData()->theDate=currentDaySource;
		node2->GetData()->session=1;
	 }else if(node2==NULL){		  // didn't exist at all
		node2=userProfile->mainList->Append(new BikeEntryData());
		node2->GetData()->theDate=currentDaySource;
		node2->GetData()->session=1;
	 }
	 // we have the nodes, now copy the phase
	 node1->GetData()->m_planPhase=node2->GetData()->m_planPhase;

	 currentDaySource+=wxTimeSpan::Day()+wxTimeSpan::Hour();
	 currentDaySource.ResetTime();

	 currentDayTarget+=wxTimeSpan::Day()+wxTimeSpan::Hour();
	 currentDayTarget.ResetTime();
  }

  if(weekViewParent!=NULL) weekViewParent->Repaint();		  // this doesn't work
}


void BatchEditDialog::MoveBack( int days, wxDateTime start, wxDateTime end)
{
  wxDateTime currentDaySource;
  wxDateTime currentDayTarget;

  wxDateTime startDate=start;
  wxDateTime endDate=end;

  EntryList::Node *node1;
  EntryList::Node *node2;
  BikeEntryData bd;

  currentDaySource=endDate;
  currentDaySource.ResetTime();
  currentDayTarget=endDate;
  currentDayTarget+=wxTimeSpan::Days(days)+wxTimeSpan::Hour();
  currentDayTarget.ResetTime();

  node1=userProfile->mainList->GetLast(); // find pointers to the end source
														 // and end target

  node2=node1;
  while(!currentDaySource.IsEarlierThan(startDate)){ // this is the main copy loop
	 while(node1!=NULL &&			  // find target start node
			 (currentDayTarget.IsEarlierThan(node1->GetData()->theDate)
			  || (node1->GetPrevious()!=NULL 
					&& node1->GetPrevious()->GetData()->theDate.IsSameDate(currentDayTarget)))){ 
		node1=node1->GetPrevious();
	 }
	 // if target node exists, but it isn't the right day
	 if(node1!=NULL && !currentDayTarget.IsSameDate(node1->GetData()->theDate)){
		if(node1==userProfile->mainList->GetLast()){
		  node1=userProfile->mainList->Append(new BikeEntryData());
		  node1->GetData()->theDate=currentDayTarget;
		  node1->GetData()->session=1;
		  node2=node1;
		}else{
		  node1=userProfile->mainList->Insert(node1->GetNext(),
														  new BikeEntryData());
		  node1->GetData()->theDate=currentDayTarget;
		  node1->GetData()->session=1;
		}
	 }else if(node1==NULL){		  // didn't exist at all
		node1=userProfile->mainList->Insert(userProfile->mainList->GetFirst(),
														new BikeEntryData());
		node1->GetData()->theDate=currentDayTarget;
		node1->GetData()->session=1;
	 }
		
	 while(node2!=NULL &&			  // find source start node
			 (currentDaySource.IsEarlierThan(node2->GetData()->theDate)
			  || (node2->GetPrevious()!=NULL
					&& node2->GetPrevious()->GetData()->theDate.IsSameDate(currentDaySource)))){
		node2=node2->GetPrevious();
	 }
	 // source if a node exists, but it isn't the right day
	 if(node2!=NULL && !currentDaySource.IsSameDate(node2->GetData()->theDate)){
		if(node2==userProfile->mainList->GetLast()){
		  node2=userProfile->mainList->Append(new BikeEntryData());
		  node2->GetData()->theDate=currentDaySource;
		  node2->GetData()->session=1;
		}else{
		  node2=userProfile->mainList->Insert(node2->GetNext(),
														  new BikeEntryData());
		  node2->GetData()->theDate=currentDaySource;
		  node2->GetData()->session=1;
		}
	 }else if(node2==NULL){
		node2=userProfile->mainList->Insert(userProfile->mainList->GetFirst(),
														new BikeEntryData());
		node2->GetData()->theDate=currentDaySource;
		node2->GetData()->session=1;
	 }

	 // we have the nodes, now copy the phase
	 node1->GetData()->m_planPhase=node2->GetData()->m_planPhase;

	 currentDaySource-=wxTimeSpan::Day();
	 currentDaySource.ResetTime();

	 currentDayTarget-=wxTimeSpan::Day();
	 currentDayTarget.ResetTime();
  }

  if(weekViewParent!=NULL) weekViewParent->Repaint();		  // this doesn't work
}
