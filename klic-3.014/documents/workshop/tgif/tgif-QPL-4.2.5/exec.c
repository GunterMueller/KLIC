/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * Copyright (C) 2001-2009, William Chia-Wei Cheng.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/exec.c,v 1.59 2011/05/18 22:41:20 william Exp $
 */

#define _INCLUDE_FROM_EXEC_C_

#include "tgifdefs.h"
#include "patchlvl.h"
#include "expfdefs.h"
#include "cmdids.h"

#include "align.e"
#include "arc.e"
#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "cmd.e"
#include "color.e"
#include "choice.e"
#include "cursor.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "edit.e"
#include "eps.e"
#include "exec.e"
#include "expr.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "import.e"
#include "ini.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "pngtrans.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rcbox.e"
#include "rect.e"
#include "remote.e"
#include "ruler.e"
#include "select.e"
#include "setup.e"
#include "shortcut.e"
#include "special.e"
#include "stk.e"
#include "stretch.e"
#include "strtbl.e"
#include "tangram2.e"
#include "tcp.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "wb.e"
#include "xbitmap.e"
#include "xpixmap.e"
#include "xprtfltr.e"
#include "z_intrf.e"

int execAnimating=FALSE;
int execAnimateRedraw=FALSE;
int execCurDepth=0;
int replaceAttrFirstValueRedraw=TRUE;
int execNavigateBack=FALSE;
int userAbortExec=FALSE;

static char gsPath[MAXSTRING+1];

static int execInterruptEnabled=TRUE;
static int execInterruptQueued=FALSE;

struct AttrRec *warpToAttr=NULL;

char *cmdToExecAfterHyperJump=NULL;

struct StrRec *topTmpStr=NULL;
struct StrRec *botTmpStr=NULL;

#define MAXEXECOPENFILES 16

struct OpenFileRec {
   FILE *fp;
   char *fname;
   int eof;
} gaOpenFileInfo[MAXEXECOPENFILES];

#define TOK_INVALID	(INVALID)
#define TOK_EMPTY	0
#define TOK_STR		1
#define TOK_LEFT_P	2
#define TOK_RIGHT_P	3
#define TOK_LEFT_B	4
#define TOK_RIGHT_B	5
#define TOK_LEFT_CB	6
#define TOK_RIGHT_CB	7
#define TOK_COMMA	8
#define TOK_SEMI	9

static char execDummyStr[2048];
static int gnAbortExec=FALSE;
static int gnStopCmdExecuted=TRUE;

int ExecLaunch ARGS_DECL((char**, struct ObjRec *, char*));
int ExecExec ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMktemp ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUseTemplate ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUpdEPSChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUpdXBMChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUpdXPMChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDelEPSChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDelXBMChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDelXPMChild ARGS_DECL((char**, struct ObjRec *, char*));
int ExecFlipDeck ARGS_DECL((char**, struct ObjRec *, char*));
int ExecReadFileIntoAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecWriteAttrIntoFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecAppendAttrIntoFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSelectObjByName ARGS_DECL((char**, struct ObjRec *, char*));
void ExecSelectTopObj ARGS_DECL((struct ObjRec *, char*));
void ExecDelAllSelObj ARGS_DECL((struct ObjRec *, char*));
void ExecUnSelectAllObj ARGS_DECL((struct ObjRec *, char*));
int ExecMoveSelObjRel ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRepeat ARGS_DECL((char**, struct ObjRec *, char*));
int ExecHyperJump ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMakeCGIQuery ARGS_DECL((char**, struct ObjRec *, char*));
int ExecWaitClick ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSleep ARGS_DECL((char**, struct ObjRec *, char*));
void ExecBeginAnimate ARGS_DECL((struct ObjRec *, char*));
void ExecEndAnimate ARGS_DECL((struct ObjRec *, char*));
int ExecSetRedraw ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjColor ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjFill ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjPen ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjLineWidth ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjSpline ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjArrow ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjDash ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjTransPat ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelObjRCBRadius ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextVSpace ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextJust ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextFont ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextStyle ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextSize ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextUnderline ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetSelTextOverline ARGS_DECL((char**, struct ObjRec *, char*));
int ExecInc ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDec ARGS_DECL((char**, struct ObjRec *, char*));
int ExecShuffleObjToTop ARGS_DECL((char**, struct ObjRec *, char*));
int ExecShuffleObjToBot ARGS_DECL((char**, struct ObjRec *, char*));
void ExecDisableUndo ARGS_DECL((struct ObjRec *, char*));
void ExecEnableUndo ARGS_DECL((struct ObjRec *, char*));
int ExecGetDrawingArea ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetSelObjBBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetNamedObjBBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMoveSelObjAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecAssign ARGS_DECL((char**, struct ObjRec *, char*));
int ExecStrCpy ARGS_DECL((char**, struct ObjRec *, char*));
int ExecStrCat ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCopyStrToCutBuffer ARGS_DECL((char**, struct ObjRec *, char*));
int ExecWhile ARGS_DECL((char**, char**, struct ObjRec *, char*));
int ExecIf ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetCurrentFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetCurrentExportFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetCurrentDir ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetEnv ARGS_DECL((char**, struct ObjRec *, char*));
int ExecStrLen ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSubStr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecStrStr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecStrRStr ARGS_DECL((char**, struct ObjRec *, char*));
void ExecUnMakeSelObjIconic ARGS_DECL((struct ObjRec *, char*));
int ExecHyperJumpThenExec ARGS_DECL((char**, struct ObjRec *, char*));
int ExecShowAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecHideAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecShowAttrName ARGS_DECL((char**, struct ObjRec *, char*));
int ExecHideAttrName ARGS_DECL((char**, struct ObjRec *, char*));
int ExecShowValue ARGS_DECL((char**, struct ObjRec *, char*));
int ExecHideValue ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetAttrBBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSizeSelObjAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSizeNamedObjAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMessageBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetUserInput ARGS_DECL((char**, struct ObjRec *, char*));
int ExecAddAttrToSelObj ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDelAttrFromSelObj ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUserEndAnEdge ARGS_DECL((char**, struct ObjRec *, char*));
int ExecUserDrawAnEdge ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetAPolyVertexAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMoveAPolyVertexAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecPostAttrAndGetCGI ARGS_DECL((char**, struct ObjRec *, char*));
void ExecNavigateBack ARGS_DECL((struct ObjRec *, char*));
void ExecStop ARGS_DECL((struct ObjRec *, char*));
int ExecSqrt ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRandom ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSRand48 ARGS_DECL((char**, struct ObjRec *, char*));
int ExecDRand48 ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRound ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRedrawObj ARGS_DECL((char**, struct ObjRec *, char*));
void ExecRedrawDrawingArea ARGS_DECL((struct ObjRec *, char*));
int ExecIntToHex ARGS_DECL((char**, struct ObjRec *, char*));
int ExecForI ARGS_DECL((char**, struct ObjRec *, char*));
void ExecSetFileNotModified ARGS_DECL((struct ObjRec *, char*));
int ExecNewId ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRotateSelObj ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCallSimpleShortCut ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCallOneArgShortCut ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSubstituteAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetFileSize ARGS_DECL((char**, struct ObjRec *, char*));
int ExecIsFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecIndex ARGS_DECL((char**, struct ObjRec *, char*));
int ExecRIndex ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetNumberOfLines ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetLineInAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecTrim ARGS_DECL((char**, struct ObjRec *, char*));
int ExecIsAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecFindObjNames ARGS_DECL((char**, struct ObjRec *, char*));
int ExecFindObjNamesOnAllPgs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecTg2FindObjNsOnAllPgs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecTokenize ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMoveAttrRel ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetNumberOfVs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecIsObjTransformed ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMakeSelObjIconic ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMoveNamedObjRel ARGS_DECL((char**, struct ObjRec *, char*));
int ExecMoveNamedObjAbs ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetTgifVersion ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetTgifDir ARGS_DECL((char**, struct ObjRec *, char*));
int ExecGetProfileString ARGS_DECL((char**, struct ObjRec *, char*));
int ExecWriteProfileString ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSelectAdditionalObj ARGS_DECL((char**, struct ObjRec *, char*));
int ExecOpenFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCloseFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecReadFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecWriteFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecFlushFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecAppendFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetOutputFormat ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetExportClipRect ARGS_DECL((char**, struct ObjRec *, char*));
int ExecImportFile ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSetXpmOutputVersion ARGS_DECL((char**, struct ObjRec *, char*));
int ExecEditIniSection ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSelectFromIniSection ARGS_DECL((char**, struct ObjRec *, char*));
int ExecAppendLineIntoAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecInsertLineIntoAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecClearAttr ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateText ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateCornerOval ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateCenterOval ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateEdgeOval ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateRCBox ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateArc ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateFirstVertex ARGS_DECL((char**, struct ObjRec *, char*));
int ExecCreateNextVertex ARGS_DECL((char**, struct ObjRec *, char*));
void ExecCreatePoly ARGS_DECL((struct ObjRec *, char*));
void ExecCreatePolygon ARGS_DECL((struct ObjRec *, char*));
void ExecStartCreateGroup ARGS_DECL((struct ObjRec *, char*));
void ExecCreateGroup ARGS_DECL((struct ObjRec *, char*));
int ExecSetAllowInterrupt ARGS_DECL((char**, struct ObjRec *, char*));
int ExecSelectEachObjAndExec ARGS_DECL((char**, struct ObjRec *, char*));
int ExecEditAttrInTextMode ARGS_DECL((char**, struct ObjRec *, char*));
void ExecSetFileUnSavable ARGS_DECL((struct ObjRec *, char*));
int ExecPsToEpsi ARGS_DECL((char**, struct ObjRec *, char*));
int ExecObjsBBoxIntersect ARGS_DECL((char**, struct ObjRec *, char*));
void ExecDelAllAttrFromSelObj ARGS_DECL((struct ObjRec *, char*));
int ExecRndPermLinesInAttr ARGS_DECL((char**, struct ObjRec *, char*));

static
ExecInfo gExecInfo[] = {
   /* do not translate -- program constants */
   { (NLFN*)ExecLaunch,              "launch",                            1, 0},
   { (NLFN*)ExecExec,                "exec",                              1, 0},
   { (NLFN*)ExecMktemp,              "mktemp",                            2, 0},
   { (NLFN*)ExecUseTemplate,         "create_file_using_simple_template", 4, 0},
   { (NLFN*)ExecUpdEPSChild,         "update_eps_child",                  1, 0},
   { (NLFN*)ExecUpdXBMChild,         "update_xbm_child",                  1, 0},
   { (NLFN*)ExecUpdXPMChild,         "update_xpm_child",                  1, 0},
   { (NLFN*)ExecDelEPSChild,         "delete_eps_child",                  1, 0},
   { (NLFN*)ExecDelXBMChild,         "delete_xbm_child",                  1, 0},
   { (NLFN*)ExecDelXPMChild,         "delete_xpm_child",                  1, 0},
   { (NLFN*)ExecFlipDeck,            "flip_deck",                         3, 0},
   { (NLFN*)ExecReadFileIntoAttr,    "read_file_into_attr",               2, 0},
   { (NLFN*)ExecWriteAttrIntoFile,   "write_attr_into_file",              2, 0},
   { (NLFN*)ExecAppendAttrIntoFile,  "append_attr_into_file",             2, 0},
   { (NLFN*)ExecSelectObjByName,     "select_obj_by_name",                1, 0},
   { (NLFN*)ExecSelectTopObj,        "select_top_obj",                    0, 0},
   { (NLFN*)ExecDelAllSelObj,        "delete_selected_obj",               0, 0},
   { (NLFN*)ExecUnSelectAllObj,      "unselect_all_obj",                  0, 0},
   { (NLFN*)ExecMoveSelObjRel,       "move_selected_obj_relative",        2, 0},
   { (NLFN*)ExecRepeat,              "repeat",                            2, 0},
   { (NLFN*)ExecHyperJump,           "hyperjump",                         1, 0},
   { (NLFN*)ExecMakeCGIQuery,        "make_cgi_query",                    3, 0},
   { (NLFN*)ExecWaitClick,           "wait_click",                        3, 0},
   { (NLFN*)ExecSleep,               "sleep",                             2, 0},
   { (NLFN*)ExecBeginAnimate,        "begin_animate",                     0, 0},
   { (NLFN*)ExecEndAnimate,          "end_animate",                       0, 0},
   { (NLFN*)ExecSetRedraw,           "set_redraw",                        1, 0},
   { (NLFN*)ExecSetSelObjColor,      "set_selected_obj_color",            1, 0},
   { (NLFN*)ExecSetSelObjFill,       "set_selected_obj_fill",             1, 0},
   { (NLFN*)ExecSetSelObjPen,        "set_selected_obj_pen",              1, 0},
   { (NLFN*)ExecSetSelObjLineWidth,  "set_selected_obj_line_width",       3, 0},
   { (NLFN*)ExecSetSelObjSpline,     "set_selected_obj_spline",           1, 0},
   { (NLFN*)ExecSetSelObjArrow,      "set_selected_obj_arrow",            1, 0},
   { (NLFN*)ExecSetSelObjDash,       "set_selected_obj_dash",             1, 0},
   { (NLFN*)ExecSetSelObjTransPat,   "set_selected_obj_trans_pat",        1, 0},
   { (NLFN*)ExecSetSelObjRCBRadius,  "set_selected_obj_rcb_radius",       1, 0},
   { (NLFN*)ExecSetSelTextVSpace,    "set_selected_text_vspace",          1, 0},
   { (NLFN*)ExecSetSelTextJust,      "set_selected_text_just",            1, 0},
   { (NLFN*)ExecSetSelTextFont,      "set_selected_text_font",            1, 0},
   { (NLFN*)ExecSetSelTextStyle,     "set_selected_text_style",           1, 0},
   { (NLFN*)ExecSetSelTextSize,      "set_selected_text_size",            1, 0},
   { (NLFN*)ExecSetSelTextUnderline, "set_selected_text_underline",       1, 0},
   { (NLFN*)ExecSetSelTextOverline,  "set_selected_text_overline",        1, 0},
   { (NLFN*)ExecInc,                 "inc",                               2, 0},
   { (NLFN*)ExecDec,                 "dec",                               2, 0},
   { (NLFN*)ExecShuffleObjToTop,     "shuffle_obj_to_top",                1, 0},
   { (NLFN*)ExecShuffleObjToBot,     "shuffle_obj_to_bottom",             1, 0},
   { (NLFN*)ExecDisableUndo,         "disable_undo",                      0, 0},
   { (NLFN*)ExecEnableUndo,          "enable_undo",                       0, 0},
   { (NLFN*)ExecGetDrawingArea,      "get_drawing_area",                  4, 0},
   { (NLFN*)ExecGetSelObjBBox,       "get_selected_obj_bbox",             4, 0},
   { (NLFN*)ExecGetNamedObjBBox,     "get_named_obj_bbox",                5, 0},
   { (NLFN*)ExecMoveSelObjAbs,       "move_selected_obj_absolute",        2, 0},
   { (NLFN*)ExecAssign,              "assign",                            2, 0},
   { (NLFN*)ExecStrCpy,              "strcpy",                            2, 0},
   { (NLFN*)ExecStrCat,              "strcat",                            2, 0},
   { (NLFN*)ExecCopyStrToCutBuffer,  "copy_string_to_cut_buffer",         1, 0},
   { (NLFN*)ExecWhile,               "while",                            -2, 1},
   { (NLFN*)ExecIf,                  "if",                                3, 1},
   { (NLFN*)ExecGetCurrentFile,      "get_current_file",                  1, 0},
   { (NLFN*)ExecGetCurrentExportFile,"get_current_export_file",           1, 0},
   { (NLFN*)ExecGetCurrentDir,       "get_current_dir",                   1, 0},
   { (NLFN*)ExecGetEnv,              "getenv",                            2, 0},
   { (NLFN*)ExecStrLen,              "strlen",                            2, 1},
   { (NLFN*)ExecSubStr,              "substr",                            4, 1},
   { (NLFN*)ExecStrStr,              "strstr",                            3, 1},
   { (NLFN*)ExecStrRStr,             "strrstr",                           3, 1},
   { (NLFN*)ExecUnMakeSelObjIconic,  "unmake_selected_obj_iconic",        0, 0},
   { (NLFN*)ExecHyperJumpThenExec,   "hyperjump_then_exec",               2, 0},
   { (NLFN*)ExecShowAttr,            "show_attr",                         1, 0},
   { (NLFN*)ExecHideAttr,            "hide_attr",                         1, 0},
   { (NLFN*)ExecShowAttrName,        "show_attr_name",                    1, 0},
   { (NLFN*)ExecHideAttrName,        "hide_attr_name",                    1, 0},
   { (NLFN*)ExecShowValue,           "show_value",                        1, 0},
   { (NLFN*)ExecHideValue,           "hide_value",                        1, 0},
   { (NLFN*)ExecGetAttrBBox,         "get_attr_bbox",                     5, 0},
   { (NLFN*)ExecSizeSelObjAbs,       "size_selected_obj_absolute",        2, 0},
   { (NLFN*)ExecSizeNamedObjAbs,     "size_named_obj_absolute",           3, 0},
   { (NLFN*)ExecMessageBox,          "message_box",                       4, 0},
   { (NLFN*)ExecGetUserInput,        "get_user_input",                    3, 0},
   { (NLFN*)ExecAddAttrToSelObj,     "add_attr_to_selected_obj",          4, 1},
   { (NLFN*)ExecDelAttrFromSelObj,   "delete_attr_from_selected_obj",     1, 0},
   { (NLFN*)ExecUserEndAnEdge,       "user_end_an_edge",                  3, 1},
   { (NLFN*)ExecUserDrawAnEdge,      "user_draw_an_edge",                 2, 0},
   { (NLFN*)ExecGetAPolyVertexAbs,   "get_a_poly_vertex_absolute",        4, 0},
   { (NLFN*)ExecMoveAPolyVertexAbs,  "move_a_poly_vertex_absolute",       4, 0},
   { (NLFN*)ExecPostAttrAndGetCGI,   "post_attr_and_get_cgi_result",      3, 0},
   { (NLFN*)ExecNavigateBack,        "navigate_back",                     0, 0},
   { (NLFN*)ExecStop,                "stop",                              0, 0},
   { (NLFN*)ExecSqrt,                "sqrt",                              2, 0},
   { (NLFN*)ExecRandom,              "random",                            1, 0},
   { (NLFN*)ExecSRand48,             "srand48",                           1, 0},
   { (NLFN*)ExecDRand48,             "drand48",                           1, 0},
   { (NLFN*)ExecRound,               "round",                             2, 0},
   { (NLFN*)ExecRedrawObj,           "redraw_obj",                        1, 0},
   { (NLFN*)ExecRedrawDrawingArea,   "redraw_drawing_area",               0, 0},
   { (NLFN*)ExecIntToHex,            "itox",                              3, 0},
   { (NLFN*)ExecForI,                "for_i",                             5, 0},
   { (NLFN*)ExecSetFileNotModified,  "set_file_not_modified",             0, 0},
   { (NLFN*)ExecNewId,               "new_id",                            1, 0},
   { (NLFN*)ExecRotateSelObj,        "rotate_selected_obj",               1, 0},
   { (NLFN*)ExecCallSimpleShortCut,  "call_simple_shortcut",              1, 0},
   { (NLFN*)ExecCallOneArgShortCut,  "call_one_arg_shortcut",             2, 0},
   { (NLFN*)ExecSubstituteAttr,      "substitute_attr",                   4, 0},
   { (NLFN*)ExecGetFileSize,         "get_file_size",                     2, 0},
   { (NLFN*)ExecIsFile,              "is_file",                           2, 0},
   { (NLFN*)ExecIndex,               "index",                             3, 1},
   { (NLFN*)ExecRIndex,              "rindex",                            3, 1},
   { (NLFN*)ExecGetNumberOfLines,    "get_number_of_lines_in_attr",       2, 0},
   { (NLFN*)ExecGetLineInAttr,       "get_line_in_attr",                  3, 0},
   { (NLFN*)ExecTrim,                "trim",                              1, 1},
   { (NLFN*)ExecIsAttr,              "is_attr",                           2, 0},
   { (NLFN*)ExecFindObjNames,        "find_obj_names",                    3, 0},
   { (NLFN*)ExecFindObjNamesOnAllPgs,"find_obj_names_on_all_pages",       2, 0},
   { (NLFN*)ExecTg2FindObjNsOnAllPgs,"tg2_find_obj_names_on_all_pages",   2, 0},
   { (NLFN*)ExecTokenize,            "tokenize",                          3, 0},
   { (NLFN*)ExecMoveAttrRel,         "move_attr_relative",                3, 0},
   { (NLFN*)ExecGetNumberOfVs,       "get_number_of_vertices",            2, 0},
   { (NLFN*)ExecIsObjTransformed,    "is_obj_transformed",                2, 0},
   { (NLFN*)ExecMakeSelObjIconic,    "make_selected_obj_iconic",          1, 0},
   { (NLFN*)ExecMoveNamedObjRel,     "move_named_obj_relative",           3, 0},
   { (NLFN*)ExecMoveNamedObjAbs,     "move_named_obj_absolute",           3, 0},
   { (NLFN*)ExecGetTgifVersion,      "get_tgif_version",                  4, 0},
   { (NLFN*)ExecGetTgifDir,          "get_tgif_dir",                      1, 0},
   { (NLFN*)ExecGetProfileString,    "get_profile_string",                5, 0},
   { (NLFN*)ExecWriteProfileString,  "write_profile_string",              4, 0},
   { (NLFN*)ExecSelectAdditionalObj, "select_additional_obj",             1, 0},
   { (NLFN*)ExecOpenFile,            "open_file",                         3, 0},
   { (NLFN*)ExecCloseFile,           "close_file",                        1, 0},
   { (NLFN*)ExecReadFile,            "read_file",                         2, 0},
   { (NLFN*)ExecWriteFile,           "write_file",                        2, 0},
   { (NLFN*)ExecFlushFile,           "flush_file",                        1, 0},
   { (NLFN*)ExecAppendFile,          "append_file",                       2, 0},
   { (NLFN*)ExecSetOutputFormat,     "set_output_format",                 2, 0},
   { (NLFN*)ExecSetExportClipRect,   "set_export_clip_rect",              4, 0},
   { (NLFN*)ExecImportFile,          "import_file",                       4, 0},
   { (NLFN*)ExecSetXpmOutputVersion, "set_xpm_output_version",            1, 0},
   { (NLFN*)ExecEditIniSection,      "edit_ini_section",                  4, 0},
   { (NLFN*)ExecSelectFromIniSection,"select_from_ini_section",           4, 0},
   { (NLFN*)ExecAppendLineIntoAttr,  "append_line_into_attr",             2, 0},
   { (NLFN*)ExecInsertLineIntoAttr,  "insert_line_into_attr",             3, 0},
   { (NLFN*)ExecClearAttr,           "clear_attr",                        1, 0},
   { (NLFN*)ExecCreateText,          "create_text_obj",                   3, 0},
   { (NLFN*)ExecCreateBox,           "create_box_obj",                    4, 0},
   { (NLFN*)ExecCreateCornerOval,    "create_corner_oval_obj",            4, 0},
   { (NLFN*)ExecCreateCenterOval,    "create_center_oval_obj",            3, 0},
   { (NLFN*)ExecCreateEdgeOval,      "create_edge_oval_obj",              4, 0},
   { (NLFN*)ExecCreateRCBox,         "create_rcbox_obj",                  4, 0},
   { (NLFN*)ExecCreateArc,           "create_arc_obj",                    6, 0},
   { (NLFN*)ExecCreateFirstVertex,   "create_first_vertex",               2, 0},
   { (NLFN*)ExecCreateNextVertex,    "create_next_vertex",                2, 0},
   { (NLFN*)ExecCreatePoly,          "create_poly_obj",                   0, 0},
   { (NLFN*)ExecCreatePolygon,       "create_polygon_obj",                0, 0},
   { (NLFN*)ExecStartCreateGroup,    "start_create_group_obj",            0, 0},
   { (NLFN*)ExecCreateGroup,         "create_group_obj",                  0, 0},
   { (NLFN*)ExecSetAllowInterrupt,   "set_allow_interrupt",               1, 0},
   { (NLFN*)ExecSelectEachObjAndExec,"select_each_obj_and_exec",          1, 0},
   { (NLFN*)ExecEditAttrInTextMode,  "edit_attr_in_text_mode",            1, 0},
   { (NLFN*)ExecSetFileUnSavable,    "set_file_unsavable",                0, 0},
   { (NLFN*)ExecPsToEpsi,            "pstoepsi",                          3, 0},
   { (NLFN*)ExecObjsBBoxIntersect,   "objs_bbox_intersect",               3, 0},
   { (NLFN*)ExecDelAllAttrFromSelObj,"delete_all_attr_from_selected_objs",0, 0},
   { (NLFN*)ExecRndPermLinesInAttr,  "random_permute_lines_in_attr",      1, 0},
   { NULL, NULL, 0, 0 }
};

void CleanTmpStr()
{
   struct StrRec *next_str=NULL;

   for ( ; topTmpStr != NULL; topTmpStr=next_str) {
      next_str = topTmpStr->next;
      FreeStr(topTmpStr);
   }
   topTmpStr = botTmpStr = NULL;
}

int PrependToTmpStr(psz)
   char *psz;
{
   struct StrRec *str_ptr=NewStr();

   if (str_ptr == NULL) return FALSE;

   DynStrSet(&str_ptr->dyn_str, psz);

   str_ptr->prev = NULL;
   str_ptr->next = topTmpStr;

   if (topTmpStr == NULL) {
      botTmpStr = str_ptr;
   } else {
      topTmpStr->prev = str_ptr;
   }
   topTmpStr = str_ptr;
   return TRUE;
}

int AppendToTmpStr(psz)
   char *psz;
{
   struct StrRec *str_ptr=NewStr();

   if (str_ptr == NULL) return FALSE;

   DynStrSet(&str_ptr->dyn_str, psz);

   str_ptr->prev = botTmpStr;
   str_ptr->next = NULL;

   if (botTmpStr == NULL) {
      topTmpStr = str_ptr;
   } else {
      botTmpStr->next = str_ptr;
   }
   botTmpStr = str_ptr;
   return TRUE;
}

int AppendToTmpStr2(psz1, psz2, tg2)
   char *psz1, *psz2;
{
   int len1=strlen(psz1), len2=strlen(psz2), rc=0;
   char *psz=(char*)malloc((len1+len2+1)*sizeof(char));

   if (psz == NULL) FailAllocMessage();
   if (tg2) {
      sprintf(psz, "%s%s", psz2, psz1);
   } else {
      sprintf(psz, "%s%s", psz1, psz2);
   }
   rc = AppendToTmpStr(psz);
   free(psz);

   return rc;
}

int ExecuteCmdGetResult(cmd, result_list, max_lines)
   char *cmd;
   CVList *result_list;
   int max_lines;
{
   char *buf=NULL;
   FILE *pfp=NULL;
   int count=0;

   EndMeasureTooltip(FALSE);
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   SetStringStatus(gszMsgBox);
   if (!PRTGIF) XSync(mainDisplay, False);
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      return FALSE;
   }
   while ((buf=UtilGetALine(pfp)) != NULL) {
      if (PRTGIF) {
         fprintf(stderr, "%s", buf);
      } else {
         Msg(buf);
      }
      if (max_lines == (-1) || count++ < max_lines) {
         ListAppend(result_list, buf);
      }
   }
   pclose(pfp);
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));

   if (max_lines != (-1) && count < max_lines) {
      return FALSE;
   }
   return TRUE;
}

struct ObjRec *FindObjWithName(BotObj, OrigObj, obj_name, inside_root_obj,
      inside_this_obj, pp_owner_obj, pp_top_owner)
   struct ObjRec *BotObj, *OrigObj, **pp_owner_obj, **pp_top_owner;
   char *obj_name;
   int inside_root_obj, inside_this_obj;
{
   register struct AttrRec *attr_ptr=NULL;
   register struct ObjRec *obj_ptr=NULL;
   struct ObjRec *bot_obj=BotObj;
   char *bang_ptr=NULL;

   if (obj_name == NULL || *obj_name == '\0') return NULL;
   if (*obj_name == '!') {
      if (inside_root_obj || inside_this_obj) return NULL;
      obj_name++;
      inside_root_obj = TRUE;
      inside_this_obj = FALSE;
      if (pp_owner_obj != NULL) *pp_owner_obj = NULL;
      if (pp_top_owner != NULL) *pp_top_owner = NULL;
      bot_obj = botObj;
      if (*obj_name == '*') {
         if (topSel == NULL) return NULL;
         obj_name++;
         if (pp_owner_obj != NULL) *pp_owner_obj = topSel->obj;
         if (pp_top_owner != NULL) *pp_top_owner = topSel->obj;
         if (*obj_name == '\0') {
            return topSel->obj;
         }
         switch (topSel->obj->type) {
         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
         case OBJ_PIN: break;

         default: return NULL;
         }
         return FindObjWithName(topSel->obj->detail.r->last, OrigObj,
               obj_name, TRUE, FALSE, pp_owner_obj, pp_top_owner);
      }
   }
   if (*obj_name == '!') return NULL;

   if ((bang_ptr=strchr(obj_name, '!')) != NULL) {
      *bang_ptr = '\0';
      /* do not translate -- program constants */
      if (OrigObj != NULL && strcmp(obj_name, "THIS") == 0) {
         int this_ok=FALSE;

         *bang_ptr++ = '!';
         if (inside_root_obj || inside_this_obj) return NULL;
         switch (OrigObj->type) {
         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
         case OBJ_PIN: break;

         default: return NULL;
         }
         for (obj_ptr=botObj; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            if (obj_ptr == OrigObj) {
               this_ok = TRUE;
               break;
            }
         }
         if (!this_ok) return NULL;
         if (pp_top_owner != NULL) *pp_top_owner = OrigObj;
         if (pp_owner_obj != NULL) *pp_owner_obj = OrigObj;

         return FindObjWithName(OrigObj->detail.r->last, OrigObj,
               bang_ptr, FALSE, TRUE, pp_owner_obj, pp_top_owner);
      } else {
         for (obj_ptr=bot_obj; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            if (obj_ptr->fattr != NULL &&
                  (attr_ptr=FindAttrWithName(obj_ptr,"name=",NULL)) != NULL &&
                  strcmp(attr_ptr->attr_value.s, obj_name) == 0) {
               *bang_ptr++ = '!';
               switch (obj_ptr->type) {
               case OBJ_GROUP:
               case OBJ_ICON:
               case OBJ_SYM:
               case OBJ_PIN: break;

               default: return NULL;
               }
               if (pp_owner_obj != NULL) *pp_owner_obj = obj_ptr;
               if (pp_top_owner != NULL && bot_obj == botObj) {
                  *pp_top_owner = obj_ptr;
               }
               return FindObjWithName(obj_ptr->detail.r->last, obj_ptr,
                     bang_ptr, inside_root_obj, inside_this_obj, pp_owner_obj,
                     pp_top_owner);
            }
         }
         *bang_ptr = '!';
         return NULL;
      }
   } else if (strcmp(obj_name, "THIS") == 0) {
      int this_ok=FALSE;

      if (inside_root_obj || inside_this_obj) return NULL;
      for (obj_ptr=botObj; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
         if (obj_ptr == OrigObj) {
            this_ok = TRUE;
            break;
         }
      }
      if (!this_ok) return NULL;
      if (pp_top_owner != NULL) *pp_top_owner = NULL;
      if (pp_owner_obj != NULL) *pp_owner_obj = NULL;

      return OrigObj;
   }
   for (obj_ptr=bot_obj; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
      if (obj_ptr->fattr != NULL &&
            (attr_ptr=FindAttrWithName(obj_ptr,"name=",NULL)) != NULL &&
            strcmp(attr_ptr->attr_value.s, obj_name) == 0) {
         if (pp_top_owner != NULL && bot_obj == botObj) {
            *pp_top_owner = obj_ptr;
         }
         return (obj_ptr);
      }
   }
   return NULL;
}

static int gnSeenLeftParan=FALSE;

static
int EndingRightParan(inbuf)
   char *inbuf;
{
   for ( ; *inbuf != '\0'; inbuf++) {
      if (*inbuf != ' ' && *inbuf != '\t') {
         return (*inbuf == ';');
      }
   }
   return TRUE;
}

static
int OnlyBlanksLeft(inbuf)
   char *inbuf;
{
   for ( ; *inbuf != '\0'; inbuf++) {
      if (*inbuf != ' ' && *inbuf != '\t') {
         return FALSE;
      }
   }
   return TRUE;
}

static
char *GetToken(inbuf, outbuf, obuf_sz, tok_type)
   char *inbuf, *outbuf;
   int obuf_sz, *tok_type;
   /* returns NULL of the input string is invalid */
   /* otherwise, the return value points to the character */
   /*        immediately following the end of the token */
   /* *tok_type contains the token type */
{
   register char *c_ptr, *obuf_ptr=outbuf;
   int obuf_index=0;

   *tok_type = TOK_INVALID;
   while (*inbuf == ' ' || *inbuf == '\t') inbuf++;
   switch (*inbuf) {
   case '\0':
      *tok_type = TOK_EMPTY;
      return (inbuf);
/* case '\'':
   case '"':
      *tok_type = TOK_STR;
      for (c_ptr=(&inbuf[1]); *c_ptr!=(*inbuf) && *c_ptr!='\0';
            c_ptr++, obuf_ptr++) {
         switch (*c_ptr) {
         case '\\':
            switch (*(++c_ptr)) {
            case 'n': *obuf_ptr = '\n'; break;
            case 'r': *obuf_ptr = '\r'; break;
            case 't': *obuf_ptr = '\t'; break;
            default: *obuf_ptr = *c_ptr; break;
            }
            break;
         default: *obuf_ptr = *c_ptr; break;
         }
      }
      if (*c_ptr == '\0') return (NULL);
      *obuf_ptr++ = '\0';
      return (++c_ptr);
 */
   case '(':
      if (gnSeenLeftParan) {
         break;
      } else {
         strcpy(obuf_ptr,"(");
         *tok_type = TOK_LEFT_P;
         gnSeenLeftParan = TRUE;
         return(&inbuf[1]);
      }
   case ')':
      if (gnSeenLeftParan && !EndingRightParan(&inbuf[1])) {
         break;
      } else {
         strcpy(obuf_ptr,")");
         *tok_type = TOK_RIGHT_P;
         return(&inbuf[1]);
      }
/* case '[': strcpy(obuf_ptr,"["); *tok_type=TOK_LEFT_B; return(&inbuf[1]); */
/* case ']': strcpy(obuf_ptr,"]"); *tok_type=TOK_RIGHT_B; return(&inbuf[1]); */
/* case '{': strcpy(obuf_ptr,"{"); *tok_type=TOK_LEFT_CB; return(&inbuf[1]); */
/* case '}': strcpy(obuf_ptr,"}"); *tok_type=TOK_RIGHT_CB; return(&inbuf[1]); */
   case ',': strcpy(obuf_ptr,","); *tok_type=TOK_COMMA; return(&inbuf[1]);
   case ';': strcpy(obuf_ptr,";"); *tok_type=TOK_SEMI; return(&inbuf[1]);
   }
   *tok_type = TOK_STR;
   c_ptr = inbuf;
/* while (*c_ptr != '\0' && strchr (" \t()[]{},;", *c_ptr) == NULL) {
      if (*c_ptr == '$' && c_ptr[1] == '(') {
         *obuf_ptr++ = *c_ptr++;
         *obuf_ptr++ = *c_ptr++;
         while (*c_ptr != '\0' && *c_ptr != ')') *obuf_ptr++ = *c_ptr++;
         if (*c_ptr == '\0') return NULL;
      } else {
         *obuf_ptr++ = *c_ptr++;
      }
   }
 */
   while (*c_ptr != '\0') {
      int max_obuf_index=0;

      if (*c_ptr == '$' && c_ptr[1] == '(') {
         max_obuf_index = (obuf_sz-3);
         if (obuf_index >= max_obuf_index) return NULL;
         *obuf_ptr++ = (*c_ptr++);
         *obuf_ptr++ = (*c_ptr++);
         obuf_index += 2;
         max_obuf_index++;
         while (*c_ptr != '\0' && *c_ptr != ')') {
            if (obuf_index++ >= max_obuf_index) return NULL;
            *obuf_ptr++ = *c_ptr++;
         }
         if (*c_ptr == '\0') return NULL;
      } else if (*c_ptr == '"' || *c_ptr == '\'') {
         char *tmp_c_ptr=NULL;

         max_obuf_index = (obuf_sz-2);
         if (obuf_index++ >= max_obuf_index) return NULL;
         *obuf_ptr++ = (*c_ptr);
         for (tmp_c_ptr=(&c_ptr[1]); *tmp_c_ptr!=(*c_ptr) && *tmp_c_ptr!='\0';
               tmp_c_ptr++) {
            if (obuf_index++ >= max_obuf_index) return NULL;
            switch (*tmp_c_ptr) {
            case '\\':
               switch (*(++tmp_c_ptr)) {
               case 'n': *obuf_ptr++ = '\n'; break;
               case 'r': *obuf_ptr++ = '\r'; break;
               case 't': *obuf_ptr++ = '\t'; break;
               default: *obuf_ptr++ = *tmp_c_ptr; break;
               }
               break;
            default: *obuf_ptr++ = *tmp_c_ptr; break;
            }
         }
         if (*tmp_c_ptr == '\0') return NULL;
         c_ptr = tmp_c_ptr;
         if (obuf_index++ >= max_obuf_index) return NULL;
         *obuf_ptr++ = *c_ptr++;
      } else {
         if (obuf_index++ >= obuf_sz-2) return NULL;
         *obuf_ptr++ = *c_ptr++;
      }
      if (*c_ptr == ')') {
         if (!gnSeenLeftParan || EndingRightParan(&c_ptr[1])) {
            break;
         }
      } else if (*c_ptr == '(') {
         if (!gnSeenLeftParan) {
            break;
         }
      } else if (*c_ptr == ',' || *c_ptr == ';') {
         break;
      }
   }
   *obuf_ptr = '\0';
   return (c_ptr);
}

