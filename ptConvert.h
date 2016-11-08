#ifndef __PTCONVERTCLASS__
#define __PTCONVERTCLASS__

#include <wx/calctrl.h>
#include "powertap.h"
#include "powerfile.h"
#include "userProfile.h"


class PtConvert : public wxDialog
{

  wxCalendarCtrl   *dateCtrl;
  wxTextCtrl   *timeCtrl;
  wxTextCtrl   *tempCtrl;
  wxTextCtrl   *powertapCtrl;
  wxTextCtrl   *powerplanCtrl;
  wxButton     *temperatureButton;

  UserProfile *m_currentProfile;

  wxDateTime m_lastDateClicked;
  int m_sessionNumber;

public:
  
  PtConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  bool ConvertFile(wxString ptFilename, wxString powerFilename);

  void OnDate(wxCalendarEvent& WXUNUSED(event));
  void OnPTBrowse(wxCommandEvent& WXUNUSED(event));
  void OnPPBrowse(wxCommandEvent& WXUNUSED(event));
  void OnDownload(wxCommandEvent& WXUNUSED(event));
  void OnConvert(wxCommandEvent& WXUNUSED(event));
  void OnTempButton(wxCommandEvent& WXUNUSED(event));

  bool PTBrowse(wxString defname);
  bool PPBrowse(wxString defname);

  bool GetTemperature();

  DECLARE_EVENT_TABLE()
};

#endif
