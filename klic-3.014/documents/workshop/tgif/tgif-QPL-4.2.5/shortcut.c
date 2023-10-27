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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/shortcut.c,v 1.47 2011/06/09 16:11:42 cvsps Exp $
 */

#define _INCLUDE_FROM_SHORTCUT_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "choice.e"
#include "msg.e"
#include "setup.e"
#include "shortcut.e"
#include "strtbl.e"
#include "tangram2.e"
#include "util.e"

/*
 * Note:  the num_args field in shortCutXlateTbl is used by the shortcut
 *        mechanism.  This includes the call_one_arg_shortcut() internal
 *        command.
 */
/* do not translate -- program constants */
static struct ShortCutRec shortCutXlateTbl[] = {
/* Control Keys */
   { 'a', CNTRL_ONLY, "SelectAll()", 0, CMDID_SELECTALL },
   { 'b', CNTRL_ONLY, "Back()", 0, CMDID_BACK },
   { 'c', CNTRL_ONLY, "CopyToCutBuffer()", 0, CMDID_COPY },
   { 'd', CNTRL_ONLY, "Duplicate()", 0, CMDID_DUPLICATE },
   { 'e', CNTRL_ONLY, "PushCurChoice()", 0, CMDID_PUSHCURCHOICE },
   { 'f', CNTRL_ONLY, "Front()", 0, CMDID_FRONT },
   { 'g', CNTRL_ONLY, "Group()", 0, CMDID_GROUP },
   { 'i', CNTRL_ONLY, "Instantiate()", 0, CMDID_INSTANTIATE },
   { 'k', CNTRL_ONLY, "Pop()", 0, CMDID_POP },
   { 'l', CNTRL_ONLY, "AlignObjs()", 0, CMDID_ALIGNOBJS },
   { 'n', CNTRL_ONLY, "New()", 0, CMDID_NEW },
   { 'o', CNTRL_ONLY, "Open()", 1, CMDID_OPEN },
   { 'p', CNTRL_ONLY, "Print()", 0, CMDID_PRINT },
   { 'q', CNTRL_ONLY, "Quit()", 0, CMDID_QUIT },
   { 'r', CNTRL_ONLY, "Redraw()", 0, CMDID_REDRAW },
   { 's', CNTRL_ONLY, "Save()", 0, CMDID_SAVE },
   { 't', CNTRL_ONLY, "AlignToGrid()", 0, CMDID_ALIGNTOGRID },
   { 'u', CNTRL_ONLY, "UnGroup()", 0, CMDID_UNGROUP },
   { 'v', CNTRL_ONLY, "PasteFromCutBuffer()", 0, CMDID_PASTE },
   { 'w', CNTRL_ONLY, "DrawText()", 0, CMDID_DRAWTEXT },
   { '\0', 0, "Delete()", 0, CMDID_DELETE },
   { 'y', CNTRL_ONLY, "Redo()", 0, CMDID_REDO },
   { 'z', CNTRL_ONLY, "Undo()", 0, CMDID_UNDO },
   { ',', CNTRL_ONLY, "ScrollLeft()", 0, CMDID_SCROLLLEFT },
   { '.', CNTRL_ONLY, "ScrollRight()", 0, CMDID_SCROLLRIGHT },
   { '-', CNTRL_ONLY, "PrintWithCmd()", 0, CMDID_PRINTWITHCMD },
   { '1', CNTRL_ONLY, "ScreenCapture()", 0, CMDID_SCREENCAPTURE },
   { '2', CNTRL_ONLY, "FullScreenCapture()", 0, CMDID_FULLSCREENCAPTURE },
/* Meta Keys */
   { 'a', META_ONLY,  "AttachAttrs()", 0, CMDID_ATTACHATTRS },
   { 'b', META_ONLY,  "Probe()", 0, CMDID_PROBE },
   { 'c', META_ONLY,  "RotateCounter()", 0, CMDID_ROTATECOUNTER },
   { 'd', META_ONLY,  "-Grid()", 0, CMDID_DECGRID },
   { 'e', META_ONLY,  "AnimateSend()", 0, CMDID_ANIMATESEND },
   { 'f', META_ONLY,  "AnimateFlash()", 0, CMDID_ANIMATEFLASH },
   { 'g', META_ONLY,  "ToggleGrid()", 0, CMDID_TOGGLEGRID },
   { 'h', META_ONLY,  "FlipHorizontal()", 0, CMDID_FLIPHORIZONTAL },
   { 'i', META_ONLY,  "+Grid()", 0, CMDID_INCGRID },
   { 'j', META_ONLY,  "HideAttrName()", 0, CMDID_HIDEATTRNAME },
   { 'k', META_ONLY,  "SelectMode()", 0, CMDID_SELECTMODE },
   { 'l', META_ONLY,  "DistributeObjs()", 0, CMDID_DISTRIBUTEOBJS },
   { 'm', META_ONLY,  "Move/JustfyAttr()", 0, CMDID_MOVEJUSTIFYATTR },
   { 'n', META_ONLY,  "ShowAttrName()", 0, CMDID_SHOWATTRNAME },
   { 'o', META_ONLY,  "ZoomOut()", 0, CMDID_ZOOMOUT },
   { 'p', META_ONLY,  "Import()", 0, CMDID_IMPORT },
   { 'q', META_ONLY,  "DrawPoly()", 0, CMDID_DRAWPOLY },
   { 'r', META_ONLY,  "DrawBox()", 0, CMDID_DRAWBOX },
   { 's', META_ONLY,  "Solve()", 0, CMDID_SOLVE },
   { 't', META_ONLY,  "DetachAttrs()", 0, CMDID_DETACHATTRS },
   { 'u', META_ONLY,  "Animate()", 0, CMDID_ANIMATE },
   { 'v', META_ONLY,  "FlipVertical()", 0, CMDID_FLIPVERTICAL },
   { 'w', META_ONLY,  "RotateClockWise()", 0, CMDID_ROTATECLOCKWISE },
   { 'x', META_ONLY,  "Escape()", 0, CMDID_ESCAPE },
   { 'y', META_ONLY,  "Simulate()", 0, CMDID_SIMULATE },
   { 'z', META_ONLY,  "ZoomIn()", 0, CMDID_ZOOMIN },
   { '9', META_ONLY,  "SpecifyAnArc()", 0, CMDID_SPECIFYANARC },
   { '0', META_ONLY,  "Update()", 0, CMDID_UPDATE },
   { ',', META_ONLY,  "ScrollUp()", 0, CMDID_SCROLLUP },
   { '.', META_ONLY,  "ScrollDown()", 0, CMDID_SCROLLDOWN },
   { '-', META_ONLY,  "ShowAttr()", 0, CMDID_SHOWATTR },
   { '{', META_ONLY,  "AlignObjsTop()", 0, CMDID_ALIGNOBJSTOP },
   { '+', META_ONLY,  "AlignObjsMiddle()", 0, CMDID_ALIGNOBJSMIDDLE },
   { '}', META_ONLY,  "AlignObjsBottom()", 0, CMDID_ALIGNOBJSBOTTOM },
   { '[', META_ONLY,  "AlignObjsLeft()", 0, CMDID_ALIGNOBJSLEFT },
   { '=', META_ONLY,  "AlignObjsCenter()", 0, CMDID_ALIGNOBJSCENTER },
   { ']', META_ONLY,  "AlignObjsRight()", 0, CMDID_ALIGNOBJSRIGHT },
   { '"', META_ONLY,  "MakeRegPolygon()", 0, CMDID_MAKEREGPOLYGON },
   { '%', META_ONLY,  "SetReduction()", 0, CMDID_SETREDUCTION },
   { ':', META_ONLY,  "DefaultZoom()", 0, CMDID_DEFAULTZOOM },
   { '`', META_ONLY,  "ZoomWayOut()", 0, CMDID_ZOOMWAYOUT },
   { '~', META_ONLY,  "SaveSelectedAs()", 0, CMDID_SAVESELECTEDAS },
   { ';', META_ONLY,  "CutBit/Pixmap()", 0, CMDID_CUTBITPIXMAP },
   { '_', META_ONLY,  "AbutHorizontal()", 0, CMDID_ABUTHORIZONTAL },
   { '|', META_ONLY,  "AbutVertical()", 0, CMDID_ABUTVERTICAL },
   { '#', META_ONLY,  "BreakUpText()", 1, CMDID_BREAKUPTEXT },
   { '^', META_ONLY,  "ScrollToOrig()", 0, CMDID_SCROLLTOORIG },
   { '@', META_ONLY,  "ToggleMoveMode()", 0, CMDID_TOGGLEMOVEMODE },
   { '$', META_ONLY,  "SelectVertexMode()", 0, CMDID_SELECTVERTEXMODE },
   { '&', META_ONLY,  "AlignToPage()", 0, CMDID_ALIGNTOPAGE },
   { '*', META_ONLY,  "ChangeDomain()", 0, CMDID_CHANGEDOMAIN },
   { '(', META_ONLY,  "ImportEPSFile()", 1, CMDID_IMPORTEPSFILE },
   { ')', META_ONLY,  "PreciseScale()", 0, CMDID_PRECISESCALE },
   { '<', META_ONLY,  "Lock()", 0, CMDID_LOCK },
   { '>', META_ONLY,  "UnLock()", 0, CMDID_UNLOCK },
/* Control Meta Keys */
   { 'a', CNTRL_META, "AddPoint()", 0, CMDID_ADDPOINT },
   { 'b', CNTRL_META, "Bold()", 0, CMDID_BOLD },
   { 'c', CNTRL_META, "Center()", 0, CMDID_CENTER },
   { 'd', CNTRL_META, "DeletePoint()", 0, CMDID_DELETEPOINT },
   { 'e', CNTRL_META, "DrawRCBox()", 0, CMDID_DRAWRCBOX },
   { 'f', CNTRL_META, "InvertXBitmap()", 0, CMDID_INVERTXBITMAP },
   { 'g', CNTRL_META, "ToggleSnap()", 0, CMDID_TOGGLESNAP },
   { 'h', CNTRL_META, "HideAttr()", 0, CMDID_HIDEATTR },
   { 'i', CNTRL_META, "MakeIconic()", 0, CMDID_MAKEICONIC },
   { 'j', CNTRL_META, "UnMakeIconic()", 0, CMDID_UNMAKEICONIC },
   { 'k', CNTRL_META, "ToggleBW/ColorPS()", 0, CMDID_TOGGLEBWCOLORPS },
   { 'l', CNTRL_META, "Left()", 0, CMDID_LEFT },
   { 'm', CNTRL_META, "MakeSymbolic()", 0, CMDID_MAKESYMBOLIC },
   { 'n', CNTRL_META, "UnMakeSymbolic()", 0, CMDID_UNMAKESYMBOLIC },
   { 'o', CNTRL_META, "Roman()", 0, CMDID_ROMAN },
   { 'p', CNTRL_META, "BoldItalic()", 0, CMDID_BOLDITALIC },
   { 'q', CNTRL_META, "DrawPolygon()", 0, CMDID_DRAWPOLYGON },
   { 'r', CNTRL_META, "Right()", 0, CMDID_RIGHT },
   { 's', CNTRL_META, "SaveNew()", 1, CMDID_SAVENEW },
   { 't', CNTRL_META, "Italic()", 0, CMDID_ITALIC },
   { 'u', CNTRL_META, "UpdateSymbols()", 0, CMDID_UPDATESYMBOLS },
   { 'v', CNTRL_META, "DrawOval()", 0, CMDID_DRAWOVAL },
   { 'w', CNTRL_META, "ToggleLineType()", 0, CMDID_TOGGLELINETYPE },
   { 'x', CNTRL_META, "CyclePrintFormat()", 0, CMDID_CYCLEPRINFORMAT },
   { 'y', CNTRL_META, "Push()", 0, CMDID_PUSH },
   { 'z', CNTRL_META, "DrawArc()", 0, CMDID_DRAWARC },
   { '.', CNTRL_META, "ImportXBitmap()", 0, CMDID_IMPORTXBITMAP },
   { ',', CNTRL_META, "ImportXPixmap()", 0, CMDID_IMPORTXPIXMAP },
   { '-', CNTRL_META, "ToggleGridSystem()", 0, CMDID_TOGGLEGRIDSYSTEM },
   { '=', CNTRL_META, "FindAgain()", 0, CMDID_FINDAGAIN },
   { '5', CNTRL_META, "InsertRightSubscript()", 0, CMDID_INSERTRIGHTSUBSCRIPT },
   { '6', CNTRL_META, "InsertRightSuperscript()", 0,
                      CMDID_INSERTRIGHTSUPERSCRIPT },
   { '7', CNTRL_META, "ToggleEqAttrShown()", 0, CMDID_TOGGLEEQATTRSHOWN },
   { '8', CNTRL_META, "ToggleSlideShow()", 0, CMDID_TOGGLESLIDESHOW },
   { '9', CNTRL_META, "FindNoCase()", 0, CMDID_FINDNOCASE },
   { '0', CNTRL_META, "FindCaseSensitive()", 0, CMDID_FINDCASESENSITIVE },
/* Miscellaneous */
   { '\0', 0, "DrawCornerOval()", 0, CMDID_DRAWCORNEROVAL },
   { '\0', 0, "DrawCenterOval()", 0, CMDID_DRAWCENTEROVAL },
   { '\0', 0, "DrawEdgeOval()", 0, CMDID_DRAWEDGEOVAL },
   { '\0', 0, "DrawEdgeArc()", 0, CMDID_DRAWEDGEARC },
   { '\0', 0, "ScrollPageUp()", 0, CMDID_SCROLLPAGEUP },
   { '\0', 0, "ScrollPageDown()", 0, CMDID_SCROLLPAGEDOWN },
   { '\0', 0, "ScrollPageLeft()", 0, CMDID_SCROLLPAGELEFT },
   { '\0', 0, "ScrollPageRight()", 0, CMDID_SCROLLPAGERIGHT },
   { '\0', 0, "FlushUndoBuffer()", 0, CMDID_FLUSHUNDOBUFFER },
   { '\0', 0, "PrintMsgBuffer()", 0, CMDID_PRINTMSGBUFFER },
   { '\0', 0, "SaveOrigin()", 0, CMDID_SAVEORIGIN },
   { '\0', 0, "RestoreImageWH()", 0, CMDID_RESTOREIMAGEWH },
   { '\0', 0, "UpdateEPS()", 0, CMDID_UPDATEEPS },
   { '\0', 0, "ToggleMapShown()", 0, CMDID_TOGGLEMAPSHOWN },
   { '\0', 0, "ToggleUseGrayScale()", 0, CMDID_TOGGLEUSEGRAYSCALE },
   { '\0', 0, "FreeHandMode()", 0, CMDID_FREEHANDMODE },
   { '\0', 0, "SaveSymInLibrary()", 0, CMDID_SAVESYMINLIBRARY },
   { '\0', 0, "CenterAnEndPoint()", 0, CMDID_CENTERANENDPOINT },
   { '\0', 0, "CenterAVertex()", 0, CMDID_CENTERAVERTEX },
   { '\0', 0, "NextPage()", 0, CMDID_NEXTPAGE },
   { '\0', 0, "PrevPage()", 0, CMDID_PREVPAGE },
   { '\0', 0, "NamePages()", 0, CMDID_NAMEPAGES },
   { '\0', 0, "GotoPage()", 1, CMDID_GOTOPAGE },
   { '\0', 0, "AddPageBefore()", 0, CMDID_ADDPAGEBEFORE },
   { '\0', 0, "AddPageAfter()", 0, CMDID_ADDPAGEAFTER },
   { '\0', 0, "DeleteCurPage()", 0, CMDID_DELETECURPAGE },
   { '\0', 0, "TogglePageLineShown()", 0, CMDID_TOGGLEPAGELINESHOWN },
   { '\0', 0, "SpecifyDrawingSize()", 0, CMDID_SPECIFYDRAWINGSIZE },
   { '\0', 0, "PrintOnePage()", 0, CMDID_PRINTONEPAGE },
   { '\0', 0, "ToggleNamedAttrShown()", 1, INVALID },
   { '\0', 0, "AttachFileAttrs()", 0, CMDID_ATTACHFILEATTRS },
   { '\0', 0, "DetachFileAttrs()", 0, CMDID_DETACHFILEATTRS },
   { '\0', 0, "EditFileAttrs()", 0, CMDID_EDITFILEATTRS },
   { '\0', 0, "PrintSelectedObjs()", 0, CMDID_PRINTSELECTEDOBJS },
   { '\0', 0, "InputPolyPts()", 0, CMDID_INPUTPOLYPTS },
   { '\0', 0, "InputPolygonPts()", 0, CMDID_INPUTPOLYGONPTS },
   { '\0', 0, "EditAttrs()", 0, CMDID_EDITATTRS },
   { '\0', 0, "ConvertIntSpline()", 0, CMDID_CONVERTINTSPLINE },
   { '\0', 0, "PasteFromFile()", 0, CMDID_PASTEFROMFILE },
   { '\0', 0, "ToggleShowMeasurement()", 0, CMDID_TOGGLESHOWMEASUREMENT },
   { '\0', 0, "SetMeasureUnit()", 0, CMDID_SETMEASUREUNIT },
   { '\0', 0, "Cut()", 0, CMDID_CUT },
   { '\0', 0, "ToggleSmoothHinge()", 0, CMDID_TOGGLESMOOTHHINGE },
   { '\0', 0, "ToggleShowMenubar()", 0, CMDID_TOGGLESHOWMENUBAR },
   { '\0', 0, "ToggleShowStatus()", 0, CMDID_TOGGLESHOWSTATUS },
   { '\0', 0, "BrowseXBitmap()", 0, CMDID_BROWSEXBITMAP },
   { '\0', 0, "BrowseXPixmap()", 0, CMDID_BROWSEXPIXMAP },
   { '\0', 0, "SpecifyPaperSize()", 0, CMDID_SPECIFYPAPERSIZE },
   { '\0', 0, "ToggleOneMotionSelMove()", 0, CMDID_TOGGLEONEMOTIONSELMOVE },
   { '\0', 0, "GoBack()", 0, CMDID_GOBACK },
   { '\0', 0, "GoForward()", 0, CMDID_GOFORWARD },
   { '\0', 0, "RefreshCurrent()", 0, CMDID_REFRESHCURRENT },
   { '\0', 0, "HotList()", 0, CMDID_HOTLIST },
   { '\0', 0, "AddCurrentToHotList()", 0, CMDID_ADDCURRENTTOHOTLIST },
   { '\0', 0, "SessionHistory()", 0, CMDID_SESSIONHISTORY },
   { '\0', 0, "ToggleHyperSpace()", 0, CMDID_TOGGLEHYPERSPACE },
   { '\0', 0, "EmbedEPSFile()", 0, CMDID_EMBEDEPSFILE },
   { '\0', 0, "SetSelLineWidth()", 0, CMDID_SETSELLINEWIDTH },
   { '\0', 0, "AddColor()", 0, CMDID_ADDCOLOR },
   { '\0', 0, "ImportAttrs()", 0, CMDID_IMPORTATTRS },
   { '\0', 0, "ExportAttrs()", 0, CMDID_EXPORTATTRS },
   { '\0', 0, "MergeWithTable()", 0, CMDID_MERGEWITHTABLE },
   { '\0', 0, "ExportToTable()", 0, CMDID_EXPORTTOTABLE },
   { '\0', 0, "DeletePages()", 0, CMDID_DELETEPAGES },
   { '\0', 0, "PrintOneFilePerPage()", 0, CMDID_PRINTONEFILEPERPAGE },
   { '\0', 0, "ImportGIFFile()", 0, CMDID_IMPORTGIFFILE },
   { '\0', 0, "SetExportPixelTrim()", 0, CMDID_SETEXPORTPIXELTRIM },
   { '\0', 0, "ToggleColorLayers()", 0, CMDID_TOGGLECOLORLAYERS },
   { '\0', 0, "ToggleStretchableText()", 0, CMDID_TOGGLESTRETCHABLETEXT },
   { '\0', 0, "BreakUpBit/Pixmap()", 0, CMDID_BREAKUPBITPIXMAP },
   { '\0', 0, "LayoutOnArc()", 0, CMDID_LAYOUTONARC },
   { '\0', 0, "PreciseRotate()", 0, CMDID_PRECISEROTATE },
   { '\0', 0, "JoinPoly()", 0, CMDID_JOINPOLY },
   { '\0', 0, "CutPoly()", 0, CMDID_CUTPOLY },
   { '\0', 0, "GetBoundingBox()", 0, CMDID_GETBOUNDINGBOX },
   { '\0', 0, "SetTemplate()", 0, CMDID_SETTEMPLATE },
   { '\0', 0, "MakeGray()", 0, CMDID_MAKEGRAY },
   { '\0', 0, "InvertColor()", 0, CMDID_INVERTCOLOR },
   { '\0', 0, "InterpolateColor()", 0, CMDID_INTERPOLATECOLOR },
   { '\0', 0, "BrightenDarken()", 0, CMDID_BRIGHTENDARKEN },
   { '\0', 0, "ChangeSaturation()", 0, CMDID_CHANGESATURATION },
   { '\0', 0, "ChangeHue()", 0, CMDID_CHANGEHUE },
   { '\0', 0, "ContrastEnhance()", 0, CMDID_CONTRASTENHANCE },
   { '\0', 0, "ColorBalance()", 0, CMDID_COLORBALANCE },
   { '\0', 0, "Gamma()", 0, CMDID_GAMMA },
   { '\0', 0, "EdgeDetect()", 0, CMDID_EDGEDETECT },
   { '\0', 0, "Emboss()", 0, CMDID_EMBOSS },
   { '\0', 0, "ReduceColors()", 0, CMDID_REDUCECOLORS },
   { '\0', 0, "ReduceToPixmapColors()", 0, CMDID_REDUCETOPIXMAPCOLORS },
   { '\0', 0, "SetDefaultColorLevels()", 0, CMDID_SETDEFAULTCOLORLEVELS },
   { '\0', 0, "ReduceToDefaultColors()", 0, CMDID_REDUCETODEFAULTCOLORs },
   { '\0', 0, "DefaultErrorDiffuse()", 0, CMDID_DEFAULTERRORDIFFUSE },
   { '\0', 0, "Spread()", 0, CMDID_SPREAD },
   { '\0', 0, "Sharpen()", 0, CMDID_SHARPEN },
   { '\0', 0, "Blur3()", 0, CMDID_BLUR3 },
   { '\0', 0, "Blur5()", 0, CMDID_BLUR5 },
   { '\0', 0, "Blur7()", 0, CMDID_BLUR7 },
   { '\0', 0, "RunBggen()", 0, CMDID_RUNBGGEN },
   { '\0', 0, "CircularBggen()", 0, CMDID_CIRCULARBGGEN },
   { '\0', 0, "SimpleRectBggen()", 0, CMDID_SIMPLERECTBGGEN },
   { '\0', 0, "RegenerateImage()", 0, CMDID_REGENERATEIMAGE },
   { '\0', 0, "CropImage()", 0, CMDID_CROPIMAGE },
   { '\0', 0, "GetColor()", 0, CMDID_GETCOLOR },
   { '\0', 0, "ReplaceColor()", 0, CMDID_REPLACECOLOR },
   { '\0', 0, "FloodFill()", 0, CMDID_FLOODFILL },
   { '\0', 0, "CreateContour()", 0, CMDID_CREATECONTOUR },
   { '\0', 0, "Subtract()", 0, CMDID_SUBTRACT },
   { '\0', 0, "AlphaCombine()", 0, CMDID_ALPHACOMBINE },
   { '\0', 0, "XorColors()", 0, CMDID_XORCOLORS },
   { '\0', 0, "ImportOtherFile()", 0, CMDID_IMPORTOTHERFILE },
   { '\0', 0, "ImportOtherFileType()", 1, INVALID },
   { '\0', 0, "BrowseOther()", 0, CMDID_BROWSEOTHER },
   { '\0', 0, "BrowseOtherType()", 1, INVALID },
   { '\0', 0, "ToggleShowCrossHair()", 0, CMDID_TOGGLESHOWCROSSHAIR },
   { '\0', 0, "SetShapeShadow()", 0, CMDID_SETSHAPESHADOW },
   { '\0', 0, "NoTransform()", 0, CMDID_NOTRANSFORM },
   { '\0', 0, "About()", 0, CMDID_ABOUT },
   { '\0', 0, "Copyright()", 0, CMDID_COPYRIGHT },
   { '\0', 0, "SetSelFontSize()", 0, CMDID_SETSELFONTSIZE },
   { '\0', 0, "ZoomInAtCursor()", 0, CMDID_ZOOMINATCURSOR },
   { '\0', 0, "CenterAtCursor()", 0, CMDID_CENTERATCURSOR },
   { '\0', 0, "SetEditTextSize()", 0, CMDID_SETEDITTEXTSIZE },
   { '\0', 0, "SetTextRotation()", 0, CMDID_SETTEXTROTATION },
   { '\0', 0, "SetRotationIncrement()", 0, CMDID_SETROTATIONINCREMENT },
   { '\0', 0, "CurrentVersionInfo()", 0, CMDID_CURRENTVERSIONINFO },
   { '\0', 0, "LatestReleaseInfo()", 0, CMDID_LATESTRELEASEINFO },
   { '\0', 0, "VectorWarp()", 0, CMDID_VECTORWARP },
   { '\0', 0, "ConnectPins()", 0, CMDID_CONNECTPINS },
   { '\0', 0, "PasteCompoundText()", 0, CMDID_PASTECOMPOUNDTEXT },
   { '\0', 0, "CopyProperties()", 0, CMDID_COPYPROPERTIES },
   { '\0', 0, "SaveProperties()", 0, CMDID_SAVEPROPERTIES },
   { '\0', 0, "PasteProperties()", 0, CMDID_PASTEPROPERTIES },
   { '\0', 0, "RestoreProperties()", 0, CMDID_RESTOREPROPERTIES },
   { '\0', 0, "RotateShearMode()", 0, CMDID_ROTATESHEARMODE },
   { '\0', 0, "ChangeAllSelFill()", 0, CMDID_CHANGEALLSELFILL },
   { '\0', 0, "ChangeAllSelPen()", 0, CMDID_CHANGEALLSELPEN },
   { '\0', 0, "ChangeAllSelLineWidth()", 0, CMDID_CHANGEALLSELLINEWIDTH },
   { '\0', 0, "ChangeAllSelLineStyle()", 0, CMDID_CHANGEALLSELLINESTYLE },
   { '\0', 0, "ChangeAllSelLineType()", 0, CMDID_CHANGEALLSELLINETYPE },
   { '\0', 0, "ChangeAllSelLineDash()", 0, CMDID_CHANGEALLSELLINEDASH },
   { '\0', 0, "ChangeAllSelFont()", 0, CMDID_CHANGEALLSELFONT },
   { '\0', 0, "ChangeAllSelFontSize()", 0, CMDID_CHANGEALLSELFONTSIZE },
   { '\0', 0, "ChangeAllSelFontStyle()", 0, CMDID_CHANGEALLSELFONTSTYLE },
   { '\0', 0, "ChangeAllSelFontJust()", 0, CMDID_CHANGEALLSELFONTJUST },
   { '\0', 0, "ChangeAllSelColor()", 0, CMDID_CHANGEALLSELCOLOR },
   { '\0', 0, "LanscapeMode()", 0, CMDID_LANDSCAPEMODE },
   { '\0', 0, "PortraitMode()", 0, CMDID_PORTRAITMODE },
   { '\0', 0, "SetWhereToPrint()", 0, CMDID_SETWHERETOPRINT },
   { '\0', 0, "SetHoriAlign()", 0, CMDID_SETHORIZONTALALIGN },
   { '\0', 0, "SetVertAlign()", 0, CMDID_SETVERTICALALIGN },
   { '\0', 0, "SetMoveMode()", 0, CMDID_SETMOVEMODE },
   { '\0', 0, "SetStretchTextMode()", 0, CMDID_SETSTRETCHTEXTMODE },
   { '\0', 0, "CreateShape()", 0, CMDID_CREATESHAPE },
   { '\0', 0, "SetPageLayoutMode()", 0, CMDID_SETPAGELAYOUTMODE },
   { '\0', 0, "SetTransPatMode()", 0, CMDID_SETTRANSPATMODE },
   { '\0', 0, "ToggleShowMode()", 0, CMDID_TOGGLESHOWMODE },
   { '\0', 0, "SetSlideShowBorderColor()", 0, CMDID_SETSLIDESHOWBRDRCOLOR },
   { '\0', 0, "SetSlideShowWindowOffsets()", 0, CMDID_SETSLIDESHOWWINOFFSETS },
   { '\0', 0, "ExportXPixmapDeckToGIF()", 0, CMDID_EXPORTXPMDECKTOGIF },
   { '\0', 0, "ImportGIFToXPixmapDeck()", 0, CMDID_IMPORTGIFTOXPMDECK },
   { '\0', 0, "InsertThinSpace()", 0, CMDID_INSERTTHINSPACE },
   { '\0', 0, "InsertVerticalOffset()", 0, CMDID_INSERTVERTICALOFFSET },
   { '\0', 0, "RemoveVerticalOffset()", 0, CMDID_REMOVEVERTICALOFFSET },
   { '\0', 0, "InsertLeftSuperscript()", 0, CMDID_INSERTLEFTSUPERSCRIPT },
   { '\0', 0, "InsertLeftSubscript()", 0, CMDID_INSERTLEFTSUBSCRIPT },
   { '\0', 0, "InsertCenterSuperscript()", 0, CMDID_INSERTCENTERSUPERSCRIPT },
   { '\0', 0, "InsertCenterSubscript()", 0, CMDID_INSERTCENTERSUBSCRIPT },
   { '\0', 0, "SetScriptFraction()", 0, CMDID_SETSCRIPTFRACTION },
   { '\0', 0, "FakeUserAgent()", 1, CMDID_FAKEUSERAGENT },
   { '\0', 0, "FakeReferer()", 1, CMDID_FAKEREFERER },
   { '\0', 0, "ToggleKeepAlive()", 0, CMDID_TOGGLEKEEPALIVE },
   { '\0', 0, "SizeToWidest()", 0, CMDID_SIZETOWIDEST },
   { '\0', 0, "SizeToNarrowest()", 0, CMDID_SIZETONARROWEST },
   { '\0', 0, "SizeToTallest()", 0, CMDID_SIZETOTALLEST },
   { '\0', 0, "SizeToShortest()", 0, CMDID_SIZETOSHORTEST },
   { '\0', 0, "SizeToGivenWidthHeight()", 0, CMDID_SIZETOGIVENWIDTHHEIGHT },
   { '\0', 0, "SizeToGivenWidth()", 0, CMDID_SIZETOGIVENWIDTH },
   { '\0', 0, "SizeToGivenHeight()", 0, CMDID_SIZETOGIVENHEIGHT },
   { '\0', 0, "ExecCmdsFromFile()", 1, CMDID_EXECCMDSFROMFILE },
   { '\0', 0, "StartExecCmdsFromFile()", 0, CMDID_STARTEXECCMDSFROMFILE },
   { '\0', 0, "CopyPlainTextAsObject()", 0, CMDID_COPYPLAINTEXTASOBJECT },
   { '\0', 0, "SetTextFillPatternColor()", 0, CMDID_SETTEXTFILLPATTERNCOLOR },
   { '\0', 0, "AlignDirect()", 0, CMDID_ALIGNDIRECT },
   { '\0', 0, "DistributeDirect()", 0, CMDID_DISTRIBUTEDIRECT },
   { '\0', 0, "ToggleVisibleGridInSlideShow()",0,CMDID_VISIBLEGRIDINSLIDESHOW },
   { '\0', 0, "ChangeScrollMode()", 0, CMDID_CHANGESCROLLMODE },
   { '\0', 0, "SetAltEditTextBgColor()", 0, CMDID_SETALTEDITTEXTBGCOLOR },
   { '\0', 0, "SetAltEditTextHighlightColor()", 0,
               CMDID_SETALTEDITTEXTHIGHLIGHTCOLOR },
   { '\0', 0, "ToggleAltEditTextBgColor()", 0, CMDID_TOGGLEALTEDITTEXTBGCOLOR },
   { '\0', 0, "ChangeAllSelFontUnderline()",0,CMDID_CHANGEALLSELFONTUNDERLINE },
   { '\0', 0, "ChangeAllSelFontOverline()",0,CMDID_CHANGEALLSELFONTOVERLINE },
   { '\0', 0, "EditPageFileNames()", 0, CMDID_EDITPAGEFILENAMES },
   { '\0', 0, "ExportHalfToneBitmap()", 0, CMDID_EXPORTHALFTONEBITMAP },
   { '\0', 0, "ExportThresholdBitmap()", 0, CMDID_EXPORTTHRESHOLDBITMAP },
   { '\0', 0, "SetExportBitmapThreshold()", 1, CMDID_SETEXPORTBITMAPTHRESHOLD },
   { '\0', 0, "PreciseScaleEverything()", 0, CMDID_PRECISESCALEEVERYTHING },
   { '\0', 0, "SetPaperColor()", 0, CMDID_SETPAPERCOLOR },
   { '\0', 0, "DelayedFullScreenCapture()", 0, CMDID_DELAYEDFULLSCREENCAPTURE },
   { '\0', 0, "ToggleHideDuringCapture()", 0, CMDID_TOGGLEHIDEDURINGCAPTURE },
   { '\0', 0, "EditDomainPaths()", 0, CMDID_EDITDOMAINPATHS },
   { '\0', 0, "SelectDefaultDomain()", 0, CMDID_SELECTDEFAULTDOMAIN },
   { '\0', 0, "AddADomain()", 0, CMDID_ADDADOMAIN },
   { '\0', 0, "DeleteADomain()", 0, CMDID_DELETEADOMAIN },
   { '\0', 0, "ReloadDomainInfoFromX()", 0, CMDID_RELOADDOMAININFOFROMX },
   { '\0', 0, "ImportPNGFile()", 0, CMDID_IMPORTPNGFILE },
   { '\0', 0, "EditIndexedAttrInEditor()", 0, CMDID_EDITATTRINEDITOR },
   { '\0', 0, "GetProperty()", 0, CMDID_GETPROPERTY },
   { '\0', 0, "PeekDimension()", 0, CMDID_PEEKDIMENSION },
   { '\0', 0, "SetHtmlExportTemplate()", 0, CMDID_SETHTMLEXPORTTEMPLATE },
   { '\0', 0, "PrintPages()", 0, CMDID_PRINTPAGES },
   { '\0', 0, "GoHyperSpaceInSlideShow()",0, CMDID_GOHYPERSPACEINSLIDESHOW },
   { '\0', 0, "FreehandModeInSlideShow()",0, CMDID_FREEHANDMODEINSLIDESHOW },
   { '\0', 0, "OpenARecentlyUsedFile()", 0, CMDID_RECENTLYUSEDFILES },
   { '\0', 0, "MoveEditTextBox()", 0, CMDID_MOVEEDITTEXTBOX },
   { '\0', 0, "ReplaceGraphic()", 0, CMDID_REPLACEGRAPHIC },
   { '\0', 0, "ToggleShowMeasurementInTooltip()", 0,
               CMDID_TOGGLESHOWMEASUREMENTINTT },
   { '\0', 0, "ImportJPEGFile()", 0, CMDID_IMPORTJPEGFILE },
   { '\0', 0, "ToggleAutoEPSPreviewBitmap()", 0,
               CMDID_TOGGLEAUTOEPSPREVIEWBITMAP },
   { '\0', 0, "CreateThumbnails()", 0, CMDID_CREATETHUMBNAILS },
   { '\0', 0, "ConnectTwoPortsByAWire()", 0, CMDID_CONNECT2PORTSBYWIRE },
   { '\0', 0, "RenameSignalNameForAPort()", 0, CMDID_RENAMESIGNAMEFORAPORT },
   { '\0', 0, "ClearSignalNameForAPort()", 0, CMDID_CLEARSIGNAMEFORAPORT },
   { '\0', 0, "ToggleShowWireSignalName()", 0, CMDID_TOGGLESHOWWIRESIGNALNAME },
   { '\0', 0, "ToggleShowChoicebar()", 0, CMDID_TOGGLESHOWCHOICEBAR },
   { '\0', 0, "MergePortsWithAnObject()", 0, CMDID_MERGEPORTSWITHANOBJECT },
   { '\0', 0, "RenumberObjectIds()", 0, CMDID_RENUMBER_OBJECT_IDS },
   { '\0', 0, "RepeatConnectTwoPortsByAWire()", 0,
               CMDID_REPEATCONNECT2PORTSBYWIRE },
   { '\0', 0, "ConnectPortsToBroadcastWire()", 0,
               CMDID_CONNECTPORTSTOBROADCAST },
   { '\0', 0, "ImportMultipageTextFile()", 0, CMDID_IMPORTMULTIPAGETEXTFILE },
   { '\0', 0, "SetMarginsForImportMultipageTextFile()", 0,
               CMDID_SETMARGINSONIMPORTTEXT },
   { '\0', 0, "ToggleWordWrapDuringImportMultipageTextFile()", 0,
               CMDID_TOGGLEWORDWRAPONIMPORTTEXT },
   { '\0', 0, "HandleDataInMBuff()", 0, CMDID_DATA_IN_MBUFF },
   { '\0', 0, "BenchMark()", 0, CMDID_BENCHMARK },