static
char *convert_str(inbuf, obj_ptr, double_quotes_for_null)
   char *inbuf;
   struct ObjRec *obj_ptr;
   int double_quotes_for_null;
{
   register char *buf_ptr;
   char *return_str, *return_ptr, quote_char='\0';
   int cur_size=(MAXSTRING<<1), count=0, return_len=0, inside_quote=FALSE;

   return_ptr = return_str = (char *)malloc((cur_size+2)*sizeof(char));
   if (return_str == NULL) return (char*)(long)FailAllocMessage();
   *return_str = '\0';
   buf_ptr = inbuf;
   while (*inbuf != '\0') {
      struct AttrRec *attr_ptr;
      int null_string=FALSE, n, need_to_free_tmp_buf=FALSE;
      char *c_ptr=inbuf, *new_c_ptr=NULL, *cp, *cp1;
      char *tmp_buf=NULL, *attr_value=NULL;
      MiniLineInfo *pMiniLine=NULL;

      while (new_c_ptr == NULL) {
         if (inside_quote) {
            for ( ; *c_ptr != '\0'; c_ptr++) {
               if (*c_ptr == quote_char) {
                  inside_quote = FALSE;
                  c_ptr++;
                  break;
               } else if (*c_ptr == '$' && c_ptr[1] == '(') {
                  new_c_ptr = c_ptr;
                  break;
               }
            }
         } else {
            for ( ; *c_ptr != '\0'; c_ptr++) {
               if (*c_ptr == '"' || *c_ptr == '\'') {
                  quote_char = *c_ptr;
                  inside_quote = TRUE;
                  c_ptr++;
                  break;
               } else if (*c_ptr == '$' && c_ptr[1] == '(') {
                  new_c_ptr = c_ptr;
                  break;
               }
            }
         }
         if (*c_ptr == '\0') break;
      }
      if (new_c_ptr == NULL) {
         count = strlen(inbuf);
         if (count != 0) {
            if (count+return_len >= cur_size) {
               n = return_ptr-return_str;
               cur_size += count+MAXSTRING;
               return_str = (char*)realloc(return_str,
                     (cur_size+2)*sizeof(char));
               return_ptr = &return_str[n];
            }
            strncpy(return_ptr, inbuf, count);
            return_ptr += count;
            *return_ptr = '\0';
         }
         return return_str;
      }
      buf_ptr = new_c_ptr;
      count = buf_ptr-inbuf;
      return_len += count;
      if (count != 0) {
         if (count+return_len >= cur_size) {
            n = return_ptr-return_str;
            cur_size += count+MAXSTRING;
            return_str = (char*)realloc(return_str, (cur_size+2)*sizeof(char));
            return_ptr = &return_str[n];
         }
         strncpy(return_ptr, inbuf, count);
         return_ptr += count;
         *return_ptr = '\0';
      }
      if ((attr_ptr=ValidAttrArg(buf_ptr, obj_ptr, &new_c_ptr)) == NULL) {
         free(return_str);
         return NULL;
      }
      pMiniLine = attr_ptr->obj->detail.t->minilines.first;
      tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
      attr_value = UtilStrDup(tmp_buf);
      if (attr_value == NULL) FailAllocMessage();
      ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      need_to_free_tmp_buf = TRUE;

      count = strlen(attr_value);
      if (count == 0 && double_quotes_for_null && !inside_quote) {
         null_string = TRUE;
         count += 2;
      }
      if (count+return_len >= cur_size) {
         n = return_ptr-return_str;
         cur_size += count+MAXSTRING;
         return_str = (char*)realloc(return_str, (cur_size+2)*sizeof(char));
         return_ptr = &return_str[n];
      }
      if (null_string) {
         return_ptr[0] = return_ptr[1] = '"';
         return_ptr[2] = '\0';
      } else {
         strcpy(return_ptr, attr_value);
      }
      UtilFree(attr_value);

      if ((cp=strstr(return_ptr, "//")) != NULL) {
         *cp = '\0';
         count = cp-return_ptr;
      }
      if (!double_quotes_for_null && *return_ptr == '"' &&
            return_ptr[count-1] == '"') {
         int n;

         for (n=1, cp1=return_ptr, cp=(&return_ptr[1]); n < count-1; n++) {
            *cp1++ = *cp++;
         }
         *cp1 = '\0';
         count -= 2;
      }
      return_ptr += count;
      inbuf = buf_ptr = ++new_c_ptr;
   }
   return return_str;
}

int PipeReachedEOF(fp)
   FILE *fp;
{
   if (feof(fp)) return TRUE;

   /* this works because of non-blocking I/O */
   while (getc(fp) != EOF) ;
#ifdef EAGAIN
   if (errno == EINPROGRESS || errno == EAGAIN) {
#else /* ~EAGAIN */
   if (errno == EINPROGRESS) {
#endif /* EAGAIN */
      /* do nothing */
      return FALSE;
   }
   /* a real EOF */
   return TRUE;
}

int WaitForEvent(fp, fp_is_pipe, fp_is_named_pipe, pn_quit,
      what_to_do_with_x_events, pf_abort_callback, p_void)
   FILE *fp;
   int fp_is_pipe, fp_is_named_pipe, *pn_quit, what_to_do_with_x_events;
   AbortCallbackFunc *pf_abort_callback;
   void *p_void;
   /*
    * returns TRUE if there's character waiting in fp
    *
    * This routine is not really general purpose because characters can
    *       swallowed in the PipeReachedEOF() routine.
    * So if for any reason the content of the pipe is required, don't
    *       use this function!
    * fp_is_pipe is TRUE if fp is opened with popen().  This is not the
    *       same as a named pipe.
    * fp_is_named_pipe is TRUE if fp is a named pipe (S_IFIFO).
    */
{
   int rc=FALSE;

   *pn_quit = FALSE;

   if (fp != stdin && !fp_is_pipe && !fp_is_named_pipe) {
      return TRUE;
   }
   for (;;) {
      struct timeval timeout;
      fd_set fdset;
      int select_width=XConnectionNumber(mainDisplay)+1, status=0, nfds=0;

      nfds = max(select_width, (int)(fileno(fp)+1));

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

      FD_ZERO(&fdset);
      FD_SET(select_width-1, &fdset);
      FD_SET(fileno(fp), &fdset);
#ifdef __hpux
      status = select(nfds, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
      status = select(nfds, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
      if (status < 0) {
         if (errno == EINTR) {
            /* interrupted by a system call, not a problem */
            if (fp_is_pipe && !fp_is_named_pipe && PipeReachedEOF(fp)) {
               return FALSE;
            }
            CheckInterrupt(TRUE);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FUNC_SELECT_SYS_CALL_FAILED),
                  "WaitForEvent()"),
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            break;
         }
      } else if (status == 0) {
         if (fp_is_pipe && !fp_is_named_pipe && PipeReachedEOF(fp)) {
            return FALSE;
         }
         CheckInterrupt(TRUE);
      } else if (status == 1) {
         if (FD_ISSET(fileno(fp), &fdset)) {
            /* character waiting to be processed at fp */
            return TRUE;
         } else if (FD_ISSET(select_width-1, &fdset)) {
            switch (what_to_do_with_x_events) {
            case DO_ALL_X_EV:
               while (XPending(mainDisplay) > 0) {
                  if (TryProcessAnXEvent(NULL) != 0) {
                     *pn_quit = TRUE;
                     return FALSE;
                  }
               }
               break;
            case EXPOSE_AND_ESC_X_EV_ONLY:
               while (XPending(mainDisplay) > 0) {
                  CheckInterrupt(FALSE);
                  if (TryProcessAnAbortXEvent(FALSE, pf_abort_callback,
                        p_void)) {
                     if (fp_is_pipe && !fp_is_named_pipe &&
                           PipeReachedEOF(fp)) {
                        return FALSE;
                     }
                     *pn_quit = TRUE;
                     return TRUE;
                  }
               }
               break;
            }
            if (fp_is_pipe && !fp_is_named_pipe && PipeReachedEOF(fp)) {
               return FALSE;
            }
         }
      } else if (status == 2) {
         rc = TRUE;
         break;
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FUNC_INVALID_RC_FOR_SELECT),
               "WaitForEvent()", status);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         break;
      }
   }
   return rc;
}

static
int AbortLaunch(pVoid)
   void *pVoid;
{
   if (MsgBox(TgLoadString(STID_CNTRL_C_ABORT_LAUNCH), TOOL_NAME, YNC_MB) ==
         MB_ID_YES) {
      return TRUE;
   }
   return FALSE;
}

static
void LaunchIt(cmd)
   char *cmd;
{
   int len=strlen(cmd);

   while (len > 0 && (cmd[len-1] == ' ')) cmd[--len] = '\0';
   if (cmd[0] != '\0' && FindProgramInPath(cmd, NULL, FALSE)) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
      if (cmd[len-1] == '&') {
         cmd[--len] = '\0';
         while (len > 0 && (cmd[len-1] == ' ')) cmd[--len] = '\0';
         if (cmd[0] != '\0') {
#ifdef _BACKGROUND_DONT_FORK
            fprintf(stderr, TgLoadCachedString(CSTID_BACKGROUNDING_CMD), cmd);
            fprintf(stderr, "\n");
            strcat(cmd, " &");
            (void)system(cmd);
#else /* ~_BACKGROUND_DONT_FORK */
            int pid;

            fprintf(stderr, TgLoadCachedString(CSTID_BACKGROUNDING_CMD), cmd);
            fprintf(stderr, "\n");
            pid = fork();
            if (pid == 0) {
               (void)system(cmd);
               exit(0);
            }
#endif /* _BACKGROUND_DONT_FORK */
         }
      } else {
         FILE *fp=NULL;

         sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
               cmd);
         SetStringStatus(gszMsgBox);
         Msg(TgLoadCachedString(CSTID_CNTRL_C_TO_INTR_AND_ABEND));
         EndMeasureTooltip(FALSE);
         XSync(mainDisplay, False);
         if ((fp=(FILE*)popen(cmd, "r")) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), cmd);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else {
            int quit=FALSE, got_eof=FALSE, fd=fileno(fp);

            SetSocketBlockingState(&fd, FALSE);

            /*
             * Note: Calling WaitForEvent() with the second argument being
             *       TRUE can lose data in the pipe.  Can do this here
             *       because the data in the pipe is ignored.
             */
            while (WaitForEvent(fp, TRUE, FALSE, &quit,
                  EXPOSE_AND_ESC_X_EV_ONLY, AbortLaunch, NULL)) {
               if (quit) {
                  break;
               } else if (PipeReachedEOF(fp)) {
                  got_eof = TRUE;
                  break;
               }
            }
            if (quit && !got_eof) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CMD_ABORT_LAUNCH_CLOSE_TOOL),
                     cmd, TOOL_NAME);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            } else {
               pclose(fp);
            }
         }
         Msg("");
         SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
      }
      SetDefaultCursor(mainWindow);
      SetDefaultCursor(drawWindow);
   }
}

int DoLaunch(launch_attr, obj_ptr)
   struct AttrRec *launch_attr;
   struct ObjRec *obj_ptr;
{
   char *c_ptr=NULL, *buf_ptr=NULL;
   char *new_c_ptr=NULL, buf[MAXSTRING+1], *cmd=NULL, *cmd_ptr=NULL;
   int cur_size=2*MAXSTRING, count=0, cmd_len=0, n, first_time=TRUE;
   MiniLineInfo *pMiniLine=NULL;

   cmd = (char *)malloc((cur_size+1)*sizeof(char));
   if (cmd == NULL) return FailAllocMessage();
   cmd_ptr = cmd;
   buf_ptr = buf;
   for (pMiniLine=launch_attr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      struct AttrRec *attr_ptr=NULL;
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;

      if (first_time) {
         char *attr_value=NULL;

         first_time = FALSE;
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         attr_value = UtilStrDup(tmp_buf);
         if (attr_value == NULL) FailAllocMessage();
         ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         need_to_free_tmp_buf = TRUE;
         c_ptr = tmp_buf = attr_value;
      } else {
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         c_ptr = tmp_buf;
      }
      for ( ; *c_ptr != '\0'; c_ptr++, count++) {
         switch (*c_ptr) {
         case '\\': c_ptr++; *buf_ptr++ = *c_ptr; break;
         case '$':
            if (count != 0) {
               *buf_ptr = '\0';
               if (count+cmd_len >= cur_size) {
                  n = cmd_ptr-cmd;
                  cur_size += MAXSTRING;
                  cmd = (char*)realloc(cmd, (cur_size+2)*sizeof(char));
                  cmd_ptr = cmd+n;
               }
               strcpy(cmd_ptr, buf);
               cmd_ptr += count;
               count = 0;
               buf_ptr = buf;
            }
            if ((attr_ptr=ValidAttrArg(c_ptr,obj_ptr,&new_c_ptr)) == NULL) {
               free(cmd);
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_ATTR_SPEC),
                     c_ptr);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               return FALSE;
            }
            count = attr_ptr->attr_value.sz-1;
            if (count+cmd_len >= cur_size) {
               n = cmd_ptr-cmd;
               cur_size += MAXSTRING;
               cmd = (char*)realloc(cmd, (cur_size+2)*sizeof(char));
               cmd_ptr = cmd+n;
            }
            strcpy(cmd_ptr, attr_ptr->attr_value.s);
            cmd_ptr += count;
            count = -1;
            c_ptr = new_c_ptr;
            break;
         default: *buf_ptr++ = *c_ptr; break;
         }
      }
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);

      if (count != 0) {
         *buf_ptr = '\0';
         if (count+cmd_len >= cur_size) {
            n = cmd_ptr-cmd;
            cur_size += MAXSTRING;
            cmd = (char*)realloc(cmd, (cur_size+2)*sizeof(char));
            cmd_ptr = cmd+n;
         }
         strcpy(cmd_ptr, buf);
         cmd_ptr += count;
         count = 0;
         buf_ptr = buf;
      }
      if (pMiniLine->next != NULL) { *cmd_ptr++ = ' '; *cmd_ptr = '\0'; }
   }
   if (inHyperSpace && !allowLaunchInHyperSpace) {
      XFlush(mainDisplay);
      XSync(mainDisplay, False);
      sprintf(gszMsgBox, TgLoadString(STID_LAUNCH_NOT_AUTO_IN_HYPER_EXEC), cmd);
      if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
         MsgBox(TgLoadString(STID_USER_ABORT_LAUNCH_IN_HYPER), TOOL_NAME,
               INFO_MB);
         return FALSE;
      }
   }
   SaveStatusStrings();
   LaunchIt(cmd);
   RestoreStatusStrings();
   free(cmd);
   return TRUE;
}

static
int BadCmd(cmd_name)
   char *cmd_name;
{
   sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_CMD_EXEC_ABORT),
         cmd_name);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int BadSelectedObj(cmd_name)
   char *cmd_name;
{
   sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_SEL_WHILE_EXEC_CMD),
         cmd_name);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int BadAttr(attr_name, cmd_name)
   char *attr_name, *cmd_name;
{
   char msg[MAXSTRING+1];

   sprintf(msg, TgLoadString(STID_CANT_FIND_NAMED_ATTR_EXEC),
         attr_name, cmd_name);
   MsgBox(msg, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int FileAttrNotAllowed(attr_name, cmd_name)
   char *attr_name, *cmd_name;
{
   char msg[MAXSTRING+1];

   sprintf(msg, TgLoadString(STID_FILE_ATTR_NOT_ALLOWED_FOR_CMD),
         attr_name, cmd_name);
   MsgBox(msg, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int BadObjName(obj_name, cmd_name)
   char *obj_name, *cmd_name;
{
   sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_OBJ_EXEC),
         obj_name, cmd_name);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int BadArg(arg_name, cmd_name)
   char *arg_name, *cmd_name;
{
   sprintf(gszMsgBox, TgLoadString(STID_INVALID_ARG_WHILE_EXEC_CMD),
         arg_name, cmd_name);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static ReplAttrValueCallback *gpfReplaceAttrValueCallback=NULL;
static void *gpvReplAttrValueCallbackUserData=NULL;

void SetReplaceAttrValueCallback(pf_replace_attr_value, pv_userdata)
   ReplAttrValueCallback *pf_replace_attr_value;
   void *pv_userdata;
{
   gpfReplaceAttrValueCallback = pf_replace_attr_value;
   gpvReplAttrValueCallbackUserData = pv_userdata;
}

int ReplaceAttrFirstValue(obj_ptr, attr_ptr, new_value)
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   char *new_value;
   /* obj_ptr better be a top-level object */
{
   int value_len=0, same=FALSE;
   MiniLineInfo *pFirstMiniLine=attr_ptr->obj->detail.t->minilines.first;

   if (obj_ptr == NULL && historyDepth != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NEED_DISABLE_UNDO_WILL_CRASH),
            TOOL_NAME);
      MsgBox(gszMsgBox, TOOL_NAME, STOP_MB);
   }
   value_len = strlen(new_value);
   if (value_len >= 2 && (*new_value == '\'' || *new_value == '"') &&
         new_value[value_len-1] == *new_value) {
      new_value[value_len-1] = '\0';
      if (strcmp(attr_ptr->attr_value.s, &new_value[1]) == 0 &&
            (obj_ptr == tgifObj || pFirstMiniLine->first_block->next == NULL)) {
         same = TRUE;
      }
      new_value[value_len-1] = *new_value;
   } else {
      if (strcmp(attr_ptr->attr_value.s, new_value) == 0 &&
            (obj_ptr == tgifObj || pFirstMiniLine->first_block->next == NULL)) {
         same = TRUE;
      }
   }
   if (!same) {
      int ltx=0, lty=0, rbx=0, rby=0, switch_selected=FALSE;
      struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

      if (obj_ptr != NULL && obj_ptr == tgifObj && gstWBInfo.do_whiteboard) {
         /*
          * Now it's allowed.
          *
          * MsgBox(TgLoadString(STID_MODIFY_FILE_ATTR_IN_WB), TOOL_NAME,
          *       INFO_MB);
          * gnAbortExec = TRUE;
          * return FALSE;
          */
      }
      if (obj_ptr != NULL &&
            (topSel == NULL || topSel != botSel || topSel->obj != obj_ptr)) {
         switch_selected = TRUE;
         topSel = botSel = NULL;
         if (obj_ptr == tgifObj) AddObj(NULL, topObj, tgifObj);
         UpdSelBBox();
      }
      if (obj_ptr != NULL) {
         ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
         rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
      }
      PrepareToReplaceAnObj(obj_ptr);
      if (value_len >= 2 && (*new_value == '\'' || *new_value == '"') &&
            new_value[value_len-1] == *new_value) {
         new_value[value_len-1] = '\0';
         DynStrSet(&attr_ptr->attr_value, &new_value[1]);
         new_value[value_len-1] = *new_value;
      } else {
         DynStrSet(&attr_ptr->attr_value, new_value);
      }
      FreeStrBlockList(pFirstMiniLine->first_block->next);
      pFirstMiniLine->first_block->next = NULL;
      pFirstMiniLine->last_block = pFirstMiniLine->first_block;
      UpdAttr(attr_ptr);

      attr_ptr->obj->detail.t->cached_zoom = 0;
      if (attr_ptr->obj->detail.t->cached_bitmap != None) {
         XFreePixmap(mainDisplay, attr_ptr->obj->detail.t->cached_bitmap);
         attr_ptr->obj->detail.t->cached_bitmap = None;
      }
      if (obj_ptr != NULL && attr_ptr->shown) {
         AdjObjCache(obj_ptr);
         AdjObjBBox(obj_ptr);
      }
      if (gpfReplaceAttrValueCallback != NULL) {
         ReplAttrValueCallbackInfo ravci;

         memset(&ravci, 0, sizeof(ReplAttrValueCallbackInfo));
         ravci.obj_ptr = obj_ptr;
         ravci.attr_ptr = attr_ptr;
         ravci.new_attr_value = new_value;
         ravci.pv_userdata = gpvReplAttrValueCallbackUserData;
         if ((gpfReplaceAttrValueCallback)(&ravci)) {
         }
      }
      if (obj_ptr != NULL && obj_ptr == tgifObj) recordCmdIncludeTgifObj = TRUE;
      RecordReplaceAnObj(obj_ptr);
      if (obj_ptr != NULL && obj_ptr == tgifObj) recordCmdIncludeTgifObj = FALSE;
      if (obj_ptr != NULL && switch_selected) {
         RemoveAllSel();
         if (obj_ptr == tgifObj) UnlinkObj(topObj);
         topSel = saved_top_sel;
         botSel = saved_bot_sel;
         UpdSelBBox();
      }
      if (obj_ptr != NULL && replaceAttrFirstValueRedraw && obj_ptr != tgifObj &&
            attr_ptr->shown) {
         RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
               obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
               obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
               obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
               obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
      }
      SetFileModified(TRUE);
   }
   return (!same);
}

int ReplaceAttrAllValues(obj_ptr, attr_ptr, ppTopStr, ppBotStr)
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   struct StrRec **ppTopStr, **ppBotStr;
   /* obj_ptr better be a top-level object */
{
   int count=0, need_to_change=TRUE;
   struct StrRec *str_ptr=NULL;
   MiniLineInfo *pMiniLine=NULL;

   for (str_ptr=(*ppTopStr); str_ptr != NULL; str_ptr=str_ptr->next) {
      count++;
   }
   /*
    * if (count != attr_ptr->obj->detail.t->lines) {
    *    need_to_change = TRUE;
    * } else {
    *    for (str_ptr=(*ppTopStr),
    *          pMiniLine=attr_ptr->obj->detail.t->minilines.first;
    *          str_ptr != NULL;
    *          str_ptr=str_ptr->next, pMiniLine=pMiniLine->next) {
    *       if (pMiniLine == NULL) {
    *          need_to_change = TRUE;
    *          break;
    *       } else {
    *          if (pMiniLine == attr_ptr->obj->detail.t->minilines.first) {
    *             if (strcmp(attr_ptr->attr_value.s, str_ptr->dyn_str.s) != 0) {
    *                need_to_change = TRUE;
    *                break;
    *             }
    *          } else {
    *             if (strcmp(str_ptr->dyn_str.s, str_ptr->dyn_str.s) != 0) {
    *                need_to_change = TRUE;
    *                break;
    *             }
    *          }
    *       }
    *    }
    * }
    */
   if (need_to_change) {
      int ltx, lty, rbx, rby, switch_selected=FALSE;
      MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
      struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;
      struct StrRec *next_str=NULL;

      if (obj_ptr == tgifObj && gstWBInfo.do_whiteboard) {
         /*
          * Now it's allowed.
          *
          * MsgBox(TgLoadString(STID_MODIFY_FILE_ATTR_IN_WB), TOOL_NAME,
          *       INFO_MB);
          * gnAbortExec = TRUE;
          * return FALSE;
          */
      }
      if (topSel == NULL || topSel != botSel || topSel->obj != obj_ptr) {
         switch_selected = TRUE;
         topSel = botSel = NULL;
         if (obj_ptr == tgifObj) AddObj(NULL, topObj, tgifObj);
         UpdSelBBox();
      }
      ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
      rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
      PrepareToReplaceAnObj(obj_ptr);

      FreeMiniLines(&attr_ptr->obj->detail.t->minilines, FALSE);

      for (str_ptr=(*ppTopStr); str_ptr != NULL; str_ptr=next_str) {
         next_str = str_ptr->next;
         pMiniLine = CreateMiniLineFromString(str_ptr->dyn_str.s,
               &pFirstMiniLine, &pLastMiniLine);
         FreeStr(str_ptr);
      }
      attr_ptr->obj->detail.t->minilines.first = pFirstMiniLine;
      attr_ptr->obj->detail.t->minilines.last = pLastMiniLine;
      attr_ptr->obj->detail.t->lines = count;
      (*ppTopStr) = (*ppBotStr) = NULL;

      UpdateAttr(attr_ptr->obj->detail.t, attr_ptr);
      AdjObjCache(obj_ptr);
      AdjObjBBox(obj_ptr);

      if (obj_ptr == tgifObj) recordCmdIncludeTgifObj = TRUE;
      RecordReplaceAnObj(obj_ptr);
      if (obj_ptr == tgifObj) recordCmdIncludeTgifObj = FALSE;
      if (switch_selected) {
         RemoveAllSel();
         if (obj_ptr == tgifObj) UnlinkObj(topObj);
         topSel = saved_top_sel;
         botSel = saved_bot_sel;
         UpdSelBBox();
      }
      if (obj_ptr != tgifObj && attr_ptr->shown) {
         RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
               obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
               obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
               obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
               obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
      }
      SetFileModified(TRUE);
   }
   return need_to_change;
}

void JustReadFileIntoAttr(FP, attr_ptr, attr_owner_obj)
   FILE *FP;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj;
{
   int ltx=attr_owner_obj->bbox.ltx, lty=attr_owner_obj->bbox.ltx;
   int rbx=attr_owner_obj->bbox.rbx, rby=attr_owner_obj->bbox.rby;
   int saved_color_index=colorIndex;
   int x=attr_ptr->obj->x, baseline_y=attr_ptr->obj->detail.t->baseline_y;
   struct ObjRec *text_obj_ptr=NULL;
   struct TextRec *text_ptr=attr_ptr->obj->detail.t;

   colorIndex = text_ptr->minilines.first->first_block->seg->color;
   PushCurFont();
   ObjFontInfoToCurFontInfo(text_ptr);
   StrSegInfoToCurFontInfo(GetTextPtrFirstStrSeg(text_ptr));

   penPat = attr_ptr->obj->detail.t->pen;
   objFill = attr_ptr->obj->detail.t->fill;
   transPat = attr_ptr->obj->trans_pat;

   SetCanvasFont();

   FreeTextObj(attr_ptr->obj);
   attr_ptr->obj = NULL;
   text_obj_ptr = FormTextObjectFromFile(FP, x, baseline_y);

   PopCurFont();

   colorIndex = saved_color_index;

   if (text_obj_ptr == NULL) {
      DynStrSet(&attr_ptr->attr_value, "");
   } else {
      MiniLineInfo *pMiniLine=text_obj_ptr->detail.t->minilines.first;

      if (pMiniLine != NULL) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=ConvertMiniLineToString(pMiniLine,
               &need_to_free_tmp_buf);

         DynStrSet(&attr_ptr->attr_value, tmp_buf);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      } else {
         DynStrSet(&attr_ptr->attr_value, "");
      }
   }
   attr_ptr->obj = text_obj_ptr;
   text_obj_ptr->detail.t->attr = attr_ptr;

   UpdAttr(attr_ptr);
   if (attr_ptr->shown) {
      AdjObjCache(attr_owner_obj);
      AdjObjBBox(attr_owner_obj);
   }
   RecordReplaceAnObj(attr_owner_obj);
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.ltx-GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.lty-GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.rbx+GRID_ABS_SIZE(1),
         attr_owner_obj->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
}

static
int ReturnInterruptedExec()
{
   if (!execInterruptEnabled) {
      if (execInterruptQueued) {
         sprintf(gszMsgBox, TgLoadString(STID_USER_INTR_TOOL_WAIT_CLEANUP),
               TOOL_NAME);
         return (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) == MB_ID_YES);
      }
      execInterruptQueued = TRUE;
      return FALSE;
   }
   return TRUE;
}

static int execTightLoopWait=FALSE;
static int execTightLoopOneLineStatus=FALSE;
static char execTightLoopStatusBuf[MAX_STATUS_BTNS+1][MAXSTRING+1];

static
void ExitExecTightLoop()
{
   RestoreStatusStringsFromBuf(execTightLoopStatusBuf,
        execTightLoopOneLineStatus);
   execTightLoopWait = FALSE;
}

static
void EnterExecTightLoop()
{
   SaveStatusStringsIntoBuf(execTightLoopStatusBuf,
         &execTightLoopOneLineStatus);
   execTightLoopWait = TRUE;
}

static
int CheckExecInterrupt(orig_cmd)
   char *orig_cmd;
{
   int check_any_button=execInterruptEnabled;
   XEvent ev;

   if (mainDisplay == NULL) return FALSE;

   while (XCheckMaskEvent(mainDisplay, StructureNotifyMask, &ev)) {
      if (iconWindowShown) {
         if ((ev.xany.window == iconBaseWindow && ev.type == UnmapNotify) ||
               (ev.xany.window == mainWindow && ev.type == MapNotify)) {
            XPutBackEvent(mainDisplay, &ev);
            return ReturnInterruptedExec();
         }
      } else if ((ev.xany.window == iconBaseWindow && ev.type == MapNotify) ||
            (ev.xany.window == mainWindow && ev.type == UnmapNotify)) {
         XPutBackEvent(mainDisplay, &ev);
         return ReturnInterruptedExec();
      } else if (ev.type == ConfigureNotify) {
         Reconfigure(FALSE);
      }
   }
   while (XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      if (iconWindowShown) {
         if (ev.xany.window == mainWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return ReturnInterruptedExec();
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      } else {
         if (ev.xany.window == iconBaseWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return ReturnInterruptedExec();
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      }
   }
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
      while (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) ;
   }
   if (ESCPressed() || (check_any_button && XCheckMaskEvent(mainDisplay,
         ButtonPressMask | KeyPressMask, &ev)) || CheckInterrupt(TRUE)) {
      if (orig_cmd == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_USER_INTR));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FUNC_USER_INTR), orig_cmd);
      }
      Msg(gszMsgBox);
      return ReturnInterruptedExec();
   }
   if (execTightLoopWait) {
      while (XCheckWindowEvent(mainDisplay, dummyWindow1,
            EnterWindowMask|LeaveWindowMask, &ev)) {
         switch (ev.type) {
         case EnterNotify:
            SetMouseStatus(TgLoadCachedString(CSTID_INTERRUPT),
                  TgLoadCachedString(CSTID_INTERRUPT),
                  TgLoadCachedString(CSTID_INTERRUPT));
            break;
         case LeaveNotify:
            RestoreStatusStringsFromBuf(execTightLoopStatusBuf,
                 execTightLoopOneLineStatus);
            break;
         }
      }
   }
   if (!check_any_button) {
      while (XCheckMaskEvent(mainDisplay, ButtonPressMask|KeyPressMask, &ev)) ;
   }
   return FALSE;
}

static
int IntExpression(expr, p_ival, orig_cmd)
   char *expr, *orig_cmd;
   int *p_ival;
{
   struct VRec v;

   if (EvalExpr(expr, &v)) {
      switch (v.vtype) {
      case NULL_VAL: break;
      case INT_VAL: if (p_ival != NULL) *p_ival = v.val.i; return TRUE;
      case DBL_VAL: break;
      case STR_VAL: break;
      default: break;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_INT_EXP_EXEC_CMD), expr,
         orig_cmd);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int FloatExpression(expr, p_dval, orig_cmd)
   char *expr, *orig_cmd;
   double *p_dval;
{
   struct VRec v;

   if (EvalExpr(expr, &v)) {
      switch (v.vtype) {
      case NULL_VAL: break;
      case INT_VAL: if (p_dval != NULL) *p_dval = (double)v.val.i; return TRUE;
      case DBL_VAL: if (p_dval != NULL) *p_dval = v.val.d; return TRUE;
      case STR_VAL: break;
      default: break;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_FLOAT_EXP_EXEC_CMD), expr,
         orig_cmd);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
void SelectAnObj(sub_obj, owner_obj, top_owner, pp_saved_top_sel,
      pp_saved_bot_sel)
   struct ObjRec *sub_obj, *owner_obj, *top_owner;
   struct SelRec **pp_saved_top_sel, **pp_saved_bot_sel;
{
   if (pp_saved_top_sel != NULL && pp_saved_bot_sel != NULL) {
      *pp_saved_top_sel = topSel;
      *pp_saved_bot_sel = botSel;
   } else {
      if (topSel != NULL) RemoveAllSel();
   }
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) FailAllocMessage();

   topSel->next = topSel->prev = NULL;
   topSel->obj = (owner_obj==NULL ? sub_obj : top_owner);
   UpdSelBBox();
}

/* --------------------- Exec Routines --------------------- */

int ExecLaunch(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* launch(attribute_to_launch); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   return (DoLaunch(attr_ptr, obj_ptr));
}

int ExecExec(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* exec(attribute_to_exec); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   return (DoExec(attr_ptr, attr_owner_obj));
}

int ExecMktemp(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* mktemp(str,result_attribute); */
{
   char *file_name=argv[0], *attr_name=argv[1];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   int rc=TRUE;

   UtilRemoveQuotes(file_name);
   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(file_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

#ifndef _DONT_USE_MKTEMP
   if (mktemp(file_name) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_FUNC_CMD_EXEC_ABORT),
            "mktemp()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      rc = FALSE;
   } else {
      char *psz=UtilStrRChr(file_name, DIR_SEP);
      int changed=FALSE;

      if (psz != NULL) {
         for (psz++; *psz != '\0'; psz++) {
            if ((*psz >= '0' && *psz <= '9') || (*psz >= 'a' && *psz <= 'z') ||
                  (*psz >= 'A' && *psz <= 'Z')) {
            } else {
               *psz = 'Q';
               changed = TRUE;
            }
         }
      }
      if (changed && mktemp(file_name) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_FUNC_CMD_EXEC_ABORT),
               "mktemp()");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         rc = FALSE;
      } else {
         unlink(file_name);
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, file_name);
      }
   }
#else /* _DONT_USE_MKTEMP */
   {
      int fd=(-1);
      char *dup_fname=UtilStrDup(file_name), path[MAXPATHLENGTH];

      if (dup_fname == NULL) FailAllocMessage();
      if ((fd=mkstemp(dup_fname)) == (-1)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OBTAIN_TMP_FILE_NAME),
               file_name);
         if (PRTGIF) {
            fprintf(stderr, "%s\n", gszMsgBox);
         } else {
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
         rc = FALSE;
      } else {
         close(fd);
         UtilStrCpyN(path, sizeof(path), dup_fname);
      }
      UtilFree(dup_fname);
      /*
       * int len=0;
       * char *psz=NULL, fname[MAXPATHLENGTH], path[MAXPATHLENGTH];
       *
       * UtilStrCpyN(fname, sizeof(fname), file_name);
       * len = strlen(fname);
       * while (len > 0 && fname[len-1] == 'X') {
       *    fname[--len] = '\0';
       * }
       * psz = UtilStrRChr(fname, DIR_SEP);
       * if (psz == NULL) {
       *    UtilStrCpyN(path, sizeof(path), (char*)tempnam(NULL, fname));
       * } else {
       *    *psz = '\0';
       *    UtilStrCpyN(path, sizeof(path), (char*)tempnam(fname, &psz[1]));
       *    *psz = DIR_SEP;
       * }
       */
      UtilTrimBlanks(path);
      unlink(path);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, path);
   }
#endif /* ~_DONT_USE_MKTEMP */
   return rc;
}

