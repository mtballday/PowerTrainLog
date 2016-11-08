#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "interval.h"
#include"garmin.h"
#include"wxReadTextFile.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(GarminLineList); // used to load a powertap file

GarminLine::GarminLine()				  // constructor
{
  minutes=0; torque=0;
  kph=0;
  km=0;
  watts=0;
  hr=0;
  rpm=0;
}

GarminLine::GarminLine(double mins, long i, long h,
                       double altitudeMeters, double kperhr,
                       double longitude, double latitude,
                       double meters, double wattsread, double rpm_cadence)
{
  minutes=mins;
  hr=h; id=i;
  altitude_meters=altitudeMeters;
  km=meters/1000.0;
  kph=kperhr;
  watts=wattsread;
  rpm=rpm_cadence;
}

double GarminLine::Mph()					  // return the reading in mph
{
  return(0.6214*kph);
}

double GarminLine::Miles()
{
  return(0.6214*km);
}

double GarminLine::Seconds()
{
  return(60.0*minutes);
}

double GarminLine::Watts()
{
  return(watts);
}

double GarminLine::Kph()
{
  return(kph);
}

double GarminLine::Torque()
{
  return(0);
}

double GarminLine::Meters()
{
  return(km*1000.0);
}

double GarminLine::Altitude_Meters()
{
  return(altitude_meters);
}

double GarminLine::Rpm_Cadence()
{
  return(rpm);
}

// ----------------------------------------------------------------------
// GarminFile
// ----------------------------------------------------------------------
GarminFile::GarminFile()
{
  lineList.DeleteContents(true);  
  filteredList.DeleteContents(true);  
  m_tss=0; m_if=0;
  totalKm=0;
  m_totalJoules=0;
  m_totalHeartBeats=0;
}

wxString GarminFile::ReadTag(wxString line, wxString tag)
{
  wxString value;

  int starttagopen = line.Find("<"+tag);
  if(starttagopen<0) return("");    // no tag found
  line=line.Mid(starttagopen);
  int starttagclose= line.Find('>');
  line=line.Mid(starttagclose+1);

  int endtagopen   = line.Find(wxString("</"+tag));

  if(endtagopen>0){                     // end tag is on same line
    value=line.Left(endtagopen);
    return(value);
  }
}

