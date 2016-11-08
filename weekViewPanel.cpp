// weekViewPanel.cpp
// Shows the week views

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
#include"weekViewPanel.h"
#include"planDisplay.h"
#include"batchEdit.h"

BEGIN_EVENT_TABLE(WeekViewPanel, wxPanel)
  EVT_PAINT(WeekViewPanel::OnPaint)
  EVT_LEFT_UP(WeekViewPanel::OnLClick)
  EVT_LEFT_DCLICK(WeekViewPanel::OnDClick)
  EVT_CHAR(WeekViewPanel::OnChar)

END_EVENT_TABLE()


WeekViewPanel::WeekViewPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
							  const wxSize size, UserProfile *up)
  : wxPanel(parent, id, pos, size)
{
  currentData=NULL;
  currentProfile=up;
  m_currentDate=wxDateTime::Today();
  m_calStart=   wxDateTime::Today(); // move calstart so today is at the bottom
  m_calStart-=wxTimeSpan::Weeks(8);
  m_calEnd=wxDateTime::Today();
  m_parentFrame=NULL;
}

void WeekViewPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  DrawWeekCalendar(&dc);
}

void WeekViewPanel::OnChar(wxKeyEvent& event)
{
  switch(event.GetKeyCode()){
  case 'b':
  case WXK_LEFT:				  // pan left
	 m_currentDate-=(wxTimeSpan::Week()+wxTimeSpan::Hour()); m_currentDate.ResetTime();
	 if(m_currentDate<m_calStart)
		m_calStart-=wxTimeSpan::Weeks(WVP_WEEKS/WVP_COLUMNS);
	 Refresh(true);
	 break;
  case 'f':
  case WXK_RIGHT:
	 m_currentDate+=(wxTimeSpan::Week()+wxTimeSpan::Hour()); m_currentDate.ResetTime();
	 if(m_currentDate>=m_calEnd) m_calStart+=wxTimeSpan::Weeks(WVP_WEEKS/WVP_COLUMNS);
	 Refresh(true);
	 break;
  case 'p':
  case WXK_UP:
	 m_currentDate-=wxTimeSpan::Week();
	 if(m_currentDate<m_calStart) m_calStart=m_currentDate;
	 Refresh(true);
	 break;
  case 'n':
  case WXK_DOWN:
	 m_currentDate+=wxTimeSpan::Week();
	 if(m_currentDate>=m_calEnd) m_calStart+=wxTimeSpan::Week();
	 Refresh(true);
	 break;
  case WXK_PAGEUP:
	 m_currentDate-=wxTimeSpan::Weeks(WVP_WEEKS);
	 m_calStart-=wxTimeSpan::Weeks(WVP_WEEKS);
	 Refresh(true);
	 break;
  case WXK_PAGEDOWN:
	 m_currentDate+=wxTimeSpan::Weeks(WVP_WEEKS);
	 m_calStart+=wxTimeSpan::Weeks(WVP_WEEKS);
	 Refresh(true);
	 break;
  case 'r':
	 Refresh(true);
	 break;
  default:
	 event.Skip();
  }
}

void WeekViewPanel::OnLClick(wxMouseEvent &event)
{
  wxString text;
  int mouseX, mouseY;
  int ii, jj;

  wxRect dayRect;
  wxDateTime entryDate;
  wxDateTime pickedDate;

  mouseX=event.GetX();
  mouseY=event.GetY();

  entryDate=m_calStart;
  entryDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
  for(ii=0;ii<WVP_WEEKS;ii++){	  // find what they hit
	 dayRect=m_pickWeeks[ii];
	 dayRect.SetTop(dayRect.GetTop()+m_weekHeight);
	 for(jj=0;jj<7;jj++){		  // these are days within the week
		dayRect.SetX(m_pickWeeks[ii].GetLeft()+jj*m_pickWeeks[ii].GetWidth()/7);
		dayRect.SetWidth(m_pickWeeks[ii].GetWidth()/7);
		if(dayRect.Contains(mouseX, mouseY)){	// found the day
		  pickedDate=entryDate;
		  text=entryDate.FormatISODate()+":"
			 +entryDate.GetWeekDayName(entryDate.GetWeekDay(),
												wxDateTime::Name_Abbr);
		  if(m_parentFrame!=NULL) m_parentFrame->SetStatusText(text);
		  m_currentDate=entryDate;
		  m_currentDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
		  Refresh(true);
		}
		entryDate+=wxTimeSpan::Day()+wxTimeSpan::Hour();
		entryDate.ResetTime();
	 }	 
  }
}

