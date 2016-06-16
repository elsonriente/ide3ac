/***************************************************************
 * Name:      ide3acApp.cpp
 * Purpose:   Code for Application Class
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-03-18
 * Copyright: A. Berestyuk ()
 * License:   zlib (http://www.gzip.org/zlib/zlib_license.html)
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "ide3acApp.h"
#include "ide3acMain.h"

//--------------------------------------------------------------
//! global application name
wxString *g_appname = NULL;
//--------------------------------------------------------------

IMPLEMENT_APP(ide3acApp);

bool ide3acApp::OnInit()
{
    wxInitAllImageHandlers();

    // set application and vendor name
    SetAppName (APP_NAME);
    SetVendorName (APP_VENDOR);
    g_appname = new wxString ();
    g_appname->Append (APP_VENDOR);
    g_appname->Append (wxT("-"));
    g_appname->Append (APP_NAME);

    // create application frame
    m_frame = new ide3acFrame (0L, *g_appname);

    // set application icon
    m_frame->SetIcon (wxICON(aaaa));

    // open application frame
    m_frame->Layout ();
    m_frame->Show (true);

    return true;
}

int ide3acApp::OnExit()
{
    // delete global application name
    delete g_appname;

    return 0;
}
