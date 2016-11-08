#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include"powerfile.h"

PowerInterval& PowerInterval::operator=(PowerInterval pl){
  starttime=pl.starttime;
  endtime=pl.endtime;
  return(*this);
}


/// Default constructor
Powerline::Powerline()
{
  seconds=0;
  watts=0.0;
  hr_bpm=0;
  cadence_rpm=0; 
  speed_kph=0;
  distance_meters=0;
  altitude_meters=0;
  degrees_c=0;
  torque_nm=0;

  next=NULL;
  previous=NULL;
}

Powerline& Powerline::operator=(Powerline pl)
{
  seconds=pl.seconds;
  watts=pl.watts;
  hr_bpm=pl.hr_bpm;
  cadence_rpm=pl.cadence_rpm;
  speed_kph=pl.speed_kph;
  distance_meters=pl.distance_meters;
  altitude_meters=pl.altitude_meters;
  degrees_c=pl.degrees_c;
  torque_nm=pl.torque_nm;
  return(*this);
}


/// Initialize and append to previous entry
/*!
  Can be used to quickly fill a list
*/
Powerline::Powerline(Powerline *prev)
{
  next=NULL;
  previous=NULL;

  if(prev!=NULL){
	 prev->next=this;
	 previous=prev;
  }
  seconds=0;
  watts=0.0;
  hr_bpm=0;
  cadence_rpm=0;
  speed_kph=0;
  distance_meters=0;
  altitude_meters=0;
  degrees_c=0;
  torque_nm=0;
}


Powerfile::Powerfile()
{
  m_firstPowerline=NULL;
  m_currentLine=NULL;
  m_firstInterval=NULL;
  m_currentInterval=NULL;
  m_firstFilteredline=NULL;
  m_currentFilteredline=NULL;
  m_np=0;
  m_if=0;
  m_tss=0;
  m_trimps=0;
  m_totalJoules=0;
  m_totalHeartBeats=0;
  m_factoredHR=0;
}

Powerfile::~Powerfile()
{
  DeleteAll();
}

void Powerfile::DeleteAll()
{
  m_currentLine=m_firstPowerline;
  while(m_currentLine!=NULL){
	 m_firstPowerline=m_currentLine;
	 m_currentLine=m_currentLine->next;
	 delete(m_firstPowerline);
  }
  m_firstPowerline=NULL;
  m_currentLine=NULL;

  m_currentInterval=m_firstInterval;
  while(m_currentInterval!=NULL){
	 m_firstInterval=m_currentInterval;
	 m_currentInterval=m_currentInterval->next;
	 delete(m_firstInterval);
  }
  m_firstInterval=NULL;
  m_currentInterval=NULL;

  m_currentFilteredline=m_firstFilteredline;
  while(m_currentFilteredline!=NULL){
	 m_firstFilteredline=m_currentFilteredline;
	 m_currentFilteredline=m_currentFilteredline->next;
	 delete(m_firstFilteredline);
  }
  m_firstFilteredline=NULL;
  m_currentFilteredline=NULL;
}

// ----------------------------------------------------------------------
/// Set to first Interval
void Powerfile::FirstInterval()
{
  m_currentInterval=m_firstInterval;
}

bool Powerfile::NextInterval()
{
  if(m_currentInterval!=NULL){
	 m_currentInterval=m_currentInterval->next;
  }
  if(m_currentInterval==NULL){
	 return(false);
  }
  return(true);
}

// ----------------------------------------------------------------------
// AppendInterval
/// Append a Interval
bool Powerfile::AppendInterval()
{
  if(m_currentInterval==NULL){
	 m_currentInterval=m_firstInterval;
  }
  if(m_currentInterval==NULL){		  // first line is NULL
	 m_firstInterval=new PowerInterval();
	 m_currentInterval=m_firstInterval;
  }else{
	 while(m_currentInterval->next!=NULL){
		m_currentInterval=m_currentInterval->next;
	 }
	 m_currentInterval->next=new PowerInterval();
	 m_currentInterval=m_currentInterval->next;
  }
  if(m_currentInterval==NULL) return(false);
  return(true);
}

PowerInterval Powerfile::CurrentInterval()
{
  if(m_currentInterval!=NULL) return(*m_currentInterval);
  else return(PowerInterval());
}