bool GarminFile::LoadFile(wxString fn, wxDateTime ridedate, long dropZ0point, int session)
{
  wxDateTime    timestamp;
  wxDateTime    starttime;
  wxDateTime    tempTime;
  wxDateTime    lasttime;
  wxTimeSpan    elapsed;
  long          hrate;
  double        watts;
  double        rpm;
  double        altitude;
  double        kph;
  double        meters;
  double        lastMeters;
  double        mins;
  double        longitude;
  double        latitude;
  long          id=0;
  double        timeoffset;
  int           sessionsFound;

  double	      dtmp;
  long ii,jj;
  // bool ok;
  wxString tString, inText;
  wxString delthis;
  //  Interval *interval=NULL;
  //  int intervalCount=0;
  //  double intervalReadings;		  // used for measuring average values
  //  double wattsSum;				  // used for measuring average power
  //  double hrSum;					  // used for measuring average hr
  //  int maxHr;
  //  int minHr;
  //  int maxWatts;
  //  int minWatts;

  totalKm=0;  // every line has a new distance measurement
  if(session<1) session=1;

  filename=fn;
  wxReadTextFile fi(fn);
  if(fi.Ready()==false) return(false);	// file wasn't there.
  if(!fi.ReadLine(&inText)) return(false);	// read the leader line
  intervalList.DeleteAll();
  id=0;
  timeoffset=0;
  sessionsFound=0;
  while(fi.Ready() && sessionsFound<session){
    fi.ReadLine(&inText);
    ii=inText.Find("<Activity "); // find the activity (space excludes activityref)
    if(ii>-1){
      fi.ReadLine(&inText);
      ii=inText.Find("<Id>"); // check the date
      if(ii>-1){
        inText=inText.Mid(ii+4); // pull out the tag
        inText.Replace("T", " ");
        jj=inText.Find("Z");
        inText=inText.Left(jj);
        tempTime.ParseDateTime(inText.Left(jj));
        tempTime=tempTime.FromTimezone(wxDateTime::GMT0);
        delthis=tempTime.FormatISODate()+" "+tempTime.FormatISOTime();

        if(tempTime.IsSameDate(ridedate)){
          if(fi.Ready()) sessionsFound++;
        }
      }
    }
  }
  // -- Find the start of the first lap
  do{
    fi.ReadLine(&inText);
    ii=inText.Find("<Lap StartTime=");
    if(ii>-1){
      inText=inText.Mid(ii+16); // strip off the tag
      inText.Replace("T", " ");
      jj=inText.Find("Z");
      tempTime.ParseDateTime(inText.Left(jj));
      tempTime=tempTime.FromTimezone(wxDateTime::GMT0);
      delthis=tempTime.FormatISODate()+" "+tempTime.FormatISOTime();
    }
  }while(fi.Ready() && !((ii>-1) && (tempTime.IsSameDate(ridedate)))); // ii>-1 ensures the tempTime has been set
  if(!fi.Ready()) return(false);

  starttime=tempTime;           // get the time for the start of the ride
  m_starttime=starttime;
  lasttime=starttime;

  fi.ReadLine(&inText);
  while(fi.Ready()){
    while(fi.Ready() && inText.Find("<Trackpoint>")<0){
      if(inText.Find("<Lap StartTime")>-1) id++; // found the next lap
      if(inText.Find("</Run>")>-1 ||
         inText.Find("</Activity")>-1){ // done reading
        fi.Close();             // \todo: Fix this
      }
      fi.ReadLine(&inText); // find a trackpoint
    }

    watts=0;
    rpm=0;
    meters=-1;
    altitude=-1;
    hrate=-1;
    longitude=-1;
    latitude=-1;
    kph=0;
    while(fi.Ready() && (inText.Find("</Trackpoint>"))<0){ // get trackpoint data
      fi.ReadLine(&inText);
      if((ii=inText.Find("<Time>"))>-1){ // read in the time
        inText=inText.Mid(ii+6);
        inText.Replace("T", " ");
        ii=inText.Find("Z");
        timestamp.ParseDateTime(inText.Left(ii));
        timestamp=timestamp.FromTimezone(wxDateTime::GMT0);
      }
      if((ii=inText.Find("<Position>"))>-1){ // -- position --
        while(fi.Ready() && (inText.Find("</Position>")<0)){ // read position lines
          fi.ReadLine(&inText);
          if((ii=inText.Find("<LatitudeDegrees>"))>-1){
            inText=inText.Mid(ii+17);// parse latitude
            ii=inText.Find("</LatitudeDegrees>");
            inText=inText.Left(ii);
            inText.ToDouble(&latitude);
          }
          if((ii=inText.Find("<LongitudeDegrees>"))>-1){
            inText=inText.Mid(ii+18);// parse longitude
            ii=inText.Find("</LongitudeDegrees>");
            inText=inText.Left(ii);
            inText.ToDouble(&longitude);
          }
        }
      }
      if(ReadTag(inText, "Watts").ToDouble(&dtmp))     watts=dtmp;
      if(ReadTag(inText, "ns3:Watts").ToDouble(&dtmp)) watts=dtmp;

      /*
      if((ii=inText.Find("<Watts>"))>-1){
        inText=inText.Mid(ii+7);// parse watts
        ii=inText.Find("</Watts>");
        inText=inText.Left(ii);
        inText.ToDouble(&watts);
      }

      if((ii=inText.Find("<AltitudeMeters>"))>-1){
        inText=inText.Mid(ii+16);// parse altitude
        ii=inText.Find("</AltitudeMeters>");
        inText=inText.Left(ii);
        inText.ToDouble(&altitude);
      }
      */
      if(ReadTag(inText, "ns3:Speed").ToDouble(&dtmp))      kph=dtmp*3.6;
      if(ReadTag(inText, "Cadence").ToDouble(&dtmp))        rpm=dtmp;

      if(ReadTag(inText, "AltitudeMeters").ToDouble(&dtmp)) altitude=dtmp;
      
      if((ii=inText.Find("<DistanceMeters>"))>-1){
        inText=inText.Mid(ii+16);// parse distance
        ii=inText.Find("</DistanceMeters>");
        inText=inText.Left(ii);
        inText.ToDouble(&meters);
        if((meters-lastMeters)>500){ // filter noisy distances
          meters=lastMeters;
        }else{
          lastMeters=meters;
        }
      }
      ii=inText.Find("<HeartRateBpm xsi");
      jj=inText.Find("<HeartRateBpm>");
      if(jj>ii) ii=jj;
      if(ii>-1){ // -- heart rate --
        inText=inText.Mid(ii+14);// parse hr
        ii=inText.Find("</HeartRateBpm>"); // data is on same line
        if(ii>-1){
          inText=inText.Left(ii);
          inText.ToLong(&hrate);
        }else{                  // data is split over multiple lines
          while(fi.Ready() && (inText.Find("</HeartRateBpm>")<0)){ // read position lines
            fi.ReadLine(&inText);
            if((ii=inText.Find("<Value>"))>-1){
              inText=inText.Mid(ii+7);// parse hr
              ii=inText.Find("</Value>");
              inText=inText.Left(ii);
              inText.ToLong(&hrate);
            }
          }
        }
      }
    }
	 if((hrate>-1) || (altitude>-1)){ // have a reading
      if(dropZ0point>0 && hrate<dropZ0point){
        elapsed=timestamp-lasttime;
        mins=(double)(elapsed.GetMilliseconds().ToLong())/60000.0;
        timeoffset+=mins;
      }else{
        elapsed=timestamp-starttime;
        mins=(double)(elapsed.GetMilliseconds().ToLong())/60000.0-timeoffset;
        lineList.Append(new GarminLine(mins, id, hrate,
                                       altitude, kph, longitude, latitude, meters, watts, rpm));
      }
      lasttime=timestamp;
    }
  }
  return(true);
}

