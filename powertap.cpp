#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "interval.h"
#include"powertap.h"
#include"wxReadTextFile.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(PowerTapLineList); // used to load a powertap file

PowerTapLine::PowerTapLine()				  // constructor
{
  minutes=0; torque=0;
  kph=0;
  watts=0;
  km=0;
}

PowerTapLine::PowerTapLine(double min, double torq, double kp,
									int watt, double k, 
									int cad, int h, int i)
{
  minutes=min; torque=torq;
  kph=kp;
  watts=watt;
  km=k;
  cadence=cad;
  hr=h; id=i;
}

double PowerTapLine::Mph()					  // return the reading in mph
{
  return(0.6214*kph);
}

double PowerTapLine::Miles()
{
  return(0.6214*km);
}

double PowerTapLine::Seconds()
{
  return(60.0*minutes);
}

// ----------------------------------------------------------------------
// PowerTapFile
// ----------------------------------------------------------------------
PowerTapFile::PowerTapFile()
{
  lineList.DeleteContents(true);  
  filteredList.DeleteContents(true);  
  m_tss=0; m_if=0;
  totalKm=0;
  m_totalJoules=0;
  m_totalHeartBeats=0;
}

bool PowerTapFile::LoadFile(wxString fn)
{
  double minutes, torque, kmh, km;
  int watts, cadence, hrate, id;
  bool ok;
  wxString inText;
  Interval *interval=NULL;
  int intervalCount=0;
  double intervalReadings;		  // used for measuring average values
  double wattsSum;				  // used for measuring average power
  double hrSum;					  // used for measuring average hr
  int maxHr;
  int minHr;
  int maxWatts;
  int minWatts;

  totalKm=0;  // every line has a new distance measurement

  filename=fn;
  wxReadTextFile fi(fn);
  if(fi.Ready()==false) return(false);	// file wasn't there.
  if(!fi.ReadLine(&inText)) return(false);	// read the leader line
  intervalList.DeleteAll();
  while(fi.Ready()){				  // read in the file
	 ok=true;
	 if(!fi.ReadDouble(&minutes)) ok=false;
	 if(!fi.ReadDouble(&torque)) ok=false;
	 if(!fi.ReadDouble(&kmh)) ok=false;
	 if(!fi.ReadInt(&watts)) ok=false;
	 if(!fi.ReadDouble(&km)) ok=false;
	 if(!fi.ReadInt(&cadence)) ok=false;
	 if(!fi.ReadInt(&hrate)) ok=false;
	 if(!fi.ReadInt(&id)) ok=false;
	 if(ok==true){
		lineList.Append(new PowerTapLine(minutes, torque, kmh, watts, km,
													cadence, hrate, id));
		filteredList.Append(new PowerTapLine(minutes, torque, kmh, watts, km,
														 cadence, hrate, id));
		if(km!=0){
		  totalKm=km;
		}

		if(intervalReadings>0){	  // currently figuring interval statistics
		  intervalReadings++;
		  wattsSum+=watts;
		  hrSum+=hrate;
		  if(hrate>maxHr) maxHr=hrate;
		  if(hrate<minHr) minHr=hrate;
		  if(watts>maxWatts) maxWatts=watts;
		  if(watts<minWatts) minWatts=watts;
		}
		if(id!=0 && interval==NULL){ // start a new interval
		  intervalCount++;
		  interval=new Interval();
		  interval->startTime=minutes*60.0;
		  interval->note=wxString::Format("Interval %d", intervalCount);
		  intervalReadings=1;
		  wattsSum=watts;
		  hrSum=hrate;
		  maxWatts=minWatts=watts;
		  maxHr=minHr=hrate;
		}else if(id==0 && interval!=NULL){ // finish the interval
		  interval->endTime=minutes*60.0;
		  interval->avgWatts=wattsSum/intervalReadings;
		  interval->averageHr=(int)(hrSum/intervalReadings);
		  interval->maxWatts=maxWatts;
		  interval->minWatts=minWatts;
		  interval->maxHr=maxHr;
		  interval->minHr=minHr;
		  intervalList.Append(*interval);
		  delete(interval);
		  interval=NULL;
		}
	 }
  }
  if(interval!=NULL){			  // never finished last interval
	 interval->endTime=minutes*60.0;
	 interval->avgWatts=wattsSum/intervalReadings;
	 interval->averageHr=(int)(hrSum/intervalReadings);
	 interval->maxWatts=maxWatts;
	 interval->minWatts=minWatts;
	 interval->maxHr=maxHr;
	 interval->minHr=minHr;
	 intervalList.Append(*interval);
	 delete(interval);
	 interval=NULL;
  }
  return(true);
}

double PowerTapFile::TotalMiles()
{
  return(0.6214*totalKm);
}

double PowerTapFile::TotalKm()
{
  return(totalKm);
}

bool PowerTapFile::RepairHr()
{
  PowerTapLineList::Node *startLine;
  PowerTapLineList::Node *endLine;

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

bool PowerTapFile::WriteFile(wxString fn)
{
  FILE *fp;
  fp=fopen(fn, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write repaired file", "Error", wxOK);
	 return(false);
  }
  PowerTapLineList::Node *startLine;
  
  startLine=lineList.GetFirst();
  PowerTapLine *ptl;
  fprintf(fp, " Minutes, Torq (N-m),  Km/h, Watts,      Km, Cadence, Hrate,  ID\r\n");
  while(startLine!=NULL){
	 ptl=startLine->GetData();
	 fprintf(fp, "%8.3lf,%11.1lf,%6.1lf,%6d,%8.3lf,%8d,%6d,%4d\r\n",
				ptl->minutes, ptl->torque, ptl->kph, ptl->watts,
				ptl->km, ptl->cadence, ptl->hr, ptl->id);
	 startLine=startLine->GetNext();
  }  
  fclose(fp);
  return(true);
}

double PowerTapFile::ZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;

  PowerTapLineList::Node *currentLine;

  currentLine=lineList.GetFirst();
  PowerTapLine *ptl;
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

// PowerTapFile::Filter()
// average is seconds to average
void PowerTapFile::Filter(double average)
{
  double total=0;

  PowerTapLineList::Node *xyNode;
  PowerTapLineList::Node *startNode;
  PowerTapLineList::Node *endNode;
  PowerTapLineList::Node *origNode;

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

double PowerTapFile::TSS(double pLT)
{
  PowerTapLineList::Node *xyNode;
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

// ----------------------------------------------------------------------
// Sections
/// Returns the Powertap interval with index ii

/// \param[in] ii: index of desired interval
// ----------------------------------------------------------------------

Interval PowerTapFile::Sections(int ii)
{
  return(intervalList.Index(ii));
}