   { '\0', 0, "ConvertToBezier()", 0, CMDID_CONVERTTOBEZIER },
   { '\0', 0, "SetBezierConvertNumSegs()", 1, CMDID_SETBEZIERCONVERTNUMSEGS },

   { '\0', 0, "AddSquareTickMarks()", 0, CMDID_ADDSQUARETICKMARKS },
   { '\0', 0, "AddTriangleTickMarks()", 0, CMDID_ADDTRIANGLETICKMARKS },
   { '\0', 0, "AddCircleTickMarks()", 0, CMDID_ADDCIRCLETICKMARKS },
   { '\0', 0, "AddXTickMarks()", 0, CMDID_ADDXTICKMARKS },
   { '\0', 0, "AddDiamondTickMarks()", 0, CMDID_ADDDIAMONDTICKMARKS },
   { '\0', 0, "AddBowtieTickMarks()", 0, CMDID_ADDBOWTIETICKMARKS },
   { '\0', 0, "AddInvTriangleTickMarks()", 0, CMDID_ADDINVTRIANGLETICKMARKS },
   { '\0', 0, "AddPlusTickMarks()", 0, CMDID_ADDPLUSTICKMARKS },
   { '\0', 0, "AddHourGlassTickMarks()", 0, CMDID_ADDHOURGLASSTICKMARKS },
   { '\0', 0, "SetTickMarkSize()", 1, CMDID_SETTICKMARKSIZE },
   { '\0', 0, "ToggleShowChat()", 0, CMDID_TOGGLESHOWCHAT },
   { '\0', 0, "SavePagesAs()", 0, CMDID_SAVEPAGESAS },
   { '\0', 0, "ImportPPMFile()", 0, CMDID_IMPORTPPMFILE },
   { '\0', 0, "AddPageBeforeCopyAll()", 0, CMDID_ADDPAGEBEFORECOPYALL },
   { '\0', 0, "AddPageAfterCopyAll()", 0, CMDID_ADDPAGEAFTERCOPYALL },
   { '\0', 0, "InsertHexOctalChar()", 0, CMDID_INSERTHEXOCTALCHAR },
   { '\0', 0, "ResetInputMethod()", 0, CMDID_RESETINMETHOD },
   { '\0', 0, "ImportPBMFile()", 0, CMDID_IMPORTPBMFILE },
   { '\0', 0, "ImportPGMFile()", 0, CMDID_IMPORTPGMFILE },
   { '\0', 0, "EditIndexedAttrGroupInEditor()", 0,
               CMDID_EDITATTRGROUPINEDITOR },
   { '\0', 0,  "LinkExtJPEGFile()", 0, CMDID_LINKEXTJPEGFILE },
   { '\0', 0, "NextSlide()", 0, CMDID_NEXTSLIDE },
   { '\0', 0, "PrevSlide()", 0, CMDID_PREVSLIDE },
   { '\0', 0, "SetObjectShadowColor()", 0, CMDID_SETOBJSHADOWCOLOR },
   { '\0', 0, "SetObjectShadowOffsets()", 0, CMDID_SETOBJSHADOWOFFSETS },
   { '\0', 0, "AddObjectShadow()", 0, CMDID_ADDOBJSHADOW },
   { '\0', 0, "RemoveObjectShadow()", 0, CMDID_REMOVEOBJSHADOW },
   { '\0', 0, "CopyDoubleByteString()", 0, CMDID_COPYUTF8 },
   { '\0', 0, "PasteDoubleByteString()", 0, CMDID_PASTEUTF8 },
   { '\0', 0, "ReduceToMobileWebSafeColors()", 0,
               CMDID_REDUCETOMOBILEWEBCOLORS },
   { '\0', 0, "CreatePixmapFromSelected()", 0, CMDID_CREATEPIXMAPFROMSEL },
   { '\0', 0, "ToggleAutoRotatePivot()", 0, CMDID_TOGGLEROTATEPIVOT },
   { '\0', 0, "SpecifyRotatePivot()", 0, CMDID_SPECIFYROTATEPIVOT },
   { '\0', 0, "ResetRotatePivot()", 0, CMDID_RESETROTATEPIVOT },
   { '\0', 0, "NextPolyRotationPivot()", 0, CMDID_NEXTPOLYROTATEPIVOT },
   { '\0', 0, "MoveRotationPivotToArcCenter()", 0,
               CMDID_MOVEROTATEPIVOTARCCNTR },
   { '\0', 0, "MoveRotatePivotCenter()", 0, CMDID_MOVEROTATEPIVOTCENTER },
   { '\0', 0, "MoveRotatePivotLeftTop()", 0, CMDID_MOVEROTATEPIVOTLT },
   { '\0', 0, "MoveRotatePivotRightTop()", 0, CMDID_MOVEROTATEPIVOTRT },
   { '\0', 0, "MoveRotatePivotLeftBottom()", 0, CMDID_MOVEROTATEPIVOTLB },
   { '\0', 0, "MoveRotatePivotRightBottom()", 0, CMDID_MOVEROTATEPIVOTRB },
   { '\0', 0, "MoveRotatePivotLeft()", 0, CMDID_MOVEROTATEPIVOTLEFT },
   { '\0', 0, "MoveRotatePivotRight()", 0, CMDID_MOVEROTATEPIVOTRIGHT },
   { '\0', 0, "MoveRotatePivotTop()", 0, CMDID_MOVEROTATEPIVOTTOP },
   { '\0', 0, "MoveRotatePivotBottom()", 0, CMDID_MOVEROTATEPIVOTBOTTOM },
   { '\0', 0, "ChooseRotatePivot()", 0, CMDID_CHOOSEROTATEPIVOT },
   { '\0', 0, "ChooseColor()", 0, CMDID_COLORWHEEL },
   { '\0', 0, "ExtendSegment()", 0, CMDID_EXTENDSEGMENT },
   { '\0', 0, "InsertSymbol()", 0, CMDID_INSERTSYMBOL },
   { '\0', 0, "ToggleRightMargin()", 0, CMDID_TOGGLERIGHTMARGIN },
   { '\0', 0, "SpecifyRightMargin()", 0, CMDID_SPECIFYRIGHTMARGIN },
   { '\0', 0, "ToggleTighterStructuredSplines()", 0,
               CMDID_TOGGLETIGHTERSPLINES },
   { '\0', 0, "ToggleFloodReplaceColorThreshold()", 0,
               CMDID_TOGGLEFLOODREPLACECOLORTHRESH },
   { '\0', 0, "SetFloodReplaceColorThreshold()", 0,
               CMDID_SETFLOODREPLACECOLORTHRESH },
   { '\0', 0, "RemoveTransparentPixel()", 0, CMDID_REMOVETRANSPIXEL },
   { '\0', 0, "ReplaceColorWithTrans()", 0, CMDID_REPLACECOLORWITHTRANS },
   { '\0', 0, "MakeBoxObjFromBoundingBox()", 0, CMDID_NEWRECTFROMBOUNDINGBOX },
   { '\0', 0, "MakeRCBoxObjFromBoundingBox()", 0, CMDID_NEWRCBOXFROMBOUNDINGBOX },
   { '\0', 0, "MakeOvalObjFromBoundingBox()", 0, CMDID_NEWOVALFROMBOUNDINGBOX },
   { '\0', 0, "", 0, 0 }
};

