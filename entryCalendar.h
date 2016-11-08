#ifndef __ENTRYCALENDAR__
#define __ENTRYCALENDAR__

#include<wx/listctrl.h>
#include<wx/calctrl.h>
#include"userProfile.h"

#define NUMWEEKS 5
#define MAXDAYTOTAL 14400.0
#define MAXWEEKTOTAL 36000.0
class EntryCalendar:public wxPanel
{
  wxCalendarCtrl *calendar;
  UserProfile *currentProfile;
  wxDateTime  calStart, calEnd;
  wxRect      calPicks[8*NUMWEEKS];
  BikeEntryData *calPickEntries[8*NUMWEEKS];
  wxDateTime  calPickDates[8*NUMWEEKS];
  int         m_reportFormat;

public:
  wxDateTime  currentDate;
  EntryCalendar(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  void OnPaint(wxPaintEvent& WXUNUSED(event));
  void DrawCalendar(wxDC *dc);
  void OnChar(wxKeyEvent& event);

  void OnInsert(wxCommandEvent& WXUNUSED(event));
  void OnDelete(wxCommandEvent& WXUNUSED(event));
  bool FillList();
  void OnMouseLeftUp(wxMouseEvent & event);
  void OnDoubleclick(wxMouseEvent &event);

  void OnViewTotals();
  void OnViewHeartInZone();
  void OnViewPlan();
  void OnTssView();


  void ExportWeekCSV(wxString filename);

  void InsertNewEntry(wxDateTime theDate, bool justPlan);
  //  void OnMouseLeftUp (wxMouseEvent & event);

  DECLARE_EVENT_TABLE()
	 ;
  enum
  {
	 CAL_TOTALTIME=1,
	 CAL_ZONETIMES,
	 CAL_PLANDESCRIPTION,
	 CAL_TIMELESSZ1,
	 CAL_STACKEDBARS,
	 CAL_STACKEDTIMES,
	 CAL_TSS,
   CAL_NOTES,
	 CAL_LASTONE
  };
};


#endif
