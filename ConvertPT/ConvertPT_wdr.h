//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: ConvertPT.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_ConvertPT_H__
#define __WDR_ConvertPT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ConvertPT_wdr.h"
#endif

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/toolbar.h>

// Declare window functions

const int ID_FILENAMELABEL = 10000;
const int ID_FILENAMECTRL = 10001;
const int ID_BROWSEBUTTON = 10002;
const int ID_TEXT = 10003;
const int ID_OUTPUTDIR = 10004;
const int ID_BROWSEOUTPUTDIRBUTTON = 10005;
const int ID_HRLIMIT = 10006;
const int ID_FILELIST = 10007;
const int ID_GAUGE = 10008;
const int ID_CONVERTBUTTON = 10009;
wxSizer *ConvertPanel( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

const int ID_FILEMENU = 10010;
const int ID_HELPMENU = 10011;
wxMenuBar *MyMenuBarFunc();

// Declare toolbar functions

void MyToolBarFunc( wxToolBar *parent );

// Declare bitmap functions

#endif

// End of generated file
