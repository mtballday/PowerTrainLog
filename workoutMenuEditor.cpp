// workoutMenuEditor.cpp

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include<wx/file.h>
#include<wx/colordlg.h>
#include"userProfile.h"
#include"workoutMenuEditor.h"

enum
  {
	 ID_WORKOUTMENUCTRL=100,
	 ID_WORKOUTMENUADD,
	 ID_WORKOUTMENUDELETE,
	 ID_PHASESCTRL,
	 ID_SAVELIST
  };


BEGIN_EVENT_TABLE(WorkoutMenuEditor, wxDialog)
  EVT_LISTBOX_DCLICK(ID_WORKOUTMENUCTRL, WorkoutMenuEditor::OnClickedWorkout)
  EVT_BUTTON(ID_WORKOUTMENUADD, WorkoutMenuEditor::AddWorkout)
  EVT_BUTTON(ID_SAVELIST,       WorkoutMenuEditor::SaveWorkout)
  EVT_BUTTON(ID_WORKOUTMENUDELETE, WorkoutMenuEditor::DeleteWorkout)
  EVT_LIST_ITEM_ACTIVATED(ID_PHASESCTRL, WorkoutMenuEditor::EditPhase)

  END_EVENT_TABLE()
  ;

WorkoutMenuEditor::WorkoutMenuEditor(wxWindow *parent, UserProfile *pf)
  :wxDialog(parent, -1, "Workout Menu Editor",
			  wxDefaultPosition, wxSize(500, 700),
			  wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU|wxRESIZE_BORDER){
  // Init vars
  userProfile=pf;
  // Create Window
  wxStaticText *workoutListTitle=new wxStaticText(this, -1, "Workout menu");
  workoutMenuCtrl=new wxListBox(this, ID_WORKOUTMENUCTRL, wxDefaultPosition,
										  wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_HSCROLL);
  
  wxButton *saveButton=new wxButton(this, ID_SAVELIST, "&Save List");
  wxButton *addButton=new wxButton(this, ID_WORKOUTMENUADD, "&Add Workout");
  wxButton *deleteButton=new wxButton(this, ID_WORKOUTMENUDELETE, "De&lete Workout");

  wxStaticText *workoutTitleText=new wxStaticText(this, -1, "&Title");
  workoutTitleCtrl=new wxTextCtrl(this, -1, "");

  wxStaticText *workoutAbbrevText=new wxStaticText(this, -1, "&Abbreviation");
  workoutAbbrevCtrl=new wxTextCtrl(this, -1, "");

  wxStaticText *workoutDescriptionText=new wxStaticText(this, -1, "&Description");
  workoutDescriptionCtrl=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize,
													 wxTE_MULTILINE);

  wxStaticText *phasesText=new wxStaticText(this, -1, "&Phases");
  phasesCtrl=new wxListCtrl(this, ID_PHASESCTRL, wxDefaultPosition,
									 wxDefaultSize, wxLC_LIST, wxDefaultValidator, "Phases");

  phasesCtrl->InsertColumn(0, "Phase");
  wxBoxSizer *addDeleteSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *col1Sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *colContainer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *okcancelSizer=new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer *titleSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *abbrevSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *col2Sizer=new wxBoxSizer(wxVERTICAL);


  wxBoxSizer *mainSizer=new wxBoxSizer(wxVERTICAL);

  col1Sizer->Add(workoutListTitle, 0, wxGROW, 1);
  col1Sizer->Add(workoutMenuCtrl, 1, wxGROW, 1);
  addDeleteSizer->Add(saveButton, 0, wxGROW, 1);
  addDeleteSizer->Add(addButton, 0, wxGROW, 1);
  addDeleteSizer->Add(deleteButton, 0, wxGROW, 1);
  col1Sizer->Add(addDeleteSizer);

  titleSizer->Add(workoutTitleText, 0, wxGROW, 1);
  titleSizer->Add(workoutTitleCtrl, 1, 0, 1);
  col2Sizer->Add(titleSizer, 0, wxGROW, 1);

  abbrevSizer->Add(workoutAbbrevText, 0, wxGROW, 1);
  abbrevSizer->Add(workoutAbbrevCtrl, 1, 0, 1);
  col2Sizer->Add(abbrevSizer, 0, wxGROW, 1);
  col2Sizer->Add(workoutDescriptionText, 0, wxGROW, 1);
  col2Sizer->Add(workoutDescriptionCtrl, 2, wxGROW, 1);
  col2Sizer->Add(phasesText, 0, wxGROW, 1);
  col2Sizer->Add(phasesCtrl, 4, wxGROW, 1);

  colContainer->Add(col1Sizer, 1, wxGROW, 1);
  colContainer->Add(col2Sizer, 1, wxGROW, 1);
  mainSizer->Add(colContainer, 1, wxGROW, 1);
  mainSizer->Add(okcancelSizer, 0, wxGROW, 1);

  FillWorkouts();
  FillPhases();

  SetSizer(mainSizer);
  //  mainSizer->Fit(this);
  Centre(wxBOTH);
}

