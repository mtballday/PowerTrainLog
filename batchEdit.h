#include"timePeriod.h"
#include<wx/calctrl.h>
#include<wx/datetime.h>

#include"bikeEntryData.h"
#include"userProfile.h"			  // this must be down here

#ifndef __DEFBATCHEDIT__
#define __DEFBATCHEDIT__
class BatchEditDialog : public wxDialog
{
  // --- Vars ---
  UserProfile *userProfile;
  wxWindow *m_parent;

  wxDateTime theDate;
  int session;

  wxString m_planDescription, m_planNotes, m_planPhase;
  int      m_planMesoCycleWeek;

  wxComboBox *phasesCtrl;
  wxTextCtrl *m_daysToInsertCtrl;

  BikeEntryData theData;

  void FillPhases();

public:
  wxCalendarCtrl *startDateCalendar;
  wxCalendarCtrl *endDateCalendar;
  WeekViewPanel *weekViewParent;

  // vars
  BatchEditDialog(wxWindow *parent, UserProfile*);
  //  void OnSelectWorkout(wxCommandEvent &event);
  //  void OnSetWorkout(wxCommandEvent &event);

  void OnFill(wxCommandEvent& WXUNUSED(event));
  void OnCancel(wxCommandEvent& WXUNUSED(event));
  BikeEntryData GetData();
  void AdjustEnd(wxCalendarEvent& WXUNUSED(event));
  void AdjustStart(wxCalendarEvent& WXUNUSED(event));
  void OnMoveBack(wxCommandEvent& WXUNUSED(event));
  void OnMoveUp(wxCommandEvent& WXUNUSED(event));

  void MoveUp(int days, wxDateTime start, wxDateTime end);
  void MoveBack( int days, wxDateTime start, wxDateTime end);

  DECLARE_EVENT_TABLE()
};
#endif
