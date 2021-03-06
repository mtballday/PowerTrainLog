//------------------------------------------------------------------------------
// Source code generated by wxDesigner from file: ConvertPT.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ConvertPT_wdr.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private header
#include "ConvertPT_wdr.h"


// Euro sign hack of the year
#if wxUSE_UNICODE
    #define __WDR_EURO__ wxT("\u20ac")
#else
    #if defined(__WXMAC__)
        #define __WDR_EURO__ wxT("\xdb")
    #elif defined(__WXMSW__)
        #define __WDR_EURO__ wxT("\x80")
    #else
        #define __WDR_EURO__ wxT("\xa4")
    #endif
#endif

// Implement window functions

wxSizer *ConvertPanel( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item2 = new wxStaticText( parent, ID_FILENAMELABEL, wxT("Input Dir"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_FILENAMECTRL, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxButton *item4 = new wxButton( parent, ID_BROWSEBUTTON, wxT("&Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("&Output Dir"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTER, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_OUTPUTDIR, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item5->Add( item7, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxButton *item8 = new wxButton( parent, ID_BROWSEOUTPUTDIRBUTTON, wxT("Bro&wse"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxALIGN_CENTER, 5 );

    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item10 = new wxStaticText( parent, ID_TEXT, wxT("Upper HR Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxTextCtrl *item11 = new wxTextCtrl( parent, ID_HRLIMIT, wxT("185"), wxDefaultPosition, wxSize(80,-1), 0 );
    item9->Add( item11, 1, wxALIGN_CENTER, 5 );

    item0->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxString *strs12 = (wxString*) NULL;
    wxListBox *item12 = new wxListBox( parent, ID_FILELIST, wxDefaultPosition, wxDefaultSize, 0, strs12, wxLB_SINGLE|wxLB_SORT );
    item0->Add( item12, 2, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxGauge *item13 = new wxGauge( parent, ID_GAUGE, 100, wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item13, 1, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, wxID_CANCEL, wxT("C&lose"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    wxButton *item16 = new wxButton( parent, ID_CONVERTBUTTON, wxT("&Convert"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->SetDefault();
    item14->Add( item16, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

// Implement menubar functions

wxMenuBar *MyMenuBarFunc()
{
    wxMenuBar *item0 = new wxMenuBar;
    
    wxMenu* item1 = new wxMenu;
    item1->Append( wxID_EXIT, wxT("Quit"), wxT("") );
    item0->Append( item1, wxT("&File") );
    
    wxMenu* item2 = new wxMenu;
    item2->Append( wxID_ABOUT, wxT("&About"), wxT("") );
    item0->Append( item2, wxT("&Help") );
    
    return item0;
}

// Implement toolbar functions

void MyToolBarFunc( wxToolBar *parent )
{
    parent->SetMargins( 2, 2 );
    
    
    parent->Realize();
}

// Implement bitmap functions


// End of generated file
