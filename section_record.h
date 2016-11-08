#ifndef __SECTIONRECORD__
#define __SECTIONRECORD__

#include<wx/datetime.h>
#include"timePeriod.h"

// ----------------------------------------------------------------------
class Section_Record
{
public:
    wxDateTime          date;
    wxString            name;
    wxString            description;
    double              seconds;
    Section_Record      *next;

    Section_Record();
};

// ----------------------------------------------------------------------
class Section_Record_List
{
private:
    Section_Record      *m_first_record;
    Section_Record      *m_current_record;

public:
    Section_Record_List();
    ~Section_Record_List();
    void clear_list();
    bool add_section();

    wxDateTime date();
    wxString name();
    wxString description();
    double seconds();

    bool set_date(wxDateTime date);
    bool set_name(wxString name);
    bool set_description(wxString description);
    bool set_seconds(double seconds);

    bool first_record();
    bool last_record();
    bool next_record();
};

#endif
