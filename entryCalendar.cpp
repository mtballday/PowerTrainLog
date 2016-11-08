#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"wx/datetime.h"
#include"planDisplay.h"
#include"entryCalendar.h"
#include"bikeDayEntry.h"
#include"dayEntrySelectionDialog.h"

enum
  {
	 INSERTBUTTON=1,
	 DELETEBUTTON,
	 LISTBOX,
	 CALENDAR
  }
;

BEGIN_EVENT_TABLE(EntryCalendar, wxPanel)
  EVT_CHAR(EntryCalendar::OnChar)
  EVT_PAINT(EntryCalendar::OnPaint)
  EVT_BUTTON(INSERTBUTTON, EntryCalendar::OnInsert)
  EVT_BUTTON(DELETEBUTTON, EntryCalendar::OnDelete)
  EVT_LEFT_UP(EntryCalendar::OnMouseLeftUp)
  EVT_LEFT_DCLICK(EntryCalendar::OnDoubleclick)
  //  EVT_LISTBOX_DCLICK(LISTBOX, EntryCalendar::OnDoubleclick)

  END_EVENT_TABLE()
  ;


EntryCalendar::EntryCalendar(wxWindow *parent, wxWindowID id, const wxPoint pos,
									const wxSize size, UserProfile *up)
  : wxPanel(parent, id, pos, size, wxWANTS_CHARS)
{
  int ii;
  currentProfile=up;
  currentDate=wxDateTime::Today();
  calStart=   wxDateTime::Today(); // move calstart so today is at the bottom
  calEnd=     wxDateTime::Today();
  while(calStart.GetWeekDay()!= wxDateTime::Mon) calStart-=wxTimeSpan::Day();
  for(ii=0;ii<4;ii++)
	 calStart-=wxTimeSpan::Week();
  m_reportFormat=CAL_ZONETIMES;

}


void EntryCalendar::OnChar(wxKeyEvent& event)
{
  wxString outtxt;
  switch(event.GetKeyCode()){
  case 'p':
  case WXK_LEFT:				  // pan left
	 currentDate-=(wxTimeSpan::Day()-wxTimeSpan::Hours(2));
	 currentDate.ResetTime();
	 if(currentDate<calStart){
		calStart-=(wxTimeSpan::Week()-wxTimeSpan::Hours(2));
		calStart.ResetTime();
	 }
	 Refresh(true);
	 break;
  case 'n':
  case WXK_RIGHT:
	 currentDate+=wxTimeSpan::Day()+wxTimeSpan::Hours(2);
	 currentDate.ResetTime();
	 if(currentDate>=calEnd){
		calStart+=wxTimeSpan::Week()+wxTimeSpan::Hours(2);
		calStart.ResetTime();
	 }
	 Refresh(true);
	 outtxt=wxString::Format("%s %s.%d", currentDate.FormatISODate().GetData(),
									 currentDate.FormatISOTime().GetData(),
									 currentDate.GetMillisecond());
	 //	 wxMessageBox(outtxt, "HI", wxOK);
	 break;
  case WXK_UP:
	 currentDate-=(wxTimeSpan::Week()-wxTimeSpan::Hour());
	 currentDate.ResetTime();
	 if(currentDate<calStart){
		calStart-=(wxTimeSpan::Week()-wxTimeSpan::Hour());
		calStart.ResetTime();
		calStart.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
	 }
	 Refresh(true);
	 break;
  case WXK_DOWN:
	 currentDate+=wxTimeSpan::Week()+wxTimeSpan::Hour();
	 currentDate.ResetTime();
	 if(currentDate>=calEnd){
		calStart+=wxTimeSpan::Week()+wxTimeSpan::Hour();
		calStart.ResetTime();
		calStart.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
	 }
	 Refresh(true);
	 break;
  case WXK_PAGEUP:
	 currentDate-=wxTimeSpan::Weeks(4);
	 currentDate.ResetTime();
	 while(currentDate<calStart) calStart-=wxTimeSpan::Week();
	 calStart.ResetTime();
	 Refresh(true);
	 break;
  case WXK_PAGEDOWN:
	 currentDate+=wxTimeSpan::Weeks(4)+wxTimeSpan::Hour();
	 currentDate.ResetTime();
	 while(currentDate>calEnd){
		calStart+=wxTimeSpan::Week()+wxTimeSpan::Hour(); calStart.ResetTime();
		calEnd+=  wxTimeSpan::Week()+wxTimeSpan::Hour(); calEnd.ResetTime();
	 }
	 calStart+=wxTimeSpan::Week()+wxTimeSpan::Hour();
	 calStart.ResetTime();
	 Refresh(true);
	 //	 outtxt=calStart.FormatISODate()+" "+calStart.FormatISOTime();
	 //	 wxMessageBox(outtxt, "HI", wxOK);
	 break;
  case 'f':
	 m_reportFormat++;
	 if(m_reportFormat==CAL_LASTONE)
		m_reportFormat=CAL_TOTALTIME;
	 Refresh(true);
	 break;
  case 'r':
	 Refresh(true);
	 break;
  default:
	 event.Skip();
  }
}

