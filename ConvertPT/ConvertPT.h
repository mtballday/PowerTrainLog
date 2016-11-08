/////////////////////////////////////////////////////////////////////////////
// Name:        ConvertPT.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   
/////////////////////////////////////////////////////////////////////////////

#ifndef __ConvertPT_H__
#define __ConvertPT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ConvertPT.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "ConvertPT_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
  bool ConvertFile(wxString ptFilename, wxString powerFilename);

public:
    // constructors and destructors
    MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    
private:
    // WDR: method declarations for MyFrame
    wxTextCtrl* GetOutputdir()  { return (wxTextCtrl*) FindWindow( ID_OUTPUTDIR ); }
    wxTextCtrl* GetHrlimit()  { return (wxTextCtrl*) FindWindow( ID_HRLIMIT ); }
    wxGauge* GetGauge()  { return (wxGauge*) FindWindow( ID_GAUGE ); }
    wxListBox* GetFilelist()  { return (wxListBox*) FindWindow( ID_FILELIST ); }
    wxTextCtrl* GetFilenamectrl()  { return (wxTextCtrl*) FindWindow( ID_FILENAMECTRL ); }
    void CreateMyMenuBar();
    void CreateMyToolBar();
    
private:
    // WDR: member variable declarations for MyFrame
    
private:
    // WDR: handler declarations for MyFrame
    void OnClose( wxCommandEvent &event );
    void OnBrowseOutputDir( wxCommandEvent &event );
    void OnConvertButton( wxCommandEvent &event );
    void OnBrowseButton( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    
private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    MyApp();
    
    virtual bool OnInit();
    virtual int OnExit();
};

#endif
