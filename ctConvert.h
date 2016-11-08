#ifndef __CTCONVERTCLASS__
#define __CTCONVERTCLASS__

#include <wx/calctrl.h>
#include "computrainer.h"
#include "powerfile.h"
#include "userProfile.h"


class CtConvert : public wxDialog
{

  wxCalendarCtrl   *dateCtrl;
  wxTextCtrl   *timeCtrl;
  wxTextCtrl   *tempCtrl;
  wxTextCtrl   *computrainerCtrl;
  wxTextCtrl   *powerplanCtrl;

  UserProfile *m_currentProfile;

public:
  
  CtConvert(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  bool ConvertFile(wxString ctFilename, wxString powerFilename);

  void OnDate(wxCalendarEvent& WXUNUSED(event));
  void OnCTBrowse(wxCommandEvent& WXUNUSED(event));
  void OnPPBrowse(wxCommandEvent& WXUNUSED(event));
  void OnConvert(wxCommandEvent& WXUNUSED(event));

  bool CTBrowse(wxString defname);
  bool PPBrowse(wxString defname);



  DECLARE_EVENT_TABLE()
};

#endif