void WorkoutMenuEditor::FillWorkouts()
{
  WorkoutList::Node *workoutNode;

  workoutMenuCtrl->Clear();

  workoutNode=userProfile->workoutMenu.workoutList->GetFirst();
  while(workoutNode!=NULL){
	 workoutMenuCtrl->Append(wxString::Format("%s: %s", 
														workoutNode->GetData()->m_abbreviation.GetData(),
														workoutNode->GetData()->m_title.GetData()));
	 workoutNode=workoutNode->GetNext();
  }
}

void WorkoutMenuEditor::FillPhases()
{
  int ii=0;

  PhaseList::Node *currentLine;
  currentLine=userProfile->workoutMenu.phaseList->GetFirst();
  phasesCtrl->DeleteAllItems();
  while(currentLine!=NULL){
	 phasesCtrl->InsertItem(ii, currentLine->GetData()->m_phase);
	 phasesCtrl->SetItemTextColour(ii, currentLine->GetData()->m_phaseColour);
	 ii++;
	 currentLine=currentLine->GetNext();
  }
}

void WorkoutMenuEditor::OnClickedWorkout(wxCommandEvent &event)
{
  WorkoutList::Node *workoutNode;
  WorkoutEntry *workoutData;
  PhaseList::Node *phaseNode;
  WorkoutPhase *phaseData;
  wxString tStr, tPhases;

  int index;

  index=workoutMenuCtrl->GetSelection(); // get the item
  if(index<0) return;
  workoutNode=userProfile->workoutMenu.workoutList->Item(index);
  
  workoutData=workoutNode->GetData();
  workoutTitleCtrl->SetValue(workoutData->m_title);
  workoutAbbrevCtrl->SetValue(workoutData->m_abbreviation);
  workoutDescriptionCtrl->SetValue(workoutData->m_description);

  index=0;							  // fill applicable phases
  phaseNode=userProfile->workoutMenu.phaseList->GetFirst();
  tPhases=","+workoutData->m_phases+",";
  while(phaseNode!=NULL){		  // check each phase and see if it is in the
										  // phases string
	 phaseData=phaseNode->GetData();
	 tStr=","+phaseData->m_phase+","; // commas provide specificity
	 if(tPhases.Find(tStr)>-1){
		phasesCtrl->SetItemState(index,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);  
	 }else{
		phasesCtrl->SetItemState(index,0, wxLIST_STATE_SELECTED);
	 }
	 index++;
	 phaseNode=phaseNode->GetNext();
  }
}

void WorkoutMenuEditor::DeleteWorkout(wxCommandEvent &event)
{
  int index;
  wxString tmpStr;
  WorkoutList::Node *workoutNode;
  WorkoutEntry *workoutData;

  index=workoutMenuCtrl->GetSelection(); // get the item
  if(index<0)return;
  workoutNode=userProfile->workoutMenu.workoutList->Item(index);
  workoutData=workoutNode->GetData();

  tmpStr=wxString::Format("Delete %s. Are you sure?", workoutData->m_title.GetData());
  wxMessageDialog md(this, tmpStr, "Notice", wxYES_NO|wxYES_DEFAULT);
  if(md.ShowModal()==wxID_NO){
	 return;
  }
  userProfile->workoutMenu.workoutList->DeleteNode(workoutNode);
  FillWorkouts();
}