double GarminFile::TotalMiles()
{
  return(0.6214*totalKm);
}

double GarminFile::TotalKm()
{
  return(totalKm);
}

bool GarminFile::RepairHr()
{
  GarminLineList::Node *startLine;
  GarminLineList::Node *endLine;

  int ii;
  int starty, endy;
  double slope;
  
  startLine=lineList.GetFirst();
  while(startLine!=NULL){
	 while(startLine!=NULL && startLine->GetData()->hr>0){
		startLine=startLine->GetNext();
	 }
	 if(startLine!=NULL){		  // found one with a 0
		endLine=startLine->GetNext();
		startLine=startLine->GetPrevious();
		ii=1;

		while(endLine!=NULL && endLine->GetData()->hr==0){ // move to no more 0's
		  ii++;
		  endLine=endLine->GetNext();
		}
		if(endLine==NULL)return(false);
		starty=startLine->GetData()->hr;
		endy=endLine->GetData()->hr;
		slope=((double)endy-(double)starty)/(double)ii;
		ii=0;
		startLine=startLine->GetNext();
		do{
		  ii++;
		  startLine->GetData()->hr=(int)((double)starty+slope*(double)ii+.00001);
		  startLine=startLine->GetNext();
		}while(startLine!=endLine);
	 }	// done correcting the 0
  }
  return(true);
}

