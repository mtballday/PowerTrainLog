#include "wx/wxprec.h"
#include <math.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"computrainer.h"
#include"wxReadTextFile.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ComputrainerLineList); // used to load a computrainer file

ComputrainerLine::ComputrainerLine()				  // constructor
{
  ms=0;
  mph=0; watts=0;
  cad=0; hr=0; ss=0; lss=0; rss=0;
  lpwr=0; rpwr=0; miles=0.0;
  hrf=0; rpmf=0; wind=0; grade=0.0; load=0;
}


ComputrainerLine::ComputrainerLine(double MS, double MPH, int WATTS,
					  int CAD, int HR, int SS, int LSS, int RSS,
					  int LPWR, int RPWR, double MILES,
					  int HRF, int RPMF, int WIND, double GRADE, int LOAD)
{
  ms=MS;
  mph=MPH; watts=WATTS;
  cad=CAD; hr=HR; ss=SS; lss=LSS; rss=RSS;
  lpwr=LPWR; rpwr=RPWR; miles=MILES;
  hrf=HRF; rpmf=RPMF; wind=WIND; grade=GRADE; load=LOAD;
}

double ComputrainerLine::Kph()
{
  return(mph*1.609344);
}

double ComputrainerLine::Mph()					  // return the reading in mph
{
  return(mph);
}

double ComputrainerLine::Km()
{
  return(miles*1.609344);
}

double ComputrainerLine::Miles()
{
  return(miles);
}

double ComputrainerLine::Seconds()
{
  return(ms/1000.0);
}

// ----------------------------------------------------------------------
// ComputrainerFile
// ----------------------------------------------------------------------
ComputrainerFile::ComputrainerFile()
{
  lineList.DeleteContents(true);  
  filteredList.DeleteContents(true);
  m_if=0;
  totalMiles=0;
}

bool ComputrainerFile::LoadFile(wxString fn, double wattsCorrection)
{
  double ms, mph; int watts;
  int cad, hr, ss, lss, rss;
  int lpwr, rpwr;
  double miles;
  int hrf,  rpmf,  wind,  load;
  double grade;

  bool ok;
  wxString inText;
  wxString headerLine;

  wxBusyCursor waitCursor;
  filename=fn;
  wxReadTextFile fi(fn);

  totalMiles=0;
  do{
	 if(!fi.ReadLine(&inText)) return(false);	// read the leader lines
	 if(inText.Find("speed")<0) m_headerText+=inText+"\n";	// this assumes speed
																			// is there
	 // the headings line will not be included in the header
  }while(inText.Find("speed")==-1);	// assume speed is always in there
  readMs=   (inText.Find("ms")==-1)?false:true;
  readMph=  (inText.Find("speed")==-1)?false:true;
  readWatts=(inText.Find("watts")==-1)?false:true;
  readCad=  (inText.Find("rpm")==-1)?false:true;
  readHr  = (inText.Find("hr")==-1)?false:true;
  readSs = (inText.Find(" ss ")==-1)?false:true;
  readLss = (inText.Find("lss")==-1)?false:true;
  readRss = (inText.Find("rss")==-1)?false:true;
  readLpwr = (inText.Find("lpwr")==-1)?false:true;
  readRpwr = (inText.Find("rpwr")==-1)?false:true;
  readMiles = (inText.Find("miles")==-1)?false:true;
  readHrf = (inText.Find("hrf")==-1)?false:true;
  readRpmf = (inText.Find("rpmf")==-1)?false:true;
  readWind = (inText.Find("wind")==-1)?false:true;
  readGrade = (inText.Find("grade")==-1)?false:true;
  readLoad = (inText.Find("load")==-1)?false:true;

  fi.ReadLine(&inText);			  // one last one
  while(fi.Ready()){				  // read in the file
	 ms=mph=0;
	 watts=cad=hr=ss=lss=rss=lpwr=rpwr=0;
	 hrf=rpmf=wind=load=0;
	 miles=grade=0.0;

	 ok=true;
	 if(readMs && !fi.ReadDouble(&ms)) ok=false;
	 if(readMph && !fi.ReadDouble(&mph)) ok=false;
	 if(readWatts && !fi.ReadInt(&watts)) ok=false;
	 if(readCad && !fi.ReadInt(&cad)) ok=false;
	 if(readHr && !fi.ReadInt(&hr)) ok=false;
	 if(readSs && !fi.ReadInt(&ss)) ok=false;
	 if(readLss && !fi.ReadInt(&lss)) ok=false;
	 if(readRss && !fi.ReadInt(&rss)) ok=false;
	 if(readLpwr && !fi.ReadInt(&lpwr)) ok=false;
	 if(readRpwr && !fi.ReadInt(&rpwr)) ok=false;
	 miles=0;
	 if(readMiles && !fi.ReadDouble(&miles)){
		ok=false;
	 }
	 if(miles)totalMiles=miles;
	 if(readHrf  && !fi.ReadInt(&hrf)) ok=false;
	 if(readRpmf && !fi.ReadInt(&rpmf)) ok=false;
	 if(readWind && !fi.ReadInt(&wind)) ok=false;
	 if(readGrade && !fi.ReadDouble(&grade)) ok=false;
	 if(readLoad && !fi.ReadInt(&load)) ok=false;
	 if(ok==true){
		lineList.Append(new ComputrainerLine(ms, mph, (int)(watts*wattsCorrection), cad, hr, ss, lss,
														 rss, lpwr, rpwr, miles, hrf, rpmf, 
														 wind, grade, load));
		filteredList.Append(new ComputrainerLine(ms, mph, (int)(watts*wattsCorrection), cad, hr, ss, lss,
														 rss, lpwr, rpwr, miles, hrf, rpmf, 
															  wind, grade, load));
	 }
  }
  return(true);
}

