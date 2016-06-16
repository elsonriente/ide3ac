/***************************************************************
 * Name:      EditorCtrl.cpp
 * Purpose:   Code for Editor Ctrl
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-03-18
 * Copyright: A. Berestyuk ()
 * License:   zlib (http://www.gzip.org/zlib/zlib_license.html)
 **************************************************************/

//--------------------------------------------------------------
// headers
//--------------------------------------------------------------

#ifdef WX_PRECOMP
#include "../wx_pch.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//! headers
#include <algorithm>

//! wxWidgets headers
#include "wx/file.h"     // raw file io support
#include "wx/filename.h" // filename support
#include "wx/string.h"   // strings

//! application headers
//#include "../include/Definitions.h" // definitions
#include "../include/EditorCtrl.h" // edit module

//--------------------------------------------------------------
// implementation
//--------------------------------------------------------------

//--------------------------------------------------------------
// EditorCtrl
//--------------------------------------------------------------

wxBEGIN_EVENT_TABLE (EditorCtrl, wxStyledTextCtrl)
    // common
    EVT_SIZE (                         EditorCtrl::OnSize)
    // edit
    EVT_MENU (wxID_CLEAR,              EditorCtrl::OnEditClear)
    EVT_MENU (wxID_CUT,                EditorCtrl::OnEditCut)
    EVT_MENU (wxID_COPY,               EditorCtrl::OnEditCopy)
    EVT_MENU (wxID_PASTE,              EditorCtrl::OnEditPaste)
    EVT_MENU (wxID_SELECTALL,          EditorCtrl::OnEditSelectAll)
    EVT_MENU (myID_SELECTLINE,         EditorCtrl::OnEditSelectLine)
    EVT_MENU (wxID_REDO,               EditorCtrl::OnEditRedo)
    EVT_MENU (wxID_UNDO,               EditorCtrl::OnEditUndo)
    // find
    EVT_MENU (wxID_FIND,               EditorCtrl::OnFind)
    EVT_MENU (myID_FINDNEXT,           EditorCtrl::OnFindNext)
    EVT_MENU (myID_REPLACE,            EditorCtrl::OnReplace)
    EVT_MENU (myID_REPLACENEXT,        EditorCtrl::OnReplaceNext)
    EVT_MENU (myID_GOTO,               EditorCtrl::OnGoto)
    // view
    EVT_MENU (myID_DISPLAYEOL,         EditorCtrl::OnDisplayEOL)
    EVT_MENU (myID_INDENTGUIDE,        EditorCtrl::OnIndentGuide)
    EVT_MENU (myID_LINENUMBER,         EditorCtrl::OnLineNumber)
    EVT_MENU (myID_LONGLINEON,         EditorCtrl::OnLongLineOn)
    EVT_MENU (myID_WHITESPACE,         EditorCtrl::OnWhiteSpace)
    EVT_MENU (myID_FOLDTOGGLE,         EditorCtrl::OnFoldToggle)
    EVT_MENU (myID_OVERTYPE,           EditorCtrl::OnSetOverType)
    EVT_MENU (myID_READONLY,           EditorCtrl::OnSetReadOnly)
    EVT_MENU (myID_WRAPMODEON,         EditorCtrl::OnWrapmodeOn)
    //EVT_MENU (myID_CHARSETANSI,        EditorCtrl::OnUseCharset)
    //EVT_MENU (myID_CHARSETMAC,         EditorCtrl::OnUseCharset)
    // extra
    //EVT_MENU (myID_CONVERTCR,          EditorCtrl::OnConvertEOL)
    //EVT_MENU (myID_CONVERTCRLF,        EditorCtrl::OnConvertEOL)
    //EVT_MENU (myID_CONVERTLF,          EditorCtrl::OnConvertEOL)
    // stc
    EVT_STC_MARGINCLICK (wxID_ANY,     EditorCtrl::OnMarginClick)
    EVT_STC_CHARADDED (wxID_ANY,       EditorCtrl::OnCharAdded)
    EVT_STC_KEY (wxID_ANY,             EditorCtrl::OnKey)
    EVT_KEY_DOWN (                     EditorCtrl::OnKeyDown)
    EVT_STC_STYLENEEDED (wxID_ANY,     EditorCtrl::OnStyleNeeded)
wxEND_EVENT_TABLE()

