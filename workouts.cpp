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

#include"wxReadTextFile.h"
#include"workouts.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(WorkoutList);
WX_DEFINE_LIST(PhaseList);

WorkoutEntry::WorkoutEntry()
{
  ;									  // nothing for now
}

WorkoutEntry::WorkoutEntry(wxString title, wxString abbrev, wxString description)
{
  m_title=title;
  m_abbreviation=abbrev;
  m_description=description;
}

WorkoutEntry::WorkoutEntry(wxString title, wxString abbrev, wxString description, wxString phases)
{
  m_title=title;
  m_abbreviation=abbrev;
  m_description=description;
  m_phases=phases;
}

WorkoutMenu::WorkoutMenu()
{
  workoutList=new WorkoutList();
  workoutList->DeleteContents(true);

  phaseList=new PhaseList();
  phaseList->DeleteContents(true);
}

bool WorkoutMenu::LoadWorkouts(wxString fn)
{
  int ii,jj;
  wxString inTxt, tTxt;
  long red,green,blue;

  wxString title, abbrev, description, phases;
  WorkoutPhase *phase;

  m_filename=fn;					  // store for later
  wxReadTextFile fi(m_filename);
  while(fi.ReadLine(&inTxt)==true){
	 jj=inTxt.Find('>');			  // find the end of the key
	 jj++;

	 if(inTxt.Left(jj)=="<workoutVersion>"){
		;								  // ignore this for now
	 }
	 if(inTxt.Left(jj)=="<title>"){ // new entry into the list
		title=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<abbrev>"){
		abbrev=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<description>"){
		description=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<applicablephases>"){
		phases=inTxt.Mid(jj);
	 }
	 if(inTxt.Left(jj)=="<end>"){
		if(title.IsEmpty()==false){ // just to be safe
		  workoutList->Append(new WorkoutEntry(title, abbrev, description, phases));
		  title.Empty();
		}
	 }
	 if(inTxt.Left(jj)=="<phase>"){
		phaseList->Append(new WorkoutPhase(inTxt.Mid(jj)));
	 }
	 if(inTxt.Left(jj)=="<phasecolour>"){ // display color for a given phase
		phase=phaseList->GetLast()->GetData();
		if(phase!=NULL){
		  inTxt=inTxt.Mid(13);
		  ii=inTxt.Find(',');
		  tTxt=inTxt.Left(ii);
		  tTxt.ToLong(&red);
		  tTxt=inTxt.Mid(ii+1);
		  ii=tTxt.Find(',');
		  tTxt.Left(ii).ToLong(&green);
		  tTxt=tTxt.Mid(ii+1);
		  tTxt.ToLong(&blue);

		  phase->m_phaseColour=wxColour((int)red,(int)green,(int)blue);
		}
	 }
	 inTxt.Empty();
  }
  return(true);					  // loaded successfully
}

bool WorkoutMenu::SaveWorkouts()
{
  PhaseList::Node *currentLine;

  WorkoutList::Node *workoutNode;
  WorkoutEntry *workoutData;

  WorkoutPhase *workoutPhase;

  FILE *fp;
  wxString tmpStr;

  fp=fopen(m_filename, "w");
  if(fp==NULL){
	 wxMessageBox("Could not write file", "Error", wxOK);
	 return(false);
  }

  workoutNode=workoutList->GetFirst();
  tmpStr=wxString::Format("<workoutVersion>1\n");
  fprintf(fp, tmpStr);

  while(workoutNode!=NULL){
	 workoutData=workoutNode->GetData();
	 tmpStr=wxString::Format("<title>%s\n", workoutData->m_title.GetData());
	 fprintf(fp, tmpStr);
	 tmpStr=wxString::Format("<abbrev>%s\n", workoutData->m_abbreviation.GetData());
	 fprintf(fp, tmpStr);
	 tmpStr=wxString::Format("<description>%s\n", workoutData->m_description.GetData());
	 fprintf(fp, tmpStr);
	 tmpStr=wxString::Format("<applicablephases>%s\n", workoutData->m_phases.GetData());
	 fprintf(fp, tmpStr);
	 tmpStr=wxString::Format("<end>\n");
	 fprintf(fp, tmpStr);
	 workoutNode=workoutNode->GetNext();
  }

  currentLine=phaseList->GetFirst();
  while(currentLine!=NULL){
	 workoutPhase=currentLine->GetData();
	 tmpStr=wxString::Format("<phase>%s\n", workoutPhase->m_phase.GetData());
	 fprintf(fp, tmpStr);
	 tmpStr=wxString::Format("<phasecolour>%d,%d,%d\n", workoutPhase->m_phaseColour.Red(),
									 workoutPhase->m_phaseColour.Green(),
									 workoutPhase->m_phaseColour.Blue());

	 fprintf(fp, tmpStr);
	 currentLine=currentLine->GetNext();
  }
  fclose(fp);
  return(true);
}
