// singleWeekViewPanel.cpp
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
#include"singleWeekViewPanel.h"
#include"planDisplay.h"
#include"weekViewPanel.h"
#include"batchEdit.h"

BEGIN_EVENT_TABLE(SingleWeekViewPanel, wxPanel)
  EVT_PAINT(SingleWeekViewPanel::OnPaint)
  EVT_LEFT_UP(SingleWeekViewPanel::OnLClick)
  EVT_LEFT_DCLICK(SingleWeekViewPanel::OnDClick)
  EVT_CHAR(SingleWeekViewPanel::OnChar)

END_EVENT_TABLE()


SingleWeekViewPanel::SingleWeekViewPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
							  const wxSize size, UserProfile *up)
  : wxPanel(parent, id, pos, size)
{
  currentProfile=up;
  m_calStart=wxDateTime::Today();
  mousePick.x=-1;
  mousePick.y=-1;
}

void SingleWeekViewPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  DrawWeek(&dc);
}

void SingleWeekViewPanel::OnChar(wxKeyEvent& event)
{
  switch(event.GetKeyCode()){
  case 'b':
  case WXK_LEFT:				  // pan left
	 m_calStart-=(wxTimeSpan::Week()+wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case 'f':
  case WXK_RIGHT:
	 m_calStart+=(wxTimeSpan::Week()+wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case 'p':
  case WXK_UP:
	 m_calStart-=(wxTimeSpan::Day()-wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case 'n':
  case WXK_DOWN:
	 m_calStart+=(wxTimeSpan::Day()+wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case WXK_PAGEUP:
	 m_calStart-=(wxTimeSpan::Week()-wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case WXK_PAGEDOWN:
	 m_calStart+=(wxTimeSpan::Week()+wxTimeSpan::Hour());
	 m_calStart.ResetTime();
	 Refresh(true);
	 break;
  case 'r':
	 Refresh(true);
	 break;
  default:
	 event.Skip();
  }
}

void SingleWeekViewPanel::OnDClick(wxMouseEvent &event)
{
  int mouseX, mouseY;

  mouseX=event.GetX();
  mouseY=event.GetY();

  Refresh(true);
}

void SingleWeekViewPanel::OnLClick(wxMouseEvent &event)
{
  mousePick.x=event.GetX();
  mousePick.y=event.GetY();
  Refresh(false);
}

void SingleWeekViewPanel::DrawWeek(wxDC *dc)
{
  int dayNumber;
  int ii, jj, tWidth, tHeight;
  wxSize clientSize;
  wxRect clientRect;
  wxPoint orig;
  wxRect dayRect[7];
  wxRect distanceRect[3], intensityRect[3], recoveryRect;
  wxRect tRect;
  wxDateTime currentDate;
  wxString outtxt;
  wxString tText, t2Text;
  EntryList::Node *dataNode;
  int textWidth, textHeight, textX, textY;
  WorkoutList::Node *workoutNode;

  orig=GetClientAreaOrigin();	  // set up the clientRectangle that we paint in
  clientSize=GetClientSize();
  clientRect.SetX(orig.x);
  clientRect.SetY(orig.y);
  clientRect.SetHeight(clientSize.GetHeight());
  clientRect.SetWidth(clientSize.GetWidth());

  currentDate=m_calStart;	  // find the current entry

  dataNode=currentProfile->mainList->GetFirst(); // set node to first data
  for(dayNumber=0;dayNumber<7;dayNumber++){ // set the day rectangles
	 while(dataNode!=NULL			  // find the data
			 && dataNode->GetData()->theDate.IsEarlierThan(currentDate))
		dataNode=dataNode->GetNext();
	 if(dataNode!=NULL) currentData=dataNode->GetData(); // get the data for the
	 // day
	 else currentData=NULL;

	 if(currentData!=NULL
		 && !currentDate.IsSameDate(currentData->theDate))
		currentData=NULL;

	 dayRect[dayNumber].SetLeft(clientRect.GetLeft());
	 dayRect[dayNumber].SetRight(clientRect.GetRight());
	 dayRect[dayNumber].SetTop(dayNumber*
										(clientRect.GetBottom()-
										 clientRect.GetTop())/7);
	 dayRect[dayNumber].SetBottom((dayNumber+1)*
											(clientRect.GetBottom()-
											 clientRect.GetTop())/7);
	 dc->DrawRectangle(dayRect[dayNumber]); // draw main rectangle

	  // intensity and distance rectangles
	 tWidth=dayRect[dayNumber].GetWidth()/10;
	 tHeight=dayRect[dayNumber].GetHeight()/5;
	 dc->SetBrush(*wxLIGHT_GREY_BRUSH);
	 dc->DrawRectangle(dayRect[dayNumber].GetRight()-2*tWidth,
							 dayRect[dayNumber].GetTop(),
							 2*tWidth, tHeight);
	 dc->GetTextExtent("D", &textWidth, &textHeight);
	 dc->DrawText("D", (int)(dayRect[dayNumber].GetRight()-1.5*tWidth-textWidth/2),
					  dayRect[dayNumber].GetTop()+tHeight-textHeight);
	 dc->DrawText("I", (int)(dayRect[dayNumber].GetRight()-.5*tWidth-textWidth/2),
					  (int)(dayRect[dayNumber].GetTop()+tHeight-textHeight));
	 dc->DrawLine(dayRect[dayNumber].GetRight()-tWidth,
					  dayRect[dayNumber].GetTop()+tHeight-textHeight/2,
					  dayRect[dayNumber].GetRight()-tWidth,
					  dayRect[dayNumber].GetTop()+tHeight);

	 dc->SetBrush(*wxTRANSPARENT_BRUSH);
	 for(ii=0;ii<3;ii++){	
		distanceRect[ii].SetLeft(dayRect[dayNumber].GetRight()-2*tWidth);
		if(ii>0)
		  distanceRect[ii].SetTop(distanceRect[ii-1].GetBottom());
		else
		  distanceRect[ii].SetTop(dayRect[dayNumber].GetTop()+(ii+1)*tHeight);
		distanceRect[ii].SetRight(dayRect[dayNumber].GetRight()-tWidth);
		distanceRect[ii].SetHeight(tHeight);
		intensityRect[ii].SetLeft(distanceRect[ii].GetRight());
		intensityRect[ii].SetTop(distanceRect[ii].GetTop());
		intensityRect[ii].SetRight(dayRect[dayNumber].GetRight());
		intensityRect[ii].SetHeight(distanceRect[ii].GetHeight());
		dc->DrawRectangle(distanceRect[ii]);
		dc->DrawRectangle(intensityRect[ii]);
	 }
	 // set up recovery rect
	 recoveryRect.SetTop(distanceRect[2].GetBottom());
	 recoveryRect.SetLeft(distanceRect[2].GetLeft());
	 recoveryRect.SetRight(intensityRect[2].GetRight());
	 recoveryRect.SetBottom(dayRect[dayNumber].GetBottom());
	 dc->DrawRectangle(recoveryRect);

	 outtxt=currentDate.FormatISODate()
		+" ("+currentDate.GetWeekDayName(currentDate.GetWeekDay(),
													wxDateTime::Name_Abbr)+")"": ";
	 dc->DrawText(outtxt, dayRect[dayNumber].GetLeft(),
					  dayRect[dayNumber].GetTop());
	 dc->GetTextExtent(outtxt, &textWidth, &textHeight);
	 if(currentData!=NULL){		  // there is an entry for today
		textX=dayRect[dayNumber].GetLeft()+textWidth;
		textY=dayRect[dayNumber].GetTop();
		outtxt=currentData->m_planPhase+". ";
		dc->GetTextExtent(outtxt, &textWidth, &textHeight);
		dc->DrawText(outtxt, textX, textY);

		if(currentData->m_planTime.Seconds()!=0){
		  outtxt="Planned Time: "+currentData->m_planTime.HHMM();
		  dc->DrawText(outtxt, textX+textWidth, textY);
		}
		textX=dayRect[dayNumber].GetLeft()+2;
		textY+=textHeight;
		ii=0; jj=0;
		tText=currentData->m_planDescription;
		while(ii>=0){				  // write the workout titles
		  ii=tText.Find(",");
		  t2Text=tText.Left(ii);

		  workoutNode=currentProfile->workoutMenu.workoutList->GetFirst();
		  while(workoutNode!=NULL){ // found the matching workout
			 if(workoutNode->GetData()->m_abbreviation.GetData()==t2Text){
				if(jj!=0) outtxt=", "; else outtxt=" ";
				outtxt+=t2Text.Append(":").Append(workoutNode->GetData()->m_title.GetData());
				dc->DrawText(outtxt, textX, textY);
				dc->GetTextExtent(outtxt, &textWidth, &textHeight);
				tRect.SetTop(textY);
				tRect.SetLeft(textX);
				tRect.SetWidth(textWidth);
				tRect.SetHeight(textHeight);
				if(tRect.Contains(mousePick)==true){
				  wxMessageBox(workoutNode->GetData()->m_description,
									"Workout detail", wxOK);
				}

				workoutNode=NULL;
				jj++;
			 }else{
				workoutNode=workoutNode->GetNext();
			 }
		  }
		  if(jj==0){				  // workout didn't match any in the menu
			 // so just print it
				dc->DrawText(currentData->m_planDescription, textX, textY);
				dc->GetTextExtent(currentData->m_planDescription, 
										&textWidth, &textHeight);
		  }
		  textX+=textWidth;
		  tText=tText.Mid(ii+1);
		}

		textX=dayRect[dayNumber].GetLeft()+2;
		textY+=textHeight;
		outtxt=currentData->m_planNotes;
		dc->DrawText(outtxt, textX, textY);
		
		dc->SetBrush(*wxBLACK_BRUSH);	// fill in the duration box
		if(currentProfile->shortRideTime>=currentData->m_planTime
			&& currentData->m_planTime>0
			&& currentData->m_planIntensity!=1){ // short ride
		  dc->DrawRectangle(distanceRect[2]);
		}else if(currentProfile->longRideTime>=currentData->m_planTime
					&& currentData->m_planTime>0
					&& currentData->m_planIntensity!=1){ // med ride
		  dc->DrawRectangle(distanceRect[1]);
		}else if(currentData->m_planTime.Seconds()>0
					&& currentData->m_planIntensity!=1){
		  dc->DrawRectangle(distanceRect[0]);
		}
		// fill in the intensity box and time boxes
		if(currentData->m_planIntensity>1){
		  dc->DrawRectangle(intensityRect[4-currentData->m_planIntensity]);
		}else if(currentData->m_planIntensity==1){ // recovery workout
		  dc->DrawRectangle(recoveryRect);
		}

		dc->SetBrush(*wxTRANSPARENT_BRUSH);
	 }
	 currentDate+=wxTimeSpan::Day()+wxTimeSpan::Hour();
	 currentDate.ResetTime();
  }
  mousePick.x=-1;
  mousePick.y=-1;
}

