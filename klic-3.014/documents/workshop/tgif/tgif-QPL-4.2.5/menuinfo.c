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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/menuinfo.c,v 1.60 2011/06/09 15:55:15 cvsps Exp $
 */

#define _INCLUDE_FROM_MENUINFO_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "align.e"
#include "color.e"
#include "cutpaste.e"
#include "drawing.e"
#include "edit.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "help.e"
#include "imgproc.e"
#include "import.e"
#include "menu.e"
#include "menuinfo.e"
#include "names.e"
#include "navigate.e"
#include "page.e"
#include "pattern.e"
#include "raster.e"
#include "scroll.e"
#include "shape.e"
#include "special.e"
#include "stream.e"
#include "stretch.e"
#include "tangram2.e"
#include "text.e"
#include "xbitmap.e"

/* ------------------ color menus ------------------ */

TgMenuInfo colorMenuInfo={ TGMUTYPE_COLOR, NULL, CreateColorMenu };

/* ------------------ bitmap menus ------------------ */

static TgMenuItemInfo modeMenuItemInfo[] = {
   { (char*)(&choicePixmap[NOTHING]), NULL,
     N_("Enter select/move/resize object mode"), NULL, CMDID_SELECTMODE },
   { (char*)(&choicePixmap[DRAWTEXT]), NULL,
     N_("Enter draw text mode"), NULL, CMDID_DRAWTEXT },
   { (char*)(&choicePixmap[DRAWBOX]), NULL,
     N_("Enter draw rectangle mode"), NULL, CMDID_DRAWBOX },
   { (char*)(&choicePixmap[DRAWCORNEROVAL]), NULL,
     N_("Enter draw corner oval mode"), NULL, CMDID_DRAWCORNEROVAL },
   { (char*)(&choicePixmap[DRAWCENTEROVAL]), NULL,
     N_("Enter draw center oval mode"), NULL, CMDID_DRAWCENTEROVAL },
   { (char*)(&choicePixmap[DRAWEDGECIRCLE]), NULL,
     N_("Enter draw edge circle mode"), NULL, CMDID_DRAWEDGEOVAL },
   { (char*)(&choicePixmap[DRAWPOLY]), NULL,
     N_("Enter draw poly/open spline mode"), NULL, CMDID_DRAWPOLY },
   { (char*)(&choicePixmap[DRAWPOLYGON]), NULL,
     N_("Enter draw polygon/close spline mode"), NULL, CMDID_DRAWPOLYGON },
   { (char*)(&choicePixmap[DRAWARC]), NULL,
     N_("Enter draw arc mode (specify center first)"), NULL, CMDID_DRAWARC },
   { (char*)(&choicePixmap[DRAWEDGEARC]), NULL,
     N_("Enter draw arc mode (specify endpoints first)"), NULL,
     CMDID_DRAWEDGEARC },
   { (char*)(&choicePixmap[DRAWRCBOX]), NULL,
     N_("Enter draw rcbox mode"), NULL, CMDID_DRAWRCBOX },
   { (char*)(&choicePixmap[FREEHAND]), NULL,
     N_("Enter draw freehand poly/open spline mode"), NULL,
     CMDID_FREEHANDMODE },
   { (char*)(&choicePixmap[VERTEXMODE]), NULL,
     N_("Enter select/move vertices mode"), NULL, CMDID_SELECTVERTEXMODE },
   { (char*)(&choicePixmap[ROTATEMODE]), NULL,
     N_("Enter rotate/shear object mode"), NULL, CMDID_ROTATESHEARMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo modeMenuInfo={ TGMUTYPE_BITMAP, modeMenuItemInfo, CreateModeMenu };

TgMenuInfo fillMenuInfo={ TGMUTYPE_BITMAP, NULL, CreateFillMenu };
TgMenuInfo penMenuInfo={ TGMUTYPE_BITMAP, NULL, CreatePenMenu };
TgMenuInfo lineWidthMenuInfo={ TGMUTYPE_BITMAP, NULL, CreateLineWidthMenu };

static TgMenuItemInfo lineStyleMenuItemInfo[] = {
   { (char*)(&lineStylePixmap[LS_PLAIN]), NULL,
     N_("No arrows for a poly/open spline"), NULL,
     CMDID_CHANGEALLSELLINESTYLE },
   { (char*)(&lineStylePixmap[LS_RIGHT]), NULL,
     N_("An arrow at the end of a poly/open spline"),
     NULL, CMDID_CHANGEALLSELLINESTYLE },
   { (char*)(&lineStylePixmap[LS_LEFT]), NULL,
     N_("An arrow at the beginning of a poly/open spline"),
     NULL, CMDID_CHANGEALLSELLINESTYLE },
   { (char*)(&lineStylePixmap[LS_DOUBLE]), NULL,
     N_("Arrows at both ends of a poly/open spline"),
     NULL, CMDID_CHANGEALLSELLINESTYLE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo lineStyleMenuInfo={ TGMUTYPE_BITMAP, lineStyleMenuItemInfo,
                               CreateLineStyleMenu };

static TgMenuItemInfo lineTypeMenuItemInfo[] = {
   { (char*)(&lineTypePixmap[LT_STRAIGHT]), NULL,
     N_("Straight line segments"), NULL, CMDID_CHANGEALLSELLINETYPE },
   { (char*)(&lineTypePixmap[LT_SPLINE]), NULL,
     N_("Open/closed splines"), NULL, CMDID_CHANGEALLSELLINETYPE },
   { (char*)(&lineTypePixmap[LT_INTSPLINE]), NULL,
     N_("Interpolated splines"), NULL, CMDID_CHANGEALLSELLINETYPE },
   { (char*)(&lineTypePixmap[LT_STRUCT_SPLINE]), NULL,
     N_("Structured splines"), NULL, CMDID_CHANGEALLSELLINETYPE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo lineTypeMenuInfo={ TGMUTYPE_BITMAP, lineTypeMenuItemInfo,
                              CreateLineTypeMenu };

TgMenuInfo lineDashMenuInfo={ TGMUTYPE_BITMAP, NULL, CreateLineDashMenu };

static TgMenuItemInfo horiAlignMenuItemInfo[] = {
   { (char*)(&alignHoriPixmap[ALIGN_N]), NULL,
     N_("Set horizontal alignment to none"),
     NULL, CMDID_SETHORIZONTALALIGN },
   { (char*)(&alignHoriPixmap[ALIGN_L]), NULL,
     N_("Set horizontal alignment to left aligned"),
     NULL, CMDID_SETHORIZONTALALIGN },
   { (char*)(&alignHoriPixmap[ALIGN_C]), NULL,
     N_("Set horizontal alignment to center aligned"),
     NULL, CMDID_SETHORIZONTALALIGN },
   { (char*)(&alignHoriPixmap[ALIGN_R]), NULL,
     N_("Set horizontal alignment to right aligned"),
     NULL, CMDID_SETHORIZONTALALIGN },
   { (char*)(&alignHoriPixmap[ALIGN_S]), NULL,
     N_("Set horizontal distribution to equal-spacing"),
     NULL, CMDID_SETHORIZONTALALIGN },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo horiAlignMenuInfo={ TGMUTYPE_BITMAP, horiAlignMenuItemInfo,
                               CreateHoriAlignMenu };

static TgMenuItemInfo vertAlignMenuItemInfo[] = {
   { (char*)(&alignVertPixmap[ALIGN_N]), NULL,
     N_("Set vertical alignment to none"),
     NULL, CMDID_SETVERTICALALIGN },
   { (char*)(&alignVertPixmap[ALIGN_T]), NULL,
     N_("Set vertical alignment to top aligned"),
     NULL, CMDID_SETVERTICALALIGN },
   { (char*)(&alignVertPixmap[ALIGN_M]), NULL,
     N_("Set vertical alignment to middle aligned"),
     NULL, CMDID_SETVERTICALALIGN },
   { (char*)(&alignVertPixmap[ALIGN_B]), NULL,
     N_("Set vertical alignment to bottom aligned"),
     NULL, CMDID_SETVERTICALALIGN },
   { (char*)(&alignVertPixmap[ALIGN_S]), NULL,
     N_("Set vertical distribution to equal-spacing"),
     NULL, CMDID_SETVERTICALALIGN },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo vertAlignMenuInfo={ TGMUTYPE_BITMAP, vertAlignMenuItemInfo,
                               CreateVertAlignMenu };

static TgMenuItemInfo moveModeMenuItemInfo[] = {
   { (char*)(&moveModePixmap[CONST_MOVE]), NULL,
     N_("Set constrained move mode"), NULL, CMDID_SETMOVEMODE },
   { (char*)(&moveModePixmap[UNCONST_MOVE]), NULL,
     N_("Set unconstrained move mode"), NULL, CMDID_SETMOVEMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo moveModeMenuInfo={ TGMUTYPE_BITMAP, moveModeMenuItemInfo,
                              CreateMoveModeMenu };

static TgMenuItemInfo stretchableTextModeMenuItemInfo[] = {
   { (char*)(&stretchableModePixmap[NO_STRETCHABLE_TEXT]), NULL,
     N_("Make text objects not stretchable"), NULL, CMDID_SETSTRETCHTEXTMODE },
   { (char*)(&stretchableModePixmap[STRETCHABLE_TEXT]), NULL,
     N_("Make text objects stretchable"), NULL, CMDID_SETSTRETCHTEXTMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo stretchableTextModeMenuInfo={ TGMUTYPE_BITMAP,
                                         stretchableTextModeMenuItemInfo,
                                         CreateStretchableTextModeMenu };

static TgMenuItemInfo objShadowMenuItemInfo[] = {
   { N_("Add Object Shadow"), NULL,
     N_("Add shadows to selected objects"), NULL, CMDID_ADDOBJSHADOW },
   { N_("Remove Object Shadow"), NULL,
     N_("Remove shadows from selected objects"), NULL, CMDID_REMOVEOBJSHADOW },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Object Shadow Color..."), NULL,
     N_("Set the shadow color to be used for creating object shadows"), NULL,
     CMDID_SETOBJSHADOWCOLOR },
   { N_("Set Object Shadow Offsets..."), NULL,
     N_("Set the x and y offset for generic object shadow"), NULL,
     CMDID_SETOBJSHADOWOFFSETS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo objShadowMenuInfo={ TGMUTYPE_TEXT,
                                      objShadowMenuItemInfo,
                                      TgCreateMenuFromMenuInfo };

TgMenuInfo shapeMenuInfo={ TGMUTYPE_BITMAP, NULL, CreateShapeMenu };

static TgMenuItemInfo pageLayoutMenuItemInfo[] = {
   { (char*)(&pageLayoutPixmap[PAGE_STACK]), NULL,
     N_("Stacked page mode"), NULL, CMDID_SETPAGELAYOUTMODE },
   { (char*)(&pageLayoutPixmap[PAGE_TILE]), NULL,
     N_("Tiled page mode"), NULL, CMDID_SETPAGELAYOUTMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo pageLayoutMenuInfo={ TGMUTYPE_BITMAP, pageLayoutMenuItemInfo,
                                CreatePageLayoutMenu };

static TgMenuItemInfo transPatModeMenuItemInfo[] = {
   { (char*)(&transPatPixmap[NO_TRANSPAT_MODE]), NULL,
     N_("Make fill and pen patterns opaque"), NULL, CMDID_SETTRANSPATMODE },
   { (char*)(&transPatPixmap[TRANSPAT_MODE]), NULL,
     N_("Make fill and pen patterns transparent"), NULL,
     CMDID_SETTRANSPATMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo transPatModeMenuInfo={ TGMUTYPE_BITMAP, transPatModeMenuItemInfo,
                                  CreateTransPatModeMenu };

static TgMenuItemInfo pageStyleMenuItemInfo[] = {
   { N_("Portrait"), NULL, N_("Portrait page style"), NULL,
     CMDID_PORTRAITMODE },
   { N_("Landscape"), NULL, N_("Lanscape page style"), NULL,
     CMDID_LANDSCAPEMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo pageStyleMenuInfo={ TGMUTYPE_TEXT, pageStyleMenuItemInfo,
                               CreatePageStyleMenu };

/* ------------------ text menus ------------------ */

TgMenuInfo fontMenuInfo={ TGMUTYPE_TEXT, NULL, CreateFontMenu };

static TgMenuItemInfo fontStyleMenuItemInfo[] = {
   { N_("Roman"), "Alt+Cntrl+o", N_("Roman text style"), NULL,
     CMDID_CHANGEALLSELFONTSTYLE },
   { N_("Bold"), "Alt+Cntrl+b", N_("Bold text style"), NULL,
     CMDID_CHANGEALLSELFONTSTYLE },
   { N_("Italic"), "Alt+Cntrl+t", N_("Italic text style"), NULL,
     CMDID_CHANGEALLSELFONTSTYLE },
   { N_("BoldItalic"), "Alt+Cntrl+p", N_("Bold-italic text style"), NULL,
     CMDID_CHANGEALLSELFONTSTYLE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Left"), "Alt+Cntrl+l", N_("Left justified text"), NULL,
     CMDID_CHANGEALLSELFONTJUST },
   { N_("Center"), "Alt+Cntrl+c", N_("Center justified text"), NULL,
     CMDID_CHANGEALLSELFONTJUST },
   { N_("Right"), "Alt+Cntrl+r", N_("Right justified text"), NULL,
     CMDID_CHANGEALLSELFONTJUST },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("No Underline"), NULL, N_("No underlined text"), NULL,
     CMDID_CHANGEALLSELFONTUNDERLINE },
   { N_("Underline"), NULL, N_("Underlined text"), NULL,
     CMDID_CHANGEALLSELFONTUNDERLINE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("No Overline"), NULL, N_("No overlined text"), NULL,
     CMDID_CHANGEALLSELFONTOVERLINE },
   { N_("Overline"), NULL, N_("Overlined text"), NULL,
     CMDID_CHANGEALLSELFONTOVERLINE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo fontStyleMenuInfo={ TGMUTYPE_TEXT, fontStyleMenuItemInfo,
                               CreateFontStyleMenu };

TgMenuInfo fontSizeMenuInfo={ TGMUTYPE_TEXT, NULL, CreateFontSizeMenu };

static TgMenuItemInfo stackedPageMenuItemInfo[] = {
   { N_("Next Page"), NULL, N_("Go to next page"), NULL, CMDID_NEXTPAGE },
   { N_("Prev Page"), NULL, N_("Go to previous page"), NULL, CMDID_PREVPAGE },
   { N_("Goto Page..."), NULL, N_("Go to a specified page"), NULL,
     CMDID_GOTOPAGE },
   { N_("Name Pages..."), NULL, N_("Assign names to pages"), NULL,
     CMDID_NAMEPAGES },
   { N_("Edit Page File Names..."), NULL,
     N_("Assign file names to pages to be used for PrintOneFilePerPage()"),
     NULL, CMDID_EDITPAGEFILENAMES },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Add Page Before..."), NULL, N_("Add a page before the current page"),
     NULL, CMDID_ADDPAGEBEFORE },
   { N_("Add Page After..."), NULL,
     N_("Add a page after the current page"), NULL, CMDID_ADDPAGEAFTER },
   { N_("Add Page Before (Copy All)..."), NULL,
     N_("Add a page before the current page and copy everything on the current page to the new page"),
     NULL, CMDID_ADDPAGEBEFORECOPYALL },
   { N_("Add Page After (Copy All)..."), NULL,
     N_("Add a page after the current page and copy everything on the current page to the new page"),
     NULL, CMDID_ADDPAGEAFTERCOPYALL },
   { N_("Delete Current Page"), NULL, N_("Delete the current page"), NULL,
     CMDID_DELETECURPAGE },
   { N_("Delete Pages..."), NULL, N_("Delete user-specified pages"), NULL,
     CMDID_DELETEPAGES },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Print One Page"), NULL, N_("Print the curret page"), NULL,
     CMDID_PRINTONEPAGE },
   { N_("Print Pages..."), NULL, N_("Print user-specified pages"), NULL,
     CMDID_PRINTPAGES },
   { N_("Print One File Per Page..."), NULL,
     N_("Print into a separate file for each page"),
     NULL, CMDID_PRINTONEFILEPERPAGE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Specify Paper Size..."), NULL,
     N_("Set the physical size of the paper"), NULL, CMDID_SPECIFYPAPERSIZE },
   { N_("Set Paper Color..."), NULL, N_("Set the color of the paper"), NULL,
     CMDID_SETPAPERCOLOR },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo stackedPageMenuInfo={ TGMUTYPE_TEXT, stackedPageMenuItemInfo,
                                 TgCreateMenuFromMenuInfo };

static TgMenuItemInfo tiledPageMenuItemInfo[] = {
   { N_("Toggle Page Line Shown"), NULL,
     N_("Toggle the showing of page boundaries"), NULL,
     CMDID_TOGGLEPAGELINESHOWN },
   { N_("Specify Drawing Size..."), NULL, N_("Change the drawing size"), NULL,
     CMDID_SPECIFYDRAWINGSIZE },
   { N_("Print One Page"), NULL, N_("Select and print one of the pages"), NULL,
     CMDID_PRINTONEPAGE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Specify Paper Size..."), NULL,
     N_("Set the physical size of the paper"), NULL, CMDID_SPECIFYPAPERSIZE },
   { N_("Set Paper Color..."), NULL, N_("Set the color of the paper"), NULL,
     CMDID_SETPAPERCOLOR },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo tiledPageMenuInfo={ TGMUTYPE_TEXT, tiledPageMenuItemInfo,
                               TgCreateMenuFromMenuInfo };

static TgMenuItemInfo pageMenuItemInfo[] = {
   { N_("Stacked Page"), TGMUITEM_PINNABLESUBMENU,
     N_("StackedPage Menu"), &stackedPageMenuInfo, MENU_STACKEDPAGE },
   { N_("Tiled Page"), TGMUITEM_PINNABLESUBMENU,
     N_("TiledPage Menu"), &tiledPageMenuInfo, MENU_TILEDPAGE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo pageMenuInfo={ TGMUTYPE_TEXT, pageMenuItemInfo, CreatePageMenu };

static TgMenuItemInfo reduceColorMenuItemInfo[] = {
   { N_("Reduce Colors..."), NULL,
     N_("Reduce the number of colors of a selected image"),
     NULL, CMDID_REDUCECOLORS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Default Color Levels..."), NULL,
     N_("Set the number of R G B color levels for the ``standard colors''"),
     NULL, CMDID_SETDEFAULTCOLORLEVELS },
   { N_("Reduce To Default Colors"), NULL,
     N_("Reduce the colors of a selected image to standard colors"), NULL,
     CMDID_REDUCETODEFAULTCOLORs },
   { N_("Default Error Diffuse"), NULL,
     N_("Error diffuse to reduce colors of a selected image to standard colors"),
     NULL, CMDID_DEFAULTERRORDIFFUSE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Reduce To Pixmap Colors..."), NULL,
     N_("Reduce the colors of a selected image to a the colors in an XPM file"),
     NULL, CMDID_REDUCETOPIXMAPCOLORS },
   { N_("Reduce To Mobileweb Safe Colors"), NULL,
     N_("Reduce the colors of a selected image to colors that are \"MobileWeb safe\" (6 shades of red/green/blue each)"),
     NULL, CMDID_REDUCETOMOBILEWEBCOLORS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo reduceColorMenuInfo={ TGMUTYPE_TEXT, reduceColorMenuItemInfo,
                                        TgCreateMenuFromMenuInfo };

static TgMenuItemInfo effectsMenuItemInfo[] = {
   { N_("Edge Detect"), NULL, N_("Perform edge-detection on a selected image"),
     NULL, CMDID_EDGEDETECT },
   { N_("Emboss"), NULL, N_("Convert a selected image to gray and then emboss"),
     NULL, CMDID_EMBOSS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Spread..."), NULL, N_("Spread the pixels of a selected image around"),
     NULL, CMDID_SPREAD },
   { N_("Sharpen"), NULL, N_("Sharpen a selected image"), NULL, CMDID_SHARPEN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Blur (3x3)"), NULL, N_("Blur a selected image using a 3 by 3 filter"),
     NULL, CMDID_BLUR3 },
   { N_("Blur (5x5)"), NULL, N_("Blur a selected image using a 5 by 5 filter"),
     NULL, CMDID_BLUR5 },
   { N_("Blur (7x7)"), NULL, N_("Blur a selected image using a 7 by 7 filter"),
     NULL, CMDID_BLUR7 },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo effectsMenuInfo={ TGMUTYPE_TEXT, effectsMenuItemInfo,
                                    TgCreateMenuFromMenuInfo };

static TgMenuItemInfo generateMenuItemInfo[] = {
   { N_("Run Bggen..."), NULL,
     N_("Run 'bggen' to create an image using the size of a selected image"),
     NULL, CMDID_RUNBGGEN },
   { N_("Circular Bggen..."), NULL,
     N_("Create a gray circular image using the size of a selected image"),
     NULL, CMDID_CIRCULARBGGEN },
   { N_("Simple Rect Bggen..."), NULL,
     N_("Create a single-color rectanglar image using the size of selected image"),
     NULL, CMDID_SIMPLERECTBGGEN },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo generateMenuInfo={ TGMUTYPE_TEXT, generateMenuItemInfo,
                                     TgCreateMenuFromMenuInfo };

static TgMenuItemInfo fillReplaceMenuItemInfo[] = {
   { N_("Replace Color"), NULL,
     N_("Replace a color in a selected image with the current color"), NULL,
     CMDID_REPLACECOLOR },
   { N_("Replace Color with Transparent"), NULL,
     N_("Replace a color in a selected image with a transparent color"), NULL,
     CMDID_REPLACECOLORWITHTRANS },
   { N_("Flood Fill"), NULL,
     N_("Flood-fill a selected image with the current color"),
     NULL, CMDID_FLOODFILL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Enable Threshold-based Flood and Replace Color"), NULL,
     N_("Enable flood fill and replace color to apply to colors similar to the selected pixel (+/- a threshold)"),
     NULL, CMDID_TOGGLEFLOODREPLACECOLORTHRESH },
   { N_("Set Flood and Replace Color Threshold"), NULL,
     N_("Set flood fill and replace color threshold"),
     NULL, CMDID_SETFLOODREPLACECOLORTHRESH },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo fillReplaceMenuInfo={ TGMUTYPE_TEXT, fillReplaceMenuItemInfo,
                                        CreateImageProcMenu };
static TgMenuItemInfo imageProcMenuItemInfo[] = {
   { N_("Make Gray"), NULL, N_("Convert a selected image into gray-scale"),
     NULL, CMDID_MAKEGRAY },
   { N_("Brighten/Darken..."), NULL, N_("Brighten or darken a selected image"),
     NULL, CMDID_BRIGHTENDARKEN },
   { N_("Contrast Enhance..."), NULL, N_("Adjust contrast of a selected image"),
     NULL, CMDID_CONTRASTENHANCE },
   { N_("Gamma Correct..."), NULL,
     N_("Apply gamma correction a selected image"), NULL, CMDID_GAMMA },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Change Saturation..."), NULL,
     N_("Change saturation of a selected image"), NULL,
     CMDID_CHANGESATURATION },
   { N_("Change Hue..."), NULL, N_("Change hue of a selected image"), NULL,
     CMDID_CHANGEHUE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Invert Color"), NULL,
     N_("Subtract every pixel of a selected image from White (in RGB-model)"),
     NULL, CMDID_INVERTCOLOR },
   { N_("Interpolate Color..."), NULL,
     N_("Map pixel intensities of a selected image between two specified colors"),
     NULL, CMDID_INTERPOLATECOLOR },
   { N_("Color Balance..."), NULL,
     N_("Balance the RGB components of a selected image"),
     NULL, CMDID_COLORBALANCE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Reduce Number Of Colors"), TGMUITEM_SUBMENU,
     N_("Reduce number of colors submenu >>>"), &reduceColorMenuInfo, INVALID },
   { N_("Effects"), TGMUITEM_SUBMENU,
     N_("Effects submenu >>>"), &effectsMenuInfo, INVALID },
   { N_("Generate"), TGMUITEM_SUBMENU,
     N_("Generate submenu >>>"), &generateMenuInfo, INVALID },
   { N_("Flood Fill / Replace Colors"), TGMUITEM_SUBMENU,
     N_("Flood fill and replace colors submenu >>>"), &fillReplaceMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Regenerate Image"), NULL,
     N_("Regenerated a selected image"), NULL, CMDID_REGENERATEIMAGE },
   { N_("Crop Image"), NULL, N_("Crop a selected image"), NULL,
     CMDID_CROPIMAGE },
   { N_("Get Color"), NULL,
     N_("Pick a color from a selected image to be the current color"), NULL,
     CMDID_GETCOLOR },
   { N_("Create Contour"), NULL,
     N_("Generate a contour from a point in a selected image"), NULL,
     CMDID_CREATECONTOUR },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Subtract"), NULL,
     N_("Subtract one image from another"), NULL, CMDID_SUBTRACT },
   { N_("Alpha Combine"), NULL,
     N_("Combine 2 images using another image as the alpha channel"), NULL,
     CMDID_ALPHACOMBINE },
   { N_("Xor Color"), NULL, N_("XOR the colors of one image into another"),
     NULL, CMDID_XORCOLORS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Vector Warp"), NULL,
     N_("Warp selected image in a specified direction for a specified amount"),
     NULL, CMDID_VECTORWARP },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo imageProcMenuInfo={ TGMUTYPE_TEXT, imageProcMenuItemInfo,
                               TgCreateMenuFromMenuInfo };

static TgMenuItemInfo driverMenuItemInfo[] = {
   { N_("Solve"), "Alt+s", N_("Escape to driver"), NULL, CMDID_SOLVE },
   { N_("Simulate"), "Alt+y", N_("Escape to driver"), NULL, CMDID_SIMULATE },
   { N_("Probe"), "Alt+b", N_("Escape to driver"), NULL, CMDID_PROBE },
   { N_("Animate"), "Alt+u", N_("Escape to driver"), NULL, CMDID_ANIMATE },
   { N_("Escape"), "Alt+x", N_("Escape to driver"), NULL, CMDID_ESCAPE },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo driverMenuInfo={ TGMUTYPE_TEXT, driverMenuItemInfo,
                                   TgCreateMenuFromMenuInfo };

static TgMenuItemInfo browseMenuItemInfo[] = {
   { N_("Create Thumbnails"), NULL,
     N_("Create a thumbnail map of all object files"), NULL,
     CMDID_CREATETHUMBNAILS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Browse X Bitmap"), NULL,
     N_("Recursively read in all X11 bitmap files"), NULL,
     CMDID_BROWSEXBITMAP },
   { N_("Browse X Pixmap"), NULL,
     N_("Recursively read in all X11 pixmap files"), NULL,
     CMDID_BROWSEXPIXMAP },
   { N_("Browse Other..."), NULL,
     N_("Recursively read in all another type of files"), NULL,
     CMDID_BROWSEOTHER },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo browseMenuInfo={ TGMUTYPE_TEXT, browseMenuItemInfo,
                                   TgCreateMenuFromMenuInfo };

static TgMenuItemInfo importMultipageTextMenuItemInfo[] = {
   { N_("Import Multipage Text File"), NULL, N_("Import a multipage text file"),
     NULL, CMDID_IMPORTMULTIPAGETEXTFILE },
   { N_("Set Margins"), NULL,
     N_("Set margins for importing multipage text files"), NULL,
     CMDID_SETMARGINSONIMPORTTEXT },
   { N_("Word Wrap"), NULL,
     N_("Turn word wrapping on and off for importing multipage text files"),
     NULL, CMDID_TOGGLEWORDWRAPONIMPORTTEXT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo importMultipageTextMenuInfo={ TGMUTYPE_TEXT,
		importMultipageTextMenuItemInfo,
		CreateImportMutipageTextFileMenu };

static TgMenuItemInfo captureMenuItemInfo[] = {
   { N_("Screen Capture"), "Cntrl+1",
     N_("Capture a rectangular area of the screen"), NULL,
     CMDID_SCREENCAPTURE },
   { N_("Full Screen Capture"), "Cntrl+2", N_("Capture the whole screen"), NULL,
     CMDID_FULLSCREENCAPTURE },
   { N_("Delayed Full Screen Capture..."), NULL,
     N_("Capture the whole screen after a specified delay"), NULL,
     CMDID_DELAYEDFULLSCREENCAPTURE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Hide During Capture"), NULL,
     N_("Toggle between hiding and showing tgif during capture"), NULL,
     CMDID_TOGGLEHIDEDURINGCAPTURE },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo captureMenuInfo={ TGMUTYPE_TEXT, captureMenuItemInfo,
                                    CreateScreenCaptureMenu };

static TgMenuItemInfo importMenuItemInfo[] = {
   { N_("Import..."), "Alt+p", N_("Embed/Import another drawing"), NULL,
     CMDID_IMPORT },
   { N_("Import X Bitmap..."), "Alt+Cntrl+.",
     N_("Embed/Import an X11 bitmap file"), NULL, CMDID_IMPORTXBITMAP },
   { N_("Import X Pixmap..."), "Alt+Cntrl+,",
     N_("Embed/Import an X11 pixmap file"), NULL, CMDID_IMPORTXPIXMAP },
   { N_("Import EPS File..."), "Alt+(", N_("Link to a PS/EPS file"), NULL,
     CMDID_IMPORTEPSFILE },
   { N_("Import GIF File..."), NULL, N_("Embed/Import a GIF file"), NULL,
     CMDID_IMPORTGIFFILE },
   { N_("Import PNG File..."), NULL, N_("Embed/Import a PNG file"), NULL,
     CMDID_IMPORTPNGFILE },
   { N_("Import JPEG File..."), NULL, N_("Embed/Import a JPEG file"), NULL,
     CMDID_IMPORTJPEGFILE },
   { N_("Import PBM File..."), NULL, N_("Embed/Import a PBM file"), NULL,
     CMDID_IMPORTPBMFILE },
   { N_("Import PGM File..."), NULL, N_("Embed/Import a PGM file"), NULL,
     CMDID_IMPORTPGMFILE },
   { N_("Import PPM File..."), NULL, N_("Embed/Import a PPM file"), NULL,
     CMDID_IMPORTPPMFILE },
   { N_("Import Other File..."), NULL, N_("Embed/Import another type of file"),
     NULL, CMDID_IMPORTOTHERFILE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Embed EPS File..."), NULL, N_("Embed/Import a PS/EPS file"), NULL,
     CMDID_EMBEDEPSFILE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Link External JPEG File..."), NULL,
     N_("Link to an external JPEG file"), NULL, CMDID_LINKEXTJPEGFILE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Auto EPS Preview Bitmap"), NULL,
     N_("Auto-generate a preview bitmap for a PS/EPS file when embedding/importing it (if it does not have one already)."), NULL,
     CMDID_TOGGLEAUTOEPSPREVIEWBITMAP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Paste From File..."), NULL,
     N_("Select a file and paste the content as text"), NULL,
     CMDID_PASTEFROMFILE },
   { N_("Import Multipage Text File"), TGMUITEM_SUBMENU,
     N_("Import Multipage Text File submenu >>>"),
     &importMultipageTextMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Browse"), TGMUITEM_SUBMENU,
     N_("Browse submenu >>>"), &browseMenuInfo, INVALID },
   { N_("Screen Capture"), TGMUITEM_SUBMENU,
     N_("Screen Capture submenu >>>"), &captureMenuInfo, INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo importMenuInfo={ TGMUTYPE_TEXT, importMenuItemInfo,
                                   CreateImportMenu };

static TgMenuItemInfo domainMenuItemInfo[] = {
   { N_("Change Domain..."), "Alt+*", N_("Change to a different domain"), NULL,
     CMDID_CHANGEDOMAIN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Edit Domain Paths..."), NULL, N_("Edit domain paths"), NULL,
     CMDID_EDITDOMAINPATHS },
   { N_("Select Default Domain..."), NULL, N_("Select the default domain"),
     NULL, CMDID_SELECTDEFAULTDOMAIN },
   { N_("Add A Domain..."), NULL, N_("Add a new domain"), NULL,
     CMDID_ADDADOMAIN },
   { N_("Delete A Domain..."), NULL, N_("Delete an existing domain"), NULL,
     CMDID_DELETEADOMAIN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Reload Domain Info From X..."), NULL,
     N_("Reload domain information from X defaults"), NULL,
     CMDID_RELOADDOMAININFOFROMX },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo domainMenuInfo={ TGMUTYPE_TEXT, domainMenuItemInfo,
                                   CreateDomainMenu };

TgMenuInfo exportFormatMenuInfo={ TGMUTYPE_BITMAP, NULL,
                                  CreateExportFormatMenu };

static TgMenuItemInfo gifAnimateMenuItemInfo[] = {
   { N_("Export X Pixmap Deck To GIF"), NULL,
     N_("Export a deck of X11 pixmap images to an animated GIF file"), NULL,
     CMDID_EXPORTXPMDECKTOGIF },
   { N_("Import GIF To X Pixmap Deck..."), NULL,
     N_("Import an animated GIF file into a deck of X11 pixmap images"), NULL,
     CMDID_IMPORTGIFTOXPMDECK },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo gifAnimateMenuInfo={ TGMUTYPE_TEXT, gifAnimateMenuItemInfo,
                                       TgCreateMenuFromMenuInfo };

TgMenuInfo recentFilesMenuInfo={ TGMUTYPE_TEXT, NULL, CreateRecentFilesMenu };

#ifdef _TGIF_DBG
static TgMenuItemInfo mimeMenuItemInfo[] = {
   { N_("Fake User Agent..."), NULL,
     N_("Set 'UserAgent' to be used when making an HTTP request"), NULL,
     CMDID_FAKEUSERAGENT },
   { N_("Fake Referer..."), NULL,
     N_("Set 'Referer' to be used when making an HTTP request"), NULL,
     CMDID_FAKEREFERER },
   { N_("Toggle Keep Alive"), NULL,
     N_("Toggle between using Keep-Alive and non-Keep-Alive HTTP connections"),
     NULL, CMDID_TOGGLEKEEPALIVE },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo mimeMenuInfo={ TGMUTYPE_TEXT, mimeMenuItemInfo,
                                 TgCreateMenuFromMenuInfo };
#endif /* _TGIF_DBG */

static TgMenuItemInfo fileMenuItemInfo[] = {
   { N_("New"), "Cntrl+n", N_("Start with a blank/new drawing"), NULL,
     CMDID_NEW },
   { N_("Open..."), "Cntrl+o", N_("Open an existing drawing"), NULL,
     CMDID_OPEN },
   { N_("Import"), TGMUITEM_SUBMENU,
     "Import submenu >>>", &importMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Save"), "Cntrl+s", N_("Save current drawing"), NULL, CMDID_SAVE },
   { N_("Save New..."), "Alt+Cntrl+s",
     N_("Save current drawing in a different file"), NULL, CMDID_SAVENEW },
   { N_("Save Selected As..."), "Alt+~",
     N_("Save selected objects in a different file"), NULL,
     CMDID_SAVESELECTEDAS },
   { N_("Save Pages As..."), NULL,
     N_("Save user-specified pages in a different file"), NULL,
     CMDID_SAVEPAGESAS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Print"), "Cntrl+p", N_("Print/export current drawing"), NULL,
     CMDID_PRINT },
   { N_("Print/Export Format"), TGMUITEM_SUBMENU,
     N_("Select print/export format submenu >>>"), &exportFormatMenuInfo,
     INVALID },
   { N_("Set HTML Export Template..."), NULL,
     N_("Specify a file to be used as a template for genearting HTML files"),
     NULL, CMDID_SETHTMLEXPORTTEMPLATE },
   { N_("Print/Export In Color"), "Alt+Cntrl+k",
     N_("Toggle between color and black & white printing modes"), NULL,
     CMDID_TOGGLEBWCOLORPS },
   { N_("Print With Cmd..."), "Cntrl+-",
     N_("Print/export current drawing with a specific command"), NULL,
     CMDID_PRINTWITHCMD },
   { N_("Print Selected Objects"), NULL,
     N_("Print/export only selected objects"), NULL, CMDID_PRINTSELECTEDOBJS },
   { N_("Print One Page"), NULL, N_("Print a page in a multipage drawing"),
     NULL, CMDID_PRINTONEPAGE },
   { N_("Print Pages..."), NULL, N_("Print user-specified pages"), NULL,
     CMDID_PRINTPAGES },
   { N_("Print One File Per Page..."), NULL,
     N_("Print into a separate file for each page"),
     NULL, CMDID_PRINTONEFILEPERPAGE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("GIF Animation"), TGMUITEM_SUBMENU,
     N_("GIFAnimation submenu >>>"), &gifAnimateMenuInfo, INVALID },
#ifdef _TGIF_DBG
   { N_("Mime"), TGMUITEM_SUBMENU, N_("Mime submenu >>>"),
     &mimeMenuInfo, INVALID },
#endif /* _TGIF_DBG */
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Print Msg Buffer..."), NULL,
     N_("Display/save the contents of the message window"), NULL,
     CMDID_PRINTMSGBUFFER },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Export Pixel Trim..."), NULL,
     N_("Specify the number of pixels to trim when exporting"), NULL,
     CMDID_SETEXPORTPIXELTRIM },
   { N_("Set Template..."), NULL,
     N_("Select a template file be used in STACKED page mode"), NULL,
     CMDID_SETTEMPLATE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Recent Files"), TGMUITEM_SUBMENU,
     N_("Recently used files submenu >>>"), &recentFilesMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Quit"), "Cntrl+q", N_("Exit <<PROGRAM_NAME>>"), NULL, CMDID_QUIT },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo fileMenuInfo={ TGMUTYPE_TEXT, fileMenuItemInfo, CreateFileMenu };

static TgMenuItemInfo transformMenuItemInfo[] = {
   { N_("Precise Scale..."), "Alt+)", N_("Scale selected objects"), NULL,
     CMDID_PRECISESCALE },
   { "Precise Scale Everything...", NULL, N_("Scale everything"), NULL,
     CMDID_PRECISESCALEEVERYTHING },
   { N_("Precise Rotate..."), NULL,
     N_("Rotate selected objects by a specified number of degrees"), NULL,
     CMDID_PRECISEROTATE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Restore Image Width & Height"), NULL,
     N_("Restore a seleted bitmap/pixmap/EPS object to its original dimension"),
     NULL, CMDID_RESTOREIMAGEWH },
   { N_("No Transform"), NULL,
     N_("Get rid of transformations on selected simple objects"), NULL,
     CMDID_NOTRANSFORM },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo transformMenuInfo={ TGMUTYPE_TEXT, transformMenuItemInfo,
                                      TgCreateMenuFromMenuInfo };

static TgMenuItemInfo bitPixmapMenuItemInfo[] = {
   { N_("Invert X Bitmap"), "Alt+Cntrl+f",
     N_("Invert pixels for selected bitmap objects"), NULL,
     CMDID_INVERTXBITMAP },
   { N_("Cut Bit/Pixmap..."), "Alt+;",
     N_("Trim/scale a selected bitmap/pixmap object"), NULL,
     CMDID_CUTBITPIXMAP },
   { N_("Break Up Bit/Pixmap..."), NULL,
     N_("Break selected bitmap/pixmap object into smaller objects"), NULL,
     CMDID_BREAKUPBITPIXMAP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Remove Transparent Pixel For Pixmap..."), NULL,
     N_("Remove transparent pixels for selected pixmap objects"), NULL,
     CMDID_REMOVETRANSPIXEL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Export Half Tone Bitmap"), NULL,
     N_("Use Floyd-Steinberg half-tone method when exporting an X11 bitmap file"),
     NULL, CMDID_EXPORTHALFTONEBITMAP },
   { N_("Export Threshold Bitmap"), NULL,
     N_("Use simple thresholding method when exporting an X11 bitmap file"),
     NULL, CMDID_EXPORTTHRESHOLDBITMAP },
   { N_("Set Export Bitmap Threshold..."), NULL,
     N_("Set bitmap threshold value for the simple thresholding method"),
     NULL, CMDID_SETEXPORTBITMAPTHRESHOLD },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Update EPS"), NULL, N_("Refresh a selected EPS object"), NULL,
     CMDID_UPDATEEPS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo bitPixmapMenuInfo={ TGMUTYPE_TEXT, bitPixmapMenuItemInfo,
                                      CreateBitPixmapMenu };

static TgMenuItemInfo addTickMarksMenuItemInfo[] = {
   { N_("Set Tick Mark Size..."), NULL,
     N_("Set the size of tick marks for various commands"), NULL,
     CMDID_SETTICKMARKSIZE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Add Squares"), NULL,
     N_("Add square tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDSQUARETICKMARKS },
   { N_("Add Triangles"), NULL,
     N_("Add triangle tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDTRIANGLETICKMARKS },
   { N_("Add Circles"), NULL,
     N_("Add circle tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDCIRCLETICKMARKS },
   { N_("Add Xs"), NULL,
     N_("Add X tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDXTICKMARKS },
   { N_("Add Diamonds"), NULL,
     N_("Add diamond tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDDIAMONDTICKMARKS },
   { N_("Add Bowties"), NULL,
     N_("Add bowtie tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDBOWTIETICKMARKS },
   { N_("Add Inverted Triangles"), NULL,
     N_("Add inverted triangle tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDINVTRIANGLETICKMARKS },
   { N_("Add Plus"), NULL,
     N_("Add plus tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDPLUSTICKMARKS },
   { N_("Add Hour Glasss"), NULL,
     N_("Add hour glass tick marks at vertices of selected polys/polygons/splines"),
     NULL, CMDID_ADDHOURGLASSTICKMARKS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo addTickMarksMenuInfo={ TGMUTYPE_TEXT,
                                         addTickMarksMenuItemInfo,
                                         TgCreateMenuFromMenuInfo };

static TgMenuItemInfo polyMenuItemInfo[] = {
   { N_("Delete Point"), "Alt+Cntrl+d",
     N_("Delete vertices from a poly/polygon/spline"), NULL,
     CMDID_DELETEPOINT },
   { N_("Add Point"), "Alt+Cntrl+a",
     N_("Add vertices to a polygon/polygon/spline"), NULL, CMDID_ADDPOINT },
   { N_("Extend Segment"), NULL,
     N_("Extend or shrink a line segment"), NULL, CMDID_EXTENDSEGMENT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Convert Interpolated Spline"), NULL,
     N_("Convert selected interpolated splines to regular splines"), NULL,
     CMDID_CONVERTINTSPLINE },
   { N_("Smooth<->Hinge"), NULL,
     N_("Toggle smooth/hinge vertices for a selected poly/polygon/spline"),
     NULL, CMDID_TOGGLESMOOTHHINGE },
#ifdef NOT_DEFINED
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Use Tighter Structured Splines"), NULL,
     N_("Toggle between tighter and looser structured splines"),
     NULL, CMDID_TOGGLETIGHTERSPLINES },
#endif /* NOT_DEFINED */
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Convert To Bezier Curve"), NULL,
     N_("Convert selected splines to Bezier curves"), NULL,
     CMDID_CONVERTTOBEZIER },
   { N_("Set Segments For Convert To Bezier..."), NULL,
     N_("Set the number of segments for the convert to Bezier command"), NULL,
     CMDID_SETBEZIERCONVERTNUMSEGS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Join Poly"), NULL, N_("Join polylines or open-splines at endpoints"),
     NULL, CMDID_JOINPOLY },
   { N_("Cut Poly"), NULL, N_("Cut a poly/polygon at a vertex into two pieces"),
     NULL, CMDID_CUTPOLY },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Make Regular Polygon..."), "Alt+\"",
     N_("Make a selected polygon/closed spline into a regular one"), NULL,
     CMDID_MAKEREGPOLYGON },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Add Tick Marks"), TGMUITEM_SUBMENU,
     N_("Add Tick Marks submenu >>>"), &addTickMarksMenuInfo, INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo polyMenuInfo={ TGMUTYPE_TEXT, polyMenuItemInfo,
                                 CreatePolySplineMenu };

static TgMenuItemInfo textMenuItemInfo[] = {
   { N_("Break Up Text"), "Alt+#",
     N_("Break up selected text objects into one-character text objects"), NULL,
     CMDID_BREAKUPTEXT },
   { N_("Set Text Fill Pattern Color"), NULL,
     N_("Set the fill pattern color of a text object to be the current color"),
     NULL, CMDID_SETTEXTFILLPATTERNCOLOR },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Insert Right Superscript"), "Alt+Cntrl+6",
     N_("Insert superscript to the right of the text cursor"), NULL,
     CMDID_INSERTRIGHTSUPERSCRIPT },
   { N_("Insert Right Subscript"), "Alt+Cntrl+5",
     N_("Insert subscript to the right of the text cursor"), NULL,
     CMDID_INSERTRIGHTSUBSCRIPT },
   { N_("Insert Left Superscript"), NULL,
     N_("Insert superscript to the left of the text cursor"), NULL,
     CMDID_INSERTLEFTSUPERSCRIPT },
   { N_("Insert Left Subscript"), NULL,
     N_("Insert subscript to the left of the text cursor"), NULL,
     CMDID_INSERTLEFTSUBSCRIPT },
   { N_("Insert Center Superscript"), NULL,
     N_("Insert superscript above the text cursor"), NULL,
     CMDID_INSERTCENTERSUPERSCRIPT },
   { N_("Insert Center Subscript"), NULL,
     N_("Insert subscript below the text cursor"), NULL,
     CMDID_INSERTCENTERSUBSCRIPT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Insert Symbol..."), NULL,
     N_("Insert a symbol from the Symbol Font"), NULL,
     CMDID_INSERTSYMBOL },
#ifdef _TGIF_DBG
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Insert Hex/Octal Character..."), NULL,
     N_("Insert a character using hex or octal code"), NULL,
     CMDID_INSERTHEXOCTALCHAR },
#endif /* _TGIF_DBG */
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Reset input method..."), NULL,
     N_("Reset connection to the input method (in case it is stuck)"), NULL,
     CMDID_RESETINMETHOD },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Insert Thin Space..."), NULL,
     N_("Insert a think space at the text cursor"), NULL,
     CMDID_INSERTTHINSPACE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Insert Vertical Offset..."), NULL,
     N_("Insert a vertical offset at the text cursor"), NULL,
     CMDID_INSERTVERTICALOFFSET },
   { N_("Remove Vertical Offset..."), NULL,
     N_("Remove any vertical offset at the text cursor"), NULL,
     CMDID_REMOVEVERTICALOFFSET },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Script Fraction..."), NULL,
     N_("Set the relative size of super/subscript text"), NULL,
     CMDID_SETSCRIPTFRACTION },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo textMenuInfo={ TGMUTYPE_TEXT, textMenuItemInfo,
                                 CreateEditTextMenu };

static TgMenuItemInfo createObjectMenuItemInfo[] = {
   { N_("Input Poly Pts"), NULL,
     N_("Read points from terminal and create a poly"), NULL,
     CMDID_INPUTPOLYPTS },
   { N_("Input Polygon Pts"), NULL,
     N_("Read points from terminal and create a polygon"),
     NULL, CMDID_INPUTPOLYGONPTS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Add Tick Marks"), TGMUITEM_SUBMENU,
     N_("Add Tick Marks submenu >>>"), &addTickMarksMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Specify An Arc..."), "Alt+9", N_("Create and specify an arc"), NULL,
     CMDID_SPECIFYANARC },
   { N_("Get Bounding Box"), NULL,
     N_("Get rectangular bounding boxes of selected objects"), NULL,
     CMDID_GETBOUNDINGBOX },
   { N_("Create Rectangle From Bounding Box"), NULL,
     N_("Create a rectangle from the overall bounding box of selected objects"), NULL,
     CMDID_NEWRECTFROMBOUNDINGBOX },
   { N_("Create Oval From Bounding Box"), NULL,
     N_("Create an oval from the overall bounding box of selected objects"), NULL,
     CMDID_NEWOVALFROMBOUNDINGBOX },
   { N_("Create Rounded-corner Rectangle From Bounding Box"), NULL,
     N_("Create a rounded-corner rectangle from the overall bounding box of selected objects"), NULL,
     CMDID_NEWRCBOXFROMBOUNDINGBOX },
   { N_("Create Pixmap From Selected"), NULL,
     N_("Create an X11 pixmap object from selected objects"), NULL,
     CMDID_CREATEPIXMAPFROMSEL },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo createObjectMenuInfo={ TGMUTYPE_TEXT,
                                         createObjectMenuItemInfo,
                                         TgCreateMenuFromMenuInfo };

static TgMenuItemInfo editMenuItemInfo[] = {
   { N_("Redraw"), "Cntrl+r", N_("Redraw canvas window"), NULL, CMDID_REDRAW },
   { N_("Duplicate"), "Cntrl+d", N_("Duplicate selected objects"), NULL,
     CMDID_DUPLICATE },
   { N_("Delete"), NULL, N_("Delete selected objects"), NULL, CMDID_DELETE },
   { N_("Select All"), "Cntrl+a", N_("Select all objects on the current page"),
     NULL, CMDID_SELECTALL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Undo"), "Cntrl+z", N_("Undo the most recent command"), NULL,
     CMDID_UNDO },
   { N_("Redo"), "Cntrl+y", N_("Redo the most undone command"), NULL,
     CMDID_REDO },
   { N_("Flush Undo Buffer"), NULL, N_("Flush undo buffer and unused colors"),
     NULL, CMDID_FLUSHUNDOBUFFER },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Copy"), "Cntrl+c", N_("Copy selected objects into the cut buffer"),
     NULL, CMDID_COPY },
   { N_("Copy Plain Text As Object"), NULL,
     N_("Copy highlighted plain text as an object into the cut buffer (to be pasted into Tgif)"), NULL,
     CMDID_COPYPLAINTEXTASOBJECT },
   { N_("Cut"), NULL, N_("Cut selected objects into the cut buffer"), NULL,
     CMDID_CUT },
   { N_("Paste"), "Cntrl+v", N_("Paste from the cut buffer"), NULL,
     CMDID_PASTE },
   { N_("Paste From File..."), NULL,
     N_("Select a file and paste the content as text"), NULL,
     CMDID_PASTEFROMFILE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Copy Double-byte (UTF-8) String"), NULL,
     N_("Copy selected double-byte text string into the cut buffer (UTF-8) encoding"), NULL,
     CMDID_COPYUTF8 },
   { N_("Paste Double-byte (UTF-8) String"), NULL,
     N_("Paste from the cut buffer (UTF-8 encoding) into a double-byte text object"), NULL,
     CMDID_PASTEUTF8 },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Generic Object Shadow"), TGMUITEM_SUBMENU,
     N_("Generic Object Shadow submenu >>>"), &objShadowMenuInfo, INVALID },
   { N_("Shape"), TGMUITEM_PINNABLESUBMENU,
     N_("Shape submenu >>>"), &shapeMenuInfo, MENU_SHAPE },
   { N_("Set Shape Shadow..."), NULL, N_("Specify the shadow for shapes"), NULL,
     CMDID_SETSHAPESHADOW },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Image Proc"), TGMUITEM_PINNABLESUBMENU,
     N_("ImageProc submenu >>>"), &imageProcMenuInfo, MENU_IMAGEPROC },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties submenu >>>"), &propertiesMenuInfo, MENU_PROPERTIES },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Transformation"), TGMUITEM_SUBMENU,
     N_("Transformation submenu >>>"), &transformMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Bitmap/Pixmap/EPS"), TGMUITEM_SUBMENU,
     N_("Bitmap/Pixmap/EPS submenu >>>"), &bitPixmapMenuInfo, INVALID },
   { N_("Poly/Polygon"), TGMUITEM_SUBMENU,
     N_("Poly/Polygon submenu >>>"), &polyMenuInfo, INVALID },
   { N_("Text"), TGMUITEM_SUBMENU, N_("Text submenu >>>"), &textMenuInfo,
     INVALID },
   { N_("Create Object"), TGMUITEM_SUBMENU,
     N_("Create object submenu >>>"), &createObjectMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Push Current Choice"), "Cntrl+e",
     N_("Save/restore current drawing mode"), NULL, CMDID_PUSHCURCHOICE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Find Case Sensitive..."), "Alt+Cntrl+0",
     N_("Find string in visible text objects (case sensitive)"), NULL,
     CMDID_FINDCASESENSITIVE },
   { N_("Find No Case..."), "Alt+Cntrl+9",
     N_("Find string in visible text objects (case insensitive)"), NULL,
     CMDID_FINDNOCASE },
   { N_("Find Again"), "Alt+Cntrl+=", N_("Repeat the last Find command"), NULL,
     CMDID_FINDAGAIN },
/*
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("ConnectPins", NULL, "Connect pins", NULL, CMDID_CONNECTPINS },
 */
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo editMenuInfo={ TGMUTYPE_TEXT, editMenuItemInfo, CreateEditMenu };

static TgMenuItemInfo alignMenuItemInfo[] = {
   { N_("Align Objects Top"), "Alt+{",
     N_("Align top sides of selected objects"), NULL, CMDID_ALIGNOBJSTOP },
   { N_("Align Objects Middle"), "Alt++",
     N_("Align vertical centers of selected objects"),
     NULL, CMDID_ALIGNOBJSMIDDLE },
   { N_("Align Objects Bottom"), "Alt+}",
     N_("Align bottom sides of selected objects"), NULL,
     CMDID_ALIGNOBJSBOTTOM },
   { N_("Align Objects Left"), "Alt+[",
     N_("Align left sides of selected objects"), NULL, CMDID_ALIGNOBJSLEFT },
   { N_("Align Objects Center"), "Alt+=",
     N_("Align horizontal centers of selected objects"),
     NULL, CMDID_ALIGNOBJSCENTER },
   { N_("Align Objects Right"), "Alt+]",
     N_("Align right sides of selected objects"), NULL, CMDID_ALIGNOBJSRIGHT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Center An End Point"), NULL,
     N_("Move an endpoint of a selected poly to the center of another object"),
     NULL, CMDID_CENTERANENDPOINT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo alignMenuInfo={ TGMUTYPE_TEXT, alignMenuItemInfo,
                                  TgCreateMenuFromMenuInfo };

static TgMenuItemInfo sizeMenuItemInfo[] = {
   { N_("Size To Widest"), NULL,
     N_("Resize all selected objects to be as wide as the widest one"), NULL,
     CMDID_SIZETOWIDEST },
   { N_("Size To Narrowest"), NULL,
     N_("Resize all selected objects to be as narrow as the narrowest one"),
     NULL, CMDID_SIZETONARROWEST },
   { N_("Size To Tallest"), NULL,
     N_("Resize all selected objects to be as tall as the tallest one"), NULL,
     CMDID_SIZETOTALLEST },
   { N_("Size To Shortest"), NULL,
     N_("Resize all selected objects to be as short as the shortest one"), NULL,
     CMDID_SIZETOSHORTEST },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Size To Given Width & Height..."), NULL,
     N_("Resize all selected objects to a specified width and height"), NULL,
     CMDID_SIZETOGIVENWIDTHHEIGHT },
   { N_("Size To Given Width..."), NULL,
     N_("Resize all selected objects to a specified width"), NULL,
     CMDID_SIZETOGIVENWIDTH },
   { N_("Size To Given Height..."), NULL,
     N_("Resize all selected objects to a specified height"), NULL,
     CMDID_SIZETOGIVENHEIGHT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo sizeMenuInfo={ TGMUTYPE_TEXT, sizeMenuItemInfo,
                                 TgCreateMenuFromMenuInfo };

static TgMenuInfo alignObjsDirectMenuInfo={ TGMUTYPE_BITMAP, NULL,
                                            CreateAlignObjsDirectMenu };
static TgMenuInfo alignToGridDirectMenuInfo={ TGMUTYPE_BITMAP, NULL,
                                              CreateAlignToGridDirectMenu };
static TgMenuInfo alignToPageDirectMenuInfo={ TGMUTYPE_BITMAP, NULL,
                                              CreateAlignToPageDirectMenu };

static TgMenuInfo distrObjsDirectMenuInfo={ TGMUTYPE_BITMAP, NULL,
                                            CreateDistributeDirectMenu };

static TgMenuItemInfo autoRotatePivotMenuItemInfo[] = {
   { (char*)(&rotateModePixmap[AUTO_ROTATE_PIVOT]), NULL,
     N_("Set rotation pivot mode to automatic"),
     NULL, CMDID_CHOOSEROTATEPIVOT },
   { (char*)(&rotateModePixmap[USER_ROTATE_PIVOT]), NULL,
     N_("Set rotation pivot mode to user-specified"),
     NULL, CMDID_CHOOSEROTATEPIVOT },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo autoRotatePivotMenuInfo={ TGMUTYPE_BITMAP,
                                     autoRotatePivotMenuItemInfo,
                                     CreateAutoRotatePivotMenu };

static TgMenuItemInfo moveStdRotatePivotMenuItemInfo[] = {
   { N_("Move Rotation Pivot To Center"), NULL,
     N_("Move rotation pivot to the center of selected objects"), NULL,
     CMDID_MOVEROTATEPIVOTCENTER },
   { N_("Move Rotation Pivot To LeftTop Corner"), NULL,
     N_("Move rotation pivot to the left-top corner of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTLT },
   { N_("Move Rotation Pivot To RightTop Corner"), NULL,
     N_("Move rotation pivot to the right-top corner of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTRT },
   { N_("Move Rotation Pivot To LeftBottom Corner"), NULL,
     N_("Move rotation pivot to the left-bottom corner of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTLB },
   { N_("Move Rotation Pivot To RightBottom Corner"), NULL,
     N_("Move rotation pivot to the right-bottom corner of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTRB },
   { N_("Move Rotation Pivot To Left Center"), NULL,
     N_("Move rotation pivot to the left center of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTLEFT },
   { N_("Move Rotation Pivot To Right Center"), NULL,
     N_("Move rotation pivot to the right center of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTRIGHT },
   { N_("Move Rotation Pivot To Top Center"), NULL,
     N_("Move rotation pivot to the top center of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTTOP },
   { N_("Move Rotation Pivot To Bottom Center"), NULL,
     N_("Move rotation pivot to the bottom center of selected objects"),
     NULL, CMDID_MOVEROTATEPIVOTBOTTOM },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo rotatePivotMenuInfo={ TGMUTYPE_TEXT, moveStdRotatePivotMenuItemInfo,
                                CreateMoveStdRotatePivotMenu };

static TgMenuItemInfo flipRotateMenuItemInfo[] = {
   { N_("Flip Horizontal"), "Alt+h",
     N_("Flip selected objects horizontally"), NULL, CMDID_FLIPHORIZONTAL },
   { N_("Flip Vertical"), "Alt+v", N_("Flip selected objects vertically"), NULL,
     CMDID_FLIPVERTICAL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Rotate Clock-Wise"), "Alt+w", N_("Rotate selected objects clockwise"),
     NULL, CMDID_ROTATECLOCKWISE },
   { N_("Rotate Counter"), "Alt+c",
     N_("Rotate selected objects counter-clockwise"), NULL,
     CMDID_ROTATECOUNTER },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Text Rotation..."), NULL, N_("Set text rotation in degrees"), NULL,
     CMDID_SETTEXTROTATION },
   { N_("Set Rotation Increment..."), NULL,
     N_("Set rotation increment in degrees"), NULL,
     CMDID_SETROTATIONINCREMENT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Rotation Pivot"), TGMUITEM_SUBMENU,
     N_("Rotation pivot submenu >>>"), &autoRotatePivotMenuInfo, INVALID },
   { N_("Reset Rotation Pivot"), NULL,
     N_("Reset user-specified rotation pivot to center of selected objects"),
     NULL, CMDID_RESETROTATEPIVOT },
   { N_("Speficy Rotation Pivot..."), NULL,
     N_("Specify the coordinates for the user-specified rotation pivot"), NULL,
     CMDID_SPECIFYROTATEPIVOT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Next Poly/Polygon Rotation Pivot"), NULL,
     N_("Move the rotation pivot to the next vertex of a poly/polygon object"),
     NULL, CMDID_NEXTPOLYROTATEPIVOT },
   { N_("Move Rotation Pivot To Arc Center"), NULL,
     N_("Move the rotation pivot to the center of the circle which an arc object is part of"),
     NULL, CMDID_MOVEROTATEPIVOTARCCNTR },
   { N_("Move Standard Rotation Pivot"), TGMUITEM_SUBMENU,
     N_("Move standard rotation pivot submenu >>>"), &rotatePivotMenuInfo,
     INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo flipRotateMenuInfo={ TGMUTYPE_TEXT, flipRotateMenuItemInfo,
                                CreateFlipRotateMenu };

static TgMenuItemInfo arrangeMenuItemInfo[] = {
   { N_("Front"), "Cntrl+f", N_("Bring selected objects to the front"), NULL,
     CMDID_FRONT },
   { N_("Back"), "Cntrl+b", N_("Send selected objects to the back"), NULL,
     CMDID_BACK },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Group"), "Cntrl+g",
     N_("Group selected objects into a compound object"), NULL, CMDID_GROUP },
   { N_("UnGroup"), "Cntrl+u",
     N_("Break up selected grouped objects into its components"), NULL,
     CMDID_UNGROUP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Lock"), "Alt+<", N_("Lock the positions of selected objects"), NULL,
     CMDID_LOCK },
   { N_("UnLock"), "Alt+>", N_("UnLock the positions of selected objects"),
     NULL, CMDID_UNLOCK },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Horizontal Align"), TGMUITEM_PINNABLESUBMENU,
     N_("HoriAlign submenu >>>"), &horiAlignMenuInfo, MENU_HORIALIGN },
   { N_("Vertical Align"), TGMUITEM_PINNABLESUBMENU,
     N_("VertAlign submenu >>>"), &vertAlignMenuInfo,
     MENU_VERTALIGN },
   { N_("Align Objects"), "Cntrl+l",
     N_("Align selected objects with each other using the current alignments"),
     NULL, CMDID_ALIGNOBJS },
   { N_("Align Objects Direct"), TGMUITEM_SUBMENU,
     N_("Align objects direct submenu >>>"), &alignObjsDirectMenuInfo,
     INVALID },
   { N_("Align To Grid"), "Cntrl+t",
     N_("Align selected objects to grid points using the current alignments"),
     NULL, CMDID_ALIGNTOGRID },
   { N_("Align To Grid Direct"), TGMUITEM_SUBMENU,
     N_("Align to grid direct submenu >>>"), &alignToGridDirectMenuInfo,
     INVALID },
   { N_("Align To Page"), "Alt+&",
     N_("Align selected objects to the page using the current alignments"),
     NULL, CMDID_ALIGNTOPAGE },
   { N_("Align To Page Direct"), TGMUITEM_SUBMENU,
     N_("Align to page direct submenu >>>"), &alignToPageDirectMenuInfo,
     INVALID },
   { N_("More Object Alignments"), TGMUITEM_SUBMENU,
     N_("Align objects submenu >>>"), &alignMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Size Objects"), TGMUITEM_SUBMENU,
     N_("Size objects submenu >>>"), &sizeMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Distribute Objects"), "Alt+l",
     N_("Distribute selected objects using the current alignments"), NULL,
     CMDID_DISTRIBUTEOBJS },
   { N_("Distribute Objects Direct"), TGMUITEM_SUBMENU,
     N_("Distribute objects direct submenu >>>"), &distrObjsDirectMenuInfo,
     INVALID },
   { N_("Layout On Arc"), NULL, N_("Layout objects on an arc"), NULL,
     CMDID_LAYOUTONARC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Abut Horizontal"), "Alt+_",
     N_("Squish selected objects together horizontally"),
     NULL, CMDID_ABUTHORIZONTAL },
   { N_("Abut Vertical"), "Alt+|",
     N_("Squish selected objects together vertically"), NULL,
     CMDID_ABUTVERTICAL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Flip/Rotate"), TGMUITEM_SUBMENU,
     N_("Flip and rotate submenu >>>"), &flipRotateMenuInfo, INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo arrangeMenuInfo={ TGMUTYPE_TEXT, arrangeMenuItemInfo,
                             TgCreateMenuFromMenuInfo };

static TgMenuItemInfo scrollModeMenuItemInfo[] = {
   { N_("Off"), NULL, N_("Do not update canvas window while scrolling"), NULL,
     CMDID_CHANGESCROLLMODE },
   { N_("Jump"), NULL, N_("Jump update canvas window while scrolling"), NULL,
     CMDID_CHANGESCROLLMODE },
   { N_("Smooth"), NULL, N_("Update canvas window smoothly while scrolling"),
     NULL, CMDID_CHANGESCROLLMODE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo scrollModeMenuInfo={ TGMUTYPE_TEXT, scrollModeMenuItemInfo,
                                CreateScrollModeMenu };

static TgMenuItemInfo showHideMenuItemInfo[] = {
   { N_("Show Bit/Pixmap"), NULL,
     N_("Toggle between hiding and showing detailed bitmap/pixmap objects"),
     NULL, CMDID_TOGGLEMAPSHOWN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Show Measurement"), NULL,
     N_("Show cursor measurements"), NULL, CMDID_TOGGLESHOWMEASUREMENT },
   { N_("Show Measurement in Tooltip"), NULL,
     N_("Show cursor measurements in tooltip"), NULL,
     CMDID_TOGGLESHOWMEASUREMENTINTT },
   { N_("Set Measure Unit..."), NULL, N_("Set the unit of measurements"), NULL,
     CMDID_SETMEASUREUNIT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Enable Right Margin"), NULL,
     N_("Toggle between using (simple) right margin or not"), NULL,
     CMDID_TOGGLERIGHTMARGIN },
   { N_("Specify Right Margin..."), NULL,
     N_("Specify the (simple) right margin"), NULL,
     CMDID_SPECIFYRIGHTMARGIN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Show Menubar"), NULL, N_("Show menubar window"), NULL,
     CMDID_TOGGLESHOWMENUBAR },
   { N_("Show Choice"), NULL, N_("Show choice and message window"), NULL,
     CMDID_TOGGLESHOWCHOICEBAR },
   { N_("Show Status"), NULL, N_("Show status window"), NULL,
     CMDID_TOGGLESHOWSTATUS },
   { N_("Show Mode"), NULL, N_("Show mode window"), NULL,
     CMDID_TOGGLESHOWMODE },
   { N_("Show Chat"), NULL, N_("Show chat window"), NULL,
     CMDID_TOGGLESHOWCHAT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo showHideMenuInfo={ TGMUTYPE_TEXT, showHideMenuItemInfo,
                                     CreateShowHideMenu };

static TgMenuItemInfo slideShowModeMenuItemInfo[] = {
   { N_("Go HyperSpace In Slide Show"), NULL,
     N_("Enter HyperSpace mode in slideshow"), NULL,
     CMDID_GOHYPERSPACEINSLIDESHOW },
   { N_("Freehand Mode In Slide Show"), NULL,
     N_("Go into freehand drawing mode in slideshow"), NULL,
     CMDID_FREEHANDMODEINSLIDESHOW },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Next Slide"), NULL, N_("Go to next slide"), NULL,
     CMDID_NEXTPAGE },
   { N_("Prev Slide"), NULL, N_("Go to previous slide"), NULL,
     CMDID_PREVPAGE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Color"), TGMUITEM_PINNABLESUBMENU,
     N_("Color Menu"), &colorMenuInfo, MENU_COLOR },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Exit Slide Show Mode"), "Esc", N_("Exit SlideShow mode"), NULL,
     CMDID_TOGGLESLIDESHOW },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo slideShowModeMenuInfo={ TGMUTYPE_TEXT, slideShowModeMenuItemInfo,
                                   CreateSlideShowModeMenu };

static TgMenuItemInfo slideShowMenuItemInfo[] = {
   { N_("Slide Show"), "Alt+Cntrl+8", N_("Toggle SlideShow mode"), NULL,
     CMDID_TOGGLESLIDESHOW },
   { N_("Set Slide Show Border Color..."), NULL,
     N_("Set the color of the slideshow border"), NULL,
     CMDID_SETSLIDESHOWBRDRCOLOR },
   { N_("Set Slide Show Window Offsets..."), NULL,
     N_("Set the offsets of the slideshow window"), NULL,
     CMDID_SETSLIDESHOWWINOFFSETS },
   { N_("Visible Grid In Slide Show"), NULL,
     N_("Toggle visible grid in slideshow mode"), NULL,
     CMDID_VISIBLEGRIDINSLIDESHOW },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Go HyperSpace In Slide Show"), NULL,
     N_("Enter HyperSpace mode in slideshow"), NULL,
     CMDID_GOHYPERSPACEINSLIDESHOW },
   { N_("Freehand Mode In Slide Show"), NULL,
     N_("Go into freehand drawing mode in slideshow"), NULL,
     CMDID_FREEHANDMODEINSLIDESHOW },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo slideShowMenuInfo={ TGMUTYPE_TEXT, slideShowMenuItemInfo,
                                      CreateSlideShowMenu };

static TgMenuItemInfo zoomMenuItemInfo[] = {
   { N_("Zoom In"), "Alt+z", N_("Zoom in"), NULL, CMDID_ZOOMIN },
   { N_("Zoom Out"), "Alt+o", N_("Zoom out"), NULL, CMDID_ZOOMOUT },
   { N_("Default Zoom"), "Alt+:", N_("No zoom"), NULL, CMDID_DEFAULTZOOM },
   { N_("Zoom Way Out"), "Alt+`", N_("Zoom way out to see the whole drawing"),
     NULL, CMDID_ZOOMWAYOUT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo zoomMenuInfo={ TGMUTYPE_TEXT, zoomMenuItemInfo,
                                 TgCreateMenuFromMenuInfo };

static TgMenuItemInfo layoutMenuItemInfo[] = {
   { N_("+Grid"), "Alt+i", N_("Increment grid size"), NULL, CMDID_INCGRID },
   { N_("-Grid"), "Alt+d", N_("Decrement grid size"), NULL, CMDID_DECGRID },
   { N_("Visible Grid"), "Alt+g", N_("Hide/show grid lines"), NULL,
     CMDID_TOGGLEGRID },
   { N_("Snap To Grid"), "Alt+Cntrl+g",
     N_("Toggle the snapping to grid points effect"), NULL, CMDID_TOGGLESNAP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Save Origin"), NULL,
     N_("Save current location as the 'origin' of the drawing"),
     NULL, CMDID_SAVEORIGIN },
   { N_("Scroll To Origin"), "Alt+^",
     N_("Scroll to the 'origin' of the drawing"), NULL, CMDID_SCROLLTOORIG },
   { N_("Scroll Mode"), TGMUITEM_SUBMENU,
     N_("ScrollMode submenu >>>"), &scrollModeMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Zoom"), TGMUITEM_SUBMENU, N_("Zoom submenu >>>"), &zoomMenuInfo,
     INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Portrait"), NULL, N_("Portrait page style"), NULL,
     CMDID_PORTRAITMODE },
   { N_("Landscape"), NULL, N_("Lanscape page style"), NULL,
     CMDID_LANDSCAPEMODE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Stacked Page"), TGMUITEM_PINNABLESUBMENU,
     N_("StackedPage submenu >>>"), &stackedPageMenuInfo, MENU_STACKEDPAGE },
   { N_("Tiled Page"), TGMUITEM_PINNABLESUBMENU,
     N_("TiledPage submenu >>>"), &tiledPageMenuInfo, MENU_TILEDPAGE },
   { N_("Page Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("PageLayout submenu >>>"), &pageLayoutMenuInfo, MENU_PAGELAYOUT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Print/Export Format"), TGMUITEM_SUBMENU,
     N_("Select print/export format submenu >>>"), &exportFormatMenuInfo,
     INVALID },
   { N_("Print/Export In Color"), "Alt+Cntrl+k",
     N_("Toggle between color and black & white printing modes"), NULL,
     CMDID_TOGGLEBWCOLORPS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Reduction..."), "Alt+%",
     N_("Specify reduction/magnification of the whole drawing"), NULL,
     CMDID_SETREDUCTION },
   { N_("Metric Grid"), "Alt+Cntrl+-",
     N_("Toggle between English and Metric grid systems"), NULL,
     CMDID_TOGGLEGRIDSYSTEM },
   { N_("One Motion Select Move"), NULL,
     N_("Toggle between click-select-move and click-select-click-move modes"),
     NULL, CMDID_TOGGLEONEMOTIONSELMOVE },
   { N_("Use Gray Scale"), NULL,
     N_("Toggle between using gray scales to tile patterns to speed up printing"),
     NULL, CMDID_TOGGLEUSEGRAYSCALE },
   { N_("Use Color Layers"), NULL, N_("Toggle color layers"), NULL,
     CMDID_TOGGLECOLORLAYERS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Show/Hide"), TGMUITEM_SUBMENU,
     N_("Show/Hide submenu >>>"), &showHideMenuInfo, INVALID },
   { N_("Slide Show"), TGMUITEM_SUBMENU,
     N_("SlideShow submenu >>>"), &slideShowMenuInfo, INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo layoutMenuInfo={ TGMUTYPE_TEXT, layoutMenuItemInfo,
                            CreateLayoutMenu };

static TgMenuItemInfo propertiesMenuItemInfo[] = {
   { N_("Copy Properties..."), NULL,
     N_("Copy properties (e.g., color, fill, etc.) into the cut buffer"), NULL,
     CMDID_COPYPROPERTIES },
   { N_("Paste Properties..."), NULL,
     N_("Paste properties (e.g., color, fill, etc.) from the cut buffer"), NULL,
     CMDID_PASTEPROPERTIES },
   { N_("Save Properties..."), NULL,
     N_("Save and name a property (e.g., color, fill, etc.) set"), NULL,
     CMDID_SAVEPROPERTIES },
   { N_("Restore Properties..."), NULL,
     N_("Restore a named property (e.g., color, fill, etc.) set"), NULL,
     CMDID_RESTOREPROPERTIES },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
/*
 * { N_("Update Objects"), "Alt+0",
 *   N_("Update selected objects to have current drawing properties"), NULL,
 *   CMDID_UPDATE },
 * { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
 */
   { N_("Set Selected Line Width..."), NULL,
     N_("Change the line width (and arrow width/height) of selected objects"),
     NULL, CMDID_SETSELLINEWIDTH },
   { N_("Set Selected Font Size..."), NULL,
     N_("Change the font size of selected objects"), NULL,
     CMDID_SETSELFONTSIZE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Add Color..."), NULL, N_("Add colors to the current palette"), NULL,
     CMDID_ADDCOLOR },
   { N_("Choose Color..."), NULL,
     N_("Choose a color from a color wheel and add it to the current palette"),
     NULL, CMDID_COLORWHEEL },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Edit Text Size..."), NULL,
     N_("Set the text size to be used in editing existing text objects"), NULL,
     CMDID_SETEDITTEXTSIZE },
   { N_("Set Alt Edit Text BgColor..."), NULL,
     N_("Set the background color to be used in editing existing text objects"),
     NULL, CMDID_SETALTEDITTEXTBGCOLOR },
   { N_("Set Alt Edit Text Highlight Color..."), NULL,
     N_("Set the highlight color to be used in editing existing text objects"),
     NULL, CMDID_SETALTEDITTEXTHIGHLIGHTCOLOR },
   { N_("Use Alt Edit Text BgColor"), NULL,
     N_("Toggles the background color to be used in editing existing text objects"),
     NULL, CMDID_TOGGLEALTEDITTEXTBGCOLOR },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Font"), TGMUITEM_PINNABLESUBMENU,
     N_("Font submenu >>>"), &fontMenuInfo, MENU_FONT },
   { N_("Text Style"), TGMUITEM_PINNABLESUBMENU,
     N_("TextStyle submenu >>>"), &fontStyleMenuInfo, MENU_STYLE },
   { N_("Text Size"), TGMUITEM_PINNABLESUBMENU,
     N_("TextSize submenu >>>"), &fontSizeMenuInfo, MENU_SIZE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Line Dash"), TGMUITEM_PINNABLESUBMENU,
     N_("LineDash submenu >>>"), &lineDashMenuInfo, MENU_LINEDASH },
   { N_("Line Style"), TGMUITEM_PINNABLESUBMENU,
     N_("LineStyle submenu >>>"), &lineStyleMenuInfo, MENU_LINESTYLE },
   { N_("Line Type"), TGMUITEM_PINNABLESUBMENU,
     N_("LineType submenu >>>"), &lineTypeMenuInfo, MENU_LINETYPE },
   { N_("Line Width"), TGMUITEM_PINNABLESUBMENU,
     N_("LineWidth submenu >>>"), &lineWidthMenuInfo, MENU_LINEWIDTH },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Fill"), TGMUITEM_PINNABLESUBMENU,
     N_("Fill submenu >>>"), &fillMenuInfo, MENU_FILL },
   { N_("Pen"), TGMUITEM_PINNABLESUBMENU,
     N_("Pen submenu >>>"), &penMenuInfo, MENU_PEN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Color"), TGMUITEM_PINNABLESUBMENU, N_("Color submenu >>>"),
     &colorMenuInfo, MENU_COLOR },
   { N_("Transparent Pattern"), TGMUITEM_PINNABLESUBMENU,
     N_("TransPattern submenu >>>"), &transPatModeMenuInfo, MENU_TRANSPAT },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo propertiesMenuInfo={ TGMUTYPE_TEXT, propertiesMenuItemInfo,
                                CreatePropertiesMenu };

static TgMenuItemInfo navigateMenuItemInfo[] = {
   { N_("Go Back"), NULL, N_("Go back one file"), NULL, CMDID_GOBACK },
   { N_("Go Forward"), NULL, N_("Go forward one file"), NULL, CMDID_GOFORWARD },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Refresh Current"), NULL, N_("Reload the current file"), NULL,
     CMDID_REFRESHCURRENT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Hot List..."), NULL, N_("Navigate using the hot-list"), NULL,
     CMDID_HOTLIST },
   { N_("Add Current To Hot List"), NULL,
     N_("Add the current file to the hot-list"), NULL,
     CMDID_ADDCURRENTTOHOTLIST },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Session History..."), NULL,
     N_("Go to a file visited during this session"), NULL,
     CMDID_SESSIONHISTORY },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Hyper Space"), NULL, N_("Toggle HyperSpace mode"), NULL,
     CMDID_TOGGLEHYPERSPACE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo navigateMenuInfo={ TGMUTYPE_TEXT, navigateMenuItemInfo,
                              CreateNavigateMenu };

static TgMenuItemInfo attrMenuItemInfo[] = {
   { N_("Attach Attributes"), "Alt+a",
     N_("Attach selected text objects as attributes for the non-text object"),
     NULL, CMDID_ATTACHATTRS },
   { N_("Detach Attributes"), "Alt+t",
     N_("Detach all attributes of selected objects"), NULL,
     CMDID_DETACHATTRS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Edit Attributes..."), NULL,
     N_("Edit attributes of a selected object"), NULL,
     CMDID_EDITATTRS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Show Attributes"), "Alt+-",
     N_("Make all attributes of selected objects visible"), NULL,
     CMDID_SHOWATTR },
   { N_("Show Attribute Names"), "Alt+n",
     N_("Make all attribute names of selected objects visible"), NULL,
     CMDID_SHOWATTRNAME },
   { N_("Hide Attributes"), "Alt+Cntrl+h",
     N_("Hide all attributes for selected objects"), NULL,
     CMDID_HIDEATTR },
   { N_("Hide Attribute Names"), "Alt+j",
     N_("Hide all attribute names for selected objects"), NULL,
     CMDID_HIDEATTRNAME },
   { N_("Toggle Eq Attribute Shown..."), "Alt+Cntrl+7",
     N_("Showing/hiding the 'eq=' attibute (for a LaTeX equation object)"),
     NULL, CMDID_TOGGLEEQATTRSHOWN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Move/Justfy An Attribute"), "Alt+m",
     N_("Move/justify an attribute for a selected object"), NULL,
     CMDID_MOVEJUSTIFYATTR },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Import Attributes..."), NULL,
     N_("Import attributes of a selected object from a text file"), NULL,
     CMDID_IMPORTATTRS },
   { N_("Export Attributes..."), NULL,
     N_("Export attributes of a selected object to a text file"), NULL,
     CMDID_EXPORTATTRS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo attrMenuInfo={ TGMUTYPE_TEXT, attrMenuItemInfo,
                                 TgCreateMenuFromMenuInfo };

static TgMenuItemInfo fileAttrMenuItemInfo[] = {
   { N_("Attach File Attributes"), NULL,
     N_("Attach selected text objects as file attributes"), NULL,
     CMDID_ATTACHFILEATTRS },
   { N_("Detach File Attributes"), NULL,
     N_("Detach all file attributes"), NULL,
     CMDID_DETACHFILEATTRS },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Edit File Attributes..."), NULL,
     N_("Edit file attributes"), NULL,
     CMDID_EDITFILEATTRS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo fileAttrMenuInfo={ TGMUTYPE_TEXT, fileAttrMenuItemInfo,
                                     TgCreateMenuFromMenuInfo };

static TgMenuItemInfo animateMenuItemInfo[] = {
   { N_("Animate Send"), "Alt+e",
     N_("Animate a little token on a selected poly (not very useful)"), NULL,
     CMDID_ANIMATESEND },
   { N_("Animate Flash"), "Alt+f",
     N_("Flash a selected poly (not very useful)"), NULL,
     CMDID_ANIMATEFLASH },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Renumber Object IDs"), "Alt+f",
     N_("Renumber object IDs for all objects"), NULL,
     CMDID_RENUMBER_OBJECT_IDS },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo animateMenuInfo={ TGMUTYPE_TEXT, animateMenuItemInfo,
                                    TgCreateMenuFromMenuInfo };

static TgMenuItemInfo portMenuItemInfo[] = {
   { N_("Connect Two Ports by a Wire..."), NULL,
     N_("Connect two ports by a wire"), NULL,
     CMDID_CONNECT2PORTSBYWIRE },
   { N_("Repeat Connect Two Ports by a Wire..."), NULL,
     N_("Repeatedly connecting two ports by a wire (will not prompt if the wire name can be detected).  Wire signal name will not be shown."), NULL,
     CMDID_REPEATCONNECT2PORTSBYWIRE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Show Wire Signal Name"), NULL,
     N_("Toggle between showing (with placing) and hiding the wire signal name when connecting ports"), NULL,
     CMDID_TOGGLESHOWWIRESIGNALNAME },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Merge Ports with An Object"), NULL,
     N_("Merge selected floating port objects with a selected composite object"), NULL,
     CMDID_MERGEPORTSWITHANOBJECT },
   { N_("Connect Ports to a Broadcast Wire"), NULL,
     N_("Connect selected floating port objects to a selected broadcast-type wire"), NULL,
     CMDID_CONNECTPORTSTOBROADCAST },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Clear Signal Name for a Port..."), NULL,
     N_("Clear the 'signal_name=' attribute for a port"), NULL,
     CMDID_CLEARSIGNAMEFORAPORT },
   { N_("Rename Signal Name for a Port..."), NULL,
     N_("Rename the 'signal_name=' attribute for a port"), NULL,
     CMDID_RENAMESIGNAMEFORAPORT },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo portMenuInfo={ TGMUTYPE_TEXT, portMenuItemInfo,
                                 CreatePortMenu };

static TgMenuItemInfo tableMenuItemInfo[] = {
   { N_("Merge With Table..."), NULL,
     N_("Merge selected object with a table file"), NULL,
     CMDID_MERGEWITHTABLE },
   { N_("Export To Table..."), NULL,
     N_("Export attributes of selected objects to a table file"), NULL,
     CMDID_EXPORTTOTABLE },
   { NULL, NULL, NULL, NULL, INVALID }
};
static TgMenuInfo tableMenuInfo={ TGMUTYPE_TEXT, tableMenuItemInfo,
                                  TgCreateMenuFromMenuInfo };

static TgMenuItemInfo specialMenuItemInfo[] = {
   { N_("Make Symbolic"), "Alt+Cntrl+m",
     N_("Turn a selected grouped/icon object into a symbol object"), NULL,
     CMDID_MAKESYMBOLIC },
   { N_("UnMake Symbolic"), "Alt+Cntrl+n",
     N_("Turn a selected symbol object into a grouped object"), NULL,
     CMDID_UNMAKESYMBOLIC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Instantiate..."), "Cntrl+i",
     N_("Instantiate a building-block object from the current domain"), NULL,
     CMDID_INSTANTIATE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Make Iconic..."), "Alt+Cntrl+i",
     N_("Turn a selected grouped/symbol object into an icon object"), NULL,
     CMDID_MAKEICONIC },
   { N_("UnMake Iconic"), "Alt+Cntrl+j",
     N_("Turn an icon object into a grouped object"), NULL,
     CMDID_UNMAKEICONIC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Push"), "Alt+Cntrl+y",
     N_("Push into (edit) the symbol file which defines the selected icon object"),
     NULL, CMDID_PUSH },
   { N_("Pop"), "Cntrl+k",
     N_("Pop back to a high lever (reverse of Push)"), NULL,
     CMDID_POP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Attribute"), TGMUITEM_SUBMENU,
     N_("Attribute submenu >>>"), &attrMenuInfo, INVALID },
   { N_("File Attribute"), TGMUITEM_SUBMENU,
     N_("File attribute submenu >>>"), &fileAttrMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Update Symbols"), "Alt+Cntrl+u",
     N_("Refresh selected icon objects from their definition files"), NULL,
     CMDID_UPDATESYMBOLS },
   { N_("Save Sym In Library..."), NULL,
     N_("Save symbol file in a library path within the current domain"), NULL,
     CMDID_SAVESYMINLIBRARY },
   { N_("Replace Graphic"), NULL,
     N_("Replace the graphical part of selected objects by the graphical object in the cut buffer"), NULL,
     CMDID_REPLACEGRAPHIC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Domain"), TGMUITEM_SUBMENU,
     N_("Domain submenu >>>"), &domainMenuInfo, INVALID },
   { N_("Ports and Signals"), TGMUITEM_SUBMENU,
     N_("Ports and Signal submenu >>>"), &portMenuInfo, INVALID },
   { N_("Table"), TGMUITEM_SUBMENU,
     N_("Table submenu >>>"), &tableMenuInfo, INVALID },
   { N_("Misc"), TGMUITEM_SUBMENU,
     N_("Misc submenu >>>"), &animateMenuInfo, INVALID },
   { N_("Return"), TGMUITEM_SUBMENU,
     N_("Return submenu >>>"), &driverMenuInfo, INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Exec Cmds From File..."), NULL,
     N_("Execute internal commands from a file"), NULL,
     CMDID_EXECCMDSFROMFILE },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo specialMenuInfo={ TGMUTYPE_TEXT, specialMenuItemInfo,
                             TgCreateMenuFromMenuInfo };

static TgMenuItemInfo helpMenuItemInfo[] = {
   { N_("Latest Release Info"), NULL,
     N_("Find out what is the version number of the most recently released tgif"),
     NULL, CMDID_LATESTRELEASEINFO },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Copyright"), NULL, N_("Copyright information"), NULL,
     CMDID_COPYRIGHT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("About"), NULL,
     N_("Version information, where to send bug reports, etc."), NULL,
     CMDID_ABOUT },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo helpMenuInfo={ TGMUTYPE_TEXT, helpMenuItemInfo,
                          TgCreateMenuFromMenuInfo };

/* ------------------ context menu ------------------ */

TgMenuInfo editAttrInEditorMenuInfo={ TGMUTYPE_TEXT, NULL,
                                      CreateEditAttrInEditorMenu };

TgMenuInfo editAttrGroupInEditorMenuInfo={ TGMUTYPE_TEXT, NULL,
                                           CreateEditAttrGroupInEditorMenu };

TgMenuInfo getPropertyMenuInfo={ TGMUTYPE_TEXT, NULL, CreateGetPropertyMenu };

TgMenuInfo peekDimensionMenuInfo={ TGMUTYPE_TEXT, NULL,
                                   CreatePeekDimensionMenu };

static TgMenuItemInfo baseContextMenuItemInfo[] = {
   { N_("Mode"), TGMUITEM_PINNABLESUBMENU, N_("Mode Menu"), &modeMenuInfo,
     MENU_MODE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Copy"), "Cntrl+c", N_("Copy selected objects into the cut buffer"),
     NULL, CMDID_COPY },
   { N_("Duplicate"), "Cntrl+d", N_("Duplicate selected objects"), NULL,
     CMDID_DUPLICATE },
   { N_("Delete"), "Cntrl+x", N_("Delete selected objects"), NULL,
     CMDID_DELETE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties Menu"), &propertiesMenuInfo, MENU_PROPERTIES },
   { N_("Image Proc"), TGMUITEM_PINNABLESUBMENU,
     N_("ImageProc Menu"), &imageProcMenuInfo, MENU_IMAGEPROC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Edit Attribute In Editor"), TGMUITEM_SUBMENU,
     N_("Edit attribute in editor submenu >>>"), &editAttrInEditorMenuInfo,
     INVALID },
   { N_("Edit Attribute Group In Editor"), TGMUITEM_SUBMENU,
     N_("Edit a group of attributes in editor submenu >>>"),
     &editAttrGroupInEditorMenuInfo, INVALID },
   { N_("Get Property"), TGMUITEM_SUBMENU,
     N_("Get property submenu >>>"), &getPropertyMenuInfo, INVALID },
   { N_("Peek Dimension"), TGMUITEM_SUBMENU,
     N_("Peek dimension submenu >>>"), &peekDimensionMenuInfo, INVALID },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo baseContextMenuInfo={ TGMUTYPE_TEXT, baseContextMenuItemInfo,
                                 CreateContextMenu };

static TgMenuItemInfo editTextContextMenuItemInfo[] = {
   { N_("Mode"), TGMUITEM_PINNABLESUBMENU, N_("Mode Menu"), &modeMenuInfo,
     MENU_MODE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Copy"), "Cntrl+c", N_("Copy selected objects into the cut buffer"),
     NULL, CMDID_COPY },
   { N_("Copy Plain Text As Object"), NULL,
     N_("Copy highlighted plain text as an object into the cut buffer"), NULL,
     CMDID_COPYPLAINTEXTASOBJECT },
   { N_("Cut"), NULL, N_("Cut selected text into the cut buffer"), NULL,
     CMDID_CUT },
   { N_("Paste"), "Cntrl+v", N_("Paste from the cut buffer"), NULL,
     CMDID_PASTE },
   { N_("Duplicate"), "Cntrl+d", N_("Duplicate selected objects"), NULL,
     CMDID_DUPLICATE },
   { N_("Delete"), "Cntrl+x", N_("Delete selected objects"), NULL,
     CMDID_DELETE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Move Edit Text Box..."), NULL, N_("Move the edit text box"), NULL,
     CMDID_MOVEEDITTEXTBOX },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties Menu"), &propertiesMenuInfo, MENU_PROPERTIES },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo editTextContextMenuInfo={ TGMUTYPE_TEXT, editTextContextMenuItemInfo,
                                     CreateEditTextContextMenu };

/* ------------------ grid menu ------------------ */

static TgMenuItemInfo gridMenuItemInfo[] = {
   { N_("+Grid"), "Alt+i", N_("Increment grid size"), NULL, CMDID_INCGRID },
   { N_("-Grid"), "Alt+d", N_("Decrement grid size"), NULL, CMDID_DECGRID },
   { N_("Visible Grid"), "Alt+g", N_("Hide/show grid lines"), NULL,
     CMDID_TOGGLEGRID },
   { N_("Snap To Grid"), "Alt+Cntrl+g",
     N_("Toggle the snapping to grid points effect"), NULL, CMDID_TOGGLESNAP },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Save Origin"), NULL,
     N_("Save current location as the 'origin' of the drawing"),
     NULL, CMDID_SAVEORIGIN },
   { N_("Scroll To Origin"), "Alt+^",
     N_("Scroll to the 'origin' of the drawing"), NULL, CMDID_SCROLLTOORIG },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Zoom"), TGMUITEM_SUBMENU, N_("Zoom submenu >>>"), &zoomMenuInfo,
     INVALID },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Set Reduction..."), "Alt+%",
     N_("Specify reduction/magnification of the whole drawing"), NULL,
     CMDID_SETREDUCTION },
   { N_("Metric Grid"), "Alt+Cntrl+-",
     N_("Toggle between English and Metric grid systems"), NULL,
     CMDID_TOGGLEGRIDSYSTEM },
   { N_("Enable Right Margin"), NULL,
     N_("Toggle between using (simple) right margin or not"), NULL,
     CMDID_TOGGLERIGHTMARGIN },
   { N_("Specify Right Margin..."), NULL,
     N_("Specify the (simple) right margin"), NULL,
     CMDID_SPECIFYRIGHTMARGIN },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo gridMenuInfo={ TGMUTYPE_TEXT, gridMenuItemInfo, CreateGridMenu };

/* ------------------ main menu ------------------ */

static TgMenuItemInfo mainMenuItemInfo[] = {
   { N_("File"), TGMUITEM_PINNABLESUBMENU, N_("File Menu"), &fileMenuInfo,
     MENU_FILE },
   { N_("Edit"), TGMUITEM_PINNABLESUBMENU, N_("Edit Menu"), &editMenuInfo,
     MENU_EDIT },
   { N_("Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("Layout Menu"), &layoutMenuInfo, MENU_LAYOUT },
   { N_("Arrange"), TGMUITEM_PINNABLESUBMENU,
     N_("Arrange Menu"), &arrangeMenuInfo, MENU_ARRANGE },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties Menu"), &propertiesMenuInfo, MENU_PROPERTIES },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Stacked Page"), TGMUITEM_PINNABLESUBMENU,
     N_("StackedPage Menu"), &stackedPageMenuInfo, MENU_STACKEDPAGE },
   { N_("Tiled Page"), TGMUITEM_PINNABLESUBMENU,
     N_("TiledPage Menu"), &tiledPageMenuInfo, MENU_TILEDPAGE },
   { N_("Page Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("PageLayout Menu"), &pageLayoutMenuInfo, MENU_PAGELAYOUT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Horizontal Align"), TGMUITEM_PINNABLESUBMENU,
     N_("HoriAlign Menu"), &horiAlignMenuInfo, MENU_HORIALIGN },
   { N_("Vertical Align"), TGMUITEM_PINNABLESUBMENU,
     N_("VertAlign Menu"), &vertAlignMenuInfo, MENU_VERTALIGN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Font"), TGMUITEM_PINNABLESUBMENU, N_("Font Menu"), &fontMenuInfo,
     MENU_FONT },
   { N_("Text Style"), TGMUITEM_PINNABLESUBMENU,
     N_("TextStyle Menu"), &fontStyleMenuInfo, MENU_STYLE },
   { N_("Text Size"), TGMUITEM_PINNABLESUBMENU,
     N_("TextSize Menu"), &fontSizeMenuInfo, MENU_SIZE },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Line Dash"), TGMUITEM_PINNABLESUBMENU,
     N_("LineDash Menu"), &lineDashMenuInfo, MENU_LINEDASH },
   { N_("Line Style"), TGMUITEM_PINNABLESUBMENU,
     N_("LineStyle Menu"), &lineStyleMenuInfo, MENU_LINESTYLE },
   { N_("Line Type"), TGMUITEM_PINNABLESUBMENU,
     N_("LineType Menu"), &lineTypeMenuInfo, MENU_LINETYPE },
   { N_("Line Width"), TGMUITEM_PINNABLESUBMENU,
     N_("LineWidth Menu"), &lineWidthMenuInfo, MENU_LINEWIDTH },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Fill"), TGMUITEM_PINNABLESUBMENU, N_("Fill Menu"), &fillMenuInfo,
     MENU_FILL },
   { N_("Pen"), TGMUITEM_PINNABLESUBMENU, N_("Pen Menu"), &penMenuInfo,
     MENU_PEN },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Move Mode"), TGMUITEM_PINNABLESUBMENU,
     N_("MoveMode Menu"), &moveModeMenuInfo, MENU_MOVEMODE },
   { N_("Shape"), TGMUITEM_PINNABLESUBMENU,
     N_("Shape Menu"), &shapeMenuInfo, MENU_SHAPE },
   { N_("Stretchable Text"), TGMUITEM_PINNABLESUBMENU,
     N_("StretchText Menu"), &stretchableTextModeMenuInfo, MENU_STRETCHTEXT },
   { N_("Transparent Pattern"), TGMUITEM_PINNABLESUBMENU,
     N_("TransPattern Menu"), &transPatModeMenuInfo,
         MENU_TRANSPAT },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Image Proc"), TGMUITEM_PINNABLESUBMENU,
     N_("ImageProc Menu"), &imageProcMenuInfo, MENU_IMAGEPROC },
   { TGMUITEM_SEPARATOR, NULL, NULL, NULL, INVALID },
   { N_("Color"), TGMUITEM_PINNABLESUBMENU,
     N_("Color Menu"), &colorMenuInfo, MENU_COLOR },
   { N_("Navigate"), TGMUITEM_PINNABLESUBMENU,
     N_("Navigate Menu"), &navigateMenuInfo, MENU_NAVIGATE },
   { N_("Special"), TGMUITEM_PINNABLESUBMENU,
     N_("Special Menu"), &specialMenuInfo, MENU_SPECIAL },
   { N_("Tangram-II"), TGMUITEM_PINNABLESUBMENU,
     N_("Tangram-II Menu"), &tangram2MenuInfo, MENU_TANGRAM2 },
   { N_("Help"), TGMUITEM_PINNABLESUBMENU, N_("Help Menu"), &helpMenuInfo,
     MENU_HELP },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo mainMenuInfo={ TGMUTYPE_TEXT, mainMenuItemInfo, CreateMainMenu };

/* ------------------ menubar ------------------ */

/*
 * Don't put any separators here!
 */

static TgMenuItemInfo minimalMenubarMenuItemInfo[] = {
   { N_("File"), TGMUITEM_PINNABLESUBMENU, N_("File Menu"), &fileMenuInfo,
     MENU_FILE },
   { N_("Edit"), TGMUITEM_PINNABLESUBMENU, N_("Edit Menu"), &editMenuInfo,
     MENU_EDIT },
   { N_("Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("Layout Menu"), &layoutMenuInfo, MENU_LAYOUT },
   { N_("Arrange"), TGMUITEM_PINNABLESUBMENU,
     N_("Arrange Menu"), &arrangeMenuInfo, MENU_ARRANGE },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties Menu"), &propertiesMenuInfo, MENU_PROPERTIES },
   { N_("Color"), TGMUITEM_PINNABLESUBMENU,
     N_("Color Menu"), &colorMenuInfo, MENU_COLOR },
   { N_("Navigate"), TGMUITEM_PINNABLESUBMENU,
     N_("Navigate Menu"), &navigateMenuInfo, MENU_NAVIGATE },
   { N_("Special"), TGMUITEM_PINNABLESUBMENU,
     N_("Special Menu"), &specialMenuInfo, MENU_SPECIAL },
   { N_("Tangram-II"), TGMUITEM_PINNABLESUBMENU,
     N_("Tangram-II Menu"), &tangram2MenuInfo, MENU_TANGRAM2 },
   { N_("Help"), TGMUITEM_PINNABLESUBMENU, N_("Help Menu"), &helpMenuInfo,
     MENU_HELP },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo minimalMenubarMenuInfo={ TGMUTYPE_TEXT, minimalMenubarMenuItemInfo,
                                    NULL };

static TgMenuItemInfo maximalMenubarMenuItemInfo[] = {
   { N_("File"), TGMUITEM_PINNABLESUBMENU, N_("File Menu"), &fileMenuInfo,
     MENU_FILE },
   { N_("Edit"), TGMUITEM_PINNABLESUBMENU, N_("Edit Menu"), &editMenuInfo,
     MENU_EDIT },
   { N_("Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("Layout Menu"), &layoutMenuInfo, MENU_LAYOUT },
   { N_("Arrange"), TGMUITEM_PINNABLESUBMENU,
     N_("Arrange Menu"), &arrangeMenuInfo, MENU_ARRANGE },
   { N_("Properties"), TGMUITEM_PINNABLESUBMENU,
     N_("Properties Menu"), &propertiesMenuInfo, MENU_PROPERTIES },
   { N_("Move Mode"), TGMUITEM_PINNABLESUBMENU,
     N_("MoveMode Menu"), &moveModeMenuInfo, MENU_MOVEMODE },
   { N_("Page"), TGMUITEM_PINNABLESUBMENU,
     N_("Page Menu"), &pageMenuInfo, MENU_PAGE },
   { N_("Page Layout"), TGMUITEM_PINNABLESUBMENU,
     N_("PageLayout Menu"), &pageLayoutMenuInfo, MENU_PAGELAYOUT },
   { N_("Horizontal Align"), TGMUITEM_PINNABLESUBMENU,
     N_("HoriAlign Menu"), &horiAlignMenuInfo, MENU_HORIALIGN },
   { N_("Vertical Align"), TGMUITEM_PINNABLESUBMENU,
     N_("VertAlign Menu"), &vertAlignMenuInfo, MENU_VERTALIGN },
   { N_("Font"), TGMUITEM_PINNABLESUBMENU, N_("Font Menu"), &fontMenuInfo,
     MENU_FONT },
   { N_("Text Style"), TGMUITEM_PINNABLESUBMENU,
     N_("TextStyle Menu"), &fontStyleMenuInfo, MENU_STYLE },
   { N_("Text Size"), TGMUITEM_PINNABLESUBMENU,
     N_("TextSize Menu"), &fontSizeMenuInfo, MENU_SIZE },
   { N_("Shape"), TGMUITEM_PINNABLESUBMENU,
     N_("Shape Menu"), &shapeMenuInfo, MENU_SHAPE },
   { N_("Stretchable Text"), TGMUITEM_PINNABLESUBMENU,
     N_("StretchText Menu"), &stretchableTextModeMenuInfo, MENU_STRETCHTEXT },
   { N_("Line Dash"), TGMUITEM_PINNABLESUBMENU,
     N_("LineDash Menu"), &lineDashMenuInfo, MENU_LINEDASH },
   { N_("Line Style"), TGMUITEM_PINNABLESUBMENU,
     N_("LineStyle Menu"), &lineStyleMenuInfo, MENU_LINESTYLE },
   { N_("Line Type"), TGMUITEM_PINNABLESUBMENU,
     N_("LineType Menu"), &lineTypeMenuInfo, MENU_LINETYPE },
   { N_("Line Width"), TGMUITEM_PINNABLESUBMENU,
     N_("LineWidth Menu"), &lineWidthMenuInfo, MENU_LINEWIDTH },
   { N_("Fill"), TGMUITEM_PINNABLESUBMENU, N_("Fill Menu"), &fillMenuInfo,
     MENU_FILL },
   { N_("Pen"), TGMUITEM_PINNABLESUBMENU, N_("Pen Menu"), &penMenuInfo,
     MENU_PEN },
   { N_("Transparent Pattern"), TGMUITEM_PINNABLESUBMENU,
     N_("TransPattern Menu"), &transPatModeMenuInfo, MENU_TRANSPAT },
   { N_("Color"), TGMUITEM_PINNABLESUBMENU,
     N_("Color Menu"), &colorMenuInfo, MENU_COLOR },
   { N_("Image Proc"), TGMUITEM_PINNABLESUBMENU,
     N_("ImageProc Menu"), &imageProcMenuInfo, MENU_IMAGEPROC },
   { N_("Navigate"), TGMUITEM_PINNABLESUBMENU,
     N_("Navigate Menu"), &navigateMenuInfo, MENU_NAVIGATE },
   { N_("Special"), TGMUITEM_PINNABLESUBMENU,
     N_("Special Menu"), &specialMenuInfo, MENU_SPECIAL },
   { N_("Tangram-II"), TGMUITEM_PINNABLESUBMENU,
     N_("Tangram-II Menu"), &tangram2MenuInfo, MENU_TANGRAM2 },
   { N_("Help"), TGMUITEM_PINNABLESUBMENU, N_("Help Menu"), &helpMenuInfo,
     MENU_HELP },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo maximalMenubarMenuInfo={ TGMUTYPE_TEXT, maximalMenubarMenuItemInfo,
                                    NULL };