void EntryCalendar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  DrawCalendar(&dc);
}

void EntryCalendar::OnInsert(wxCommandEvent& WXUNUSED(event))
{
  /*
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
  BikeEntryDialog *be=new BikeEntryDialog(this, *currentProfile, *theData);
  if(be->ShowModal()==wxID_OK){
	 *theData=be->GetData();		  // get the data
	 if(index==-1 || node1==NULL) currentProfile->mainList->Append(theData);
		else currentProfile->mainList->Insert(index, theData);
  }else{
	 delete(theData);
  }
  FillList();
  */
  //  return(true);
}

void EntryCalendar::OnDelete(wxCommandEvent& WXUNUSED(event))
{
  return;
}

bool EntryCalendar::FillList()
{
  /*
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
  */
  return(true);
}

void EntryCalendar::DrawCalendar(wxDC *dc)
{
  int xx, yy, ii, jj, kk, index;

  wxSize clientSize;
  wxRect clientRect;
  wxRect legendRect;
  wxCoord height, width;
  int    maxNumWidth[8*NUMWEEKS];
  long dayTotalTSS, weekTotalTSS=0, monthTotalTSS=0;
  long dayTotalHRTSS, weekTotalHRTSS=0, monthTotalHRTSS=0;
  double totalIf, ifCount;

  double totalTime=0, weekTotalTime=0; // I don't know why totalTime can't be set to 0
  double weekZoneTotals[NUMZONES], zoneTotals[NUMZONES], weekTotalTimes[NUMZONES];


  wxString ifText;

  wxPoint orig;
  wxPen thePen;

  wxString outtxt;

  wxDateTime tDate=calStart;

  orig=GetClientAreaOrigin();

  clientSize=GetClientSize();
  clientRect.SetX(orig.x);		  // set up the clientRectangle that we paint in
  clientRect.SetY(orig.y);
  clientRect.SetHeight(clientSize.GetHeight());
  clientRect.SetWidth(clientSize.GetWidth());
  if(m_reportFormat==CAL_TOTALTIME) outtxt=wxString::Format("Format: Total Time");
  if(m_reportFormat==CAL_ZONETIMES) outtxt=wxString::Format("Format: Zone Bars");
  if(m_reportFormat==CAL_PLANDESCRIPTION) outtxt=wxString::Format("Format: Plan Description");
  if(m_reportFormat==CAL_TIMELESSZ1) outtxt=wxString::Format("Format: Total Time Minus Z1");
  if(m_reportFormat==CAL_STACKEDBARS) outtxt=wxString::Format("Format: Stacked Bars");
  if(m_reportFormat==CAL_STACKEDTIMES) outtxt=wxString::Format("Format: Stacked Times");
  if(m_reportFormat==CAL_NOTES) outtxt=wxString::Format("Format: Notes");
  if(m_reportFormat==CAL_TSS) outtxt=wxString::Format("Format: TSS Display");
  outtxt+=" ("+currentDate.FormatISODate()+")";
  dc->DrawText(outtxt, clientRect.GetLeft(), clientRect.GetTop());
  dc->GetTextExtent(outtxt, &width, &height);
  clientRect.SetY(orig.y+height);
  clientRect.SetHeight(clientSize.GetHeight()-height);

  dc->DrawRectangle(clientRect);
  // draw vert lines

  for(ii=0;ii<8;ii++){			  // seven days plus a week box
	 xx=clientRect.GetLeft()+clientRect.GetWidth()*ii/8;
	 dc->DrawLine(xx, clientRect.GetTop(), xx, clientRect.GetBottom());
	 for(jj=0;jj<NUMWEEKS;jj++){
		index=ii+jj*8;
		calPickEntries[index]=NULL;
		calPicks[index].SetX(xx);
		calPicks[index].SetWidth(clientRect.GetWidth()/8);
	 }
  }
  // draw horiz lines
  for(ii=0;ii<NUMWEEKS;ii++){
	 yy=clientRect.GetTop()+clientRect.GetHeight()*ii/NUMWEEKS;
	 dc->DrawLine(clientRect.GetLeft(), yy, clientRect.GetRight(), yy);
	 for(jj=0;jj<8;jj++){
		calPicks[ii*8+jj].SetY(yy);
		calPicks[ii*8+jj].SetHeight(clientRect.GetTop()+clientRect.GetHeight()*(ii+1)/NUMWEEKS-yy+1);
	 }
  }
  // draw numbers
  for(ii=0;ii<NUMWEEKS;ii++){
	 for(jj=0;jj<7;jj++){
		index=ii*8+jj;
		calPickDates[index]=tDate;
		if(tDate.IsSameDate(currentDate)){
		  dc->SetTextForeground(wxColour(255,0,0));
		  outtxt=wxString::Format("%d ", tDate.GetDay());
		}else{
		  dc->SetTextForeground(wxColour(0,0,0));
		  outtxt=wxString::Format("%d ", tDate.GetDay());
		}
		if(tDate.GetDay()==1){
		  outtxt+=wxString::Format(" %s ",
											wxDateTime::GetMonthName(tDate.GetMonth(),wxDateTime::Name_Abbr).GetData());
		}
		dc->GetTextExtent(outtxt, &width, &height);
		maxNumWidth[index]=width;
		dc->DrawText(outtxt, calPicks[index].x, calPicks[index].y);
		tDate+=wxTimeSpan::Day()+wxTimeSpan::Hour();
		tDate.ResetTime();
	 }
  }
  // draw the report
  EntryList::Node *node1;
  BikeEntryData *dat;
  weekTotalTime=0;

  node1=currentProfile->mainList->GetFirst();
  calStart.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
  tDate=calStart;
  for(ii=0;ii<NUMWEEKS;ii++){	  // each week 
	 for(kk=0;kk<NUMZONES;kk++) weekZoneTotals[kk]=0;
	 for(jj=0;jj<7;jj++){		  // each day
		while(node1!=NULL && node1->GetData()->theDate.IsEarlierThan(tDate)){ // find the node
		  node1=node1->GetNext();                              // for this date
		}
		if(node1!=NULL && node1->GetData()->theDate.IsSameDate(tDate)){// have the data
		  calPickEntries[ii*8+jj]=node1->GetData();
		  dat=node1->GetData();
		  outtxt=dat->notes;		  // print any notes that have stars
		  if(outtxt.Find("*")>-1){
			 index=outtxt.Find("*");
			 outtxt=outtxt.Mid(index+1);
			 index=outtxt.Find("*");
			 if(index>-1) outtxt=outtxt.Left(index);
			 dc->DrawText(outtxt, calPicks[ii*8+jj].x+maxNumWidth[ii*8+jj], calPicks[ii*8+jj].y);
		  }
		  if(m_reportFormat==CAL_TOTALTIME ||
			  m_reportFormat==CAL_TIMELESSZ1 ||
			  m_reportFormat==CAL_STACKEDTIMES){ // --- print total times ---
			 totalTime=0;
			 for(kk=0;kk<NUMZONES;kk++) zoneTotals[kk]=0;
			 while(node1!=NULL && node1->GetData()->theDate.IsSameDate(tDate)){ // get all sessions for the day
				dat=node1->GetData();
				for(kk=0;kk<NUMZONES;kk++){ // get all the zones
				  if(dat!=NULL && (kk!=0 || m_reportFormat==CAL_TOTALTIME || m_reportFormat==CAL_STACKEDTIMES)){
					 // add time for day zone totals
					 zoneTotals[kk]+=dat->fileZoneTimes[kk].Seconds()+
						dat->manualZoneTimes[kk].Seconds();
					 
					 // add time for week total
					 totalTime+=dat->fileZoneTimes[kk].Seconds();
					 totalTime+=dat->manualZoneTimes[kk].Seconds();
				  }
				}
				node1=node1->GetNext();
			 }
			 if(totalTime>0){		  // don't print if no time
				if(m_reportFormat==CAL_STACKEDTIMES){
				  for(kk=NUMZONES-2;kk>=0;kk--){
					 zoneTotals[kk]+=zoneTotals[kk+1];
				  }
				}

				TimePeriod tp(totalTime);
				outtxt=wxString::Format("T:%s", tp.HHMMSSHH().GetData());
				dc->DrawText(outtxt, calPicks[ii*8+jj].x, calPicks[ii*8+jj].y+height);
				weekTotalTime+=totalTime;	  // track week total time

				// print day zone total times
				for(kk=0;kk<NUMZONES;kk++){
				  switch(kk){		  // select color for fill
				  case 0:dc->SetTextForeground(*wxBLACK); break;
				  case 1:dc->SetTextForeground(*wxLIGHT_GREY);  break;
				  case 2:dc->SetTextForeground(*wxGREEN);       break;
				  case 3:dc->SetTextForeground(*wxCYAN);        break;
				  case 4:dc->SetTextForeground(*wxBLACK); break;
				  case 5:dc->SetTextForeground(*wxBLUE); break;
				  case 6:dc->SetTextForeground(*wxRED); break;
				  default:
					 dc->SetTextForeground(*wxBLACK);break;
				  }

				  if(zoneTotals[kk]>0){
					 weekZoneTotals[kk]+=zoneTotals[kk];
					 TimePeriod tp(zoneTotals[kk]);
					 outtxt=wxString::Format("%s", tp.HHMMSSHH().GetData());
					 dc->DrawText(outtxt,
									  calPicks[ii*8+jj].x, 
									  calPicks[ii*8+jj].y+(kk+2)*height);
				  }
				}
			 }
		  }

		  if(m_reportFormat==CAL_TSS){	// TSS Report
			 dayTotalTSS=0;
			 dayTotalHRTSS=0;
			 ifCount=0;
			 ifText.Empty();
			 while(node1!=NULL && node1->GetData()->theDate.IsSameDate(tDate)){ // get all sessions for the day
				dat=node1->GetData();
				totalIf=0;			  // this will be used to store if for the entry
										  // (not the day)
				if(dat->ppTss!=0){  // add tss totals
				  dayTotalTSS+=dat->ppTss;
				  totalIf=dat->ppIf;
				  ifCount++;
				}else if(dat->ptTss!=0){
				  dayTotalTSS+=dat->ptTss;
				  totalIf=dat->ptIf;
				  ifCount++;
				}else if(dat->ctTss!=0){
				  dayTotalTSS+=dat->ctTss;
				  totalIf=dat->ctIf;
				  ifCount++;
				}else if(dat->polarTss!=0){
				  dayTotalTSS+=dat->polarTss;
				  totalIf=dat->polarIf;
				  ifCount++;
				}

				if(dat->polarTss!=0){
				  dayTotalHRTSS+=dat->polarTss;
				}else if(dat->ppTss!=0){  // add hrtss totals
				  dayTotalHRTSS+=dat->ppTss;
				}else if(dat->ptTss!=0){
				  dayTotalHRTSS+=dat->ptTss;
				}else if(dat->ctTss!=0){
				  dayTotalHRTSS+=dat->ctTss;
				}

				if(ifText.IsEmpty()){	  // first if entry for the day
				  if(totalIf>0){
					 ifText=wxString::Format("%3.2lf", totalIf);
				  }else{				  // special formatting for 0
					 ifText=wxString::Format("-");
				  }
				}else{
				  if(totalIf>0){
					 ifText+=wxString::Format(",%3.2lf", totalIf);
				  }else{
					 ifText+=wxString::Format(",-");
				  }
				}
				node1=node1->GetNext();
			 }
			 weekTotalTSS+=dayTotalTSS;
			 weekTotalHRTSS+=dayTotalHRTSS;

			 dc->SetBrush(*wxTRANSPARENT_BRUSH);
			 dc->SetTextForeground(*wxBLACK);
			 if(dayTotalTSS>0 || dayTotalHRTSS>0){
				outtxt=wxString::Format("P-%ld:H-%ld", dayTotalTSS, dayTotalHRTSS);
				dc->DrawText(outtxt,
								 calPicks[ii*8+jj].x, 
								 calPicks[ii*8+jj].y+2*height);
				outtxt=wxString::Format("IF:(%d) ", (int)ifCount)+ifText;
				dc->DrawText(outtxt,
								 calPicks[ii*8+jj].x, 
								 calPicks[ii*8+jj].y+3*height);
			 }
		  }

      if(m_reportFormat==CAL_NOTES){
        wxCoord textWidth, textHeight;
        int charsThatFit;

        outtxt="\n";
        while(node1!=NULL && node1->GetData()->theDate.IsSameDate(tDate)){ // get all sessions for the day
          dat=node1->GetData();
          outtxt.append(dat->notes.Mid(dat->notes.Find('*',true)+1));
          outtxt.append('\n');
          node1=node1->GetNext();
        }
        dc->SetTextForeground(wxColour(0,0,0));
        dc->SetTextBackground(wxColour(255,255,255));
        dc->SetBackgroundMode(wxPENSTYLE_TRANSPARENT);

        dc->GetTextExtent("XX", &textWidth, &textHeight); // How wide are two letters?
        charsThatFit=calPicks[ii*8+jj].width*2/textWidth;
        int zz=charsThatFit;
        while(zz<outtxt.length()){
          int yy;
          yy=outtxt.rfind(' ', zz);
          if(yy>zz-charsThatFit+2) zz=yy;
          outtxt.insert(zz,"\n");
          zz=zz+charsThatFit;
        }
        dc->GetTextExtent(outtxt, &textWidth, &textHeight);
        dc->DrawText(outtxt, calPicks[ii*8+jj].x,
                     calPicks[ii*8+jj].y);
      }


		  if(m_reportFormat==CAL_PLANDESCRIPTION){ // --- print plan descriptions ---
			 wxString firstHalf, secondHalf;
			 wxCoord textWidth, textHeight;

			 int nextSpace;
			 int leftBorder, rightBorder, bottomBorder, xPos, yPos;

			 dat=node1->GetData();
			 if(dat->m_planDescription.IsEmpty()==false
				 || dat->m_planNotes.IsEmpty()==false
				 || dat->m_planTime.Seconds()>0
				 || dat->m_priority>0){		  // don't print if empty

				if(dat->m_priority==3){	// priority A
				  outtxt="*Priority A*";
				  dc->SetTextForeground(wxColour(255,255,0));
				  dc->SetTextBackground(wxColour(0,0,0));
				  dc->SetBackgroundMode(wxSOLID);
				  dc->GetTextExtent(outtxt, &textWidth, &textHeight);
				  dc->DrawText(outtxt, calPicks[ii*8+jj].x+calPicks[ii*8+jj].width-textWidth,
									calPicks[ii*8+jj].y);

				}
				if(dat->m_priority==2){	// priority B
				  outtxt="*Priority B*";
				  dc->SetTextForeground(wxColour(255,255,0));
				  dc->SetTextBackground(wxColour(180,180,180));
				  dc->SetBackgroundMode(wxSOLID);
				  dc->GetTextExtent(outtxt, &textWidth, &textHeight);
				  dc->DrawText(outtxt, calPicks[ii*8+jj].x+calPicks[ii*8+jj].width-textWidth,
									calPicks[ii*8+jj].y);
				}
				if(dat->m_priority==1){	// priority C
				  outtxt="*Priority C*";
				  dc->SetTextForeground(wxColour(255,255,0));
				  dc->SetTextBackground(wxColour(210,210,210));
				  dc->SetBackgroundMode(wxSOLID);
				  dc->GetTextExtent(outtxt, &textWidth, &textHeight);
				  dc->DrawText(outtxt, calPicks[ii*8+jj].x+calPicks[ii*8+jj].width-textWidth,
									calPicks[ii*8+jj].y);
				}
				if(dat->m_priority==0){	// no priority
				  dc->SetTextForeground(wxColour(0,0,0));
				  dc->SetBackgroundMode(wxTRANSPARENT);
				}

				dc->SetTextForeground(wxColour(0,0,255));
				outtxt=wxString::Format("%s: %d", dat->m_planPhase.GetData(),
												dat->m_mesoCycleWeek);
				dc->DrawText(outtxt, calPicks[ii*8+jj].x, calPicks[ii*8+jj].y+height);
				
				dc->SetTextForeground(wxColour(255,0,0));
				outtxt=wxString::Format("%s", dat->m_planDescription.GetData());
				dc->DrawText(outtxt, calPicks[ii*8+jj].x, calPicks[ii*8+jj].y+2*height);

				dc->SetTextForeground(wxColour(255,0,0));
				outtxt=wxString::Format("%s", dat->m_planTime.HHMM().GetData());
				dc->DrawText(outtxt, calPicks[ii*8+jj].x, calPicks[ii*8+jj].y+3*height);

				dc->SetTextForeground(wxColour(0,255,0));
				outtxt=wxString::Format("%s", dat->m_planNotes.GetData());
				nextSpace=outtxt.Find(' ');

				leftBorder=calPicks[ii*8+jj].x;
				rightBorder=calPicks[ii*8+jj].x+calPicks[ii*8+jj].GetWidth();
				bottomBorder=calPicks[ii*8+jj].y+calPicks[ii*8+jj].GetHeight();;
				xPos=calPicks[ii*8+jj].x;
				yPos=calPicks[ii*8+jj].y+4*height;

				do{
				  if(nextSpace==-1)nextSpace=outtxt.Length();
				  firstHalf=outtxt.Left(nextSpace+1);
				  outtxt=outtxt.Mid(nextSpace+1);
				  dc->GetTextExtent(firstHalf, &textWidth, &textHeight);
				  if(xPos+textWidth>rightBorder){ // can't fit on the line
					 xPos=leftBorder;
					 yPos+=textHeight;
				  }
				  if(yPos<bottomBorder) dc->DrawText(firstHalf, xPos, yPos);
				  xPos+=textWidth;
				  nextSpace=outtxt.Find(' ');
				}while(!outtxt.IsEmpty() && yPos<bottomBorder);
			 }
		  }
		  if(m_reportFormat==CAL_ZONETIMES || m_reportFormat==CAL_STACKEDBARS){ // --- graph the zones ---
			 for(kk=0;kk<NUMZONES;kk++) zoneTotals[kk]=0;
			 while(node1!=NULL && node1->GetData()->theDate.IsSameDate(tDate)){ // get all sessions for the day
				dat=node1->GetData();
				for(kk=0;kk<NUMZONES;kk++){ // get all the zones
				  zoneTotals[kk]+=dat->fileZoneTimes[kk].Seconds();
				  zoneTotals[kk]+=dat->manualZoneTimes[kk].Seconds();
				  totalTime+=zoneTotals[kk];
				}
				node1=node1->GetNext();
			 }

			 if(m_reportFormat==CAL_STACKEDBARS){
				for(kk=NUMZONES-2;kk>=0;kk--){
				  zoneTotals[kk]+=zoneTotals[kk+1];
				}
			 }

			 if(totalTime>0){		  // don't draw if no time
				for(kk=0;kk<NUMZONES;kk++){
				  switch(kk){		  // select color for fill
				  case 0:dc->SetBrush(*wxTRANSPARENT_BRUSH); break;
				  case 1:dc->SetBrush(*wxLIGHT_GREY_BRUSH);  break;
				  case 2:dc->SetBrush(*wxGREEN_BRUSH);       break;
				  case 3:dc->SetBrush(*wxCYAN_BRUSH);        break;
				  case 4:dc->SetBrush(*wxGREY_BRUSH); break;
				  case 5:dc->SetBrush(*wxBLUE_BRUSH); break;
				  case 6:dc->SetBrush(*wxRED_BRUSH); break;
				  default:
					 dc->SetBrush(*wxTRANSPARENT_BRUSH);break;
				  }

				  xx=ii*8+jj;
				  dc->DrawRectangle(calPicks[xx].x+calPicks[xx].GetWidth()*kk/NUMZONES, 
										  calPicks[xx].GetBottom(),
										  calPicks[xx].GetWidth()/NUMZONES,
										  -(int)(calPicks[xx].GetHeight()*zoneTotals[kk]/MAXDAYTOTAL));
				  weekTotalTimes[kk]+=zoneTotals[kk];	  // track week total time
				}
			 }
		  }
		}
		tDate+=wxTimeSpan::Day()+wxTimeSpan::Hour(); tDate.ResetTime();
	 }
	 if((m_reportFormat==CAL_TOTALTIME
		  || m_reportFormat==CAL_TIMELESSZ1)
		 && weekTotalTime>0){		  // if there was anytime this week
		TimePeriod tp(weekTotalTime);
		outtxt=wxString::Format("T:%s", tp.HHMMSSHH().GetData());
		dc->DrawText(outtxt, calPicks[ii*8+7].x, calPicks[ii*8+7].y+height);
		// draw the zone totals for the week
		for(kk=0;kk<NUMZONES;kk++){
		  TimePeriod tp(weekZoneTotals[kk]);
		  outtxt=wxString::Format("%s", tp.HHMMSSHH().GetData());
		  if(m_reportFormat==CAL_TOTALTIME || kk>0){
			 switch(kk){		  // select color for fill
			 case 0:dc->SetTextForeground(*wxBLACK); break;
			 case 1:dc->SetTextForeground(*wxLIGHT_GREY);  break;
			 case 2:dc->SetTextForeground(*wxGREEN);       break;
			 case 3:dc->SetTextForeground(*wxCYAN);        break;
			 case 4:dc->SetTextForeground(*wxBLACK); break;
			 case 5:dc->SetTextForeground(*wxBLUE); break;
			 case 6:dc->SetTextForeground(*wxRED); break;
			 default:
				dc->SetTextForeground(*wxBLACK);break;
			 }

			 dc->DrawText(outtxt,
							  calPicks[ii*8+jj].x, 
							  calPicks[ii*8+jj].y+(kk+2)*height);
		  }
		}
	 }
	 if(m_reportFormat==CAL_ZONETIMES){
		for(kk=0;kk<NUMZONES;kk++){
		  switch(kk){		  // select color for fill
		  case 0:dc->SetBrush(*wxTRANSPARENT_BRUSH); break;
		  case 1:dc->SetBrush(*wxLIGHT_GREY_BRUSH);  break;
		  case 2:dc->SetBrush(*wxGREEN_BRUSH);       break;
		  case 3:dc->SetBrush(*wxCYAN_BRUSH);        break;
		  case 4:dc->SetBrush(*wxGREY_BRUSH); break;
		  case 5:dc->SetBrush(*wxBLUE_BRUSH); break;
		  case 6:dc->SetBrush(*wxRED_BRUSH); break;
		  default:
			 dc->SetBrush(*wxTRANSPARENT_BRUSH);break;
		  }

		  xx=ii*8+7;
		  dc->DrawRectangle(calPicks[xx].x+calPicks[xx].GetWidth()*kk/NUMZONES, 
								  calPicks[xx].GetBottom(),
								  calPicks[xx].GetWidth()/NUMZONES,
								  -(int)(calPicks[xx].GetHeight()*weekTotalTimes[kk]/MAXWEEKTOTAL));
		  weekTotalTimes[kk]=0;
		}
	 }
	 if(m_reportFormat==CAL_TSS){	// draw the text for the week total TSS
		outtxt=wxString::Format("P-%ld:H-%ld", weekTotalTSS, weekTotalHRTSS);
		xx=ii*8+7;
		dc->DrawText(outtxt,
						 calPicks[xx].x, 
						 calPicks[xx].y+2*height);
		if(ii>0 && ii<5){
		  monthTotalTSS+=weekTotalTSS;
		  monthTotalHRTSS+=weekTotalHRTSS;
		}
		if(ii==4){// draw the text for the month total TSS
		  outtxt=wxString::Format("Four Week: P-%ld:H-%ld", monthTotalTSS, monthTotalHRTSS);
		  dc->DrawText(outtxt,
							calPicks[xx].x,
							calPicks[xx].y+3*height);
		}
	 }

	 weekTotalTime=0;
	 weekTotalTSS=0;
	 weekTotalHRTSS=0;
  }
  calEnd=tDate;
}
void EntryCalendar::OnMouseLeftUp(wxMouseEvent & event)
{
  int  entryIndex;
  int  mouseX, mouseY;
  int  ii,jj,index;

  mouseX=event.GetX();
  mouseY=event.GetY();

  entryIndex=-1;
  for(ii=0;ii<NUMWEEKS;ii++){	  // find what they hit
	 for(jj=0;jj<7;jj++){
		index=ii*8+jj;
		if(calPicks[index].Contains(mouseX, mouseY)){
		  if(calPickEntries[index]!=NULL){
			 currentDate=calPickEntries[index]->theDate;
			 Refresh(true);
			 jj=8; ii=NUMWEEKS;
		  }else{						  // insert new entry, will be -1 if existing
										  // entry found
			 entryIndex=index;
		  }
		}
	 }	 
  }
}