void WeekViewPanel::OnDClick(wxMouseEvent &event)
{
  wxString text;
  int mouseX, mouseY;
  int ii, jj;

  wxRect dayRect;
  wxDateTime entryDate;
  wxDateTime pickedDate;

  mouseX=event.GetX();
  mouseY=event.GetY();

  EntryList::Node *entryNode;
  BikeEntryData *theEntry;

  entryDate=m_calStart;
  entryDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
  for(ii=0;ii<WVP_WEEKS;ii++){	  // find what they hit
	 dayRect=m_pickWeeks[ii];
	 dayRect.SetTop(dayRect.GetTop()+m_weekHeight);
	 for(jj=0;jj<7;jj++){		  // these are days within the week
		dayRect.SetX(m_pickWeeks[ii].GetLeft()+jj*m_pickWeeks[ii].GetWidth()/7);
		dayRect.SetWidth(m_pickWeeks[ii].GetWidth()/7);
		if(dayRect.Contains(mouseX, mouseY)){	// found the day
		  pickedDate=entryDate;
		  text=entryDate.FormatISODate();
		}
		entryDate+=wxTimeSpan::Day()+wxTimeSpan::Hour();
		entryDate.ResetTime();
	 }	 
  }

  entryNode=currentProfile->mainList->GetFirst();
  while(entryNode!=NULL && entryNode->GetData()->theDate<pickedDate)
	 entryNode=entryNode->GetNext();
  if(entryNode == NULL ||
	  pickedDate.IsSameDate(entryNode->GetData()->theDate)==false){
	 // entry doesn't exist
	 InsertNewEntry(pickedDate);
	 return;
  }else{
	 // the entry is already there so we are OK
	 theEntry=entryNode->GetData();
	 PlanDisplayDialog *pd=new PlanDisplayDialog(this, currentProfile, *theEntry);
	 if(pd->ShowModal()==wxID_OK){
		*theEntry=pd->GetData();
	 }
	 delete(pd);
  }
  Refresh(true);
}

void WeekViewPanel::InsertNewEntry(wxDateTime theDate)
{
  BikeEntryData *theData=new BikeEntryData();
  EntryList::Node *node1;
  int index, session;


  theData->theDate=theDate;
  node1=currentProfile->mainList->GetFirst();
  session=0;
  while(node1!=NULL && node1->GetData()->theDate<=theDate){
	 if(node1->GetData()->theDate.IsSameDate(theDate)) session++;
	 node1=node1->GetNext();
  }
  if(node1!=NULL)index=currentProfile->mainList->IndexOf(node1->GetData());
  else index=-1;
  theData->session=session;

  PlanDisplayDialog *be=new PlanDisplayDialog(this, currentProfile, *theData);
  if(be->ShowModal()==wxID_OK){
	 *theData=be->GetData();		  // get the data
	 theData->session=session+1;
	 if(index==-1 || node1==NULL) currentProfile->mainList->Append(theData);
	 else currentProfile->mainList->Insert(index, theData);
	 delete(theData);				  // delete this?
  }else{
	 delete(theData);
  }
  delete(be);
  Refresh(true);
}


