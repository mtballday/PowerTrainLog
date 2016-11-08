#include"timePeriod.h"
#include<wx/datetime.h>

#include"interval.h"
#include"bikeEntryData.h"
#include"userProfile.h"			  // this must be down here

#ifndef __BIKEENTRYDIALOG__
#define __BIKEENTRYDIALOG__

class BikeEntryDialog : public wxDialog
{
  // --- Vars ---
  UserProfile *userProfile;

  wxDateTime theDate;
  int session;
  bool       dialogInitialized;

  wxTextCtrl *getUpTimeCtrl, *sleepTimeCtrl, *weightCtrl, *fatCtrl;
  wxTextCtrl *restHrCtrl;
  wxTextCtrl *notesCtrl;
  wxCheckBox *keyEventCtrl;
  wxCheckBox *sickCtrl;
  wxChoice   *m_recoveryRatingCtrl;
  wxTextCtrl *distanceCtrl;
  wxCheckBox *mtbCheck;
  wxListBox  *intervalListCtrl; /// list box for viewing the list of intervals
  wxComboBox *bikeCtrl;

  wxStaticText *fileZoneTimes[8];
  wxStaticText *powerZoneTimes[8];
  wxStaticText *powerZonePercent[8];
  wxTextCtrl   *manualZoneTimes[7];
  
  wxStaticText  *totalManualTime;
  wxStaticText *totalTimes[8];

  wxTextCtrl *ppFileCtrl, *polarFileCtrl, *ptFileCtrl, *ctFileCtrl;

  wxTextCtrl *ppTssCtrl, *polarTssCtrl, *ptTssCtrl, *ctTssCtrl;
  wxTextCtrl *ppIfCtrl, *polarIfCtrl, *ptIfCtrl, *ctIfCtrl;
  wxButton *viewPPButton, *viewPolarButton,   *viewPtButton,   *viewCtButton;
  wxButton *repairPPButton, *ppToTssButton;
  wxButton *ppBrowseButton, *polarBrowseButton, *ptBrowseButton, *ctBrowseButton;
  wxButton *ppAnalyzeButton, *polarAnalyzeButton, *ptAnalyzeButton, *ctAnalyzeButton;

  wxString m_planDescription, m_planNotes, m_planPhase;
  int      m_planMesoCycleWeek;
  int      m_priority;
  TimePeriod m_planTime;
  int      m_planIntensity;
  IntervalList m_intervals;

public:
  // vars
  BikeEntryDialog(wxWindow *parent, UserProfile*, BikeEntryData);

  // functions
  void OnManual(wxCommandEvent& WXUNUSED(event));
  void OnBrowsePP(wxCommandEvent& WXUNUSED(event));
  void OnBrowsePolar(wxCommandEvent& WXUNUSED(event));
  void OnBrowsePt(wxCommandEvent& WXUNUSED(event));
  void OnBrowseCt(wxCommandEvent& WXUNUSED(event));

  void OnAnalyzePP(wxCommandEvent& WXUNUSED(event));
  void OnAnalyzePolar(wxCommandEvent& WXUNUSED(event));
  void OnAnalyzePt(wxCommandEvent& WXUNUSED(event));
  void OnAnalyzeCt(wxCommandEvent& WXUNUSED(event));

  void OnRepairPP(wxCommandEvent& WXUNUSED(event));
  void OnPPHRtoTSS(wxCommandEvent& WXUNUSED(event));

  void OnOK(wxCommandEvent& WXUNUSED(event));
  void OnCancel(wxCommandEvent& WXUNUSED(event));

  void OnViewPP(wxCommandEvent& WXUNUSED(event));
  void OnViewPolar(wxCommandEvent& WXUNUSED(event));
  void OnViewPT(wxCommandEvent& WXUNUSED(event));
  void OnViewCT(wxCommandEvent& WXUNUSED(event));

  void SetIntervalList();

  BikeEntryData GetData();

  DECLARE_EVENT_TABLE()
};
#endif
