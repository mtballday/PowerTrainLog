// UserProfile
// Also holds the list for all the data for the user

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include"userProfile.h"
#include"wxReadTextFile.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(EntryList);
WX_DEFINE_LIST(BikeList); // used to load a powertap file

UserProfile::~UserProfile()
{
  delete(mainList);
}

UserProfile::UserProfile()
{
  int ii;
  for(ii=0;ii<NUMZONES;ii++){	  // [0] will always be 0
	 zones[ii]=0;
  }
  for(ii=0;ii<9;ii++){
	 powerZones[ii]=0;
  }

  restHr=0;
  hrlimit=9999;
  thresholdPower=0;
  mtbThresholdPower=0;
  thresholdHR=0;
  hrCutoff=120;
  acuteTc=5;
  chronicTc=40;
  ctCorrection=1.0;
  ptCorrection=1.0;
  polarCorrection=1.0;
  mainList=new EntryList();
  mainList->DeleteContents(true);
  bikeList.DeleteContents(true);
}

// Simple Helper Function
const char *Cast(wxString data){
	return(data.ToUTF8().data());
}

// ----------------------------------------------------------------------
// `Filename
// ----------------------------------------------------------------------
wxString UserProfile::Filename()
{
  return(filename);
}

// ----------------------------------------------------------------------
// `DatabaseFilename
// ----------------------------------------------------------------------
wxString UserProfile::DatabaseFilename()
{
  return(databaseFilename);
}

// ----------------------------------------------------------------------
// `WorkoutFilename
// ----------------------------------------------------------------------
wxString UserProfile::WorkoutFilename()
{
  return(workoutFilename);
}

// load the profile from disk
bool UserProfile::LoadProfile(wxString fn)
{
  int ii,jj;
  long tLong;
  double tDouble;

  filename=fn;
  wxString inTxt;
  wxReadTextFile tf(filename);
  tf.ReadLine(&inTxt);
  bikeList.Clear();
  if(inTxt!="POWERTRAININGLOG10"){
	 return(false);
  }
  while(tf.ReadLine(&inTxt)==true){
	 jj=inTxt.Find('>');
	 jj++;
	 if(inTxt.Left(jj)=="<name>"){
		name=inTxt.Mid(jj);
	 }
	 for(ii=1;ii<NUMZONES;ii++){
		if(inTxt.Left(jj)==wxString::Format("<zone%d>", ii)){
		  if(inTxt.Mid(jj).ToLong(&tLong)==true){
			 zones[ii]=(int)tLong;
			 zones[ii+1]=300;
		  }
		}
	 }
	 if(inTxt.Left(jj)=="<cp02>"){
		inTxt.Mid(jj).ToLong(&powerZones[8]);
	 }
	 if(inTxt.Left(jj)=="<cp1>"){
		inTxt.Mid(jj).ToLong(&powerZones[7]);
	 }
	 if(inTxt.Left(jj)=="<cp6>"){
		inTxt.Mid(jj).ToLong(&powerZones[6]);
	 }
	 if(inTxt.Left(jj)=="<cp12>"){
		inTxt.Mid(jj).ToLong(&powerZones[5]);
	 }
	 if(inTxt.Left(jj)=="<cp30>"){
		inTxt.Mid(jj).ToLong(&powerZones[4]);
	 }
	 if(inTxt.Left(jj)=="<cp60>"){
		inTxt.Mid(jj).ToLong(&powerZones[3]);
	 }
	 if(inTxt.Left(jj)=="<cp90>"){
		inTxt.Mid(jj).ToLong(&powerZones[2]);
	 }
	 if(inTxt.Left(jj)=="<cp180>"){
		inTxt.Mid(jj).ToLong(&powerZones[1]);
	 }
	 if(inTxt.Left(jj)=="<resthr>"){
		restHr=atoi(inTxt.Mid(jj).GetData());
	 }
	 if(inTxt.Left(jj)=="<hrlimit>"){ // will cut off any hr data above this value
		hrlimit=atoi(inTxt.Mid(jj).GetData());
	 }
	 if(inTxt.Left(jj)=="<ctdir>"){
		ctDefDir=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<ptdir>"){
		ptDefDir=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<ppdir>"){
		ppDefDir=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<ppprefix>"){
		ppPrefix=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<polardir>"){
		polarDefDir=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<garminfile>"){
		garminDefFile=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<database>"){
		databaseFilename=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<workout>"){
		workoutFilename=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<shortride>"){
		inTxt.Mid(jj).ToLong(&tLong);
		shortRideTime.Set((double)tLong);
	 }
	 if(inTxt.Left(jj)=="<longride>"){
		inTxt.Mid(jj).ToLong(&tLong);
		longRideTime.Set((double)tLong);
	 }
	 if(inTxt.Left(jj)=="<ctcorrectionfactor>"){
		inTxt.Mid(jj).ToDouble(&tDouble);
		if(tDouble==0) tDouble=1; // 0 is not allowed, and is prob a mistake
		ctCorrection=tDouble;
	 }
	 if(inTxt.Left(jj)=="<ptcorrectionfactor>"){
		inTxt.Mid(jj).ToDouble(&tDouble);
		if(tDouble==0) tDouble=1; // 0 is not allowed, and is prob a mistake
		ptCorrection=tDouble;
	 }
	 if(inTxt.Left(jj)=="<polarcorrectionfactor>"){
		inTxt.Mid(jj).ToDouble(&tDouble);
		if(tDouble==0) tDouble=1; // 0 is not allowed, and is prob a mistake
		polarCorrection=tDouble;
	 }

	 if(inTxt.Left(jj)=="<thresholdpower>"){
		inTxt.Mid(jj).ToLong(&thresholdPower);
		if(thresholdPower<0) thresholdPower=0;
		if(thresholdPower>1000) thresholdPower=0;
	 }
	 if(inTxt.Left(jj)=="<mtbthreshold>"){
		inTxt.Mid(jj).ToLong(&mtbThresholdPower);
		if(mtbThresholdPower<0) mtbThresholdPower=0;
		if(mtbThresholdPower>1000) mtbThresholdPower=0;
	 }
	 if(inTxt.Left(jj)=="<thresholdhr>"){
		inTxt.Mid(jj).ToLong(&thresholdHR);
		if(thresholdHR<0) thresholdHR=0;
		if(thresholdHR>250) thresholdHR=0;
	 }
	 if(inTxt.Left(jj)=="<hrcutoff>"){
		inTxt.Mid(jj).ToLong(&hrCutoff);
		if(hrCutoff<0) hrCutoff=0;
		if(hrCutoff>250) hrCutoff=0;
	 }
	 if(inTxt.Left(jj)=="<acutetc>"){
		inTxt.Mid(jj).ToLong(&acuteTc);
		if(acuteTc<1) acuteTc=1;
	 }
	 if(inTxt.Left(jj)=="<chronictc>"){
		inTxt.Mid(jj).ToLong(&chronicTc);
		if(chronicTc<1) chronicTc=1;
	 }
	 if(inTxt.Left(jj)=="<bike>"){
		bikeList.Append(new wxString(inTxt.Mid(jj)));
	 }
  }
  workoutMenu.LoadWorkouts(workoutFilename);
  return(true);
}

