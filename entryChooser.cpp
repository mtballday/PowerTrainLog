#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"entryChooser.h"
#include"bikeDayEntry.h"

enum
  {
	 INSERTBUTTON=1,
	 DELETEBUTTON,
	 LISTBOX,
	 CALENDAR
  }
;

BEGIN_EVENT_TABLE(EntryChooser, wxPanel)
  EVT_BUTTON(INSERTBUTTON, EntryChooser::OnInsert)
  EVT_BUTTON(DELETEBUTTON, EntryChooser::OnDelete)

  EVT_LISTBOX_DCLICK(LISTBOX, EntryChooser::OnDoubleclick)

  END_EVENT_TABLE()
  ;


EntryChooser::EntryChooser(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxPanel(parent, id, pos, size)
{
  currentProfile=up;

  wxBoxSizer *mainSizer=new wxBoxSizer(wxHORIZONTAL);
  theList=new wxListBox(this, LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
  //  theList->Append("2003-05-11 SUN ");
  mainSizer->Add(theList, 1, wxGROW, 1);

  // calendar
  wxSizer *rightCol=new wxBoxSizer(wxVERTICAL);
  calendar=new wxCalendarCtrl(this, CALENDAR, wxDateTime::Now(), wxDefaultPosition, 
										wxDefaultSize, wxCAL_MONDAY_FIRST|wxCAL_SHOW_SURROUNDING_WEEKS);
  rightCol->Add(calendar, 0, wxGROW, 1);

  // Buttons
  wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
  wxButton *insertButton=new wxButton(this, INSERTBUTTON, "&Insert");
  wxButton *deleteButton=new wxButton(this, DELETEBUTTON, "Delete");
  buttonSizer->Add(deleteButton, 1, wxGROW, 1);
  buttonSizer->Add(insertButton, 2, wxGROW, 1);
  rightCol->Add(buttonSizer, 1, wxGROW, 1);
  mainSizer->Add(rightCol, 1, wxGROW, 1);
  SetSizer(mainSizer);
}

void EntryChooser::OnInsert(wxCommandEvent& WXUNUSED(event))
{
  wxDateTime    theDate;
  BikeEntryData *theData=new BikeEntryData();
  EntryList::Node *node1;
  int session;
  int index;

  theDate=calendar->GetDate();
  theData->theDate=theDate;
  node1=currentProfile->mainList->GetFirst();
  session=1;
  while(node1!=NULL && node1->GetData()->theDate<=theDate){
	 if(node1->GetData()->theDate==theDate) session++;
	 node1=node1->GetNext();
  }
  if(node1!=NULL)index=currentProfile->mainList->IndexOf(node1->GetData());
  else index=-1;
  theData->session=session;
  BikeEntryDialog *be=new BikeEntryDialog(this, currentProfile, *theData);
  if(be->ShowModal()==wxID_OK){
	 *theData=be->GetData();		  // get the data
	 if(index==-1 || node1==NULL) currentProfile->mainList->Append(theData);
		else currentProfile->mainList->Insert(index, theData);
  }else{
	 delete(theData);
  }
  delete(be);
  FillList();
}

void EntryChooser::OnDelete(wxCommandEvent& WXUNUSED(event))
{
  ;
}

bool EntryChooser::FillList()
{
  int thisWeek;
  EntryList::Node *node1;
  BikeEntryData *dat;
  wxString outtxt;
  int session;
  int ii;

  double totalTime;
  double weekTotalTime=0;

  theList->Clear();
  node1=currentProfile->mainList->GetFirst();
  if(node1!=NULL){
	 dat=node1->GetData();
	 thisWeek=dat->theDate.GetWeekOfYear(wxDateTime::Monday_First);
  }
  while(node1!=NULL){			  // loop through the entries
	 if(node1->GetPrevious()!=NULL && node1->GetPrevious()->GetData()->theDate==dat->theDate){
		session++;
	 }else{
		session=1;
	 }
	 totalTime=0;
	 for(ii=0;ii<NUMZONES;ii++){
		totalTime+=dat->fileZoneTimes[ii].Seconds();
		totalTime+=dat->manualZoneTimes[ii].Seconds();
	 }
	 TimePeriod tp(totalTime);
	 weekTotalTime+=totalTime;	  // track week total time
	 outtxt=wxString::Format("%s #%d: %lf %s", dat->theDate.FormatISODate().GetData(), session,
									 dat->weight, tp.HHMMSSHH().GetData());
	 for(ii=0;ii<NUMZONES;ii++){
		totalTime=dat->fileZoneTimes[ii].Seconds()+dat->manualZoneTimes[ii].Seconds();
		TimePeriod tp1(totalTime);
		outtxt+=wxString::Format(" %d)%s", ii+1, tp1.HHMMSSHH().GetData());
	 }
	 node1=node1->GetNext();
	 if(node1!=NULL)dat=node1->GetData();
	 if(node1==NULL || dat->theDate.GetWeekOfYear(wxDateTime::Monday_First)!=thisWeek){	// week is totaled
		if(node1!=NULL) thisWeek=dat->theDate.GetWeekOfYear(wxDateTime::Monday_First);
		TimePeriod tp2(weekTotalTime);
		outtxt+=wxString::Format(" Week: %s", tp2.HHMMSSHH().GetData());
		weekTotalTime=0;
	 }

	 theList->Append(outtxt);
  }
  return(true);
}

void EntryChooser::OnDoubleclick(wxCommandEvent& WXUNUSED(event))
{
  wxDateTime    theDate;
  BikeEntryData *theData;
  EntryList::Node *node1;
  int session;

  int ii;

  node1=currentProfile->mainList->GetFirst();
  session=1;
  ii=theList->GetSelection();
  for(;ii>0;ii--){
	 node1=node1->GetNext();
  }
  theData=node1->GetData();
  BikeEntryDialog *be=new BikeEntryDialog(this, currentProfile, *theData);
  if(be->ShowModal()==wxID_OK){
	 *theData=be->GetData();		  // get the data
  }
  delete(be);
  FillList();
}
