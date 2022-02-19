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

#ifndef __DLG_MAIN_H__
#define __DLG_MAIN_H__

#include <wx/frame.h>
#include <wx/checklst.h>
#include <wx/filepicker.h>

#include <wx/intl.h>
#include <wx/xrc/xmlres.h>

// use std::vector, because wxArray is extreme!
#include <vector>

class dlgMain : public wxFrame
{
	public:

		dlgMain(wxWindow* parent,wxWindowID id = -1);
		virtual ~dlgMain();

        wxCheckListBox *m_GameName;
        wxCheckListBox *m_GameVersion;

        wxFilePickerCtrl *m_FilePicker;

	protected:
        typedef struct version_hash_s
        {
            wxString ver;
            wxString hash;
        } version_hash_t;
        typedef struct game_name_s
        {
            wxString name_of_game;
            std::vector<version_hash_t> versions;
        } game_name_t;

        std::vector<game_name_t> game_names;

        void OnOpenAboutDialog(wxCommandEvent& event);
		void OnFileCheck(wxCommandEvent &event);
		
		wxInt32 LoadHashes();
		
		void CheckListBox_UncheckAll(wxCheckListBox &);
	private:
        
		DECLARE_EVENT_TABLE()
};

#endif