EditorCtrl::EditorCtrl(wxWindow *parent, wxWindowID id,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style)
    : wxStyledTextCtrl(parent, id, pos, size, style)
{
    m_filename = wxEmptyString;

    m_LineNrID = 0;
    m_DividerID = 1;
    m_FoldingID = 2;

    // initialize language
    m_language = NULL;

    // Use all the bits in the style byte as styles, not indicators.
    SetStyleBits(8);

    // default font for all styles
    SetViewEOL(g_CommonPrefs.displayEOLEnable);
    SetIndentationGuides(g_CommonPrefs.indentGuideEnable);
    SetEdgeMode(g_CommonPrefs.longLineOnEnable?
                 wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
    SetViewWhiteSpace(g_CommonPrefs.whiteSpaceEnable?
                       wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
    SetOvertype(g_CommonPrefs.overTypeInitial);
    SetReadOnly(g_CommonPrefs.readOnlyInitial);
    SetWrapMode(g_CommonPrefs.wrapModeInitial?
                 wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
    StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    StyleSetForeground(wxSTC_STYLE_DEFAULT, *wxBLACK);
    StyleSetBackground(wxSTC_STYLE_DEFAULT, *wxWHITE);
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour (wxT("DARK GREY")));
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (wxT("DARK GREY")));
    InitializePrefs(DEFAULT_LANGUAGE);

    // set visibility
    SetVisiblePolicy(wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
    SetXCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
    SetYCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);

    // markers
    MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_DOTDOTDOT, wxT("BLACK"), wxT("BLACK"));
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN, wxT("BLACK"), wxT("BLACK"));
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY,     wxT("BLACK"), wxT("BLACK"));
    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_DOTDOTDOT, wxT("BLACK"), wxT("WHITE"));
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, wxT("BLACK"), wxT("WHITE"));
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY,     wxT("BLACK"), wxT("BLACK"));
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY,     wxT("BLACK"), wxT("BLACK"));

    // annotations
    AnnotationSetVisible(wxSTC_ANNOTATION_BOXED);

    // miscellaneous
    m_LineNrMargin = TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_999999"));
    m_FoldingMargin = 16;
    CmdKeyClear(wxSTC_KEY_TAB, 0); // this is done by the menu accelerator key
    SetLayoutCache(wxSTC_CACHE_PAGE);

    InitializePrefs(wxT("ThreeAC"));
}

EditorCtrl::~EditorCtrl () {}

//----------------------------------------------------------------------------
// common event handlers
void EditorCtrl::OnSize( wxSizeEvent& event )
{
    int x = GetClientSize().x +
            (g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0) +
            (g_CommonPrefs.foldEnable? m_FoldingMargin: 0);
    if (x > 0) SetScrollWidth(x);
    event.Skip();
}

// edit event handlers
void EditorCtrl::OnEditRedo(wxCommandEvent &WXUNUSED(event))
{
    if (!CanRedo()) return;
    Redo();
}

void EditorCtrl::OnEditUndo(wxCommandEvent &WXUNUSED(event))
{
    if (!CanUndo()) return;
    Undo();
}

void EditorCtrl::OnEditClear(wxCommandEvent &WXUNUSED(event))
{
    if (GetReadOnly()) return;
    Clear();
}

void EditorCtrl::OnKey(wxStyledTextEvent &WXUNUSED(event))
{
    wxMessageBox("OnKey");
}

void EditorCtrl::OnKeyDown(wxKeyEvent &event)
{
    if (CallTipActive())
        CallTipCancel();
    if (event.GetKeyCode() == WXK_SPACE && event.ControlDown() && event.ShiftDown())
    {
        int pos = GetCurrentPos();
        CallTipSetBackground(*wxYELLOW);
        CallTipShow(pos,
                    "This is a CallTip with multiple lines.\n"
                    "It is meant to be a context sensitive popup helper for the user.");
        return;
    }
    event.Skip();
}

void EditorCtrl::OnEditCut (wxCommandEvent &WXUNUSED(event))
{
    if (GetReadOnly() || (GetSelectionEnd()-GetSelectionStart() <= 0)) return;
    Cut();
}

void EditorCtrl::OnEditCopy (wxCommandEvent &WXUNUSED(event))
{
    if (GetSelectionEnd()-GetSelectionStart() <= 0) return;
    Copy();
}

void EditorCtrl::OnEditPaste(wxCommandEvent &WXUNUSED(event))
{
    if (!CanPaste()) return;
    Paste();
}

