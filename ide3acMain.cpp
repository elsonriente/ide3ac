/***************************************************************
 * Name:      ide3acMain.cpp
 * Purpose:   Code for Application Frame
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

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/filename.h>

#include "ide3acMain.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(ide3acFrame, wxFrame)
    // common
    EVT_CLOSE (                      ide3acFrame::OnClose)
    EVT_MENU (myID_TOOLBAR_TOGGLE,   ide3acFrame::OnMainToolbarToggle)
    // file
    EVT_MENU (wxID_OPEN,             ide3acFrame::OnFileOpen)
    EVT_MENU (wxID_SAVE,             ide3acFrame::OnFileSave)
    EVT_MENU (wxID_SAVEAS,           ide3acFrame::OnFileSaveAs)
    EVT_MENU (wxID_CLOSE,            ide3acFrame::OnFileClose)
    // properties
    EVT_MENU (myID_PROPERTIES,       ide3acFrame::OnProperties)
    // exit
    EVT_MENU (idMenuQuit,            ide3acFrame::OnExit)
    EVT_MENU (wxID_EXIT,             ide3acFrame::OnExit)
    // Menu items with standard IDs forwarded to the editor.
    EVT_MENU (wxID_CLEAR,            ide3acFrame::OnEdit)
    EVT_MENU (wxID_CUT,              ide3acFrame::OnEdit)
    EVT_MENU (wxID_COPY,             ide3acFrame::OnEdit)
    EVT_MENU (wxID_PASTE,            ide3acFrame::OnEdit)
    EVT_MENU (wxID_SELECTALL,        ide3acFrame::OnEdit)
    EVT_MENU (wxID_REDO,             ide3acFrame::OnEdit)
    EVT_MENU (wxID_UNDO,             ide3acFrame::OnEdit)
    EVT_MENU (wxID_FIND,             ide3acFrame::OnEdit)
    // And all our edit-related menu commands.
    EVT_MENU_RANGE (myID_EDIT_FIRST, myID_EDIT_LAST,
                                     ide3acFrame::OnEdit)
    // target platform change
    //EVT_MENU (myID_X86,              ide3acFrame::OnTargetPlatformChange)
    //EVT_MENU (myID_ARM,              ide3acFrame::OnTargetPlatformChange)
    //EVT_MENU (myID_MIPS32,           ide3acFrame::OnTargetPlatformChange)
    // symbol table
    //EVT_MENU (myID_SYMBOL_TABLE,     ide3acFrame::OnSymbolTable)
    // simulator run
    //EVT_MENU (myID_SIMULATOR_RUN,    ide3acFrame::OnSimulatorRun)
    // options
    //EVT_MENU (myID_OPTIONS,          ide3acFrame::OnOptions)
    // pages
    //EVT_MENU (myID_PAGENEXT,         ide3acFrame::OnPageNext)
    //EVT_MENU (myID_PAGEPREV,         ide3acFrame::OnPagePrev)
    // help
    //EVT_MENU (wxID_HELP_CONTENTS,    ide3acFrame::OnHelpContents)
    EVT_MENU (wxID_ABOUT,            ide3acFrame::OnAbout)
    // editor
    EVT_STC_MODIFIED (wxID_ANY,      ide3acFrame::OnModified)
END_EVENT_TABLE()

ide3acFrame::ide3acFrame(wxFrame *frame, const wxString& title)
    : wxFrame(0L, wxID_ANY, title, wxDefaultPosition, wxSize(790,550), wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    // set icon and background
    SetIcon(wxICON(sample));
    SetTitle (*g_appname);
    SetBackgroundColour (wxT("WHITE"));

#if wxUSE_MENUS
    // create a menu bar
    m_menuBar = new wxMenuBar;
    CreateMenu ();
#endif // wxUSE_MENUS

    // open first page
    //m_edit = new Edit (this, wxID_ANY);
    //m_edit->SetFocus();

    //FileOpen (wxT("stctest.cpp"));

    // main sizer of the application layout
    m_vbox = new wxBoxSizer(wxVERTICAL);

#if wxUSE_TOOLBAR
    // create a tool bar with some frequently used buttons
    m_toolBar = new wxToolBar (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_TEXT);
    CreateToolbar ();

    m_vbox->Add(m_toolBar, 0, wxEXPAND);

    if (m_menuBar->IsChecked(myID_TOOLBAR_TOGGLE))
        m_vbox->Show(m_toolBar);
    else
        m_vbox->Hide(m_toolBar);

#endif // wxUSE_TOOLBAR

    // initialize important variables
    //m_edit = NULL;

    // notebook
    m_notebook = new wxNotebook(this, myID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0, _T("myID_NOTEBOOK"));
    // unnamed new files counter
    nonameFileCounter = 1;


    m_splittermain = new wxSplitterWindow(m_notebook, myID_SPLITTER_MAIN, wxDefaultPosition, wxDefaultSize, wxSP_3D, _T("myID_SPLITTER_MAIN"));
    m_splittermain->SetMinimumPaneSize(150);
    m_splittermain->SetSashGravity(0.8);

    m_splitCode = new wxSplitterWindow(m_splittermain, myID_SPLIT_CODE, wxDefaultPosition, wxDefaultSize, wxSP_3D, _T("myID_SPLIT_CODE"));
    m_splitCode->SetMinimumPaneSize(250);
    m_splitCode->SetSashGravity(0.7);

    // editor window
    wxPanel *editorPanel=new wxPanel(m_splitCode, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxNO_BORDER);
    wxStaticBoxSizer *editorSizer=new wxStaticBoxSizer(wxVERTICAL, editorPanel, _("Editor"));
    m_editor = new EditorCtrl(editorPanel, myID_EDITOR, wxDefaultPosition, wxDefaultSize);
    editorSizer->Add(m_editor, 1, wxALL|wxEXPAND, 0);
    editorPanel->SetSizer(editorSizer);

    // asm code window
    wxPanel *asmPanel=new wxPanel(m_splitCode, wxID_ANY,wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxNO_BORDER);
    wxStaticBoxSizer *asmSizer=new wxStaticBoxSizer(wxVERTICAL, asmPanel, _("Target Platform ASM Code"));
    m_asm = new wxTextCtrl(asmPanel, myID_ASM_WINDOW, _(""), wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("myID_ASM_WINDOW"));
    asmSizer->Add(m_asm, 1, wxALL|wxEXPAND, 0);
    asmPanel->SetSizer(asmSizer);

    m_splitCode->SplitVertically(editorPanel, asmPanel);
    m_splitCode->SetSashPosition(500);

    // output window
    wxPanel *logPanel=new wxPanel(m_splittermain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxNO_BORDER);
    wxStaticBoxSizer *logSizer=new wxStaticBoxSizer(wxVERTICAL, logPanel, _("Output Log"));
    m_log = new wxTextCtrl(logPanel, myID_LOG, _(""), wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("myID_LOG"));
    logSizer->Add(m_log, 1, wxALL|wxEXPAND, 0);
    logPanel->SetSizer(logSizer);

    m_splittermain->SplitHorizontally(m_splitCode, logPanel);
    m_splittermain->SetSashPosition(350);



    m_notebook->AddPage(m_splittermain, NONAME + wxString() << nonameFileCounter++, true);
    m_vbox->Add(m_notebook, 1, wxEXPAND, 0);

    SetSizer(m_vbox);
    m_vbox->Layout();
    //m_vbox->SetSizeHints(this);

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Welcome to the Three Address Code IDE!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR
    Centre();
}


ide3acFrame::~ide3acFrame()
{
}

// common event handlers
void ide3acFrame::OnClose(wxCloseEvent &event)
{
    wxCommandEvent evt;
    OnFileClose(evt);
    if (m_editor && m_editor->IsModified())
    {
        if (event.CanVeto()) event.Veto(true);
        return;
    }
    Destroy();
}

void ide3acFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void ide3acFrame::OnExit (wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

void ide3acFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void ide3acFrame::OnMainToolbarToggle(wxCommandEvent &WXUNUSED(event))
{
    if (m_menuBar->IsChecked(myID_TOOLBAR_TOGGLE))
        m_vbox->Show(m_toolBar);
    else
        m_vbox->Hide(m_toolBar);
    m_vbox->Layout();
}

// file event handlers
void ide3acFrame::OnFileNew(wxCommandEvent &event)
{
    wxCommandEvent evt;
    OnFileClose (evt);
}

void ide3acFrame::OnFileOpen(wxCommandEvent &WXUNUSED(event))
{
    if (!m_editor) return;
#if wxUSE_FILEDLG
    wxString fname;
    wxFileDialog dlg(this, wxT("Open file"), wxEmptyString, wxEmptyString, wxT("Three Address Code file(*.3ac)|*.3ac|Any file (*)|*"),
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
    if (dlg.ShowModal() != wxID_OK)
        return;
    fname = dlg.GetPath();
    FileOpen(fname);
#endif // wxUSE_FILEDLG
}

void ide3acFrame::OnFileSave(wxCommandEvent &WXUNUSED(event))
{
    if (!m_editor)
        return;
    if (!m_editor->IsModified())
    {
        wxMessageBox(_("There is nothing to save!"), _("Save file"),
                      wxOK | wxICON_EXCLAMATION);
        return;
    }
    m_editor->SaveFile();
    wxFileName w(m_editor->GetFilename());
    w.Normalize();
    m_notebook->SetPageText(m_notebook->GetSelection(), w.GetName());
}

void ide3acFrame::OnFileSaveAs (wxCommandEvent &WXUNUSED(event))
{
    if (!m_editor)
        return;
#if wxUSE_FILEDLG
    wxString filename = wxEmptyString;
    wxFileDialog dlg(this, wxT("Save file"), wxEmptyString, wxEmptyString, wxT("Three Address Code file(*.3ac)|*.3ac|Any file (*)|*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK)
        return;
    filename = dlg.GetPath();
    m_editor->SaveFile(filename);
    wxFileName w(filename);
    w.Normalize();
    m_notebook->SetPageText(m_notebook->GetSelection(), w.GetName());
#endif // wxUSE_FILEDLG
}

void ide3acFrame::OnFileClose(wxCommandEvent &WXUNUSED(event))
{
    if (!m_editor)
        return;
    if (m_editor->IsModified())
    {
        if (wxMessageBox(_("Source file is not saved, save before closing?"), _("Close"),
                          wxYES_NO | wxICON_QUESTION) == wxYES)
        {
            m_editor->SaveFile();
            if (m_editor->IsModified())
            {
                wxMessageBox(_("Text could not be saved!"), _("Close abort"),
                              wxOK | wxICON_EXCLAMATION);
                return;
            }
        }
    }
    m_editor->SetFilename(wxEmptyString);
    m_editor->Clear();
    m_editor->DiscardEdits();
    m_editor->ClearAll();
    m_editor->SetSavePoint();
    m_notebook->SetPageText(m_notebook->GetSelection(), NONAME + wxString() << nonameFileCounter++);
}

// properties event handlers
void ide3acFrame::OnProperties (wxCommandEvent &WXUNUSED(event))
{
    if (!m_editor)
        return;
    EditorCtrlProperties dlg(m_editor, 0);
}

// edit events
void ide3acFrame::OnEdit (wxCommandEvent &event)
{
    if (m_editor)
        m_editor->GetEventHandler()->ProcessEvent(event);
}

void ide3acFrame::OnModified(wxStyledTextEvent &WXUNUSED(event))
{
    wxFileName w(m_editor->GetFilename());
    w.Normalize();
    m_notebook->SetPageText(m_notebook->GetSelection(), wxT("*") + w.GetName());
}

// private functions
void ide3acFrame::CreateMenu ()
{
    // File menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append (wxID_OPEN, _("&Open ..\tCtrl+O"));
    menuFile->Append (wxID_SAVE, _("&Save\tCtrl+S"));
    menuFile->Append (wxID_SAVEAS, _("Save &as ..\tCtrl+Shift+S"));
    menuFile->Append (wxID_CLOSE, _("&Close\tCtrl+W"));
    menuFile->AppendSeparator();
    menuFile->Append (myID_PROPERTIES, _("Proper&ties ..\tCtrl+I"));
    menuFile->AppendSeparator();
    menuFile->Append (wxID_EXIT, _("&Quit\tCtrl+Q"));

    // Edit menu
    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append (wxID_UNDO, _("&Undo\tCtrl+Z"));
    menuEdit->Append (wxID_REDO, _("&Redo\tCtrl+Shift+Z"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_CUT, _("Cu&t\tCtrl+X"));
    menuEdit->Append (wxID_COPY, _("&Copy\tCtrl+C"));
    menuEdit->Append (wxID_PASTE, _("&Paste\tCtrl+V"));
    menuEdit->Append (wxID_CLEAR, _("&Delete\tDel"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_FIND, _("&Find\tCtrl+F"));
    menuEdit->Enable (wxID_FIND, false);
    menuEdit->Append (myID_FINDNEXT, _("Find &next\tF3"));
    menuEdit->Enable (myID_FINDNEXT, false);
    menuEdit->Append (myID_REPLACE, _("&Replace\tCtrl+H"));
    menuEdit->Enable (myID_REPLACE, false);
    menuEdit->Append (myID_REPLACENEXT, _("Replace &again\tShift+F4"));
    menuEdit->Enable (myID_REPLACENEXT, false);
    menuEdit->AppendSeparator();
    menuEdit->Append (myID_GOTO, _("&Goto\tCtrl+G"));
    menuEdit->Enable (myID_GOTO, false);
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_SELECTALL, _("&Select all\tCtrl+A"));
    menuEdit->Append (myID_SELECTLINE, _("Select &line\tCtrl+L"));

    // View menu
    wxMenu *menuView = new wxMenu;
    menuView->AppendCheckItem (myID_TOOLBAR_TOGGLE, _("Tool&bar"));
    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_FOLDTOGGLE, _("&Toggle current fold\tCtrl+T"));
    menuView->AppendCheckItem (myID_OVERTYPE, _("&Overwrite mode\tIns"));
    menuView->AppendCheckItem (myID_WRAPMODEON, _("&Wrap mode\tCtrl+U"));
    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_DISPLAYEOL, _("Show line &endings"));
    menuView->AppendCheckItem (myID_LINENUMBER, _("Show line &numbers"));
    menuView->AppendCheckItem (myID_LONGLINEON, _("Show &long line marker"));
    menuView->AppendCheckItem (myID_WHITESPACE, _("Show white&space"));

    // target platform submenu
    wxMenu *menuTargetPlatform = new wxMenu;
    menuTargetPlatform->AppendRadioItem (myID_X86, _("x86"));
    menuTargetPlatform->AppendRadioItem (myID_ARM, _("Arm"));
    menuTargetPlatform->AppendRadioItem (myID_MIPS32, _("MIPS32"));

    // Project menu
    wxMenu *menuProject = new wxMenu;
    menuProject->AppendSubMenu (menuTargetPlatform, _("Target platform"));
    menuProject->AppendSeparator();
    menuProject->Append (myID_SYMBOL_TABLE, _("Edit &symbol table.."));

    // Simulator menu
    wxMenu *menuSimulator = new wxMenu;
    menuSimulator->Append (myID_SIMULATOR_RUN, _("&Run in simulator.."));

    // Tools menu
    wxMenu *menuTools = new wxMenu;
    menuTools->Append (myID_OPTIONS, _("&Options"));

    // Window menu
    wxMenu *menuWindow = new wxMenu;
    menuWindow->Append (myID_PAGEPREV, _("&Previous\tCtrl+Shift+Tab"));
    menuWindow->Append (myID_PAGENEXT, _("&Next\tCtrl+Tab"));

    // Help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append (wxID_HELP_CONTENTS, _("Help &Contents"));
    menuHelp->Append (wxID_ABOUT, _("&About ..\tCtrl+D"));

    // construct menu
    m_menuBar->Append (menuFile, _("&File"));
    m_menuBar->Append (menuEdit, _("&Edit"));
    m_menuBar->Append (menuView, _("&View"));
    m_menuBar->Append (menuProject, _("&Project"));
    m_menuBar->Append (menuSimulator, _("&Simulator"));
    m_menuBar->Append (menuTools, _("&Tools"));
    m_menuBar->Append (menuWindow, _("&Window"));
    m_menuBar->Append (menuHelp, _("&Help"));
    SetMenuBar (m_menuBar);

    m_menuBar->Check(myID_X86, true);
    m_menuBar->Check(myID_TOOLBAR_TOGGLE, true);
}

void ide3acFrame::CreateToolbar ()
{
    wxBitmap newb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_TOOLBAR));
    wxBitmap openb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR));
    wxBitmap saveb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE")),wxART_TOOLBAR));
    wxBitmap undob(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_UNDO")),wxART_TOOLBAR));
    wxBitmap redob(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_REDO")),wxART_TOOLBAR));
    wxBitmap cutb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_CUT")),wxART_TOOLBAR));
    wxBitmap copyb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_COPY")),wxART_TOOLBAR));
    wxBitmap pasteb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_PASTE")),wxART_TOOLBAR));
    wxBitmap findb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FIND")),wxART_TOOLBAR));
    wxBitmap replaceb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FIND_AND_REPLACE")),wxART_TOOLBAR));
    wxBitmap symtableb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_REPORT_VIEW")),wxART_TOOLBAR));
    wxBitmap targetb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_TOOLBAR));
    wxBitmap runb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_TOOLBAR));
    wxBitmap optionsb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_LIST_VIEW")),wxART_TOOLBAR));
    wxBitmap questionb(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUESTION")),wxART_TOOLBAR));

    m_toolBar->AddTool(wxID_NEW, wxT("New"), newb, newb, wxITEM_NORMAL, wxT("Create new source file"));
    m_toolBar->AddTool(wxID_OPEN, wxT("Open"), openb, openb, wxITEM_NORMAL, wxT("Open source file"));
    m_toolBar->AddTool(wxID_SAVE, wxT("Save"), saveb, saveb, wxITEM_NORMAL, wxT("Save source file"));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(wxID_UNDO, wxT("Undo"), undob, undob, wxITEM_NORMAL, wxT("Undo"));
    m_toolBar->AddTool(wxID_REDO, wxT("Redo"), redob, redob, wxITEM_NORMAL, wxT("Redo"));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(wxID_CUT, wxT("Cut"), cutb, cutb, wxITEM_NORMAL, wxT("Cut"));
    m_toolBar->AddTool(wxID_COPY, wxT("Copy"), copyb, copyb, wxITEM_NORMAL, wxT("Copy"));
    m_toolBar->AddTool(wxID_PASTE, wxT("Paste"), pasteb, pasteb, wxITEM_NORMAL, wxT("Paste"));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(wxID_FIND, wxT("Find"), findb, findb, wxITEM_NORMAL, wxT("Find"));
    m_toolBar->AddTool(wxID_REPLACE, wxT("Replace"), replaceb, replaceb, wxITEM_NORMAL, wxT("Replace"));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(myID_SYMBOL_TABLE, wxT("Symbol Table"), symtableb, symtableb, wxITEM_NORMAL, wxT("Edit symbol table.."));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(myID_TARGET_PLATFORM, wxT("Target Platform"), targetb, targetb, wxITEM_DROPDOWN, wxT("Choose target platform"));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(myID_SIMULATOR_RUN, wxT("Run"), runb, runb, wxITEM_NORMAL, wxT("Run in Simulator.."));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(myID_OPTIONS, wxT("Options"), optionsb, optionsb, wxITEM_NORMAL, wxT("Open options dialog.."));
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(wxID_HELP_CONTENTS, wxT("Help"), questionb, questionb, wxITEM_NORMAL, wxT("Open help Contents.."));

    m_toolBar->Realize();

    Connect(wxID_NEW, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnFileNew));
    Connect(wxID_OPEN, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnFileOpen));
    Connect(wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnFileSave));
    Connect(wxID_UNDO, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(wxID_REDO, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(wxID_CUT, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(wxID_COPY, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(wxID_PASTE, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    /*
    Connect(wxID_FIND, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(wxID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnEdit));
    Connect(myID_SYMBOL_TABLE, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnSymbolTable));
    Connect(myID_TARGET_PLATFORM, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnTargetPlatformChange));
    Connect(myID_SIMULATOR_RUN, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnSimulatorRun));
    Connect(myID_OPTIONS, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnOptions));
    Connect(wxID_HELP_CONTENTS, wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(ide3acFrame::OnHelpContents));*/
}

void ide3acFrame::FileOpen(wxString fname)
{
    wxFileName w(fname);
    w.Normalize();
    fname = w.GetFullPath();
    m_editor->LoadFile(fname);
    m_editor->SelectNone();
    m_notebook->SetPageText(m_notebook->GetSelection(), w.GetName());
}
