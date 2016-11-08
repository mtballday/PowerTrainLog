// interval.cpp
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

// ----------------------------------------------------------------------
/// Interval initializes the member variables for this class.
// ----------------------------------------------------------------------
Interval::Interval()
{
  startTime=0;
  endTime=0;
  averageHr=0;
  maxHr=0;
  minHr=0;

  maxWatts=0;
  minWatts=0;
  avgWatts=0;

  next=NULL;
}

// ----------------------------------------------------------------------
// Interval
/// Initializes with data
/// \param[in] start_time: Time interval started (seconds)
/// \param[in] end_time: Time interval ended (seconds)
/// \param[in] a_note: A note
// ----------------------------------------------------------------------
Interval::Interval(double start_time, double end_time, wxString a_note)
{
  startTime=start_time;
  if(end_time>startTime){		  // don't allow start to be after end
	 endTime=end_time;
  }else{
	 endTime=start_time;
  }
  note=a_note;
  next=NULL;
}

// ----------------------------------------------------------------------
// Duration
/// Member function returns the difference between endTime and startTime
// ----------------------------------------------------------------------
TimePeriod Interval::Duration()
{
  TimePeriod elapsedTime(endTime-startTime);
  return(elapsedTime);
}

// ----------------------------------------------------------------------
// IsEmpty
/// Determines if there is any data in the object
// ----------------------------------------------------------------------
bool Interval::IsEmpty()
{
  if(startTime==0 && endTime==0 && note.IsEmpty()
	  && averageHr==0 && maxHr==0 && minHr==0
	  && maxWatts==0 && minWatts==0 && avgWatts==0){
	 return(true);
  }else{
	 return(false);
  }
}

Interval& Interval::operator=(Interval in)
{
  startTime=in.startTime;
  endTime=in.endTime;
  averageHr=in.averageHr;
  maxHr=in.maxHr;
  minHr=in.minHr;
  maxWatts=in.maxWatts;
  minWatts=in.minWatts;
  avgWatts=in.avgWatts;
  note=in.note;
	 return(*this);
}

IntervalList::IntervalList()
{
  m_firstInterval=NULL;
  m_currentInterval=NULL;
}

IntervalList::~IntervalList()
{
  DeleteAll();
}

void IntervalList::DeleteAll()
{
  m_currentInterval=m_firstInterval;
  while(m_currentInterval!=NULL){
	 m_firstInterval=m_currentInterval;
	 m_currentInterval=m_currentInterval->next;
	 delete(m_firstInterval);
  }	 
}

void IntervalList::First()
{
  m_currentInterval=m_firstInterval;
}

void IntervalList::Append()
{
  if(m_currentInterval==NULL){
	 m_currentInterval=m_firstInterval;
  }
  if(m_currentInterval==NULL){
	 m_currentInterval=new Interval();
	 m_firstInterval=m_currentInterval;
  }else{
	 while(m_currentInterval->next!=NULL) m_currentInterval=m_currentInterval->next;
	 m_currentInterval->next=new Interval();
	 m_currentInterval=m_currentInterval->next;
  }
}

void IntervalList::Append(Interval inv)
{
  Append();
  *m_currentInterval=inv;
}

Interval IntervalList::Index(long index)
{
  long ii=0;
  First();
  while(m_currentInterval!=NULL && ii<index){
	 m_currentInterval=m_currentInterval->next;
	 ii++;
  }
  if(m_currentInterval==NULL) return(Interval()); // return empty if too large
  return(*m_currentInterval);
}
