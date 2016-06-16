/***************************************************************
 * Name:      ide3acMain.h
 * Purpose:   Defines Application Frame
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-03-18
 * Copyright: A. Berestyuk ()
 * License:   zlib (http://www.gzip.org/zlib/zlib_license.html)
 **************************************************************/

#ifndef IDE3ACMAIN_H
#define IDE3ACMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

//!wxWidgets headers
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>

//!application headers
#include "include/Definitions.h"
#include "ide3acApp.h"
#include "include/EditorCtrl.h"

//--------------------------------------------------------------
//! frame of the application APP_VENDOR-APP_NAME.
class ide3acFrame: public wxFrame
{
    friend class ide3acApp;

    public:
        //! constructor
        ide3acFrame(wxFrame *frame, const wxString& title);
        //! destructor
        ~ide3acFrame();

        //! event handlers
        //! common
        void OnClose(wxCloseEvent &event);
        void OnQuit(wxCommandEvent &event);
        void OnAbout(wxCommandEvent &event);
        void OnExit(wxCommandEvent &event);
        void OnMainToolbarToggle(wxCommandEvent &event);
        //! file
        void OnFileNew(wxCommandEvent &event);
        void OnFileOpen(wxCommandEvent &event);
        void OnFileSave(wxCommandEvent &event);
        void OnFileSaveAs(wxCommandEvent &event);
        void OnFileClose(wxCommandEvent &event);
        //! properties
        void OnProperties (wxCommandEvent &event);
        //! edit events
        void OnEdit(wxCommandEvent &event);
        void OnModified(wxStyledTextEvent &event);

    private:
        void FileOpen(wxString fname);

 #if wxUSE_MENUS
        //! creates the application menu bar
        wxMenuBar *m_menuBar;
        void CreateMenu();
#endif // wxUSE_MENUS

#if wxUSE_TOOLBAR
        //! creates a tool bar with some frequently used buttons
        wxToolBar *m_toolBar;
        void CreateToolbar();
#endif // wxUSE_TOOLBAR

        // important variables

        // main sizer of the application layout
        wxBoxSizer *m_vbox;
        // notebook object
        wxNotebook* m_notebook;
        // unnamed new files counter
        int nonameFileCounter;
        // splitter windows
        wxSplitterWindow* m_splittermain;
        wxSplitterWindow* m_splitCode;
        // editor object
        EditorCtrl* m_editor;
        // asm code window
        wxTextCtrl* m_asm;
        // output window
        wxTextCtrl* m_log;

        DECLARE_EVENT_TABLE()
};


#endif // IDE3ACMAIN_H
