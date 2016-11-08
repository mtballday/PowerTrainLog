#ifndef __TRAININGBALANCE__
#define __TRAININGBALANCE__

#include "userProfile.h"
#include "graphPanel.h"
#include "wx/calctrl.h"

class TrainingBalance;

class Trainingbalanceframe : public wxFrame
{
public:
  // -- vars --
  GraphPanel   *graphPanel;

  TrainingBalance *m_parentWindow;
  wxCalendarCtrl *m_startCal, *m_endCal;
  wxTextCtrl *m_acuteTc;
  wxTextCtrl *m_chronicTc;
  wxButton *m_recalcButton;

  // -- funcs --
  Trainingbalanceframe(wxWindow *par, const wxString& title, const wxPoint& pos, const wxSize& size,
			 long style = wxDEFAULT_FRAME_STYLE);
  void OnDateChange(wxCalendarEvent& evt);
  void OnRecalcButton(wxCommandEvent& WXUNUSED(event));

  ~Trainingbalanceframe();
  void OnQuit(wxCommandEvent& WXUNUSED(event));
  void OnAbout(wxCommandEvent& WXUNUSED(event));

  UserProfile *theProfile;

private:

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE();
};

class TrainingBalance : public wxWindow{
  Trainingbalanceframe *frame;
public:
  TrainingBalance(const wxString& title, 
						const wxPoint& pos, const wxSize& size,
						UserProfile*);

  bool CalcBalance(bool repaint);

};
#endif
