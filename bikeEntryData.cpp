// dayEntry.cpp
// Class to show a window that displays the data for a given day

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "interval.h"
#include "bikeEntryData.h"

BikeEntryData::BikeEntryData()
{
  weight=0; fat=0; session=0;
  ppTss=0; polarTss=0; ptTss=0; ctTss=0;
  ppIf=0; polarIf=0; ptIf=0; ctIf=0;
  miles=0;

  keyEvent=false;
  sick=false;
  offRoad=false;
  sleepTime=0;
  arriseTime.ParseTime("12:00 AM");
  restHr=0;
  m_priority=0;  					  // 0=none, 1=C, 2=B, 3=A
  m_planIntensity=0;				  // 0=none, 1=Rec, 2=Low, 3=Med, 4=High
  m_mesoCycleWeek=0;
}
