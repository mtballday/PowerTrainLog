#include"timePeriod.h"
#include<wx/datetime.h>

#include"bikeEntryData.h"
#include"userProfile.h"			  // this must be down here

#ifndef __SINGLEWEEKVIEWPANEL__
#define __SINGLEWEEKVIEWPANEL__

#define WVP_WEEKS 52
#define WVP_COLUMNS 4

class SingleWeekViewPanel : public wxPanel
{
private:
  DECLARE_EVENT_TABLE();

  wxRect m_pickWeeks[WVP_WEEKS];
  int    m_weekHeight;

  int m_reportFormat;
  UserProfile *currentProfile;
  BikeEntryData *currentData;

  wxPoint mousePick;

public:
  wxDateTime m_calStart;

  SingleWeekViewPanel(wxWindow *parent, wxWindowID id, const wxPoint pos,
					 const wxSize size, UserProfile *up);
  void OnChar(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& WXUNUSED(event));
  void DrawWeek(wxDC *dc);
  void OnDClick(wxMouseEvent &event);
  void OnLClick(wxMouseEvent &event);
};

#endif