static int maxShortCuts=0;
static int shortCutIndex[256];
static char *shortCutArgs[256];

typedef struct tagFuncKeyShortCutInfo {
   char code;
   unsigned int state;
   char *name;
   char *arg;
} FuncKeyShortCutInfo;

static FuncKeyShortCutInfo funcKeyInfo[13];

static int shortCutCmdIdIndex[MAXCMDIDS-CMDID_BASE];

static
int FunctionKeySpec(buf, pn_code, pn_index_return)
   char *buf;
   int *pn_code, *pn_index_return;
{
   int value=0;
   char *colon_ptr=strchr(buf, ':');

   if (colon_ptr == NULL) return FALSE;
   *colon_ptr = '\0';
   if (buf[0] != 'F' || sscanf(&buf[1], "%d", &value) != 1) {
      *colon_ptr = ':';
      return FALSE;
   }
   *colon_ptr++ = ':';
   if (value < 1 || value > 12) return FALSE;

   *pn_code = value;
   *pn_index_return = (int)(colon_ptr-buf);

   return TRUE;
}

void InitShortCut()
{
   int i=0, num_cmdids=MAXCMDIDS-CMDID_BASE;
   char *c_ptr=NULL;

   for (i=0; i < 256; i++) {
      shortCutIndex[i] = INVALID;
      shortCutArgs[i] = NULL;
   }
   memset(funcKeyInfo, 0, 13*sizeof(FuncKeyShortCutInfo));

   maxShortCuts = 0;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ShortCuts")) != NULL) {
      int j=0, precise_match=0, ok=0, len=0;
      unsigned int code=0;
      char *saved_c_ptr=NULL, *tmp_c_ptr=NULL, *buf=NULL;

      len = strlen(c_ptr);
      if ((buf=(char *)malloc((len+1)*sizeof(char))) == NULL) {
         FailAllocMessage();
         return;
      }
      while (*c_ptr!=':' && *c_ptr!='!' && *c_ptr!='<' && *c_ptr!='\0') c_ptr++;
      while (*c_ptr != '\0') {
         saved_c_ptr = c_ptr;
         precise_match = FALSE;
         ok = TRUE;
         if ((*c_ptr==':' || *c_ptr=='!') && strncmp(&c_ptr[1],"<Key>",5)==0) {
            precise_match = TRUE;
            strcpy(buf, &c_ptr[6]);
         } else if (strncmp(c_ptr,"<Key>",5)==0) {
            strcpy(buf, &c_ptr[5]);
         } else {
            ok = FALSE;
         }
         if (ok) {
            tmp_c_ptr = buf;
            while (*tmp_c_ptr != ')' && *tmp_c_ptr != '\n' &&
                  *tmp_c_ptr != '\0') {
               tmp_c_ptr++;
            }
            if (*tmp_c_ptr == '\0') {
               ok = FALSE;
            } else {
               int func_index=0;

               if (*tmp_c_ptr == '\n') {
                  *tmp_c_ptr = '\0';
               } else {
                  *(++tmp_c_ptr) = '\0';
               }
               code = buf[0]&0xff;
               if (buf[0] != '\0' && buf[1] == ':' &&
                     ((code>0x20 && code<=0x7f) || (code>0xa0 && code<=0xff))) {
                  int found=TRUE;
                  char *paren_ptr=NULL;
   
                  if ((paren_ptr=strchr(&buf[2], '(')) == NULL) {
                     fprintf(stderr,
                           TgLoadString(STID_INVALID_SHORTCUT_ENTRY_SKIP), buf);
                     fprintf(stderr, "\n");
                     continue;
                  }
                  len = paren_ptr-(&buf[2]);
                  for (j=0; *(shortCutXlateTbl[j].name) != '\0'; j++) {
                     if (strncmp(shortCutXlateTbl[j].name, &buf[2], len) == 0 &&
                           shortCutXlateTbl[j].name[len] == '(') {
                        if (shortCutXlateTbl[j].num_args != 0) {
                           shortCutArgs[code] = (char*)malloc(
                                 (strlen(&buf[len+2])+1)*sizeof(char));
                           if (shortCutArgs[code] == NULL) FailAllocMessage();
                           strcpy(shortCutArgs[code], ++paren_ptr);
                        }
                        if (shortCutIndex[code] != INVALID) {
                           fprintf(stderr,
                                 TgLoadString(STID_DUP_SHORTCUT_ENTRY_FOR_CHAR),
                                 code);
                           fprintf(stderr, "\n");
                        }
                        shortCutIndex[code] = j;
                        if (!precise_match) {
                           if (*buf>='a' && *buf<='z') {
                              code = *buf-'a'+'A';
                              if (shortCutIndex[code] != INVALID) {
                                 fprintf(stderr, TgLoadString(
                                       STID_DUP_SHORTCUT_ENTRY_FOR_CHAR), code);
                                 fprintf(stderr, "\n");
                              }
                              shortCutIndex[code] = j;
                           } else if (*buf>='A' && *buf<='Z') {
                              code = *buf-'A'+'a';
                              if (shortCutIndex[code] != INVALID) {
                                 fprintf(stderr, TgLoadString(
                                       STID_DUP_SHORTCUT_ENTRY_FOR_CHAR), code);
                                 fprintf(stderr, "\n");
                              }
                              shortCutIndex[code] = j;
                           }
                        }
                        maxShortCuts++;
                        break;
                     }
                  }
                  if (*(shortCutXlateTbl[j].name) == '\0') {
                     if (cmdLineTgrm2) {
                        int num_args=0;
                        int index=ValidTangram2CmdName(&buf[2], len, &num_args);

                        if (index != 0) {
                           if (num_args != 0) {
                              shortCutArgs[code] = (char*)malloc(
                                    (strlen(&buf[len+2])+1)*sizeof(char));
                              if (shortCutArgs[code] == NULL) {
                                 FailAllocMessage();
                              }
                              strcpy(shortCutArgs[code], ++paren_ptr);
                           }
                           if (shortCutIndex[code] != INVALID) {
                              fprintf(stderr, TgLoadString(
                                    STID_DUP_SHORTCUT_ENTRY_FOR_CHAR), code);
                              fprintf(stderr, "\n");
                           }
                           shortCutIndex[code] = index;
                           if (!precise_match) {
                              if (*buf>='a' && *buf<='z') {
                                 code = *buf-'a'+'A';
                                 if (shortCutIndex[code] != INVALID) {
                                    fprintf(stderr, TgLoadString(
                                          STID_DUP_SHORTCUT_ENTRY_FOR_CHAR),
                                          code);
                                    fprintf(stderr, "\n");
                                 }
                                 shortCutIndex[code] = index;
                              } else if (*buf>='A' && *buf<='Z') {
                                 code = *buf-'A'+'a';
                                 if (shortCutIndex[code] != INVALID) {
                                    fprintf(stderr, TgLoadString(
                                          STID_DUP_SHORTCUT_ENTRY_FOR_CHAR),
                                          code);
                                    fprintf(stderr, "\n");
                                 }
                                 shortCutIndex[code] = index;
                              }
                           }
                           maxShortCuts++;
                        } else {
                           found = FALSE;
                        }
                     } else {
                        found = FALSE;
                     }
                  }
                  if (!found) {
                     fprintf(stderr,
                           TgLoadString(STID_INVALID_SHORTCUT_ENTRY_SKIP), buf);
                     fprintf(stderr, "\n");
                  }
               } else if (FunctionKeySpec(buf, (int*)&code, &func_index)) {
                  int found=FALSE;
                  char *paren_ptr=NULL;
   
                  if ((paren_ptr=strchr(&buf[func_index], '(')) == NULL) {
                     fprintf(stderr,
                           TgLoadString(STID_INVALID_SHORTCUT_ENTRY_SKIP), buf);
                     fprintf(stderr, "\n");
                     continue;
                  }
                  len = paren_ptr-(&buf[func_index]);
                  for (j=0; *(shortCutXlateTbl[j].name) != '\0'; j++) {
                     if (strncmp(shortCutXlateTbl[j].name, &buf[func_index],
                           len) == 0 && shortCutXlateTbl[j].name[len] == '(') {
                        if (funcKeyInfo[code].name != NULL) {
                           fprintf(stderr,
                                 TgLoadString(STID_DUP_SHORTCUT_FUNCTION_KEY),
                                 code);
                           fprintf(stderr, "\n");
                        }
                        funcKeyInfo[code].code = shortCutXlateTbl[j].code;
                        funcKeyInfo[code].state = shortCutXlateTbl[j].state;
                        funcKeyInfo[code].name =
                              UtilStrDup(shortCutXlateTbl[j].name);
                        if (funcKeyInfo[code].name == NULL) {
                           FailAllocMessage();
                        }
                        if (shortCutXlateTbl[j].num_args != 0) {
                           funcKeyInfo[code].arg = (char*)malloc(
                                 (strlen(&buf[len+func_index])+1)*sizeof(char));
                           if (funcKeyInfo[code].arg == NULL) {
                              FailAllocMessage();
                           }
                           strcpy(funcKeyInfo[code].arg, ++paren_ptr);
                        }
                        found = TRUE;
                        break;
                     }
                  }
                  if (!found) {
                     fprintf(stderr,
                           TgLoadString(STID_INVALID_SHORTCUT_ENTRY_SKIP), buf);
                     fprintf(stderr, "\n");
                  }
               } else {
                  fprintf(stderr,
                        TgLoadString(STID_INVALID_SHORTCUT_ENTRY_SKIP), buf);
                  fprintf(stderr, "\n");
               }
            }
         }
         if (ok) {
            while (*c_ptr!=')' && *c_ptr!='\0') c_ptr++;
            while (*c_ptr!=':' && *c_ptr!='!' && *c_ptr!='<' && *c_ptr!='\0') {
               c_ptr++;
            }
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_SHORTCUT_ENTRY),
                  saved_c_ptr);
            fprintf(stderr, "\n");
            break;
         }
      }
      free(buf);
   }
   for (i=0; i < num_cmdids; i++) {
      shortCutCmdIdIndex[i] = INVALID;
   }
   for (i=0; *(shortCutXlateTbl[i].name) != '\0'; i++) {
      int cmdid=shortCutXlateTbl[i].cmdid;

      if (cmdid != INVALID) {
         int index=cmdid-CMDID_BASE;

         if (shortCutCmdIdIndex[index] == INVALID) {
            shortCutCmdIdIndex[index] = i;
         } else {
            fprintf(stderr, TgLoadString(STID_WARN_DUP_CMDID), cmdid);
            fprintf(stderr, "\n");
         }
      }
   }
}