bool Powerfile::AppendInterval(PowerInterval interval)
{
  if(AppendInterval()==false) return(false);
  *m_currentInterval=interval;
  return(true);
}

/// Return the start time in seconds
double Powerfile::IntervalStartSeconds()
{
  if(m_currentInterval==NULL) return(0);
  return(m_currentInterval->starttime);
}

/// Return the end time is seconds
double Powerfile::IntervalEndSeconds()
{
  if(m_currentInterval==NULL) return(0);
  return(m_currentInterval->endtime);
}

bool Powerfile::SetIntervalStart(double seconds)
{
  if(m_currentInterval==NULL) return(false);
  m_currentInterval->starttime=seconds;
  return(true);
}

bool Powerfile::SetIntervalEnd(double seconds)
{
  if(m_currentInterval==NULL) return(false);
  m_currentInterval->endtime=seconds;
  return(true);
}

// ----------------------------------------------------------------------
// Firstline
/// Set the current line to the first line
void Powerfile::Firstline()
{
  m_currentLine=m_firstPowerline;
}

// ----------------------------------------------------------------------
// Nextline
/// Set the current line to the next line
bool Powerfile::Nextline()
{
  if(m_currentLine!=NULL){
	 m_currentLine=m_currentLine->next;
  }
  if(m_currentLine==NULL){
	 return(false);
  }
  return(true);
}

// ----------------------------------------------------------------------
/// Return the data for the current line
Powerline Powerfile::Currentline()
{
  return(*m_currentLine);
}


// ----------------------------------------------------------------------
// Appendline
/// Append a line to the end of the file and set the current line to the new line
bool Powerfile::Appendline()
{
  if(m_currentLine==NULL){
	 m_currentLine=m_firstPowerline;
  }
  if(m_currentLine==NULL){		  // first line is NULL
	 m_firstPowerline=new Powerline();
	 m_currentLine=m_firstPowerline;
  }else{
	 while(m_currentLine->next!=NULL){
		m_currentLine=m_currentLine->next;
	 }
	 m_currentLine=new Powerline(m_currentLine);
  }
  if(m_currentLine==NULL) return(false);

  // -- Filtered Line --
  if(m_currentFilteredline==NULL){
	 m_currentFilteredline=m_firstFilteredline;
  }
  if(m_currentFilteredline==NULL){		  // first line is NULL
	 m_firstFilteredline=new Powerline();
	 m_currentFilteredline=m_firstFilteredline;
  }else{
	 while(m_currentFilteredline->next!=NULL){
		m_currentFilteredline=m_currentFilteredline->next;
	 }
	 m_currentFilteredline=new Powerline(m_currentFilteredline);
  }
  if(m_currentFilteredline==NULL) return(false);

  return(true);
}

// ----------------------------------------------------------------------
// Appendline
/// Append a line to the end of the file
/// and set the current line to the new line with data
bool Powerfile::Appendline(Powerline pl)
{
  if(Appendline()==false) return(false);
  *m_currentLine=pl;
  return(true);
}

// ----------------------------------------------------------------------
// `FixSpeedFromDistance
// If the speeds are all 0 but there is distance data, use that to calc the speed
bool Powerfile::FixSpeedFromDistance(){
  Powerline   *lineIterator;
  bool        hasSpeedData=false;

  lineIterator=m_firstPowerline;
  while(lineIterator!=NULL){
    if(lineIterator->speed_kph>0.0) hasSpeedData=true;
    lineIterator=lineIterator->next;
  }
  if(hasSpeedData==true) return(false); // already has data, so do nothing

  lineIterator=m_firstPowerline;
  lineIterator=lineIterator->next;

  while(lineIterator!=NULL){
    lineIterator->speed_kph=((lineIterator->distance_meters - lineIterator->previous->distance_meters)/
      (lineIterator->seconds - lineIterator->previous->seconds))*3.6; // m/s
                                                                   // converted
                                                                   // to kph
    lineIterator=lineIterator->next;
  }
  return(true);
}


// ----------------------------------------------------------------------
// SetSeconds
/// Set the seconds of the current line
void Powerfile::SetSeconds(double secs)
{
  if(m_currentLine!=NULL){
	 m_currentLine->seconds=secs;
	 m_currentFilteredline->seconds=secs;
  }
}

