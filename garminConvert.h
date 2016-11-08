#ifndef __GARMINCONVERTCLASS__
#define __GARMINCONVERTCLASS__

#include <wx/calctrl.h>
#include "garmin.h"
#include "powerfile.h"
#include "userProfile.h"


class GarminConvert : public wxDialog
{

  wxCalendarCtrl   *dateCtrl;
  wxTextCtrl   *tempCtrl;
  wxTextCtrl   *garminCtrl;
  wxTextCtrl   *powerplanCtrl;
  wxCheckBox   *dropZ0Ctrl;
  wxStaticText *sessionNumberCtrl;
  wxButton     *temperatureButton;

  UserProfile *m_currentProfile;

  int m_converted;

  wxString m_lastDateString;

public:
  
  GarminConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  bool ConvertFile(wxString grFilename, wxString powerFilename, int session);

  void OnDate(wxCalendarEvent& WXUNUSED(event));
  void OnGarminBrowse(wxCommandEvent& WXUNUSED(event));
  void OnPPBrowse(wxCommandEvent& WXUNUSED(event));
  void OnConvert(wxCommandEvent& WXUNUSED(event));
  void OnTempButton(wxCommandEvent& WXUNUSED(event));

  bool GarminBrowse(wxString defname);
  bool PPBrowse(wxString defname);

  bool GetTemperature();

  DECLARE_EVENT_TABLE()
};

#endif
