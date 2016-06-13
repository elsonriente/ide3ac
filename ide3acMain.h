/***************************************************************
 * Name:      ide3acMain.h
 * Purpose:   Defines Application Frame
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-03-18
 * Copyright: A. Berestyuk ()
 * License:
 **************************************************************/

#ifndef IDE3ACMAIN_H
#define IDE3ACMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// wxWidgets headers
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/sizer.h>

#include "ide3acApp.h"

// ----------------------------------------------------------------------------
// standard IDs
// ----------------------------------------------------------------------------
enum
{
    // menu IDs
    idMenuQuit = 1000,
    idMenuAbout,
    myID_PROPERTIES = wxID_HIGHEST,
    myID_EDIT_FIRST,
    myID_FINDNEXT,
    myID_REPLACE,
    myID_REPLACENEXT,
    myID_GOTO,
    myID_PAGEACTIVE,
    myID_DISPLAYEOL,
    myID_LINENUMBER,
    myID_LONGLINEON,
    myID_WHITESPACE,
    myID_FOLDTOGGLE,
    myID_OVERTYPE,
    myID_READONLY,
    myID_WRAPMODEON,
    myID_PAGEPREV,
    myID_PAGENEXT,
    myID_SELECTLINE,
    myID_EDIT_LAST = myID_SELECTLINE,
    myID_TOOLBAR_TOGGLE,
    myID_X86,
    myID_ARM,
    myID_MIPS32,
    myID_SYMBOL_TABLE,
    myID_TARGET_PLATFORM,
    myID_SIMULATOR_RUN,
    myID_OPTIONS,
    myID_NOTEBOOK,
    myID_SPLITTER_MAIN,
    myID_SPLIT_CODE,
    myID_EDITOR,
    myID_ASM_WINDOW,
    myID_LOG,
    // other IDs
    myID_STATUSBAR,

    // dialog find IDs
    myID_DLG_FIND_TEXT,
};

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
        //! edit events
        void OnEdit(wxCommandEvent &event);

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
        wxTextCtrl* m_editor;
        // asm code window
        wxTextCtrl* m_asm;
        // output window
        wxTextCtrl* m_log;

        DECLARE_EVENT_TABLE()
};


#endif // IDE3ACMAIN_H