int ExecUseTemplate(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_file_using_simple_template(template_file, */
   /*      output_file,string_to_match,substitution_attribute); */
{
   char *template_name=argv[0], *file_name=argv[1];
   char *match_str=argv[2], *attr_name=argv[3], msg[MAXSTRING+1];
   struct AttrRec *attr_ptr=NULL;
   struct TextRec *text_ptr=NULL;
   FILE *in_fp=NULL, *out_fp=NULL;

   UtilRemoveQuotes(template_name);
   UtilRemoveQuotes(file_name);
   UtilRemoveQuotes(match_str);
   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if ((out_fp=fopen(file_name, "w")) != NULL &&
         (in_fp=fopen(template_name, "r")) != NULL) {
      int len_to_match=strlen(match_str), len, rc=TRUE;
      char tmp_buf[MAXSTRING+1];

      writeFileFailed = FALSE;
      while (fgets(tmp_buf, MAXSTRING, in_fp) != NULL) {
         len = strlen(tmp_buf);
         if (tmp_buf[len-1] == '\n') {
            tmp_buf[--len] = '\0';
            if (len==len_to_match && strcmp(tmp_buf, match_str)==0) {
               if (*attr_ptr->attr_value.s != '\0') {
                  if (fprintf(out_fp, "%s\n", attr_ptr->attr_value.s) == EOF) {
                     writeFileFailed = TRUE;
                  }
               }
               text_ptr = attr_ptr->obj->detail.t;

               if (text_ptr->minilines.first != NULL) {
                  MiniLineInfo *pMiniLine=NULL;

                  for (pMiniLine=text_ptr->minilines.first->next;
                        pMiniLine != NULL; pMiniLine = pMiniLine->next) {
                     int need_to_free_tmp_buf=FALSE;
                     char *tmp_buf=ConvertMiniLineToString(pMiniLine,
                           &need_to_free_tmp_buf);

                     if (fprintf(out_fp, "%s\n", tmp_buf) == EOF) {
                        writeFileFailed = TRUE;
                     }
                     if (need_to_free_tmp_buf) UtilFree(tmp_buf);
                     if (writeFileFailed) break;
                  }
               }
            } else if (fprintf(out_fp, "%s\n", tmp_buf) == EOF) {
               writeFileFailed = TRUE;
            }
         } else if (len==len_to_match && strcmp(tmp_buf, match_str)==0) {
            if (*attr_ptr->attr_value.s != '\0') {
               if (fprintf(out_fp, "%s\n", attr_ptr->attr_value.s) == EOF) {
                  writeFileFailed = TRUE;
               }
            }
            text_ptr = attr_ptr->obj->detail.t;

            if (text_ptr->minilines.first != NULL) {
               MiniLineInfo *pMiniLine=NULL;

               for (pMiniLine=text_ptr->minilines.first->next;
                     pMiniLine != NULL; pMiniLine = pMiniLine->next) {
                  int need_to_free_tmp_buf=FALSE;
                  char *tmp_buf=ConvertMiniLineToString(pMiniLine,
                        &need_to_free_tmp_buf);

                  if (fprintf(out_fp, "%s\n", tmp_buf) == EOF) {
                     writeFileFailed = TRUE;
                  }
                  if (need_to_free_tmp_buf) UtilFree(tmp_buf);
                  if (writeFileFailed) break;
               }
            }
         } else if (fprintf(out_fp, "%s\n", tmp_buf) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (writeFileFailed) {
         FailToWriteFileMessage(file_name);
         rc = FALSE;
      }
      fclose(in_fp);
      fclose(out_fp);
      return rc;
   } else if (out_fp == NULL) {
      sprintf(msg, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING), file_name);
      MsgBox(msg, TOOL_NAME, INFO_MB);
   } else if (in_fp == NULL) {
      sprintf(msg, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            template_name);
      MsgBox(msg, TOOL_NAME, INFO_MB);
   }
   if (out_fp != NULL) fclose(out_fp);
   if (in_fp != NULL) fclose(in_fp);
   return FALSE;
}

#define UPDATE_EPS_CHILD 0
#define UPDATE_XBM_CHILD 1
#define UPDATE_XPM_CHILD 2

#define UPDATE_CHILD_NO_ALIGNMENT 0
#define UPDATE_CHILD_NORMAL_ALIGNMENT 1
#define UPDATE_CHILD_NO_OVERLAP_ALIGNMENT 2

static int updateChildUsingAlignment=INVALID;

static
int ExecUpdChild(argv, obj_ptr, orig_cmd, upd_type)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int upd_type;
   /* update_eps_child(eps_file_name); */
   /* update_xbm_child(xbm_file_name); */
   /* update_xpm_child(xpm_file_name); */
{
   struct XPmRec *xpm_ptr=NULL;
   char *file_name=argv[0], png_fname[MAXPATHLENGTH], ppm6_fname[MAXPATHLENGTH];
   char jpg_fname[MAXPATHLENGTH];
   char msg[MAXPATHLENGTH+1], deflated_fname[MAXPATHLENGTH];
   char **lines=NULL, write_date[32];
   struct ObjRec *sub_obj_ptr=NULL, *del_obj=NULL, *tmp_obj;
   int rc=BitmapOpenFailed, len=0, png_not_xpm=FALSE, jpg_not_xpm=FALSE;
   int num_lines=0, epsf_level=0, image_w=0, image_h=0;
   int del_obj_ltx=0, del_obj_lty=0, ltx=0, lty=0;
   int rbx=0, rby=0, x_hot, y_hot, w=0, h=0, ncolors=0;
   int chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   struct BBRec parent_obbox;
   unsigned int ui_image_w=0, ui_image_h=0;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   float llx=0, lly=0, urx=0, ury=0;
   Pixmap bitmap=None, pixmap=None;
   XImage *image=NULL, *bitmap_image=NULL;

   *png_fname = *jpg_fname = *ppm6_fname = '\0';
   if (obj_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_SPEC_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   UtilRemoveQuotes(file_name);
   if (updateChildUsingAlignment == INVALID) {
      char *c_ptr;

      updateChildUsingAlignment = UPDATE_CHILD_NO_ALIGNMENT;
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UpdateChildUsingAlignment")) != NULL) {
         if (UtilStrICmp(c_ptr, "true") == 0) {
            updateChildUsingAlignment = UPDATE_CHILD_NORMAL_ALIGNMENT;
         } else if (UtilStrICmp(c_ptr, "no_overlap") == 0) {
            updateChildUsingAlignment = UPDATE_CHILD_NO_OVERLAP_ALIGNMENT;
         }
      }
   }
   switch (obj_ptr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN: break;

   default:
      sprintf(msg, TgLoadString(STID_FUNC_ONLY_WORK_COMPOSITE_OBJ), orig_cmd);
      MsgBox(msg, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   parent_obbox.ltx = obj_ptr->obbox.rbx;
   parent_obbox.lty = obj_ptr->obbox.rby;
   parent_obbox.rbx = obj_ptr->obbox.ltx;
   parent_obbox.rby = obj_ptr->obbox.lty;
   switch (upd_type) {
   case UPDATE_EPS_CHILD:
      for (tmp_obj=obj_ptr->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XBM &&
               tmp_obj->detail.xbm->real_type!=XBM_XBM) {
            del_obj_ltx = tmp_obj->obbox.ltx;
            del_obj_lty = tmp_obj->obbox.lty;
            del_obj = tmp_obj;
            if (!updateChildUsingAlignment) break;
         } else if (updateChildUsingAlignment) {
            if (tmp_obj->obbox.ltx < parent_obbox.ltx)
               parent_obbox.ltx = tmp_obj->obbox.ltx;
            if (tmp_obj->obbox.lty < parent_obbox.lty)
               parent_obbox.lty = tmp_obj->obbox.lty;
            if (tmp_obj->obbox.rbx > parent_obbox.rbx)
               parent_obbox.rbx = tmp_obj->obbox.rbx;
            if (tmp_obj->obbox.rby > parent_obbox.rby)
               parent_obbox.rby = tmp_obj->obbox.rby;
         }
      }
      break;
   case UPDATE_XBM_CHILD:
      for (tmp_obj=obj_ptr->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XBM &&
               tmp_obj->detail.xbm->real_type==XBM_XBM) {
            del_obj_ltx = tmp_obj->obbox.ltx;
            del_obj_lty = tmp_obj->obbox.lty;
            del_obj = tmp_obj;
            if (!updateChildUsingAlignment) break;
         } else if (updateChildUsingAlignment) {
            if (tmp_obj->obbox.ltx < parent_obbox.ltx)
               parent_obbox.ltx = tmp_obj->obbox.ltx;
            if (tmp_obj->obbox.lty < parent_obbox.lty)
               parent_obbox.lty = tmp_obj->obbox.lty;
            if (tmp_obj->obbox.rbx > parent_obbox.rbx)
               parent_obbox.rbx = tmp_obj->obbox.rbx;
            if (tmp_obj->obbox.rby > parent_obbox.rby)
               parent_obbox.rby = tmp_obj->obbox.rby;
         }
      }
      break;
   case UPDATE_XPM_CHILD:
      for (tmp_obj=obj_ptr->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XPM) {
            del_obj_ltx = tmp_obj->obbox.ltx;
            del_obj_lty = tmp_obj->obbox.lty;
            del_obj = tmp_obj;
            if (!updateChildUsingAlignment) break;
         } else if (updateChildUsingAlignment) {
            if (tmp_obj->obbox.ltx < parent_obbox.ltx)
               parent_obbox.ltx = tmp_obj->obbox.ltx;
            if (tmp_obj->obbox.lty < parent_obbox.lty)
               parent_obbox.lty = tmp_obj->obbox.lty;
            if (tmp_obj->obbox.rbx > parent_obbox.rbx)
               parent_obbox.rbx = tmp_obj->obbox.rbx;
            if (tmp_obj->obbox.rby > parent_obbox.rby)
               parent_obbox.rby = tmp_obj->obbox.rby;
         }
      }
      len = strlen(file_name);
      if (len > 4 && UtilStrNCaseCmp(&file_name[len-4], ".png", 4) == 0) {
         png_not_xpm = TRUE;
         UtilStrCpyN(png_fname, sizeof(png_fname), file_name);
         strcpy(&file_name[len-4], ".xpm");
      } else if (len > 4 &&
            UtilStrNCaseCmp(&file_name[len-4], ".jpg", 4) == 0) {
         jpg_not_xpm = TRUE;
         UtilStrCpyN(jpg_fname, sizeof(jpg_fname), file_name);
         strcpy(&file_name[len-4], ".xpm");
      }
      break;
   default: return BadCmd(orig_cmd);
   }
   if (del_obj == NULL) {
      del_obj_ltx = obj_ptr->obbox.ltx;
      /*
       * Use lty to cover the invisible object.
       * Use to use rby to tile below the invisible object.
       */
      del_obj_lty = obj_ptr->obbox.lty;
   }
   importingFile = TRUE;
   switch (upd_type) {
   case UPDATE_EPS_CHILD:
      rc = MyReadEPSFile(file_name, &image_w, &image_h, &bitmap,
            &image, &num_lines, &lines, &epsf_level, &llx, &lly, &urx, &ury,
            write_date);
      break;
   case UPDATE_XBM_CHILD:
      rc = XReadBitmapFile(mainDisplay, mainWindow, file_name,
            &ui_image_w, &ui_image_h, &bitmap, &x_hot, &y_hot);
      image_w = ui_image_w; image_h = ui_image_h;
      break;
   case UPDATE_XPM_CHILD:
      if (png_not_xpm || jpg_not_xpm) {
         if (fullTrueColorMode && HasZlibSupport()) {
            if (png_not_xpm) {
               rc = ConvertPngToPpm6(png_fname, ppm6_fname, sizeof(ppm6_fname));
            } else if (jpg_not_xpm) {
               rc = ConvertJpegToPpm6(jpg_fname, ppm6_fname,
                     sizeof(ppm6_fname));
            } else {
               /* should not come here */
               rc = BitmapFileInvalid;
            }
            if (rc) {
               ResetPngHeaderInfo(&gPngHeaderInfo);
               sub_obj_ptr = CreatePpmTrueObjFromFile(ppm6_fname);
            }
            if (sub_obj_ptr != NULL &&
                  MkTempFile(deflated_fname, sizeof(deflated_fname), tmpDir,
                  TOOL_NAME) != NULL && DeflateFile(ppm6_fname,
                  deflated_fname)) {
               /* nothing to do here */
            } else {
               unlink(ppm6_fname);
               if (sub_obj_ptr != NULL) FreeObj(sub_obj_ptr);
               importingFile = FALSE;
               return FALSE;
            }
            unlink(ppm6_fname);
            xpm_ptr = sub_obj_ptr->detail.xpm;
            xpm_ptr->real_type = PPM_TRUE;
            xpm_ptr->ppm_data_compress = PPM_DATA_DEFLATED;
            xpm_ptr->ppm_data = ReadFileIntoBuf(deflated_fname,
                  &xpm_ptr->ppm_data_size);
            xpm_ptr->ppm_mask_data = NULL;
            xpm_ptr->ppm_mask_size = 0;
            unlink(deflated_fname);
            rc = BitmapSuccess;
         } else {
            if (png_not_xpm) {
               rc = ConvertPngToXpm(png_fname, file_name, strlen(file_name)+1);
            } else if (jpg_not_xpm) {
               rc = ConvertJpegToXpm(jpg_fname, file_name, strlen(file_name)+1);
            } else {
               /* should not come here */
               rc = BitmapFileInvalid;
            }
            if (rc) {
               rc = MyReadPixmapFile(file_name, &image_w, &image_h, &w, &h,
                     &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
                     &chars_per_pixel, &first_pixel_is_bg, &color_char,
                     &color_str, &pixels, &xpm_data);
            } else {
               rc = BitmapFileInvalid;
            }
         }
      } else {
         rc = MyReadPixmapFile(file_name, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
      }
      break;
   }
   if (rc != BitmapSuccess) {
      importingFile = FALSE;
      sprintf(msg, TgLoadString(STID_FUNC_FAIL_TO_IMPORT_GIVEN), orig_cmd,
            file_name);
      MsgBox(msg, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
   rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;

   PrepareToReplaceAnObj(obj_ptr);

   switch (upd_type) {
   case UPDATE_EPS_CHILD:
      saveEPSLines = TRUE;
      sub_obj_ptr = CreateEPSObj(file_name, image_w, image_h, bitmap,
            image, num_lines, lines, epsf_level, &llx, &lly, &urx, &ury,
            write_date);
      saveEPSLines = FALSE;
      if (strcmp(defaultEPSScalingStr,"1") != 0) {
         ScaleAnEPSObj(sub_obj_ptr, &defaultEPSScaling);
      }
      break;
   case UPDATE_XBM_CHILD:
      sub_obj_ptr = CreateXBmObj(image_w, image_h, image_w, image_h,
            bitmap, image);
      break;
   case UPDATE_XPM_CHILD:
      if (sub_obj_ptr == NULL) {
         sub_obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image,
               bitmap, bitmap_image, ncolors, chars_per_pixel,
               first_pixel_is_bg, color_char, color_str, pixels, xpm_data);
      } else {
         /* already created */
      }
      break;
   }
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   if (updateChildUsingAlignment != UPDATE_CHILD_NO_ALIGNMENT) {
      switch (horiAlign) {
      case ALIGN_N:
      case ALIGN_L:
         del_obj_ltx = parent_obbox.ltx;
         break;
      case ALIGN_C:
      case ALIGN_S:
         del_obj_ltx = ((parent_obbox.ltx+parent_obbox.rbx)>>1) -
               ((sub_obj_ptr->obbox.rbx-sub_obj_ptr->obbox.ltx)>>1);
         break;
      case ALIGN_R:
         del_obj_ltx = parent_obbox.rbx -
               (sub_obj_ptr->obbox.rbx-sub_obj_ptr->obbox.ltx);
         break;
      }
      switch (updateChildUsingAlignment) {
      case UPDATE_CHILD_NORMAL_ALIGNMENT:
         switch (vertAlign) {
         case ALIGN_T:
            del_obj_lty = parent_obbox.lty;
            break;
         case ALIGN_M:
         case ALIGN_S:
            del_obj_lty = ((parent_obbox.lty+parent_obbox.rby)>>1) -
                  ((sub_obj_ptr->obbox.rby-sub_obj_ptr->obbox.lty)>>1);
            break;
         case ALIGN_B:
            del_obj_lty = parent_obbox.rby -
                  (sub_obj_ptr->obbox.rby-sub_obj_ptr->obbox.lty);
            break;
         case ALIGN_N:
            del_obj_lty = parent_obbox.rby;
            break;
         }
         break;
      case UPDATE_CHILD_NO_OVERLAP_ALIGNMENT:
         switch (vertAlign) {
         case ALIGN_T:
            del_obj_lty = parent_obbox.lty -
                  (sub_obj_ptr->obbox.rby-sub_obj_ptr->obbox.lty);
            break;
         case ALIGN_M:
         case ALIGN_S:
            del_obj_lty = ((parent_obbox.lty+parent_obbox.rby)>>1) -
                  ((sub_obj_ptr->obbox.rby-sub_obj_ptr->obbox.lty)>>1);
            break;
         case ALIGN_B:
         case ALIGN_N:
            del_obj_lty = parent_obbox.rby;
            break;
         }
         break;
      }
   }
   MoveObj(sub_obj_ptr, del_obj_ltx-sub_obj_ptr->obbox.ltx,
         del_obj_lty-sub_obj_ptr->obbox.lty);
   if (del_obj != NULL) {
      if (del_obj == obj_ptr->detail.r->first) {
         obj_ptr->detail.r->first = del_obj->next;
      } else {
         del_obj->prev->next = del_obj->next;
      }
      if (del_obj == obj_ptr->detail.r->last) {
         obj_ptr->detail.r->last = del_obj->prev;
      } else {
         del_obj->next->prev = del_obj->prev;
      }
      FreeObj(del_obj);
   }
   sub_obj_ptr->prev = NULL;
   sub_obj_ptr->next = obj_ptr->detail.r->first;
   if (obj_ptr->detail.r->first == NULL) {
      obj_ptr->detail.r->last = sub_obj_ptr;
   } else {
      obj_ptr->detail.r->first->prev = sub_obj_ptr;
   }
   obj_ptr->detail.r->first = sub_obj_ptr;
   AdjObjBBox(obj_ptr);

   RecordReplaceAnObj(obj_ptr);
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         obj_ptr->bbox.ltx-GRID_ABS_SIZE(1), obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);

   importingFile = FALSE;

   return TRUE;
}

int ExecUpdEPSChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecUpdChild(argv, obj_ptr, orig_cmd, UPDATE_EPS_CHILD);
}

int ExecUpdXBMChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecUpdChild(argv, obj_ptr, orig_cmd, UPDATE_XBM_CHILD);
}

int ExecUpdXPMChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecUpdChild(argv, obj_ptr, orig_cmd, UPDATE_XPM_CHILD);
}

static
int ExecDelChild(argv, obj_ptr, orig_cmd, upd_type)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int upd_type;
   /* delete_eps_child(obj_name); */
   /* delete_xbm_child(obj_name); */
   /* delete_xpm_child(obj_name); */
{
   char *obj_name=argv[0];
   struct ObjRec *top_owner=NULL, *named_obj=NULL;

   struct ObjRec *del_obj=NULL, *tmp_obj=NULL;
   int ltx=0, lty=0, rbx=0, rby=0;

   UtilRemoveQuotes(obj_name);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   switch (named_obj->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN: break;

   default:
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ONLY_WORK_COMPOSITE_OBJ),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ltx = top_owner->bbox.ltx; lty = top_owner->bbox.lty;
   rbx = top_owner->bbox.rbx; rby = top_owner->bbox.rby;

   switch (upd_type) {
   case UPDATE_EPS_CHILD:
      for (tmp_obj=named_obj->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XBM &&
               tmp_obj->detail.xbm->real_type!=XBM_XBM) {
            del_obj = tmp_obj;
            break;
         }
      }
      break;
   case UPDATE_XBM_CHILD:
      for (tmp_obj=named_obj->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XBM &&
               tmp_obj->detail.xbm->real_type==XBM_XBM) {
            del_obj = tmp_obj;
            break;
         }
      }
      break;
   case UPDATE_XPM_CHILD:
      for (tmp_obj=named_obj->detail.r->last; tmp_obj!=NULL;
            tmp_obj=tmp_obj->prev) {
         if (tmp_obj->type==OBJ_XPM) {
            del_obj = tmp_obj;
            break;
         }
      }
      break;
   default: return BadCmd(orig_cmd);
   }
   if (del_obj == NULL) {
      /* no child to delete, no problem */
      return TRUE;
   }
   PrepareToReplaceAnObj(top_owner);

   if (del_obj != NULL) {
      if (del_obj == named_obj->detail.r->first) {
         named_obj->detail.r->first = del_obj->next;
      } else {
         del_obj->prev->next = del_obj->next;
      }
      if (del_obj == named_obj->detail.r->last) {
         named_obj->detail.r->last = del_obj->prev;
      } else {
         del_obj->next->prev = del_obj->prev;
      }
      FreeObj(del_obj);
   }
   AdjObjBBox(top_owner);

   RecordReplaceAnObj(top_owner);
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         top_owner->bbox.ltx-GRID_ABS_SIZE(1),
         top_owner->bbox.lty-GRID_ABS_SIZE(1),
         top_owner->bbox.rbx+GRID_ABS_SIZE(1),
         top_owner->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);

   return TRUE;
}

int ExecDelEPSChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecDelChild(argv, obj_ptr, orig_cmd, UPDATE_EPS_CHILD);
}

int ExecDelXBMChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecDelChild(argv, obj_ptr, orig_cmd, UPDATE_XBM_CHILD);
}

int ExecDelXPMChild(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
{
   return ExecDelChild(argv, obj_ptr, orig_cmd, UPDATE_XPM_CHILD);
}

#ifndef _NO_GETTIMEOFDAY
static
long ms_time(tv)
   struct timeval *tv;
{
   return ((long)(tv->tv_usec / 1000.0)) + ((long)(tv->tv_sec * 1000));
}
#endif /* ~_NO_GETTIMEOFDAY */

#define FD_STYLE_LINEAR 0
#define FD_STYLE_PINGPONG 1

#define FD_FORWARD 0
#define FD_REVERSE 1

int ExecFlipDeck(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* flip_deck(times,frames_per_second,style); */
   /*       style=[linear | pingpong] */
{
   char *times_str=argv[0];
   char *fps_str=argv[1], *style_str=argv[2];
   int num_bm, iteration, times, fps, style;
   int rc=TRUE;
   struct ObjRec *bm_obj=NULL, *cur_obj=NULL, *prev_obj=NULL, *next_obj=NULL;
   struct BBRec obbox;
   int select_width=XConnectionNumber(mainDisplay)+1;
#ifdef _NO_GETTIMEOFDAY
   struct timeb start;
#else /* ~_NO_GETTIMEOFDAY */
   struct timeval start;
   struct timezone zone;
#endif /* _NO_GETTIMEOFDAY */
   long ms_start_time, ms_frame_interval;
   fd_set fdset;

   memset(&obbox, 0, sizeof(struct BBRec));
   if (obj_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_SPEC_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   UtilRemoveQuotes(times_str);
   UtilRemoveQuotes(fps_str);
   UtilRemoveQuotes(style_str);
   switch (obj_ptr->type) {
   case OBJ_GROUP:
   case OBJ_ICON:
   case OBJ_SYM:
   case OBJ_PIN: break;

   default:
      sprintf(execDummyStr, TgLoadString(STID_FUNC_ONLY_WORK_COMPOSITE_OBJ),
            orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   num_bm = 0;
   for (bm_obj=obj_ptr->detail.r->last; bm_obj != NULL; bm_obj=bm_obj->prev) {
      if (bm_obj->type != OBJ_XBM && bm_obj->type != OBJ_XPM) {
         sprintf(gszMsgBox, TgLoadString(STID_FUNC_OBJ_NON_XBM_XPM_SUB_OBJS),
               orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return (FALSE);
      } else if (num_bm++ == 0) {
         obbox.ltx = bm_obj->obbox.ltx; obbox.lty = bm_obj->obbox.lty;
         obbox.rbx = bm_obj->obbox.rbx; obbox.rby = bm_obj->obbox.rby;
      } else {
         if (obbox.ltx!=bm_obj->obbox.ltx || obbox.lty!=bm_obj->obbox.lty ||
               obbox.rbx!=bm_obj->obbox.rbx || obbox.rby!=bm_obj->obbox.rby) {
            sprintf(gszMsgBox, TgLoadString(STID_FUNC_DIFF_SZ_XBM_XPM_SUBOBJS),
                  orig_cmd);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            return FALSE;
         }
      }
   }
   if (num_bm < 2) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_MORE_1_XBM_XPM_SUBOBJS),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!IntExpression(fps_str, &fps, orig_cmd)) {
      return FALSE;
   }
   if (fps < 1 || fps > 60) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
            orig_cmd, "frames_per_second", 1, 60, fps);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ms_frame_interval = round(1000 / fps) - 1;
   if (strcmp(style_str, "linear") == 0) {
      style = FD_STYLE_LINEAR;
   } else if (strcmp(style_str, "ping_pong") == 0) {
      style = FD_STYLE_PINGPONG;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_UNDEFINED_ARG),
            orig_cmd, "style", 1, 60);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (strcmp(times_str, "infinite") == 0) {
      times = (-1);
   } else {
      if (!IntExpression(times_str, &times, orig_cmd)) {
         return FALSE;
      }
   }
   iteration = 0;
#ifdef _NO_GETTIMEOFDAY
   ftime(&start);
   ms_start_time = ((long)start.time * 1000) + ((long)start.millitm);
#else /* ~_NO_GETTIMEOFDAY */
   gettimeofday(&start, &zone);
   ms_start_time = ms_time(&start);
#endif /* _NO_GETTIMEOFDAY */
   while (rc == TRUE && (times < 0 || iteration < times)) {
      int looping=TRUE, direction=FD_FORWARD;

      if (times >= 0) iteration++;
      cur_obj = obj_ptr->detail.r->first;
      while (looping) {
         struct timeval timeout;
#ifdef _NO_GETTIMEOFDAY
         struct timeb now;
#else /* ~_NO_GETTIMEOFDAY */
         struct timeval now;
#endif /* _NO_GETTIMEOFDAY */
         long ms_cur_time, ms_interval;
         int status;

         prev_obj = cur_obj->prev;
         next_obj = cur_obj->next;
         cur_obj->prev = cur_obj->next = NULL;
         switch (cur_obj->type) {
         case OBJ_XBM:
            if (iconWindowShown) {
               DrawXBmObj(iconWindow, 0, 0, cur_obj);
            } else {
               DrawXBmObj(drawWindow, drawOrigX, drawOrigY, cur_obj);
            }
            break;
         case OBJ_XPM:
            if (iconWindowShown) {
               DrawXPmObj(iconWindow, 0, 0, cur_obj);
            } else {
               DrawXPmObj(drawWindow, drawOrigX, drawOrigY, cur_obj);
            }
            break;
         }
         XSync(mainDisplay, FALSE);
         cur_obj->prev = prev_obj;
         cur_obj->next = next_obj;
         switch (style) {
         case FD_STYLE_LINEAR:
            if ((cur_obj=cur_obj->next) == NULL)
               looping = FALSE;
            break;
         case FD_STYLE_PINGPONG:
            switch (direction) {
            case FD_FORWARD:
               if (cur_obj->next == NULL) {
                  if ((cur_obj=cur_obj->prev) == obj_ptr->detail.r->first) {
                     looping = FALSE;
                  } else {
                     direction = FD_REVERSE;
                  }
               } else {
                  cur_obj = cur_obj->next;
               }
               break;
            case FD_REVERSE:
               if ((cur_obj=cur_obj->prev) == obj_ptr->detail.r->first) {
                  looping = FALSE;
               }
               break;
            }
            break;
         }
         EnterExecTightLoop();
         do
         {
            FD_ZERO(&fdset);
            FD_SET(select_width-1, &fdset);

#ifdef _NO_GETTIMEOFDAY
            ftime(&now);
            ms_cur_time = ((long)now.time * 1000) + ((long)now.millitm);
#else /* ~_NO_GETTIMEOFDAY */
            gettimeofday(&now, &zone);
            ms_cur_time = ms_time(&now);
#endif /* _NO_GETTIMEOFDAY */
            ms_interval = ms_start_time + ms_frame_interval - ms_cur_time;
            while (ms_interval <= 0) {
               ms_start_time = ms_cur_time;
               ms_interval = ms_start_time + ms_frame_interval - ms_cur_time;
            }
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000 * ms_interval;

#ifdef __hpux
            status = select(select_width, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
            status = select(select_width, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */

            if (status < 0) {
               if (errno == EINTR) {
                  /* interrupted by a system call, minor timing problem */
                  CheckInterrupt(TRUE);
               } else {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_FUNC_SELECT_SYS_CALL_FAILED),
                        orig_cmd);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  looping = FALSE;
                  rc = FALSE;
                  break;
               }
            } else if (status == 0) {
               /* timeout without any X events */
               CheckInterrupt(TRUE);
               break;
            } else {
               if (CheckExecInterrupt(orig_cmd)) {
                  looping = FALSE;
                  rc = FALSE;
                  userAbortExec = TRUE;
                  break;
               }
            }
         } while (looping && ms_interval > 0);
         ExitExecTightLoop();
      }
   }

/* RedrawAnArea(botObj, obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         obj_ptr->bbox.lty-GRID_ABS_SIZE(1), obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         obj_ptr->bbox.rby+GRID_ABS_SIZE(1)); */

   cur_obj = obj_ptr->detail.r->first;
   prev_obj = cur_obj->prev;
   next_obj = cur_obj->next;
   cur_obj->prev = cur_obj->next = NULL;
   switch (cur_obj->type) {
   case OBJ_XBM:
      if (iconWindowShown) {
         DrawXBmObj(iconWindow, 0, 0, cur_obj);
      } else {
         DrawXBmObj(drawWindow, drawOrigX, drawOrigY, cur_obj);
      }
      break;
   case OBJ_XPM:
      if (iconWindowShown) {
         DrawXPmObj(iconWindow, 0, 0, cur_obj);
      } else {
         DrawXPmObj(drawWindow, drawOrigX, drawOrigY, cur_obj);
      }
      break;
   }
   XSync(mainDisplay, FALSE);
   cur_obj->prev = prev_obj;
   cur_obj->next = next_obj;

   return rc;
}

static
int DoReadFileIntoAttr(attr_ptr, attr_owner_obj, file_name, orig_cmd)
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj;
   char *file_name, *orig_cmd;
{
   FILE *fp=NULL;

   if (*file_name == '|') {
      if (inHyperSpace && !allowLaunchInHyperSpace) {
         XFlush(mainDisplay);
         XSync(mainDisplay, False);
         sprintf(gszMsgBox, TgLoadString(STID_LAUNCH_NOT_AUTO_IN_HYPER_EXEC),
               &file_name[1]);
         if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
            MsgBox(TgLoadString(STID_USER_ABORT_LAUNCH_IN_HYPER), TOOL_NAME,
                  INFO_MB);
            return FALSE;
         }
      }
      if (!FindProgramInPath(&file_name[1], NULL, FALSE)) {
         return FALSE;
      }
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
            &file_name[1]);
      SetStringStatus(gszMsgBox);
      if (!PRTGIF) XSync(mainDisplay, False);
      fp = (FILE*)popen(&file_name[1], "r");
   } else {
      fp = fopen(file_name, "r");
   }
   if (fp == NULL) {
      if (*file_name == '|') {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD_EXEC_CMD),
               &file_name[1], orig_cmd);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_FILE_READ_EXEC_CMD),
               file_name, orig_cmd);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   PrepareToReplaceAnObj(attr_owner_obj);

   JustReadFileIntoAttr(fp, attr_ptr, attr_owner_obj);

   if (*file_name == '|') {
      pclose(fp);
      SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   } else {
      fclose(fp);
   }
   return TRUE;
}

int ExecReadFileIntoAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* read_file_into_attr(file_name,result_attribute); */
{
   char *file_name=argv[0], *attr_name=argv[1];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj;
   int rc;

   UtilRemoveQuotes(file_name);
   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   SaveStatusStrings();
   rc = DoReadFileIntoAttr(attr_ptr, attr_owner_obj, file_name, orig_cmd);
   RestoreStatusStrings();
   return rc;
}