void WorkoutMenuEditor::EditPhase(wxListEvent &event)
{
  PhaseList::Node *phaseNode;
  WorkoutPhase *phaseData;

  wxColourData colData;
  phaseNode=userProfile->workoutMenu.phaseList->Item(event.m_itemIndex);
  colData.SetColour(phaseNode->GetData()->m_phaseColour);
  wxColourDialog cd(this, &colData);
  if(cd.ShowModal()==wxID_OK){
	 phaseNode->GetData()->m_phaseColour=cd.GetColourData().GetColour();
  }
  FillPhases();
}


void WorkoutMenuEditor::AddWorkout(wxCommandEvent &event)
{
  WorkoutList::Node *workoutNode;
  WorkoutEntry *workoutData;
  PhaseList::Node *phaseNode;
  WorkoutPhase *phaseData;
  int index;

  wxString title;
  wxString abbreviation;

  char ret[]={10, 0};
  char lf[]={13, 0};
  char empty[]={' ',0};


  title=workoutTitleCtrl->GetValue();
  abbreviation=workoutAbbrevCtrl->GetValue();

  if(title.IsEmpty()==true){
	 wxMessageBox("Must enter a title", "Error", wxOK);
	 return;
  }
  if(abbreviation.IsEmpty()==true){
	 wxMessageBox("Must enter an abbreviation", "Error", wxOK);
	 return;
  }
  
  workoutNode=userProfile->workoutMenu.workoutList->GetFirst();

  while(workoutNode!=NULL){
	 workoutData=workoutNode->GetData();
	 if(workoutData->m_title==title){ // this workout title already exists
		wxMessageDialog md(this, "This workout exists. Replace it?", "Notice", wxYES_NO|wxYES_DEFAULT);
		if(md.ShowModal()==wxID_NO){
		  return;
		}else{
		  workoutData->m_abbreviation=abbreviation;
		  workoutData->m_description=workoutDescriptionCtrl->GetValue();
		  workoutData->m_description.Replace(ret, empty, true);
		  workoutData->m_description.Replace(lf, empty, true);
		  workoutData->m_phases.Empty();

		  index=0;							  // fill applicable phases
		  phaseNode=userProfile->workoutMenu.phaseList->GetFirst();
		  while(phaseNode!=NULL){
			 phaseData=phaseNode->GetData();
			 if(phasesCtrl->GetItemState(index, wxLIST_STATE_SELECTED)!=0){
				workoutData->m_phases=workoutData->m_phases+","+phaseData->m_phase;
			 }
			 index++;
			 phaseNode=phaseNode->GetNext();
		  }
		  while(workoutData->m_phases.Left(1)==",") workoutData->m_phases=workoutData->m_phases.Mid(1);

		  FillWorkouts();
		  workoutTitleCtrl->SetValue("");
		  workoutAbbrevCtrl->SetValue("");
		  workoutDescriptionCtrl->SetValue("");
		  return;
		}
	 }else if(workoutData->m_abbreviation==abbreviation){	// if different title
																			// but same abbrev
		wxMessageBox("Abbreviation is already taken", "Error", wxOK);
		return;
	 }
	 workoutNode=workoutNode->GetNext();
  }
  workoutData=new WorkoutEntry(title, abbreviation, workoutDescriptionCtrl->GetValue());
  userProfile->workoutMenu.workoutList->Append(workoutData);
  workoutData->m_description.Replace(ret, empty, true);
  workoutData->m_description.Replace(lf, empty, true);
  FillWorkouts();
  
  workoutTitleCtrl->SetValue("");
  workoutAbbrevCtrl->SetValue("");
  workoutDescriptionCtrl->SetValue("");
}

void WorkoutMenuEditor::SaveWorkout(wxCommandEvent &event)
{
  userProfile->workoutMenu.SaveWorkouts();
}
