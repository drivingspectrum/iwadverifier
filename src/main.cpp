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
//	Main application sequence
//	AUTHOR:	Russell Rice
//
//-----------------------------------------------------------------------------


// main dialog resource
#include "xrc_resource.h"

#include "main.h"

#include <wx/xrc/xmlres.h>
#include <wx/image.h>

IMPLEMENT_APP(Application)

bool Application::OnInit()
{
    // now run application
	::wxInitAllImageHandlers();

	wxXmlResource::Get()->InitAllHandlers();

    // load resources
    InitXmlResource();

    // create main window and show it
    MAIN_DIALOG = new dlgMain(0L);
    
    if (MAIN_DIALOG) 
        MAIN_DIALOG->Show();
    
    return true;
}
