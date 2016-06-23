/***************************************************************
 * Name:      CodeGen.h
 * Purpose:   Defines Asm Code Generation Class
 * Author:    A. Berestyuk (elsonriente.work@gmail.com)
 * Created:   2016-06-18
 * Copyright: A. Berestyuk ()
 * License:   zlib (http://www.gzip.org/zlib/zlib_license.html)
 **************************************************************/

#ifndef CODEGEN_H
#define CODEGEN_H

//--------------------------------------------------------------
// headers
//--------------------------------------------------------------

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/textdlg.h"
#endif

//! headers
#include <vector>

//! wxWidgets headers
#include <wx/stc/stc.h>  // styled text control

//! application headers
#include "EditorCtrl.h" // editor

//--------------------------------------------------------------
// declarations
//--------------------------------------------------------------

//! Instruction types
enum instructionType
{
    // binary operation assign
    tacINSTR_ASSIGN_BIN_OP = 0,
    // unary operation assign
    tacINSTR_ASSIGN_UN_OP,
    // copy operation
    tacINSTR_ASSIGN_COPY,
    // unconditional goto
    tacINSTR_GOTO,
    // conditional operations
    tacINSTR_IF_TRUE,
    tacINSTR_IF_FALSE,
    // procedure/function call
    tacINSTR_PARAM,
    tacINSTR_CALL_PROC,
    tacINSTR_CALL_FUNC,
    tacINSTR_RETURN_PROC,
    tacINSTR_RETURN_FUNC,
    // indexed assign
    tacINSTR_ASSIGN_INDEXED_ARG1,
    tacINSTR_ASSIGN_INDEXED_RESULT,
    // pointer and address assign
    tacINSTR_ASSIGN_ADDRESS,
    tacINSTR_ASSIGN_PTR_ARG1,
    tacINSTR_ASSIGN_PTR_RESULT
};

//! Operator types
enum opType
{
    // assign
    tacASSIGN = 0,
    // (un)conditional goto
    tacIF_TRUE,
    tacIF_FALSE,
    tacGOTO,
    tacPARAM,
    tacCALL,
    tacRETURN
};

//! Lexem types
enum lexType
{
    tacOPERATOR,
    tacVAR,
    tacCONST,
    tacLABEL
};

struct TACLexem
{
    lexType type;
    int startPos;
    int endPos;
    TACLexem (lexType newType, int newStartPos, int newEndPos)
                : type(newType), startPos(newStartPos), endPos(newEndPos) {}
};

class TACode;

//--------------------------------------------------------------
//! CodeGen
class CodeGen
{
public:

    //! constructor
    CodeGen(EditorCtrl *editor, wxTextCtrl *codewin, wxTextCtrl *logwin);

    bool generate();
    TACode* GetProgram() { return m_program; }

    //! destructor
    ~CodeGen();

private:
    // three address code program
    // editor
    EditorCtrl *m_editor;
    // code output window
    wxTextCtrl *m_codewin;
    // log window
    wxTextCtrl *m_logwin;

    // three address code program
    TACode *m_program;
};

//--------------------------------------------------------------
//! TACLine
class TACLine
{
public:

    //! constructor
    TACLine(TACLexem n_op, TACLexem n_arg1, TACLexem n_arg2, TACLexem n_result);
    //! destructor
    ~TACLine();

private:
    // text line No
    int line;
    // operator
    TACLexem op;
    // operand 1
    TACLexem arg1;
    // operand 2
    TACLexem arg2;
    // result
    TACLexem result;
};

//--------------------------------------------------------------
//! TACode
class TACode
{
public:

    //! constructor
    TACode();
    //! destructor
    ~TACode();

private:
    // three address code lines
    std::vector<TACLine> m_lines;

};


#endif // CODEGEN_H
