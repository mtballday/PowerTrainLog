// Class to hold records of commonly ridden sections
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "section_record.h"

// ----------------------------------------------------------------------
// Section_Description
Section_Record::Section_Record()
{
    next=NULL;
}

// ----------------------------------------------------------------------
// Section_Record_List
Section_Record_List::Section_Record_List()
{
    m_first_record=NULL;
    m_current_record=NULL;
}

Section_Record_List::~Section_Record_List()
{
    clear_list();
}

void Section_Record_List::clear_list()
{
    m_current_record=m_first_record;
    while(m_current_record!=NULL){
        m_current_record=m_current_record->next;
        delete(m_first_record);
        m_first_record=m_current_record;
    }
    m_first_record=NULL;
    m_current_record=NULL;
}

bool Section_Record_List::add_section()
{
    Section_Record *ptr;
    
    ptr=new Section_Record();
    m_current_record=m_first_record;
    if(m_current_record==NULL){
        m_current_record=ptr;
        m_first_record=ptr;
        return(true);
    }

    while(m_current_record->next!=NULL){
        m_current_record=m_current_record->next;
    }
    m_current_record->next=ptr;
    m_current_record=m_current_record->next;
    return(true);
}

wxDateTime Section_Record_List::date()
{
    if(m_current_record!=NULL)
        return(m_current_record->date);
    else
        return(wxDateTime());
}

wxString Section_Record_List::name()
{
    if(m_current_record!=NULL)
        return(m_current_record->name);
    else
        return(wxString(""));
}

wxString Section_Record_List::description()
{
    if(m_current_record!=NULL)
        return(m_current_record->description);
    else
        return(wxString(""));
}

double Section_Record_List::seconds()
{
    if(m_current_record!=NULL)
        return(m_current_record->seconds);
    else
        return(-1);
}

bool Section_Record_List::set_date(wxDateTime date)
{
    if(m_current_record==NULL) return(false);
    m_current_record->date=date;
    return(true);
}

bool Section_Record_List::set_name(wxString name)
{
    if(m_current_record==NULL) return(false);
    m_current_record->name=name;
    return(true);
}

bool Section_Record_List::set_description(wxString description)
{
    if(m_current_record==NULL) return(false);
    m_current_record->description=description;
    return(true);
}

bool Section_Record_List::set_seconds(double seconds)
{
    if(m_current_record==NULL) return(false);
    m_current_record->seconds=seconds;
    return(true);
}

bool Section_Record_List::first_record()
{
    if(m_first_record==NULL) return(false);
    m_current_record=m_first_record;
    return(true);
}

bool Section_Record_List::next_record()
{
    if(m_current_record->next==NULL) return(false);
    m_current_record=m_current_record->next;
    return(true);
}

bool Section_Record_List::last_record()
{
    if(m_first_record==NULL) return(false);
    m_current_record=m_first_record;
    while(m_current_record->next!=NULL) m_current_record=m_current_record->next;
    return(true);
}