void EditorCtrl::OnFind(wxCommandEvent &WXUNUSED(event))
{
}

void EditorCtrl::OnFindNext(wxCommandEvent &WXUNUSED(event))
{
}

void EditorCtrl::OnReplace(wxCommandEvent &WXUNUSED(event))
{
}

void EditorCtrl::OnReplaceNext(wxCommandEvent &WXUNUSED(event))
{
}
/*
void EditorCtrl::OnBraceMatch (wxCommandEvent &WXUNUSED(event)) {
    int min = GetCurrentPos ();
    int max = BraceMatch (min);
    if (max > (min+1)) {
        BraceHighlight (min+1, max);
        SetSelection (min+1, max);
    }else{
        BraceBadLight (min);
    }
}
*/
void EditorCtrl::OnGoto(wxCommandEvent &WXUNUSED(event))
{
}

void EditorCtrl::OnEditSelectAll(wxCommandEvent &WXUNUSED(event))
{
    SetSelection(0, GetTextLength ());
}

void EditorCtrl::OnEditSelectLine (wxCommandEvent &WXUNUSED(event))
{
    int lineStart = PositionFromLine(GetCurrentLine());
    int lineEnd = PositionFromLine(GetCurrentLine() + 1);
    SetSelection(lineStart, lineEnd);
}
/*
void EditorCtrl::OnHilightLang (wxCommandEvent &event) {
    InitializePrefs (g_LanguagePrefs [event.GetId() - myID_HILIGHTFIRST].name);
}
*/
void EditorCtrl::OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
{
    SetViewEOL(!GetViewEOL());
}

void EditorCtrl::OnIndentGuide(wxCommandEvent &WXUNUSED(event))
{
    SetIndentationGuides(!GetIndentationGuides());
}

void EditorCtrl::OnLineNumber(wxCommandEvent &WXUNUSED(event))
{
    SetMarginWidth(m_LineNrID,
                    GetMarginWidth(m_LineNrID) == 0? m_LineNrMargin: 0);
}