// ----------------------------------------------------------------------
// SetWatts
/// Set the watts for the current line
void Powerfile::SetWatts(double w)
{
  if(m_currentLine!=NULL){
	 m_currentLine->watts=w;
	 m_currentFilteredline->watts=w;
  }
}

// ----------------------------------------------------------------------
// SetHR
/// Set the watts for the current line
void Powerfile::SetHR(unsigned int hr)
{
  if(m_currentLine!=NULL){
	 m_currentLine->hr_bpm=hr;
	 m_currentFilteredline->hr_bpm=hr;
  }
}

// ----------------------------------------------------------------------
// SetCadence
/// Set the watts for the current line
void Powerfile::SetCadence(unsigned char cad)
{
  if(m_currentLine!=NULL){
	 m_currentLine->cadence_rpm=cad;
	 m_currentFilteredline->cadence_rpm=cad;
  }
}

// ----------------------------------------------------------------------
// SetKPH
/// Set the speed in KPH for current line
void Powerfile::SetKPH(double kph)
{
  if(m_currentLine!=NULL){
	 m_currentLine->speed_kph=kph;
	 m_currentFilteredline->speed_kph=kph;
  }
}

// ----------------------------------------------------------------------
// SetDistanceMeters
/// Set the distance in meters for current line
void Powerfile::SetDistanceMeters(double m)
{
  if(m_currentLine!=NULL){
	 m_currentLine->distance_meters=m;
	 m_currentFilteredline->distance_meters=m;
  }
}

// ----------------------------------------------------------------------
// SetAltitudeMeters
/// Set the altitude in meters
void Powerfile::SetAltitudeMeters(double m)
{
  if(m_currentLine!=NULL){
	 m_currentLine->altitude_meters=m;
	 m_currentFilteredline->altitude_meters=m;
  }
}

// ----------------------------------------------------------------------
// SetTempC
/// Set the temperature in C for current line
void Powerfile::SetTempC(int deg)
{
  if(m_currentLine!=NULL){
	 m_currentLine->degrees_c=deg;
	 m_currentFilteredline->degrees_c=deg;
  }
}

// ----------------------------------------------------------------------
// SetTorque
/// Set torque for current line. Units are an issue for this one
void Powerfile::SetTorque(double tq)
{
  if(m_currentLine!=NULL){
	 m_currentLine->torque_nm=tq;
	 m_currentFilteredline->torque_nm=tq;
  }
}

// ----------------------------------------------------------------------
// SaveFile
/// Save the file
bool Powerfile::Save(wxString fn)
{
  PowerInterval *currentInterval;
  Powerline *currentLine;
  FILE *fp;

  if(fn.IsEmpty()) return(false);

  currentLine=m_firstPowerline;
  if(m_firstPowerline==NULL) return(false); // don't save if no data

  fp=fopen(fn, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write Powerplan file", "Error", wxOK);
	 return(false);
  }
  fprintf(fp, "%% <starttime>%s\n", startTime.ToUTF8().data());
  currentInterval=m_firstInterval;
  while(currentInterval!=NULL){
	 fprintf(fp, "%% <int>%6.2lf, %6.2lf\n", currentInterval->starttime, currentInterval->endtime);
	 currentInterval=currentInterval->next;
  }

  fprintf(fp, "%% Seconds, Power, HR, Cad,Speed(kph), Dist(m),Alt(m),Temp(C),Torq(nm)\n");
  while(currentLine!=NULL){
	 fprintf(fp, "%8.2lf,%7.1lf,%3d,%4d, %9.2lf,%8.1lf,%6.1lf,%7d,%7.3lf\n",
				currentLine->seconds, currentLine->watts, currentLine->hr_bpm,
				currentLine->cadence_rpm, currentLine->speed_kph, currentLine->distance_meters,
				currentLine->altitude_meters, currentLine->degrees_c, currentLine->torque_nm);
	 currentLine=currentLine->next;
  }  
  fclose(fp);
  return(true);
}

bool Powerfile::OnALine()
{
  if(m_currentLine==NULL) return(false);
  return(true);
}


