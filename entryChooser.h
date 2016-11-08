#ifndef __ENTRYCHOOSER__
#define __ENTRYCHOOSER__

#include<wx/listctrl.h>
#include<wx/calctrl.h>
#include"userProfile.h"

class EntryChooser:public wxPanel
{
  wxListBox *theList;
  wxCalendarCtrl *calendar;
  UserProfile *currentProfile;

public:
  EntryChooser(wxWindow *parent, wxWindowID id, const wxPoint pos,
					const wxSize size, UserProfile *up);

  void OnInsert(wxCommandEvent& WXUNUSED(event));
  void OnDelete(wxCommandEvent& WXUNUSED(event));
  bool FillList();
  void OnDoubleclick(wxCommandEvent& WXUNUSED(event));

  DECLARE_EVENT_TABLE()
};


#endif
