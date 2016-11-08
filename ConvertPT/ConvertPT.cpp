/////////////////////////////////////////////////////////////////////////////
// Name:        ConvertPT.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ConvertPT.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private headers
#include "ConvertPT.h"
#include "../powertap.h"
#include "../powerfile.h"
// WDR: class implementations

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

// WDR: event table for MyFrame

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_BUTTON( ID_BROWSEBUTTON, MyFrame::OnBrowseButton )
    EVT_BUTTON( ID_CONVERTBUTTON, MyFrame::OnConvertButton )
    EVT_BUTTON( ID_BROWSEOUTPUTDIRBUTTON, MyFrame::OnBrowseOutputDir )
    EVT_BUTTON( wxID_CANCEL, MyFrame::OnClose )
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMyMenuBar();
    
    CreateMyToolBar();
    
    CreateStatusBar(1);
    SetStatusText( wxT("Welcome!") );
    
     
     /*    wxPanel mainPanel(this, -1, wxDefaultPosition, wxDefaultSize,
             wxTAB_TRAVERSAL, "mainPanel");*/
    ConvertPanel(this, true, true );

     // insert main window here
}

void MyFrame::CreateMyMenuBar()
{
    SetMenuBar( MyMenuBarFunc() );
}

void MyFrame::CreateMyToolBar()
{
    wxToolBar *tb = CreateToolBar( wxTB_HORIZONTAL|wxNO_BORDER /* |wxTB_FLAT */ );
    
    MyToolBarFunc( tb );
}

// WDR: handler implementations for MyFrame

void MyFrame::OnClose( wxCommandEvent &event )
{
  Destroy();
}

void MyFrame::OnBrowseOutputDir( wxCommandEvent &event )
{
  wxDirDialog dd(this, "Choose output directory", ""/*default dir*/,
                      wxDD_NEW_DIR_BUTTON);
  if(dd.ShowModal()==wxID_CANCEL) return;

  GetOutputdir()->SetValue(dd.GetPath());
}

void MyFrame::OnConvertButton( wxCommandEvent &event )
{
  long numfiles;
  long jj;
  long progress;

  wxString fname;
  wxString dir;
  wxString outdir;

  wxArrayString fnames;

  numfiles=GetFilelist()->GetCount();
  dir=GetFilenamectrl()->GetValue();
  outdir=GetOutputdir()->GetValue();
  
  for(jj=0;jj<numfiles;jj++){
     fname=GetFilelist()->GetString(jj);
     if(ConvertFile(dir+"/"+fname, outdir+"/pp_"+fname)==false){
         wxMessageBox("Could not convert "+fname, "Error", wxOK);
      }
     progress=100*jj/numfiles;
     GetGauge()->SetValue(progress);
  }
  GetGauge()->SetValue(0);
}

void MyFrame::OnBrowseButton( wxCommandEvent &event )
{
  wxArrayString fnames;
  wxFileDialog fd(this, "Choose a Powertap file", ""/*default dir*/,
                    "" /*default file*/,
                    "PT Files (*.csv)|*.csv|All files (*.*)|*.*",
                    wxFD_OPEN|wxMULTIPLE);
  if(fd.ShowModal()==wxID_CANCEL) return;

  fd.GetFilenames(fnames);

  GetFilelist()->Clear();
  GetFilelist()->InsertItems(fnames,0);

  GetFilenamectrl()->SetValue(fd.GetDirectory());
  /*
  defPtDir=fd.GetDirectory();
  filename=fd.GetPath();
  ptfile.LoadFile(filename);
  ptfile.WriteFile(filename+".old");
  ptfile.RepairHr();
  ptfile.WriteFile(filename);
  */
}

void MyFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("ConvertPT\n(C)opyright Richard Abbott, 2005"),
        wxT("About ConvertPT"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void MyFrame::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void MyFrame::OnCloseWindow( wxCloseEvent &event )
{
    // if ! saved changes -> return
    
    Destroy();
}

bool MyFrame::ConvertFile(wxString ptFilename, wxString powerFilename)
{
  PowerTapFile ptFile;
  PowerTapLineList::Node *oneLine;
  PowerTapLine *dat;
  int temperature;
  Powerfile    powerFile;
  int interval;
  bool intervalInProgress;
  long hrlimit=180;

  if(ptFile.LoadFile(ptFilename)==false){
     return(false);
  }
  
  temperature=0;
  /*   don't know what the temperature should have been 
  temperature=atoi(tempCtrl->GetValue().GetData());
  temperature=(int)(5.0*((double)temperature-32.0)/9.0);
  */

  /* don't know what the startTime should have been either  
  powerFile.startTime=timeCtrl->GetValue();
  */
  GetHrlimit()->GetValue().ToLong(&hrlimit);

  oneLine=ptFile.lineList.GetFirst();
  interval=0;
  intervalInProgress=false;

  while(oneLine!=NULL){
     dat=oneLine->GetData();
     powerFile.Appendline();
     powerFile.SetSeconds(dat->Seconds());
     powerFile.SetWatts(dat->watts);
     if(dat->hr<hrlimit){
        powerFile.SetHR(dat->hr);
     }else{
        powerFile.SetHR(0);
     }
     powerFile.SetCadence(dat->cadence);
     powerFile.SetKPH(dat->kph);
     powerFile.SetDistanceMeters(dat->km*1000.0);
     powerFile.SetAltitudeMeters(0);
     powerFile.SetTempC(temperature);
     powerFile.SetTorque(dat->torque);
     if(dat->id!=interval){
        if(intervalInProgress==true){             // finishing an interval
          powerFile.SetIntervalEnd(dat->Seconds());
          if(dat->id!=0){             // starting directly into new interval
             powerFile.AppendInterval();
             powerFile.SetIntervalStart(dat->Seconds());
          }else{
             intervalInProgress=false;  // ending interval
          }
        }else{                        // Starting new interval
          powerFile.AppendInterval();
          powerFile.SetIntervalStart(dat->Seconds());
          intervalInProgress=true;
        }
     }
     interval=dat->id;
     oneLine=oneLine->GetNext();
  }
  if(intervalInProgress==true){ // never ended last interval
     powerFile.SetIntervalEnd(dat->Seconds());
     intervalInProgress=false;
  }
  if(powerFile.Save(powerFilename)==false){
     return(false);
  }
  return(true);
}

//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( NULL, -1, wxT("ConvertPT"), wxPoint(20,20), wxSize(1000,800) );
    frame->Show( TRUE );
    
    return TRUE;
}

int MyApp::OnExit()
{
    return 0;
}