// ----------------------------------------------------------------------
// LoadFile
bool Powerfile::Load(wxString fn)
{
  double secs, watt;
  int hrate, cadence;
  double kmh, meters, alt, torq, tDouble;
  int degs;
  
  bool ok;
  wxString inText;
  int startmarker, endmarker;
  int tInt;
  char ltr;

  m_totalMeters=0;
  meters=0;  // every line has a new distance measurement

  DeleteAll();

  m_filename=fn;
  wxReadTextFile fi(fn);
  if(fi.Ready()==false) return(false);	// file wasn't there.

  meters=0;
  while(fi.Ready()){				  // read in the file
	 ltr=fi.Peek();
	 if(ltr=='%'){	  // comment line
		ok=true;
		fi.ReadLine(&inText);

		startmarker=inText.Find("<");
		endmarker=inText.Find(">");
		if(endmarker>-1) endmarker++;
		if(inText.Find("<starttime>")>-1){
		  startTime=inText.Mid(endmarker);
		}else if(inText.Find("<int>")>-1){
		  AppendInterval();
		  SetIntervalStart(atoi(inText.Mid(endmarker).GetData()));
		  tInt=inText.Find(",")+1;
		  if(tInt>-1) SetIntervalEnd(atoi(inText.Mid(tInt).GetData()));
		}
	 }else{
		ok=true;
		if(!fi.ReadDouble(&secs)) ok=false;
		if(!fi.ReadDouble(&watt)) ok=false;
		if(!fi.ReadInt(&hrate)) ok=false;
		if(!fi.ReadInt(&cadence)) ok=false;
		if(!fi.ReadDouble(&kmh)) ok=false;
		if(!fi.ReadDouble(&meters)) ok=false;
		if(!fi.ReadDouble(&alt)) ok=false;
		if(!fi.ReadDouble(&tDouble)) ok=false;
    degs=(int)tDouble;
		if(!fi.ReadDouble(&torq)) ok=false;
    if(fi.lastChar!='\n') while(fi.Ready() && fi.ReadChar()!='\n');

		if(ok==true){
		  Appendline();
		  SetSeconds(secs);
		  SetWatts(watt);
		  SetHR(hrate);
		  SetCadence(cadence);
		  SetKPH(kmh);
		  SetDistanceMeters(meters);
		  SetAltitudeMeters(alt);
		  SetTempC(degs);
		  SetTorque(torq);
		}
	 }
  }
  if(meters!=0){
	 m_totalMeters=meters;
  }

  return(true);
}


void Powerfile::Filter(double average)
{
  double total=0;

  Powerline *start;
  Powerline *current;
  Powerline *end;

  Powerline *currentFiltered;

  double starttime, endtime;

  double halfWidth;
  long count;

  //  numPoints=theTrace->average;
  halfWidth=average/2.0;
  count=1;

  currentFiltered=m_firstFilteredline;
  current=m_firstPowerline;

  start=current;
  end=current;
  
  count=0;
  total=0;

  while(current!=NULL && currentFiltered!=NULL){
	 starttime=current->seconds-halfWidth;
	 endtime  =current->seconds+halfWidth;

	 while(start!=NULL && start->seconds<starttime){
		total-=start->watts;
		count--;
		start=start->next;
	 }

	 while(end!=NULL && end->seconds<endtime){
		total+=end->watts;
		count++;
		end=end->next;
	 }
	 currentFiltered->watts=total/count;
	 current=current->next;
	 currentFiltered=currentFiltered->next;
  }
}