void EntryCalendar::OnDoubleclick(wxMouseEvent &event)
  //void EntryCalendar::OnMouseLeftUp (wxMouseEvent & event)
{
  wxString text;
  int mouseX, mouseY;
  int ii, jj, index, session;
  int  entryIndex;
  BikeEntryData *theEntry;//, *countingEntry;
  EntryList::Node *node1, *node2;

  mouseX=event.GetX();
  mouseY=event.GetY();

  theEntry=NULL;
  entryIndex=-1;
  for(ii=0;ii<NUMWEEKS;ii++){	  // find what they hit
	 for(jj=0;jj<7;jj++){
		index=ii*8+jj;
		if(calPicks[index].Contains(mouseX, mouseY)){
		  if(calPickEntries[index]!=NULL){
			 currentDate=calPickEntries[index]->theDate;
			 theEntry=calPickEntries[index];
		  }else{						  // insert new entry, will be -1 if existing
										  // entry found
			 entryIndex=index;
		  }
		}
	 }	 
  }

  if(m_reportFormat==CAL_PLANDESCRIPTION){ // display the detailed plan view
	 if(entryIndex>=0){
		InsertNewEntry(calPickDates[entryIndex], true);
	 }else{
		PlanDisplayDialog *pd=new PlanDisplayDialog(this, currentProfile,
																  *theEntry);
		if(pd->ShowModal()==wxID_OK){
		  *theEntry=pd->GetData();
		}
		delete(pd);
	 }
  }else{								  // -- display the ride data --
	 if(entryIndex>-1){			  // new entry
		InsertNewEntry(calPickDates[entryIndex], false);
	 }else{							  // at least one entry for today already exists
		node1=currentProfile->mainList->GetFirst();
		while(node1!=NULL && node1->GetData()!=theEntry) node1=node1->GetNext();
		node2=node1;

		session=0;					  // count the sessions
		while(node2!=NULL && node2->GetData()->theDate.IsSameDate(node1->GetData()->theDate)){
		  session++;
		  node2=node2->GetNext();
		}
		DayEntrySelectionDialog *des=new DayEntrySelectionDialog(this, session);
		if(des->ShowModal()==wxID_CANCEL){ // if they cancel, don't do anything
		  Refresh(true);
		  delete(des);
		  return;
		}
		session=des->GetSessionSelected();
		delete(des);
		if(session==-1){			  // new entry selected
		  InsertNewEntry(theEntry->theDate, false);
		}else{						  // open the selected entry
		  for(ii=1;ii<session;ii++){
			 node1=node1->GetNext();
		  }
		  theEntry=node1->GetData();

		  BikeEntryDialog *be=new BikeEntryDialog(this, currentProfile,
																*theEntry);
		  if(be->ShowModal()==wxID_OK){
			 *theEntry=be->GetData();		  // get the data
		  }
		  delete(be);
		}
	 }
  }
  Refresh(true);
}