// save profile to disk
bool UserProfile::SaveProfile(wxString filename)
{
  int ii;
  FILE *fp;

  wxBikeListNode *curNode;

  fp=fopen(Cast(filename), "w");
  if(fp==false)return(false);
  fprintf(fp, "POWERTRAININGLOG10\n");
  fprintf(fp, "<name>%s\n", Cast(name));
  for(ii=1;ii<NUMZONES;ii++)
	 fprintf(fp, "<zone%d>%d\n", ii, zones[ii]);
  fprintf(fp, "<resthr>%d\n", restHr);
  fprintf(fp, "<hrlimit>%d\n", hrlimit);
  fprintf(fp, "<ptdir>%s\n", Cast(ptDefDir));
  fprintf(fp, "<ctdir>%s\n", Cast(ctDefDir));
  fprintf(fp, "<polardir>%s\n", Cast(polarDefDir));
  fprintf(fp, "<garminfile>%s\n", Cast(garminDefFile));
  fprintf(fp, "<ppdir>%s\n", Cast(ppDefDir));
  fprintf(fp, "<ppprefix>%s\n", Cast(ppPrefix));

  fprintf(fp, "<database>%s\n", Cast(databaseFilename));
  fprintf(fp, "<workout>%s\n", Cast(workoutFilename));
  fprintf(fp, "<cp02>%ld\n", powerZones[8]);
  fprintf(fp, "<cp1>%ld\n", powerZones[7]);
  fprintf(fp, "<cp6>%ld\n", powerZones[6]);
  fprintf(fp, "<cp12>%ld\n", powerZones[5]);
  fprintf(fp, "<cp30>%ld\n", powerZones[4]);
  fprintf(fp, "<cp60>%ld\n", powerZones[3]);
  fprintf(fp, "<cp90>%ld\n", powerZones[2]);
  fprintf(fp, "<cp180>%ld\n", powerZones[1]);

  fprintf(fp, "<ctcorrectionfactor>%lf\n", ctCorrection);
  fprintf(fp, "<ptcorrectionfactor>%lf\n", ptCorrection);
  fprintf(fp, "<polarcorrectionfactor>%lf\n", polarCorrection);

  fprintf(fp, "<shortride>%ld\n", (long)shortRideTime.Seconds());
  fprintf(fp, "<longride>%ld\n", (long)longRideTime.Seconds());
  
  fprintf(fp, "<thresholdpower>%ld\n", thresholdPower);
  fprintf(fp, "<mtbthreshold>%ld\n", mtbThresholdPower);
  fprintf(fp, "<thresholdhr>%ld\n", thresholdHR);
  fprintf(fp, "<hrcutoff>%ld\n", hrCutoff);
  fprintf(fp, "<acutetc>%ld\n", acuteTc);
  fprintf(fp, "<chronictc>%ld\n", chronicTc);

  curNode=bikeList.GetFirst();
  while(curNode!=NULL){
	 fprintf(fp, "<bike>%s\n", curNode->GetData()->ToUTF8().data());
	 curNode=curNode->GetNext();
  }

  return(true);
}

bool UserProfile::SetDatabaseFilename(wxString fn){
  wxString tString;

  fn.Replace(" ", "_", true);	  // don't allow spaces
  tString=fn;

  if(tString.MakeUpper().Right(4)!=".PDT"){
	 fn=fn+".pdt";
  }
  databaseFilename=fn;
  return(true);
}

bool UserProfile::SetWorkoutsFilename(wxString fn){
  wxString tString;

  fn.Replace(" ", "_", true);	  // don't allow spaces
  tString=fn;

  if(tString.MakeUpper().Right(4)!=".DAT"){
	 fn=fn+".dat";
  }
  workoutFilename=fn;
  return(true);
}

bool UserProfile::LoadData(){
  return(true);
}