bool ComputrainerFile::RepairHr()
{
  ComputrainerLineList::Node *startLine;
  ComputrainerLineList::Node *endLine;

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

// not done
/*
bool ComputrainerFile::WriteFile(wxString fn)
{
  FILE *fp;
  fp=fopen(fn, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write repaired file", "Error", wxOK);
	 return(false);
  }
  ComputrainerLineList::Node *startLine;
  
  startLine=lineList.GetFirst();
  ComputrainerLine *ptl;
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
}*/

double ComputrainerFile::ZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;

  ComputrainerLineList::Node *currentLine;

  currentLine=lineList.GetFirst();
  ComputrainerLine *ptl;
  if(currentLine==NULL) return(false);
  ptl=currentLine->GetData();
  lastTime=ptl->ms/1000.0;
  currentLine=currentLine->GetNext();
  while(currentLine!=NULL){
	 ptl=currentLine->GetData();
	 thisTime=ptl->ms/1000.0;
	 if(ptl->hr>=lower && ptl->hr<=higher)
		totalTime+=thisTime-lastTime;
	 lastTime=thisTime;
	 currentLine=currentLine->GetNext();
  }
  return(totalTime);
}

// average is seconds to average
void ComputrainerFile::Filter(double average)
{
  double total=0;

  ComputrainerLineList::Node *xyNode;
  ComputrainerLineList::Node *startNode;
  ComputrainerLineList::Node *endNode;
  ComputrainerLineList::Node *origNode;

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

double ComputrainerFile::TotalMiles()
{
  return(totalMiles);
}

double ComputrainerFile::TotalKm()
{
  return(totalMiles*1.609344);
}

double ComputrainerFile::TSS(double pLT)
{
  ComputrainerLineList::Node *xyNode;
  double sum, watts;
  double IF, normWork, rawTSS, normTSS;
  long count;

  Filter(30);						  // filter based on 30s rolling average

  xyNode=filteredList.GetFirst();
  
  sum=0; count=0;					  // find mean of 4th power filtered
  while(xyNode!=NULL){			  // xyNode has the node we're counting to
	 watts=xyNode->GetData()->watts;
	 sum+=pow(watts, 4);
	 count++;
	 xyNode=xyNode->GetNext();
  }
  sum=pow(sum/(double)count,.25);		  // Step 4: take fourth root of mean
  IF=sum/pLT;						  // Step 5: IF
  m_if=IF;
  normWork=sum*filteredList.GetLast()->GetData()->Seconds();	// Step 6: Norm Work
  rawTSS=IF*normWork;
  normTSS=rawTSS/(3600.0*pLT)*100.0;
  return(normTSS);
}