void EntryCalendar::InsertNewEntry(wxDateTime theDate, bool justPlan){
  BikeEntryData *theData=new BikeEntryData();
  EntryList::Node *node1;
  int session;
  int index;

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
  if(justPlan==false){
	 BikeEntryDialog *be=new BikeEntryDialog(this, currentProfile, *theData);
	 if(be->ShowModal()==wxID_OK){
		*theData=be->GetData();		  // get the data
		if(index==-1 || node1==NULL) currentProfile->mainList->Append(theData);
		else currentProfile->mainList->Insert(index, theData);
	 }else{
		delete(theData);
	 }
	 delete(be);
  }else{
	 PlanDisplayDialog *be=new PlanDisplayDialog(this, currentProfile, *theData);
	 if(be->ShowModal()==wxID_OK){
	 *theData=be->GetData();		  // get the data
	 if(index==-1 || node1==NULL) currentProfile->mainList->Append(theData);
		else currentProfile->mainList->Insert(index, theData);
	 }else{
		delete(theData);
	 }
	 delete(be);
  }
  Refresh(true);
}

void EntryCalendar::OnViewPlan()
{
  m_reportFormat=CAL_PLANDESCRIPTION;  
  Refresh();
}

void EntryCalendar::OnTssView()
{
  m_reportFormat=CAL_TSS;  
  Refresh();
}