void CleanUpShortCut()
{
   register int i;

   for (i=0; i < 256; i++) {
      if (shortCutArgs[i] != NULL) {
         free(shortCutArgs[i]);
      }
   }
   for (i=0; i < 13; i++) {
      UtilFree(funcKeyInfo[i].name);
      UtilFree(funcKeyInfo[i].arg);
   }
}

int FetchShortCut(c, code, state, name, args, args_sz)
   int c, args_sz;
   char *code, **name, *args;
   unsigned int *state;
{
   if (shortCutIndex[c] == INVALID) return FALSE;

   if (cmdLineTgrm2 && (shortCutIndex[c] & CMDID_TANGRAM2_BASE) != 0) {
      /* for tangram-II, just do it */
      int num_args=0;

      if (FetchTangram2ShortCutNumArgs(shortCutIndex[c]-CMDID_TANGRAM2_BASE,
            &num_args) && num_args > 0) {
         strcpy(args, shortCutArgs[c]);
      } else {
         *args = '\0';
      }
      DoTangram2ShortCut(shortCutIndex[c]-CMDID_TANGRAM2_BASE, args);

      return FALSE;
   }
   *code = shortCutXlateTbl[shortCutIndex[c]].code;
   *state = shortCutXlateTbl[shortCutIndex[c]].state;
   *name = shortCutXlateTbl[shortCutIndex[c]].name;
   if (shortCutXlateTbl[shortCutIndex[c]].num_args == 0) {
      *args = '\0';
   } else {
      strcpy(args, shortCutArgs[c]);
   }
   return TRUE;
}