static
int ExecWriteOrAppendAttrIntoFile(argv, obj_ptr, orig_cmd, open_mode)
   char **argv, *orig_cmd, *open_mode;
   struct ObjRec *obj_ptr;
   /* write_attr_into_file(attribute,file_name); */
   /* append_attr_into_file(attribute,file_name); */
{
   char *attr_name=argv[0], *file_name=argv[1];
   struct AttrRec *attr_ptr=NULL;
   FILE *fp=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(file_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (inHyperSpace && !allowLaunchInHyperSpace) {
      XFlush(mainDisplay);
      XSync(mainDisplay, False);
      sprintf(gszMsgBox, TgLoadString(STID_WRITE_NOT_AUTO_IN_HYPER_EXEC),
            file_name);
      if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
         MsgBox(TgLoadString(STID_USER_ABORT_WRITE_IN_HYPER), TOOL_NAME,
               INFO_MB);
         return FALSE;
      }
   }
   if ((fp=fopen(file_name, open_mode)) == NULL) {
      sprintf(execDummyStr, TgLoadString(STID_CANT_OPEN_FILE_READ_EXEC_CMD),
            file_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;

   fprintf(fp, "%s\n", attr_ptr->attr_value.s);
   if (attr_ptr->obj->detail.t->minilines.first != NULL) {
      MiniLineInfo *pMiniLine=NULL;

      for (pMiniLine=attr_ptr->obj->detail.t->minilines.first->next;
            pMiniLine != NULL; pMiniLine=pMiniLine->next) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=ConvertMiniLineToString(pMiniLine,
               &need_to_free_tmp_buf);

         if (fprintf(fp, "%s\n", tmp_buf) == EOF) {
            writeFileFailed = TRUE;
         }
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         if (writeFileFailed) break;
      }
   }
   fclose(fp);

   if (writeFileFailed) return FailToWriteFileMessage(file_name);
   return TRUE;
}

int ExecWriteAttrIntoFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* write_attr_into_file(attribute,file_name); */
{
   return ExecWriteOrAppendAttrIntoFile(argv, obj_ptr, orig_cmd, "w");
}

int ExecAppendAttrIntoFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* append_attr_into_file(attribute,file_name); */
{
   return ExecWriteOrAppendAttrIntoFile(argv, obj_ptr, orig_cmd, "a");
}

int ExecSelectObjByName(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* select_obj_by_name(obj_name); */
{
   char *obj_name=argv[0];
   struct ObjRec *owner_obj=NULL, *named_obj;

   UtilRemoveQuotes(obj_name);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, NULL);
   if (topSel != NULL) RemoveAllSel();
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (owner_obj != NULL) {
      sprintf(execDummyStr, TgLoadString(STID_BAD_NAME_TOP_LEVEL_ONLY_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   AddNewSelObj(named_obj);
   UpdSelBBox();

   return TRUE;
}

void ExecSelectTopObj(obj_ptr, orig_cmd)
   char *orig_cmd;
   struct ObjRec *obj_ptr;
   /* select_top_obj(); */
{
   if (topObj == NULL) {
      sprintf(execDummyStr, TgLoadString(STID_NO_OBJ_SEL_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      gnAbortExec = TRUE;
   } else {
      if (topSel != NULL) RemoveAllSel();

      SelectTopObj();
   }
}

void ExecDelAllSelObj(obj_ptr, orig_cmd)
   char *orig_cmd;
   struct ObjRec *obj_ptr;
   /* delete_selected_obj(); */
{
   if (topSel == NULL) {
      sprintf(execDummyStr, TgLoadString(STID_NO_OBJ_SEL_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      gnAbortExec = TRUE;
   } else {
      DelAllSelObj();
   }
}

void ExecUnSelectAllObj(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* unselect_all_obj(); */
{
   RemoveAllSel();
}

int ExecMoveSelObjRel(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_selected_obj_relative(dx,dy); */
{
   char *dx_str=argv[0], *dy_str=argv[1];
   int dx, dy;

   if (topSel == NULL) return BadSelectedObj(orig_cmd);

   UtilRemoveQuotes(dx_str);
   UtilRemoveQuotes(dy_str);
   if (!IntExpression(dx_str, &dx, orig_cmd) ||
         !IntExpression(dy_str, &dy, orig_cmd)) {
      return FALSE;
   }
   if (dx != 0 || dy != 0) {
      MoveAllSel(dx, dy);
      UpdSelBBox();
      SetFileModified(TRUE);
   }
   return TRUE;
}

int ExecRepeat(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* repeat(attr_name,times); */
{
   char *attr_name=argv[0], *times_str=argv[1];
   int i, times=(-1), rc=TRUE;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(times_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (strcmp(times_str, "infinite") != 0) {
      if (!IntExpression(times_str, &times, orig_cmd)) {
         return FALSE;
      }
   }
   for (i=0; times < 0 || i < times; i++) {
      rc = DoExec(attr_ptr, attr_owner_obj);
      if (!rc) break;
      if (times < 0) i--;
   }
   return rc;
}

int ExecHyperJump(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* hyperjump(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   warpToAttr = attr_ptr;
   return TRUE;
}

/* do not translate -- program constants */
static char *CGICharMap[] = {
   "                                ",
   "+         *  -. 0123456789      ",
   "@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _",
   " abcdefghijklmnopqrstuvwxyz     ",
   NULL
};

static
int MapCGIChars(buf, buf_sz, orig_str)
   char *buf, *orig_str;
   int buf_sz;
{
   int count=0;

   for ( ; *orig_str != '\0'; orig_str++) {
      unsigned char orig_code=(unsigned char)(*orig_str);
      char code;

      if (orig_code < 0x20) {
         code = ' ';
      } else if (orig_code < 0x40) {
         code = CGICharMap[1][orig_code-0x20];
      } else if (orig_code < 0x60) {
         code = CGICharMap[2][orig_code-0x40];
      } else if (orig_code < 0x80) {
         code = CGICharMap[3][orig_code-0x60];
      } else {
         code = ' ';
      }
      if (code == ' ') {
         if (count+3 >= buf_sz) break;
         sprintf(&buf[count], "%%%02X", (int)orig_code);
         count += 3;
      } else {
         if (count+1 >= buf_sz) break;
         buf[count++] = code;
      }
   }
   buf[count] = '\0';
   return count;
}

int ExecMakeCGIQuery(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* make_cgi_query(dest_attr_name,url_name,list_attr_name); */
{
   char *dest_attr_name=argv[0], *url_name=argv[1], *list_attr_name=argv[2];
   struct AttrRec *dest_attr=NULL, *list_attr=NULL;
   struct ObjRec *dest_attr_owner_obj=NULL;
   MiniLineInfo *pMiniLine=NULL;
   char *buf=NULL, *c_ptr, *attr_names_buf=NULL;
   int buf_sz=0, buf_len=0, tmp_sz=0, num_attrs=0;
   int found_name=FALSE, seen_first_attr=FALSE, null_url=FALSE;

   UtilRemoveQuotes(dest_attr_name);
   UtilRemoveQuotes(url_name);
   UtilRemoveQuotes(list_attr_name);

   if (strcmp(url_name, "NULL") == 0) {
      null_url = TRUE;
      *url_name = '\0';
   }
   buf_sz = strlen(url_name);
   buf = (char*)malloc((buf_sz+1)*sizeof(char));
   if (buf == NULL) return FailAllocMessage();

   sprintf(buf, "%s", url_name);
   buf_len = buf_sz;

   sprintf(execDummyStr, "%s=", dest_attr_name);
   dest_attr = FindAttrWithName(obj_ptr, execDummyStr, &dest_attr_owner_obj);
   if (dest_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", list_attr_name);
   list_attr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (list_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   tmp_sz = 0;

   for (pMiniLine=list_attr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);

      tmp_sz += strlen(tmp_buf)+1;
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }
   attr_names_buf = (char*)malloc((tmp_sz+2)*sizeof(char));
   if (attr_names_buf == NULL) {
      free(buf);
      return FailAllocMessage();
   }
   tmp_sz = 0;
   found_name = (!list_attr->nameshown);
   for (pMiniLine=list_attr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);

      if (found_name) {
         strcpy(&attr_names_buf[tmp_sz], tmp_buf);
         tmp_sz += strlen(tmp_buf);
      } else if ((c_ptr=strchr(tmp_buf, '=')) != NULL) {
         found_name = TRUE;
         strcpy(&attr_names_buf[tmp_sz], ++c_ptr);
         tmp_sz += strlen(c_ptr);
      } else {
         continue;
      }
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);

      attr_names_buf[tmp_sz++] = '\n';
   }
   attr_names_buf[tmp_sz] = '\0';
   for (num_attrs=0, c_ptr=strtok(attr_names_buf, " ,\t\n\r"); c_ptr != NULL;
         num_attrs++, c_ptr=strtok(NULL, " ,\t\n\r")) {
      struct AttrRec *attr_ptr;
      int len, buf1_index;
      char *buf1=NULL;

      sprintf(execDummyStr, "%s=", c_ptr);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
      if (attr_ptr == NULL) {
         free(buf);
         free(attr_names_buf);
         return BadAttr(execDummyStr, orig_cmd);
      }
      len = strlen(execDummyStr);
      for (pMiniLine=attr_ptr->obj->detail.t->minilines.first;
            pMiniLine != NULL; pMiniLine=pMiniLine->next) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=NULL;

         if (pMiniLine == attr_ptr->obj->detail.t->minilines.first) {
            char *attr_value=NULL;

            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            attr_value = UtilStrDup(tmp_buf);
            if (attr_value == NULL) FailAllocMessage();
            ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
            len += strlen(attr_value)+1;
            UtilFree(attr_value);
         } else {
            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            len += strlen(tmp_buf)+1;
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         }
      }
      len = len*3;
      buf1 = (char*)malloc((len+1)*sizeof(char));
      if (buf1 == NULL) {
         free(buf);
         free(attr_names_buf);
         return FailAllocMessage();
      }
      if (seen_first_attr) {
         *buf1 = '&';
      } else {
         *buf1 = '?';
         seen_first_attr = TRUE;
      }
      buf1_index = 1+MapCGIChars(&buf1[1], len-1, c_ptr);
      buf1[buf1_index++] = '=';
      for (pMiniLine=attr_ptr->obj->detail.t->minilines.first;
            pMiniLine != NULL; pMiniLine=pMiniLine->next) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=NULL;

         if (pMiniLine == attr_ptr->obj->detail.t->minilines.first) {
            char *attr_value=NULL;

            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            attr_value = UtilStrDup(tmp_buf);
            if (attr_value == NULL) FailAllocMessage();
            ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
            buf1_index += MapCGIChars(&buf1[buf1_index], len-buf1_index,
                  attr_value);
            UtilFree(attr_value);
         } else {
            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            buf1_index += MapCGIChars(&buf1[buf1_index], len-buf1_index,
                  tmp_buf);
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         }
         if (pMiniLine->next != NULL && buf1_index < len-1) {
            buf1_index += MapCGIChars(&buf1[buf1_index], len-buf1_index, "\n");
         }
      }
      buf1[buf1_index] = '\0';
      if ((buf=(char*)realloc(buf, buf1_index+buf_sz+1)) == NULL) {
         free(buf);
         free(attr_names_buf);
         free(buf1);
         return FailAllocMessage();
      }
      strcpy(&buf[buf_sz], buf1);
      buf_sz += buf1_index;
      free(buf1);
   }
   if (null_url && *buf == '?') {
      char *psz=buf, *psz1=(&buf[1]);

      while (*psz1 != '\0') {
         *psz++ = *psz1++;
      }
      *psz = '\0';
   }
   ReplaceAttrFirstValue(dest_attr_owner_obj, dest_attr, buf);
   free(buf);
   free(attr_names_buf);
   return TRUE;
}

static XComposeStatus c_stat;

int ExecWaitClick(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* wait_click(cursor_name,grab,attr_name); */
{
   char *cursor_name=argv[0], *grab_str=argv[1], *attr_name=argv[2], buf[80];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   Cursor cursor=(Cursor)0;
   int grab=FALSE, rc=TRUE, done=FALSE;

   UtilRemoveQuotes(cursor_name);
   UtilRemoveQuotes(grab_str);
   UtilRemoveQuotes(attr_name);
   grab = (strcmp(grab_str, "TRUE") == 0);
   if (strcmp(cursor_name, "NULL") != 0) {
      cursor = NewFontCursor(cursor_name);
      if (cursor == (Cursor)0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_CURSOR_WHILE_EXEC_CMD),
               cursor_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) {
      if (cursor != (Cursor)0) DeleteFontCursor(cursor);
      return BadAttr(execDummyStr, orig_cmd);
   }
   if (cursor != (Cursor)0) {
      SetWindowCursor(drawWindow, cursor);
      SetWindowCursor(mainWindow, cursor);
   }
   if (grab) {
      XGrabPointer(mainDisplay, rootWindow, False, ButtonPressMask,
            GrabModeAsync, GrabModeAsync, None,
            cursor==(Cursor)0 ? defaultCursor : cursor, CurrentTime);
   }
   while (!done) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      if (input.type == Expose || input.type == VisibilityNotify) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.type == KeyPress) {
         char s[80];
         XKeyEvent *key_ev=(&(input.xkey));
         KeySym key_sym;
         int has_ch=FALSE;

         has_ch = XLookupString(key_ev, s, sizeof(s), &key_sym, &c_stat);
         TranslateKeys(s, &key_sym);
         if (CharIsESC(key_ev, s, key_sym, &has_ch)) {
            userAbortExec = TRUE;
            rc = FALSE;
            done = TRUE;
         } else {
            int button_num=INVALID;

            switch (key_sym) {
            case XK_Up:
            case XK_KP_Up: button_num = 0; break;

            case XK_Left:
            case XK_KP_Left: button_num = 1; break;

            case XK_space:
            case XK_KP_Space:
            case XK_Tab:
            case XK_Return:
            case XK_KP_Enter: button_num = 2; break;

            case XK_Right:
            case XK_KP_Right: button_num = 3; break;

            case XK_Down:
            case XK_KP_Down: button_num = 4; break;

            default: break;
            }
            if (button_num != INVALID) {
               sprintf(buf, "%1d", button_num);
               ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
               done = TRUE;
            }
         }
      } else if (input.type == ButtonPress) {
         int button_num=0;

         if (grab) {
            XUngrabPointer(mainDisplay, CurrentTime);
         }
         switch (input.xbutton.button) {
         case Button1: button_num = 1; break;
         case Button2: button_num = 2; break;
         case Button3: button_num = 3; break;
         default: button_num = (input.xbutton.button-Button1)+1; break;
         }
         sprintf(buf, "%1d", button_num);
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
         done = TRUE;
      }
   }
   if (cursor != (Cursor)0) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
      DeleteFontCursor(cursor);
   }
   return rc;
}

int ExecSleep(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* sleep(cursor_name,ms_interval); */
{
   char *cursor_name=argv[0], *ms_interval_str=argv[1];
   Cursor cursor=(Cursor)0;
   int select_width=XConnectionNumber(mainDisplay)+1, rc=TRUE, val;
   long ms_start_time, ms_interval, ms_togo;
   struct timeval timeout;
#ifdef _NO_GETTIMEOFDAY
   struct timeb now;
#else /* ~_NO_GETTIMEOFDAY */
   struct timeval now;
   struct timezone zone;
#endif /* _NO_GETTIMEOFDAY */
   fd_set fdset;

   UtilRemoveQuotes(cursor_name);
   UtilRemoveQuotes(ms_interval_str);
   if (!IntExpression(ms_interval_str, &val, orig_cmd)) {
      return FALSE;
   }
   ms_interval = (long)val;
   if (strcmp(cursor_name, "NULL") != 0) {
      cursor = NewFontCursor(cursor_name);
      if (cursor == (Cursor)0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_CURSOR_WHILE_EXEC_CMD),
               cursor_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   if (cursor != (Cursor)0) {
      SetWindowCursor(drawWindow, cursor);
      SetWindowCursor(mainWindow, cursor);
      XSync(mainDisplay, False);
   }
#ifdef _NO_GETTIMEOFDAY
   ftime(&now);
   ms_start_time = ((long)now.time * 1000) + ((long)now.millitm);
#else /* ~_NO_GETTIMEOFDAY */
   gettimeofday(&now, &zone);
   ms_start_time = ms_time(&now);
#endif /* _NO_GETTIMEOFDAY */
   ms_togo = ms_interval;

   EnterExecTightLoop();

   while (rc && ms_togo > 0) {
      int status;
      long ms_cur_time;

      FD_ZERO(&fdset);
      FD_SET(select_width-1, &fdset);
      timeout.tv_sec = (long)(ms_togo/1000);
      timeout.tv_usec = 1000 * (ms_togo % 1000);
#ifdef __hpux
      status = select(select_width, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
      status = select(select_width, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
      if (status < 0) {
         if (errno == EINTR) {
            /* interrupted by a system call, not a problem */
            CheckInterrupt(TRUE);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_INTERNAL_CMD_SELECT_FAILED),
                  orig_cmd);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            rc = FALSE;
         }
      } else if (status == 0) {
         /* good timeout */
         CheckInterrupt(TRUE);
         break;
      } else {
         rc = (!CheckExecInterrupt(orig_cmd));
         if (!rc) {
            userAbortExec = TRUE;
         }
      }
      if (rc) {
#ifdef _NO_GETTIMEOFDAY
         ftime(&now);
         ms_cur_time = ((long)now.time * 1000) + ((long)now.millitm);
#else /* ~_NO_GETTIMEOFDAY */
         gettimeofday(&now, &zone);
         ms_cur_time = ms_time(&now);
#endif /* _NO_GETTIMEOFDAY */
         ms_togo = ms_start_time+ms_interval-ms_cur_time;
      }
   }
   ExitExecTightLoop();

   if (cursor != (Cursor)0) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
      DeleteFontCursor(cursor);
   }
   return rc;
}

void ExecBeginAnimate(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* begin_animate(); */
{
   if (!BeginExecAnimate()) {
      gnAbortExec = TRUE;
   } else {
      ShowInterrupt(1);
   }
}

void ExecEndAnimate(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* end_animate(); */
{
   EndExecAnimate();
   while (HideInterrupt() > 0) ;
}

int ExecSetRedraw(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_redraw(true_or_false); */
{
   char *redraw_str=argv[0];

   UtilRemoveQuotes(redraw_str);
   if (strcmp(redraw_str, "TRUE") == 0) {
      execAnimateRedraw = TRUE;
   } else if (strcmp(redraw_str, "FALSE") == 0) {
      execAnimateRedraw = FALSE;
   } else {
      return BadArg("true_or_false", orig_cmd);
   }
   return TRUE;
}

int ExecSetSelObjColor(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_color(color_str); */
{
   char *color_str=argv[0];
   int index, new_alloc;

   UtilRemoveQuotes(color_str);
   UtilTrimBlanks(color_str);

   if ((index=QuickFindColorIndex(NULL, color_str, &new_alloc, FALSE)) ==
         INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_COLOR_NAME_WHILE_EXEC_CMD),
            color_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (obj_ptr != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[index]);
   }
   ChangeAllSelColor(index, FALSE);
   return TRUE;
}

int ExecSetSelObjFill(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_fill(fill_index); */
{
   char *index_str=argv[0];
   int index;

   UtilRemoveQuotes(index_str);

   if (!IntExpression(index_str, &index, orig_cmd)) {
      return FALSE;
   }
   if (index < 0 || index >= MAXPATTERNS) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
            orig_cmd, "fill_index", 0, MAXPATTERNS-1, index);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelFill(index, FALSE);
   return TRUE;
}

int ExecSetSelObjPen(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_pen(pen_index); */
{
   char *index_str=argv[0];
   int index;

   UtilRemoveQuotes(index_str);

   if (!IntExpression(index_str, &index, orig_cmd)) {
      return FALSE;
   }
   if (index < 0 || index >= MAXPATTERNS) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
            orig_cmd, "pen_index", 0, MAXPATTERNS-1, index);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelPen(index, FALSE);
   return TRUE;
}

int ExecSetSelObjLineWidth(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_line_width(width,aw,ah); */
{
   char *width_str=argv[0], *aw_str=argv[1], *ah_str=argv[2];
   int width=0, aw=0, ah=0;

   UtilRemoveQuotes(width_str);
   UtilRemoveQuotes(aw_str);
   UtilRemoveQuotes(ah_str);
   if (!IntExpression(width_str, &width, orig_cmd) ||
         !IntExpression(aw_str, &aw, orig_cmd) ||
         !IntExpression(ah_str, &ah, orig_cmd)) {
      return FALSE;
   }
   if (width < 0) {
      sprintf(gszMsgBox, TgLoadString(STID_WIDTH_RANGE_WHILE_EXEC_CMD),
            width_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (topSel == NULL) {
      int index=GetBestLineWidthIndex(width, aw, ah);

      ChangeAllSelLineWidth(index, FALSE);
   } else {
      char width_spec[40], aw_spec[40], ah_spec[40];

      sprintf(width_spec, "%1d", width);
      sprintf(aw_spec, "%1d", aw);
      sprintf(ah_spec, "%1d", ah);
      ChangeAllSelRealLineWidth(CHANGE_LINE_ALL, width, aw, ah,
            width_spec, aw_spec, ah_spec, FALSE);
   }
   return TRUE;
}

int ExecSetSelObjSpline(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_spline(spline_type); */
{
   char *spline_spec=argv[0];
   int index;

   UtilRemoveQuotes(spline_spec);
   UtilTrimBlanks(spline_spec);

   if (strcmp(spline_spec,"straight") == 0) {
      index = LT_STRAIGHT;
   } else if (strcmp(spline_spec,"spline") == 0) {
      index = LT_SPLINE;
   } else if (strcmp(spline_spec,"interpolated") == 0) {
      index = LT_INTSPLINE;
   } else if (strcmp(spline_spec,"structured") == 0) {
      index = LT_STRUCT_SPLINE;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_SPLINE_TYPE_WHILE_EXEC_CMD),
            spline_spec, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelLineType(index, FALSE);
   return TRUE;
}

int ExecSetSelObjArrow(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_arrow(arrow_type); */
{
   char *arrow_spec=argv[0];
   int index;

   UtilRemoveQuotes(arrow_spec);
   UtilTrimBlanks(arrow_spec);

   if (strcmp(arrow_spec,"none") == 0) {
      index = LS_PLAIN;
   } else if (strcmp(arrow_spec,"right") == 0) {
      index = LS_RIGHT;
   } else if (strcmp(arrow_spec,"left") == 0) {
      index = LS_LEFT;
   } else if (strcmp(arrow_spec,"double") == 0) {
      index = LS_DOUBLE;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ARROW_STYLE_WHILE_EXEC_CMD),
            arrow_spec, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelLineStyle(index, FALSE);
   return TRUE;
}

int ExecSetSelObjDash(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_dash(dash_index); */
{
   char *index_str=argv[0];
   int index;

   UtilRemoveQuotes(index_str);

   if (!IntExpression(index_str, &index, orig_cmd)) {
      return FALSE;
   }
   if (index < 0 || index >= MAXDASHES) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
            orig_cmd, "dash_index", 0, MAXDASHES-1, index);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelLineDash(index, FALSE);
   return TRUE;
}

int ExecSetSelObjTransPat(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_trans_pat(trans_pat); */
{
   char *trans_pat_str=argv[0];
   int trans_pat=0;

   UtilRemoveQuotes(trans_pat_str);

   if (!IntExpression(trans_pat_str, &trans_pat, orig_cmd)) {
      return FALSE;
   }
   if (trans_pat != 0) {
      trans_pat = TRANSPAT_MODE;
   } else {
      trans_pat = NO_TRANSPAT_MODE;
   }
   ChangeAllSelTransPat(trans_pat, FALSE);
   return TRUE;
}

int ExecSetSelObjRCBRadius(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_obj_rcb_radius(rcb_radius); */
{
   char *rcb_radius_str=argv[0];
   int rcb_radius=0;

   UtilRemoveQuotes(rcb_radius_str);

   if (!IntExpression(rcb_radius_str, &rcb_radius, orig_cmd)) {
      return FALSE;
   }
   if (rcb_radius < 4) {
      sprintf(gszMsgBox, TgLoadString(STID_RCB_RAD_RANGE_WHILE_EXEC_CMD),
            rcb_radius_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeAllSelRCBRadius(rcb_radius);
   return TRUE;
}

int ExecSetSelTextVSpace(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_vspace(v_space); */
{
   char *v_space_str=argv[0];
   int v_space=0;

   UtilRemoveQuotes(v_space_str);

   if (!IntExpression(v_space_str, &v_space, orig_cmd)) {
      return FALSE;
   }
   ChangeVSpace(v_space);
   return TRUE;
}

int ExecSetSelTextJust(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_just(just_type); */
{
   char *just_spec=argv[0];
   int just=JUST_L;

   UtilRemoveQuotes(just_spec);
   UtilTrimBlanks(just_spec);

   if (strcmp(just_spec,"left") == 0) {
      just = JUST_L;
   } else if (strcmp(just_spec,"center") == 0) {
      just = JUST_C;
   } else if (strcmp(just_spec,"right") == 0) {
      just = JUST_R;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_TEXT_JUST_WHILE_EXEC_CMD),
            just_spec, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeFontJust(just);
   return TRUE;
}

int ExecSetSelTextFont(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_font(font_spec); */
{
   char *font_spec=argv[0];
   int font=0, style=0;

   UtilRemoveQuotes(font_spec);
   UtilTrimBlanks(font_spec);

   if (!GetFontAndStyle(font_spec, &font, &style, TRUE)) {
      if (PRTGIF && !cmdLineOpenDisplay) {
         for (font=0; font < MAXFONTS; font++) {
            if (strcmp(font_spec, defFontMenuStr[font]) == 0) {
               ChangeFont(font, TRUE);
               return TRUE;
            }
         }
      } else {
         for (font=0; font < numFonts; font++) {
            if (strcmp(font_spec, fontMenuStr[font]) == 0) {
               ChangeFont(font, TRUE);
               return TRUE;
            }
         }
      }
      sprintf(gszMsgBox, TgLoadString(STID_BAD_FONT_NAME_WHILE_EXEC_CMD),
            font_spec, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeFont(font, TRUE);
   ChangeFontStyle(style);

   return TRUE;
}

int ExecSetSelTextStyle(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_style(textstyle); */
{
   char *style_spec=argv[0];
   int style=0;

   UtilRemoveQuotes(style_spec);
   UtilTrimBlanks(style_spec);

   if (strcmp(style_spec, "r") == 0) {
      style = STYLE_NR;
   } else if (strcmp(style_spec, "b") == 0) {
      style = STYLE_BR;
   } else if (strcmp(style_spec, "i") == 0) {
      style = STYLE_NI;
   } else if (strcmp(style_spec, "bi") == 0) {
      style = STYLE_BI;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_STYLE_NAME_WHILE_EXEC_CMD),
            style_spec, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ChangeFontStyle(style);

   return TRUE;
}

int ExecSetSelTextSize(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_size(size); */
{
   char *size_spec=argv[0], *psz=NULL;
   int size=INVALID, sz_unit=INVALID, use_point_size=FALSE;
   int size_index=0, min_diff=0, min_size_index=0;

   UtilRemoveQuotes(size_spec);

   if ((psz=strstr(size_spec, "point")) != NULL ||
         (psz=strstr(size_spec, "pt")) != NULL) {
      *psz = '\0';
      use_point_size = TRUE;
   }
   if (!IntExpression(size_spec, &size, orig_cmd)) {
      return FALSE;
   }
   if (use_point_size) {
      sz_unit = PointSizeToSzUnit(size);
   } else {
      sz_unit = FontSizeToSzUnit(size);
   }
   if (topSel == NULL) {
      min_diff = abs(fontSzUnits[0] - sz_unit);
      for (size_index=1; size_index < numFontSizes; size_index++) {
         int diff=abs(fontSzUnits[size_index]-sz_unit);

         if (diff < min_diff) {
            min_diff = diff;
            min_size_index = size_index;
         }
      }
      ChangeFontSize(min_size_index);
   } else {
      ChangeAllSelRealSzUnit(sz_unit, FALSE);
   }
   return TRUE;
}

int ExecSetSelTextUnderline(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_underline(underline); */
{
   char *underline_on_str=argv[0];
   int underline_on=0;

   UtilRemoveQuotes(underline_on_str);

   if (!IntExpression(underline_on_str, &underline_on, orig_cmd)) {
      return FALSE;
   }
   ChangeFontUnderline(underline_on != 0);
   return TRUE;
}

int ExecSetSelTextOverline(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_selected_text_overline(overline); */
{
   char *overline_on_str=argv[0];
   int overline_on=0;

   UtilRemoveQuotes(overline_on_str);

   if (!IntExpression(overline_on_str, &overline_on, orig_cmd)) {
      return FALSE;
   }
   ChangeFontOverline(overline_on != 0);
   return TRUE;
}

int ExecInc(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* inc(attr_name,expr); */
{
   char *attr_name=argv[0], *expr=argv[1], buf[40];
   int orig_ival, val;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(expr);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   orig_ival = atoi(attr_ptr->attr_value.s);
   if (!IntExpression(expr, &val, orig_cmd)) return FALSE;
   if (val != 0) {
      sprintf(buf, "%1d", orig_ival+val);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   return TRUE;
}

int ExecDec(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* dec(attr_name,expr); */
{
   char *attr_name=argv[0], *expr=argv[1], buf[40];
   int orig_ival, val;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(expr);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   orig_ival = atoi(attr_ptr->attr_value.s);
   if (!IntExpression(expr, &val, orig_cmd)) return FALSE;
   if (val != 0) {
      sprintf(buf, "%1d", orig_ival-val);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   return TRUE;
}

#define SHUFFLE_TO_TOP (TRUE)
#define SHUFFLE_TO_BOT (FALSE)

static
int DoShuffleObjToTopOrBottom(argv, obj_ptr, orig_cmd, to_top)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int to_top;
   /* shuffle_obj_to_top(obj_name); */
   /* shuffle_obj_to_bottom(obj_name); */
{
   char *obj_name=argv[0];
   struct ObjRec *owner_obj=NULL, *top_owner=NULL, *named_obj;
   int ltx, lty, rbx, rby;
   struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

   UtilRemoveQuotes(obj_name);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, &top_owner);
   if (named_obj == NULL) return BadObjName(obj_name, orig_cmd);

   if (owner_obj == NULL) {
      if (to_top) {
         if (named_obj == topObj) return TRUE;
      } else {
         if (named_obj == botObj) return TRUE;
      }
      ltx = named_obj->bbox.ltx; lty = named_obj->bbox.lty;
      rbx = named_obj->bbox.rbx; rby = named_obj->bbox.rby;
   } else {
      if (to_top) {
         if (named_obj == owner_obj->detail.r->first) return TRUE;
      } else {
         if (named_obj == owner_obj->detail.r->last) return TRUE;
      }
      ltx = top_owner->bbox.ltx; lty = top_owner->bbox.lty;
      rbx = top_owner->bbox.rbx; rby = top_owner->bbox.rby;
   }
   topSel = botSel = (struct SelRec *)malloc(sizeof(struct SelRec));
   if (topSel == NULL) {
      FailAllocMessage();
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      return FALSE;
   }
   topSel->next = topSel->prev = NULL;
   topSel->obj = (owner_obj==NULL ? named_obj : top_owner);
   UpdSelBBox();

   if (owner_obj == NULL) {
      if (to_top) {
         MoveSelToTop();
      } else {
         MoveSelToBot();
      }
   } else {
      PrepareToRecord (CMD_REPLACE, topSel, botSel, numObjSelected);

      if (to_top) {
         named_obj->prev->next = named_obj->next;
         if (named_obj == owner_obj->detail.r->last) {
            owner_obj->detail.r->last = named_obj->prev;
         } else {
            named_obj->next->prev = named_obj->prev;
         }
         named_obj->prev = NULL;
         named_obj->next = owner_obj->detail.r->first;
         owner_obj->detail.r->first->prev = named_obj;
         owner_obj->detail.r->first = named_obj;
      } else {
         named_obj->next->prev = named_obj->prev;
         if (named_obj == owner_obj->detail.r->first) {
            owner_obj->detail.r->first = named_obj->next;
         } else {
            named_obj->prev->next = named_obj->next;
         }
         named_obj->next = NULL;
         named_obj->prev = owner_obj->detail.r->last;
         owner_obj->detail.r->last->next = named_obj;
         owner_obj->detail.r->last = named_obj;
      }
      RecordCmd (CMD_REPLACE, NULL, topSel, botSel, numObjSelected);
   }
   free(topSel);
   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();

   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);

   return TRUE;
}

int ExecShuffleObjToTop(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* shuffle_obj_to_top(obj_name); */
{
   return DoShuffleObjToTopOrBottom(argv, obj_ptr, orig_cmd, SHUFFLE_TO_TOP);
}

int ExecShuffleObjToBot(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* shuffle_obj_to_bottom(obj_name); */
{
   return DoShuffleObjToTopOrBottom(argv, obj_ptr, orig_cmd, SHUFFLE_TO_BOT);
}

void ExecDisableUndo(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* disable_undo(); */
{
   if (historyDepth != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_UNDO_BUF_FLUSHED_BY_CMD), orig_cmd);
      Msg(gszMsgBox);
   }
   DisableUndo();
}

void ExecEnableUndo(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* enable_undo(); */
{
   EnableUndo();
}

int ExecGetDrawingArea(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_drawing_area(ltx_attr,lty_attr,rbx_attr,rby_attr); */
{
   char *ltx_attr_name=argv[0], *lty_attr_name=argv[1];
   char *rbx_attr_name=argv[2], *rby_attr_name=argv[3], buf[40];
   struct AttrRec *ltx_attr_ptr, *lty_attr_ptr, *rbx_attr_ptr, *rby_attr_ptr;
   struct ObjRec *ltx_attr_owner_obj=NULL, *lty_attr_owner_obj=NULL;
   struct ObjRec *rbx_attr_owner_obj=NULL, *rby_attr_owner_obj=NULL;

   UtilRemoveQuotes(ltx_attr_name); UtilRemoveQuotes(lty_attr_name);
   UtilRemoveQuotes(rbx_attr_name); UtilRemoveQuotes(rby_attr_name);
   sprintf(execDummyStr, "%s=", ltx_attr_name);
   ltx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &ltx_attr_owner_obj);
   if (ltx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", lty_attr_name);
   lty_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &lty_attr_owner_obj);
   if (lty_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rbx_attr_name);
   rbx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rbx_attr_owner_obj);
   if (rbx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rby_attr_name);
   rby_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rby_attr_owner_obj);
   if (rby_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", drawOrigX);
   ReplaceAttrFirstValue(ltx_attr_owner_obj, ltx_attr_ptr, buf);
   sprintf(buf, "%1d", drawOrigY);
   ReplaceAttrFirstValue(lty_attr_owner_obj, lty_attr_ptr, buf);
   sprintf(buf, "%1d", drawOrigX+drawWinW);
   ReplaceAttrFirstValue(rbx_attr_owner_obj, rbx_attr_ptr, buf);
   sprintf(buf, "%1d", drawOrigY+drawWinH);
   ReplaceAttrFirstValue(rby_attr_owner_obj, rby_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecGetSelObjBBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_selected_obj_bbox(ltx_attr,lty_attr,rbx_attr,rby_attr); */
{
   char *ltx_attr_name=argv[0], *lty_attr_name=argv[1];
   char *rbx_attr_name=argv[2], *rby_attr_name=argv[3], buf[40];
   struct AttrRec *ltx_attr_ptr, *lty_attr_ptr, *rbx_attr_ptr, *rby_attr_ptr;
   struct ObjRec *ltx_attr_owner_obj=NULL, *lty_attr_owner_obj=NULL;
   struct ObjRec *rbx_attr_owner_obj=NULL, *rby_attr_owner_obj=NULL;

   UtilRemoveQuotes(ltx_attr_name); UtilRemoveQuotes(lty_attr_name);
   UtilRemoveQuotes(rbx_attr_name); UtilRemoveQuotes(rby_attr_name);
   if (topSel == NULL) return BadSelectedObj(orig_cmd);

   sprintf(execDummyStr, "%s=", ltx_attr_name);
   ltx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &ltx_attr_owner_obj);
   if (ltx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", lty_attr_name);
   lty_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &lty_attr_owner_obj);
   if (lty_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rbx_attr_name);
   rbx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rbx_attr_owner_obj);
   if (rbx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rby_attr_name);
   rby_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rby_attr_owner_obj);
   if (rby_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", selObjLtX);
   ReplaceAttrFirstValue(ltx_attr_owner_obj, ltx_attr_ptr, buf);
   sprintf(buf, "%1d", selObjLtY);
   ReplaceAttrFirstValue(lty_attr_owner_obj, lty_attr_ptr, buf);
   sprintf(buf, "%1d", selObjRbX);
   ReplaceAttrFirstValue(rbx_attr_owner_obj, rbx_attr_ptr, buf);
   sprintf(buf, "%1d", selObjRbY);
   ReplaceAttrFirstValue(rby_attr_owner_obj, rby_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecGetNamedObjBBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_named_obj_bbox(obj_name,ltx_attr,lty_attr,rbx_attr,rby_attr); */
{
   char *obj_name=argv[0];
   char *ltx_attr_name=argv[1], *lty_attr_name=argv[2];
   char *rbx_attr_name=argv[3], *rby_attr_name=argv[4], buf[40];
   struct ObjRec *top_owner=NULL, *named_obj=NULL;
   struct AttrRec *ltx_attr_ptr, *lty_attr_ptr, *rbx_attr_ptr, *rby_attr_ptr;
   struct ObjRec *ltx_attr_owner_obj=NULL, *lty_attr_owner_obj=NULL;
   struct ObjRec *rbx_attr_owner_obj=NULL, *rby_attr_owner_obj=NULL;

   UtilRemoveQuotes(obj_name);
   UtilTrimBlanks(obj_name);
   UtilRemoveQuotes(ltx_attr_name); UtilRemoveQuotes(lty_attr_name);
   UtilRemoveQuotes(rbx_attr_name); UtilRemoveQuotes(rby_attr_name);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   sprintf(execDummyStr, "%s=", ltx_attr_name);
   ltx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &ltx_attr_owner_obj);
   if (ltx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", lty_attr_name);
   lty_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &lty_attr_owner_obj);
   if (lty_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rbx_attr_name);
   rbx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rbx_attr_owner_obj);
   if (rbx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rby_attr_name);
   rby_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rby_attr_owner_obj);
   if (rby_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", named_obj->obbox.ltx);
   ReplaceAttrFirstValue(ltx_attr_owner_obj, ltx_attr_ptr, buf);
   sprintf(buf, "%1d", named_obj->obbox.lty);
   ReplaceAttrFirstValue(lty_attr_owner_obj, lty_attr_ptr, buf);
   sprintf(buf, "%1d", named_obj->obbox.rbx);
   ReplaceAttrFirstValue(rbx_attr_owner_obj, rbx_attr_ptr, buf);
   sprintf(buf, "%1d", named_obj->obbox.rby);
   ReplaceAttrFirstValue(rby_attr_owner_obj, rby_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecMoveSelObjAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_selected_obj_absolute(ltx,lty); */
{
   char *ltx_str=argv[0], *lty_str=argv[1];
   int ltx, lty;

   UtilRemoveQuotes(ltx_str);
   UtilRemoveQuotes(lty_str);
   if (topSel == NULL) return BadSelectedObj(orig_cmd);

   if (!IntExpression(ltx_str, &ltx, orig_cmd) ||
         !IntExpression(lty_str, &lty, orig_cmd)) {
      return FALSE;
   }
   if (ltx != selObjLtX || lty != selObjLtY) {
      MoveAllSel(ltx-selObjLtX, lty-selObjLtY);
      UpdSelBBox();
      SetFileModified(TRUE);
   }
   return TRUE;
}

int ExecAssign(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* assign(attr_name,expr); */
{
   char *attr_name=argv[0], *expr=argv[1], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   struct VRec v;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(expr);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!EvalExpr(expr, &v)) return FALSE;

   switch (v.vtype) {
   case INT_VAL:
      sprintf(buf, "%1d", v.val.i);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
      break;
   case DBL_VAL:
      sprintf(buf, "%.12f", (float)(v.val.d));
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
      break;
   case NULL_VAL:
   case STR_VAL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_NUM_EXP_EXEC_CMD),
            expr, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (v.vtype == STR_VAL && v.val.s != NULL) free(v.val.s);
      return FALSE;
   }
   return TRUE;
}

int ExecStrCpy(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* strcpy(attr_name,str); */
{
   char *attr_name=argv[0], *the_str=argv[1];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, the_str);
   return TRUE;
}

int ExecStrCat(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* strcat(attr_name,str); */
{
   char *attr_name=argv[0], *the_str=argv[1], *new_str=NULL;
   int len=0;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   len = strlen(attr_ptr->attr_value.s)+strlen(the_str);
   new_str = (char*)malloc(len+1);
   if (new_str == NULL) FailAllocMessage();
   sprintf(new_str, "%s%s", attr_ptr->attr_value.s, the_str);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, new_str);
   free(new_str);

   return TRUE;
}

int ExecCopyStrToCutBuffer(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* copy_string_to_cut_buffer(str); */
{
   char *the_str=argv[0];
   int len=0;

   UtilRemoveQuotes(the_str);

   len = strlen(the_str);
   if (!WriteBufToCutBuffer(the_str, strlen(the_str), TRUE, FALSE, NULL)) {
      sprintf(gszMsgBox, TgLoadString(STID_WRITE_CUT_BUF_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

int ExecWhile(argv, raw_argv, obj_ptr, orig_cmd)
   char **argv, **raw_argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* while(expr,attr_to_exec); */
{
   char *raw_expr=raw_argv[0], *attr_name=argv[1];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   int rc=TRUE;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   while (rc) {
      int val;
      char *expr=convert_str(raw_expr, obj_ptr, TRUE);

      if (expr == NULL) {
         BadAttr(raw_expr, orig_cmd);
         return FALSE;
      } else if (!IntExpression(expr, &val, orig_cmd)) {
         free(expr);
         return FALSE;
      }
      free(expr);
      if (val == 0) break;
      if (!DoExec(attr_ptr, attr_owner_obj)) return FALSE;
      rc = (!CheckExecInterrupt(orig_cmd));
      if (!rc) {
         userAbortExec = TRUE;
      }
   }
   return rc;
}

int ExecIf(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* if(expr,then_attr_to_exec,else_attr_to_exec); */
{
   char *expr=argv[0], *then_attr_name=argv[1], *else_attr_name=argv[2];
   int val=0;

   UtilRemoveQuotes(then_attr_name);
   UtilRemoveQuotes(else_attr_name);

   if (!IntExpression(expr, &val, orig_cmd)) return FALSE;
   if (val) {
      struct AttrRec *then_attr_ptr=NULL;
      struct ObjRec *then_attr_owner_obj=NULL;

      if (strcmp(then_attr_name, "NULL") != 0) {
         sprintf(execDummyStr, "%s=", then_attr_name);
         then_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
               &then_attr_owner_obj);
         if (then_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      }
      if (then_attr_ptr != NULL &&
            !DoExec(then_attr_ptr, then_attr_owner_obj)) {
         return FALSE;
      }
   } else {
      struct AttrRec *else_attr_ptr=NULL;
      struct ObjRec *else_attr_owner_obj=NULL;

      if (strcmp(else_attr_name, "NULL") != 0) {
         sprintf(execDummyStr, "%s=", else_attr_name);
         else_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
               &else_attr_owner_obj);
         if (else_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      }
      if (else_attr_ptr != NULL &&
            !DoExec(else_attr_ptr, else_attr_owner_obj)) {
         return FALSE;
      }
   }
   return TRUE;
}

int ExecGetCurrentFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_current_file(attr_name); */
{
   char *attr_name=argv[0], *full_name=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (curFileDefined) {
      if (*curSymDir == '\0') {
         sprintf(gszMsgBox, "%s%c%s", curDir, DIR_SEP, curFileName);
      } else {
         sprintf(gszMsgBox, "%s%c%s", curSymDir, DIR_SEP, curFileName);
      }
      full_name = UtilStrDup(gszMsgBox);
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr,
         (full_name==NULL ? "" : full_name));
   if (full_name != NULL) free(full_name);
   return TRUE;

}

int ExecGetCurrentExportFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_current_export_file(attr_name); */
{
   char *attr_name=argv[0], *full_name=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (curFileDefined) {
      char *psz=NULL;

      sprintf(gszMsgBox, "%s%c%s", curDir, DIR_SEP, curFileName);
      if ((psz=UtilStrRChr(gszMsgBox, '.')) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_DOT_WHILE_EXEC_CMD),
               curDir, DIR_SEP, curFileName, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      strcpy(++psz, GetExportExt(whereToPrint));
      full_name = UtilStrDup(gszMsgBox);
      if (full_name == NULL) FailAllocMessage();
   }

   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr,
         (full_name==NULL ? "" : full_name));
   if (full_name != NULL) free(full_name);
   return TRUE;
}

int ExecGetCurrentDir(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_current_dir(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, curDir);
   return TRUE;

}

int ExecGetEnv(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* getenv(attr_name,env_var_name); */
{
   char *attr_name=argv[0], *env_var_name=argv[1], *c_ptr, *env_var_value=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(env_var_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if ((c_ptr=getenv(env_var_name)) != NULL) {
      env_var_value = UtilStrDup(c_ptr);
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr,
         (env_var_value==NULL ? "" : env_var_value));
   if (env_var_value != NULL) free(env_var_value);
   return TRUE;
}

int ExecStrLen(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* strlen(attr_name,str); */
{
   char *attr_name=argv[0], *the_str=argv[1], val_str[40];
   int len;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   len = strlen(the_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(val_str, "%1d", len);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, val_str);
   return TRUE;
}

int ExecSubStr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* substr(attr_name,str,start_index,length); */
{
   char *attr_name=argv[0], *the_str=argv[1];
   char *start_index_str=argv[2], *length_str=argv[3], *buf;
   int len, start_index=(-1), length=(-1);
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(start_index_str);
   len = strlen(the_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!IntExpression(start_index_str, &start_index, orig_cmd) ||
         !IntExpression(length_str, &length, orig_cmd)) {
      return FALSE;
   }
   if (start_index < 0) start_index = 0;
   if (length+start_index > len) {
      length = len-start_index;
   }
   if (length < 0) length = 0;
   if ((buf=UtilStrDup(the_str)) == NULL) {
      FailAllocMessage();
   } else {
      strncpy(buf, &the_str[start_index], length);
      buf[length] = '\0';
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, (buf==NULL ? "" : buf));
   if (buf != NULL) free(buf);
   return TRUE;
}

int ExecStrStr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* strstr(attr_name,str,substr); */
{
   char *attr_name=argv[0], *the_str=argv[1], *sub_str=argv[2], *c_ptr;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(sub_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   c_ptr = strstr(the_str, sub_str);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, (c_ptr==NULL ? "" : c_ptr));
   return TRUE;
}

int ExecStrRStr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* strrstr(attr_name,str,substr); */
{
   char *attr_name=argv[0], *the_str=argv[1], *sub_str=argv[2];
   char *c_ptr, *last_match=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(sub_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   for (c_ptr=strstr(the_str, sub_str); c_ptr != NULL;
         c_ptr=strstr(&last_match[1], sub_str)) {
      last_match = c_ptr;
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr,
         (last_match==NULL ? "" : last_match));
   return TRUE;
}

void ExecUnMakeSelObjIconic(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* unmake_selected_obj_iconic(); */
{
   struct SelRec *sel_ptr;
   int modified=FALSE;

   if (topSel == NULL) {
      BadSelectedObj(orig_cmd);
      gnAbortExec = TRUE;
      return;
   }
   StartCompositeCmd();
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      struct ObjRec *obj_ptr=sel_ptr->obj;

      if (obj_ptr->type == OBJ_ICON || obj_ptr->type == OBJ_PIN) {
         struct AttrRec *attr_ptr;

         modified = TRUE;
         PrepareToReplaceAnObj(obj_ptr);
         obj_ptr->type = OBJ_GROUP;
         attr_ptr = obj_ptr->fattr;
         for ( ; attr_ptr != NULL; attr_ptr = attr_ptr->next) {
            attr_ptr->inherited = FALSE;
         }
         AdjObjBBox(obj_ptr);
         RecordReplaceAnObj(obj_ptr);
      }
   }
   EndCompositeCmd();
   if (modified) {
      SetFileModified(TRUE);
   }
}

int ExecHyperJumpThenExec(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* hyperjump_then_exec(attr_name,attr_name_to_exec); */
{
   char *attr_name=argv[0], *attr_name_to_exec=argv[1];
   struct AttrRec *attr_ptr;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(attr_name_to_exec);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   warpToAttr = attr_ptr;
   if (cmdToExecAfterHyperJump != NULL) free(cmdToExecAfterHyperJump);
   sprintf(execDummyStr, "%s=", attr_name_to_exec);
   cmdToExecAfterHyperJump = UtilStrDup(execDummyStr);

   return TRUE;
}

#define SHOW_ATTR 0
#define HIDE_ATTR 1
#define SHOW_ATTR_NAME 2
#define HIDE_ATTR_NAME 3
#define SHOW_VALUE 4
#define HIDE_VALUE 5

static
void ReplaceAttrShown(obj_ptr, attr_ptr, show_type)
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;
   int show_type;
   /* obj_ptr better be a top-level object */
{
   int ltx, lty, rbx, rby, needs_redraw=FALSE;

   ltx = obj_ptr->bbox.ltx; lty = obj_ptr->bbox.lty;
   rbx = obj_ptr->bbox.rbx; rby = obj_ptr->bbox.rby;
   PrepareToReplaceAnObj(obj_ptr);
   switch (show_type) {
   case SHOW_ATTR:
   case SHOW_VALUE:
      attr_ptr->shown = TRUE;
      needs_redraw = TRUE;
      break;
   case HIDE_ATTR:
   case HIDE_VALUE:
      attr_ptr->shown = FALSE;
      needs_redraw = TRUE;
      break;
   case SHOW_ATTR_NAME:
      attr_ptr->nameshown = TRUE;
      if (attr_ptr->shown) needs_redraw = TRUE;
      UpdAttr(attr_ptr);
      break;
   case HIDE_ATTR_NAME:
      attr_ptr->nameshown = FALSE;
      if (attr_ptr->shown) needs_redraw = TRUE;
      UpdAttr(attr_ptr);
      break;
   }
   attr_ptr->obj->detail.t->cached_zoom = 0;
   if (attr_ptr->obj->detail.t->cached_bitmap != None) {
      XFreePixmap(mainDisplay, attr_ptr->obj->detail.t->cached_bitmap);
      attr_ptr->obj->detail.t->cached_bitmap = None;
   }
   if (attr_ptr->shown) {
      AdjObjCache(obj_ptr);
      AdjObjBBox(obj_ptr);
   }
   RecordReplaceAnObj(obj_ptr);
   if (needs_redraw) {
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
            obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
            obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
            obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   }
   SetFileModified(TRUE);
}

static
int ExecShowHideAttr(argv, obj_ptr, orig_cmd, show_type)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int show_type;
   /* show_attr(attr_name); */
   /* hide_attr(attr_name); */
   /* show_attr_name(attr_name); */
   /* hide_attr_name(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   if (attr_owner_obj == tgifObj) {
      return FileAttrNotAllowed(execDummyStr, orig_cmd);
   }
   switch (show_type) {
   case SHOW_ATTR:
      if (attr_ptr->shown) return TRUE;
      break;
   case HIDE_ATTR:
      if (!attr_ptr->shown) return TRUE;
      break;
   case SHOW_ATTR_NAME:
      if (*attr_ptr->attr_name.s == '\0' || attr_ptr->nameshown) return TRUE;
      break;
   case HIDE_ATTR_NAME:
      if (*attr_ptr->attr_name.s == '\0' || !attr_ptr->nameshown) return TRUE;
      break;
   }
   ReplaceAttrShown(attr_owner_obj, attr_ptr, show_type);

   return TRUE;
}

int ExecShowAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* show_attr(attr_name); */
{
   return ExecShowHideAttr(argv, obj_ptr, orig_cmd, SHOW_ATTR);
}

int ExecHideAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* hide_attr(attr_name); */
{
   return ExecShowHideAttr(argv, obj_ptr, orig_cmd, HIDE_ATTR);
}

int ExecShowAttrName(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* show_attr_name(attr_name); */
{
   return ExecShowHideAttr(argv, obj_ptr, orig_cmd, SHOW_ATTR_NAME);
}

int ExecHideAttrName(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* hide_attr_name(attr_name); */
{
   return ExecShowHideAttr(argv, obj_ptr, orig_cmd, HIDE_ATTR_NAME);
}

static
int ExecShowHideValue(argv, obj_ptr, orig_cmd, show_type)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int show_type;
   /* show_value(attr_value); */
   /* hide_value(attr_value); */
{
   char *attr_value=argv[0];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_value);
   sprintf(execDummyStr, "%s", attr_value);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   if (attr_owner_obj == tgifObj) {
      return FileAttrNotAllowed(execDummyStr, orig_cmd);
   }
   switch (show_type) {
   case SHOW_VALUE:
      if (attr_ptr->shown) return TRUE;
      break;
   case HIDE_VALUE:
      if (!attr_ptr->shown) return TRUE;
      break;
   }
   ReplaceAttrShown(attr_owner_obj, attr_ptr, show_type);

   return TRUE;
}

int ExecShowValue(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* show_value(attr_value); */
{
   return ExecShowHideValue(argv, obj_ptr, orig_cmd, SHOW_VALUE);
}

int ExecHideValue(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* hide_value(attr_value); */
{
   return ExecShowHideValue(argv, obj_ptr, orig_cmd, HIDE_VALUE);
}

int ExecGetAttrBBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_attr_bbox(ltx_attr,lty_attr,rbx_attr,rby_attr,attr_name); */
{
   char *ltx_attr_name=argv[0], *lty_attr_name=argv[1];
   char *rbx_attr_name=argv[2], *rby_attr_name=argv[3], buf[40];
   char *attr_name=argv[4];
   struct AttrRec *ltx_attr_ptr, *lty_attr_ptr, *rbx_attr_ptr, *rby_attr_ptr;
   struct AttrRec *attr_ptr;
   struct ObjRec *ltx_attr_owner_obj=NULL, *lty_attr_owner_obj=NULL;
   struct ObjRec *rbx_attr_owner_obj=NULL, *rby_attr_owner_obj=NULL;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(ltx_attr_name); UtilRemoveQuotes(lty_attr_name);
   UtilRemoveQuotes(rbx_attr_name); UtilRemoveQuotes(rby_attr_name);
   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   if (attr_owner_obj == tgifObj) {
      return FileAttrNotAllowed(execDummyStr, orig_cmd);
   }
   sprintf(execDummyStr, "%s=", ltx_attr_name);
   ltx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &ltx_attr_owner_obj);
   if (ltx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", lty_attr_name);
   lty_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &lty_attr_owner_obj);
   if (lty_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rbx_attr_name);
   rbx_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rbx_attr_owner_obj);
   if (rbx_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", rby_attr_name);
   rby_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &rby_attr_owner_obj);
   if (rby_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", attr_ptr->obj->obbox.ltx);
   ReplaceAttrFirstValue(ltx_attr_owner_obj, ltx_attr_ptr, buf);
   sprintf(buf, "%1d", attr_ptr->obj->obbox.lty);
   ReplaceAttrFirstValue(lty_attr_owner_obj, lty_attr_ptr, buf);
   sprintf(buf, "%1d", attr_ptr->obj->obbox.rbx);
   ReplaceAttrFirstValue(rbx_attr_owner_obj, rbx_attr_ptr, buf);
   sprintf(buf, "%1d", attr_ptr->obj->obbox.rby);
   ReplaceAttrFirstValue(rby_attr_owner_obj, rby_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecSizeSelObjAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* size_selected_obj_absolute(abs_w,abs_h); */
{
   char *abs_w_str=argv[0], *abs_h_str=argv[1];
   int abs_w, abs_h;

   UtilRemoveQuotes(abs_w_str);
   UtilRemoveQuotes(abs_h_str);
   if (topSel == NULL) return BadSelectedObj(orig_cmd);

   if (!IntExpression(abs_w_str, &abs_w, orig_cmd) ||
         !IntExpression(abs_h_str, &abs_h, orig_cmd)) {
      return FALSE;
   }
   if (abs_w < 0 || abs_h < 0) {
      if (abs_w < 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_WIDTH_WHILE_EXEC_CMD),
               abs_w_str, orig_cmd);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_HEIGHT_WHILE_EXEC_CMD),
               abs_h_str, orig_cmd);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (abs_w != selObjRbX-selObjLtX || abs_h != selObjRbY-selObjLtY) {
      SizeAllSelObj(abs_w, abs_h);
   }
   return TRUE;
}

int ExecSizeNamedObjAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* size_named_obj_absolute(obj_name,abs_w,abs_h); */
{
   char *obj_name=argv[0], *abs_w_str=argv[1], *abs_h_str=argv[2];
   int abs_w=0, abs_h=0;
   struct ObjRec *top_owner=NULL, *named_obj=NULL;

   UtilRemoveQuotes(obj_name);
   UtilTrimBlanks(obj_name);
   UtilRemoveQuotes(abs_w_str);
   UtilRemoveQuotes(abs_h_str);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (!IntExpression(abs_w_str, &abs_w, orig_cmd) ||
         !IntExpression(abs_h_str, &abs_h, orig_cmd)) {
      return FALSE;
   }
   if (abs_w < 0 || abs_h < 0) {
      if (abs_w < 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_WIDTH_WHILE_EXEC_CMD),
               abs_w_str, orig_cmd);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_HEIGHT_WHILE_EXEC_CMD),
               abs_h_str, orig_cmd);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (abs_w != named_obj->obbox.rbx-named_obj->obbox.ltx ||
         abs_h != named_obj->obbox.lty-named_obj->obbox.rby) {
      /* SizeAnObj() can only be called from within an internal command */
      SizeAnObj(named_obj, top_owner, abs_w, abs_h);
   }
   return TRUE;
}

int ExecMessageBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* message_box(attr_name,msg,title,style); */
{
   char *attr_name=argv[0], *msg=argv[1], *title=argv[2], *style_str=argv[3];
   char buf[MAXSTRING];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   int style=INFO_MB;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(msg);
   UtilRemoveQuotes(title);
   UtilRemoveQuotes(style_str);
   if (strcmp(attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (strcmp(title, "NULL") == 0) {
      title = TOOL_NAME;
   }
   if (strcmp(style_str, "NULL") == 0) {
      style = INFO_MB;
   } else if (strcmp(style_str, "info") == 0) {
      style = INFO_MB;
   } else if (strcmp(style_str, "ync") == 0) {
      style = YNC_MB;
   } else if (strcmp(style_str, "yn") == 0) {
      style = YN_MB;
   } else if (strcmp(style_str, "stop") == 0) {
      style = STOP_MB;
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_STYLE_WHILE_EXEC_CMD),
            style_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   /* do not translate -- program constants */
   switch (MsgBox(msg, title, style)) {
   case MB_ID_FAILED: strcpy(buf, "FAILED"); break;
   case MB_ID_OK: strcpy(buf, "OK"); break;
   case MB_ID_CANCEL: strcpy(buf, "CANCEL"); break;
   case MB_ID_YES: strcpy(buf, "YES"); break;
   case MB_ID_NO: strcpy(buf, "NO"); break;
   case MB_ID_EXTRA: strcpy(buf, "EXTRA"); break;
   default: strcpy(buf, "(unknown)"); break;
   }
   if (attr_ptr != NULL) {
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   return TRUE;
}

int ExecGetUserInput(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_user_input(attr_name,msg1,msg2); */
{
   char *attr_name=argv[0], *msg1=argv[1], *msg2=argv[2];
   char buf[MAXPATHLENGTH+1];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   int rc=INVALID;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(msg1);
   UtilRemoveQuotes(msg2);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   *buf = '\0';
   if (strcmp(msg2, "USE_CURRENT_DIR") == 0) {
      if (curDirIsLocal) {
         sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS), curDir);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_WORKING_DIRECTORY_IS),
               curLocalDir);
      }
      rc = Dialog(msg1, gszMsgBox, buf);
   } else if (strcmp(msg2, "NULL") == 0) {
      rc = Dialog(msg1, NULL, buf);
   } else {
      rc = Dialog(msg1, msg2, buf);
   }
   if (rc == INVALID) *buf = '\0';
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecAddAttrToSelObj(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* add_attr_to_selected_obj(attr_name,attr_value,abs_x,abs_y); */
{
   char *attr_name=argv[0], *attr_value=argv[1];
   char *abs_x_str=argv[2], *abs_y_str=argv[3];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *selected_obj=NULL;
   int abs_x=0, abs_y=0, ignore_x=FALSE, ignore_y=FALSE;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(attr_value);
   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_y_str);
   if (attr_name[0] == '!' && attr_name[1] == '.') {
      struct ObjRec *attr_owner_obj=NULL;

      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) {
         AddObj(NULL, topObj, tgifObj);
         AddNewSelObj(topObj);

         PrepareToReplaceAnObj(tgifObj);
         attr_ptr = AddAttrByNameAndValue(tgifObj, &execDummyStr[2],
               attr_value);
         recordCmdIncludeTgifObj = TRUE;
         RecordReplaceAnObj(tgifObj);
         recordCmdIncludeTgifObj = FALSE;

         UnlinkObj(topObj);
         FreeTopSel();
         SetFileModified(TRUE);
      } else if (attr_owner_obj != tgifObj) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_ATTR_NAME_WHILE_EXEC_CMD),
               attr_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         ReplaceAttrFirstValue(tgifObj, attr_ptr, attr_value);
      }
      return TRUE;
   } else if (topSel == NULL) {
      return BadSelectedObj(orig_cmd);
   } else if (topSel != botSel) {
      sprintf(gszMsgBox, TgLoadString(STID_TOO_MANY_OBJ_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (strchr(attr_name, '.') != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_ATTR_NAME_WHILE_EXEC_CMD),
            attr_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (strcmp(abs_x_str, "NULL") == 0) {
      ignore_x = TRUE;
   } else if (!IntExpression(abs_x_str, &abs_x, orig_cmd)) {
      return FALSE;
   }
   if (strcmp(abs_y_str, "NULL") == 0) {
      ignore_y = TRUE;
   } else if (!IntExpression(abs_y_str, &abs_y, orig_cmd)) {
      return FALSE;
   }
   selected_obj = topSel->obj;

   if (strcmp(attr_name, "NULL") == 0 || *attr_name == '\0') {
      *execDummyStr = '\0';
   } else {
      sprintf(execDummyStr, "%s=", attr_name);
   }
   attr_ptr = FindAttrWithName(selected_obj, execDummyStr, NULL);
   if (attr_ptr == NULL) {
      int ltx, lty, rbx, rby, x, y;

      ltx = selected_obj->bbox.ltx; lty = selected_obj->bbox.lty;
      rbx = selected_obj->bbox.rbx; rby = selected_obj->bbox.rby;
      x = (ignore_x ? selected_obj->obbox.ltx : abs_x);
      y = (ignore_y ? selected_obj->obbox.rby : abs_y);
      PrepareToReplaceAnObj(selected_obj);
      attr_ptr = AddAttrByNameAndValue(selected_obj, execDummyStr, attr_value);
      attr_ptr->shown = TRUE;
      attr_ptr->obj->color = colorIndex;
      if (mainDisplay != NULL) {
         UtilStrCpyN(attr_ptr->obj->color_str, sizeof(attr_ptr->obj->color_str),
               colorMenuItems[colorIndex]);
      }
      MoveObj(attr_ptr->obj, x-attr_ptr->obj->x, y-attr_ptr->obj->y);
      UpdTextBBox(attr_ptr->obj);
      AdjObjBBox(selected_obj);
      UpdSelBBox();
      RecordReplaceAnObj(selected_obj);
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
               rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
               selected_obj->bbox.ltx-GRID_ABS_SIZE(1),
               selected_obj->bbox.lty-GRID_ABS_SIZE(1),
               selected_obj->bbox.rbx+GRID_ABS_SIZE(1),
               selected_obj->bbox.rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   } else {
      ReplaceAttrFirstValue(selected_obj, attr_ptr, attr_value);
   }
   return TRUE;
}

int ExecDelAttrFromSelObj(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* delete_attr_from_selected_obj(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   int ltx=0, lty=0, rbx=0, rby=0;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (attr_owner_obj == tgifObj) {
      AddObj(NULL, topObj, tgifObj);
      AddNewSelObj(topObj);

      PrepareToReplaceAnObj(attr_owner_obj);

      UnlinkAttr(attr_ptr);
      FreeTextObj(attr_ptr->obj);
      FreeAttr(attr_ptr);
      AdjObjBBox(attr_owner_obj);

      recordCmdIncludeTgifObj = TRUE;
      RecordReplaceAnObj(attr_owner_obj);
      recordCmdIncludeTgifObj = FALSE;

      UnlinkObj(topObj);
      FreeTopSel();
      SetFileModified(TRUE);

      return TRUE;
   }
   ltx = attr_owner_obj->bbox.ltx;
   lty = attr_owner_obj->bbox.lty;
   rbx = attr_owner_obj->bbox.rbx;
   rby = attr_owner_obj->bbox.rby;

   PrepareToReplaceAnObj(attr_owner_obj);

   UnlinkAttr(attr_ptr);
   FreeTextObj(attr_ptr->obj);
   FreeAttr(attr_ptr);
   AdjObjBBox(attr_owner_obj);
   if (outerSel != NULL) AdjAncestorsBBox();
   UpdSelBBox();

   RecordReplaceAnObj(attr_owner_obj);
   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecUserEndAnEdge(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* user_end_an_edge(attr_name,abs_x,abs_y); */
{
   char *attr_name=argv[0], *abs_x_str=argv[1], *abs_y_str=argv[2];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj;
   int abs_x, abs_y, poly_created=FALSE, already_in_hyperspace=inHyperSpace;
   XEvent ev;
   XButtonEvent *button_ev_ptr;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_y_str);
   if (strcmp(attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (topSel != NULL) {
      RemoveAllSel();
   }
   if (!IntExpression(abs_x_str, &abs_x, orig_cmd) ||
         !IntExpression(abs_y_str, &abs_y, orig_cmd)) {
      return FALSE;
   }
   ResetWiringNodeInfo();
   SetCurChoice(DRAWPOLY);
   SetStringStatus(TgLoadString(STID_END_AN_EDGE_AT_A_PORT));

   ev.type = ButtonPress;
   button_ev_ptr = &(ev.xbutton);
   button_ev_ptr->button = Button1;
   button_ev_ptr->x = OFFSET_X(abs_x);
   button_ev_ptr->y = OFFSET_Y(abs_y);

   polyDrawn = FALSE;
   gstWiringInfo.num_ports_to_connect = 1;
   DrawPoly(&ev);
   gstWiringInfo.num_ports_to_connect = 0;
   poly_created = polyDrawn;
   SetCurChoice(NOTHING);
   if (already_in_hyperspace && !inHyperSpace) ToggleHyperSpace(TRUE);

   if (attr_ptr != NULL) {
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr,
            (poly_created ? gstWiringInfo.last_port_name : ""));
   }
   return TRUE;
}

int ExecUserDrawAnEdge(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* user_draw_an_edge(start_attr_name,end_attr_name); */
{
   char *start_attr_name=argv[0], *end_attr_name=argv[1];
   struct AttrRec *start_attr_ptr=NULL, *end_attr_ptr=NULL;
   struct ObjRec *start_attr_owner_obj, *end_attr_owner_obj;
   int poly_created=FALSE, already_in_hyperspace=inHyperSpace;
   XGCValues values;

   UtilRemoveQuotes(start_attr_name);
   UtilRemoveQuotes(end_attr_name);
   if (strcmp(start_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", start_attr_name);
      start_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
            &start_attr_owner_obj);
      if (start_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (strcmp(end_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", end_attr_name);
      end_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
            &end_attr_owner_obj);
      if (end_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (topSel != NULL) {
      RemoveAllSel();
   }
   ResetWiringNodeInfo();
   connectingPortsByWire = TRUE;
   UseWireMenuItemInModeItem(TRUE);

   values.line_width = 3;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);

   SetCurChoice(DRAWPOLY);
   gstWiringInfo.num_ports_to_connect = 2;
   drawPolyHighlightedNode = NULL;
   SetStringStatus(TgLoadString(STID_DRAW_AN_EDGE_BETWEEN_PORTS));
   polyDrawn = FALSE;
   for (;;) {
      XEvent input;

      XNextEvent(mainDisplay, &input);
      if (input.type == Expose) {
         ExposeEventHandler(&input, TRUE);
      } else if (input.xany.window == drawWindow) {
         polyDrawn = FALSE;
         DrawingEventHandler(&input);
         if (curChoice == DRAWPOLY) {
            if (polyDrawn) {
               break;
            } else if (gstWiringInfo.num_ports_to_connect == (-1)) {
               break;
            }
         } else {
            polyDrawn = FALSE;
            break;
         }
      }
   }
   gstWiringInfo.num_ports_to_connect = 0;
   poly_created = polyDrawn;
   values.line_width = 1;
   XChangeGC(mainDisplay, revGrayGC, GCLineWidth, &values);
   connectingPortsByWire = FALSE;
   SetCurChoice(NOTHING);
   RedrawModeWindow();
   UseWireMenuItemInModeItem(FALSE);
   UpdatePinnedMenu(MENU_MODE);

   if (already_in_hyperspace && !inHyperSpace) ToggleHyperSpace(TRUE);

   if (start_attr_ptr != NULL) {
      ReplaceAttrFirstValue(start_attr_owner_obj, start_attr_ptr,
            (poly_created ? gstWiringInfo.first_port_name : ""));
   }
   if (end_attr_ptr != NULL) {
      ReplaceAttrFirstValue(end_attr_owner_obj, end_attr_ptr,
            (poly_created ? gstWiringInfo.last_port_name : ""));
   }
   return TRUE;
}

int ExecGetAPolyVertexAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_a_poly_vertex_absolute(x_attr,y_attr,obj_name,v_index); */
{
   char *x_attr_name=argv[0], *y_attr_name=argv[1], *obj_name=argv[2];
   char *v_index_str=argv[3], buf[40];
   int v_index=0, abs_x=0, abs_y=0;
   struct AttrRec *x_attr_ptr, *y_attr_ptr;
   struct ObjRec *x_attr_owner_obj=NULL, *y_attr_owner_obj=NULL;
   struct ObjRec *owner_obj=NULL, *named_obj;

   UtilRemoveQuotes(x_attr_name);
   UtilRemoveQuotes(y_attr_name);
   UtilRemoveQuotes(obj_name);
   UtilRemoveQuotes(v_index_str);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, NULL);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (!IntExpression(v_index_str, &v_index, orig_cmd)) return FALSE;

   switch (named_obj->type) {
   case OBJ_POLY:
      if (v_index < 0 || v_index >= named_obj->detail.p->n) {
         sprintf(execDummyStr, TgLoadString(STID_CANT_FIND_VRTX_WHILE_EXEC_CMD),
               v_index, obj_name, orig_cmd);
         MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      abs_x = (int)named_obj->detail.p->vlist[v_index].x;
      abs_y = (int)named_obj->detail.p->vlist[v_index].y;
      break;
   case OBJ_POLYGON:
      if (v_index < 0 || v_index >= named_obj->detail.g->n) {
         sprintf(execDummyStr, TgLoadString(STID_CANT_FIND_VRTX_WHILE_EXEC_CMD),
               v_index, obj_name, orig_cmd);
         MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      abs_x = (int)named_obj->detail.g->vlist[v_index].x;
      abs_y = (int)named_obj->detail.g->vlist[v_index].y;
      break;
   default:
      sprintf(execDummyStr, TgLoadString(STID_POLY_NOR_PGON_WHILE_EXEC_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   sprintf(execDummyStr, "%s=", x_attr_name);
   x_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &x_attr_owner_obj);
   if (x_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   sprintf(execDummyStr, "%s=", y_attr_name);
   y_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &y_attr_owner_obj);
   if (y_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", abs_x);
   ReplaceAttrFirstValue(x_attr_owner_obj, x_attr_ptr, buf);
   sprintf(buf, "%1d", abs_y);
   ReplaceAttrFirstValue(y_attr_owner_obj, y_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecMoveAPolyVertexAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_a_poly_vertex_absolute(obj_name,v_index,abs_x,abs_y); */
{
   char *obj_name=argv[0], *v_index_str=argv[1];
   char *abs_x_str=argv[2], *abs_y_str=argv[3];
   int v_index=0, abs_x=0, abs_y=0, cur_x=0, cur_y=0, ltx, lty, rbx, rby;
   int auto_center_attr;
   struct ObjRec *owner_obj=NULL, *top_owner=NULL, *named_obj;
   struct SelRec *saved_top_sel=topSel, *saved_bot_sel=botSel;

   UtilRemoveQuotes(obj_name);
   UtilRemoveQuotes(v_index_str);
   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_y_str);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (!IntExpression(v_index_str, &v_index, orig_cmd)) return FALSE;
   if (!IntExpression(abs_x_str, &abs_x, orig_cmd)) return FALSE;
   if (!IntExpression(abs_y_str, &abs_y, orig_cmd)) return FALSE;

   switch (named_obj->type) {
   case OBJ_POLY:
      if (v_index < 0 || v_index >= named_obj->detail.p->n) {
         sprintf(execDummyStr, TgLoadString(STID_CANT_FIND_VRTX_WHILE_EXEC_CMD),
               v_index, obj_name, orig_cmd);
         MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      cur_x = named_obj->detail.p->vlist[v_index].x;
      cur_y = named_obj->detail.p->vlist[v_index].y;
      break;
   case OBJ_POLYGON:
      if (v_index < 0 || v_index >= named_obj->detail.g->n) {
         sprintf(execDummyStr, TgLoadString(STID_CANT_FIND_VRTX_WHILE_EXEC_CMD),
               v_index, obj_name, orig_cmd);
         MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      cur_x = named_obj->detail.g->vlist[v_index].x;
      cur_y = named_obj->detail.g->vlist[v_index].y;
      break;
   default:
      sprintf(execDummyStr, TgLoadString(STID_POLY_NOR_PGON_WHILE_EXEC_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (cur_x == abs_x && cur_y == abs_y) return TRUE;
   if (owner_obj == NULL) {
      ltx = named_obj->bbox.ltx; lty = named_obj->bbox.lty;
      rbx = named_obj->bbox.rbx; rby = named_obj->bbox.rby;
   } else {
      ltx = top_owner->bbox.ltx; lty = top_owner->bbox.lty;
      rbx = top_owner->bbox.rbx; rby = top_owner->bbox.rby;
   }
   SelectAnObj(named_obj, owner_obj, top_owner, &saved_top_sel, &saved_bot_sel);

   PrepareToRecord(CMD_REPLACE, topSel, botSel, numObjSelected);
   switch (named_obj->type) {
   case OBJ_POLY:
      named_obj->detail.p->vlist[v_index].x = abs_x;
      named_obj->detail.p->vlist[v_index].y = abs_y;
      AdjObjSplineVs(named_obj);
      if (named_obj->detail.p->curved != LT_INTSPLINE) {
         UpdPolyBBox(named_obj, named_obj->detail.p->n,
               named_obj->detail.p->vlist);
      } else {
         UpdPolyBBox(named_obj, named_obj->detail.p->intn,
               named_obj->detail.p->intvlist);
      }
      break;
   case OBJ_POLYGON:
      named_obj->detail.g->vlist[v_index].x = abs_x;
      named_obj->detail.g->vlist[v_index].y = abs_y;
      if (v_index == 0) {
         named_obj->detail.g->vlist[named_obj->detail.g->n-1].x = abs_x;
         named_obj->detail.g->vlist[named_obj->detail.g->n-1].y = abs_y;
      } else if (v_index == named_obj->detail.g->n-1) {
         named_obj->detail.g->vlist[0].x = abs_x;
         named_obj->detail.g->vlist[0].y = abs_y;
      }
      AdjObjSplineVs(named_obj);
      if (named_obj->detail.g->curved != LT_INTSPLINE) {
         UpdPolyBBox(named_obj, named_obj->detail.g->n,
               named_obj->detail.g->vlist);
      } else {
         UpdPolyBBox(named_obj, named_obj->detail.g->intn,
               named_obj->detail.g->intvlist);
      }
      break;
   }
   auto_center_attr = AutoCenterAttr(named_obj);
   if (auto_center_attr) {
      struct AttrRec *attr_ptr;
      int modified=FALSE;

      for (attr_ptr=named_obj->fattr; attr_ptr != NULL;
            attr_ptr=attr_ptr->next) {
         if (attr_ptr->shown) {
            struct BBRec bbox;

            CenterObjInOBBox(attr_ptr->obj, named_obj->obbox, &bbox);
            if (bbox.ltx < ltx) ltx = bbox.ltx;
            if (bbox.lty < lty) lty = bbox.lty;
            if (bbox.rbx > rbx) rbx = bbox.rbx;
            if (bbox.rby > rby) rby = bbox.rby;
            modified = TRUE;
         }
      }
      if (modified) AdjObjBBox(named_obj);
   }
   RecursivelyAdjObjBBox(named_obj, owner_obj, top_owner);
   RecordCmd(CMD_REPLACE, NULL, topSel, botSel, numObjSelected);

   UpdSelBBox();
   if (owner_obj == NULL) {
      if (named_obj->bbox.ltx < ltx) ltx = named_obj->bbox.ltx;
      if (named_obj->bbox.lty < lty) lty = named_obj->bbox.lty;
      if (named_obj->bbox.rbx > rbx) rbx = named_obj->bbox.rbx;
      if (named_obj->bbox.rby > rby) rby = named_obj->bbox.rby;
   } else {
      if (top_owner->bbox.ltx < ltx) ltx = top_owner->bbox.ltx;
      if (top_owner->bbox.lty < lty) lty = top_owner->bbox.lty;
      if (top_owner->bbox.rbx > rbx) rbx = top_owner->bbox.rbx;
      if (top_owner->bbox.rby > rby) rby = top_owner->bbox.rby;
   }
   free(topSel);
   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();

   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);
   return TRUE;
}

int ExecPostAttrAndGetCGI(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* post_attr_and_get_cgi_result(url_attr,query_attr,result_attr); */
{
   char *url_attr_name=argv[0], *query_attr_name=argv[1];
   char *result_attr_name=argv[2];
   struct AttrRec *url_attr, *query_attr, *result_attr;
   struct ObjRec *result_attr_owner_obj=NULL;
   char *remote_buf=NULL, *tmp_remote_fname=NULL;
   int is_html=FALSE, remote_buf_sz=0, rc;
   FILE *fp;

   UtilRemoveQuotes(url_attr_name);
   UtilRemoveQuotes(query_attr_name);
   UtilRemoveQuotes(result_attr_name);
   sprintf(execDummyStr, "%s=", url_attr_name);
   url_attr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (url_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", query_attr_name);
   query_attr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (query_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!FileIsRemote(url_attr->attr_value.s)) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_REM_FNAME_WHILE_EXEC_CMD),
            url_attr->attr_value.s, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   fnameForPostingCGIQuery = (char*)malloc((strlen(tmpDir)+20)*sizeof(char));
   if (fnameForPostingCGIQuery == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   if (MkTempFile(fnameForPostingCGIQuery, (strlen(tmpDir)+19)*sizeof(char),
         tmpDir, TOOL_NAME) == NULL) {
      free(fnameForPostingCGIQuery);
      fnameForPostingCGIQuery = NULL;
      return FALSE;
   }
   if ((fp=fopen(fnameForPostingCGIQuery, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_FILE_WRITE_EXEC_CMD),
            fnameForPostingCGIQuery, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

      free(fnameForPostingCGIQuery);
      fnameForPostingCGIQuery = NULL;
      return FALSE;
   }
   writeFileFailed = FALSE;
   fprintf(fp, "%s\r\n", query_attr->attr_value.s);
   if (query_attr->obj->detail.t->minilines.first != NULL) {
      MiniLineInfo *pMiniLine=NULL;

      for (pMiniLine=query_attr->obj->detail.t->minilines.first->next;
            pMiniLine != NULL; pMiniLine=pMiniLine->next) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=ConvertMiniLineToString(pMiniLine,
               &need_to_free_tmp_buf);

         if (fprintf(fp, "%s\r\n", tmp_buf) == EOF) {
            writeFileFailed = TRUE;
         }
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         if (writeFileFailed) break;
      }
   }
   fclose(fp);
   if (writeFileFailed) {
      FailToWriteFileMessage(fnameForPostingCGIQuery);
      unlink(fnameForPostingCGIQuery);
      free(fnameForPostingCGIQuery);
      fnameForPostingCGIQuery = NULL;
      return FALSE;
   }
   postingCGIQuery = TRUE;
   SaveStatusStrings();
   rc = LoadRemoteFileInMem(url_attr->attr_value.s, &remote_buf, NULL,
         &remote_buf_sz, &is_html, TRUE, NULL, 0);
   RestoreStatusStrings();
   postingCGIQuery = FALSE;
   unlink(fnameForPostingCGIQuery);
   free(fnameForPostingCGIQuery);
   fnameForPostingCGIQuery = NULL;

   if (!rc) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXP_ERR_EXEC_CMD_ABORT), orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   tmp_remote_fname = WriteRemoteFileIntoTemp(remote_buf, remote_buf_sz, NULL);
   if (tmp_remote_fname == NULL) {
      return FALSE;
   }
   SaveStatusStrings();
   rc = DoReadFileIntoAttr(result_attr, result_attr_owner_obj,
         tmp_remote_fname, orig_cmd);
   RestoreStatusStrings();

   if (remote_buf != NULL) FreeRemoteBuf(remote_buf);
   if (tmp_remote_fname != NULL) {
      unlink(tmp_remote_fname);
      FreeRemoteBuf(tmp_remote_fname);
   }
   return rc;
}

void ExecNavigateBack(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* navigate_back(); */
{
   execNavigateBack = TRUE;
}

void ExecStop(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* stop(); */
{
   gnAbortExec = TRUE;
   gnStopCmdExecuted = TRUE;
   sprintf(gszMsgBox, TgLoadString(STID_GIVEN_CMD_EXECUTED), orig_cmd);
   Msg(gszMsgBox);
}

int ExecSqrt(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* sqrt(attr_name,expr); */
{
   char *attr_name=argv[0], *expr=argv[1], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   struct VRec v;
   double d_val=(double)0.0;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(expr);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!EvalExpr(expr, &v)) return FALSE;

   switch (v.vtype) {
   case INT_VAL: d_val = (double)v.val.i; break;
   case DBL_VAL: d_val = (double)v.val.d; break;
   case NULL_VAL:
   case STR_VAL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_NUM_EXP_EXEC_CMD),
            expr, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (v.vtype == STR_VAL && v.val.s != NULL) free(v.val.s);
      return FALSE;
   }
   if (d_val < (double)0.0) {
      sprintf(gszMsgBox, TgLoadString(STID_SQRT_NEG_NUM_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   d_val = (double)sqrt((double)d_val);
   sprintf(buf, "%.12f", (float)(d_val));
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecRandom(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* random(attr_name); */
{
   static int snSeeded=FALSE;
   char *attr_name=argv[0], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!snSeeded) {
      snSeeded = TRUE;
      srand(0);
   }
   sprintf(buf, "%1d", rand());
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

#ifndef _NO_RAND48
static int gnDRand48Seeded=FALSE;
#endif /* ~_NO_RAND48 */

#ifdef _NO_RAND48
static
int NoRand48(orig_cmd)
   char *orig_cmd;
{
   sprintf(gszMsgBox, TgLoadString(STID_CMD_NOT_AVAIL_ON_PLATFORM), orig_cmd);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}
#endif /* _NO_RAND48 */

int ExecSRand48(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* srand48(use_cur_time_as_seed); */
{
#ifndef _NO_RAND48
   char *use_cur_time_str=argv[0];
   int use_cur_time=0;

   UtilRemoveQuotes(use_cur_time_str);
   UtilTrimBlanks(use_cur_time_str);
   if (!IntExpression(use_cur_time_str, &use_cur_time, orig_cmd)) {
      return FALSE;
   }
   if (use_cur_time == 0) {
      srand48(0L);
   } else {
      time_t tloc;

      time(&tloc);
      srand48((long)tloc);
   }
   gnDRand48Seeded = TRUE;

   return TRUE;
#else /* _NO_RAND48 */
   return NoRand48(orig_cmd);
#endif /* ~_NO_RAND48 */
}

int ExecDRand48(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* drand48(attr_name); */
{
#ifndef _NO_RAND48
   char *attr_name=argv[0], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   double d_val=(double)0;

   UtilRemoveQuotes(attr_name);
   UtilTrimBlanks(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!gnDRand48Seeded) {
      gnDRand48Seeded = TRUE;
      srand48(0L);
   }
   d_val = (double)drand48();
   sprintf(buf, "%.12f", (float)(d_val));
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);

   return TRUE;
#else /* _NO_RAND48 */
   return NoRand48(orig_cmd);
#endif /* ~_NO_RAND48 */
}

int ExecRound(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* round(attr_name,expr); */
{
   char *attr_name=argv[0], *expr=argv[1], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   struct VRec v;
   double d_val=(double)0.0;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(expr);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!EvalExpr(expr, &v)) return FALSE;

   switch (v.vtype) {
   case INT_VAL: d_val = (double)v.val.i; break;
   case DBL_VAL: d_val = (double)v.val.d; break;
   case NULL_VAL:
   case STR_VAL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_NUM_EXP_EXEC_CMD),
            expr, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (v.vtype == STR_VAL && v.val.s != NULL) free(v.val.s);
      return FALSE;
   }
   sprintf(buf, "%1d", round(d_val));
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecRedrawObj(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* redraw_obj(obj_name); */
{
   char *obj_name=argv[0];
   struct ObjRec *owner_obj=NULL, *top_owner=NULL, *named_obj;
   int ltx, lty, rbx, rby;

   UtilRemoveQuotes(obj_name);
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, &top_owner);
   if (named_obj == NULL) return BadObjName(obj_name, orig_cmd);

   ltx = named_obj->bbox.ltx; lty = named_obj->bbox.lty;
   rbx = named_obj->bbox.rbx; rby = named_obj->bbox.rby;
   RedrawAnArea(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1));

   return TRUE;
}

void ExecRedrawDrawingArea(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* redraw_drawing_area(); */
{
   RedrawDrawWindow(botObj);
}

int ExecIntToHex(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* itox(attr_name,digits,expr); */
{
   char *attr_name=argv[0], *digits_str=argv[1], *expr=argv[2];
   char buf[40], buf1[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   struct VRec v;
   int digits=0, i_val=0;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(digits_str);
   UtilRemoveQuotes(expr);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!IntExpression(digits_str, &digits, orig_cmd)) {
      return FALSE;
   }
   if (digits <= 0 || digits > 8) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
            orig_cmd, "digits", 1, 8, digits);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }

   if (!EvalExpr(expr, &v)) return FALSE;

   switch (v.vtype) {
   case INT_VAL: i_val = v.val.i; break;
   case DBL_VAL: i_val = round(v.val.d); break;
   case NULL_VAL:
   case STR_VAL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_NUM_EXP_EXEC_CMD),
            expr, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (v.vtype == STR_VAL && v.val.s != NULL) free(v.val.s);
      return FALSE;
   }
   sprintf(buf1, "%%0%1dx", digits);
   sprintf(buf, buf1, i_val);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecForI(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* for_i(attr_name,min_val,max_val,inc,attr_to_exec); */
{
   char *attr_name=argv[0], *min_str=argv[1], *max_str=argv[2];
   char *inc_str=argv[3], *exec_attr_name=argv[4], num_buf[40];
   char *compare_buf=NULL, *assign_buf=NULL;
   struct AttrRec *attr_ptr=NULL, *exec_attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL, *exec_attr_owner_obj=NULL;
   int min_val=0, max_val=0, inc_val=1, ok=TRUE;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(min_str);
   UtilRemoveQuotes(max_str);
   UtilRemoveQuotes(inc_str);
   UtilRemoveQuotes(exec_attr_name);
   if (strcmp(attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   sprintf(execDummyStr, "%s=", exec_attr_name);
   exec_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &exec_attr_owner_obj);
   if (exec_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!IntExpression(min_str, &min_val, orig_cmd) ||
         !IntExpression(max_str, &max_val, orig_cmd) ||
         !IntExpression(inc_str, &inc_val, orig_cmd)) {
      return FALSE;
   }
   if (attr_ptr != NULL) {
      compare_buf = (char*)malloc((strlen(attr_name)+40)*sizeof(char));
      assign_buf = (char*)malloc((strlen(attr_name)+40)*sizeof(char));
      if (compare_buf == NULL || assign_buf == NULL) {
         FailAllocMessage();
         if (compare_buf != NULL) free(compare_buf);
         if (assign_buf != NULL) free(assign_buf);
         return FALSE;
      }
      sprintf(compare_buf, "$(%s) <= %1d", attr_name, max_val);
      sprintf(assign_buf, "$(%s)", attr_name);

      sprintf(num_buf, "%1d", min_val);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, num_buf);
   }
   while (ok) {
      if (attr_ptr != NULL) {
         int val;
         char *expr=convert_str(compare_buf, obj_ptr, TRUE);

         if (expr == NULL) {
            BadAttr(compare_buf, orig_cmd);
            free(compare_buf);
            return FALSE;
         } else if (!IntExpression(expr, &val, orig_cmd)) {
            free(expr);
            free(compare_buf);
            return FALSE;
         }
         free(expr);
         if (val == 0) break;
      } else {
         if (min_val > max_val) break;
      }
      if (!DoExec(exec_attr_ptr, exec_attr_owner_obj)) return FALSE;
      if (attr_ptr != NULL) {
         int val;
         char *expr=convert_str(assign_buf, obj_ptr, TRUE);

         if (expr == NULL) {
            BadAttr(assign_buf, orig_cmd);
            free(assign_buf);
            return FALSE;
         } else if (!IntExpression(expr, &val, orig_cmd)) {
            free(expr);
            free(assign_buf);
            return FALSE;
         }
         free(expr);
         min_val = val + inc_val;

         sprintf(num_buf, "%1d", min_val);
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, num_buf);
      } else {
         min_val += inc_val;
      }
      ok = (!CheckExecInterrupt(orig_cmd));
      if (!ok) {
         userAbortExec = TRUE;
      }
   }
   if (compare_buf != NULL) free(compare_buf);
   if (assign_buf != NULL) free(assign_buf);
   return ok;
}

void ExecSetFileNotModified(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* set_file_not_modified(); */
{
   justDupped = FALSE;
   SetFileModified(FALSE);
   RedrawTitleWindow();
}

int ExecNewId(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* new_id(attr_name); */
{
   char *attr_name=argv[0], buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(buf, "%1d", objId++);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecRotateSelObj(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* rotate_selected_obj(angle); */
{
   char *expr=argv[0];
   struct VRec v;
   double d_val=(double)0.0;

   UtilRemoveQuotes(expr);
   if (topSel == NULL) return BadSelectedObj(orig_cmd);

   if (!EvalExpr(expr, &v)) return FALSE;

   switch (v.vtype) {
   case INT_VAL: d_val = (double)v.val.i; break;
   case DBL_VAL: d_val = (double)v.val.d; break;
   case NULL_VAL:
   case STR_VAL:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EVAL_NUM_EXP_EXEC_CMD),
            expr, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      if (v.vtype == STR_VAL && v.val.s != NULL) free(v.val.s);
      return FALSE;
   }
   RotateAllSelObj(d_val);
   return TRUE;
}

int ExecCallSimpleShortCut(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* call_simple_shortcut(shortcut_name); */
{
   char *shortcut_name=argv[0], *func_name=NULL, *buf=NULL, code='\0';
   unsigned int state=0;
   int len, rc=TRUE;

   UtilRemoveQuotes(shortcut_name);

   if (!ValidShortCut(shortcut_name, 0, &code, &state)) {
      int found=FALSE;

      if (cmdLineTgrm2) {
         int actual_num_args=0;
         int index=ValidTangram2CmdName(shortcut_name, strlen(shortcut_name),
               &actual_num_args);

         if (index != 0 && actual_num_args == 0) {
            found = TRUE;
            DoTangram2ShortCut(index-CMDID_TANGRAM2_BASE, "");
         }
      }
      if (!found) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_SHORTCUT_WHILE_EXEC_CMD),
               shortcut_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return found;
   }
   len = strlen(shortcut_name);
   if ((func_name=(char*)malloc((len+3)*sizeof(char))) == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(func_name, "%s()", shortcut_name);
   if ((buf=UtilStrDup(func_name)) == NULL) {
      free(func_name);
      FailAllocMessage();
      return FALSE;
   }
   rc = CallShortCut(func_name, 1, &buf, &code, state);
   free(buf);
   free(func_name);
   if (!rc) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_SHORTCUT_WHILE_EXEC_CMD),
            shortcut_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      /* do not translate -- program constants */
      if (UtilStrICmp(shortcut_name, "Quit") == 0) {
         gnAbortExec = TRUE;
      }
   }
   return rc;
}

int ExecCallOneArgShortCut(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* call_one_arg_shortcut(shortcut_name,arg); */
{
   char *shortcut_name=argv[0], *arg=argv[1], **ppsz_buf=NULL, code='\0';
   char *func_name=NULL;
   unsigned int state;
   int len=0;

   UtilRemoveQuotes(shortcut_name);
   UtilRemoveQuotes(arg);

   /* do not translate -- program constants */
   if (strcmp(shortcut_name, "Quit") == 0 ||
         !ValidShortCut(shortcut_name, 1, &code, &state)) {
      int found=FALSE;

      if (cmdLineTgrm2) {
         int actual_num_args=0;
         int index=ValidTangram2CmdName(shortcut_name, strlen(shortcut_name),
               &actual_num_args);

         if (index != 0 && actual_num_args == 1) {
            found = TRUE;
            DoTangram2ShortCut(index-CMDID_TANGRAM2_BASE, arg);
         }
      }
      if (!found) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_SHORTCUT_WHILE_EXEC_CMD),
               shortcut_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return found;
   }
   len = strlen(shortcut_name);
   if ((func_name=(char*)malloc((len+3)*sizeof(char))) == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(func_name, "%s()", shortcut_name);
   if ((ppsz_buf=(char**)malloc(2*sizeof(char*))) == NULL) {
      free(func_name);
      FailAllocMessage();
      return FALSE;
   }
   if ((ppsz_buf[0]=UtilStrDup(func_name)) == NULL) {
      free(ppsz_buf);
      free(func_name);
      FailAllocMessage();
      return FALSE;
   }
   len = strlen(arg);
   if ((ppsz_buf[1]=(char*)malloc((len+2)*sizeof(char))) == NULL) {
      free(ppsz_buf[0]);
      free(ppsz_buf);
      free(func_name);
      FailAllocMessage();
      return FALSE;
   }
   sprintf(ppsz_buf[1], "%s)", arg);
   CallShortCut(func_name, 2, ppsz_buf, &code, state);
   free(ppsz_buf[1]);
   free(ppsz_buf[0]);
   free(ppsz_buf);
   free(func_name);
   return TRUE;
}

int ExecSubstituteAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* substitute_attr(attr_name,src_attr_name,replace_attr_name,pattern_str); */
{
   char *attr_name=argv[0], *src_attr_name=argv[1];
   char *replace_attr_name=argv[2], *pattern_str=argv[3];
   char tmp_fname[MAXPATHLENGTH+1];
   struct AttrRec *attr_ptr=NULL, *src_attr_ptr=NULL, *replace_attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   MiniLineInfo *pMiniLine=NULL;
   int rc, pattern_len=0;
   FILE *fp;

   if (*pattern_str == '\0') return BadArg("pattern_str", orig_cmd);

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(src_attr_name);
   UtilRemoveQuotes(replace_attr_name);
   UtilRemoveQuotes(pattern_str);
   pattern_len = strlen(pattern_str);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", src_attr_name);
   src_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (src_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", replace_attr_name);
   replace_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
   if (replace_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (MkTempFile(tmp_fname, sizeof(tmp_fname), tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   if ((fp=fopen(tmp_fname, "w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_FILE_WRITE_EXEC_CMD),
            tmp_fname, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;

   for (pMiniLine=src_attr_ptr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      char *c_ptr, *buf=NULL;
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;

      if (pMiniLine == src_attr_ptr->obj->detail.t->minilines.first) {
         char *attr_value=NULL;

         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         attr_value = UtilStrDup(tmp_buf);
         if (attr_value == NULL) FailAllocMessage();
         ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         need_to_free_tmp_buf = TRUE;
         buf = tmp_buf = attr_value;
      } else {
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         buf = tmp_buf;
      }
      for (c_ptr=strstr(buf, pattern_str); c_ptr != NULL;
            c_ptr=strstr(buf, pattern_str)) {
         char saved_ch=(*c_ptr);
         MiniLineInfo *pReplaceMiniLine=NULL;

         *c_ptr = '\0';
         if (fprintf(fp, "%s", buf) == EOF) {
            writeFileFailed = TRUE;
         }
         for (pReplaceMiniLine=replace_attr_ptr->obj->detail.t->minilines.first;
               pReplaceMiniLine != NULL;
               pReplaceMiniLine=pReplaceMiniLine->next) {
            char *buf1=NULL;
            int need_to_free_tmp_buf1=FALSE;
            char *tmp_buf1=ConvertMiniLineToString(pMiniLine,
                  &need_to_free_tmp_buf1);

            if (pReplaceMiniLine ==
                  replace_attr_ptr->obj->detail.t->minilines.first) {
               char *attr_value=NULL;

               tmp_buf1 = ConvertMiniLineToString(pReplaceMiniLine,
                     &need_to_free_tmp_buf1);
               attr_value = UtilStrDup(tmp_buf1);
               if (attr_value == NULL) FailAllocMessage();
               ParseAttrStr(tmp_buf1, NULL, 0, attr_value,
                     strlen(attr_value)+1);
               if (need_to_free_tmp_buf1) UtilFree(tmp_buf1);
               need_to_free_tmp_buf1 = TRUE;
               buf1 = tmp_buf1 = attr_value;
            } else {
               tmp_buf1 = ConvertMiniLineToString(pReplaceMiniLine,
                     &need_to_free_tmp_buf1);
               buf1 = tmp_buf1;
            }
            if (fprintf(fp, "%s%s", buf1,
                  pReplaceMiniLine->next==NULL ? "" : "\n") == EOF) {
               writeFileFailed = TRUE;
            }
            if (need_to_free_tmp_buf1) UtilFree(tmp_buf1);
            if (writeFileFailed) break;
         }
         *c_ptr = saved_ch;
         buf = (&c_ptr[pattern_len]);
      }
      if (buf != NULL) {
         if (fprintf(fp, "%s\n", buf) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      if (writeFileFailed) break;
   }
   fclose(fp);
   if (writeFileFailed) {
      FailToWriteFileMessage(tmp_fname);
      unlink(tmp_fname);
      return FALSE;
   }
   SaveStatusStrings();
   rc = DoReadFileIntoAttr(attr_ptr, attr_owner_obj, tmp_fname, orig_cmd);
   RestoreStatusStrings();
   unlink(tmp_fname);
   return rc;
}

int ExecGetFileSize(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_file_size(attr_name,file_name); */
{
   char *attr_name=argv[0], *file_name=argv[1], buf[40];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   struct stat stat_buf;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(file_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (stat(file_name, &stat_buf) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_STAT_FAILED_WHILE_EXEC_CMD),
            file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   sprintf(buf, "%ld", (long)stat_buf.st_size);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecIsFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* is_file(attr_name,file_name); */
{
   char *attr_name=argv[0], *file_name=argv[1], buf[40];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   FILE *fp;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(file_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if ((fp=fopen(file_name, "r")) != NULL) {
      fclose(fp);
      strcpy(buf, "1");
   } else {
      strcpy(buf, "0");
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecIndex(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* index(attr_name,str,substr); */
{
   char *attr_name=argv[0], *the_str=argv[1], *sub_str=argv[2], *c_ptr, buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(sub_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   c_ptr = strstr(the_str, sub_str);
   if (c_ptr == NULL) {
      strcpy(buf, "-1");
   } else {
      sprintf(buf, "%1d", (int)(c_ptr-the_str));
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecRIndex(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* rindex(attr_name,str,substr); */
{
   char *attr_name=argv[0], *the_str=argv[1], *sub_str=argv[2];
   char *c_ptr, *last_match=NULL, buf[40];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(sub_str);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   for (c_ptr=strstr(the_str, sub_str); c_ptr != NULL;
         c_ptr=strstr(&last_match[1], sub_str)) {
      last_match = c_ptr;
   }
   if (last_match == NULL) {
      strcpy(buf, "-1");
   } else {
      sprintf(buf, "%1d", (int)(last_match-the_str));
   }
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   return TRUE;
}

int ExecGetNumberOfLines(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_number_of_lines_in_attr(result_attr,attr_name); */
{
   char *result_attr_name=argv[0], *attr_name=argv[1], buf[40];
   int count;
   MiniLineInfo *pMiniLine=NULL;
   struct AttrRec *result_attr_ptr, *attr_ptr;
   struct ObjRec *result_attr_owner_obj=NULL, *attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   count = 0;
   for (pMiniLine=attr_ptr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      count++;
   }
   sprintf(buf, "%1d", count);
   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecGetLineInAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_line_in_attr(result_attr,attr_name,line_number); */
{
   char *result_attr_name=argv[0], *attr_name=argv[1];
   char *line_number_str=argv[2], *psz=NULL;
   int count, line_number=(-1);
   MiniLineInfo *pMiniLine=NULL;
   struct AttrRec *result_attr_ptr, *attr_ptr;
   struct ObjRec *result_attr_owner_obj=NULL, *attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(line_number_str);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!IntExpression(line_number_str, &line_number, orig_cmd)) {
      return FALSE;
   }
   count = 0;
   for (pMiniLine=attr_ptr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      if (count == line_number) {
         int need_to_free_tmp_buf=FALSE;
         char *tmp_buf=NULL;

         if (pMiniLine == attr_ptr->obj->detail.t->minilines.first) {
            char *attr_value=NULL;

            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            attr_value = UtilStrDup(tmp_buf);
            if (attr_value == NULL) FailAllocMessage();
            ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
            psz = UtilStrDup(attr_value);
            UtilFree(attr_value);
         } else {
            tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
            psz = UtilStrDup(tmp_buf);
            if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         }
         if (psz == NULL) FailAllocMessage();
         break;
      }
      count++;
   }
   if (psz == NULL) psz = UtilStrDup("");
   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr_ptr, psz);
   UtilFree(psz);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecTrim(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* trim(attr_name); */
{
   char *attr_name=argv[0], *psz;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   psz = UtilStrDup(attr_ptr->attr_value.s);
   if (psz == NULL) FailAllocMessage();
   UtilTrimBlanks(psz);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, psz);
   UtilFree(psz);
   return TRUE;
}

int ExecIsAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* is_attr(result_attr,attr_name); */
{
   char *result_attr_name=argv[0], *attr_name=argv[1], buf[40];
   struct AttrRec *result_attr_ptr, *attr_ptr;
   struct ObjRec *result_attr_owner_obj=NULL, *attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   strcpy(buf, (attr_ptr==NULL ? "0" : "1"));

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecFindObjNames(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* find_obj_names(result_attr,obj_name,attr_name_value); */
{
   char *result_attr_name=argv[0], *obj_name=argv[1], *attr_str=argv[2];
   char *psz=NULL, *attr_name=NULL, *attr_value=NULL;
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL, *optr=NULL;
   struct ObjRec *parent_obj_ptr=NULL, *first_obj_ptr=NULL;
   int ok=TRUE;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(obj_name);
   UtilRemoveQuotes(attr_str);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (*obj_name == '\0') {
      parent_obj_ptr = NULL;
   } else if ((parent_obj_ptr=FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, NULL)) == NULL) {
      return BadObjName(obj_name, orig_cmd);
   } else {
      switch (parent_obj_ptr->type) {
      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         break;
      default:
         sprintf(gszMsgBox, TgLoadString(STID_NON_GROUPED_WHILE_EXEC_CMD),
               obj_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   if (strcmp(attr_str, "") == 0) {
      attr_name = attr_value = NULL;
   } else if ((psz=strchr(attr_str,'=')) != NULL) {
      if (*(++psz) != '\0') {
         if (strcmp(psz, "*") == 0) {
            attr_value = NULL;
         } else {
            attr_value = UtilStrDup(psz);
            if (attr_value == NULL) FailAllocMessage();
         }
         *psz = '\0';
      } else {
         attr_value = UtilStrDup("");
         if (attr_value == NULL) FailAllocMessage();
      }
      attr_name = attr_str;
   } else {
      attr_name = NULL;
      attr_value = UtilStrDup(attr_str);
      if (attr_value == NULL) FailAllocMessage();
   }
   CleanTmpStr();

   if (parent_obj_ptr == NULL) {
      first_obj_ptr = topObj;
   } else {
      first_obj_ptr = parent_obj_ptr->detail.r->first;
   }
   for (optr=first_obj_ptr; optr != NULL; optr=optr->next) {
      struct AttrRec *name_attr=FindAttrWithName(optr,"name=",NULL);

      if (name_attr != NULL) {
         if (attr_name == NULL && attr_value == NULL) {
            if (!AppendToTmpStr(name_attr->attr_value.s)) {
               ok = FALSE;
               break;
            }
         } else {
            struct AttrRec *aptr=NULL;

            for (aptr=optr->fattr; aptr != NULL; aptr=aptr->next) {
               if (attr_name == NULL) {
                  if (*aptr->attr_name.s == '\0' &&
                        strcmp(aptr->attr_value.s, attr_value) == 0) {
                     break;
                  }
               } else if (attr_value == NULL) {
                  if (strcmp(aptr->attr_name.s, attr_name) == 0) {
                     break;
                  }
               } else {
                  if (strcmp(aptr->attr_name.s, attr_name) == 0 &&
                        strcmp(aptr->attr_value.s, attr_value) == 0) {
                     break;
                  }
               }
            }
            if (aptr != NULL && !AppendToTmpStr(name_attr->attr_value.s)) {
               ok = FALSE;
               break;
            }
         }
      }
   }
   if (ok) {
      int count=0;
      char *count_buf=(char*)malloc((strlen(result_attr_name)+40)*sizeof(char));
      struct StrRec *str_ptr;

      if (count_buf == NULL) FailAllocMessage();
      for (str_ptr=topTmpStr; str_ptr != NULL; str_ptr=str_ptr->next) {
         count++;
      }
      sprintf(count_buf, "%s=%d", result_attr_name, count);
      if (PrependToTmpStr(count_buf)) {
         ReplaceAttrAllValues(result_attr_owner_obj, result_attr_ptr,
               &topTmpStr, &botTmpStr);
      }
      free(count_buf);
   }
   CleanTmpStr();
   if (attr_value != NULL) UtilFree(attr_value);
   return TRUE;
}

static
int FindObjNamesOnAllPgs(argv, obj_ptr, orig_cmd, tg2)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   int tg2;
   /*
    * find_obj_names_on_all_pages(result_attr,attr_name_value) or
    * tg2_find_obj_names_on_all_pages(result_attr,attr_name_value)
    */
{
   char *result_attr_name=argv[0], *attr_str=argv[1];
   char *psz=NULL, *attr_name=NULL, *attr_value=NULL;
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL;
   struct PageRec *page_ptr=NULL;
   int ok=TRUE, page_num=0;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_str);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (strcmp(attr_str, "") == 0) {
      attr_name = attr_value = NULL;
   } else if ((psz=strchr(attr_str,'=')) != NULL) {
      if (*(++psz) != '\0') {
         if (strcmp(psz, "*") == 0) {
            attr_value = NULL;
         } else {
            attr_value = UtilStrDup(psz);
            if (attr_value == NULL) FailAllocMessage();
         }
         *psz = '\0';
      } else {
         attr_value = UtilStrDup("");
         if (attr_value == NULL) FailAllocMessage();
      }
      attr_name = attr_str;
   } else {
      attr_name = NULL;
      attr_value = UtilStrDup(attr_str);
      if (attr_value == NULL) FailAllocMessage();
   }
   CleanTmpStr();

   for (page_ptr=firstPage, page_num=1; page_ptr != NULL;
         page_ptr=page_ptr->next, page_num++) {
      char prefix[80];
      struct ObjRec *optr=NULL;

      if (tg2) {
         sprintf(prefix, "_Page%1d", page_num);
      } else {
         sprintf(prefix, "##%1d!", page_num);
      }
      for (optr=page_ptr->top; optr != NULL; optr=optr->next) {
         struct AttrRec *name_attr=FindAttrWithName(optr,"name=",NULL);

         if (name_attr != NULL) {
            if (attr_name == NULL && attr_value == NULL) {
               if (firstPage == lastPage) {
                  if (!AppendToTmpStr(name_attr->attr_value.s)) {
                     ok = FALSE;
                     break;
                  }
               } else {
                  if (!AppendToTmpStr2(prefix, name_attr->attr_value.s, tg2)) {
                     ok = FALSE;
                     break;
                  }
               }
            } else {
               struct AttrRec *aptr=NULL;

               for (aptr=optr->fattr; aptr != NULL; aptr=aptr->next) {
                  if (attr_name == NULL) {
                     if (*aptr->attr_name.s == '\0' &&
                           strcmp(aptr->attr_value.s, attr_value) == 0) {
                        break;
                     }
                  } else if (attr_value == NULL) {
                     if (strcmp(aptr->attr_name.s, attr_name) == 0) {
                        break;
                     }
                  } else {
                     if (strcmp(aptr->attr_name.s, attr_name) == 0 &&
                           strcmp(aptr->attr_value.s, attr_value) == 0) {
                        break;
                     }
                  }
               }
               if (aptr != NULL) {
                  if (firstPage == lastPage) {
                     if (!AppendToTmpStr(name_attr->attr_value.s)) {
                        ok = FALSE;
                        break;
                     }
                  } else {
                     if (!AppendToTmpStr2(prefix, name_attr->attr_value.s,
                           tg2)) {
                        ok = FALSE;
                        break;
                     }
                  }
               }
            }
         }
      }
   }
   if (ok) {
      int count=0;
      char *count_buf=(char*)malloc((strlen(result_attr_name)+40)*sizeof(char));
      struct StrRec *str_ptr;

      if (count_buf == NULL) FailAllocMessage();
      for (str_ptr=topTmpStr; str_ptr != NULL; str_ptr=str_ptr->next) {
         count++;
      }
      sprintf(count_buf, "%s=%d", result_attr_name, count);
      if (PrependToTmpStr(count_buf)) {
         ReplaceAttrAllValues(result_attr_owner_obj, result_attr_ptr,
               &topTmpStr, &botTmpStr);
      }
      free(count_buf);
   }
   CleanTmpStr();
   if (attr_value != NULL) UtilFree(attr_value);
   return TRUE;
}

int ExecFindObjNamesOnAllPgs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* find_obj_names_on_all_pages(result_attr,attr_name_value); */
{
   char *result_attr_name=argv[0], *attr_str=argv[1];
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_str);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   return FindObjNamesOnAllPgs(argv, obj_ptr, orig_cmd, FALSE);
}

int ExecTg2FindObjNsOnAllPgs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* tg2_find_obj_names_on_all_pages(result_attr,attr_name_value); */
{
   char *result_attr_name=argv[0], *attr_str=argv[1];
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(attr_str);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   return FindObjNamesOnAllPgs(argv, obj_ptr, orig_cmd, TRUE);
}

int ExecTokenize(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* tokenize(result_attr,string,separator); */
{
   char *result_attr_name=argv[0], *str=argv[1], *separator=argv[2], *psz;
   int ok=TRUE, just_got_sep=FALSE;
   struct AttrRec *result_attr_ptr;
   struct ObjRec *result_attr_owner_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(str);
   UtilRemoveQuotes(separator);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (strlen(separator) != 1) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_SEP_LONG_WHILE_EXEC_CMD),
            separator, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (*separator == ' ' || *separator == '"' || *separator == '\'') {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_SEP_WHILE_EXEC_CMD),
            separator, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   CleanTmpStr();

   psz = str;
   while (*psz != '\0') {
      char sep=(*separator), quote='\0', *start=NULL;
      int done=FALSE, inc=TRUE;

      while (*psz == ' ') psz++;

      start = psz;
      if (*psz == '\0') {
         if (just_got_sep) {
            UtilRightTrim(start);
            ok = AppendToTmpStr(start);
            just_got_sep = FALSE;
         }
         break;
      }
      if (*psz == '"' || *psz == '\'') {
         just_got_sep = FALSE;
         quote = (*psz);
         for (psz++; *psz != '\0' && *psz != quote; psz++) ;
         if (*psz == quote) {
            start = (&start[1]);
            *psz = '\0';
            ok = AppendToTmpStr(start);
            *psz = quote;
            if (ok) {
               psz++;
               while (*psz == ' ') psz++;
               if (*psz == '\0') {
                  done = TRUE;
               } else if (*psz == sep) {
                  just_got_sep = TRUE;
               } else {
                  inc = FALSE;
               }
            }
         } else {
            done = TRUE;
            UtilRightTrim(start);
            ok = AppendToTmpStr(start);
         }
      } else if (*psz == sep) {
         *psz = '\0';
         UtilRightTrim(start);
         ok = AppendToTmpStr(start);
         *psz = sep;
         just_got_sep = TRUE;
         if (!ok) break;
      } else {
         just_got_sep = FALSE;
         for (psz++; *psz != '\0' && *psz != sep; psz++) ;
         if (*psz == '\0') {
            if (start != psz) {
               char ch=(*psz);

               *psz = '\0';
               UtilRightTrim(start);
               ok = AppendToTmpStr(start);
               *psz = ch;
            }
            done = TRUE;
         } else if (*psz == sep) {
            *psz = '\0';
            UtilRightTrim(start);
            ok = AppendToTmpStr(start);
            *psz = sep;
            just_got_sep = TRUE;
         }
      }
      if (!ok || done) break;
      if (inc) psz++;
   }
   if (ok && just_got_sep) {
      ok = AppendToTmpStr("");
   }
   if (ok) {
      int count=0;
      char *count_buf=(char*)malloc((strlen(result_attr_name)+40)*sizeof(char));
      struct StrRec *str_ptr;

      if (count_buf == NULL) FailAllocMessage();
      for (str_ptr=topTmpStr; str_ptr != NULL; str_ptr=str_ptr->next) {
         count++;
      }
      sprintf(count_buf, "%s%d", result_attr_ptr->attr_name.s, count);
      if (PrependToTmpStr(count_buf)) {
         ReplaceAttrAllValues(result_attr_owner_obj, result_attr_ptr,
               &topTmpStr, &botTmpStr);
      }
      free(count_buf);
   }
   CleanTmpStr();
   return TRUE;
}

int ExecMoveAttrRel(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_attr_relative(attr_name,dx,dy); */
{
   char *attr_name=argv[0];
   char *dx_str=argv[1], *dy_str=argv[2];
   int dx, dy;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   UtilRemoveQuotes(dx_str);
   UtilRemoveQuotes(dy_str);
   if (!IntExpression(dx_str, &dx, orig_cmd) ||
         !IntExpression(dy_str, &dy, orig_cmd)) {
      return FALSE;
   }
   if (dx != 0 || dy != 0) {
      MoveAnAttr(attr_ptr, attr_owner_obj, dx, dy);
      SetFileModified(TRUE);
   }
   return TRUE;
}

int ExecGetNumberOfVs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_number_of_vertices(result_attr,obj_name); */
{
   char *result_attr_name=argv[0], *obj_name=argv[1], buf[40];
   int count=0;
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL, *owner_obj=NULL, *named_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(obj_name);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, NULL);
   if (named_obj == NULL) return BadObjName(obj_name, orig_cmd);

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   switch (named_obj->type) {
   case OBJ_POLY: count = named_obj->detail.p->n; break;
   case OBJ_POLYGON: count = named_obj->detail.g->n-1; break;
   default:
      sprintf(execDummyStr, TgLoadString(STID_POLY_NOR_PGON_WHILE_EXEC_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   sprintf(buf, "%1d", count);
   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecIsObjTransformed(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* is_obj_transformed(result_attr,obj_name); */
{
   char *result_attr_name=argv[0], *obj_name=argv[1], buf[40];
   struct AttrRec *result_attr_ptr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL, *owner_obj=NULL, *named_obj=NULL;

   UtilRemoveQuotes(result_attr_name);
   UtilRemoveQuotes(obj_name);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, NULL);
   if (named_obj == NULL) return BadObjName(obj_name, orig_cmd);
   strcpy(buf, (named_obj->ctm==NULL ? "0" : "1"));

   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr_ptr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr_ptr, buf);

   SetFileModified(TRUE);
   return TRUE;
}

int ExecMakeSelObjIconic(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* make_selected_obj_iconic(sym_path); */
{
   char *sym_path=argv[0];

   UtilRemoveQuotes(sym_path);

   if (topSel == NULL) return BadSelectedObj(orig_cmd);
   if (topSel != botSel) {
      sprintf(execDummyStr, TgLoadString(STID_MORE_THAN_ONE_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (*sym_path != DIR_SEP) {
      sprintf(execDummyStr, TgLoadString(STID_MUST_FULL_PATH_WHILE_EXEC_CMD),
            sym_path, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!MakeIconic(sym_path, TRUE)) {
      return FALSE;
   }
   return TRUE;
}

int ExecMoveNamedObjRel(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_named_obj_relative(obj_name,dx,dy); */
{
   char *obj_name=argv[0], *dx_str=argv[1], *dy_str=argv[2];
   int dx=0, dy=0;
   struct ObjRec *top_owner=NULL, *named_obj=NULL;

   UtilRemoveQuotes(obj_name);
   UtilTrimBlanks(obj_name);
   UtilRemoveQuotes(dx_str);
   UtilRemoveQuotes(dy_str);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (!IntExpression(dx_str, &dx, orig_cmd) ||
         !IntExpression(dy_str, &dy, orig_cmd)) {
      return FALSE;
   }
   if (dx == 0 && dy == 0) {
      return TRUE;
   }
   /* MoveAnObj() can only be called from within an internal command */
   MoveAnObj(named_obj, top_owner, dx, dy);
   SetFileModified(TRUE);

   return TRUE;
}

int ExecMoveNamedObjAbs(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* move_named_obj_absolute(obj_name,ltx,lty); */
{
   char *obj_name=argv[0], *ltx_str=argv[1], *lty_str=argv[2];
   int ltx=0, lty=0;
   struct ObjRec *top_owner=NULL, *named_obj=NULL;

   UtilRemoveQuotes(obj_name);
   UtilTrimBlanks(obj_name);
   UtilRemoveQuotes(ltx_str);
   UtilRemoveQuotes(lty_str);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, NULL, &top_owner);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (!IntExpression(ltx_str, &ltx, orig_cmd) ||
         !IntExpression(lty_str, &lty, orig_cmd)) {
      return FALSE;
   }
   if (ltx == named_obj->obbox.ltx && lty == named_obj->obbox.lty) {
      return TRUE;
   }
   /* MoveAnObj() can only be called from within an internal command */
   MoveAnObj(named_obj, top_owner, ltx-named_obj->obbox.ltx,
         lty-named_obj->obbox.lty);
   SetFileModified(TRUE);

   return TRUE;
}

int ExecGetTgifVersion(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_tgif_version(major_attr,minor_attr,patchlevel_attr,build_attr); */
{
   char *major_attr_name=argv[0], *minor_attr_name=argv[1];
   char *patchlevel_attr_name=argv[2], *build_attr_name=argv[3];
   char buf[40], *psz=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(major_attr_name);
   UtilRemoveQuotes(minor_attr_name);
   UtilRemoveQuotes(patchlevel_attr_name);
   UtilRemoveQuotes(build_attr_name);
   if (strcmp(major_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", major_attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      strcpy(buf, versionString);
      if ((psz=strchr(buf, '.')) != NULL) {
         *psz = '\0';
      }
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   if (strcmp(minor_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", minor_attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      strcpy(buf, versionString);
      if ((psz=strchr(buf, '.')) != NULL) {
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, ++psz);
      } else {
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, "");
      }
   }
   if (strcmp(patchlevel_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", patchlevel_attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      sprintf(buf, "%1d", TGIF_PATCHLEVEL);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   if (strcmp(build_attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", build_attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, specialBuild);
   }
   return TRUE;
}

int ExecGetTgifDir(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_tgif_dir(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, tgifDir);
   return TRUE;
}

int ExecGetProfileString(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* get_profile_string(attr_name,section,key,def_value,ini_path); */
{
   char *attr_name=argv[0], *section=argv[1], *key=argv[2], *def_value=argv[3];
   char *ini_path=argv[4];
   char *values=NULL;
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(section);
   UtilRemoveQuotes(key);
   UtilRemoveQuotes(ini_path);
   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (*ini_path != DIR_SEP) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PATH_MUST_FULL),
            ini_path);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   values = tgGetProfileString((*section=='\0' ? NULL : section),
         (*key=='\0' ? NULL : key), ini_path);

   if (values != NULL) {
      if (*section == '\0' || *key == '\0') {
         int count=0, ok=TRUE;
         char *psz=NULL;
         char *count_buf=(char*)malloc((strlen(attr_name)+40)*sizeof(char));

         if (count_buf == NULL) FailAllocMessage();
         CleanTmpStr();
         for (psz=values; *psz != '\0'; psz++) {
            if (!AppendToTmpStr(psz)) {
               ok = FALSE;
               break;
            }
            psz += strlen(psz);
            count++;
         }
         if (ok) {
            sprintf(count_buf, "%s=%d", attr_name, count);
            if (PrependToTmpStr(count_buf)) {
               ReplaceAttrAllValues(attr_owner_obj, attr_ptr, &topTmpStr,
                     &botTmpStr);
            }
         }
         free(count_buf);
         CleanTmpStr();
      } else {
         ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, values);
      }
      tgFreeProfileString(values);
   } else {
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, def_value);
   }
   return TRUE;
}

int ExecWriteProfileString(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* write_profile_string(section,key,value,ini_path); */
{
   char *section=argv[0], *key=argv[1], *value=argv[2], *ini_path=argv[3];

   UtilRemoveQuotes(section);
   UtilRemoveQuotes(key);
   UtilRemoveQuotes(value);
   UtilRemoveQuotes(ini_path);

   if (inHyperSpace && !allowLaunchInHyperSpace) {
      XFlush(mainDisplay);
      XSync(mainDisplay, False);
      sprintf(gszMsgBox, TgLoadString(STID_WRITE_NOT_AUTO_IN_HYPER_EXEC),
            ini_path);
      if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
         MsgBox(TgLoadString(STID_USER_ABORT_WRITE_IN_HYPER), TOOL_NAME,
               INFO_MB);
         return FALSE;
      }
   }
   if (*ini_path != DIR_SEP) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PATH_MUST_FULL),
            ini_path);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (tgWriteProfileString((*section=='\0' ? NULL : section),
         (*key=='\0' ? NULL : key), value, ini_path)) {
      tgWriteProfileString(NULL, NULL, NULL, ini_path);
   }
   return TRUE;
}

int ExecSelectAdditionalObj(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* select_additional_obj(obj_name); */
{
   char *obj_name=argv[0];
   struct ObjRec *owner_obj=NULL, *named_obj=NULL;

   UtilRemoveQuotes(obj_name);
   UtilTrimBlanks(obj_name);

   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE,
         FALSE, &owner_obj, NULL);
   if (named_obj == NULL) {
      return BadObjName(obj_name, orig_cmd);
   }
   if (owner_obj != NULL) {
      sprintf(execDummyStr, TgLoadString(STID_BAD_NAME_TOP_LEVEL_ONLY_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (AlreadySelected(named_obj)) {
      sprintf(execDummyStr, TgLoadString(STID_OBJ_ALRDY_SEL_WHILE_EXEC_CMD),
            obj_name, orig_cmd);
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   AddNewSelObj(named_obj);
   UpdSelBBox();

   return TRUE;
}

int ExecOpenFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* open_file(file_number,file_name,file_mode); */
{
   char *file_number_str=argv[0], *file_name=argv[1], *file_mode=argv[2];
   int file_number=(-1);

   UtilRemoveQuotes(file_number_str);
   UtilRemoveQuotes(file_name);
   UtilRemoveQuotes(file_mode);
   UtilTrimBlanks(file_name);
   UtilTrimBlanks(file_mode);

   if (*file_name == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNAME_WHILE_EXEC_CMD),
            file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (*file_mode == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FMODE_WHILE_EXEC_CMD),
            file_mode, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (!IntExpression(file_number_str, &file_number, orig_cmd)) {
      return FALSE;
   }
   if (inHyperSpace && !allowLaunchInHyperSpace &&
         (*file_mode == 'w' || *file_mode == 'a')) {
      XFlush(mainDisplay);
      XSync(mainDisplay, False);
      sprintf(gszMsgBox, TgLoadString(STID_WRITE_NOT_AUTO_IN_HYPER_EXEC),
            file_name);
      if (MsgBox(gszMsgBox, TOOL_NAME, YN_MB) != MB_ID_YES) {
         MsgBox(TgLoadString(STID_USER_ABORT_WRITE_IN_HYPER), TOOL_NAME,
               INFO_MB);
         return FALSE;
      }
   }
   if (file_number == 0) {
      rewind(stdin);
   } else if (file_number < 0 || file_number == 1 || file_number == 2 ||
         file_number >= MAXEXECOPENFILES) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNUM_WHILE_EXEC_CMD),
            file_number_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].fp != NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FNUM_ARDY_OPEN_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if ((gaOpenFileInfo[file_number].fp=fopen(file_name, file_mode)) ==
         NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_FMOD_WRITE_EXEC_CMD),
            file_name, file_mode, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if ((gaOpenFileInfo[file_number].fname=UtilStrDup(file_name)) ==
         NULL) {
      FailAllocMessage();
   }
   gaOpenFileInfo[file_number].eof = FALSE;
   return TRUE;
}

int ExecCloseFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* close_file(file_number); */
{
   char *file_number_str=argv[0];
   int file_number=(-1);

   UtilRemoveQuotes(file_number_str);

   if (!IntExpression(file_number_str, &file_number, orig_cmd)) {
      return FALSE;
   }
   if (file_number == 0) {
      rewind(stdin);
   } else if (file_number < 0 || file_number == 1 || file_number == 2 ||
         file_number >= MAXEXECOPENFILES) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNUM_WHILE_EXEC_CMD),
            file_number_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FNUM_NOT_OPEN_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      fclose(gaOpenFileInfo[file_number].fp);
      UtilFree(gaOpenFileInfo[file_number].fname);
      gaOpenFileInfo[file_number].fp = NULL;
      gaOpenFileInfo[file_number].fname = NULL;
      gaOpenFileInfo[file_number].eof = FALSE;
   }
   return TRUE;
}

int ExecReadFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* read_file(file_number,result_attr); */
{
   char *file_number_str=argv[0], *result_attr_name=argv[1], *buf=NULL;
   int file_number=(-1);
   struct AttrRec *result_attr=NULL;
   struct ObjRec *result_attr_owner_obj=NULL;

   UtilRemoveQuotes(file_number_str);
   UtilRemoveQuotes(result_attr_name);
   UtilTrimBlanks(result_attr_name);

   if (!IntExpression(file_number_str, &file_number, orig_cmd)) {
      return FALSE;
   }
   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (file_number < 0 || file_number == 1 || file_number == 2 ||
         file_number >= MAXEXECOPENFILES) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNUM_WHILE_EXEC_CMD),
            file_number_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FNUM_NOT_OPEN_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].eof) {
      sprintf(gszMsgBox, TgLoadString(STID_EOF_FOR_FNUM_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      buf = UtilGetALine(gaOpenFileInfo[file_number].fp);
      if (buf == NULL) {
         gaOpenFileInfo[file_number].eof = TRUE;
         sprintf(gszMsgBox, TgLoadString(STID_EOF_FOR_FNUM_WHILE_EXEC_CMD),
               file_number, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr,
         (buf==NULL ? "" : buf));
   if (buf != NULL) UtilFree(buf);

   return TRUE;
}

int ExecWriteFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* write_file(file_number,string); */
{
   char *file_number_str=argv[0], *str=argv[1];
   int file_number=(-1);

   UtilRemoveQuotes(file_number_str);
   UtilRemoveQuotes(str);

   if (!IntExpression(file_number_str, &file_number, orig_cmd)) {
      return FALSE;
   }
   if (file_number < 0 || file_number >= MAXEXECOPENFILES) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNUM_WHILE_EXEC_CMD),
            file_number_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FNUM_NOT_OPEN_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (fprintf(gaOpenFileInfo[file_number].fp, "%s", str) == EOF) {
      if (file_number < 3) {
         sprintf(gszMsgBox, TgLoadString(STID_WRI_FNUM_FAIL_WHILE_EXEC_CMD),
               file_number, orig_cmd);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_WRI_FNN_FAIL_WHILE_EXEC_CMD),
               file_number, gaOpenFileInfo[file_number].fname, orig_cmd);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }

   return TRUE;
}

int ExecFlushFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* flush_file(file_number); */
{
   char *file_number_str=argv[0];
   int file_number=(-1);

   UtilRemoveQuotes(file_number_str);

   if (!IntExpression(file_number_str, &file_number, orig_cmd)) {
      return FALSE;
   }
   if (file_number <= 0 || file_number >= MAXEXECOPENFILES) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNUM_WHILE_EXEC_CMD),
            file_number_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (gaOpenFileInfo[file_number].fp == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FNUM_NOT_OPEN_WHILE_EXEC_CMD),
            file_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   fflush(gaOpenFileInfo[file_number].fp);

   return TRUE;
}

int ExecAppendFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* append_file(dest_file_name,src_file_name); */
{
   char *dest_file_name=argv[0], *src_file_name=argv[1];

   UtilRemoveQuotes(dest_file_name);
   UtilRemoveQuotes(src_file_name);
   UtilTrimBlanks(dest_file_name);
   UtilTrimBlanks(src_file_name);

   if (!UtilPathExists(src_file_name)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PATH_WHILE_EXEC_CMD),
            src_file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (UtilPathExists(dest_file_name)) {
      char buf[0x1000];
      int fd1=(-1), fd2=(-1), failed=TRUE, bytes_read=0;

      if ((fd1=open(src_file_name, O_RDONLY)) != (-1) &&
            (fd2=open(dest_file_name, O_WRONLY|O_APPEND)) != (-1)) {
         failed = FALSE;
         while ((bytes_read=read(fd1, buf, sizeof(buf))) > 0) {
            if (write(fd2, buf, bytes_read) != bytes_read) {
               failed = TRUE;
               break;
            }
         }
      }
      if (fd1 != (-1)) close(fd1);
      if (fd2 != (-1)) close(fd2);
      if (failed) {
         sprintf(gszMsgBox, TgLoadString(STID_APPEND_FAILED_WHILE_EXEC_CMD),
               src_file_name, dest_file_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   } else if (UtilCopyFile(src_file_name, dest_file_name) !=
         TG_REMOTE_STATUS_OK) {
      sprintf(gszMsgBox, TgLoadString(STID_COPY_FAILED_WHILE_EXEC_CMD),
            src_file_name, dest_file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

/* do not translate -- program constants */
static char *gaszOutputFormats[MAXDEFWHERETOPRINT+1] = {
   "Printer", "EPS", "PS", "Bitmap", "Text", "EPSI", "GIF", "HTML", "PDF",
   "TiffEPSI", "PNG", "JPEG", "PPM", "NetList", NULL
};

int ExecSetOutputFormat(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_output_format(format,color_output); */
{
   char *format=argv[0], *color_output_str=argv[1];
   int i, color_output=(-1);

   UtilRemoveQuotes(format);
   UtilRemoveQuotes(color_output_str);
   UtilTrimBlanks(format);

   if (!IntExpression(color_output_str, &color_output, orig_cmd)) {
      return FALSE;
   }
   if (*format == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FORMAT_WHILE_EXEC_CMD),
            format, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (color_output != 0) color_output = TRUE;

   for (i=0; i < MAXDEFWHERETOPRINT; i++) {
      if (UtilStrICmp(format, gaszOutputFormats[i]) == 0) {
         SetWhereToPrint(i);
         if (color_output != colorDump) ToggleColorPostScript();
         return TRUE;
      }
   }
   if (color_output != colorDump) ToggleColorPostScript();
   for (i=MAXDEFWHERETOPRINT; i < maxWhereToPrint; i++) {
      if (UtilStrICmp(format, GetExportName(i, EXPORT_THIS)) == 0) {
         SetWhereToPrint(i);
         return TRUE;
      }
   }
   sprintf(gszMsgBox, TgLoadString(STID_INVALID_FORMAT_WHILE_EXEC_CMD),
         format, orig_cmd);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

int ExecSetExportClipRect(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_export_clip_rect(ltx,lty,rbx,rby); */
{
   char *ltx_str=argv[0], *lty_str=argv[1], *rbx_str=argv[2], *rby_str=argv[3];
   int ltx=0, lty=0, rbx=0, rby=0;

   UtilRemoveQuotes(ltx_str);
   UtilRemoveQuotes(lty_str);
   UtilRemoveQuotes(rbx_str);
   UtilRemoveQuotes(rby_str);

   if (!IntExpression(ltx_str, &ltx, orig_cmd) ||
         !IntExpression(lty_str, &lty, orig_cmd) ||
         !IntExpression(rbx_str, &rbx, orig_cmd) ||
         !IntExpression(rby_str, &rby, orig_cmd)) {
      return FALSE;
   }
   if (ltx >= rbx || lty >= rby) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_REC_WHILE_EXEC_CMD),
            ltx, lty, rbx, rby, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (gpExportClipBBox == NULL) {
      gpExportClipBBox = (struct BBRec *)malloc(sizeof(struct BBRec));
      if (gpExportClipBBox == NULL) FailAllocMessage();
   }
   gpExportClipBBox->ltx = ltx;
   gpExportClipBBox->lty = lty;
   gpExportClipBBox->rbx = rbx;
   gpExportClipBBox->rby = rby;

   return TRUE;
}

int ExecImportFile(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* import_file(file_name,format,ltx,lty); */
{
   char *file_name=argv[0], *format=argv[1], *ltx_str=argv[2], *lty_str=argv[3];
   int ltx=0, lty=0, import_type=INVALID, x_hot, y_hot, w=0, h=0, ncolors=0;
   unsigned int ui_image_w=0, ui_image_h=0;
   Pixmap bitmap=None, pixmap=None;
   XImage *image=NULL, *bitmap_image=NULL;
   char *color_char=NULL, **color_str=NULL, *xpm_data=NULL;
   char xpm_fname[MAXPATHLENGTH];
   int chars_per_pixel=0, first_pixel_is_bg=0, *pixels=NULL;
   int rc=BitmapOpenFailed, image_w=0, image_h=0;
   struct ObjRec *new_obj_ptr=NULL;

   UtilRemoveQuotes(file_name);
   UtilRemoveQuotes(format);
   UtilRemoveQuotes(ltx_str);
   UtilRemoveQuotes(lty_str);
   UtilTrimBlanks(file_name);
   UtilTrimBlanks(format);

   if (*file_name == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_FNAME_WHILE_EXEC_CMD),
            file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else if (!IntExpression(ltx_str, &ltx, orig_cmd) ||
         !IntExpression(lty_str, &lty, orig_cmd)) {
      return FALSE;
   }
   if (topSel != NULL) RemoveAllSel();

   if (UtilStrICmp(format, "TGIF") == 0) {
      int dx=0, dy=0, saved_ltx=0, saved_lty=0, saved_rbx=0, saved_rby=0;
      int saved_move_mode=moveMode;

      import_type = FILE_IMPORT;
      StartCompositeCmd();
      importingFile = TRUE;
      importingFromExec = TRUE;
      rc = ImportGivenFile(file_name, FALSE, FALSE);
      importingFromExec = FALSE;
      importingFile = FALSE;
      if (rc != TRUE) {
         EndCompositeCmd();
         sprintf(gszMsgBox, TgLoadString(STID_IMPORT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else if (topSel == NULL) {
         EndCompositeCmd();
         sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_TO_IMP_WHILE_EXEC_CMD),
               file_name, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      UpdSelBBox();
      saved_ltx = selLtX; saved_lty = selLtY;
      saved_rbx = selRbX; saved_rby = selRbY;
      dx = ltx-selLtX;
      dy = lty-selLtY;
      moveMode = UNCONST_MOVE;
      MoveAllSel(dx, dy);
      moveMode = saved_move_mode;
      RedrawAreas(botObj,
            saved_ltx-GRID_ABS_SIZE(1), saved_lty-GRID_ABS_SIZE(1),
            saved_rbx+GRID_ABS_SIZE(1), saved_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      EndCompositeCmd();

      return TRUE;
   }

   if (UtilStrICmp(format, "XBM") == 0) {
      import_type = FILE_IMPORTXBM;
      rc = XReadBitmapFile(mainDisplay, mainWindow, file_name,
            &ui_image_w, &ui_image_h, &bitmap, &x_hot, &y_hot);
      image_w = ui_image_w; image_h = ui_image_h;
   } else if (UtilStrICmp(format, "XPM") == 0) {
      import_type = FILE_IMPORTXPM;
      rc = MyReadPixmapFile(file_name, &image_w, &image_h, &w, &h,
            &pixmap, &image, &bitmap, &bitmap_image, &ncolors, &chars_per_pixel,
            &first_pixel_is_bg, &color_char, &color_str, &pixels, &xpm_data);
   } else if (UtilStrICmp(format, "GIF") == 0) {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertGifToXpm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   } else if (UtilStrICmp(format, "PNG") == 0) {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertPngToXpm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   } else if (UtilStrICmp(format, "JPEG") == 0) {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertJpegToXpm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   } else if (UtilStrICmp(format, "PPM") == 0) {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertPpmToXpm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   } else if (UtilStrICmp(format, "PGM") == 0) {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertPgmToXpm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   } else if (UtilStrICmp(format, "PBM") == 0) {
      import_type = FILE_IMPORTXBM;
      *xpm_fname = '\0';
      if (!ConvertPbmToXbm(file_name, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_TO_XBM_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = XReadBitmapFile(mainDisplay, mainWindow, xpm_fname,
               &ui_image_w, &ui_image_h, &bitmap, &x_hot, &y_hot);
         image_w = ui_image_w; image_h = ui_image_h;
         unlink(xpm_fname);
      }
   } else {
      import_type = FILE_IMPORTXPM;
      *xpm_fname = '\0';
      if (!ConvertOtherToXpm(file_name, format, xpm_fname, sizeof(xpm_fname))) {
         sprintf(gszMsgBox, TgLoadString(STID_CONVERT_FORMAT_WHILE_EXEC_CMD),
               file_name, format, orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      } else {
         rc = MyReadPixmapFile(xpm_fname, &image_w, &image_h, &w, &h,
               &pixmap, &image, &bitmap, &bitmap_image, &ncolors,
               &chars_per_pixel, &first_pixel_is_bg, &color_char, &color_str,
               &pixels, &xpm_data);
         unlink(xpm_fname);
      }
   }
   importingFile = TRUE;
   if (rc != BitmapSuccess) {
      importingFile = FALSE;
      sprintf(gszMsgBox, TgLoadString(STID_IMPORT_FAILED_WHILE_EXEC_CMD),
            file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   switch (import_type) {
   case FILE_IMPORTXBM:
      new_obj_ptr = CreateXBmObj(image_w, image_h, image_w, image_h,
            bitmap, image);
      break;
   case FILE_IMPORTXPM:
      new_obj_ptr = CreateXPmObj(image_w, image_h, w, h, pixmap, image, bitmap,
            bitmap_image, ncolors, chars_per_pixel, first_pixel_is_bg,
            color_char, color_str, pixels, xpm_data);
      break;
   default: importingFile = FALSE; return FALSE;
   }
   importingFile = FALSE;

   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   MoveObj(new_obj_ptr, ltx-new_obj_ptr->obbox.ltx, lty-new_obj_ptr->obbox.lty);
   AddObj(NULL, topObj, new_obj_ptr);
   SelectTopObj();
   UpdSelBBox();

   RecordNewObjCmd();
   RedrawAnArea(botObj,
         new_obj_ptr->bbox.ltx-GRID_ABS_SIZE(1),
         new_obj_ptr->bbox.lty-GRID_ABS_SIZE(1),
         new_obj_ptr->bbox.rbx+GRID_ABS_SIZE(1),
         new_obj_ptr->bbox.rby+GRID_ABS_SIZE(1));
   SetFileModified(TRUE);

   return TRUE;
}

int ExecSetXpmOutputVersion(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_xpm_output_version(version_number); */
{
   char *version_number_str=argv[0];
   int version_number=(-1);

   UtilRemoveQuotes(version_number_str);

   if (!IntExpression(version_number_str, &version_number, orig_cmd)) {
      return FALSE;
   }
   if (version_number != 1 && version_number != 3) {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_VER_NUM_WHILE_EXEC_CMD),
            version_number, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   SetXpmOutputVersion(version_number);
   return TRUE;
}

int ExecEditIniSection(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* edit_ini_section(attr_name,title,section,ini_path); */
{
   char *attr_name=argv[0], *title=argv[1], *section=argv[2], *ini_path=argv[3];
   char buf[40], *psz_keys=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(title);
   UtilRemoveQuotes(section);
   UtilRemoveQuotes(ini_path);
   if (strcmp(attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (strcmp(title, "NULL") == 0) {
      title = NULL;
   }
   if ((psz_keys=tgGetProfileString(section, NULL, ini_path)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_EMPTY_INI_SEC_WHILE_EXEC_CMD),
            section, ini_path, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      tgFreeProfileString(psz_keys);
   }
   if (EditIniSection(title, section, ini_path, NULL)) {
      strcpy(buf, "OK");
   } else {
      strcpy(buf, "CANCEL");
   }
   if (attr_ptr != NULL) {
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, buf);
   }
   return TRUE;
}

int ExecSelectFromIniSection(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* select_from_ini_section(attr_name,title,section,ini_path); */
{
   char *attr_name=argv[0], *title=argv[1], *section=argv[2], *ini_path=argv[3];
   char *psz_keys=NULL, *buf=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(title);
   UtilRemoveQuotes(section);
   UtilRemoveQuotes(ini_path);
   if (strcmp(attr_name, "NULL") != 0) {
      sprintf(execDummyStr, "%s=", attr_name);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);
   }
   if (strcmp(title, "NULL") == 0) {
      title = NULL;
   }
   if ((psz_keys=tgGetProfileString(section, NULL, ini_path)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_EMPTY_INI_SEC_WHILE_EXEC_CMD),
            section, ini_path, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   } else {
      tgFreeProfileString(psz_keys);
   }
   buf = SelectFromIniSection(title, section, ini_path);
   if (attr_ptr != NULL) {
      ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, (buf==NULL ? "" : buf));
   }
   UtilFree(buf);

   return TRUE;
}

int ExecAppendLineIntoAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* append_line_into_attr(attr_name,str); */
{
   char *attr_name=argv[0], *the_str=argv[1];
   MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   struct TextRec *text_ptr=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   PrepareToReplaceAnObj(attr_owner_obj);

   text_ptr = attr_ptr->obj->detail.t;
   CreateMiniLineFromString(the_str, &pFirstMiniLine, &pLastMiniLine);
   text_ptr->minilines.last->next = pFirstMiniLine;
   pFirstMiniLine->prev = text_ptr->minilines.last;
   text_ptr->minilines.last = pLastMiniLine;
   text_ptr->lines++;
   RecalcTextMetrics(text_ptr, attr_ptr->obj->x, text_ptr->baseline_y);
   UpdTextBBox(attr_ptr->obj);
   AdjObjBBox(attr_owner_obj);

   RecordReplaceAnObj(attr_owner_obj);
   RedrawAnArea(attr_ptr->obj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.lty-GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.rbx+GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecInsertLineIntoAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* insert_line_into_attr(attr_name,str,line_number); */
{
   char *attr_name=argv[0], *the_str=argv[1];
   char *line_number_str=argv[2];
   int count=0, line_number=(-1), ltx=0, lty=0, rbx=0, rby=0, found=FALSE;
   MiniLineInfo *pMiniLine=NULL, *pFirstMiniLine=NULL, *pLastMiniLine=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   struct TextRec *text_ptr=NULL;

   UtilRemoveQuotes(attr_name);
   UtilRemoveQuotes(the_str);
   UtilRemoveQuotes(line_number_str);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   if (!IntExpression(line_number_str, &line_number, orig_cmd)) {
      return FALSE;
   }
   if (line_number <= 0) {
      if (line_number == 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_LINENUM_WHILE_EXEC_CMD),
               line_number, orig_cmd);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_SMALL_LNUM_WHILE_EXEC_CMD),
               line_number, orig_cmd);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ltx = attr_ptr->obj->bbox.ltx; lty = attr_ptr->obj->bbox.lty;
   rbx = attr_ptr->obj->bbox.rbx; rby = attr_ptr->obj->bbox.rby;
   PrepareToReplaceAnObj(attr_owner_obj);

   text_ptr = attr_ptr->obj->detail.t;

   count = 0;
   for (pMiniLine=attr_ptr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next, count++) {
      if (count == line_number) {
         CreateMiniLineFromString(the_str, &pFirstMiniLine, &pLastMiniLine);
         pMiniLine->prev->next = pFirstMiniLine;
         pFirstMiniLine->prev = pMiniLine->prev;
         pFirstMiniLine->next = pMiniLine;
         pMiniLine->prev = pFirstMiniLine;
         text_ptr->lines++;
         found = TRUE;
         break;
      }
   }
   while (!found) {
      if (count++ == line_number) {
         CreateMiniLineFromString(the_str, &pFirstMiniLine, &pLastMiniLine);
         text_ptr->minilines.last->next = pFirstMiniLine;
         pFirstMiniLine->prev = text_ptr->minilines.last;
         text_ptr->minilines.last = pLastMiniLine;
         found = TRUE;
      } else {
         CreateMiniLineFromString("", &pFirstMiniLine, &pLastMiniLine);
         text_ptr->minilines.last->next = pFirstMiniLine;
         pFirstMiniLine->prev = text_ptr->minilines.last;
         text_ptr->minilines.last = pLastMiniLine;
         pFirstMiniLine = pLastMiniLine = NULL;
      }
      text_ptr->lines++;
   }
   RecalcTextMetrics(text_ptr, attr_ptr->obj->x, text_ptr->baseline_y);
   UpdTextBBox(attr_ptr->obj);
   AdjObjBBox(attr_owner_obj);

   RecordReplaceAnObj(attr_owner_obj);
   RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
         rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.ltx-GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.lty-GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.rbx+GRID_ABS_SIZE(1),
         attr_ptr->obj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecClearAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* clear_attr(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   struct TextRec *text_ptr=NULL;
   int changed=FALSE, ltx=0, lty=0, rbx=0, rby=0;

   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   ltx = attr_ptr->obj->bbox.ltx; lty = attr_ptr->obj->bbox.lty;
   rbx = attr_ptr->obj->bbox.rbx; rby = attr_ptr->obj->bbox.rby;

   StartCompositeCmd();

   text_ptr = attr_ptr->obj->detail.t;
   if (text_ptr->minilines.first->next != NULL) {
      MiniLineInfo *pMiniLine=NULL, *pNextMiniLine=NULL;

      if (attr_owner_obj == tgifObj) {
         AddObj(NULL, topObj, tgifObj);
         AddNewSelObj(topObj);
      }
      PrepareToReplaceAnObj(attr_owner_obj);
      for (pMiniLine=text_ptr->minilines.first->next; pMiniLine != NULL;
            pMiniLine=pNextMiniLine) {
         pNextMiniLine = pMiniLine->next;
         UnlinkMiniLine(pMiniLine);
         FreeMiniLine(pMiniLine);
      }
      if (attr_owner_obj == tgifObj) recordCmdIncludeTgifObj = TRUE;
      RecordReplaceAnObj(attr_owner_obj);
      if (attr_owner_obj == tgifObj) {
         recordCmdIncludeTgifObj = FALSE;
         UnlinkObj(topObj);
         FreeTopSel();
      }
      changed = TRUE;
   }
   replaceAttrFirstValueRedraw = FALSE;
   if (ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, "")) {
      changed = TRUE;
   }
   replaceAttrFirstValueRedraw = TRUE;

   if (changed) {
      RedrawAreas(botObj, ltx-GRID_ABS_SIZE(1), lty-GRID_ABS_SIZE(1),
            rbx+GRID_ABS_SIZE(1), rby+GRID_ABS_SIZE(1),
            attr_ptr->obj->bbox.ltx-GRID_ABS_SIZE(1),
            attr_ptr->obj->bbox.lty-GRID_ABS_SIZE(1),
            attr_ptr->obj->bbox.rbx+GRID_ABS_SIZE(1),
            attr_ptr->obj->bbox.rby+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
   }
   EndCompositeCmd();

   return TRUE;
}

int ExecCreateText(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_text_obj(abs_x,abs_baseline_y,str); */
{
   char *abs_x_str=argv[0], *abs_baseline_y_str=argv[1];
   char *the_str=argv[2];
   int abs_x=0, abs_baseline_y=0;

   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_baseline_y_str);
   UtilRemoveQuotes(the_str);
   if (!IntExpression(abs_x_str, &abs_x, orig_cmd) ||
         !IntExpression(abs_baseline_y_str, &abs_baseline_y, orig_cmd)) {
      return FALSE;
   }
   if (*the_str == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_BAD_EMPTY_STR_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   MakeQuiescent();

   NewCurText();
   PrepareToReplaceAnObj(curTextObj);
   DynStrSet(&GetTextObjFirstStrSeg(topObj)->dyn_str, the_str);
   curTextModified = TRUE;
   CreateTextObj(TRUE, FALSE);
   curTextModified = FALSE;
   MoveObj(topObj, abs_x-topObj->x,
         abs_baseline_y-topObj->detail.t->baseline_y);

   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_box_obj(abs_ltx,abs_lty,abs_rbx,abs_rby); */
{
   char *abs_ltx_str=argv[0], *abs_lty_str=argv[1];
   char *abs_rbx_str=argv[2], *abs_rby_str=argv[3];
   int abs_ltx=0, abs_lty=0, abs_rbx=0, abs_rby=0;

   UtilRemoveQuotes(abs_ltx_str);
   UtilRemoveQuotes(abs_lty_str);
   UtilRemoveQuotes(abs_rbx_str);
   UtilRemoveQuotes(abs_rby_str);
   if (!IntExpression(abs_ltx_str, &abs_ltx, orig_cmd) ||
         !IntExpression(abs_lty_str, &abs_lty, orig_cmd) ||
         !IntExpression(abs_rbx_str, &abs_rbx, orig_cmd) ||
         !IntExpression(abs_rby_str, &abs_rby, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   CreateBoxObj(abs_ltx, abs_lty, abs_rbx, abs_rby, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateCornerOval(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_corner_oval_obj(abs_ltx,abs_lty,abs_rbx,abs_rby); */
{
   char *abs_ltx_str=argv[0], *abs_lty_str=argv[1];
   char *abs_rbx_str=argv[2], *abs_rby_str=argv[3];
   int abs_ltx=0, abs_lty=0, abs_rbx=0, abs_rby=0;
   struct BBRec obbox;

   UtilRemoveQuotes(abs_ltx_str);
   UtilRemoveQuotes(abs_lty_str);
   UtilRemoveQuotes(abs_rbx_str);
   UtilRemoveQuotes(abs_rby_str);
   if (!IntExpression(abs_ltx_str, &abs_ltx, orig_cmd) ||
         !IntExpression(abs_lty_str, &abs_lty, orig_cmd) ||
         !IntExpression(abs_rbx_str, &abs_rbx, orig_cmd) ||
         !IntExpression(abs_rby_str, &abs_rby, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   SetBBRec(&obbox, abs_ltx, abs_lty, abs_rbx, abs_rby);
   CreateOvalObj(&obbox, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateCenterOval(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_center_oval_obj(abs_cx,abs_cy,radius); */
{
   char *abs_cx_str=argv[0], *abs_cy_str=argv[1], *radius_str=argv[2];
   int abs_cx=0, abs_cy=0, radius=0;
   struct BBRec obbox;

   UtilRemoveQuotes(abs_cx_str);
   UtilRemoveQuotes(abs_cy_str);
   UtilRemoveQuotes(radius_str);
   if (!IntExpression(abs_cx_str, &abs_cx, orig_cmd) ||
         !IntExpression(abs_cy_str, &abs_cy, orig_cmd) ||
         !IntExpression(radius_str, &radius, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   SetBBRec(&obbox, abs_cx-radius, abs_cy-radius, abs_cx+radius, abs_cy+radius);
   CreateOvalObj(&obbox, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateEdgeOval(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_edge_oval_obj(abs_ltx,abs_lty,abs_rbx,abs_rby); */
{
   char *abs_ltx_str=argv[0], *abs_lty_str=argv[1];
   char *abs_rbx_str=argv[2], *abs_rby_str=argv[3];
   int abs_ltx=0, abs_lty=0, abs_rbx=0, abs_rby=0, cx=0, cy=0, w=0;
   double dx=(double)0, dy=(double)0, r=(double)0;
   struct BBRec obbox;

   UtilRemoveQuotes(abs_ltx_str);
   UtilRemoveQuotes(abs_lty_str);
   UtilRemoveQuotes(abs_rbx_str);
   UtilRemoveQuotes(abs_rby_str);
   if (!IntExpression(abs_ltx_str, &abs_ltx, orig_cmd) ||
         !IntExpression(abs_lty_str, &abs_lty, orig_cmd) ||
         !IntExpression(abs_rbx_str, &abs_rbx, orig_cmd) ||
         !IntExpression(abs_rby_str, &abs_rby, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   cx = ((abs_rbx + abs_ltx) >> 1);
   cy = ((abs_rby + abs_lty) >> 1);
   dx = (double)(cx - abs_ltx);
   dy = (double)(cy - abs_lty);
   r = (double)sqrt(dx*dx+dy*dy);
   w = round(r);
   SetBBRec(&obbox, cx-w, cy-w, cx+w, cy+w);
   CreateOvalObj(&obbox, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateRCBox(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_rcbox_obj(abs_ltx,abs_lty,abs_rbx,abs_rby); */
{
   char *abs_ltx_str=argv[0], *abs_lty_str=argv[1];
   char *abs_rbx_str=argv[2], *abs_rby_str=argv[3];
   int abs_ltx=0, abs_lty=0, abs_rbx=0, abs_rby=0;

   UtilRemoveQuotes(abs_ltx_str);
   UtilRemoveQuotes(abs_lty_str);
   UtilRemoveQuotes(abs_rbx_str);
   UtilRemoveQuotes(abs_rby_str);
   if (!IntExpression(abs_ltx_str, &abs_ltx, orig_cmd) ||
         !IntExpression(abs_lty_str, &abs_lty, orig_cmd) ||
         !IntExpression(abs_rbx_str, &abs_rbx, orig_cmd) ||
         !IntExpression(abs_rby_str, &abs_rby, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   CreateRCBoxObj(abs_ltx, abs_lty, abs_rbx, abs_rby, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

int ExecCreateArc(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_arc_obj(abs_cx,abs_cy,radius,dir,theta1,theta2); */
{
   char *abs_cx_str=argv[0], *abs_cy_str=argv[1], *radius_str=argv[2];
   char *dir_str=argv[3], *theta1_str=argv[4], *theta2_str=argv[5];
   int abs_cx=0, abs_cy=0, radius=0, dir=0, x1=0, y1=0, x2=0, y2=0;
   double theta1=(double)0, theta2=(double)0;
   double angle_in_radian=(double)0, angle2=(double)0;

   UtilRemoveQuotes(abs_cx_str);
   UtilRemoveQuotes(abs_cy_str);
   UtilRemoveQuotes(radius_str);
   UtilRemoveQuotes(dir_str);
   UtilRemoveQuotes(theta1_str);
   UtilRemoveQuotes(theta2_str);
   if (!IntExpression(abs_cx_str, &abs_cx, orig_cmd) ||
         !IntExpression(abs_cy_str, &abs_cy, orig_cmd) ||
         !IntExpression(radius_str, &radius, orig_cmd) ||
         !FloatExpression(theta1_str, &theta1, orig_cmd) ||
         !FloatExpression(theta2_str, &theta2, orig_cmd)) {
      return FALSE;
   }
   UtilTrimBlanks(dir_str);
   switch (*dir_str) {
   case '+': dir = ARC_CW; break;
   case '-': dir = ARC_CCW; break;
   default:
      sprintf(gszMsgBox, TgLoadString(STID_BAD_DIRECTION_WHILE_EXEC_CMD),
            dir_str, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (radius < 1) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CREATE_ARC_WITH_RAD1),
            TOOL_NAME, radius);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   MakeQuiescent();

   switch (dir) {
   case ARC_CCW:
      theta1 += (double)90;
      theta2 += (double)90;
      break;
   case ARC_CW:
      theta1 = -theta1+((double)90);
      theta2 = -theta2+((double)90);
      break;
   }
   angle_in_radian = theta1 * M_PI / ((double)180);
   x1 = round(((double)radius)*cos(angle_in_radian));
   y1 = -round(((double)radius)*sin(angle_in_radian));
   angle_in_radian = theta2 * M_PI / ((double)180);
   x2 = round(((double)radius)*cos(angle_in_radian));
   y2 = -round(((double)radius)*sin(angle_in_radian));

   while (theta1 < (double)0) theta1 += (double)360;
   while (theta2 > theta1) theta2 -= (double)360;
   while (theta2 < theta1) theta2 += (double)360;

   switch (dir) {
   case ARC_CCW:
      angle2 = theta2-theta1;
      if (angle2 == 0) angle2 = (double)360;
      break;
   case ARC_CW:
      angle2 = theta2-theta1-((double)360);
      break;
   }
   CreateArcObj(abs_cx, abs_cy, abs_cx+x1, abs_cy+y1, abs_cx+x2, abs_cy+y2, dir,
         -radius, -radius, (radius<<1), (radius<<1), round(theta1*((double)64)),
         round(angle2*((double)64)), TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
   return TRUE;
}

static IntPoint firstVertexInCreate, lastVertexInCreate;

int ExecCreateFirstVertex(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_first_vertex(abs_x,abs_y); */
{
   char *abs_x_str=argv[0], *abs_y_str=argv[1];
   int abs_x=0, abs_y=0;

   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_y_str);
   if (!IntExpression(abs_x_str, &abs_x, orig_cmd) ||
         !IntExpression(abs_y_str, &abs_y, orig_cmd)) {
      return FALSE;
   }
   MakeQuiescent();

   ResetCreatePoly();
   AddPtToCreatePoly(abs_x, abs_y);
   firstVertexInCreate.x = abs_x;
   firstVertexInCreate.y = abs_y;

   return TRUE;
}

int ExecCreateNextVertex(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* create_next_vertex(abs_x,abs_y); */
{
   char *abs_x_str=argv[0], *abs_y_str=argv[1];
   int abs_x=0, abs_y=0;

   UtilRemoveQuotes(abs_x_str);
   UtilRemoveQuotes(abs_y_str);
   if (!IntExpression(abs_x_str, &abs_x, orig_cmd) ||
         !IntExpression(abs_y_str, &abs_y, orig_cmd)) {
      return FALSE;
   }
   if (numPtsInPoly == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_START_VRTX_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   AddPtToCreatePoly(abs_x, abs_y);
   lastVertexInCreate.x = abs_x;
   lastVertexInCreate.y = abs_y;

   return TRUE;
}

void ExecCreatePoly(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* create_poly_obj(); */
{
   if (numPtsInPoly <= 1) {
      sprintf(gszMsgBox, TgLoadString(STID_TOO_FEW_VRTX_WHILE_EXEC_CMD),
            numPtsInPoly, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ResetCreatePoly();
      gnAbortExec = TRUE;
      return;
   }
   MakeQuiescent();

   CreatePolyObj(numPtsInPoly, TRUE);

   RecordNewObjCmd();
   RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
         topObj->bbox.lty-GRID_ABS_SIZE(1),
         topObj->bbox.rbx+GRID_ABS_SIZE(1),
         topObj->bbox.rby+GRID_ABS_SIZE(1));

   SetFileModified(TRUE);
}

void ExecCreatePolygon(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* create_polygon_obj(); */
{
   if (numPtsInPoly <= 2) {
      sprintf(gszMsgBox, TgLoadString(STID_TOO_FEW_VRTX_WHILE_EXEC_CMD),
            numPtsInPoly, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ResetCreatePoly();
   } else {
      MakeQuiescent();

      if (firstVertexInCreate.x != lastVertexInCreate.x ||
            firstVertexInCreate.y != lastVertexInCreate.y) {
         AddPtToCreatePoly(firstVertexInCreate.x, firstVertexInCreate.y);
         lastVertexInCreate.x = firstVertexInCreate.x;
         lastVertexInCreate.y = firstVertexInCreate.y;
      }
      CreatePolygonObj(numPtsInPoly, TRUE);

      RecordNewObjCmd();
      RedrawAnArea(botObj, topObj->bbox.ltx-GRID_ABS_SIZE(1),
            topObj->bbox.lty-GRID_ABS_SIZE(1),
            topObj->bbox.rbx+GRID_ABS_SIZE(1),
            topObj->bbox.rby+GRID_ABS_SIZE(1));

      SetFileModified(TRUE);
   }
}

static struct SelRec *topSelBeforeCreateGroup=NULL;
static int createGroupDepth=0;

static
void ResetCreateGroup()
{
   struct SelRec *next_sel=NULL;

   while (topSelBeforeCreateGroup != NULL) {
      next_sel = topSelBeforeCreateGroup->next;
      free(topSelBeforeCreateGroup);
      topSelBeforeCreateGroup = next_sel;
   }
   createGroupDepth = 0;
}

void ExecStartCreateGroup(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* start_create_group_obj(); */
{
   struct SelRec *sel_ptr=NULL;

   MakeQuiescent();
   sel_ptr = SelectThisObject(topObj);
   sel_ptr->next = topSelBeforeCreateGroup;
   topSelBeforeCreateGroup = sel_ptr;
   createGroupDepth++;
}

void ExecCreateGroup(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* create_group_obj(); */
{
   struct SelRec *next_sel=NULL;
   struct ObjRec *optr=NULL, *start_obj=NULL;
   int num_objs=0;

   if (createGroupDepth <= 0 || topSelBeforeCreateGroup == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_START_OBJ_WHILE_EXEC_CMD),
            "start_create_group_obj", orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ResetCreateGroup();
      gnAbortExec = TRUE;
      return;
   }
   MakeQuiescent();

   if (topSelBeforeCreateGroup->obj != NULL) {
      for (optr=topObj; optr != NULL; optr=optr->next) {
         if (optr == topSelBeforeCreateGroup->obj) {
            break;
         }
      }
      if (optr == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_MARK_WHILE_EXEC_CMD),
               "start_create_group_obj", orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ResetCreateGroup();
         gnAbortExec = TRUE;
         return;
      }
   }
   if (topSelBeforeCreateGroup->obj == NULL) {
      start_obj = botObj;
   } else {
      start_obj = topSelBeforeCreateGroup->obj->prev;
   }
   for (optr=start_obj, num_objs=0; optr != NULL; optr=optr->prev, num_objs++) {
   }
   if (num_objs == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_OBJ_GROUP_WHILE_EXEC_CMD),
            orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      ResetCreateGroup();
      gnAbortExec = TRUE;
      return;
   } else if (num_objs == 1) {
      if (start_obj->type != OBJ_POLY && start_obj->type != OBJ_POLYGON) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_GRP_SNGL_WHILE_EXEC_CMD),
               orig_cmd);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ResetCreateGroup();
         gnAbortExec = TRUE;
         return;
      }
   }
   for (optr=start_obj; optr != NULL; optr=optr->prev) {
      struct SelRec *sel_ptr=SelectThisObject(optr);

      sel_ptr->next = topSel;
      sel_ptr->prev = NULL;
      if (topSel == NULL) {
         botSel = sel_ptr;
      } else {
         topSel->prev = sel_ptr;
      }
      topSel = sel_ptr;
   }
   UpdSelBBox();
   GroupSelObj(TRUE, TRUE, TRUE);
   RemoveAllSel();
   justDupped = FALSE;

   next_sel = topSelBeforeCreateGroup->next;
   free(topSelBeforeCreateGroup);
   topSelBeforeCreateGroup = next_sel;
   createGroupDepth--;
}

int ExecSetAllowInterrupt(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* set_allow_interrupt(true_or_false); */
{
   char *allow_str=argv[0];
   int currently_enabled=execInterruptEnabled;

   UtilRemoveQuotes(allow_str);
   if (strcmp(allow_str, "TRUE") == 0) {
      execInterruptEnabled = TRUE;
   } else if (strcmp(allow_str, "FALSE") == 0) {
      execInterruptEnabled = FALSE;
   } else {
      return BadArg("true_or_false", orig_cmd);
   }
   if (!currently_enabled && execInterruptEnabled && execInterruptQueued) {
      gnAbortExec = TRUE;
      return FALSE;
   }
   return TRUE;
}

static
int DoExecSelectEachObjAndExec(attr_name)
   char *attr_name;
{
   struct ObjRec *optr=NULL;
   int rc=TRUE;

   for (optr=botObj; rc && optr != NULL; optr=optr->prev) {
      struct AttrRec *attr_ptr=NULL;

      attr_ptr = FindAttrWithName(optr, attr_name, NULL);
      if (attr_ptr != NULL) {
         struct SelRec *sel_ptr=SelectThisObject(optr);

         AddSel(NULL, topSel, sel_ptr);
         UpdSelBBox();
         if (!DoExec(attr_ptr, optr)) {
            rc = FALSE;
         }
         RemoveAllSel();
      }
   }
   return rc;
}

int ExecSelectEachObjAndExec(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* select_each_obj_and_exec(attr_name_to_exec); */
{
   char *attr_name_to_exec=argv[0];
   int rc=TRUE;

   UtilRemoveQuotes(attr_name_to_exec);

   if (topSel != NULL) {
      struct AttrRec *attr_ptr=NULL;

      sprintf(execDummyStr, "%s=", attr_name_to_exec);
      attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, NULL);
      if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

      rc = DoExecSelectEachObjAndExec(execDummyStr);
   } else {
      char *attr_name=(char*)malloc(strlen(attr_name_to_exec)+2);

      if (attr_name == NULL) FailAllocMessage();
      sprintf(attr_name, "%s=", attr_name_to_exec);

      MakeQuiescent();
      StartCompositeCmd();
      if (lastPageNum == 1) {
         rc = DoExecSelectEachObjAndExec(attr_name);
      } else {
         int saved_cur_page_num=curPageNum, prev_page_num=curPageNum;

         for (curPageNum=1; rc && curPageNum <= lastPageNum; curPageNum++) {
            if (prev_page_num != curPageNum) {
               PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, prev_page_num);
               GotoPageNum(curPageNum);
               ShowPage();
               XSync(mainDisplay, False);
               RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, curPageNum);
               ClearAndRedrawDrawWindow();
               prev_page_num = curPageNum;
            }
            rc = DoExecSelectEachObjAndExec(attr_name);
         }
         if (prev_page_num != curPageNum) {
            PrepareToRecord(CMD_GOTO_PAGE, NULL, NULL, prev_page_num);
            GotoPageNum(saved_cur_page_num);
            ShowPage();
            RecordCmd(CMD_GOTO_PAGE, NULL, NULL, NULL, saved_cur_page_num);
            ClearAndRedrawDrawWindow();
            prev_page_num = curPageNum;
         }
      }
      EndCompositeCmd();

      UtilFree(attr_name);
   }
   return rc;
}

int ExecEditAttrInTextMode(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* edit_attr_in_text_mode(attr_name); */
{
   char *attr_name=argv[0];
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;

   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   EditTextInAttr(attr_ptr);

   return TRUE;
}

void ExecSetFileUnSavable(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* set_file_unsavable(); */
{
   if (!IsFiletUnSavable()) {
      if (!fileModified) SetFileModified(TRUE);
      SetUnSavableFile(TRUE);
      RedrawTitleWindow();
   }
}

static
void ExternalPsToEpsi(scale_val, src_file_name, dest_file_name)
   int scale_val;
   char *src_file_name, *dest_file_name;
{
   char cmd[MAXSTRING<<1];

   switch (scale_val) {
   case 1:
      snprintf(cmd, sizeof(cmd), "pstoepsi %s %s", src_file_name,
            dest_file_name);
      break;
   case 2:
      snprintf(cmd, sizeof(cmd), "pstoepsi -2x %s %s", src_file_name,
            dest_file_name);
      break;
   }
   ExecuteCmd(cmd, FALSE);
}

#define GS_TYPE_GNU  0
#define GS_TYPE_AFPL 1

static
void FindGsVersion(gs_copyright, pn_gs_type, pn_gs_version)
   char *gs_copyright;
   int *pn_gs_type, *pn_gs_version;
{
   char *psz_type=NULL, *psz_name=NULL, *psz_copy=UtilStrDup(gs_copyright);

   if (psz_copy == NULL) FailAllocMessage();
   psz_type = strtok(psz_copy, " \t\r\n");
   if (psz_type != NULL) {
      if (UtilStrICmp(psz_type, "GNU") == 0) {
         if (pn_gs_type != NULL) *pn_gs_type = GS_TYPE_GNU;
      } else if (UtilStrICmp(psz_type, "GPL") == 0) {
         if (pn_gs_type != NULL) *pn_gs_type = GS_TYPE_GNU;
      } else if (UtilStrICmp(psz_type, "AFPL") == 0) {
         if (pn_gs_type != NULL) *pn_gs_type = GS_TYPE_AFPL;
      } else if (UtilStrICmp(psz_type, "Aladdin") == 0) {
         if (pn_gs_type != NULL) *pn_gs_type = GS_TYPE_AFPL;
      } else {
         /* unrecognized type */
         psz_type = NULL;
      }
   }
   if (psz_type != NULL) {
      psz_name = strtok(NULL, " \t\r\n");
      if (UtilStrICmp(psz_name, "Ghostscript") != 0) {
         /* shouldn't come here */
         psz_name = NULL;
      }
   }
   if (psz_type != NULL && psz_name != NULL) {
      for (;;) {
         char *psz=strtok(NULL, " \t\r\n");

         if (psz == NULL) break;
         if (*psz >= '0' && *psz <= '9') {
            int gs_version=(-1);

            if (sscanf(psz, "%d", &gs_version) == 1) {
               if (pn_gs_version != NULL) *pn_gs_version = gs_version;
               break;
            }
         }
      }
   }
   UtilFree(psz_copy);
}

static
int PbmToEpsi(d_scale, pbm_fname, out_fname, orig_cmd)
   double d_scale;
   char *pbm_fname, *out_fname, *orig_cmd;
{
   char xbm_fname[MAXPATHLENGTH];
   int rc=0, x_hot=0, y_hot=0, image_w=0, image_h=0;
   Pixmap bitmap=None;
   FILE *out_fp=NULL;

   *xbm_fname = '\0';
   out_fp = fopen(out_fname, "w");
   if (out_fp == NULL) {
      FailToOpenMessage(out_fname, "w", NULL);
      return FALSE;
   }
   if (!ConvertPbmToXbm(pbm_fname, xbm_fname, sizeof(xbm_fname))) {
      sprintf(gszMsgBox, TgLoadString(STID_CONVERT_TO_XBM_WHILE_EXEC_CMD),
            pbm_fname, "PBM", orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      fclose(out_fp);
      return FALSE;
   }
   rc = XReadBitmapFile(mainDisplay, mainWindow, xbm_fname,
         (unsigned int *)(&image_w), (unsigned int *)(&image_h), &bitmap,
         &x_hot, &y_hot);
   unlink(xbm_fname);

   if (rc == BitmapSuccess) {
      int watch_cursor=watchCursorOnMainWindow;
      XImage *image=NULL;

      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      image = XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h, 1,
            ZPixmap);
      if (image == NULL) {
         MsgBox(TgLoadString(STID_XGETIMAGE_MAY_RUN_OUT_VMEM), TOOL_NAME,
               INFO_MB);
      } else {
         int row=0, col=0, llx=0, lly=0, urx=0, ury=0;
         int top=image_h+1, bottom=(-1), left=image_w+1, right=(-1);
         double dllx=(double)0, dlly=(double)0, durx=(double)0, dury=(double)0;

         for (row=0; row < image_h; row++) {
            for (col=0; col < image_w; col++) {
               if (XGetPixel(image, col, row)) {
                  if (row < top) top = row;
                  if (row > bottom) bottom = row;
                  if (col < left) left = col;
                  if (col > right) right = col;
               }
            }
         }
         dllx = ((double)(left))/d_scale;
         dlly = ((double)(image_h-bottom))/d_scale;
         durx = ((double)(right))/d_scale;
         dury = ((double)(image_h-top))/d_scale;

         llx = round(dllx); lly = round(dlly);
         urx = round(durx); ury = round(dury);

#ifdef _NOT_DEFINED
         fprintf(stderr, "image_w = %1d\n", image_w);
         fprintf(stderr, "image_h = %1d\n", image_h);
         fprintf(stderr, "llx = %1d\n", left);
         fprintf(stderr, "lly = %1d\n", image_h-bottom);
         fprintf(stderr, "urx = %1d\n", right);
         fprintf(stderr, "ury = %1d\n", image_h-top);
         fprintf(stderr, "%%!PS-Adobe-2.0 EPSF-1.2\n");
         fprintf(stderr, "%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
         fprintf(stderr, "%%%%BeginPreview: %d %d 1 %d\n", right-left+1,
               bottom-top+1, bottom-top+1);
         fprintf(stderr, "%%%%EndImage\n");
         fprintf(stderr, "%%%%EndPreview\n");
#endif /* _NOT_DEFINED */

         fprintf(out_fp, "%%!PS-Adobe-2.0 EPSF-1.2\n");
         fprintf(out_fp, "%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
         fprintf(out_fp, "%%%%BeginPreview: %d %d 1 %d\n", right-left+1,
               bottom-top+1, bottom-top+1);

         for (row=top; row <= bottom; row++) {
            int even=TRUE;

            fprintf(out_fp, "%% ");
            for (col=left; col <= right; col+=4) {
               int nibble=0;

               if (XGetPixel(image, col, row)) nibble |= 0x08;
               if (XGetPixel(image, col+1, row)) nibble |= 0x04;
               if (XGetPixel(image, col+2, row)) nibble |= 0x02;
               if (XGetPixel(image, col+3, row)) nibble |= 0x01;
               fprintf(out_fp, "%x", nibble);
               even = !even;
            }
            fprintf(out_fp, "%s\n", (even ? "" : "0"));
         }
         fprintf(out_fp, "%%%%EndImage\n");
         fprintf(out_fp, "%%%%EndPreview\n");

         if (!watch_cursor) {
            SetDefaultCursor(mainWindow);
            ShowCursor();
         }
         XDestroyImage(image);
      }
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_IMPORT_FAILED_WHILE_EXEC_CMD),
            xbm_fname, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   if (bitmap != None) XFreePixmap(mainDisplay, bitmap);
   fclose(out_fp);

   return TRUE;
}

static
int InternalPsToEpsi(scale, src_file_name, dest_file_name, orig_cmd)
   double scale;
   char *src_file_name, *dest_file_name, *orig_cmd;
{
   CVList list;
   CVListElem *elem=NULL;
   char cmd[MAXSTRING<<1], *gs_copyright=NULL;
   char tmp_fname1[MAXPATHLENGTH], tmp_pbm_fname[MAXPATHLENGTH];
   int gs_type=(-1), gs_version=(-1), ps_scale=0;
   double tmp_scale=(double)0;

   if (MkTempFile(tmp_fname1, sizeof(tmp_fname1), tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   sprintf(tmp_pbm_fname, "%s.pbm", tmp_fname1);
   unlink(tmp_pbm_fname);

   CVListInit(&list);
   snprintf(cmd, sizeof(cmd), "%s -help | grep Ghostscript", gsPath);
   if (!ExecuteCmdGetResult(cmd, &list, 1)) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD_EXEC_CMD),
            cmd, orig_cmd);
      return FALSE;
   }
   for (elem=ListFirst(&list); elem != NULL; elem=ListNext(&list, elem)) {
      gs_copyright = UtilStrDup((char*)(elem->obj));
      FindGsVersion(gs_copyright, &gs_type, &gs_version);
      break;
   }
   if (gs_type == (-1) || gs_version == (-1)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IDEN_GS), gs_copyright);
      Msg(gszMsgBox);
   }
   tmp_scale = ((double)72)*scale;
   ps_scale = round(tmp_scale);
   if (ps_scale < 1) ps_scale = 1;
   
   if (fabs(scale-((double)1)) < EQ_TOL) {
      snprintf(cmd, sizeof(cmd), "%s %s -sOutputFile=%s -- %s",
            gsPath, "-q -dBATCH -dNOPAUSE -sDEVICE=pbm", tmp_pbm_fname,
            src_file_name);
   } else {
      snprintf(cmd, sizeof(cmd), "%s %s -r%1dx%1d -sOutputFile=%s -- %s",
            gsPath, "-q -dBATCH -dNOPAUSE -sDEVICE=pbm", ps_scale, ps_scale,
            tmp_pbm_fname, src_file_name);
   }
   ExecuteCmd(cmd, FALSE);
   PbmToEpsi(scale, tmp_pbm_fname, tmp_fname1, orig_cmd);
   UtilConcatFile(tmp_fname1, src_file_name, dest_file_name);

   unlink(tmp_fname1);
   unlink(tmp_pbm_fname);

   /* clean up */
   for (elem=ListFirst(&list); elem != NULL; elem=ListNext(&list, elem)) {
      UtilFree((char*)(elem->obj));
   }
   ListUnlinkAll(&list);
   if (gs_copyright != NULL) UtilFree(gs_copyright);

   return TRUE;
}

static int externalPsToEpsi=INVALID;

int ExecPsToEpsi(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* pstoepsi(dest_file_name,src_file_name,scale); */
{
   char *dest_file_name=argv[0], *src_file_name=argv[1], *scale_str=argv[2];
   double scale=(double)0;

   UtilRemoveQuotes(dest_file_name);
   UtilRemoveQuotes(src_file_name);
   UtilTrimBlanks(dest_file_name);
   UtilTrimBlanks(src_file_name);

   if (PRTGIF && !cmdLineOpenDisplay) {
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_NOT_INTV_WHILE_EXEC_CMD),
            TOOL_NAME, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!UtilPathExists(src_file_name)) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PATH_WHILE_EXEC_CMD),
            src_file_name, orig_cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (!FloatExpression(scale_str, &scale, orig_cmd)) {
      return FALSE;
   }
   if (scale < ((double)0.1) || scale > ((double)10)) {
      sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_STRS_RANGE),
            orig_cmd, "scale", "0.1", "10", scale_str);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (externalPsToEpsi == INVALID) {
      char *c_ptr;
   
      externalPsToEpsi = FALSE;
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ExternalPsToEpsi")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         externalPsToEpsi = TRUE;
      }
   }
   if (externalPsToEpsi) {
      int scale_val=round(scale);

      if (scale_val < 1 || scale_val > 2) {
         sprintf(gszMsgBox, TgLoadString(STID_FUNC_ARG_RANGE),
               orig_cmd, "scale", 1, 2, scale_val);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
      ExternalPsToEpsi(scale_val, src_file_name, dest_file_name);
   } else {
      if (!InternalPsToEpsi(scale, src_file_name, dest_file_name,
            orig_cmd)) {
         /* failed, try external one? */
      }
   }
   return TRUE;
}

int ExecObjsBBoxIntersect(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* objs_bbox_intersect(obj1_name,obj2_name,result_attr); */
{
   char *obj1_name=argv[0], *obj2_name=argv[1], *result_attr_name=argv[2];
   char buf[40];
   struct ObjRec *named_obj1=NULL, *named_obj2=NULL;
   struct ObjRec *top_owner1=NULL, *top_owner2=NULL;
   struct ObjRec *result_attr_owner_obj=NULL;
   struct AttrRec *result_attr=NULL;

   UtilRemoveQuotes(obj1_name);
   UtilRemoveQuotes(obj2_name);
   UtilRemoveQuotes(result_attr_name);
   UtilTrimBlanks(obj1_name);
   UtilTrimBlanks(obj2_name);
   UtilTrimBlanks(result_attr_name);

   named_obj1 = FindObjWithName(botObj, obj_ptr, obj1_name, FALSE,
         FALSE, NULL, &top_owner1);
   if (named_obj1 == NULL) {
      return BadObjName(obj1_name, orig_cmd);
   }
   named_obj2 = FindObjWithName(botObj, obj_ptr, obj2_name, FALSE,
         FALSE, NULL, &top_owner2);
   if (named_obj2 == NULL) {
      return BadObjName(obj2_name, orig_cmd);
   }
   sprintf(execDummyStr, "%s=", result_attr_name);
   result_attr = FindAttrWithName(obj_ptr, execDummyStr,
         &result_attr_owner_obj);
   if (result_attr == NULL) return BadAttr(execDummyStr, orig_cmd);

   strcpy(buf, (BBoxIntersect(named_obj1->bbox, named_obj2->bbox) ?  "1" :
         "0"));

   ReplaceAttrFirstValue(result_attr_owner_obj, result_attr,
         (buf==NULL ? "" : buf));

   return TRUE;
}

void ExecDelAllAttrFromSelObj(obj_ptr, orig_cmd)
   struct ObjRec *obj_ptr;
   char *orig_cmd;
   /* delete_all_attr_from_selected_objs(); */
{
   struct SelRec *sel_ptr=NULL;
   int changed=FALSE;

   if (topSel == NULL) return;

   HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj->fattr != NULL) {
         changed = TRUE;
         PrepareToReplaceAnObj(sel_ptr->obj);
         DelAllAttrs(sel_ptr->obj->fattr);
         sel_ptr->obj->fattr = sel_ptr->obj->lattr = NULL;
         AdjObjBBox(sel_ptr->obj);
         RecordReplaceAnObj(sel_ptr->obj);
      }
   }
   EndCompositeCmd();

   if (changed) {
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      UpdSelBBox();
      justDupped = FALSE;
   }
   HighLightForward();
}

typedef struct tagRandomPermutationInfo {
   int num_states;
   unsigned int *iv;
   unsigned int *state;
} RandomPermutationInfo;

static
void Swap(psz, i, j)
   unsigned int *psz;
   int i, j;
{
    unsigned int t=psz[i];

    psz[i] = psz[j];
    psz[j] = t;
}

static
void RandomPermutation(prpi, num_states)
   RandomPermutationInfo *prpi;
   int num_states;
{
   int i=0, j=0;
   time_t tloc;

   memset(prpi, 0, sizeof(RandomPermutationInfo));
   prpi->num_states = num_states;
   prpi->iv = (unsigned int *)malloc(num_states*sizeof(unsigned int));
   prpi->state = (unsigned int *)malloc(num_states*sizeof(unsigned int));
   if (prpi->iv == NULL || prpi->state == NULL) FailAllocMessage();
   memset(prpi->iv, 0, num_states*sizeof(unsigned int));
   memset(prpi->state, 0, num_states*sizeof(unsigned int));
   for (i=0; i < num_states; i++) prpi->state[i] = i;

   time(&tloc);
#ifndef _NO_RAND48
   srand48((long)tloc);
   for (i=0; i < num_states; i++) {
      double dval=drand48();
      int ival=0;

      if (num_states > 256) {
         ival = (unsigned int)(dval*((double)num_states));
         if (ival >= num_states) ival = num_states - 1;
         if (ival < 0) ival = 0;
      } else {
         ival = (unsigned int)(dval*((double)256));
         if (ival >= 256) ival = 255;
         if (ival < 0) ival = 0;
      }
      prpi->iv[i] = (unsigned int)ival;
   }
#else /* _NO_RAND48 */
   srand((unsigned int)tloc);
   for (i=0; i < num_states; i++) {
      unsigned int uival=(rand() & 0x0ffff);

      uival *= 60013;
      if (num_states > 256) {
         uival = uival % num_states;
      } else {
         uival = (uival & 0x0ff);
      }
      prpi->iv[i] = uival;
   }
#endif /* ~_NO_RAND48 */

   for (i=0, j=0; i < num_states; i++) {
      j = j + (int)(prpi->state[i]) + (int)(prpi->iv[i]);
      j = j % num_states;
      Swap(prpi->state, i, j);
   }
}

int ExecRndPermLinesInAttr(argv, obj_ptr, orig_cmd)
   char **argv, *orig_cmd;
   struct ObjRec *obj_ptr;
   /* random_permute_lines_in_attr(attr); */
{
   char *attr_name=argv[0], *tmp_buf=NULL, **word_array=NULL;
   int i=0, count=0, index=0, need_to_free_tmp_buf=FALSE;
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pMiniLine=NULL;
   struct AttrRec *attr_ptr=NULL;
   struct ObjRec *attr_owner_obj=NULL;
   RandomPermutationInfo rpi;

   UtilRemoveQuotes(attr_name);

   sprintf(execDummyStr, "%s=", attr_name);
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) return BadAttr(execDummyStr, orig_cmd);

   count = 0;
   for (pMiniLine=attr_ptr->obj->detail.t->minilines.first->next;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      count++;
   }
   if (count <= 0) return TRUE;

   word_array = (char**)malloc(count*sizeof(char*));

   if (word_array == NULL) FailAllocMessage();
   memset(word_array, 0, count*sizeof(char*));

   for (pMiniLine=attr_ptr->obj->detail.t->minilines.first->next;
         pMiniLine != NULL; pMiniLine=pMiniLine->next, index++) {
      need_to_free_tmp_buf = FALSE;
      tmp_buf = NULL;
      if (pMiniLine == attr_ptr->obj->detail.t->minilines.first) {
         char *attr_value=NULL;

         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         attr_value = UtilStrDup(tmp_buf);
         if (attr_value == NULL) FailAllocMessage();
         ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         word_array[index] = UtilStrDup(attr_value);
         UtilFree(attr_value);
      } else {
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         word_array[index] = UtilStrDup(tmp_buf);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
      }
      if (word_array[index] == NULL) FailAllocMessage();
   }
   memset(&rpi, 0, sizeof(RandomPermutationInfo));
   RandomPermutation(&rpi, count);

   CleanTmpStr();

   need_to_free_tmp_buf = FALSE;
   tmp_buf = NULL;
   minilines = (&attr_ptr->obj->detail.t->minilines);
   tmp_buf = ConvertAttrNameFirstMiniLineToString(attr_ptr,
         &need_to_free_tmp_buf);
   if (!AppendToTmpStr(tmp_buf)) FailAllocMessage();
   if (need_to_free_tmp_buf) UtilFree(tmp_buf);

   for (i=0; i < count; i++) {
      AppendToTmpStr(word_array[rpi.state[i]]);
   }
   ReplaceAttrAllValues(attr_owner_obj, attr_ptr, &topTmpStr, &botTmpStr);
   CleanTmpStr();

   return TRUE;
}

/* --------------------- End of Exec Routines --------------------- */

static
char *ExecFreeArgv(need_raw_argv, argc, p_argv, p_raw_argv)
   int need_raw_argv, argc;
   char ***p_argv, ***p_raw_argv;
{
   register int i;

   for (i=0; i < argc; i++) {
      if ((*p_argv)[i] == NULL) break;
      free((*p_argv)[i]);
   }
   if (*p_argv != NULL) free(*p_argv);
   if (need_raw_argv) {
      for (i=0; i < argc; i++) {
         if ((*p_raw_argv)[i] == NULL) break;
         free((*p_raw_argv)[i]);
      }
      if (*p_raw_argv != NULL) free(*p_raw_argv);
   }
   return NULL;
}

static
void ShowExecStatus(need_raw_argv, argc, argv, raw_argv, func_name)
   int need_raw_argv, argc;
   char **argv, **raw_argv, *func_name;
{
   register int i, index;

   sprintf(execDummyStr, "EXEC: %s", func_name);
   index = strlen(execDummyStr);
   for (i=0; index < sizeof(execDummyStr)-7 && i < argc; i++) {
      int len=strlen(need_raw_argv ? raw_argv[i] : argv[i]);

      if (index+len+1 >= sizeof(execDummyStr)-7) {
         break;
      }
      if (i == 0) {
         execDummyStr[index++] = '(';
      } else {
         execDummyStr[index++] = ',';
      }
      strcpy(&execDummyStr[index], need_raw_argv ? raw_argv[i] : argv[i]);
      index += len;
   }
   if (i == argc) {
      if (argc == 0) execDummyStr[index++] = '(';
      execDummyStr[index++] = ')';
      execDummyStr[index] = '\0';
   } else {
      strcpy(&execDummyStr[index], ",...)");
   }
   SetStringStatus(execDummyStr);
   if (mainDisplay != NULL) XSync(mainDisplay, False);
}

static
char *ExecuteACommand(cmd_ptr, obj_ptr)
   char * cmd_ptr;
   struct ObjRec * obj_ptr;
{
   ExecInfo *pei=NULL;
   int i, tok_type, rc=TRUE, func_argc=0, need_raw_argv=FALSE;
   char buf[MAXSTRING<<2], **argv=NULL, **raw_argv=NULL;
   char *c_ptr=GetToken(cmd_ptr, buf, sizeof(buf), &tok_type);

   if (c_ptr == NULL) return NULL;
   if (*c_ptr == '\0') return c_ptr;
   for (pei=gExecInfo; pei->pfunc != NULL; pei++) {
      if (strcmp(pei->func_name, buf) == 0) {
         break;
      }
   }
   if (cmdLineTgrm2) {
      if (pei->pfunc == NULL) {
         pei = Tangram2GetExecInfo(buf);
      }
   }
   if (pei == NULL || pei->pfunc == NULL) {
      int len=strlen(buf);

      if (len > 0 && buf[len-1] == ' ') {
         sprintf(execDummyStr, TgLoadString(STID_UNRECOG_CMD_MAY_BE_BLANKS_ABT),
              buf);
      } else {
         sprintf(execDummyStr, TgLoadString(STID_UNRECOG_CMD_DOWNLOAD_TOOL),
              buf, TOOL_NAME, currentReleaseURL);
      }
      MsgBox(execDummyStr, TOOL_NAME, INFO_MB);
      return NULL;
   }
   func_argc = pei->func_argc;
   if (func_argc < 0) {
      func_argc = (-func_argc);
      need_raw_argv = TRUE;
   }
   if (func_argc > 0) {
      if (need_raw_argv) {
         raw_argv = (char**)malloc(func_argc*sizeof(char*));
         if (raw_argv == NULL) return (char*)(long)FailAllocMessage();
         for (i=0; i < func_argc; i++) raw_argv[i] = NULL;
      }
      argv = (char**)malloc(func_argc*sizeof(char*));
      if (argv == NULL) return (char*)(long)FailAllocMessage();
      for (i=0; i < func_argc; i++) argv[i] = NULL;
   }
   for (i=0; i < func_argc; i++) {
      if ((c_ptr=GetToken(c_ptr, buf, sizeof(buf), &tok_type)) == NULL ||
            tok_type == TOK_EMPTY) {
         BadCmd(pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
      if (i == 0) {
         if (tok_type != TOK_LEFT_P) {
            BadCmd(pei->func_name);
            return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
         }
      } else {
         if (tok_type != TOK_COMMA) {
            BadCmd(pei->func_name);
            return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
         }
      }
      if ((c_ptr=GetToken(c_ptr, buf, sizeof(buf), &tok_type)) == NULL ||
            tok_type == TOK_EMPTY) {
         BadCmd(pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
      if (tok_type != TOK_STR) {
         BadCmd(pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
      if (need_raw_argv) {
         raw_argv[i] = UtilStrDup(buf);
         if (raw_argv[i] == NULL) {
            FailAllocMessage();
            return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
         }
      }
      argv[i] = convert_str(buf, obj_ptr, pei->double_quotes_for_null);

      if (argv[i] == NULL) {
         BadAttr(buf, pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
   }
   if (func_argc == 0) {
      if ((c_ptr=GetToken(c_ptr, buf, sizeof(buf), &tok_type)) == NULL ||
            tok_type == TOK_EMPTY) {
         BadCmd(pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
      if (tok_type != TOK_LEFT_P) {
         BadCmd(pei->func_name);
         return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
      }
   }
   if ((c_ptr=GetToken(c_ptr, buf, sizeof(buf), &tok_type)) == NULL ||
         tok_type == TOK_EMPTY) {
      BadCmd(pei->func_name);
      return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
   }
   if (tok_type != TOK_RIGHT_P) {
      BadCmd(pei->func_name);
      return ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
   }

   ShowExecStatus(need_raw_argv, func_argc, argv, raw_argv, pei->func_name);
   if (func_argc > 0) {
      if (need_raw_argv) {
         rc = ((RawExecFunc*)(pei->pfunc))(argv, raw_argv, obj_ptr,
               pei->func_name);
      } else {
         rc = ((ExecFunc*)(pei->pfunc))(argv, obj_ptr, pei->func_name);
      }
   } else if (func_argc == 0) {
      gnAbortExec = FALSE;
      ((SimpleExecFunc*)(pei->pfunc))(obj_ptr, pei->func_name);
      if (gnAbortExec) rc = FALSE;
   }
   ExecFreeArgv(need_raw_argv, func_argc, &argv, &raw_argv);
   return (rc ? c_ptr : NULL);
}

static
int ExecuteCommands(cmd, obj_ptr)
   char *cmd;
   struct ObjRec *obj_ptr;
{
   int tok_type=TOK_INVALID, rc=TRUE;
   char *cmd_ptr=cmd;

   while (rc && *cmd_ptr != '\0') {
      gnSeenLeftParan = FALSE;
      if ((cmd_ptr=ExecuteACommand(cmd_ptr, obj_ptr)) == NULL) {
         rc = FALSE;
      } else if (*cmd_ptr != '\0') {
         char buf[MAXSTRING<<2];

         if (execNavigateBack) {
            break;
         } else if (warpToAttr != NULL) {
            break;
         } else if (OnlyBlanksLeft(cmd_ptr)) {
            break;
         } else if ((cmd_ptr=GetToken(cmd_ptr, buf, sizeof(buf), &tok_type)) ==
               NULL) {
            rc = FALSE;
         } else if (tok_type != TOK_SEMI) {
            MsgBox(TgLoadString(STID_UNRECOG_CMD_ABORT), TOOL_NAME, INFO_MB);
            rc = FALSE;
         }
      }
      if (rc && CheckExecInterrupt(NULL)) {
         Msg(TgLoadString(STID_USER_INTR));
         rc = FALSE;
         userAbortExec = TRUE;
      }
   }
   return rc;
}

static
int CommentLine(buf)
   char *buf;
{
   while (*buf == ' ') buf++;

   return (buf[0] == '/' && buf[1] == '/');
}

int DoExec(exec_attr, obj_ptr)
   struct AttrRec *exec_attr;
   struct ObjRec *obj_ptr;
{
   char *c_ptr, *cmd, *cmd_ptr;
   int cur_size=2*MAXSTRING, count=0, cmd_len=0, n;
   int first_time=TRUE, rc=TRUE;
   MiniLineInfo *pMiniLine=NULL;

   executingCommands = TRUE;
   escPressedWhileExecutingCommands = FALSE;

   cmd = (char*)malloc((cur_size+4)*sizeof(char));
   if (cmd == NULL) FailAllocMessage();
   cmd_ptr = cmd;
   for (pMiniLine=exec_attr->obj->detail.t->minilines.first;
         pMiniLine != NULL; pMiniLine=pMiniLine->next) {
      int need_to_free_tmp_buf=FALSE;
      char *tmp_buf=NULL;

      if (first_time) {
         char *attr_value=NULL;

         first_time = FALSE;
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         attr_value = UtilStrDup(tmp_buf);
         if (attr_value == NULL) FailAllocMessage();
         ParseAttrStr(tmp_buf, NULL, 0, attr_value, strlen(attr_value)+1);
         if (need_to_free_tmp_buf) UtilFree(tmp_buf);
         need_to_free_tmp_buf = TRUE;
         c_ptr = tmp_buf = attr_value;
      } else {
         tmp_buf = ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
         c_ptr = tmp_buf;
      }
      if (CommentLine(tmp_buf)) {
      } else if ((count=strlen(c_ptr)) != 0) {
         if (count+cmd_len >= cur_size) {
            n = cmd_ptr-cmd;
            cur_size += count+MAXSTRING;
            cmd = (char*)realloc(cmd, (cur_size+4)*sizeof(char));
            cmd_ptr = &cmd[n];
         }
         strncpy(cmd_ptr, c_ptr, count);
         cmd_ptr[count] = '\0';
         cmd_ptr += count;
         cmd_len += count;
         if (pMiniLine->next != NULL) {
            *cmd_ptr++ = ' ';
            *cmd_ptr = '\0';
            cmd_len++;
         }
      }
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }
   *cmd_ptr = '\0';
   justDupped = FALSE;
   execCurDepth++;
   StartCompositeCmd();

   rc = ExecuteCommands(cmd, obj_ptr);

   EndCompositeCmd();
   execCurDepth--;
   justDupped = FALSE;
   free(cmd);

   return rc;
}

void ResetExec(nBegin)
   int nBegin;
{
   static int sSavedXpmOutputVersion=(-1);
   static struct BBRec sTrimBBox;
   int i;

   executingCommands = FALSE;
   escPressedWhileExecutingCommands = FALSE;

   for (i=3; i < MAXEXECOPENFILES; i++) {
      if (gaOpenFileInfo[i].fp != NULL) {
         fclose(gaOpenFileInfo[i].fp);
         UtilFree(gaOpenFileInfo[i].fname);
      }
   }
   memset(gaOpenFileInfo, 0, MAXEXECOPENFILES*sizeof(struct OpenFileRec));
   gaOpenFileInfo[0].fp = stdin;
   gaOpenFileInfo[1].fp = stdout;
   gaOpenFileInfo[2].fp = stderr;
   rewind(stdin);

   if (gpExportClipBBox != NULL) {
      free(gpExportClipBBox);
      gpExportClipBBox = NULL;
   }
   if (nBegin) {
      userAbortExec = FALSE;
      execInterruptEnabled = TRUE;
      execInterruptQueued = FALSE;
      sSavedXpmOutputVersion = GetXpmOutputVersion();
      sTrimBBox.ltx = leftExportPixelTrim;
      sTrimBBox.lty = topExportPixelTrim;
      sTrimBBox.rbx = rightExportPixelTrim;
      sTrimBBox.rby = bottomExportPixelTrim;
      leftExportPixelTrim = topExportPixelTrim = rightExportPixelTrim =
            bottomExportPixelTrim = 0;
   } else {
      SetXpmOutputVersion(sSavedXpmOutputVersion);
      leftExportPixelTrim = sTrimBBox.ltx;
      topExportPixelTrim = sTrimBBox.lty;
      rightExportPixelTrim = sTrimBBox.rbx;
      bottomExportPixelTrim = sTrimBBox.rby;
   }
   ResetCreatePoly();
   ResetCreateGroup();
}

static
char *ReadACmdFromFile(fp, is_named_pipe, pn_quit)
   FILE *fp;
   int is_named_pipe, *pn_quit;
{
   char *buf=NULL;

   while (WaitForEvent(fp, FALSE, is_named_pipe, pn_quit, DO_ALL_X_EV, NULL,
         NULL) && (buf=UtilGetALine(fp)) != NULL) {
      int len=0;

      UtilTrimBlanks(buf);
      if (*buf == '\0') {
         UtilFree(buf);
         continue;
      } else if (strcmp(buf, ".\n") == 0) {
         UtilFree(buf);
         return NULL;
      }
      if (buf[0] == '/' && buf[1] == '/') {
         /* lines that start with "//" are comments */
         UtilFree(buf);
         continue;
      }
      len = strlen(buf);
      if (len > 0 && buf[len-1] == '\\') {
         char *buf1=NULL;
         int done=FALSE;

         buf[len-1] = ' ';
         while (!done && WaitForEvent(fp, FALSE, is_named_pipe, pn_quit,
               DO_ALL_X_EV, NULL, NULL) && (buf1=UtilGetALine(fp)) != NULL) {
            int len1=0;

            UtilTrimBlanks(buf1);
            len1 = strlen(buf1);
            if (len1 > 0 && buf1[len1-1] == '\\') {
               buf1[len1-1] = ' ';
            } else {
               done = TRUE;
            }
            buf = (char*)realloc(buf, len+len1+1);
            if (buf == NULL) FailAllocMessage();
            strcpy(&buf[len], buf1);
            len += len1;
            UtilFree(buf1);
         }
      }
      break;
   }
   if (buf == NULL && fp == stdin) {
      printf("\n");
   }
   return buf;
}

static
void DoExecCmdsFromFile(fp, is_named_pipe, pn_quit)
   FILE *fp;
   int is_named_pipe, *pn_quit;
{
   char prompt[MAXSTRING];

   if (fp == stdin) rewind(stdin);
   XSync(mainDisplay, False);

   executingCommands = TRUE;
   escPressedWhileExecutingCommands = FALSE;
   justDupped = FALSE;
   execCurDepth++;

   sprintf(prompt, "(%s) ", TOOL_NAME);
   UtilStrLower(prompt);

   while (!gnQuit && !gnStopCmdExecuted) {
      char *buf=NULL;
      int exec_rc=INVALID;

      if (fp == stdin) {
         printf("%s", prompt);
         fflush(stdout);
      }
      buf = ReadACmdFromFile(fp, is_named_pipe, pn_quit);
      if (buf == NULL) {
         if (fp == stdin) rewind(stdin);
         break;
      }
      if (fp == stdin || is_named_pipe) {
         gnDisableShortcuts = FALSE;
      }
      warpToAttr = NULL;
      exec_rc = ExecuteCommands(buf, NULL);
      if (exec_rc == TRUE && warpToAttr != NULL) {
         struct AttrRec *exec_attr=NULL;

         DoTeleport(warpToAttr);
         if ((exec_attr=FindFileAttrWithName("auto_exec=")) != NULL) {
            DoExecLoop(NULL, exec_attr);
         }
      }
      if (fp == stdin || is_named_pipe) {
         XSync(mainDisplay, False);
         gnDisableShortcuts = TRUE;
      }
      UtilFree(buf);
   }
   if (gnQuit) *pn_quit = TRUE;

   execCurDepth--;
   justDupped = FALSE;
}

void ExecCmdsFromFile(buf)
   char *buf;
{
   static int stnExecutingCmdsFromFile=FALSE;
   FILE *fp=NULL;
   char spec[MAXSTRING<<1], fname[MAXPATHLENGTH];
   int quit=FALSE, is_named_pipe=FALSE;

   *fname = '\0';
   gnStopCmdExecuted = FALSE;
   if (stnExecutingCmdsFromFile) {
      /* do not translate -- program constants */
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_NOT_DONE_WITH_PREV_FUNC),
            TOOL_NAME, "ExecCmdsFromFile()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (buf != NULL && strcmp(buf, "-1") != 0) {
      int len=0;

      UtilStrCpyN(spec, sizeof(spec), buf);
      UtilTrimBlanks(spec);
      len = strlen(spec);
      if (len > 0 && spec[len-1] == ')') spec[len-1] = '\0';
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_INTERN_CMD_FILE_NAME),
            TOOL_NAME);
      *spec = '\0';
      if (Dialog(gszMsgBox, NULL, spec) == INVALID) {
         return;
      }
   }
   UtilTrimBlanks(spec);
   if (strcmp(spec, "-") != 0) {
      struct stat stat_buf;

      if (*spec == DIR_SEP) {
         strcpy(fname, spec);
      } else {
         sprintf(fname, "%s%c%s", bootDir, DIR_SEP, spec);
      }
      if (stat(fname, &stat_buf) != 0) {
         int show_msg=TRUE;
#ifndef _NO_LSTAT
         if (lstat(fname, &stat_buf) == 0 &&
               ((stat_buf.st_mode & S_IFLNK) == S_IFLNK)) {
            show_msg = FALSE;
         }
#endif /* _NO_LSTAT */
         if (show_msg) {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_STAT_GIVEN_FILE),
                  fname);
            Msg(gszMsgBox);
            /* ummm... continue below anyway */
         }
#ifdef S_IFIFO
      } else if ((stat_buf.st_mode & S_IFIFO) == S_IFIFO) {
         is_named_pipe = TRUE;
#endif /* S_IFIFO */
      }
      if ((fp=fopen(fname, (is_named_pipe ? "r+" : "r"))) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      }
   } else {
      fp = stdin;
   }
   MakeQuiescent();

   stnExecutingCmdsFromFile = TRUE;
   if (fp == stdin || is_named_pipe) {
      gnDisableShortcuts = TRUE;
      RedrawDummyWindow2();
   }
   ResetExec(TRUE);
   if (fp == stdin) {
      DoExecCmdsFromFile(fp, FALSE, &quit);
      fprintf(stdin, "%s\n", TgLoadString(STID_FINISH_EXEC_CMD_FROM_STDIN));
   } else if (is_named_pipe) {
      for (;;) {
         DoExecCmdsFromFile(fp, TRUE, &quit);
         if (quit || gnStopCmdExecuted) break;
         fclose(fp);
         if ((fp=fopen(fname, "r+")) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
                  fname);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            break;
         }
      }
      if (fp != NULL) fclose(fp);
   } else {
      DoExecCmdsFromFile(fp, FALSE, &quit);
      fclose(fp);
   }
   if (fp == stdin || is_named_pipe) {
      gnDisableShortcuts = FALSE;
      RedrawDummyWindow2();
   }
   stnExecutingCmdsFromFile = FALSE;
   if (quit) {
      SendCommandToSelf(CMDID_QUIT, 0);
   }
}

void ExecCmdsFromTmp(tmp_fname)
   char *tmp_fname;
{
   static int stnExecutingCmdsFromTmp=FALSE;
   FILE *fp=NULL;
   int quit=FALSE;

   gnStopCmdExecuted = FALSE;
   if (stnExecutingCmdsFromTmp) {
      /* do not translate -- program constants */
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_NOT_DONE_WITH_PREV_FUNC),
            TOOL_NAME, "ExecCmdsFromTmp()");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   if ((fp=fopen(tmp_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   stnExecutingCmdsFromTmp = TRUE;

   ResetExec(TRUE);
   DoExecCmdsFromFile(fp, FALSE, &quit);
   fclose(fp);

   stnExecutingCmdsFromTmp = FALSE;
   if (quit) {
      SendCommandToSelf(CMDID_QUIT, 0);
   }
}

void StartExecCmdsFromFile()
{
   if (*cmdLineFileToExec == '\0') return;

   ExecCmdsFromFile(cmdLineFileToExec);
}

int BeginExecCommandsFromBuffer()
{
   ResetExec(TRUE);

   executingCommands = TRUE;
   escPressedWhileExecutingCommands = FALSE;
   justDupped = FALSE;
   execCurDepth++;

   return TRUE;
}

int ExecACommandFromBuffer(buf, pn_quit)
   char *buf;
   int *pn_quit;
{
   int exec_rc=INVALID;

   warpToAttr = NULL;
   exec_rc = ExecuteCommands(buf, NULL);
   if (exec_rc == TRUE && warpToAttr != NULL) {
      struct AttrRec *exec_attr=NULL;

      DoTeleport(warpToAttr);
      if ((exec_attr=FindFileAttrWithName("auto_exec=")) != NULL) {
         DoExecLoop(NULL, exec_attr);
      }
   }
   if (gnQuit) {
      if (pn_quit != NULL) *pn_quit = TRUE;

      execCurDepth--;
      justDupped = FALSE;

      SendCommandToSelf(CMDID_QUIT, 0);

      return FALSE;
   }
   return TRUE;
}

void EndExecCommandsFromBuffer()
{
   execCurDepth--;
   justDupped = FALSE;
   ShowCurChoiceMouseStatus(curChoice, 0, FALSE);
}

void CleanUpExec()
{
   if (gpExportClipBBox != NULL) {
      free(gpExportClipBBox);
      gpExportClipBBox = NULL;
   }
   if (cmdToExecAfterHyperJump != NULL) {
      free(cmdToExecAfterHyperJump);
      cmdToExecAfterHyperJump = NULL;
   }
}

static char defGsPath[]="gs";

void InitExec()
{
   char *c_ptr=NULL;

   gpExportClipBBox = NULL;
   executingCommands = FALSE;
   escPressedWhileExecutingCommands = FALSE;
   memset(gaOpenFileInfo, 0, MAXEXECOPENFILES*sizeof(struct OpenFileRec));
   cmdToExecAfterHyperJump = NULL;

   UtilStrCpyN(gsPath, sizeof(gsPath), defGsPath);
   if (!PRTGIF || cmdLineOpenDisplay) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GsPath")) != NULL) {
         if (!FindProgramInPath(c_ptr, NULL, TRUE)) {
            snprintf(gszMsgBox, sizeof(gszMsgBox),
                  TgLoadString(STID_INVALID_FILE_IN_X_DEFAULT),
                  c_ptr, TOOL_NAME, "GsPath");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else {
            UtilStrCpyN(gsPath, sizeof(gsPath), c_ptr);
         }
      }
   }
}

