/***************************************************************
 * Name:      CodeGen.cpp
 * Purpose:   Code for Asm Code Generation Class
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-06-18
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
//#include <algorithm>

//! wxWidgets headers
#include "wx/string.h"   // strings

//! application headers
//#include "../include/Definitions.h" // definitions
#include "../include/CodeGen.h"

//--------------------------------------------------------------
// implementation
//--------------------------------------------------------------

//--------------------------------------------------------------
// CodeGen
//--------------------------------------------------------------
CodeGen::CodeGen(EditorCtrl *editor, wxTextCtrl *codewin, wxTextCtrl *logwin)
{
    // important variables
    // editor
    EditorCtrl *m_editor = editor;
    // code output window
    wxTextCtrl *m_codewin = codewin;
    // log window
    wxTextCtrl *m_log = logwin;
}

CodeGen::~CodeGen()
{
    // free resources
    if (m_program)
        delete m_program;
}

//--------------------------------------------------------------
// TACLine
//--------------------------------------------------------------

// constructor
TACLine::TACLine(TACLexem n_op, TACLexem n_arg1, TACLexem n_arg2, TACLexem n_result)
                : op(n_op), arg1(n_arg1), arg2(n_arg2), result(n_result)
{
}
// destructor
TACLine::~TACLine()
{
}

//--------------------------------------------------------------
// TACode
//--------------------------------------------------------------

// constructor
TACode::TACode()
{
}
// destructor
TACode::~TACode()
{
}