double GarminFile::ZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;

  GarminLineList::Node *currentLine;

  currentLine=lineList.GetFirst();
  GarminLine *ptl;
  if(currentLine==NULL) return(false);
  ptl=currentLine->GetData();
  lastTime=ptl->minutes*60.0;
  currentLine=currentLine->GetNext();
  while(currentLine!=NULL){
	 ptl=currentLine->GetData();
	 thisTime=ptl->minutes*60.0;
	 if(ptl->hr>=lower && ptl->hr<=higher)
		totalTime+=thisTime-lastTime;
	 lastTime=thisTime;
	 currentLine=currentLine->GetNext();
  }
  return(totalTime);
}

/*
// GarminFile::Filter()
// average is seconds to average
void GarminFile::Filter(double average)
{
  double total=0;

  GarminLineList::Node *xyNode;
  GarminLineList::Node *startNode;
  GarminLineList::Node *endNode;
  GarminLineList::Node *origNode;

  double halfWidth;
  long pointCount;

  //  numPoints=theTrace->average;
  halfWidth=average/2.0;

  origNode=lineList.GetFirst();

  xyNode=filteredList.GetFirst();
  startNode=lineList.GetFirst();
  endNode=lineList.GetFirst();

  total=endNode->GetData()->watts;  // prime total with first number

  pointCount=1;
  while(xyNode!=NULL){			  // xyNode has the node we're counting to
	 *xyNode->GetData()=*origNode->GetData();
	 while(endNode->GetNext()!=NULL &&
			 endNode->GetNext()->GetData()->Seconds() <= xyNode->GetData()->Seconds()+halfWidth){ // add end points
		endNode=endNode->GetNext();
		total+=endNode->GetData()->watts;
		pointCount++;
	 }
	 while(startNode!=NULL && startNode->GetData()->Seconds() <= xyNode->GetData()->Seconds()-halfWidth){ // remove beginnings
		total-=startNode->GetData()->watts;
		pointCount--;
		startNode=startNode->GetNext();
	 }

	 xyNode->GetData()->watts=(int)(total/(double)(pointCount));
	 xyNode=xyNode->GetNext();
	 origNode=origNode->GetNext();
  }
}

double GarminFile::TSS(double pLT)
{
  GarminLineList::Node *xyNode;
  double sum, watts;
  double IF, normWork, rawTSS, normTSS;
  long count;
  double joules;
  double lastTime;				  // used to see how many seconds...
  double time;						  // between recordings
  double totalHeartBeats;		  // total heart beats on file

  //  FILE *fout;

  Filter(30);						  // filter based on 30s rolling average

  xyNode=filteredList.GetFirst();
  
  sum=0; count=0;					  // find mean of 4th power filtered
  joules=0;
  lastTime=0;
  totalHeartBeats=0;

  while(xyNode!=NULL){			  // xyNode has the node we're counting to
	 watts=xyNode->GetData()->watts;
	 time=xyNode->GetData()->Seconds();
	 joules+=watts*(time-lastTime);
	 totalHeartBeats+=(double)xyNode->GetData()->hr/60.0*(time-lastTime);
	 lastTime=time;
	 sum+=pow(watts, 4);
	 count++;
	 xyNode=xyNode->GetNext();
  }
  sum=pow(sum/(double)count,.25);		  // Step 4: take fourth root of mean
  m_np=sum;
  IF=sum/pLT;						  // Step 5: IF
  normWork=sum*filteredList.GetLast()->GetData()->Seconds();	// Step 6: Norm Work
  rawTSS=IF*normWork;
  normTSS=rawTSS/(3600.0*pLT)*100.0;
  m_if=IF;
  m_tss=sum;
  m_totalJoules=joules;
  m_totalHeartBeats=(long)totalHeartBeats;
  return(normTSS);
}
*/
// ----------------------------------------------------------------------
// Sections
/// Returns the Powertap interval with index ii

/// \param[in] ii: index of desired interval
// ----------------------------------------------------------------------

Interval GarminFile::Sections(int ii)
{
  return(intervalList.Index(ii));
}