void EntryCalendar::OnViewHeartInZone()
{
  m_reportFormat=CAL_ZONETIMES;
  Refresh();
}

void EntryCalendar::OnViewTotals()
{
  m_reportFormat=CAL_TOTALTIME;
  Refresh();
}

void EntryCalendar::ExportWeekCSV(wxString filename)
{
  FILE *fp;
  TimePeriod zoneTotals[NUMZONES];
  int ii;

  EntryList::Node *node1;
  BikeEntryData   *entry;
  wxDateTime theDate;
  wxDateTime nextWeek;

  node1=currentProfile->mainList->GetFirst();
  entry=node1->GetData();
  theDate=entry->theDate;
  theDate.ResetTime();
  theDate.SetToWeekDayInSameWeek(wxDateTime::Mon, wxDateTime::Monday_First);
  nextWeek=theDate+wxTimeSpan::Week()+wxTimeSpan::Hour();
  nextWeek.ResetTime();

  fp=fopen(filename, "w");
  if(fp==NULL){
	 wxMessageBox("Error", "Could write to file", wxOK);
	 return;
  }

  // loop through all entries
  while(node1!=NULL){
	 for(ii=0;ii<NUMZONES;ii++) zoneTotals[ii]=0; // reset week totals
	 while(node1!=NULL && entry->theDate.IsEarlierThan(nextWeek)){	// move through the week
		for(ii=0;ii<NUMZONES;ii++){
		  zoneTotals[ii]=zoneTotals[ii]+entry->fileZoneTimes[ii]+entry->manualZoneTimes[ii];
		}
		node1=node1->GetNext();
		if(node1!=NULL){
		  entry=node1->GetData();
		}else{
		  entry=NULL;
		}
	 }
	 fprintf(fp, "%s", theDate.FormatISODate().ToUTF8().data());
	 for(ii=0;ii<NUMZONES;ii++){
		fprintf(fp,",%s", zoneTotals[ii].HHMMSSHH().ToUTF8().data());
	 }
	 fprintf(fp, "\n");
	 theDate=nextWeek;
	 nextWeek+=wxTimeSpan::Week()+wxTimeSpan::Hour();
	 nextWeek.ResetTime();
  }
  fclose(fp);
}
