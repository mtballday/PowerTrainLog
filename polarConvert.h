#ifndef __POLARCONVERTCLASS__
#define __POLARCONVERTCLASS__

#include <wx/calctrl.h>
#include "polar.h"
#include "powerfile.h"
#include "userProfile.h"


class PolarConvert : public wxDialog
{

  wxCalendarCtrl   *dateCtrl;
  wxTextCtrl   *timeCtrl;
  wxTextCtrl   *tempCtrl;
  wxTextCtrl   *polarCtrl;
  wxTextCtrl   *powerplanCtrl;
  wxCheckBox   *dropZ0Ctrl;

  UserProfile *m_currentProfile;

  int m_converted;

  wxString m_lastDateString;

public:
  
  PolarConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  bool ConvertFile(wxString ptFilename, wxString powerFilename);

  void OnDate(wxCalendarEvent& WXUNUSED(event));
  void OnPolarBrowse(wxCommandEvent& WXUNUSED(event));
  void OnPPBrowse(wxCommandEvent& WXUNUSED(event));
  void OnConvert(wxCommandEvent& WXUNUSED(event));

  bool PolarBrowse(wxString defname);
  bool PPBrowse(wxString defname);



  DECLARE_EVENT_TABLE()
};

#endif
