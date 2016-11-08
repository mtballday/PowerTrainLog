#ifndef __POWERPLOTWINDOW__
#define __POWERPLOTWINDOW__

#include "graphPanel.h"

class ppWinFrame : public wxFrame
{
public:
  // ctor(s)
  ppWinFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size,
			 long style = wxDEFAULT_FRAME_STYLE);
  ~ppWinFrame();

  UserProfile *theProfile;

  // event handlers (these functions should _not_ be virtual)
  void OnFileOpen(wxCommandEvent& WXUNUSED(event));

  void OnOpenPP(wxCommandEvent& WXUNUSED(event));
  void OnOpenPolar(wxCommandEvent& WXUNUSED(event)); // gui
  void OnOpenPT(wxCommandEvent& WXUNUSED(event)); // gui
  void OnOpenCT(wxCommandEvent& WXUNUSED(event)); // gui

  void OpenPP(wxString filename);
  void OpenPolar(wxString filename);
  void OpenCT(wxString filename);
  void OpenPT(wxString filename);

  void OnFixPT(wxCommandEvent& event);
  void OnTest(wxCommandEvent& event);
  void OnCalcTSS(wxCommandEvent& WXUNUSED(event));
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void StartHRRepair(wxCommandEvent& WXUNUSED(event));
  
private:
  GraphPanel   *graphPanel;
  wxString     iniFilename;

  double CalcTss(TraceClass *theTrace);
  bool CreateSmooth(TraceClass *origTrace, TraceClass *newTrace,
						  double rollingTime);

  wxString defDir;
  wxString defPtDir;

  int m_filesLoaded;
  bool m_repairingHR;

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE();
};

class PowerPlotWindow{// : public wxDialog{
  ppWinFrame *frame;
public:
  PowerPlotWindow(wxWindow* parent, const wxString& title, 
						const wxPoint& pos, const wxSize& size,
						UserProfile*);
  void OpenPP(wxString filename){
	 frame->OpenPP(filename);
  }
  void OpenPolar(wxString filename){
	 frame->OpenPolar(filename);
  }
  void OpenPT(wxString filename){
	 frame->OpenPT(filename);
  }
  void OpenCT(wxString filename){
	 frame->OpenCT(filename);
  }
};
#endif