int FetchFuncKeyShortCut(c, code, state, name, args, args_sz)
   int c, args_sz;
   char *code, **name, *args;
   unsigned int *state;
{
   int index=0;
   char buf[MAXSTRING];

   switch (c) {
   case XK_F1: index=1; break;
   case XK_F2: index=2; break;
   case XK_F3: index=3; break;
   case XK_F4: index=4; break;
   case XK_F5: index=5; break;
   case XK_F6: index=6; break;
   case XK_F7: index=7; break;
   case XK_F8: index=8; break;
   case XK_F9: index=9; break;
   case XK_F10: index=10; break;
   case XK_F11: index=11; break;
   case XK_F12: index=12; break;
   default: return FALSE;
   }
   if (curChoice == DRAWTEXT) {
      sprintf(buf, "DrawTextFuncKey_F%1d", index);
      if (XGetDefault(mainDisplay, TOOL_NAME, buf) != NULL) {
         return FALSE;
      }
   }
   if (funcKeyInfo[index].name == NULL) return FALSE;

   *code = funcKeyInfo[index].code;
   *state = funcKeyInfo[index].state;
   *name = funcKeyInfo[index].name;

   if (funcKeyInfo[index].arg == NULL) {
      *args = '\0';
   } else {
      strcpy(args, funcKeyInfo[index].arg);
   }
   return TRUE;
}