void WeekViewPanel::DrawWeekCalendar(wxDC *dc)
{
  int xx, ii, jj, kk, index;
  int currentWeekIndex;
  int rows;
  wxPen *dottedGreyPen;
  wxPen *fatBlackPen;
  wxPen *fatCyanPen;
  wxBrush *phaseBrush;

  wxSize clientSize;
  wxRect clientRect;
  wxRect legendRect;
  wxRect dayRect;
  wxRect timeRect, intensityRect;
  TimePeriod weekTime;

  int textHeight, textWidth;

  wxPoint orig;
  wxPen thePen;
  wxString outtxt;

  EntryList::Node *dataNode;
  PhaseList::Node *phaseNode;
  WorkoutPhase *phaseData;

  int maxPriority;
  wxString races;
  wxString workouts;
  wxString phase;
  wxColour phaseColour;
  wxBrush  priorityBrush;

  dataNode=currentProfile->mainList->GetFirst(); // set node to first data
  if(dataNode==NULL) currentData=NULL;

  dottedGreyPen=new wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT); // create pen for drawing days
  fatBlackPen=new wxPen(*wxBLACK, 3, wxPENSTYLE_SOLID);
  fatCyanPen=new wxPen(*wxCYAN, 3, wxPENSTYLE_SOLID);
  dc->SetPen(*dottedGreyPen);

  m_calStart.ResetTime();
  m_currentDate.ResetTime();

  wxDateTime tDate=m_calStart;
  wxDateTime tToday=m_currentDate;

  tToday.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);

  orig=GetClientAreaOrigin();

  clientSize=GetClientSize();
  clientRect.SetX(orig.x);		  // set up the clientRectangle that we paint in
  clientRect.SetY(orig.y);
  clientRect.SetHeight(clientSize.GetHeight());
  clientRect.SetWidth(clientSize.GetWidth());

  rows=WVP_WEEKS/WVP_COLUMNS;
  for(ii=0;ii<rows;ii++){  // rows
	 for(jj=0;jj<WVP_COLUMNS;jj++){ // columns
		index=ii+jj*rows;
		if(ii==0){			  	  // set the top to the top of the client window
		  m_pickWeeks[index].SetTop(clientRect.GetTop());
		}else{						  // set the top to the bottom of the box above it
		  m_pickWeeks[index].SetTop(m_pickWeeks[ii-1+jj*rows].GetBottom());
		} // set the height
		m_pickWeeks[index].SetHeight(clientRect.GetHeight()/rows);
		if(jj==0){					  // set the left to the left of client
		  m_pickWeeks[index].SetLeft(clientRect.GetLeft());
		}else{						  // set the left to the right of the one next to it
		  m_pickWeeks[index].SetLeft(m_pickWeeks[ii+(jj-1)*rows].GetRight());
		} // set the width
		m_pickWeeks[index].SetWidth(clientRect.GetWidth()/WVP_COLUMNS);
	 }
  }

  dc->GetTextExtent("TEXT", &textWidth, &textHeight);	// find out how tall text is
  m_weekHeight=2*textHeight;

  tDate.SetToWeekDayInSameWeek(wxDateTime::Mon,	// start at beginning of week
										 wxDateTime::Monday_First);
																			
  for(ii=0;ii<WVP_WEEKS;ii++){
	 while(dataNode!=NULL && dataNode->GetData()->theDate.IsEarlierThan(m_calStart)) dataNode=dataNode->GetNext();
	 if(tDate.IsSameDate(tToday)){ // highlight selected week with red text
		dc->SetTextForeground(wxColour(255,0,0));
		currentWeekIndex=ii;
	 }else{
		dc->SetTextForeground(wxColour(0,0,0));
	 }
	 outtxt=wxString::Format("%d-%s-%d (%d)", tDate.GetYear(),
									 tDate.GetMonthName(tDate.GetMonth(), 
															  wxDateTime::Name_Abbr).GetData(),
									 tDate.GetDay(),
									 tDate.GetWeekOfYear());
	 m_calEnd=tDate;

	 maxPriority=0;
	 races.Empty();
	 workouts.Empty();
	 phase.Empty();

	 dc->SetPen(*dottedGreyPen); // -- draw in days --
	 weekTime.Set(0);
	 for(kk=0;kk<7;kk++){
		while(dataNode!=NULL && dataNode->GetData()->theDate.IsEarlierThan(tDate))
		  dataNode=dataNode->GetNext();
		if(dataNode!=NULL){
		  currentData=dataNode->GetData(); // advance one day in data
		  if(currentData->theDate.IsSameDate(tDate)){
			 phase=currentData->m_planPhase; // get phase
			 if(currentData->m_priority>maxPriority){
				maxPriority=currentData->m_priority;
				races+=currentData->m_planDescription+" ";
			 }else{
				workouts+=currentData->m_planDescription+",";
			 }
		  }else{
			 races.Empty();
			 workouts.Empty();
			 phase.Empty();
		  }
		}
		//		phaseColour=wxColour(255,255,255); // set the phase colour
		phaseColour=*wxWHITE;
		if(phase.IsEmpty()==false){			  // 
		  phaseNode=currentProfile->workoutMenu.phaseList->GetFirst();
		  while(phaseNode!=NULL){		  // check each phase and see if it is in the
			 // phases string
			 phaseData=phaseNode->GetData();
			 if(phaseData->m_phase==phase){
				phaseColour=phaseData->m_phaseColour;
			 }
			 phaseNode=phaseNode->GetNext();
		  }
		}

		dc->DrawLine(m_pickWeeks[ii].GetLeft(),	// draw H-line below number
						 m_pickWeeks[ii].GetTop()+m_weekHeight,
						 m_pickWeeks[ii].GetRight(),
						 m_pickWeeks[ii].GetTop()+m_weekHeight);
		xx=m_pickWeeks[ii].GetLeft()+kk*m_pickWeeks[ii].GetWidth()/7; // draw day v-lines
		dayRect.SetLeft(xx);
		dayRect.SetRight(m_pickWeeks[ii].GetLeft()+(kk+1)*m_pickWeeks[ii].GetWidth()/7);
		dayRect.SetTop(m_pickWeeks[ii].GetTop()+m_weekHeight);
		dayRect.SetBottom(m_pickWeeks[ii].GetBottom());
		dc->DrawLine(dayRect.GetLeft(), dayRect.GetTop(), // actually draw day v-line
						 xx, m_pickWeeks[ii].GetBottom());
		phaseBrush=new wxBrush(phaseColour, wxBRUSHSTYLE_SOLID);
		dc->SetBrush(*phaseBrush);
		dc->DrawRectangle(dayRect); // draw the day with the brush
		delete(phaseBrush);

		if(currentData!=NULL){
		  timeRect=dayRect;			  // draw the box showing duration
		  timeRect.SetWidth(dayRect.GetWidth()/2);
		  timeRect.SetHeight(dayRect.GetHeight()/4);
		  intensityRect=timeRect;

		  if(currentProfile->shortRideTime>=currentData->m_planTime){ // short ride
			 timeRect.SetTop(dayRect.GetBottom()-2*timeRect.GetHeight());
		  }else if(currentProfile->longRideTime>=currentData->m_planTime){	// medium ride
			 timeRect.SetTop(dayRect.GetBottom()-3*timeRect.GetHeight());
		  }else{						  // long ride
			 timeRect.SetTop(dayRect.GetBottom()-4*timeRect.GetHeight());
		  }
		  dc->SetBrush(*wxBLACK_BRUSH);
		  if(currentData->m_planTime.Seconds()>0
			  && currentData->m_planIntensity!=1){
			 dc->DrawRectangle(timeRect);
		  }
										  // draw the box showing intensity
		  if(currentData->m_planIntensity>1){
			 intensityRect.SetLeft(timeRect.GetRight());
			 intensityRect.SetRight(dayRect.GetRight());
			 intensityRect.SetTop(dayRect.GetBottom()
										 -intensityRect.GetHeight()
										 *currentData->m_planIntensity);
			 intensityRect.SetHeight(timeRect.GetHeight());
		  }else{
			 intensityRect.SetLeft(dayRect.GetLeft());
			 intensityRect.SetRight(dayRect.GetRight());
			 intensityRect.SetTop(dayRect.GetBottom()-dayRect.GetHeight()/4);
			 intensityRect.SetHeight(dayRect.GetHeight()/4);
		  }
		  if(currentData->m_planIntensity>0){
			 dc->DrawRectangle(intensityRect);
		  }
		  if(currentData->m_planTime>0)
			 weekTime=weekTime+currentData->m_planTime;
		}
		tDate+=wxTimeSpan::Day()+wxTimeSpan::Hour();
		tDate.ResetTime();
	 }
	 dc->SetBrush(*wxTRANSPARENT_BRUSH);

	 while(workouts.Left(1)==",") workouts=workouts.Mid(1);
	 while(workouts.Right(1)==",") workouts.RemoveLast();	// don't want a comma
																			// on the end
	 while(workouts.Find(",,")>=0) workouts.Replace(",,", ",", TRUE);

	 if(maxPriority==0) priorityBrush=*wxTRANSPARENT_BRUSH;
	 if(maxPriority==1) priorityBrush=*wxLIGHT_GREY_BRUSH;
	 if(maxPriority==2) priorityBrush=*wxGREEN_BRUSH;
	 if(maxPriority==3) priorityBrush=*wxRED_BRUSH;

	 dc->SetBrush(priorityBrush);
	 dayRect=m_pickWeeks[ii];	 
	 dayRect.SetHeight(m_weekHeight);
	 dc->DrawRectangle(dayRect); //  draw the priority color box
	 dc->SetBrush(*wxTRANSPARENT_BRUSH);
	 dc->SetPen(*fatBlackPen);	
	 dc->DrawRectangle(m_pickWeeks[ii]); // draw fat line around the week
	 outtxt+=" ";
	 dc->GetTextExtent(outtxt, &textWidth, &textHeight);
	 dc->SetPen(*wxBLACK_PEN);
	 dc->DrawText(outtxt,   m_pickWeeks[ii].GetLeft(), m_pickWeeks[ii].GetTop());
	 if(phaseColour==priorityBrush.GetColour()){	// invert writing if filling the box
		phaseColour.Set(~phaseColour.Red(), ~phaseColour.Green(), ~phaseColour.Blue());
	 }
	 dc->SetTextForeground(phaseColour);
	 outtxt=phase+"-"+weekTime.HHMM();
	 dc->DrawText(outtxt, 
					  m_pickWeeks[ii].GetLeft()+textWidth,
					  m_pickWeeks[ii].GetTop());
	 dc->DrawText(races,
					  m_pickWeeks[ii].GetLeft(),
					  m_pickWeeks[ii].GetTop()+textHeight);
	 if(phaseColour!=priorityBrush.GetColour()){	// if we haven't already done,
																// invert the pen color
		phaseColour.Set(~phaseColour.Red(), ~phaseColour.Green(), ~phaseColour.Blue());
		dc->SetTextForeground(phaseColour);
	 }
	 dc->DrawText(workouts, m_pickWeeks[ii].GetLeft(), m_pickWeeks[ii].GetTop()+2*textHeight);
  }
  dc->SetPen(*fatCyanPen);
  dc->DrawRectangle(m_pickWeeks[currentWeekIndex]);

  delete(dottedGreyPen);
  delete(fatBlackPen);
  delete(fatCyanPen);
}

void WeekViewPanel::Repaint()
{
  Refresh(true);
}
