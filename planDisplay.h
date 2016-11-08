#include"timePeriod.h"
#include<wx/datetime.h>

#include"bikeEntryData.h"
#include"userProfile.h"			  // this must be down here

#ifndef __PLANDISPLAY__
#define __PLANDISPLAY__
class PlanDisplayDialog : public wxDialog
{
  // --- Vars ---
  UserProfile *userProfile;

  wxDateTime theDate;
  int session;

  wxString m_planDescription, m_planNotes, m_planPhase;
  TimePeriod m_planTime;
  int      m_planMesoCycleWeek;

  wxTextCtrl *notesTextCtrl;
  wxTextCtrl *timeTextCtrl;
  wxButton   *workoutsButton;
  wxComboBox *workoutsCtrl;
  wxComboBox *phasesCtrl;
  wxChoice   *priorityCtrl;
  wxChoice   *intensityCtrl;
  wxTextCtrl *workoutsSelected;

  BikeEntryData theData;

  void FillPhases();

public:
  // vars
  PlanDisplayDialog(wxWindow *parent, UserProfile*, BikeEntryData);
  void OnSelectWorkout(wxCommandEvent &event);
  void OnSetWorkout(wxCommandEvent &WXUNUSED(event));

  void OnOK(wxCommandEvent &WXUNUSED(event));
  void OnCancel(wxCommandEvent &WXUNUSED(event));
  BikeEntryData GetData();

  DECLARE_EVENT_TABLE()
};
#endif
