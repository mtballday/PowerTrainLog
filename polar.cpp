#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"timePeriod.h"
#include "interval.h"

#include"polar.h"
#include"wxReadTextFile.h"

#include <wx/listimpl.cpp>
//WX_DEFINE_LIST(IntervalList);
WX_DEFINE_LIST(PolarLineList); // used to load a powertap file

PolarLine::PolarLine()				  // constructor
{
  hr=0;
  seconds=0;
  speed=0;
  cadence=0;
}

PolarLine::PolarLine(long sec, int hh, double spd, int cad)
{
  seconds=sec;
  hr=hh;
  speed=spd;
  cadence=cad;
}

double PolarLine::Kph()
{
  return(speed*1.609344);		  // return reading kph
}

double PolarLine::Mph()					  // return the reading in mph
{
  return(speed);
}

double PolarLine::Watts()		  // don't have this right now
{
  return(0);
}

double PolarLine::Meters()		  // doesn't work right now
{
  return(0);
}

double PolarLine::Torque()		  // doesn't work right now
{
  return(0);
}

/*
  double PolarLine::Miles()
  {
  //  return(0.6214*km);
  return(0);						  // this needs to be figured out
  }
*/
double PolarLine::Seconds()
{
  return(seconds);
}

// ----------------------------------------------------------------------
// PolarFile
// ----------------------------------------------------------------------
PolarFile::PolarFile()
{
  totalMiles=0;
  m_totalBeats=0;
  lineList.DeleteContents(true);  
}

bool PolarFile::LoadFile(wxString fn, long dropZ0point)
{
  bool readingHeader;
  double speed;
  int cadence, hr;
  int tInt;
  int samplePeriod;				  /// Time between reads of the HR Monitor
  bool ok;
  bool readSpeed=false;
  bool readCadence=false;
  wxString inText;
  wxString tTxt;
  int ii;
  long totalTime;
  double totalBeats;				  // used for total beats count

  TimePeriod length;				  /// total time as read from the polar unit
  TimePeriod intTime;			  /// the time that the interval was recorded
  Interval *interval; /// pointer for entries in the interval list 
  int intervalCount;				  /// Tracks the number of the interval

  filename=fn;
  wxReadTextFile fi(fn);

  intervalList.DeleteAll();			  // delete any intervals already loaded
  intervalCount=0;

  readingHeader=true;
  do{
	 if(fi.ReadLine(&inText)==true){
		headerInfo+=inText+"\n";
	 }
	 if(inText.Left(6)==("Length")){ // read the total time
		length.SetByHHMMSSHH(inText.Mid(7));
	 }
  }while(fi.Ready() && inText.Left(10)!=("[IntTimes]"));

  fi.ReadLine(&inText);

  while(inText.Find(":")>-1){
	 // intervals read here
	 headerInfo+=inText+"\n";
	 intTime.SetByHHMMSSHH(inText); // get the interval time
	 intervalCount++;

	 if(intervalList.m_firstInterval==NULL){ // first interval
		intervalList.Append(Interval(0, intTime.Seconds(), "Interval 1"));
	 }else{
		intervalList.Append(Interval(interval->endTime, intTime.Seconds(),
											  wxString::Format("Interval %d", intervalCount)));
	 }
	 interval=intervalList.m_currentInterval;
	 fi.ReadLine(&inText);
	 headerInfo+=inText+"\n";
	 fi.ReadLine(&inText);
	 headerInfo+=inText+"\n";
	 fi.ReadLine(&inText);
	 headerInfo+=inText+"\n";
  }
  if(intervalCount>0 && length.Seconds()>0){	// if using intervals, add one last one for the
	                                          // end of the file
	 intervalCount++;
	 intervalList.Append(Interval(interval->endTime, length.Seconds(),
												 wxString::Format("Interval %d", intervalCount)));
  }

  do{
	 if(fi.ReadLine(&inText)){
		headerInfo+=inText+"\n";

		if(inText.Find("[Trip]")>-1){	  // distance was measured
		  if(fi.ReadLine(&inText)){
			 inText.ToDouble(&totalMiles); // this probably won't work if KM
			 totalMiles/=10.0;
			 headerInfo+=inText+"\n";
		  }
		}
	 }
  }while(fi.Ready() && inText.Left(8)!=("[HRData]"));
  if(!fi.Ready()) return(false);	// premature end
  ii=headerInfo.Find("Mode=");
  ii+=5;
  if(headerInfo.Mid(ii,3)=="011"){ // read speed and cadence
	 readCadence=true;
	 readSpeed=true;
  }else if(headerInfo.Mid(ii,3)=="311"){ // hr and speed
	 readSpeed=true;
  }else if(headerInfo.Mid(ii,3)=="310"){ // hr and speed
	 readSpeed=true;
  }else if(headerInfo.Mid(ii,3)=="001"){ // just HR
	 ;									  // don't do anything
  }else if(headerInfo.Mid(ii,3)=="301"){// just HR
	 ;									  // don't do anything
  }else{
	 wxMessageBox("Don't understand HR mode. Will still attempt", "Error", wxOK);
  }
  ii=headerInfo.Find("Interval="); // read the samplePeriod between reads
  samplePeriod=atoi(headerInfo.Mid(ii+9).GetData());
  if(samplePeriod<1){
	 wxMessageBox("Can't read samplePeriod", "Error", wxOK);
	 return(false);
  }

  totalTime=0;
  speed=0;
  cadence=0;
  totalBeats=0;
  while(fi.Ready()){				  // read in the file
	 ok=true;
	 if(!fi.ReadInt(&hr)) ok=false;
	 if(readSpeed==true){		  // read speed
		if(fi.ReadInt(&tInt)==false){
		  ok=false;
		}else{
		  speed=(double)tInt/10.0;
		}
	 }
	 if(readCadence==true){		  // read cadence
		if(fi.ReadInt(&cadence)==false){
		  ok=false;
		}
	 }
	 if(ok==true && hr>=dropZ0point){ // Z0 points are dropped here if desired. This will not agree with stuff above
      totalBeats+=(double)hr*samplePeriod/60.0;
		lineList.Append(new PolarLine(totalTime, hr, speed, cadence));
		totalTime+=samplePeriod;
	 }
  }
  m_totalBeats=(long)totalBeats;
  return(true);
}


