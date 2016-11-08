#include<wx/datetime.h>
#include"userProfile.h"			  // this must be down here

#ifndef __WORKOUTMENUEDITOR__
#define __WORKOUTMENUEDITOR__

#include"wx/listctrl.h"
class WorkoutMenuEditor : public wxDialog
{
  UserProfile *userProfile;

public:
  WorkoutMenuEditor(wxWindow *parent, UserProfile *pf);
  void FillWorkouts();
  void FillPhases();
  void OnClickedWorkout(wxCommandEvent &event);
  void AddWorkout(wxCommandEvent &event);
  void SaveWorkout(wxCommandEvent &event);
  void DeleteWorkout(wxCommandEvent &event);
  void EditPhase(wxListEvent &event);

  wxListBox *workoutMenuCtrl;
  wxTextCtrl *workoutTitleCtrl;
  wxTextCtrl *workoutAbbrevCtrl;
  wxTextCtrl *workoutDescriptionCtrl;
  wxListCtrl *phasesCtrl;

  DECLARE_EVENT_TABLE();
};
#endif
