// TimePeriod
// TimeSpan plus a few new functions

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include"timePeriod.h"

TimePeriod::TimePeriod()
{
  seconds=0;
}

TimePeriod::TimePeriod(double secs)
{
  seconds=secs;
}
TimePeriod::TimePeriod(wxString timeStr)
{
  long tmp;
  int ii;

  long hours;
  long minutes=0;
  seconds=0.0;
  // hh:mm:ss.hh
  ii=timeStr.Find(":");
  if(ii<0){							  // these are just seconds
	 timeStr.ToDouble(&seconds);
	 return;
  }
  // at least minutes
  timeStr.Left(ii).ToLong(&tmp);
  timeStr=timeStr.Mid(ii+1);
  ii=timeStr.Find(":");
  if(ii<0){							  // minutes and seconds
	 minutes=tmp;
	 timeStr.ToDouble(&seconds);
	 seconds+=(double)minutes*60;
	 return;
  }
  hours=tmp;
  timeStr.Left(ii).ToLong(&minutes);
  timeStr=timeStr.Mid(ii+1);
  timeStr.ToDouble(&seconds);
  seconds+=(double)hours*3600.0+(double)minutes*60.0;
  return;
}

void TimePeriod::Set(double secs)
{
  seconds=secs;
}

wxString TimePeriod::SetByHHMMSSHH(wxString timeStr)
{
  long tmp;
  int ii;

  long hours;
  long minutes=0;
  seconds=0.0;
  // hh:mm:ss.hh
  ii=timeStr.Find(":");
  if(ii<0){							  // these are just seconds
	 timeStr.ToDouble(&seconds);
	 return("");
  }
  // at least minutes
  timeStr.Left(ii).ToLong(&tmp);
  timeStr=timeStr.Mid(ii+1);
  ii=timeStr.Find(":");
  if(ii<0){							  // minutes and seconds
	 minutes=tmp;
	 timeStr.ToDouble(&seconds);
	 seconds+=(double)minutes*60;
	 return("");
  }
  hours=tmp;
  timeStr.Left(ii).ToLong(&minutes);
  timeStr=timeStr.Mid(ii+1);
  timeStr.ToDouble(&seconds);
  seconds+=(double)hours*3600.0+(double)minutes*60.0;
  return(HHMMSSHH());
}

wxString TimePeriod::SetByHHMM(wxString timeStr)
{
  long tmp;
  int ii;

  long hours;
  long minutes=0;
  seconds=0.0;
  // hh:mm
  ii=timeStr.Find(":");
  if(ii<0){							  // these are just seconds
	 timeStr.ToDouble(&seconds);
	 seconds*=3600.0;
	 return(HHMMSSHH());
  }
  timeStr.Left(ii).ToLong(&hours);
  timeStr=timeStr.Mid(ii+1);
  timeStr.ToDouble(&seconds);
  seconds*=60.0;
  seconds+=hours*3600.0;
  return(HHMMSSHH());
}

wxString TimePeriod::SSHH()
{
  return(wxString::Format("%3.2lf", seconds));
}

wxString TimePeriod::HHMMSSHH()
{
  wxString theTime;
  long hours;
  long minutes;
  long secs;
  long hunds;

  hours=(long)seconds/3600;
  minutes=(long)seconds%3600/60;
  secs=(long)seconds-hours*3600-minutes*60;
  hunds=(long)((seconds*100.0)+.0000001-100*(double)(hours*3600+minutes*60+secs));

  theTime=wxString::Format("%02ld:%02ld:%02ld.%02ld",
									hours, minutes, secs, hunds);
  return(theTime);
}

wxString TimePeriod::HHMM(double theSeconds)
{
  wxString theTime;
  long hours;
  long minutes;
  long secs;
  long hunds;

  hours=(long)theSeconds/3600;
  minutes=(long)theSeconds%3600/60;
  theTime=wxString::Format("%02ld:%02ld", hours, minutes);
  return(theTime);
}

wxString TimePeriod::HHMM()
{
  wxString theTime;
  long hours;
  long minutes;
  long secs;
  long hunds;

  hours=(long)seconds/3600;
  minutes=(long)seconds%3600/60;
  theTime=wxString::Format("%02ld:%02ld",
									hours, minutes);
  return(theTime);
}

double TimePeriod::Seconds()
{
  return(seconds);
}

wxString TimePeriod::HHMMSSHH(double inSecs)
{
  wxString theTime;
  long hours;
  long minutes;
  long secs;
  long hunds;

  hours=(long)inSecs/3600;
  minutes=(long)inSecs%3600/60;
  secs=(long)inSecs-hours*3600-minutes*60;
  hunds=(long)((inSecs*100.0)+.0000001-100*(double)(hours*3600+minutes*60+secs));

  theTime=wxString::Format("%02ld:%02ld:%02ld.%02ld",
									hours, minutes, secs, hunds);
  return(theTime);
}

bool TimePeriod::operator>(TimePeriod tp)
{
  if(seconds>tp.seconds) return(true); else return(false);
}
bool TimePeriod::operator>=(TimePeriod tp)
{
  if(seconds>=tp.seconds) return(true); else return(false);
}
bool TimePeriod::operator<(TimePeriod tp)
{
  if(seconds<tp.seconds) return(true); else return(false);
}
bool TimePeriod::operator<=(TimePeriod tp)
{
  if(seconds<=tp.seconds) return(true); else return(false);
}
TimePeriod TimePeriod::operator+(TimePeriod tp)
{
  return(TimePeriod(seconds+tp.seconds));
}