// ----------------------------------------------------------------------
// Sections
/// Returns the interval with index ii

/// \param[in] ii: index of desired interval
// ----------------------------------------------------------------------

Interval PolarFile::Sections(int ii)
{
  Interval var;

  var=intervalList.Index(ii);
  return(var);
}

bool PolarFile::RepairHr()
{
  PolarLineList::Node *startLine;
  PolarLineList::Node *endLine;

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

bool PolarFile::WriteFile(wxString fn)
{
  /*
  FILE *fp;
  fp=fopen(fn, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write repaired file", "Error", wxOK);
	 return(false);
  }
  PolarLineList::Node *startLine;
  
  startLine=lineList.GetFirst();
  PolarLine *ptl;
  fprintf(fp, " Minutes, Torq (N-m),  Km/h, Watts,      Km, Cadence, Hrate,  ID\r\n");
  while(startLine!=NULL){
	 ptl=startLine->GetData();
	 fprintf(fp, "%8.3lf,%11.1lf,%6.1lf,%6d,%8.3lf,%8d,%6d,%4d\r\n",
				ptl->minutes, ptl->torque, ptl->kph, ptl->watts,
				ptl->km, ptl->cadence, ptl->hr, ptl->id);
	 startLine=startLine->GetNext();
  }  
  fclose(fp);
  */  return(true);
}

double PolarFile::TotalMiles()
{
  return(totalMiles);
}

double PolarFile::TotalKm()
{
  return(totalMiles*1.609344);
}

double PolarFile::ZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;


  PolarLineList::Node *currentLine;

  currentLine=lineList.GetFirst();
  PolarLine *ptl;
  if(currentLine==NULL) return(false);
  ptl=currentLine->GetData();
  lastTime=ptl->Seconds();
  currentLine=currentLine->GetNext();
  while(currentLine!=NULL){
	 ptl=currentLine->GetData();
	 thisTime=ptl->Seconds();
	 if(ptl->hr>=lower && ptl->hr<=higher)
		totalTime+=thisTime-lastTime;
	 lastTime=thisTime;
	 currentLine=currentLine->GetNext();
  }
  return(totalTime);
}