int FetchCmdById(nCmdId, code, state, name, args)
   int nCmdId;
   char *code, **name, *args;
   unsigned int *state;
{
   int cmd_index=nCmdId-CMDID_BASE, table_index=INVALID;

   if (nCmdId >= MAXCMDIDS || shortCutCmdIdIndex[cmd_index] == INVALID) {
      return FALSE;
   }
   table_index = shortCutCmdIdIndex[cmd_index];

   if (shortCutXlateTbl[table_index].num_args != 0) {
      strcpy(args, "-1");
   }
   *code = shortCutXlateTbl[table_index].code;
   *state = shortCutXlateTbl[table_index].state;
   *name = shortCutXlateTbl[table_index].name;
   return TRUE;
}

int ValidShortCut(name, num_args, code, state)
   char *name, *code;
   int num_args;
   unsigned int *state;
{
   int j, len=strlen(name);

   if (len <= 2) return FALSE;
   for (j=0; *(shortCutXlateTbl[j].name) != '\0'; j++) {
      if (num_args == ((int)shortCutXlateTbl[j].num_args) &&
            strncmp(shortCutXlateTbl[j].name, name, len-2) == 0 &&
            shortCutXlateTbl[j].name[len] == '(' &&
            shortCutXlateTbl[j].name[len+1] == ')') {
         *code = shortCutXlateTbl[j].code;
         *state = shortCutXlateTbl[j].state;
         return TRUE;
      }
   }
   return FALSE;
}
