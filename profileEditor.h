#ifndef __PROFILEEDITOR__
#define __PROFILEEDITOR__

#include<wx/listctrl.h>
#include<wx/calctrl.h>
#include"userProfile.h"

class ProfileEditor:public wxDialog
{
  UserProfile *currentProfile;

  wxTextCtrl  *m_nameCtrl;
  wxTextCtrl  *m_databaseCtrl, *m_workoutsCtrl;
  wxTextCtrl  *m_thresholdWattsCtrl;
  wxTextCtrl  *m_mtbThresholdWattsCtrl;
  wxTextCtrl  *m_thresholdHrCtrl;
  wxTextCtrl  *m_hrCutOffCtrl;
  wxTextCtrl  *m_acuteTc;
  wxTextCtrl  *m_chronicTc;

  wxTextCtrl *m_powerplanCtrl, *m_computrainerCtrl, *m_powertapCtrl, *m_polarCtrl, *m_garminCtrl;
  wxTextCtrl *m_powerplanScaleCtrl, *m_powertapScaleCtrl, *m_computrainerScaleCtrl, *m_polarScaleCtrl;

  wxTextCtrl *m_hrzones[NUMZONES], *m_restHrCtrl, *m_maxHrCtrl;

public:
  ProfileEditor(wxWindow *parent, wxWindowID id, const wxPoint pos,
									  const wxSize size, UserProfile *up);

  void OnDatabaseBrowse(wxCommandEvent &WXUNUSED(event));
  void OnBrowseWorkoutMenu(wxCommandEvent &WXUNUSED(event));
  void OnBrowsePP(wxCommandEvent &WXUNUSED(event));
  void OnBrowsePT(wxCommandEvent &WXUNUSED(event));
  void OnBrowseCT(wxCommandEvent &WXUNUSED(event));
  void OnBrowsePolar(wxCommandEvent &WXUNUSED(event));
  void OnBrowseGarmin(wxCommandEvent &WXUNUSED(event));

  void OnOk(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  DECLARE_EVENT_TABLE()
};


#endif