void EditorCtrl::OnLongLineOn(wxCommandEvent &WXUNUSED(event))
{
    SetEdgeMode(GetEdgeMode() == 0? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
}

void EditorCtrl::OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
{
    SetViewWhiteSpace(GetViewWhiteSpace() == 0?
                       wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
}

void EditorCtrl::OnFoldToggle(wxCommandEvent &WXUNUSED(event))
{
    ToggleFold (GetFoldParent(GetCurrentLine()));
}

void EditorCtrl::OnSetOverType(wxCommandEvent &WXUNUSED(event))
{
    SetOvertype (!GetOvertype());
}

void EditorCtrl::OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
{
    SetReadOnly (!GetReadOnly());
}

void EditorCtrl::OnWrapmodeOn(wxCommandEvent &WXUNUSED(event))
{
    SetWrapMode(GetWrapMode() == 0? wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
}
/*
void EditorCtrl::OnUseCharset (wxCommandEvent &event) {
    int Nr;
    int charset = GetCodePage();
    switch (event.GetId()) {
        case myID_CHARSETANSI: {charset = wxSTC_CHARSET_ANSI; break;}
        case myID_CHARSETMAC: {charset = wxSTC_CHARSET_ANSI; break;}
    }
    for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++) {
        StyleSetCharacterSet (Nr, charset);
    }
    SetCodePage (charset);
}

void EditorCtrl::OnAnnotationAdd(wxCommandEvent& WXUNUSED(event))
{
    const int line = GetCurrentLine();

    wxString ann = AnnotationGetText(line);
    ann = wxGetTextFromUser
          (
            wxString::Format("Enter annotation for the line %d", line),
            "Edit annotation",
            ann,
            this
          );
    if ( ann.empty() )
        return;

    AnnotationSetText(line, ann);
    AnnotationSetStyle(line, ANNOTATION_STYLE);

    // Scintilla doesn't update the scroll width for annotations, even with
    // scroll width tracking on, so do it manually.
    const int width = GetScrollWidth();

    // NB: The following adjustments are only needed when using
    //     wxSTC_ANNOTATION_BOXED annotations style, but we apply them always
    //     in order to make things simpler and not have to redo the width
    //     calculations when the annotations visibility changes. In a real
    //     program you'd either just stick to a fixed annotations visibility or
    //     update the width when it changes.

    // Take into account the fact that the annotation is shown indented, with
    // the same indent as the line it's attached to.
    int indent = GetLineIndentation(line);

    // This is just a hack to account for the width of the box, there doesn't
    // seem to be any way to get it directly from Scintilla.
    indent += 3;

    const int widthAnn = TextWidth(ANNOTATION_STYLE, ann + wxString(indent, ' '));

    if (widthAnn > width)
        SetScrollWidth(widthAnn);
}

void EditorCtrl::OnAnnotationRemove(wxCommandEvent& WXUNUSED(event))
{
    AnnotationSetText(GetCurrentLine(), wxString());
}

void EditorCtrl::OnAnnotationClear(wxCommandEvent& WXUNUSED(event))
{
    AnnotationClearAll();
}

void EditorCtrl::OnAnnotationStyle(wxCommandEvent& event)
{
    int style = 0;
    switch (event.GetId()) {
        case myID_ANNOTATION_STYLE_HIDDEN:
            style = wxSTC_ANNOTATION_HIDDEN;
            break;

        case myID_ANNOTATION_STYLE_STANDARD:
            style = wxSTC_ANNOTATION_STANDARD;
            break;

        case myID_ANNOTATION_STYLE_BOXED:
            style = wxSTC_ANNOTATION_BOXED;
            break;
    }

    AnnotationSetVisible(style);
}

void EditorCtrl::OnChangeCase (wxCommandEvent &event) {
    switch (event.GetId()) {
        case myID_CHANGELOWER: {
            CmdKeyExecute (wxSTC_CMD_LOWERCASE);
            break;
        }
        case myID_CHANGEUPPER: {
            CmdKeyExecute (wxSTC_CMD_UPPERCASE);
            break;
        }
    }
}
*/
/*
void EditorCtrl::OnConvertEOL (wxCommandEvent &event) {
    int eolMode = GetEOLMode();
    switch (event.GetId()) {
        case myID_CONVERTCR: { eolMode = wxSTC_EOL_CR; break;}
        case myID_CONVERTCRLF: { eolMode = wxSTC_EOL_CRLF; break;}
        case myID_CONVERTLF: { eolMode = wxSTC_EOL_LF; break;}
    }
    ConvertEOLs (eolMode);
    SetEOLMode (eolMode);
}
*/
//! misc
void EditorCtrl::OnMarginClick (wxStyledTextEvent &event)
{
    if (event.GetMargin() == 2)
    {
        int lineClick = LineFromPosition(event.GetPosition());
        int levelClick = GetFoldLevel(lineClick);
        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
        {
            ToggleFold(lineClick);
        }
    }
}

void EditorCtrl::OnCharAdded(wxStyledTextEvent &event)
{
    char chr = (char)event.GetKey();
    int currentLine = GetCurrentLine();
    // Change this if support for mac files with \r is needed
    if (chr == '\n')
    {
        int lineInd = 0;
        if (currentLine > 0)
        {
            lineInd = GetLineIndentation(currentLine - 1);
        }
        if (lineInd == 0) return;
        SetLineIndentation(currentLine, lineInd);
        GotoPos(PositionFromLine(currentLine) + lineInd);
    }
}

void EditorCtrl::OnStyleNeeded(wxStyledTextEvent &event)
{
    int startPos = GetEndStyled() == 0 ? GetEndStyled() : GetEndStyled() - 1;
    int endPos = event.GetPosition();

    DoStyling(startPos, endPos);
}

void EditorCtrl::DoStyling(int startPos, int endPos)
{
    const int STATE_UNKNOWN = 0;
    const int STATE_IDENTIFIER = 1;
    const int STATE_NUMBER = 2;
    const int STATE_STRING = 3;

    int sl = LineFromPosition(startPos);
    int el = LineFromPosition(endPos);

    startPos = PositionFromLine(sl);
    // Start styling
    StartStyling(startPos, 31);

    for (int line = sl; line <= el; line++)
    {
        startPos = PositionFromLine(line);
        endPos = startPos + LineLength(line);

        int length = 0;
        int state = STATE_UNKNOWN;

        while (startPos < endPos)
        {
            wxChar c = GetCharAt(startPos);

        REPROCESS:
            switch (state)
            {
                case STATE_UNKNOWN:
                    if (c == '"')
                    {
                        // Start of "string"
                        SetStyling(1, mySTC_TYPE_STRING);
                        state = STATE_STRING;
                    }
                    else if (std::isdigit(c))
                    {
                        state = STATE_NUMBER;
                        goto REPROCESS;
                    }
                    else if (std::isalpha(c) || c == '_' || c == '$')
                    {
                        state = STATE_IDENTIFIER;
                        goto REPROCESS;
                    }
                    else
                    {
                        // Everything else
                        SetStyling(1, mySTC_TYPE_DEFAULT);
                    }
                    break;

                case STATE_STRING:
                    if (c == '"')
                    {
                        length++;
                        SetStyling(length, mySTC_TYPE_STRING);
                        length = 0;
                        state = STATE_UNKNOWN;
                    }
                    else
                    {
                        length++;
                    }
                    break;

                case STATE_NUMBER:
                    if (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || c == 'x')
                    {
                        length++;
                    }
                    else
                    {
                        SetStyling(length, mySTC_TYPE_NUMBER);
                        length = 0;
                        state = STATE_UNKNOWN;
                        goto REPROCESS;
                    }
                    break;

                case STATE_IDENTIFIER:
                    if (std::isalnum(c) || ((c == '_' || c== '$') && length == 0))
                    {
                        length++;
                    }
                    else
                    {
                        int style = mySTC_TYPE_IDENTIFIER;
                        wxString identifier = GetTextRange(startPos - length, startPos);
                        if (std::find(tacKeywords.begin(), tacKeywords.end(), identifier) != tacKeywords.end())
                            style = mySTC_TYPE_WORD1;

                        if(style == mySTC_TYPE_IDENTIFIER && c == ':')
                        {
                            length++;
                            SetStyling(length, mySTC_TYPE_LABEL);
                            length = 0;
                            state = STATE_UNKNOWN;
                        }
                        else
                        {
                            SetStyling(length, style);
                            length = 0;
                            state = STATE_UNKNOWN;
                            goto REPROCESS;
                        }
                    }
                    break;
            }
            startPos++;
        }
    }
}

//----------------------------------------------------------------------------
// private functions
wxString EditorCtrl::DeterminePrefs(const wxString &filename)
{
    LanguageInfo const* curInfo;

    // determine language from filepatterns
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++)
    {
        curInfo = &g_LanguagePrefs[languageNr];
        wxString filepattern = curInfo->filepattern;
        filepattern.Lower();
        while (!filepattern.empty())
        {
            wxString cur = filepattern.BeforeFirst(';');
            if ((cur == filename) ||
                (cur == (filename.BeforeLast('.') + wxT(".*"))) ||
                (cur == (wxT("*.") + filename.AfterLast('.'))))
            {
                return curInfo->name;
            }
            filepattern = filepattern.AfterFirst (';');
        }
    }
    return wxEmptyString;
}

bool EditorCtrl::InitializePrefs (const wxString &name)
{
    // initialize styles
    StyleClearAll();
    LanguageInfo const* curInfo = NULL;

    // determine language
    bool found = false;
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++)
    {
        curInfo = &g_LanguagePrefs[languageNr];
        if (curInfo->name == name)
        {
            found = true;
            break;
        }
    }
    if (!found) return false;

    // set lexer and language
    SetLexer(curInfo->lexer);
    m_language = curInfo;

    // set margin for line numbers
    SetMarginType(m_LineNrID, wxSTC_MARGIN_NUMBER);
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(wxT("DARK GREY")));
    StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
    SetMarginWidth(m_LineNrID, 0); // start out not visible

    // annotations style
    //StyleSetBackground(ANNOTATION_STYLE, wxColour(244, 220, 220));
    //StyleSetForeground(ANNOTATION_STYLE, *wxBLACK);
    //StyleSetSizeFractional(ANNOTATION_STYLE,
    //        (StyleGetSizeFractional(wxSTC_STYLE_DEFAULT)*4)/5);

    // default fonts for all styles!
    int Nr;
    for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++)
    {
        wxFont font (10, wxMODERN, wxNORMAL, wxNORMAL);
        StyleSetFont(Nr, font);
    }

    // set common styles
    StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour (wxT("DARK GREY")));
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (wxT("DARK GREY")));

    // initialize settings
    if (g_CommonPrefs.syntaxEnable)
    {
        int keywordnr = 0;
        for (Nr = 0; Nr < STYLE_TYPES_COUNT; Nr++)
        {
            if (curInfo->styles[Nr].type == -1) continue;
            const StyleInfo &curType = g_StylePrefs[curInfo->styles[Nr].type];
            wxFont font(curType.fontsize, wxMODERN, wxNORMAL, wxNORMAL, false,
                        curType.fontname);
            StyleSetFont(Nr, font);
            if (curType.foreground)
            {
                StyleSetForeground(Nr, wxColour (curType.foreground));
            }
            if (curType.background)
            {
                StyleSetBackground(Nr, wxColour (curType.background));
            }
            StyleSetBold(Nr, (curType.fontstyle & mySTC_STYLE_BOLD) > 0);
            StyleSetItalic(Nr, (curType.fontstyle & mySTC_STYLE_ITALIC) > 0);
            StyleSetUnderline(Nr, (curType.fontstyle & mySTC_STYLE_UNDERL) > 0);
            StyleSetVisible(Nr, (curType.fontstyle & mySTC_STYLE_HIDDEN) == 0);
            StyleSetCase(Nr, curType.lettercase);
            const char *pwords = curInfo->styles[Nr].words;
            if (pwords)
            {
                SetKeyWords (keywordnr, pwords);
                keywordnr += 1;
            }
        }
    }

    // set margin as unused
    SetMarginType(m_DividerID, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(m_DividerID, 0);
    SetMarginSensitive(m_DividerID, false);

    // folding
    SetMarginType(m_FoldingID, wxSTC_MARGIN_SYMBOL);
    SetMarginMask(m_FoldingID, wxSTC_MASK_FOLDERS);
    StyleSetBackground(m_FoldingID, *wxWHITE);
    SetMarginWidth(m_FoldingID, 0);
    SetMarginSensitive(m_FoldingID, false);
    if (g_CommonPrefs.foldEnable)
    {
        SetMarginWidth(m_FoldingID, curInfo->folds != 0? m_FoldingMargin: 0);
        SetMarginSensitive(m_FoldingID, curInfo->folds != 0);
        SetProperty(wxT("fold"), curInfo->folds != 0? wxT("1"): wxT("0"));
        SetProperty(wxT("fold.comment"),
                    (curInfo->folds & mySTC_FOLD_COMMENT) > 0? wxT("1"): wxT("0"));
        SetProperty(wxT("fold.bb"),
                    (curInfo->folds & mySTC_FOLD_BB) > 0? wxT("1"): wxT("0"));
    }
    SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

    // set spaces and indention
    SetTabWidth(4);
    SetUseTabs(false);
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);
    SetIndent(g_CommonPrefs.indentEnable? 4: 0);

    // others
    SetViewEOL(g_CommonPrefs.displayEOLEnable);
    SetIndentationGuides(g_CommonPrefs.indentGuideEnable);
    SetEdgeColumn(80);
    SetEdgeMode(g_CommonPrefs.longLineOnEnable? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
    SetViewWhiteSpace(g_CommonPrefs.whiteSpaceEnable?
                        wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
    SetOvertype(g_CommonPrefs.overTypeInitial);
    SetReadOnly(g_CommonPrefs.readOnlyInitial);
    SetWrapMode(g_CommonPrefs.wrapModeInitial?
                wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);

    return true;
}

bool EditorCtrl::LoadFile ()
{
#if wxUSE_FILEDLG
    // get filname
    if (!m_filename) {
        wxFileDialog dlg (this, wxT("Open file"), wxEmptyString, wxEmptyString,
                          wxT("Any file (*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
        if (dlg.ShowModal() != wxID_OK) return false;
        m_filename = dlg.GetPath();
    }

    // load file
    return LoadFile (m_filename);
#else
    return false;
#endif // wxUSE_FILEDLG
}

bool EditorCtrl::LoadFile(const wxString &filename)
{

    // load file in EditorCtrl and clear undo
    if (!filename.empty()) m_filename = filename;

    wxStyledTextCtrl::LoadFile(m_filename);

    EmptyUndoBuffer();

    // determine lexer language
    wxFileName fname(m_filename);
    InitializePrefs(DeterminePrefs(fname.GetFullName()));

    return true;
}

bool EditorCtrl::SaveFile ()
{
#if wxUSE_FILEDLG
    // return if no change
    if (!Modified()) return true;

    // get filname
    if (!m_filename) {
        wxFileDialog dlg (this, wxT("Save file"), wxEmptyString, wxEmptyString, wxT("Any file (*)|*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() != wxID_OK) return false;
        m_filename = dlg.GetPath();
    }

    // save file
    return SaveFile (m_filename);
#else
    return false;
#endif // wxUSE_FILEDLG
}

bool EditorCtrl::SaveFile (const wxString &filename) {

    // return if no change
    if (!Modified()) return true;

//     // save EditorCtrl in file and clear undo
//     if (!filename.empty()) m_filename = filename;
//     wxFile file (m_filename, wxFile::write);
//     if (!file.IsOpened()) return false;
//     wxString buf = GetText();
//     bool okay = file.Write (buf);
//     file.Close();
//     if (!okay) return false;
//     EmptyUndoBuffer();
//     SetSavePoint();

//     return true;

    return wxStyledTextCtrl::SaveFile(filename);

}

bool EditorCtrl::Modified () {

    // return modified state
    return (GetModify() && !GetReadOnly());
}

//----------------------------------------------------------------------------
// EditorCtrlProperties
//----------------------------------------------------------------------------

EditorCtrlProperties::EditorCtrlProperties (EditorCtrl *editor,
                                            long style)
                                            : wxDialog (editor, wxID_ANY, wxEmptyString,
                                                wxDefaultPosition, wxDefaultSize,
                                                style | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{

    // sets the application title
    SetTitle (_("Properties"));
    wxString text;

    // fullname
    wxBoxSizer *fullname = new wxBoxSizer (wxHORIZONTAL);
    fullname->Add (10, 0);
    fullname->Add (new wxStaticText (this, wxID_ANY, _("Full filename"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    fullname->Add (new wxStaticText (this, wxID_ANY, editor->GetFilename()),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

    // text info
    wxGridSizer *textinfo = new wxGridSizer (4, 0, 2);
    textinfo->Add (new wxStaticText (this, wxID_ANY, _("Language"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    textinfo->Add (new wxStaticText (this, wxID_ANY, editor->m_language->name),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    textinfo->Add (new wxStaticText (this, wxID_ANY, _("Lexer-ID: "),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (wxT("%d"), editor->GetLexer());
    textinfo->Add (new wxStaticText (this, wxID_ANY, text),
                   0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    wxString EOLtype = wxEmptyString;
    switch (editor->GetEOLMode())
    {
        case wxSTC_EOL_CR: {EOLtype = wxT("CR (Unix)"); break; }
        case wxSTC_EOL_CRLF: {EOLtype = wxT("CRLF (Windows)"); break; }
        case wxSTC_EOL_LF: {EOLtype = wxT("CR (Macintosh)"); break; }
    }
    textinfo->Add (new wxStaticText (this, wxID_ANY, _("Line endings"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    textinfo->Add (new wxStaticText (this, wxID_ANY, EOLtype),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // text info box
    wxStaticBoxSizer *textinfos = new wxStaticBoxSizer (
                     new wxStaticBox (this, wxID_ANY, _("Informations")),
                     wxVERTICAL);
    textinfos->Add (textinfo, 0, wxEXPAND);
    textinfos->Add (0, 6);

    // statistic
    wxGridSizer *statistic = new wxGridSizer (4, 0, 2);
    statistic->Add (new wxStaticText (this, wxID_ANY, _("Total lines"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (wxT("%d"), editor->GetLineCount());
    statistic->Add (new wxStaticText (this, wxID_ANY, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, wxID_ANY, _("Total chars"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (wxT("%d"), editor->GetTextLength());
    statistic->Add (new wxStaticText (this, wxID_ANY, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, wxID_ANY, _("Current line"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (wxT("%d"), editor->GetCurrentLine());
    statistic->Add (new wxStaticText (this, wxID_ANY, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, wxID_ANY, _("Current pos"),
                                     wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (wxT("%d"), editor->GetCurrentPos());
    statistic->Add (new wxStaticText (this, wxID_ANY, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // char/line statistics
    wxStaticBoxSizer *statistics = new wxStaticBoxSizer (
                     new wxStaticBox (this, wxID_ANY, _("Statistics")),
                     wxVERTICAL);
    statistics->Add (statistic, 0, wxEXPAND);
    statistics->Add (0, 6);

    // total pane
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (fullname, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    totalpane->Add (0, 6);
    totalpane->Add (textinfos, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 10);
    totalpane->Add (statistics, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 6);
    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (totalpane);

    ShowModal();
}
