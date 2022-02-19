//-----------------------------------------------------------------------------
//
// Copyright (C) 2006-2007 by LichSoft Software
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	User interface
//	AUTHOR:	Russell Rice
//
//-----------------------------------------------------------------------------


#include "dlg_main.h"

#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>

#include "wxMD5.h"
#include "main.h"

// Control ID assignments for events
// application icon

static const wxInt32 wxID_BUFILECHECK = XRCID("wxID_BUFILECHECK");
static const wxInt32 wxID_BUABOUT = XRCID("wxID_BUABOUT");

static const wxInt32 wxID_CLGAMENAME = XRCID("wxID_CLGAMENAME");
static const wxInt32 wxID_CLVERSION = XRCID("wxID_CLVERSION");

static const wxInt32 wxID_FPWADFILE = XRCID("wxID_FPWADFILE");

// Event handlers
BEGIN_EVENT_TABLE(dlgMain, wxFrame)
	EVT_BUTTON(wxID_BUFILECHECK, dlgMain::OnFileCheck)
	EVT_BUTTON(wxID_BUABOUT, dlgMain::OnOpenAboutDialog)
END_EVENT_TABLE()

// Main window creation
dlgMain::dlgMain(wxWindow* parent, wxWindowID id)
{
	wxXmlResource::Get()->LoadFrame(this, parent, _T("dlgmain")); 
    SetIcon(wxXmlResource::Get()->LoadIcon(_T("mainicon")));
    
    m_GameName = wxStaticCast(FindWindow(wxID_CLGAMENAME), wxCheckListBox);
    m_GameVersion = wxStaticCast(FindWindow(wxID_CLVERSION), wxCheckListBox);
    
    m_FilePicker = wxStaticCast(FindWindow(wxID_FPWADFILE), wxFilePickerCtrl);
    
    // Makes the checklistboxes greyed out, so user can't check items
    // Disables scrollbars aswell, BLAH FUCK YOU MS.
    // m_GameName->Enable(false);
    // m_GameVersion->Enable(false);
    
    // Doesn't work on MSW, if it did, it probably wouldn't work right
    /*
    wxScrollBar *sbGameVersion = m_GameVersion->GetScrollbar(wxVERTICAL);
    
    sbGameVersion->Enable(true);
    */
    
    // Load hash data
    if (LoadHashes())
        Close();
}

// Window Destructor
dlgMain::~dlgMain() 
{
    if (game_names.size())
    {
        for (wxUint32 i = 0; i < game_names.size(); i++)
        {
            // clear our versions 
            if (game_names[i].versions.size())
                game_names[i].versions.clear();
                
        }
        
        // clear our parent vector
        game_names.clear();
    }
}

// Loads the hash data for comparison
wxInt32 dlgMain::LoadHashes()
{
    wxUint32 num_games = 0, num_vers = 0;
    wxString group_name;
    
    wxFileInputStream fcInput(_T("hashes.ini"));
    wxFileConfig fcHashFile(fcInput);
    
    // do some basic checks
    if (!fcHashFile.HasGroup(_T("Main")))
    {
        wxLogError(_T("Error: Section [Main] does not exist!"));
        
        return 1;
    }
    
    // This is lame, having to set path before reading a subsection...
    fcHashFile.SetPath(_T("Main"));
    if (!(num_games = fcHashFile.Read(_T("numsections"), (long)0)))
    {
        wxLogError(_T("Error: No game sections exist or numsections is 0"));
        
        return 2;
    }
    
    // set our vector size to the number of game names we have
    game_names.resize(num_games);
    
    // load our stuff
    for (wxUint32 i = 0; i < num_games; i++)
    {       
        // read in section name
        if (!(game_names[i].name_of_game = fcHashFile.Read(wxString::Format(_T("section%d"), i),
                                                           game_names[i].name_of_game)))
            continue;
              
        // don't add an item if it already exists
        if (m_GameName->FindString(game_names[i].name_of_game) == wxNOT_FOUND)
            m_GameName->Append(game_names[i].name_of_game);

        // jump to this section
        fcHashFile.SetPath(_T(".."));
        fcHashFile.SetPath(game_names[i].name_of_game);
              
        // read in the number and resize the vector
        num_vers = fcHashFile.Read(_T("num"), (long)0);
                                
        game_names[i].versions.resize(num_vers);

        for (wxUint32 j = 0; j < game_names[i].versions.size(); j++)
        {
            game_names[i].versions[j].ver = fcHashFile.Read(wxString::Format(_T("ver%d"), j),
                                                            game_names[i].versions[j].ver);
                                                            
            game_names[i].versions[j].hash = fcHashFile.Read(wxString::Format(_T("hash%d"), j),
                                                            game_names[i].versions[j].hash);
                                                            
            // don't add an item if it already exists
            if (m_GameVersion->FindString(game_names[i].versions[j].ver) == wxNOT_FOUND)
                m_GameVersion->Append(game_names[i].versions[j].ver);
        }

        // Cheap insurance
        fcHashFile.SetPath(_T("../Main"));          
    }
    
    return 0;
}

// User wants to check the file
void dlgMain::OnFileCheck(wxCommandEvent &event)
{
    // Reset all checkboxes
    CheckListBox_UncheckAll(*m_GameName);
    CheckListBox_UncheckAll(*m_GameVersion);
        
    if (!wxFileExists(m_FilePicker->GetPath()))
    {
        wxMessageBox(_T("File doesn't exist!"), 
                     _T("IWAD Verifier"),
                     wxICON_HAND);
        
        return;
    }
    
    // Get the MD5 of the file and lower case it
    wxMD5 md5File;

    md5File.SetFile(m_FilePicker->GetPath());  
    wxString strMD5Result = md5File.GetDigest(true);
    
    bool valid_wad = false;
    
    for (wxUint32 i = 0; i < game_names.size(); i++)
    {
        for (wxUint32 j = 0; j < game_names[i].versions.size(); j++)
        {
            // if it doesn't match the hash, go to next iteration
            if (game_names[i].versions[j].hash.CmpNoCase(strMD5Result) != 0)
                continue;
            
            wxInt32 gn_index = m_GameName->FindString(game_names[i].name_of_game);
            wxInt32 gv_index = m_GameVersion->FindString(game_names[i].versions[j].ver);
            
            if (gn_index == wxNOT_FOUND || gv_index == wxNOT_FOUND)
                continue;
                
            m_GameName->Check(gn_index, true);
            m_GameVersion->Check(gv_index, true);
            
            // if it got this far, super!
            valid_wad = true;
        }
    }
    
    if (!valid_wad)
        wxMessageBox(_T("This IWAD is NOT valid!"), 
                     _T("IWAD Verifier"),
                     wxICON_EXCLAMATION);
}

// Helper to clear all the checks in a checklistbox
void dlgMain::CheckListBox_UncheckAll(wxCheckListBox &CheckList)
{   
    if (CheckList.GetCount())
    {
        for (wxUint32 i = 0; i < CheckList.GetCount(); i++)
            CheckList.Check(i, false);
    }
}

void dlgMain::OnOpenAboutDialog(wxCommandEvent &event)
{
    wxString strAbout = _T("IWAD Verifier 2.1 - CopyRight 2008 LichSoft Software\n"
                         "==============================\n"
                         "\n"
                         "Credits\n"
                         "---------\n"
                         "Id Software - For all the (Doom) games they've released\n"
                         "All the people who gave me md5 hashes\n"
                         "ryan/salazar - The service of web hosting\n"
                         "wxWidgets - Crossplatform development only comes in 1 flavour");
    
    wxMessageBox(strAbout, _T("About IWAD Verifier"));
}
