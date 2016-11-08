#ifndef __HRREPAIRCLASS__
#define __HRREPAIRCLASS__

#include <wx/calctrl.h>
#include <wx/filename.h>

#include "graphPanel.h"
#include "computrainer.h"
#include "powerfile.h"
#include "userProfile.h"

#define LEFTPOINTER  0
#define RIGHTPOINTER 1

class HrRepairDialog : public wxDialog
{

 private:
  // -- Controls --
  GraphPanel *m_graphPanel;

  long m_leftPosition;
  long m_rightPosition;

  wxStaticText *m_leftPositionCtrl, *m_leftHR;
  wxStaticText *m_rightPositionCtrl, *m_rightHR;

  wxButton *m_left_previousButton;
  wxButton *m_left_leftButton;
  wxButton *m_left_rightButton;
  wxButton *m_left_nextButton;

  wxButton *m_repairButton;

  wxButton *m_right_previousButton;
  wxButton *m_right_leftButton;
  wxButton *m_right_rightButton;
  wxButton *m_right_nextButton;

  wxButton *m_okButton, *m_cancelButton, *m_revertButton;

  // -- Member Vars --
  Powerfile m_powerfile;

public:
  
  HrRepairDialog(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size);

  bool ConvertFile(wxString ctFilename, wxString powerFilename);

  wxFileName m_filename;

  // -- Handlers --
  void OnOk(wxCommandEvent& WXUNUSED(event));

  void OnLeftLeftButton(wxCommandEvent& WXUNUSED(event));
  void OnLeftRightButton(wxCommandEvent& WXUNUSED(event));
  void OnRightRightButton(wxCommandEvent& WXUNUSED(event));
  void OnRightLeftButton(wxCommandEvent& WXUNUSED(event));

  void OnRepair(wxCommandEvent& WXUNUSED(event));

  void OnLeftNextButton(wxCommandEvent& WXUNUSED(event));
  void OnRightNextButton(wxCommandEvent& WXUNUSED(event));

  void OnLeftPreviousButton(wxCommandEvent& WXUNUSED(event));
  void OnRightPreviousButton(wxCommandEvent& WXUNUSED(event));

  void Extrapolate();

  void OnPPBrowse(wxCommandEvent& WXUNUSED(event));

  // -- Functions --
  bool SetFilename(wxFileName fn);
  bool PPBrowse(wxString defname);
  void Plot();
  void SetPointers();

  DECLARE_EVENT_TABLE()
};

#endif
