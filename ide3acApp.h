/***************************************************************
 * Name:      ide3acApp.h
 * Purpose:   Defines Application Class
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-03-18
 * Copyright: A. Berestyuk ()
 * License:
 **************************************************************/

#ifndef IDE3ACAPP_H
#define IDE3ACAPP_H

#include <wx/app.h>

//--------------------------------------------------------------
// resources
//--------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

//--------------------------------------------------------------
// declarations
//--------------------------------------------------------------

#define APP_NAME wxT("Three Address Code IDE")
#define APP_DESCR _("See readme.txt")

#define APP_MAINT wxT("A. Berestyuk")
#define APP_VENDOR wxT("A. Berestyuk")
#define APP_COPYRIGTH wxT("(C) 2016 A. Berestyuk")
#define APP_LICENCE wxT("")

#define APP_VERSION wxT("0.1.alpha")
#define APP_BUILD __DATE__

#define APP_WEBSITE wxT("http://")
#define APP_MAIL wxT("mailto://elsonriente.work@gmail.com")

#define NONAME _("Untitled")

//--------------------------------------------------------------
//! global application name
extern wxString *g_appname;

//--------------------------------------------------------------
class ide3acFrame;

//! application APP_VENDOR-APP_NAME.
class ide3acApp : public wxApp
{
    friend class ide3acFrame;

    public:
        //! the main function called during application start
        virtual bool OnInit();

        //! application exit function
        virtual int OnExit ();

    private:
        //! frame window
        ide3acFrame* m_frame;

    //wxDECLARE_EVENT_TABLE();
};

// created dynamically by wxWidgets
//DECLARE_APP (App);

#endif // IDE3ACAPP_H
