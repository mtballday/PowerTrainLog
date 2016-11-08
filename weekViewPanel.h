#include"timePeriod.h"
#include<wx/datetime.h>

#include"bikeEntryData.h"
#include"userProfile.h"			  // this must be down here

#ifndef __WEEKVIEWPANEL__
#define __WEEKVIEWPANEL__

#define WVP_WEEKS 52
#define WVP_COLUMNS 4

class WeekViewPanel : public wxPanel
{
private:
  DECLARE_EVENT_TABLE();

  wxDateTime m_calStart;
  wxDateTime m_calEnd;

  wxRect m_pickWeeks[WVP_WEEKS];
  int    m_weekHeight;

  int m_reportFormat;
  UserProfile *currentProfile;
  BikeEntryData *currentData;

public:
  wxDateTime m_currentDate;
  wxFrame *m_parentFrame;

  WeekViewPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
					 const wxSize size, UserProfile *up);
  void OnChar(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& WXUNUSED(event));
  void DrawWeekCalendar(wxDC *dc);
  void OnDClick(wxMouseEvent &event);
  void OnLClick(wxMouseEvent &event);
  void InsertNewEntry(wxDateTime theDate);
  void Repaint();
};

#endif