double Powerfile::TSS(double pLT)
{
  Powerline *filtLinePtr;
  double sum, watts;
  double IF, normWork, rawTSS, normTSS;
  long count;
  double joules;
  double lastTime;				  // used to see how many seconds...
  double time;						  // between recordings
  double totalHeartBeats;		  // total heart beats on file
  double factoredHR;
  double a1=0.00573149;
  double b1=0.0409794;

  //  FILE *fout;

  Filter(30);						  // filter based on 30s rolling average

  filtLinePtr=m_firstFilteredline;
  
  sum=0; count=0;					  // find mean of 4th power filtered
  joules=0;
  lastTime=0;
  totalHeartBeats=0;
  time=0;
  factoredHR=0;

  while(filtLinePtr!=NULL){
	 watts=filtLinePtr->watts;
	 time=filtLinePtr->seconds;
	 joules+=watts*(time-lastTime);
	 factoredHR+=a1*exp(b1*(double)filtLinePtr->hr_bpm)*(time-lastTime);
	 totalHeartBeats+=(double)filtLinePtr->hr_bpm/60.0*(time-lastTime);
	 lastTime=time;
	 sum+=pow(watts, 4);
	 count++;
	 filtLinePtr=filtLinePtr->next;
  }
  sum=pow(sum/(double)count,.25);		  // Step 4: take fourth root of mean
  m_np=sum;
  IF=sum/pLT;						  // Step 5: IF
  normWork=sum*time;	// Step 6: Norm Work
  rawTSS=IF*normWork;
  normTSS=rawTSS/(3600.0*pLT)*100.0;
  m_if=IF;
  m_tss=sum;
  m_totalJoules=joules;
  m_totalHeartBeats=(long)totalHeartBeats;
  m_factoredHR=factoredHR;
  return(normTSS);
}

// ----------------------------------------------------------------------
/// Return the distance of the current line
double Powerfile::CurrentDistance(int units)
{
  if(m_currentLine==NULL) return(0);
  if(units==UNITS_MILES) return(m_currentLine->distance_meters/1609.344);
  if(units==UNITS_METERS) return(m_currentLine->distance_meters);
  return(m_currentLine->distance_meters/1000.0);
}

// ----------------------------------------------------------------------
/// Return the distance of the entire file
double Powerfile::Distance(int units)
{
  if(units==UNITS_MILES) return(m_totalMeters/1609.344);
  if(units==UNITS_METERS) return(m_totalMeters);
  return(m_totalMeters/1000.0);
}

double Powerfile::Seconds()
{
  if(m_currentLine!=NULL) return(m_currentLine->seconds);
  return(0);
}

double Powerfile::ZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;

  Firstline();

  if(OnALine()==false) return(false);
  lastTime=Currentline().seconds;
  Nextline();
  while(OnALine()==true){
	 thisTime=Currentline().seconds;
	 if(Currentline().hr_bpm>=lower && Currentline().hr_bpm<=higher)
		totalTime+=thisTime-lastTime;
	 lastTime=thisTime;
	 Nextline();
  }
  return(totalTime);
}

double Powerfile::Trimps(int restHr, int maxHr)
{
  double BB, hr;
  double lastTime, thisTime;

  m_trimps=0;

  Firstline();
  
  if(OnALine()==false) return(false);
  lastTime=Currentline().seconds;
  Nextline();
  while(OnALine()==true){
	 thisTime=Currentline().seconds;

	 hr=Currentline().hr_bpm;
	 if(hr<maxHr){
		BB=(maxHr-hr)/(maxHr-restHr);
		m_trimps+=(double)(thisTime-lastTime)*BB*0.64*exp(1.92*BB)/28.0;
	 }
	 lastTime=thisTime;
	 Nextline();
  }
  return(m_trimps);
}

/*
double Powerfile::PowerZoneTime(int lower, int higher)
{
  double totalTime=0, lastTime, thisTime;

  Filter(10);

  Firstline();

  if(OnALine()==false) return(false);
  lastTime=Currentline().seconds;
  Nextline();
  while(OnALine()==true){
	 thisTime=Currentline().seconds;
	 if(Currentline().watts>=lower && Currentline().watts<=higher)
		totalTime+=thisTime-lastTime;
	 lastTime=thisTime;
	 Nextline();
  }
  return(totalTime);
}
*/

double Powerfile::PowerZoneTime(int lower, int higher)
{
  double totalTime=0;
  Powerline *filtLinePtr;
  double sum, watts;
  double lastTime;				  // used to see how many seconds...
  double time;						  // between recordings

  Filter(10);						  // filter based on 30s rolling average

  filtLinePtr=m_firstFilteredline;
  
  lastTime=0;
  time=0;

  while(filtLinePtr!=NULL){
	 watts=filtLinePtr->watts;
	 time=filtLinePtr->seconds;

   if(watts>=lower && watts<=higher)
     totalTime+=time-lastTime;

	 lastTime=time;
	 filtLinePtr=filtLinePtr->next;
  }
  return(totalTime);
}
