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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/strtbl.c,v 1.135 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_STRTBL_C_

#include "tgifdefs.h"

#include "choice.e"
#include "drawing.e"
#include "msg.e"
#include "poly.e"
#include "strtbl.e"
#include "special.e"
#include "text.e"
#include "util.e"

static char *gaszCachedStrTable[MAXCACHEDSTIDS];

static char *gaszCachedEnglishStrTable[] = {
   /* CSTID_OK */
   N_("Ok"),
   /* CSTID_CANCEL */
   N_("Cancel"),
   /* CSTID_PARANED_NONE */
   N_("(none)"),
   /* CSTID_SET_FONT_TO */
   N_("Set font to '%s'"),
   /* CSTID_NO_OBJ_SELECTED */
   N_("No object selected."),
   /* CSTID_SPECIFY_ARC_END */
   N_("Please specify the end of an arc."),
   /* CSTID_SPECIFY_ARC_START */
   N_("Please specify the start of an arc."),
   /* CSTID_SPECIFY_ARC_THIRD */
   N_("Please specify a 3rd point on the arc."),
   /* CSTID_SET_ARC_END */
   N_("Set end of arc"),
   /* CSTID_SET_ARC_START */
   N_("Set start of arc"),
   /* CSTID_SET_ARC_THIRD */
   N_("Set 3rd point on arc"),
   /* CSTID_SPECIFY_ARC_END_CW */
   N_("Please specify the end of the arc (clockwise)."),
   /* CSTID_SPECIFY_ARC_END_CCW */
   N_("Please specify the end of the arc (counter-clockwise)."),
   /* CSTID_CANCEL_NOT_AVAIL */
   N_("CANCEL is not a valid option."),
   /* CSTID_DLG_DEF_CONTINUE */
   N_("( <CR> or <ESC> to continue )"),
   /* CSTID_DLG_ACCEPT_CANCEL */
   N_("( <CR>: accept, <ESC>: cancel )"),
   /* CSTID_MALFORMED_INPUT_STR */
   N_("Malformed input '%s'."),
   /* CSTID_CACHING_TEXT_BMP */
   N_("Caching text bitmap..."),
   /* CSTID_PROGRESS_PERCENT */
   N_("Progress: %1d%%"),
   /* CSTID_FINISHED_CACHEING_TEXT_BMP */
   N_("Finishing caching text bitmap..."),
   /* CSTID_BAD_BOX_BBOX_ADJUSTED */
   N_("Bad box bounding box.  Adjusted."),
   /* CSTID_MAIN_MENU */
   N_("Main Menu"),
   /* CSTID_MODE_MENU */
   N_("Mode Menu"),
   /* CSTID_ROTATE_DEGREES_CW */
   N_("Rotate %s degrees clock-wise"),
   /* CSTID_ROTATE_DEGREES_CCW */
   N_("Rotate %s degrees counter-clock-wise"),
   /* CSTID_PRINT_MODE */
   N_("%s print mode"),
   /* CSTID_EXPORT_MODE */
   N_("%s export mode"),
   /* CSTID_PRINT_EXPORT_MENU */
   N_("Print/ExportFormat Menu"),
   /* CSTID_FLUSH_UNDO_PERCENT */
   N_("Flushing undo buffers... %1d%%"),
   /* CSTID_SET_COLOR_TO */
   N_("Set color to '%s'"),
   /* CSTID_CANNOT_ALLOC_COLOR_USE_ALT */
   N_("Cannot allocate color '%s', use '%s' instead."),
   /* CSTID_UPDATING_PIXELS */
   N_("Updating pixels (%1d - %1dx%1d)..."),
   /* CSTID_NONE */
   N_("none"),
   /* CSTID_PARANED_NULL */
   N_("null"),
   /* CSTID_COPY */
   N_("Copy"),
   /* CSTID_SAVE */
   N_("Save"),
   /* CSTID_PASTE */
   N_("Paste"),
   /* CSTID_RESTORE */
   N_("Restore"),
   /* CSTID_YES */
   N_("Yes"),
   /* CSTID_NO */
   N_("No"),
   /* CSTID_EXTRA */
   N_("Extra"),
   /* CSTID_INTERRUPT */
   N_("interrupt"),
   /* CSTID_LEAVE_HYPERSPACE */
   N_("Leave HyperSpace"),
   /* CSTID_ENTER_HYPERSPACE */
   N_("Enter HyperSpace"),
   /* CSTID_CACHING_IMAGE */
   N_("Caching images..."),
   /* CSTID_TOGGLE_SMOOTH_HINGE */
   N_("Toggle smooth/hinge points"),
   /* CSTID_FINISH */
   N_("Finish"),
   /* CSTID_DEL_A_VERTEX */
   N_("Delete a vertex"),
   /* CSTID_ADD_A_VERTEX */
   N_("Add a vertex"),
   /* CSTID_START_LAYOUT_ON_ARC */
   N_("Start layout on arc"),
   /* CSTID_END_LAYOUT_ON_ARC */
   N_("End layout on arc"),
   /* CSTID_EXECUTING_GIVEN_PROGRAM */
   N_("Executing '%s'..."),
   /* CSTID_DOTS_DONE */
   N_("...Done"),
   /* CSTID_IMPORTING_FILE */
   N_("Importing '%s'..."),
   /* CSTID_LOADING_FILE */
   N_("Loading '%s'..."),
   /* CSTID_EDIT_TEXT_SIZE_SUBSTR */
   N_("edit text size"),
   /* CSTID_SET_FONT_SIZE_TO_PT */
   N_("Set font size to %1dpt"),
   /* CSTID_SET_FONT_SIZE_TO */
   N_("Set font size to %1d"),
   /* CSTID_MAKING_CONN_TO_HOST */
   N_("%s: making connection to '%s'..."),
   /* CSTID_CONN_TO_HOST_ESTABLISHED */
   N_("%s: connection to '%s' established."),
   /* CSTID_LOGIN_SUCC_RETRIEVE_DATA */
   N_("%s: login successful; retrieving data..."),
   /* CSTID_LAYOUT_MENU */
   N_("Layout Menu"),
   /* CSTID_MAKING_CONN_TO_HOST_PORT */
   N_("%s: making connection to '%s:%1d'..."),
   /* CSTID_CONN_TO_HOST_PORT_ESTB */
   N_("%s: connection to '%s:%1d' established."),
   /* CSTID_SENDING_REQUESTS_DOTS */
   N_("%s: sending requests..."),
   /* CSTID_PARANED_UNKNOWN */
   N_("(unknown)"),
   /* CSTID_READING_RESPONSE_DOTS */
   N_("%s: reading responses..."),
   /* CSTID_RESPONSES_RECEIVED */
   N_("%s: responses received."),
   /* CSTID_GEN_FINAL_PIXELS_PERC */
   N_("Generating final pixels: %1d%%"),
   /* CSTID_BUILDING_HISTOGRAM_DOTS */
   N_("Building histogram..."),
   /* CSTID_HISTOGRAM_PERC */
   N_("Histogram: %1d%%"),
   /* CSTID_COMP_NEW_PIXELS_PERC */
   N_("Computing new pixels: %1d%%"),
   /* CSTID_QUANTIZING_COLORS_DOTS */
   N_("Quantizing from %1d to %1d colors..."),
   /* CSTID_REMAPPING_COLORS_DOTS */
   N_("Remapping Colors..."),
   /* CSTID_COMPUTING_NEW_PIXELS_PERC */
   N_("Computing new pixels: %1d%%"),
   /* CSTID_CALC_ERR_DIFF_PIX_VAL_PERC */
   N_("Calculating error diffused pixel values: %1d%%"),
   /* CSTID_CALC_NEW_PIX_1_VAL_PERC */
   N_("Calculating new pixel values (pass 1): %1d%%"),
   /* CSTID_CALC_NEW_PIX_2_VAL_PERC */
   N_("Calculating new pixel values (pass 2): %1d%%"),
   /* CSTID_DLG_ACCEPT_CONTINUE */
   N_("( <CR>: accept, <ESC>: continue )"),
   /* CSTID_OPENING_GIVEN */
   N_("Opening '%s'..."),
   /* CSTID_BACKGROUNDING_CMD */
   N_("Backgrounding; '%s'."),
   /* CSTID_CNTRL_C_TO_INTR_AND_ABEND */
   N_("Type ^C if you want to interrupt and end abnormally..."),
   /* CSTID_MOVE_PINNED_MENU */
   N_("Move pinned menu"),
   /* CSTID_CLOSE_PINNED_MENU */
   N_("Close pinned menu"),
   /* CSTID_RELEASE_MOUSE_TO_PIN_MENU */
   N_("Release the mouse to pin down the menu"),
   /* CSTID_LOCKED_OBJS_NOT_MOVED */
   N_("Locked objects are not moved."),
   /* CSTID_SCROLL_DOWN_1_LINE */
   N_("scroll down 1 line"),
   /* CSTID_SCROLL_AROUND */
   N_("scroll around"),
   /* CSTID_SCROLL_UP_1_LINE */
   N_("scroll up 1 line"),
   /* CSTID_ENABLE_REDRAW */
   N_("enable redraw"),
   /* CSTID_DISABLE_REDRAW */
   N_("disable redraw"),
   /* CSTID_GET_DIR_LIST_PLEASE_WAIT */
   N_("Getting directory listing of '%s', please wait..."),
   /* CSTID_ENTERING_HYPERSPACE */
   N_("Entering hyperspace..."),
   /* CSTID_GOTO */
   N_("Goto"),
   /* CSTID_DELETE */
   N_("Delete"),
   /* CSTID_CLOSE */
   N_("Close"),
   /* CSTID_BAD_OVAL_BBOX_ADJ */
   N_("Bad oval bounding box.  Adjusted."),
   /* CSTID_SHIFT_ALL_TABS_RIGHT */
   N_("Shift All Tabs Right"),
   /* CSTID_SHIFT_TABS_RIGHT */
   N_("Shift Tabs Right"),
   /* CSTID_SHIFT_TABS_LEFT */
   N_("Shift Tabs Left"),
   NULL
};

static char *gaszCachedEnglishStrTable100[] = {
   /* CSTID_SHIFT_ALL_TABS_LEFT */
   N_("Shift All Tabs Left"),
   /* CSTID_SHIFT_10_TABS_RIGHT */
   N_("Shift 10 Tabs Right"),
   /* CSTID_SHIFT_10_TABS_LEFT */
   N_("Shift 10 Tabs Left"),
   /* CSTID_PAGE_MENU */
   N_("Page Menu"),
   /* CSTID_GOTO_GIVEN_PAGE */
   N_("Goto page %1d"),
   /* CSTID_GOTO_GIVEN_PAGE_WITH_NAME */
   N_("Goto page %1d: \"%s\""),
   /* CSTID_GRAYSTR_CALLED_WITH_IDX_2 */
   N_("Error:  GrayStr() called with index <= 2."),
   /* CSTID_GRAY_SCALE_USED_FOR_PAT */
   N_("Warning:  Gray scales used instead of tiling patterns."),
   /* CSTID_SET_FILL_TO_NONE */
   N_("Set fill pattern to NONE"),
   /* CSTID_SET_FILL_TO_SOLID */
   N_("Set fill pattern to SOLID"),
   /* CSTID_SET_FILL_TO_BACKGROUND */
   N_("Set fill pattern to BACKGROUND"),
   /* CSTID_SET_FILL_TO_PAT_NUMBER */
   N_("Set fill pattern to pattern #%1d"),
   /* CSTID_SET_LINE_WIDTH_TO_GIVEN_S */
   N_("Set line width to %s"),
   /* CSTID_SET_PEN_DASH_PAT_NO_DASH */
   N_("Set dash pattern for the pen to be no dashes"),
   /* CSTID_SET_PEN_DASH_PAT_PAT_NUM */
   N_("Set dash pattern for the pen to dash pattern #%1d"),
   /* CSTID_SET_PEN_TO_NONE */
   N_("Set pen to NONE"),
   /* CSTID_SET_PEN_TO_SOLID */
   N_("Set pen to SOLID"),
   /* CSTID_SET_PEN_TO_BACKGROUND */
   N_("Set pen to BACKGROUND"),
   /* CSTID_SET_PEN_TO_PAT_NUM */
   N_("Set pen to pattern #%1d"),
   /* CSTID_SEL_A_PIN_ESC_TO_END_DOTS */
   N_("Please select a pin (or press <ESC> to end)..."),
   /* CSTID_ADD_LAST_VERTEX */
   N_("Add last vertex"),
   /* CSTID_SINGLE_PT_POLY_CONVERTED */
   N_("Single point poly (%1d,%1d) converted to double point poly."),
   /* CSTID_BAD_SMOOTHHINGE_POLY_FIXED */
   N_("Invalid poly smooth/hinge specification detected.  Fixed."),
   /* CSTID_CLOSE_POLYGON_SPLINE */
   N_("Close the polygon/spline"),
   /* CSTID_SNGL_PT_POLYGON_CONVERTED */
   N_("Single point polygon (%1d,%1d) converted to 3-point point polygon."),
   /* CSTID_TWO_PT_POLYGON_CONVERTED */
   N_("Two-point polygon [%1d,%1d,%1d,%1d] converted to 3-point point polygon."),
   /* CSTID_FILEVER_TOO_LARGE_FOR_RCBOX */
   N_("Invalid file version (=%1d) for rcbox."),
   /* CSTID_BAD_RCBOX_BBOX_ADJUSTED */
   N_("Bad rcbox bounding box.  Adjusted."),
   /* CSTID_PARANED_SEC_ELAPSED_AVG_SPEED */
   N_("(%1d sec elapsed, avg %.2fKB/sec)"),
   /* CSTID_UNZIPPING_WITH_CMD_DOTS */
   N_("Unzipping with '%s'..."),
   /* CSTID_UNCOMPRESSING_WITH_CMD_DOTS */
   N_("Uncompressing with '%s'..."),
   /* CSTID_DATA_RECEIVED */
   N_("%s: data received."),
   /* CSTID_FORWARDING_URL_TO_URL */
   N_("%s: forwarding '%s' to '%s'."),
   /* CSTID_SCROLL_UP */
   N_("scroll up"),
   /* CSTID_SCROLL_DOWN */
   N_("scroll down"),
   /* CSTID_SCROLL_PAGE_UP */
   N_("scroll page up"),
   /* CSTID_SCROLL_PAGE_DOWN */
   N_("scroll page down"),
   /* CSTID_SCROLL_VERTICALLY */
   N_("scroll vertically"),
   /* CSTID_SCROLL_HORIZONTALLY */
   N_("scroll horizontally"),
   /* CSTID_SCROLL_LEFT */
   N_("scroll left"),
   /* CSTID_SCROLL_RIGHT */
   N_("scroll right"),
   /* CSTID_SCROLL_PAGE_LEFT */
   N_("scroll page left"),
   /* CSTID_SCROLL_PAGE_RIGHT */
   N_("scroll page right"),
   /* CSTID_CANT_FIND_ATTR_WITH_COLOR */
   N_("Cannot find the '%s' attribute with color %s."),
   /* CSTID_CANT_FIND_FILEATTR_WITH_COLOR */
   N_("Cannot find the '%s' file attribute with color %s."),
   /* CSTID_CANT_FIND_NAMED_ATTR */
   N_("Cannot find the '%s' attribute."),
   /* CSTID_CANT_FIND_INTERPRETABLE_ATTR */
   N_("Cannot find interpretable attribute."),
   /* CSTID_NO_ACTION_TAKEN */
   N_("No action taken."),
   /* CSTID_FAIL_READ_ICONPXMP_USE_BMP */
   N_("Fail to read IconPixmap file: '%s'.  Default icon bitmap is used."),
   /* CSTID_DRAG_MOUSE_CREATE_SHAPE_DOTS */
   N_("Drag the mouse to create a shape..."),
   /* CSTID_CREATE_A_SHAPE_ABORTED */
   N_("Create a shape aborted."),
   /* CSTID_INSTANTIATING_FILE_DOTS */
   N_("Instantiating '%s'..."),
   /* CSTID_ATTR_PROMOTED_TO_NEW_SYMBOL */
   N_("Object's attributes are promoted to the new symbol object."),
   /* CSTID_ATTR_PROMOTED_TO_NEW_ICON */
   N_("Object's attributes are promoted to the new icon object."),
   /* CSTID_CREATING_FILE_DOTS */
   N_("Creating '%s'..."),
   /* CSTID_NAMED_FILE_CREATED */
   N_("File '%s' created."),
   /* CSTID_WRITING_ATTR_TO_NAMED_FILE */
   N_("Writing attributes to '%s'..."),
   /* CSTID_SEMICOLON_USED_AS_SEPARATOR */
   N_("';'s are used as separators."),
   /* CSTID_TAB_USED_AS_SEPARATOR */
   N_("<TAB>s are used as separators."),
   /* CSTID_PROCESSING_OBJ_NUMBER */
   N_("Processing object %1d..."),
   /* CSTID_PLACING_NAMED_OBJ */
   N_("Placing object '%s'..."),
   /* CSTID_WRITING_ATTRS_IN_TBL_FORM_TO */
   N_("Writing attributes in table form to '%s'..."),
   /* CSTID_CORRUPTED_POLY_MKMULTISPLINE */
   N_("Corrupted poly in MakeMultiSplinePolyVertex()."),
   /* CSTID_CORRUPTED_POLY_DMPMULTICURVE */
   N_("Corrupted poly in DumpMultiCurvedPolyPoints()."),
   /* CSTID_FIX_ATTEMPTED */
   N_("Fix attempted."),
   /* CSTID_PUSHING_INTO_NAMED */
   N_("Pushing into '%s'..."),
   /* CSTID_CUR_FILE_IS */
   N_("Current file is '%s'."),
   /* CSTID_DONT_KNOW_HOW_HANDLE_FRAME_CT */
   N_("Does not know how to handle frame Content-Type: %s."),
   /* CSTID_CANT_FIND_OBJ_NAME_ON_RESIZE */
   N_("Cannot find object named '%s' any more; on_resize script not invoked."),
   /* CSTID_PASTING_LINE_NUM_DOTS */
   N_("Pasting line %1d..."),
   /* CSTID_CANT_PASTE_DEL */
   N_("Cannot paste <DEL>."),
   /* CSTID_CANT_PASTE_BS */
   N_("Cannot paste <BS>."),
   /* CSTID_CANT_PASTE_ESC */
   N_("Cannot paste <ESC>."),
   /* CSTID_CANT_PASTE_TAB_REPLACE_BY_SPC */
   N_("Cannot paste <TAB>, replaced by <SPACE>."),
   /* CSTID_READING_FILE_DOTS */
   N_("Reading '%s'..."),
   /* CSTID_RETURNED_FROM_BASIC_DRIVER */
   N_("Returned from basic driver."),
   /* CSTID_CACHING_BITMAP */
   N_("Caching bitmap..."),
   /* CSTID_FINISHED_CACHEING_BITMAP */
   N_("Finishing caching bitmap..."),
   /* CSTID_NEW_BMP_SIZE_IS_W_X_H */
   N_("New bitmap size is %1dx%1d."),
   /* CSTID_BLD_COLOR_TBL_FROM_PIXEL_DOTS */
   N_("Building color table from pixels..."),
   /* CSTID_GENERATING_IMAGEMAP_FILE_DOTS */
   N_("Generating imagemap file..."),
   /* CSTID_GENERATING_HTML_FILE_DOTS */
   N_("Generating HTML file..."),
   /* CSTID_GENERATING_IMAGE */
   N_("Generating image..."),
   /* CSTID_FS_HALFTONE_BMP_THRESH_DOTS */
   N_("FS halftone bitmap (threshold=%s)..."),
   /* CSTID_THRESHOLD_BMP_THRESH_DOTS */
   N_("Threshold bitmap (threshold=%s)..."),
   /* CSTID_INVALID_FILEVER_FOR_XBM */
   N_("Invalid file version (%1d) for X11 Bitmap objects."),
   /* CSTID_FIELD_TOO_LONG_IN_OBJ_TRUNC */
   N_("%s, %d:  %s field too long in %s object.  Truncated"),
   /* CSTID_FILE_LINE_BAD_FLD_IN_OBJ_STR */
   N_("%s, %d:  Bad %s in %s object [%s]."),
   /* CSTID_MALFORMED_LINE_NUM_IN_FILE */
   N_("Malformed line %1d in '%s'."),
   /* CSTID_CACHING_PIXMAP */
   N_("Caching pixmap..."),
   /* CSTID_FINISHED_CACHEING_PIXMAP */
   N_("Finishing caching pixmap..."),
   /* CSTID_NEW_XPM_SIZE_IS_W_X_H */
   N_("New pixmap size is %1dx%1d."),
   /* CSTID_MANY_COLORS_GT_255_SKIP_XPM */
   N_("Too many colors (%1d, which is > 255) in an xpm objects.  This object is skipped during printing."),
   /* CSTID_GIVEN_ADDTNL_COLOR_ALLOCATED */
   N_("%1d additional color(s) allocated."),
   /* CSTID_INVALID_FILEVER_FOR_XPM */
   N_("Invalid file version (%1d) for X11 Pixmap objects."),
   /* CSTID_COLOR_SUBS_FOR_UNRECOG_IN_XPM */
   N_("'%s' is substituted for unrecognized colors in an X11 Pixmap object."),
   /* CSTID_CONTEXT_MENU */
   N_("Context Menu"),
   /* CSTID_GEN_TEMPLATE_HTML_FILE_DOT */
   N_("Generating HTML file through '%s' template..."),
   /* CSTID_SCALING_EVERYTHING_PAGE */
   N_("Scaling everything for page %1d of %1d..."),
   /* CSTID_SQUARE_BRACK_MODIFIED */
   N_("[Modified]"),
   NULL
};

static char *gaszCachedEnglishStrTable200[] = {
   /* CSTID_SQUARE_BRACK_MODIFIED_UNSAV */
   N_("[Modified/Unsavable]"),
   /* CSTID_SQUARE_BRACK_UNNAMED */
   N_("[Unnamed]"),
   /* CSTID_PARANED_EMPTY */
   N_("(Empty)"),
   /* CSTID_OPEN_FILE */
   N_("Open '%s'"),
   /* CSTID_INC_GRID_SIZE */
   N_("Increment grid size"),
   /* CSTID_DEC_GRID_SIZE */
   N_("Decrement grid size"),
   /* CSTID_GRID_MENU */
   N_("Grid menu"),
   /* CSTID_MOVE_EDIT_TEXT_BOX */
   N_("Move edit text box"),
   /* CSTID_START_MOVE_EDIT_TEXT_BOX */
   N_("Start moving the edit text box"),
   /* CSTID_EDIT_ATTR_IN_EDITOR */
   N_("Edit the value of the '%s' attribute in a text editor"),
   /* CSTID_GET_NAMED_PROP_FROM_SEL_OBJ */
   N_("Get the current '%s' property from the selected object"),
   /* CSTID_START_AN_EDGE_FROM_A_PORT */
   N_("start an edge from a port"),
   /* CSTID_ABORT */
   N_("abort"),
   /* CSTID_START_A_WIRE_FROM_NAMED_PORT */
   N_("Start a wire from port '%s'... (Btn2 or Btn3 to abort)"),
   /* CSTID_END_A_WIRE_AT_NAMED_PORT */
   N_("End the wire at port '%s'... (press <ESC> to abort)"),
   /* CSTID_SET_SIGNAME_FOR_NAMED_PORT */
   N_("Set signal name (currently '%s') for port '%s'..."),
   /* CSTID_CLK_ON_PORT_TO_RENAME_SIGNAME */
   N_("Please click on a port to rename its signal name..."),
   /* CSTID_CLK_ON_PORT_TO_CLEAR_SIGNAME */
   N_("Please click on a port to clear its signal name..."),
   /* CSTID_PORTNAME_TOO_LONG_SKIPPED */
   N_("Port/pin name '%s...' too long.  It is skipped in net-list generation."),
   /* CSTID_CLEAR_SIGNAME_FOR_NAMED_PORT */
   N_("Clear signal name (currently '%s') for port '%s'..."),
   /* CSTID_EDIT_ATTR_GROUP_IN_EDITOR */
   N_("Edit the '%s' attribute group (attribute list of '%s') in a text editor"),
   /* CSTID_INVALID_FILEVER_FOR_LINK_JPEG */
   N_("Invalid file version (%1d) for linked JPEG objects."),
   /* CSTID_NO_SUCH_FONT */
   N_("Warning: No such font: %s"),
   /* CSTID_ACCEPT */
   N_("Accept"),
   /* CSTID_ADD_A_STRUCT_VERTEX */
   N_("Add a vertex (drag to add control points)"),
   NULL
};

static
char *SetAndLoadCachedString(index)
   int index;
{
   if (index < 100) {
      gaszCachedStrTable[index] =
            UtilStrDup(_(gaszCachedEnglishStrTable[index]));
   } else if (index < 200) {
      gaszCachedStrTable[index] =
            UtilStrDup(_(gaszCachedEnglishStrTable100[index-100]));
   } else {
      gaszCachedStrTable[index] =
            UtilStrDup(_(gaszCachedEnglishStrTable200[index-200]));
   }
   if (gaszCachedStrTable[index] == NULL) FailAllocMessage();
   return gaszCachedStrTable[index];
}

char *TgLoadCachedString(nID)
   int nID;
   /*
    * Note: once such a string is loaded, it will never change.
    *       So, don't put any data-dependent stuff here.
    * Note: the returned string is translated already.
    */
{
   if (nID >= MINCACHEDSTIDS && nID < MAXCACHEDSTIDS) {
      int index=nID-MINCACHEDSTIDS;

      if (gaszCachedStrTable[index] != NULL) {
         return gaszCachedStrTable[index];
      }
      return SetAndLoadCachedString(index);
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   fprintf(stderr, "Cannot find cached string table entry with ID=%1d.\n", nID);
#endif /* _TGIF_DBG */

   return NULL;
}

static char *gaszStrTable[MAXNONCACHEDSTIDS];

static char *gaszEnglishStrTable[] = {
   /* STID_CANNOT_LOAD_FONTSET */
   N_("Cannot load fontset specified by %s.%s: '%s'."),
   /* STID_CANNOT_LOAD_FONT */
   N_("Cannot load font specified by %s.%s: '%s'."),
   /* STID_CANNOT_DISTRIBUTE_LOCKED_OBJS */
   N_("Cannot distribute with locked objects."),
   /* STID_VERTICES_HORI_APART */
   N_("Vertices are %1d pixels apart horizontally."),
   /* STID_VERTICES_VERT_APART */
   N_("Vertices are %1d pixels apart vertically."),
   /* STID_LEFT_SIDES_APART */
   N_("Left sides of objects are %1d pixels apart."),
   /* STID_CENTERS_APART */
   N_("Center of objects are %1d pixels apart."),
   /* STID_RIGHT_SIDES_APART */
   N_("Right sides of objects are %1d pixels apart."),
   /* STID_SPACED_APART_HORI */
   N_("Objects are spaced %1d pixels apart horizontally."),
   /* STID_TOP_SIDES_APART */
   N_("Top sides of objects are %1d pixels apart."),
   /* STID_MIDDLES_APART */
   N_("Middles of objects are %1d pixels apart."),
   /* STID_BOTTOM_SIDES_APART */
   N_("Bottom sides of objects are %1d pixels apart."),
   /* STID_SPACED_APART_VERT */
   N_("Objects are spaced %1d pixels apart vertically."),
   /* STID_DISTR_VERTEX_HORI */
   N_("Distribute selected vertices so that they are spaced equally horizontally"),
   /* STID_DISTR_VERTEX_VERT */
   N_("Distribute selected vertices so that they are spaced equally vertically"),
   /* STID_DISTR_VERTEX_VERT_AND_HORI */
   N_("Distribute selected vertices so that they are spaced equally horizontally and vertically"),
   /* STID_MOVE_ALL_VERTICES_YNC */
   N_("Okay to move all vertices to one point? [ync](y)"),
   /* STID_CANNOT_ALIGN_OBJS_TOO_MANY */
   N_("Cannot align objects.  Too many objects locked."),
   /* STID_ALIGN_VERTICES_HORI_TO_GRID */
   N_("Align selected vertices horizontally to the grid"),
   /* STID_ALIGN_VERTICES_VERT_TO_GRID */
   N_("Align selected vertices vertically to the grid"),
   /* STID_ALIGN_VERTICES_HORI */
   N_("Align selected vertices horizontally"),
   /* STID_ALIGN_VERTICES_VERT */
   N_("Align selected vertices vertically"),
   /* STID_ALIGN_VERTICES_TO_GRID */
   N_("Align selected vertices to the grid"),
   /* STID_ALIGN_VERTICES */
   N_("Align selected vertices"),
   /* STID_SELECT_ONE_POLY_ONE_NON_POLY */
   N_("Please select one polyline and one non-polyline object."),
   /* STID_CANNOT_MOVE_A_VERTEX_LOCKED */
   N_("Cannot move a vertex for a locked polyline object."),
   /* STID_SELECT_ONLY_ONE_POLY */
   N_("Please select only one POLY object."),
   /* STID_NO_ARC_CREATED */
   N_("No arc created."),
   /* STID_INVALID_ARC_VERSION */
   N_("Invalid arc version (%1d)."),
   /* STID_WARN_INCONSIST_ARC_DIR */
   N_("Warning: Inconsistent arc direction.  Corrected."),
   /* STID_ARC_SPEC */
   N_("Please enter an arc specification: [radius,dir(+/-),theta1,theta2]"),
   /* STID_INVALID_ARC_SPEC_RADIUS */
   N_("Invalid radius in arc specification '%s'."),
   /* STID_INVALID_ARC_SPEC_DIR */
   N_("Invalid direction in arc specification '%s'."),
   /* STID_INVALID_ARC_SPEC_THETA1 */
   N_("Invalid theta1 in arc specification '%s'."),
   /* STID_SELECT_AT_LEAST_ONE_TEXT */
   N_("Must select at least one text object."),
   /* STID_MORE_THAN_ONE_NON_TEXT */
   N_("More than one non-text objects selected."),
   /* STID_NO_TEXT_SELECTED_ADD_ATTRS */
   N_("No text objects selected to add as attributes."),
   /* STID_NO_NON_TEXT_SELECTED */
   N_("No objects (other than TEXT objects) selected."),
   /* STID_TEXT_ATTACHED */
   N_("Text object(s) attached."),
   /* STID_TEXT_UNLOCKED_AND_ATTACHED */
   N_("Text object(s) unlocked and attached."),
   /* STID_UNEXPECTED_EOF_IN_ABORT_READ */
   N_("%s, %d: Unexpected EOF in %s.  Read aborted."),
   /* STID_STRING_LEN_EXCEEDS_AND_TRUNC */
   N_("String length exceeds %1d.  String is truncated."),
   /* STID_MOVE_ONE_ATTR_MOUSE_MSG */
   N_("LEFT--show and move.  MIDDLE--toggle name shown.  RIGHT--hide attr."),
   /* STID_CANNOT_MOVE_ATTR_IF_LOCKED */
   N_("Cannot move attributes of locked object."),
   /* STID_SHOW_MOVE_ATTR */
   N_("Show/move an attribute"),
   /* STID_TOGGLE_NAME_SHOWN */
   N_("Toggle name shown"),
   /* STID_HIDE_ATTR */
   N_("Hide an attribute"),
   /* STID_CHANGE_ATTR_JUST_MOUSE_MSG */
   N_("LEFT--left justify.  MIDDLE--center justify.  RIGHT--right justify."),
   /* STID_LEFT_JUSTIFY */
   N_("Left justify an attribute"),
   /* STID_CENTER_JUSTIFY */
   N_("Center justify an attribute"),
   /* STID_RIGHT_JUSTIFY */
   N_("Right justify an attribute"),
   /* STID_SELECT_ONLY_ONE_OBJ */
   N_("Please select only ONE object."),
   /* STID_SELECT_HAS_NO_ATTR */
   N_("Selected object currently has NO attributes."),
   /* STID_DRAG_LEFT_BTN_TO_SEE_ATTR */
   N_("Drag left button to see attributes."),
   /* STID_CHANGE_ATTR_JUST */
   N_("Change an attribute justification"),
   /* STID_IS_ATTR_OBSOLETE_FOR_ICON_YNC */
   N_("Is the attribute '%s' obsolete for icon '%s'?  [yn](y)"),
   /* STID_BAD_FORMAT_IN_SHORTCUT_SPEC */
   N_("Invalid format in shortcut specification."),
   /* STID_CANNOT_FIND_NAMED_ATTR */
   N_("Cannot find a '%s' attribute for the selected object."),
   /* STID_ATTACH_FILE_ATTRS_IN_WB */
   N_("AttachFileAttrs() is disabled in WhiteBoard mode."),
   /* STID_CANNOT_ATTACH_NON_TEXT_OBJ */
   N_("Cannot attach non-text objects."),
   /* STID_DETACH_FILE_ATTRS_IN_WB */
   N_("DetachFileAttrs() is disabled in WhiteBoard mode."),
   /* STID_FILE_CONTAINS_NO_ATTRS */
   N_("Current file contains no attributes."),
   /* STID_SHOW_ATTR_NAME_AND_VALUE */
   N_("Will show both the name and the value for the '%s' attribute."),
   /* STID_SHOW_ATTR_VALUE_ONLY */
   N_("Will only show the value of the '%s' attribute."),
   /* STID_NOT_SHOW_ATTR */
   N_("Will not show the '%s' attribute."),
   /* STID_SHOW_ATTR */
   N_("Will show the '%s' attribute."),
   /* STID_WONT_SHOW_VALUE */
   N_("Will not show the '%s' value."),
   /* STID_TGIF_EDIT_ATTR_NAMES */
   N_("%s - Edit Attribute Names"),
   /* STID_EDIT_FILE_ATTRS_IN_WB */
   N_("EditFileAttrs() is disabled in WhiteBoard mode."),
   /* STID_EDIT_FILE_ATTRS_DOTS */
   N_("Edit File Attributes..."),
   /* STID_FILE_ATTRS_UPDATED */
   N_("File attribute(s) updated."),
   /* STID_SELECT_ONE_OBJ_FOR_EDITATTRS */
   N_("Must select only one object for EditAttrs()."),
   /* STID_OBJ_CONTAINS_NO_ATTRS */
   N_("Selected object contains no attributes."),
   /* STID_EDIT_OBJ_ATTRS_DOTS */
   N_("Edit Object Attributes (show/hide, name shown)..."),
   /* STID_OBJ_ATTRS_UPDATED */
   N_("Object attribute(s) updated."),
   /* STID_INPUT_OCTAL_STR */
   N_("Please input an octal string (e.g., \"%s\"):"),
   /* STID_INPUT_STR */
   N_("Please input a string:"),
   /* STID_WARN_PRINT_COURIER_IN_COLOR */
   N_("Warning: Printing Courier fonts with non-solid pen in color."),
   /* STID_MAY_CAUSE_ERROR_IN_PRINTOUT */
   N_("    This may cause error in the printout."),
   /* STID_BAD_OCTAL_STRING_ENCOUNTERED */
   N_("Bad octal string \\%c%c%c encountered."),
   /* STID_SERVER_NOT_STARTED */
   N_("Server not started."),
   /* STID_INVISIBLE_COLOR_SELECTED_YNC */
   N_("Invisible color #%1d (%s) is selected for drawing.\n\nWould you like to make the color visible?"),
   /* STID_COLOR_IS_CURRENTLY_INVISIBLE */
   N_("Color %1d (%s) is currently invisible."),
   /* STID_COLOR_SET_TO */
   N_("Color set to '%s'."),
   /* STID_TEXT_VSPACE_TOO_SMALL_RESET_0 */
   N_("Text vertical spacing too small.\n\nIt is reset to 0."),
   /* STID_FONT_AND_SIZE_NOT_AVAILABLE */
   N_("%s-%1d not available."),
   /* STID_LINE_WIDTH_SET_TO */
   N_("Line width set to %s."),
   /* STID_LINE_TYPE_IS_STRAIGHT */
   N_("Line type is 'straight'."),
   /* STID_LINE_TYPE_IS_SPLINE */
   N_("Line type is 'spline'."),
   /* STID_LINE_TYPE_IS_INTSPLINE */
   N_("Line type is 'interpolated spline'."),
   /* STID_TAB_FOR_FILE_COMPLETION */
   N_("Type <TAB> for file completion..."),
   /* STID_FAIL_TO_CREATE_WINDOW */
   N_("Fail to create a window."),
   /* STID_FAIL_TO_CREATE_NAMED_WINDOW */
   N_("Fail to create the %s window."),
   /* STID_UNDO_REDO_MAY_CAUSE_CRASH */
   N_("Further Undo()/Redo() may cause crashes."),
   /* STID_UNDO_IN_WB */
   N_("Undo() is disabled in WhiteBoard mode."),
   /* STID_NO_COMMANDS_TO_UNDO */
   N_("No commands to Undo."),
   /* STID_REDO_IN_WB */
   N_("Redo() is disabled in WhiteBoard mode."),
   /* STID_NO_COMMANDS_TO_REDO */
   N_("No commands to Redo."),
   /* STID_SELECT_LIST_NOT_SORTED_IN */
   N_("Select list not sorted in %s."),
   /* STID_WARN_CANNOT_PARSE_NAMED_COLOR */
   N_("Warning: Cannot parse color '%s'."),
   NULL
};

static char *gaszEnglishStrTable100[] = {
   /* STID_CANNOT_ALLOC_NAMED_COLOR */
   N_("Warning: Cannot allocate color '%s'."),
   /* STID_CANNOT_BREAKUP_EPS */
   N_("Cannot break up an EPS object."),
   /* STID_INVALID_FILE_IN_X_DEFAULT */
   N_("Invalid file '%s' specified by X default %s.%s."),
   /* STID_INVALID_XDEF_COLORXPM_GET */
   N_("Invalid %s.%s: '%s' - Fail to get colors.  Resource ignored."),
   /* STID_INVALID_XDEF_COLORXPM_TRANS */
   N_("Invalid %s.%s: '%s' - Transparent color is not allowed.  Resource ignored."),
   /* STID_FAIL_ALLOC_EDIT_TEXT_BGCOLOR */
   N_("Fail to allocate the '%s' background color for use in editing existing text objects."),
   /* STID_FAIL_ALLOC_EDIT_TEXT_HICOLOR */
   N_("Fail to allocate the '%s' highlight color for use in editing existing text objects"),
   /* STID_FAIL_ALLOC_DEF_BGCOLOR */
   N_("Fail to allocate the default background color: '%s'."),
   /* STID_INVALID_XDEF_USE_ALT_VALUE */
   N_("Invalid %s.%s: '%s', %1d is used instead."),
   /* STID_NORM_VIDEO_MODE_ASSUMED */
   N_("Normal video mode assumed since %s is specified in the command line."),
   /* STID_ZERO_COLORS_SPECIFIED */
   N_("0 colors specified (must be at least 1)."),
   /* STID_CANNOT_GET_X_DEFAULT */
   N_("Cannot get X default %s.%s."),
   /* STID_INVALID_XDEF_WILL_TRY_STR */
   N_("Invalid %s.%s: '%s'.  Will try '%s'."),
   /* STID_FAIL_TO_ALLOC_3D_COLORS */
   N_("Fail to allocate colors for 3DLook."),
   /* STID_FAIL_TO_ALLOC_COLOR_NUM */
   N_("Fail to allocate color number %1d: '%s'.  %s is aborted."),
   /* STID_DEFCOLORINDEX_GE_MAXCOLORS */
   N_("Warning: %s.%s >= %s.%s.  Use 0 for %s.%s."),
   /* STID_FAIL_ALLOC_NAMED_COLOR_ABORT */
   N_("Fail to allocate the '%s' color.  Abort."),
   /* STID_FAIL_ALLOC_COLOR_NUM_ABORT */
   N_("Fail to allocate color number %1d: '%s'.  Abort."),
   /* STID_UNEXPECTED_MAX_RGB_ZERO */
   N_("Warning: Unexpected maximum RGB intensity of 0."),
   /* STID_FG_BG_SAME_COLOR_WARNING */
   N_("Warning: Foreground and background colors are the same.  You may not be able to see much."),
   /* STID_CANNOT_SET_FG_COLOR */
   N_("Warning: Cannot set the foreground color to '%s'.  The default color '%s' is used."),
   /* STID_INVISIBLE_COLOR_SELECTED */
   N_("Invisible color #%1d (%s) is selected for drawing."),
   /* STID_ALL_COLOR_LAYERS_VISIBLE */
   N_("All color layers are now visible."),
   /* STID_ALL_COLOR_LAYERS_INVISIBLE */
   N_("All color layers are now invisible."),
   /* STID_A_COLOR_LAYER_TURNED_ON */
   N_("Color '%s' (layer %1d) is turned on."),
   /* STID_A_COLOR_LAYER_TURNED_OFF */
   N_("Color '%s' (layer %1d) is turned off."),
   /* STID_TURN_COLOR_LAYER_NUM_ON */
   N_("Turn color '%s' (layer %1d) on"),
   /* STID_TURN_COLOR_LAYER_NUM_OFF */
   N_("Turn color '%s' (layer %1d) off"),
   /* STID_COLORMAP_NOT_RESETED */
   N_("Colormap not reseted due to the DONT_FREE_COLORMAP compile flag."),
   /* STID_ADDCOLOR_FOR_BW_DPY */
   N_("Adding color is not available for a black and white display."),
   /* STID_ENTER_COLORS_TO_ADD */
   N_("Please enter colors to add:"),
   /* STID_FAIL_ALLOC_NAMED_COLOR */
   N_("Fail to allocate the '%s' color."),
   /* STID_ENTER_EDIT_BG_COLOR */
   N_("Please enter the background color to be used in editing existing a text object when UseAltEditTextBgColor is selected (please enter 'NONE' to remove): [current - %s]"),
   /* STID_ENTER_EDIT_HILIGHT_COLOR */
   N_("Please enter the highlight color to be used in editing existing a text object when UseAltEditTextBgColor is selected (please enter 'NONE' to remove): [current - %s]"),
   /* STID_COLOR_LAYERS_ENABLED */
   N_("Color layers enabled."),
   /* STID_COLOR_LAYERS_DISABLED */
   N_("Color layers disabled."),
   /* STID_TOO_MANY_LINES_IN_COLORINFO */
   N_("Warning: Too many lines in color_info."),
   /* STID_WAIT_FOR_START_CONV */
   N_("Waiting for start of input conversion."),
   /* STID_NO_NAMED_SELECTION_OWNER */
   N_("There is no Selection Owner of %s."),
   /* STID_FAIL_TO_CHANGE_CONV_PROP */
   N_("Fail to change conversion property."),
   /* STID_FAIL_TO_INTERN_NAMED_ATOM */
   N_("Fail to intern the '%s' atom."),
   /* STID_FAIL_TO_CONNECT_TO_OWNER */
   N_("Fail to connect to selection owner."),
   /* STID_FAIL_TO_GET_WIN_PROP */
   N_("Cannot get window property."),
   /* STID_FAIL_TO_DISCONNECT_FROM_OWNER */
   N_("Fail to disconnect from selection owner."),
   /* STID_LOCALE_IS */
   N_("Locale is '%s'."),
   /* STID_MODIFIER_IS */
   N_("Modifier is '%s'."),
   /* STID_CANNOT_OPEN_XIM */
   N_("Cannot open the X Input Method."),
   /* STID_OVERTHESPOT_CONV */
   N_("OverTheSpot conversion."),
   /* STID_ROOT_CONV */
   N_("Root conversion."),
   /* STID_IM_NOT_SUPPORT_GIVEN_STYLE */
   N_("This X Input Method does not support the '%s' input style."),
   /* STID_FAIL_TO_CREATE_INPUTCONTEXT */
   N_("Fail to create InputContext."),
   /* STID_OPEN_XIM */
   N_("Open X Input Method."),
   /* STID_CLOSE_XIM */
   N_("Close X Input Method."),
   /* STID_INVALID_XDEF_USE_ALT_STR */
   N_("Invalid %s.%s: '%s', '%s' is used instead."),
   /* STID_NO_OBJ_SELECTED_FOR_COPY */
   N_("No object selected for the copy/cut operation."),
   /* STID_CANNOT_OPEN_FILE_FOR_WRITING */
   N_("Cannot open '%s' for writing."),
   /* STID_CANNOT_OPEN_FILE_FOR_READING */
   N_("Cannot open '%s' for reading."),
   /* STID_FAIL_TO_FSTAT_ABORT_COPY */
   N_("Fail to fstat() '%s'.  Copy aborted."),
   /* STID_ERR_READING_FILE_COPY_ABORT */
   N_("Error reading from '%s'.  Copy aborted."),
   /* STID_COPY_FAILED_OBJ_MAYBE_TOO_BIG */
   N_("Copy to cut buffer failed.  Selected object(s) may be too big."),
   /* STID_COPY_BUFFER_UPDATED */
   N_("Copy buffer updated."),
   /* STID_ESC_KEY_PRESS_IGNORED */
   N_("An <ESC> key press is ignored."),
   /* STID_CUT_BUFFER_EMPTY */
   N_("The cut buffer is empty or there was a problem getting data from the cut buffer."),
   /* STID_STR_EXCEEDS_NUM_CH_IGNORED */
   N_("String length exceeds %1d.  Character is ignored."),
   /* STID_PASTE_FROM_NON_TGIF */
   N_("Paste from a non-tgif tool."),
   /* STID_ERR_WRITING_FILE_PASTE_ABORT */
   N_("Error writing to '%s'.  Paste aborted."),
   /* STID_FILEVER_TOO_LARGE_PASTE_ABORT */
   N_("File version too large (=%1d).  Paste aborted."),
   /* STID_CANNOT_PASTE_MIXED_TEXT */
   N_("Cannot paste mixed text at the current cursor position."),
   /* STID_STR_BLOCKS_PASTED_FROM_TGIF */
   N_("String blocks pasted from tgif."),
   /* STID_OBJS_PASTED_FROM_TGIF */
   N_("Objects pasted from tgif."),
   /* STID_SEL_A_FILE_TO_PASTE_DOTS */
   N_("Please select a file to PASTE..."),
   /* STID_PASTING_REMOTE_FILE_NOT_SUP */
   N_("Pasting remote file not supported."),
   /* STID_NAMED_FILE_IS_EMPTY */
   N_("File '%s' is empty."),
   /* STID_CANNOT_MALLOC_NUM_BYTES */
   N_("Cannot malloc %1d bytes."),
   /* STID_NO_PRIMARY_SELECTION_OWNER */
   N_("There is no selection owner of PRIMARY."),
   /* STID_CONVERTING_GIVEN_DOTS */
   N_("Converting %s..."),
   /* STID_TIMEOUT_CONVERT_GIVEN */
   N_("Timeout occurred when attempting to convert %s."),
   /* STID_PROBLEM_CONVERT_GIVEN */
   N_("Problem encountered when converting %s."),
   /* STID_PASTE_COMP_TEXT_FROM_NON_TGIF */
   N_("Paste compound-text from a non-tgif tool."),
   /* STID_CANNOT_FIND_SIZE_USE_ALT */
   N_("Cannot find font size '%1d'.  %1d is used instead."),
   /* STID_CANNOT_FIND_FONT_USE_ALT */
   N_("Cannot find screen font for '%s'.  '%s' font is used instead."),
   /* STID_CANNOT_PASTE_COMPOUND_OBJ */
   N_("Cannot paste '%s' for compound objects.  Therefore, '%s' cannot be selected."),
   /* STID_CUT_BUF_HAS_NON_ROT_COMP */
   N_("The '%s' in the cut buffer contains non-rotational components.  Text rotation of 0 degree is used."),
   /* STID_NO_PROPERTY_TO_PASTE */
   N_("No property to paste."),
   /* STID_NO_PROPERTY_TO_COPY */
   N_("No property to copy."),
   /* STID_INVALID_KEY_IN_SEC_OF_INIFILE */
   N_("Invalid key: '%s' in the [%s] section of '%s'."),
   /* STID_ENTER_NAME_FOR_PROP_SET */
   N_("Please enter a name for the property set."),
   /* STID_PROP_SET_EXISTS_OVERWRITE_YNC */
   N_("Property set [%s] already exists.  Okay to overwrite?"),
   /* STID_NO_PROP_TO_SAVE */
   N_("No property to save."),
   /* STID_WILL_COPY_NAMED_PROP */
   N_("Will copy the '%s' property."),
   /* STID_WILL_NOT_COPY_NAMED_PROP */
   N_("Will not copy the '%s' property."),
   /* STID_WILL_SAVE_NAMED_PROP */
   N_("Will save the '%s' property."),
   /* STID_WILL_NOT_SAVE_NAMED_PROP */
   N_("Will not save the '%s' property."),
   /* STID_WILL_PASTE_NAMED_PROP */
   N_("Will paste the '%s' property."),
   /* STID_WILL_NOT_PASTE_NAMED_PROP */
   N_("Will not paste the '%s' property."),
   /* STID_WILL_RESTORE_NAMED_PROP */
   N_("Will restore the '%s' property."),
   /* STID_WILL_NOT_RESTORE_NAMED_PROP */
   N_("Will not restore the '%s' property."),
   /* STID_TOOL_COPY_PROP */
   N_("%s - Copy Properties"),
   /* STID_TOOL_SAVE_PROP */
   N_("%s - Save Properties"),
   /* STID_TOOL_PASTE_PROP */
   N_("%s - Paste Properties"),
   NULL
};

static char *gaszEnglishStrTable200[] = {
   /* STID_TOOL_RESTORE_PROP */
   N_("%s - Restore Properties"),
   /* STID_SEL_ONLY_ONE_FOR_COPY_PROP */
   N_("Please select only one object for CopyProperties()."),
   /* STID_SEL_ONLY_ONE_FOR_SAVE_PROP */
   N_("Please select only one object for SaveProperties()."),
   /* STID_UNCHECK_PROP_FOR_COPY_DOTS */
   N_("Please uncheck properties you don't want to copy..."),
   /* STID_UNCHECK_PROP_FOR_SAVE_DOTS */
   N_("Please uncheck properties you don't want to save..."),
   /* STID_UNCHECK_PROP_FOR_PASTE_DOTS */
   N_("Please uncheck properties you don't want to paste..."),
   /* STID_UNCHECK_PROP_FOR_RESTORE_DOTS */
   N_("Please uncheck properties you don't want to restore..."),
   /* STID_MALFORMED_CUT_BUF_WHILE_PASTE */
   N_("Malformed cut buffer read while pasting properties."),
   /* STID_NO_PROP_SETS_TO_RESTORE */
   N_("Cannot find any property sets.  Nothing to restore."),
   /* STID_SEL_A_PROP_SET_TO_RESTORE */
   N_("Please select a property set to restore..."),
   /* STID_TOOL_EDIT_INI_SECTION */
   N_("%s - Edit Ini Section"),
   /* STID_BAD_VALUE_FOR_KEY */
   N_("Bad %s value: '%s'."),
   /* STID_NOTHING_TO_EDIT_SEC_EMPTY */
   N_("The [%s] section of '%s' is empty.\n\nNothing to edit."),
   /* STID_EDIT_VALUES_FOR_DOTS */
   N_("Edit values for [%s]..."),
   /* STID_TOOL_SEL_FROM_INI_SECTION */
   N_("%s - Select From Ini Section"),
   /* STID_NOTHING_TO_SEL_SEC_EMPTY */
   N_("The [%s] section of '%s' is empty.\n\nNothing to select from."),
   /* STID_SELECT_A_VALUE_FROM_SEC_DOTS */
   N_("Select a value from [%s]..."),
   /* STID_COPY_PROP_BEFORE_PASTE */
   N_("Please copy some properties before pasting properties."),
   /* STID_INVALID_PARAM_PASSED_TO_FUNC */
   N_("Invalid parameters passed to %s."),
   /* STID_TOOL_INPUT */
   N_("%s - Input"),
   /* STID_WARN_INVALID_NUMREDRAWBBOX */
   N_("Warning: Invalid numRedrawBBox."),
   /* STID_USER_INTR_ABORT_REPAINT */
   N_("User interrupt.  Repaint aborted."),
   /* STID_CANNOT_ALLOC_XPM_NO_ANIM */
   N_("Cannot allocate pixmap of size [%1dx%1d].\n\nCannot cache animation."),
   /* STID_SCROLLING_CANCELED */
   N_("Scrolling canceled."),
   /* STID_CANNOT_ALLOC_PIXMAP_OF_SIZE */
   N_("Cannot allocate pixmap of size [%1dx%1d]."),
   /* STID_FORCING_END_ANIMATE */
   N_("Forcing an end_animate()."),
   /* STID_TOOL_NOT_FINISH_WITH_EXEC */
   N_("%s is not finished with the previous ExecCmdsFromFile() yet."),
   /* STID_NO_INTSPLINE_SELECTED */
   N_("No interpolated spline objects selected."),
   /* STID_INTSPLINE_CONVERTED_TO_SPLINE */
   N_("Interpolated spline objects converted to splines."),
   /* STID_SELECT_ONLY_ONE_POLY_POLYGON */
   N_("Please select only one POLY or POLYGON object."),
   /* STID_CANNOT_TOGGLE_FOR_INTSPLINE */
   N_("Cannot toggle Smooth/Hinge points for interpolated spline objects."),
   /* STID_BAD_POLY_IN_TOGGLE_SMOOTH */
   N_("Bad poly in SelectModeToggleSmoothHinge()."),
   /* STID_CLICK_LEFT_BUTTON_TO_TOGGLE */
   N_("Click left mouse button to toggle Smooth/Hinge points."),
   /* STID_CLICK_OTHER_BUTTON_TO_QUIT */
   N_("Click other buttons to quit."),
   /* STID_ONLY_TOGGLE_SMOOTH_IN_MODES */
   N_("Can only toggle smooth/hinge points in VERTEX or SELECT mode."),
   /* STID_SMOOTHNESS_TOGGLED */
   N_("The smoothness of selected vertices are toggled."),
   /* STID_BREAK_TEXT_CWL */
   N_("Would you like to break text at char/word/line boundaries? [cwl](c)"),
   /* STID_BREAK_TEXT_CL */
   N_("Would you like to break text at char/line boundaries? [cl](c)"),
   /* STID_BREAK_TEXT_CW */
   N_("Would you like to break text at char/word boundaries? [cw](c)"),
   /* STID_INVALID_SPEC_ON_BREAK_TEXT */
   N_("Invalid specification on how to break up text: '%s'."),
   /* STID_NO_TEXT_OBJ_TO_BREAK_UP */
   N_("No text objects selected to break up."),
   /* STID_SOME_TEXT_NOT_BROKEN_UP_SIZE */
   N_("Some text are not broken up due to unavailable text size."),
   /* STID_TEXT_BROKEN_INTO_CHARS */
   N_("Text string is broken up into characters."),
   /* STID_CUR_TEXT_BG_SET_TO_NAMED */
   N_("Current text object's background color has been set to '%s'."),
   /* STID_SOME_TEXT_BG_SET_TO_NAMED */
   N_("Some text object's background color have been set to '%s'."),
   /* STID_SEL_ONE_POLYGON_TO_MAKE_REG */
   N_("Please select one polygon object to make it regular."),
   /* STID_POLYGON_LOCKED */
   N_("Selected polygon is locked."),
   /* STID_POLYGON_TOO_SMALL_FOR_REGULAR */
   N_("Selected polygon is too small to make regular."),
   /* STID_VERTEX_AT_3_OCLOCK_YNC */
   N_("Do you want a vertex at the 3 o'clock position?  [ync](y)"),
   /* STID_CANNOT_DEL_PT_FOR_LOCKED */
   N_("Cannot delete points for a locked object."),
   /* STID_LEFT_BTN_TO_DEL_PTS */
   N_("Click left mouse button to DELETE points."),
   /* STID_CANNOT_ADD_PT_FOR_LOCKED */
   N_("Cannot add points for a locked object."),
   /* STID_CANNOT_ADD_PT_FOR_AUTO_ARROW */
   N_("Cannot add points for an auto_retracted_arrows object."),
   /* STID_LEFT_BTN_TO_ADD_PTS */
   N_("Drag left mouse button to ADD points."),
   /* STID_UNDO_BUF_AND_CMAP_FLUSHED */
   N_("Undo buffer and Colormap flushed."),
   /* STID_NUM_COLORS_ALLOCATED */
   N_("%1d color(s) allocated."),
   /* STID_UNDO_BUF_FLUSHED */
   N_("Undo buffer flushed."),
   /* STID_SEL_ONE_XBM_OR_XPM_TO_RESTORE */
   N_("Please select one X11 Bitmap or X11 Pixmap object to restore."),
   /* STID_CANNOT_RESTORE_LOCKED */
   N_("Cannot restore a locked object."),
   /* STID_SEL_ONE_XBM_OR_XPM_TO_CUT */
   N_("Please select one X11 Bitmap or X11 Pixmap object to cut."),
   /* STID_CANNOT_CUT_LOCKED */
   N_("Cannot cut a locked object."),
   /* STID_CANNOT_CUT_TRANSFORMED_X_OBJ */
   N_("Cannot cut a stretched/rotated/sheared X11 Bitmap or X11 Pixmap object."),
   /* STID_SEL_ONE_XBM_OR_XPM_TO_BREAKUP */
   N_("Please select one X11 Bitmap or X11 Pixmap object to break up."),
   /* STID_CANNOT_BREAK_XFORMED_X_OBJ */
   N_("Cannot break up a stretched/rotated/sheared X11 Bitmap or X11 Pixmap object."),
   /* STID_CANNOT_BREAKUP_LOCKED */
   N_("Cannot break up a locked object."),
   /* STID_ENTER_NUM_ROWCOL_TO_BREAK */
   N_("Please enter the number of columns and rows to break into (original size is %1dx%1d): [Col x Row]"),
   /* STID_INVALID_ROWCOL_SPEC_REENTER */
   N_("Invalid specification: '%s'.\n\nPlease enter [Cols x Rows] or [=WxH]"),
   /* STID_ENTER_CONCAVE_OR_CONVEX */
   N_("Please specify concave (c) or convex (v):"),
   /* STID_INVALID_SPEC */
   N_("Invalid specification: '%s'."),
   /* STID_SEL_AN_ARC_OBJ */
   N_("Please select an arc object."),
   /* STID_NO_ARC_OBJ_SELECTED */
   N_("No arc object selected."),
   /* STID_TOO_MANY_ARC_SEL_ONLY_ONE_ARC */
   N_("Too many arc objects selected.\n\nPlease select only one arc object."),
   /* STID_NO_OTHER_OBJ_FOR_LAYOUTONARC */
   N_("Nothing to layout on the selected arc.\n\nPlease select other objects in addition to the selected arc."),
   /* STID_CANNOT_LAYOUTONARC_LOCKED */
   N_("Some objects are locked.\n\nPlease unlock them before retrying."),
   /* STID_ARC_XFORMED_FOR_LAYOUTONARC */
   N_("Don't know how to layout objects on an arc object that has been stretched/sheared/rotated.\n\nPlease use a simple arc."),
   /* STID_ROT_NOT_AVAIL_ON_VERTEX_MODE */
   N_("PreciseRotate() is not available in vertex mode."),
   /* STID_ENTER_AN_ANGLE_IN_DEGREES */
   N_("Please specify an angle in degrees: (positive angle is clock-wise)"),
   /* STID_INVALID_SPEC_NUM_EXPECTED */
   N_("Invalid specification '%s'.\n\nA numerical value is expected."),
   /* STID_SOME_GROUP_OBJ_NOT_MODIFIED */
   N_("Some group/symbol/icon objects are not modified."),
   /* STID_CANNOT_REM_XFORM_FOR_GROUPED */
   N_("Cannot remove transforms for group/symbol/icon objects."),
   /* STID_ENTER_EDIT_TEXT_SIZE */
   N_("Please specify text size for editing text objects (enter 0 or a value between 4 and 34, 0 means to use the actual size of text): [current: %1d]"),
   /* STID_EDIT_TEXT_SIZE_OUT_OF_RANGE */
   N_("The value entered: '%s' is out of range.\n\nPlease enter a value between 4 and 34."),
   /* STID_ACTUAL_EDIT_TEXT_SIZE */
   N_("Actual text size will be used to edit existing text."),
   /* STID_USE_SPECIFIED_EDIT_TEXT_SIZE */
   N_("Text size of %1d will be used to edit existing text."),
   /* STID_CANNOT_FIND_NAMED_STRING */
   N_("Cannot find '%s'."),
   /* STID_FIND_CMD_WRAPPED */
   N_("Find command is wrapped around."),
   /* STID_ENTER_CASE_STR_TO_FIND */
   N_("Please enter a string to find (case sensitive):"),
   /* STID_ENTER_NOCASE_STR_TO_FIND */
   N_("Please enter a string to find (case insensitive):"),
   /* STID_NO_PREVIOUS_FIND */
   N_("No previous find specified.\n\nPlease select FindCaseSensitive() or FindNoCase()."),
   /* STID_CANNOT_ABUT_IN_VERTEX_MODE */
   N_("Cannot abut in vertex mode."),
   /* STID_CANNOT_ABUT_LOCKED */
   N_("Cannot abut.  Too many objects locked."),
   /* STID_ABUTTED_HORI */
   N_("Objects are abutted horizontally."),
   /* STID_ABUTTED_VERT */
   N_("Objects are abutted vertically."),
   /* STID_ICONS_BROUGHT_UP_TO_DATE */
   N_("Selected icons are brought up to date."),
   /* STID_SEL_AT_LEAST_TWO_OBJS */
   N_("Please select at least two objects."),
   /* STID_CANNOT_SIZE_OF_GIVEN_WIDTH */
   N_("Cannot size objects to width of %1d."),
   /* STID_CANNOT_SIZE_OF_GIVEN_HEIGHT */
   N_("Cannot size objects to height of %1d."),
   /* STID_SPECIFY_WIDTH_HEIGHT */
   N_("Please specify a width and a height [WxH]:"),
   /* STID_SPECIFY_WIDTH */
   N_("Please specify a width:"),
   /* STID_SPECIFY_HEIGHT */
   N_("Please specify a height:"),
   NULL
};

static char *gaszEnglishStrTable300[] = {
   /* STID_NAMED_XDEF_IS_OBSOLETE */
   N_("The %s.%s X default is obsolete."),
   /* STID_GIVEN_BPS_PREVIEW_NOT_SUP */
   N_("%1d bits per sample preview bitmap is not supported."),
   /* STID_INVALID_PREVIEW_BMP_IN_EPS */
   N_("Invalid preview bitmap in EPS file: '%s'."),
   /* STID_FAIL_TO_WRITE_TO_STDOUT */
   N_("Fail to write to stdout.\n\nFile system may be full."),
   /* STID_FAIL_TO_PARSE_WINEPS_FILE */
   N_("Fail to parse Windows EPS file '%s'."),
   /* STID_INVALID_PREVIEW_BOX_IN_EPS */
   N_("Invalid preview box in EPS file: '%s'."),
   /* STID_CANNOT_BBOX_IN_GIVEN_FILE */
   N_("Cannot find bounding box information in '%s'."),
   /* STID_CANNOT_CONVERT_WINEPS_PREVIEW */
   N_("Unable to convert Windows EPS preview bitmap in '%s'."),
   /* STID_CANNOT_OPEN_EPS_FILE_FOR_READ */
   N_("Cannot open EPS file '%s' for reading."),
   /* STID_EPS_OBJ_SKIPPED_FOR_PRINT */
   N_("EPS object skipped during printing."),
   /* STID_EPS_FILE_NEWER_THAN_EPS_OBJ */
   N_("Warning: EPS file '%s' is newer than the EPS object."),
   /* STID_NO_TOP_LEVEL_EPS_SELECTED */
   N_("No top-level EPS object selected."),
   /* STID_CANNOT_IMPORT_GIVEN_EPS */
   N_("Cannot import EPS file '%s'."),
   /* STID_EPS_OBJECT_UPDATED */
   N_("EPS object(s) updated."),
   /* STID_CANNOT_EVAL_INVALID_OP_TYPE */
   N_("Cannot evaluate '%s'.\n\nInvalid operand type for the '%s' operator."),
   /* STID_DIVIDE_BY_ZERO_FOR_OP_TYPE */
   N_("Divide by zero encountered for the '%s' operator."),
   /* STID_ILLEGAL_EXPR_INVALID_OP_CODE */
   N_("Illegal expression: '%s'.\n\nInvalid operator code: '%1d'."),
   /* STID_ILLEGAL_EXPR_EMPTY_OP_STACK */
   N_("Illegal expression: '%s'.\n\nEmpty operator stack."),
   /* STID_ILLEGAL_EXPR_BAD_NUM_VALUE */
   N_("Illegal expression: '%s'.\n\nBad numeric value: '%s'."),
   /* STID_ILLEGAL_EXPR_BAD_STR_VALUE */
   N_("Illegal expression: '%s'.\n\nBad string value: '%s'."),
   /* STID_ILLEGAL_EXPR_BAD_IDENTIFIER */
   N_("Illegal expression: '%s'.\n\nBad identifier: '%s'."),
   /* STID_ILLEGAL_EXPR_BAD_OPERATOR */
   N_("Illegal expression: '%s'.\n\nBad operator: '%s'."),
   /* STID_ILLEGAL_EXPR_TOO_MANY_SYMBOL */
   N_("Illegal expression: '%s'.\n\nToo many '%c'."),
   /* STID_ILL_EXPR_OP_STACK_NON_EMPTY */
   N_("Illegal expression: '%s'.\n\nNon empty operator stack after evaluation."),
   /* STID_ILL_EXPR_VAL_STACK_NON_EMPTY */
   N_("Illegal expression: '%s'.\n\nNon empty value stack after evaluation."),
   /* STID_ILL_EXPR_TOO_MANY_VALS_LEFT */
   N_("Illegal expression: '%s'.\n\nToo many values left on the value stack after evaluation."),
   /* STID_CANNOT_OPEN_FOR_READ_PRINT */
   N_("Cannot open '%s' for reading.  Printing aborted."),
   /* STID_CANNOT_OPEN_FOR_WRITE_PRINT */
   N_("Cannot open '%s' for writing.  Printing aborted."),
   /* STID_CANNOT_FIND_CMD_EXEC */
   N_("Cannot find '%s'.\n\nWould you like to execute '%s' anyway?"),
   /* STID_CANNOT_FIND_CMD_IN_PATH_EXEC */
   N_("Cannot find '%s' in the PATH.\n\nWould you like to execute '%s' anyway?"),
   /* STID_CANT_FIND_CMD_MSG_EXEC */
   N_("Cannot find '%s'.  %s\n\nWould you like to execute '%s' anyway?"),
   /* STID_CANT_FIND_CMD_INPATH_MSG_EXEC */
   N_("Cannot find '%s' in the PATH.  %s\n\nWould you like to execute '%s' anyway?"),
   /* STID_FILE_EXISTS_OK_OVERWRITE_YNC */
   N_("File '%s' exists, okay to overwrite? [ync](y)"),
   /* STID_ONE_PAGE_SYM_ABORT_SAVE */
   N_("A symbol file can only have one page.\n\nSave aborted."),
   /* STID_ONE_PAGE_PIN_ABORT_SAVE */
   N_("A pin file can only have one page.\n\nSave aborted."),
   /* STID_TOO_MANY_SYM_ABORT_SAVE */
   N_("Too many symbols.\n\nSymbol file not saved."),
   /* STID_CANT_OPEN_WRITE_FILE_NOT_SAVE */
   N_("Cannot open '%s' for writing.  File not saved."),
   /* STID_SAVE_TMP_FILE_DOTS */
   N_("Saving temporary file '%s'..."),
   /* STID_TMP_FILE_SAVED */
   N_("Temporary file '%s' saved."),
   /* STID_CANNOT_CHMOD */
   N_("Cannot chmod '%s' to 0%03o."),
   /* STID_WORKING_DIRECTORY_IS */
   N_("( working directory: %s )"),
   /* STID_ENTER_NEW_FNAME_ACCEPT_CANCEL */
   N_("Please enter new file name: ( <CR>: accept, <ESC>: cancel )"),
   /* STID_CANT_SAVE_SYM_NO_SYM_FOUND */
   N_("Cannot save as a symbol file.\n\nNo symbol defined."),
   /* STID_CANT_SAVE_PIN_NO_SYM_FOUND */
   N_("Cannot save as a pin file.\n\nNo symbol defined."),
   /* STID_CANT_SAVE_OJB_ONE_SYM_FOUND */
   N_("Cannot save as an object file.\n\nOne symbol defined."),
   /* STID_CANT_SAVE_SYM_SAVE_AS_PIN */
   N_("Cannot save as a symbol file.\n\nPlease save the file as a pin file."),
   /* STID_CANT_SAVE_PIN_ONE_SYM_FOUND */
   N_("Cannot save as a pin file.\n\nOnly one symbol is defined."),
   /* STID_CANT_SAVE_OJB_TWO_SYM_FOUND */
   N_("Cannot save as an object file.\n\nTwo symbols defined."),
   /* STID_CANT_SAVE_SYM_TWO_SYM_FOUND */
   N_("Cannot save as a symbol file.\n\nTwo symbols defined."),
   /* STID_CANT_FIND_TYPE_ATTR_NOT_SAVED */
   N_("Cannot find an attribute name 'type'.\n\nPin file not saved."),
   /* STID_SYM_TYPE_WRONG_PIN_NOT_SAVED */
   N_("Symbol type is not '%s'.\n\nPin file not saved."),
   /* STID_NO_FILE_NAME_FILE_NOT_SAVED */
   N_("No file name specified.\n\nFile not saved."),
   /* STID_SAVING_DOTS */
   N_("Saving '%s'..."),
   /* STID_PROBLEM_ZIPPING_PLEASE_SAVE */
   N_("Problems encountered while (g)zipping '%s' to '%s'.\n\nThe unzipped file is '%s'.  Please save a copy of it before proceeding."),
   /* STID_FILE_SAVED */
   N_("File '%s' saved."),
   /* STID_FILE_NOT_SAVED */
   N_("File '%s' is not saved."),
   /* STID_NO_SYM_FOUND_SYM_NOT_SAVED */
   N_("No symbol found.\n\nSymbol file not saved."),
   /* STID_ENTER_NEW_FILE_NAME */
   N_("Please enter new file name:"),
   /* STID_CANT_FIND_DOT_IN_SAVEFILE */
   N_("Cannot find '.' in curFileName in %s."),
   /* STID_NO_SYM_FOUND_PIN_NOT_SAVED */
   N_("No symbol found.\n\nPin file not saved."),
   /* STID_SYM_TYPE_IS_SHOULD_SAVE_PIN */
   N_("The symbol is of type %s.\n\nIt should be saved as a pin file."),
   /* STID_TOO_MANY_SYM_FILE_NOT_SAVED */
   N_("Too many symbols.\n\nFile not saved."),
   /* STID_BAD_FIELD_IN_FUNC_ABORT_READ */
   N_("%s, %d: Invalid %s in %s.  Read aborted."),
   /* STID_UNRECOG_PAGE_STYLE */
   N_("Unrecognizable page style '%1d'."),
   /* STID_PORTRAIT_STYLE_ASSUMED */
   N_("Portrait style assumed."),
   /* STID_CANT_FIND_COLOR_NUM_USE_CUR */
   N_("In reading state, cannot find color #%1d, use '%s' as the current color."),
   /* STID_LINEWIDTH_IDX_RANGE_SET_TO_0 */
   N_("File's linewidth index '%1d' is out of range.  Set to 0."),
   /* STID_BAD_FIELD_IN_OBJ_ABORT_READ */
   N_("%s, %d: Invalid %s field in reading the %s object.  Read aborted."),
   /* STID_BAD_INPUT_WHILE_READ_PAGE */
   N_("Malformed input file (apparently reading page %1d).  Read aborted."),
   /* STID_BAD_PAGE_FILE_NAME_TRUNC_TO */
   N_("Invalid page file name: '%s'.  Truncated to '%s'."),
   /* STID_CUR_COLOR_CORRECTED_TO_BE */
   N_("(Current color is corrected to be '%s'.)"),
   /* STID_FAIL_ALLOC_BGCOLOR_USE_DEF */
   N_("Fail to allocate the background color: '%s'.  The current default background color '%s' is used instead."),
   /* STID_ONLY_PAGE_1_IMPORTED */
   N_("Only page 1 is imported from a multipage file."),
   /* STID_ONLY_GIVEN_PAGE_IMPORTED */
   N_("Only page %1d is imported from a multipage file."),
   /* STID_INVALID_REMOTE_FNAME */
   N_("Invalid remote file name '%s'."),
   /* STID_CANNOT_READ_TMP_FILE */
   N_("Cannot read temporary file '%s'."),
   /* STID_CANNOT_IMPORT_FILE */
   N_("Cannot import '%s'."),
   /* STID_INVALID_PAGE_NUM */
   N_("Invalid page number '%s' specified."),
   /* STID_USER_INTR_ABORT_DRAW */
   N_("User interrupt.  Drawing aborted."),
   /* STID_FILE_VER_ABORT_IMPORT */
   N_("File version (=%1d) too large.\n\nImport aborted.\n\nYou may need a more recent version of %s.  Please check <URL:%s>."),
   /* STID_FILE_CORRUPTED_ABORT_IMPORT */
   N_("File corrupted.\n\nImport aborted."),
   /* STID_FILE_IMPORTED */
   N_("'%s' imported."),
   /* STID_SEL_AN_OBJ_FILE_TO_IMPORT */
   N_("Please select an object file to import..."),
   /* STID_CANNOT_ALLOC_BGCOLOR_USE_DEF */
   N_("Cannot allocate bg_color '%s', default bgcolor used."),
   /* STID_CANNOT_ALLOC_FGCOLOR_USE_DEF */
   N_("Cannot allocate fg_color '%s', default fgcolor used."),
   /* STID_CANNOT_SET_BGXPM_TO */
   N_("Cannot set background pixmap to '%s'."),
   /* STID_COLORMAP_FLUSHED */
   N_("Colormap flushed."),
   /* STID_FILE_VER_ABORT_OPEN */
   N_("File version (=%1d) too large.\n\nOpen aborted.\n\nYou may need a more recent version of %s.  Please check <URL:%s>."),
   /* STID_FILE_CORRUPTED_ABORT_OPEN */
   N_("File corrupted.\n\nOpen aborted."),
   /* STID_POP_BACK_TO_NAMED */
   N_("Pop back to '%s'."),
   /* STID_WARN_PS_BBOX_EMPTY */
   N_("Warning: The PostScript bounding box is empty."),
   /* STID_TIFFEPSI_DIRECTLY_SUPPORTED */
   N_("Obsoleted %s.%s used.  Ignored.\n\tTiffEPSI export is now supported by %s directly as an export format."),
   /* STID_INVALID_XDEF */
   N_("Invalid %s.%s: '%s'."),
   /* STID_FAIL_EXEC_PROG_EPSI_NOT_GEN */
   N_("Fail to execute '%s'.\n\nEPSI file not generated."),
   /* STID_FAIL_GET_FINFO_EPSI_NOT_GEN */
   N_("Fail to get file info for '%s'.\n\nEPSI file not generated."),
   /* STID_CANNOT_OPEN_FILE_FOR_APPEND */
   N_("Cannot open '%s' for appending."),
   /* STID_CANNOT_PRINT_REMOTE_FILE */
   N_("Cannot print/export a remote file.\n\nPlease first save the file and then print/export again."),
   /* STID_NO_OBJ_TO_PRINT_ON_PAGE */
   N_("No objects to print on page %1d."),
   /* STID_NO_OBJ_TO_PRINT */
   N_("No objects to print."),
   /* STID_NO_CUR_FILE_CANNOT_GEN_FORMAT */
   N_("No current file.\n\nCannot generate %s output."),
   NULL
};

static char *gaszEnglishStrTable400[] = {
   /* STID_GENERATING_PRINT_FILE_DOTS */
   N_("Generating print file..."),
   /* STID_NO_OBJ_TO_EXPORT */
   N_("No objects to export."),
   /* STID_WRITING_TO_DOTS */
   N_("Writing to '%s'..."),
   /* STID_GENERATING_PREVIEW_BITMAP */
   N_("Generating preview bitmap..."),
   /* STID_OUTPUT_TRUNC_IN_PRINT_TILED */
   N_("Output may get truncated because %s.%s is set to 1."),
   /* STID_RECOMMEND_REDUCING_SETTING */
   N_("Reducing the above setting is recommended."),
   /* STID_PRINTING_WITH_GIVEN_CMD */
   N_("Printing with the '%s' command."),
   /* STID_CANNOT_EXEC_ABORT_PRINT */
   N_("Cannot execute '%s', print aborted."),
   /* STID_NAMED_FILE_PRINTED */
   N_("'%s' printed."),
   /* STID_PRINT_COMPLETED */
   N_("Print completed."),
   /* STID_PRINTING_INTO_NAMED_FILE */
   N_("Printing into '%s'..."),
   /* STID_GENERATING_TIFF_PREVIEW_BMP */
   N_("Generating TIFF preview bitmap..."),
   /* STID_FORMAT_FILE_NOT_GENERATED */
   N_("%s file not generated."),
   /* STID_CONVERTING_INTO_NAMED_FILE */
   N_("Converting into '%s'..."),
   /* STID_CANNOT_EXEC_FORMAT_NOT_GEN */
   N_("Cannot execute '%s', %s file not generated."),
   /* STID_FORMAT_FILE_PRINTED_INTO */
   N_("%s file printed into '%s'."),
   /* STID_NO_CUR_FILE_CANNOT_GEN_TEXT */
   N_("No current file.\n\nCannot generate text output."),
   /* STID_TEXT_OUTPUT_NOT_GEN */
   N_("Text output not generated."),
   /* STID_TEXT_PRINTED_INTO_NAMED_FILE */
   N_("Text file printed into '%s'."),
   /* STID_TOO_MANY_COLOR_FOR_XPM_EXPORT */
   N_("XPM objects that have more than 255 colors have been skipped during printing/exporting.\n\nYou can use ReduceColors() from the ReduceNumberOfColors Submenu of the ImageProc Menu to reduce their number of colors."),
   /* STID_CANNOT_PRINT_EPS_TILED_PAGE */
   N_("Cannot print in EPS format in Tiled page mode."),
   /* STID_PREPROCESS_PAGE_OF */
   N_("Preprocess page %1d of %1d..."),
   /* STID_GENERATING_PAGE_OF */
   N_("Generating page %1d of %1d..."),
   /* STID_GIVEN_PAGE_NOT_GENERATED */
   N_("Page %1d has not been generated."),
   /* STID_PAGE_RANGE_NOT_GENERATED */
   N_("Pages %1d through %1d have not been generated."),
   /* STID_PRINTWITHCMD_WORKS_PRINTER */
   N_("PrintWithCmd() only works when output device is the printer."),
   /* STID_ENTER_PRINT_CMD_NAME */
   N_("Please enter print command name:"),
   /* STID_NO_OBJ_SEL_NOTHING_TO_PRINT */
   N_("No objects selected.\n\nNothing printed."),
   /* STID_SPECIFY_PERCENT_REDUCTION */
   N_("Please specify percent reduction (<100) or enlargement (>100):"),
   /* STID_INVALID_REDUCTION */
   N_("Invalid reduction '%s'."),
   /* STID_NEW_REDUCTION_IS_PERCENT */
   N_("New reduction is %s%%."),
   /* STID_NEW_ENLARGEMENT_IS_PERCENT */
   N_("New enlargement is %s%%."),
   /* STID_OK_TO_CLEAR_WHITEBOARD */
   N_("Okay to clear WhiteBoard?"),
   /* STID_FILE_MOD_SAVE_BEFORE_CLEAR */
   N_("File modified, save file before clear? [ync](y)"),
   /* STID_EDITING_NO_FILE */
   N_("Editing no file."),
   /* STID_OPEN_IN_WB */
   N_("Open() is disabled in WhiteBoard mode."),
   /* STID_FILE_MOD_SAVE_BEFORE_OPEN */
   N_("File modified, save file before open? [ync](y)"),
   /* STID_SELECT_A_FILE_TO_OPEN */
   N_("Please select a file to OPEN..."),
   /* STID_INVALID_PAGE_SPECIFIED_FOR */
   N_("Invalid page specified for '%s'."),
   /* STID_SETTEMPLATE_IN_WB */
   N_("SetTemplate() is disabled in WhiteBoard mode."),
   /* STID_SELECT_FILE_AS_TEMPLATE */
   N_("Please select a file to be used as template..."),
   /* STID_NOT_SUP_REMOTE_TEMPLATE_FILE */
   N_("Using a remote template file is not supported."),
   /* STID_CANNOT_OPEN_STILL_TEMPLATE */
   N_("Cannot open '%s' for reading.\n\nWould you still like to use it as a template file?"),
   /* STID_STILL_USE_AS_TEMPLATE */
   N_("Would you still like to use '%s' as a template file?"),
   /* STID_TEMPLATE_SET_TO */
   N_("Template set to '%s'."),
   /* STID_FILE_MOD_SAVE_BEFORE_QUIT */
   N_("File modified, save file before quit? [ync](y)"),
   /* STID_ANCESTER_MOD_STILL_QUIT */
   N_("Ancester file modified, still quitting? [ync](y)"),
   /* STID_ILLEGAL_DOUBLE_BYTE_TRUNC */
   N_("Illegal double-byte string truncated:"),
   /* STID_TOOL_CANT_HANDLE_DB_FONT */
   N_("The '%s' double-byte font has min_byte1=\\%03o and max_byte1=\\%03o.\n%s does not know how to handle this double-byte font.\nPlease don't use this font and e-mail this error message to %s."),
   /* STID_FAIL_LOAD_FONT_FONT_SUBS */
   N_("Fail to load the following font: '%s'.\n\n%s-%1d substituted for %s-%1d."),
   /* STID_FONT_SUBSTITUTED */
   N_("%s-%1d substituted for %s-%1d."),
   /* STID_CANNOT_GET_IMAGE_OF_SIZE */
   N_("Cannot get image of size [%1dx%1d].  System resource may be low."),
   /* STID_UNRECOG_CHARCODE_DISCARD */
   N_("Unrecognized character code \\03%o.  Character discarded."),
   /* STID_CANT_FIND_FONT_USE_ALT */
   N_("Cannot find the %s font.  Use '%s' instead."),
   /* STID_BAD_XDEF_CANT_FIND_EQ_SKIP */
   N_("Invalid entry '%s' (cannot find '=') in %s.%s, skipped."),
   /* STID_BAD_XDEF_MISS_FONT_NAME_SKIP */
   N_("Invalid entry '%s' (missing font name) in %s.%s, skipped."),
   /* STID_MISS_FONT_SPEC_IN_XDEF */
   N_("Missing font specification in '%s.%s'."),
   /* STID_INVALID_X_FONT_INFO_IN_XDEF */
   N_("Invalid X font information in '%s.%s': '%s'."),
   /* STID_INVALID_X_REG_INFO_IN_XDEF */
   N_("Invalid X registry information in '%s.%s': '%s'."),
   /* STID_INVALID_PS_INFO_IN_XDEF */
   N_("Invalid PS font information in '%s.%s': '%s'."),
   /* STID_ERR_PROCESS_FONT_USE_ALT */
   N_("Warning: Error in processing %s.%s: '%s'.\n\t'%s-%s-*-%%d-*-*-*-*-*-%s' used."),
   /* STID_INVALID_SB_FONT_USE_DB_FONT */
   N_("Invalid %s.%s: '%s'.  Please use a non-double-byte font."),
   /* STID_ERR_PROCESS_SIZE_USE_DEF_SIZE */
   N_("Warning: Error in processing %s.%s.  Default font sizes used."),
   /* STID_OBSOLETE_XDEF_USE_ALT */
   N_("Obsoleted %s.%s used.  Please use %s.%s instead."),
   /* STID_CANT_OPEN_DEF_MSG_FONT_ABORT */
   N_("Cannot open the Default(Msg)Font '%s'."),
   /* STID_WARN_VERY_SMALL_DEF_FONT */
   N_("Warning: Very small default font width/height (%1d/%1d).\n\tPossibly a problem with the font path.\n\tSet default font width to 9 and height to 14 as a temporary fix."),
   /* STID_CANT_OPEN_RULER_FONT_ABORT */
   N_("Cannot open the RulerFont '%s'."),
   /* STID_WARN_VERY_SMALL_RULER_FONT */
   N_("Warning: Very small ruler font width/height (%1d/%1d).\n\tPossibly a problem with the font path.\n\tSet ruler font width to 7 and height to 12 as a temporary fix."),
   /* STID_CANNOT_SET_INITIAL_FONT_TO */
   N_("Warning: Cannot set InitialFont to '%s'."),
   /* STID_CANT_SET_INIT_FONTSTYLE_ALT */
   N_("Warning: Cannot set InitialFontStyle to '%s'.  '%s' font is used instead."),
   /* STID_CANT_SET_INIT_FONTJUST_ALT */
   N_("Warning: Cannot set InitialFontJust to '%s'.  '%s' is used instead."),
   /* STID_CANT_SET_INIT_FONTSIZE_ALT */
   N_("Warning: Cannot set InitialFontSize to '%s'.  '%s' is used instead."),
   /* STID_FONT_NOT_AVAILABLE */
   N_("The '%s' font is not available."),
   /* STID_CANT_CHANGE_SIZEPT_TO_USE_ALT */
   N_("Cannot change size to %1dpt.  %1dpt used."),
   /* STID_CANT_CHANGE_SIZE_TO_USE_ALT */
   N_("Cannot change size to %1d.  %1d used."),
   /* STID_INVALID_VSPACE_NOT_CHANGED */
   N_("Invalid vertical spacing for a text object.  Vertical spacing for that object not changed."),
   /* STID_VSPACE_TOO_SMALL_NOT_CHANGED */
   N_("Text vertical spacing too small.  No change."),
   /* STID_ENTER_POINT_SIZE */
   N_("Please enter point size:"),
   /* STID_ENTER_FONT_SIZE */
   N_("Please enter font size:"),
   /* STID_FONT_SIZE_TOO_SMALL */
   N_("Font size '%s' too small."),
   /* STID_FONT_SUB_NO_SUCH_FONT_USR_DEF */
   N_("%s substituted for font number %d.\n\nThere is no such font. Use default."),
   /* STID_NO_KANJI_FONT_GIVEN_NUMBER */
   N_("There is no kanji-font number %d."),
   /* STID_FAIL_TO_CONN_TO_HOST */
   N_("%s: Fail to connect to server on '%s'."),
   /* STID_NETWORK_READ_ERROR */
   N_("%s: Network read error."),
   /* STID_NETWORK_ERROR */
   N_("%s: Network error."),
   /* STID_CONN_ABORT_BY_USER */
   N_("%s: Aborted by the user."),
   /* STID_CONN_TERM_BY_SERVER */
   N_("%s: Connection terminated by the server."),
   /* STID_FAIL_TO_OPEN_DATA_SOCKET */
   N_("%s: Fail to open a socket for FTP data port."),
   /* STID_USING_METRIC_SYSTEM */
   N_("Using Metric system."),
   /* STID_USING_ENGLISH_SYSTEM */
   N_("Using English system."),
   /* STID_SNAP_NOT_ON_GRID_SZ_SAME */
   N_("Snap is not on, grid size not changed."),
   /* STID_AT_MAX_GRID_GRID_SZ_SAME */
   N_("Already at maximum grid, grid size not changed."),
   /* STID_AT_MIN_GRID_GRID_SZ_SAME */
   N_("Already at minimum grid, grid size not changed."),
   /* STID_SNAP_TO_GRID_ACTV */
   N_("Snapping to grid point activated."),
   /* STID_SNAP_TO_GRID_DISABLED */
   N_("Snapping to grid point disabled."),
   /* STID_WILL_PRINT_IN_COLOR */
   N_("Will print in color (PS and pixmap)."),
   /* STID_WILL_PRINT_IN_BW */
   N_("Will print in black-and-white (PS and bitmap)."),
   /* STID_CONSTRAINED_MOVE */
   N_("Constrained move."),
   /* STID_UNCONSTRAINED_MOVE */
   N_("Unconstrained move."),
   /* STID_WILL_DISPLAY_XBM_XPM */
   N_("Will display bitmap/pixmap."),
   NULL
};

static char *gaszEnglishStrTable500[] = {
   /* STID_WILL_NOT_DISPLAY_XBM_XPM */
   N_("Will not display bitmap/pixmap."),
   /* STID_GRAY_SCALE_IN_BW_PRINTING */
   N_("Gray scale enabled in black&white printing."),
   /* STID_NO_GRAY_SCALE_IN_BW_PRINTING */
   N_("Gray scale disabled in black&white printing."),
   /* STID_CURRENT_SPEC_IS */
   N_("( current spec: '%s' )"),
   /* STID_ENTER_MEASURE_UNIT_SPEC */
   N_("Please enter '<num> {pixel|cm|in}/<unit>' (e.g., 36 in/yd or 0.1 in/mil):"),
   /* STID_WILL_EXPORT_FORMAT_FILE */
   N_("Will export %s file."),
   /* STID_PRINT_DEV_SET_TO_PRINTER */
   N_("Print device set to printer."),
   /* STID_WILL_EXPORT_EPS_FILE */
   N_("Will export Encapsulated PostScript (LaTeX-Figure) file."),
   /* STID_WILL_EXPORT_RAW_PS_FILE */
   N_("Will export raw PostScript file."),
   /* STID_WILL_EXPORT_XPM_FILE */
   N_("Will export X11 Pixmap file."),
   /* STID_WILL_EXPORT_XBM_FILE */
   N_("Will export X11 Bitmap file."),
   /* STID_WILL_EXPORT_TEXT_FILE */
   N_("Will export ASCII text file."),
   /* STID_WILL_EXPORT_EPSI_FILE */
   N_("Will export EPSI file (EPS with preview bitmap)."),
   /* STID_WILL_EXPORT_GIF_FILE */
   N_("Will export GIF/ISMAP (needs xpm->gif filter) file."),
   /* STID_WILL_EXPORT_HTML_FILE */
   N_("Will export HTML (with Client-side imagemap) file."),
   /* STID_WILL_EXPORT_PDF_FILE */
   N_("Will export PDF (needs ps->pdf filter) file."),
   /* STID_WILL_EXPORT_TIFFEPSI_FILE */
   N_("Will export DOS/Windows EPS/EPSI file."),
   /* STID_ONE_MOTION_SEL_MOVE_MODE */
   N_("Click-select-move in one motion mode selected."),
   /* STID_CLICK_SEL_CLICK_MOVE_MODE */
   N_("Click-select-click-move mode selected."),
   /* STID_CANT_TOGGLE_COLOR_LAYER_BW */
   N_("Cannot toggle color layers for a black and white display."),
   /* STID_STRETCHABLE_TEXT_MODE */
   N_("Text objects are stretchable."),
   /* STID_NON_STRETCHABLE_TEXT_MODE */
   N_("Text objects are not stretchable."),
   /* STID_TRANS_PAT_MODE */
   N_("Fill and pen patterns are transparent."),
   /* STID_NON_TRANS_PAT_MODE */
   N_("Fill and pen patterns are opaque."),
   /* STID_ALREADY_AT_DEF_ZOOM */
   N_("Already at the default zoom."),
   /* STID_ALREADY_AT_HIGHEST_MAG */
   N_("Already at highest magnification, can no longer zoom in."),
   /* STID_CURSOR_IGNORED_CSBTN1 */
   N_("(the cursor position is ignored even though <Cntrl><Shift>Btn1 is used)"),
   /* STID_ZOOMIN_AROUND_EDIT_TEXT */
   N_("(Zooming in around text being edited..."),
   /* STID_SEL_ZOOM_CENTER */
   N_("Select zoom center"),
   /* STID_ZOOMIN_CANCEL_BY_USER */
   N_("ZoomIn canceled by the user."),
   /* STID_AT_PAPER_EDGE_CANT_ZOOMOUT */
   N_("Already at paper boundaries, can no longer zoom out."),
   /* STID_PAGE_STYLE_CHANGE_TO_LAND */
   N_("Page style changed to LandScape."),
   /* STID_PAGE_STYLE_CHANGE_TO_PORT */
   N_("Page style changed to Portrait."),
   /* STID_WARN_PIN_NOT_SUPPORTED */
   N_("Warning: Pins are not supported, yet."),
   /* STID_ONE_SIMPLE_GROUP_UNGROUP_ANY */
   N_("Selected object is not a (simple) grouped object.\n\nUngroup anyway?"),
   /* STID_ALL_SIMPLE_GROUP_UNGROUP_ANY */
   N_("None of the selected objects are (simple) grouped objects.\n\nUngroup anyway?"),
   /* STID_SEL_OBJ_ARE_UNGROUPED */
   N_("Selected objects are ungrouped."),
   /* STID_NO_OBJ_TO_LOCK */
   N_("No object to lock."),
   /* STID_CANNOT_LOCK_IN_VERTEX_MODE */
   N_("Cannot lock in vertex mode."),
   /* STID_SEL_OBJ_ARE_LOCKED */
   N_("Selected objects are locked."),
   /* STID_NO_OBJ_TO_UNLOCK */
   N_("No object to unlock."),
   /* STID_CANNOT_UNLOCK_IN_VERTEX_MODE */
   N_("Cannot unlock in vertex mode."),
   /* STID_SEL_OBJ_ARE_UNLOCKED */
   N_("Selected objects are unlocked."),
   /* STID_TOOL_VERSION */
   N_("%s Version %s"),
   /* STID_TOOL_VERSION_SPC_BUILD */
   N_("%s Version %s (%s)"),
   /* STID_TOOL_VERSION_PATCH */
   N_("%s Version %s (patchlevel %1d)"),
   /* STID_TOOL_VERSION_PATCH_SPC_BUILD */
   N_("%s Version %s (patchlevel %1d - %s)"),
   /* STID_HYPER_TEXT_HOME */
   N_("\n\nWWW Hypertext Home Page: %s"),
   /* STID_LATEST_REL_INFO */
   N_("\n\nLatest Release Information: %s"),
   /* STID_HYPER_GRAPHICS_INFO */
   N_("\n\nWWW Hyper-Graphics Home Page: %s"),
   /* STID_MAILING_LIST_INFO */
   N_("\n\nThe tgif announcement mailing list is %s.  To join, please send an empty message to %s (you do not need a Yahoo ID to join).  Additional information regarding tgif mailing lists can be found at: %s"),
   /* STID_SEND_BUG_REPORT_TO */
   N_("\n\nPlease send bug reports to %s."),
   /* STID_OPEN_URL_FAIL_USE_BROWSER */
   N_("Fail to open '%s' for reading.\n\nPlease view %s with a web browser."),
   /* STID_FIND_VER_FAIL_USE_BROWSER */
   N_("Fail to find version information in '%s'.\n\nPlease view %s with a web browser."),
   /* STID_TOOL_CUR_VER_IS_DIFF_INFO */
   N_("The latest release of %s is %s.\nThe %s you are running is Version %s.\n\nFor download information, please visit %s with a web browser."),
   /* STID_TOOL_CUR_VER_IS_DIFF_SPC_INFO */
   N_("The latest release of %s is %s.\nThe %s you are running is Version %s (%s).\n\nFor download information, please visit %s with a web browser."),
   /* STID_TOOL_CUR_VER_PATCH_INFO */
   N_("The latest release of %s is %s.\nThe %s you are running is Version %s Patchlevel %1d.\n\nFor download information, please visit %s with a web browser."),
   /* STID_TOOL_CUR_VER_PATCH_SPC_INFO */
   N_("The latest release of %s is %s.\nThe %s you are running is Version %s Patchlevel %1d (%s).\n\nFor download information, please visit %s with a web browser."),
   /* STID_CANT_DOWNLOAD_VER_USE_BROWSER */
   N_("Cannot download '%s' to get version information.\n\nPlease view %s with a web browser."),
   /* STID_FAIL_TO_CONN_TO_HOST_PORT */
   N_("%s: Fail to connect to server on '%s:%1d'."),
   /* STID_LINES_TOO_LONG_CONTENT_LENGTH */
   N_("%s: Unexpected lines too long detected while reading content."),
   /* STID_FAIL_TO_SEND_REQ */
   N_("%s: Fail to send requests."),
   /* STID_INVALID_FORMAT_IN_HEADER */
   N_("%s: Invalid format in the header."),
   /* STID_CHECK_FONT_FAILED_FOR_BROWSE */
   N_("The current font/size/style is not available for labeling objects.\n\nPlease use a different font/size/style for the browse operation you've selected."),
   /* STID_ERROR_ENCOUNTERED_WHILE_RECV */
   N_("%s: Error encountered in receiving responses."),
   /* STID_SINGLE_XPM_IMGPROC */
   N_("The command you have just selected: '%s' can only work with a single X11 Pixmap object."),
   /* STID_USER_INTR */
   N_("User interrupt."),
   /* STID_CANNOT_IMPORT_XPM_FILE */
   N_("Cannot import X11 Pixmap file '%s'."),
   /* STID_MAY_USED_UP_COLORS_RETRY */
   N_("May have used up all the colors.\n\nWould you like to retry it once more?"),
   /* STID_ENTER_PAIR_COLORS_INTERPOLATE */
   N_("Please enter a pair of colors (from dark to bright) for interpolation (e.g., Black Yellow):"),
   /* STID_GIVEN_IS_NOT_A_VALID_COLOR */
   N_("'%s' is not a valid color."),
   /* STID_ENTER_VAL_MINUS_PLUS_ONE_BW */
   N_("Please enter a value between -1.0 (all black) and +1.0 (all white):"),
   /* STID_FAIL_TO_PARSE_FOR_A_VAL */
   N_("Fail to parse '%s' for a value."),
   /* STID_ENTER_VAL_MINUS_PLUS_ONE_SAT */
   N_("Please enter a value between -1.0 (gray) and +1.0 (saturated):"),
   /* STID_ENTER_VAL_FOR_CHANGE_HUE */
   N_("Please enter two pairs of (color,angle) values: (e.g., \"red 60 green 60\" maps all reddish to greenish colors)"),
   /* STID_FAIL_TO_PARSE_FOR_4_VAL */
   N_("Fail to parse '%s' for 4 values."),
   /* STID_ENTER_VAL_FOR_CONTRAST_ENH */
   N_("Please enter a non-negative value (1.0 means no adjustment):"),
   /* STID_GIVEN_NEG_VAL_NOT_ALLOWED */
   N_("Nevative value: '%s' is not allowed."),
   /* STID_ENTER_VAL_FOR_COLOR_BAL */
   N_("Please enter non-negative R G B factors (1.0 1.0 1.0 means no change):"),
   /* STID_FAIL_TO_PARSE_FOR_3_VAL */
   N_("Fail to parse '%s' for 3 values."),
   /* STID_ENTER_VAL_FOR_GAMMA */
   N_("Please enter a gamma value (1.0 means no change, 2.2 is brighter):"),
   /* STID_SEL_TOO_THIN_FLAT_FOR_EDGE */
   N_("Selected object is too thin or flat for edge detection."),
   /* STID_SEL_TOO_THIN_FLAT_FOR_EMBOSS */
   N_("Selected object is too thin or flat for embossing."),
   /* STID_ENTER_NUM_COLORS_TO_REDUCE_TO */
   N_("Please enter the number of colors to reduce to (from %1d colors):"),
   /* STID_NUM_COLORS_BETWEEN_2_N_GIVEN */
   N_("Number of colors: '%s' must be between 2 and %1d."),
   /* STID_SEL_XPM_FILE_FOR_RED_COLORS */
   N_("Please select an X11 Pixmap file for colors..."),
   /* STID_INVALID_GIVEN_XPM_FILE */
   N_("Invalid X11 Pixmap file '%s.'"),
   /* STID_Q_FS_ERROR_DIFFUSE */
   N_("Would you like to apply Floyd-Steinberg error diffusion (slow)?"),
   /* STID_ENTER_NUM_BITS_IN_RGB */
   N_("Please enter number of bits to use for R, G, and B (current [R G B] levels are [%1d %1d %1d]):"),
   /* STID_RGB_LEVELS_CHANGED_TO */
   N_("Levels changed to: [%1d %1d %1d]."),
   /* STID_BAD_VAL_SUM_RGB_LEVEL */
   N_("Bad values: '%s'.  R+G+B must be <= 8."),
   /* STID_BAD_VAL_GT_0_RGB_LEVEL */
   N_("Bad values: '%s'.  Values must all be > 0."),
   /* STID_RGB_LEVELS_ARE */
   N_("[R G B] levels are [%1d %1d %1d]."),
   /* STID_ENTER_INT_AMT_TO_SPREAD */
   N_("Please enter an integer amount to spread:"),
   /* STID_SEL_TOO_THIN_FLAT_FOR_SHARPEN */
   N_("Selected object is too thin or flat for sharpening."),
   /* STID_SEL_TOO_THIN_FLAT_FOR_BLUR */
   N_("Selected object is too thin or flat for %1d by %1d blurring."),
   /* STID_IMAGE_PROC_CANT_USE_XFORMED */
   N_("'%s' cannot operate on stretched/rotated/sheared pixmap objects.\n\nYou can use '%s' in the ImageProc Menu to regenerate these objects."),
   /* STID_SEL_3_XPM_FOR_IMAGEPROC_CMD */
   N_("Please select 3 X11 Pixmap object for the '%s' command."),
   /* STID_SEL_2_XPM_FOR_IMAGEPROC_CMD */
   N_("Please select 2 X11 Pixmap object for the '%s' command."),
   /* STID_PICK_PT_WITHIN_IMAGE_BOUND */
   N_("Please pick a starting point within the image boundary"),
   NULL
};

static char *gaszEnglishStrTable600[] = {
   /* STID_IMG_TOO_SMALL_FOR_IMAGEPROC_CMD */
   N_("Image is too small for the '%s' command."),
   /* STID_NO_WARPING */
   N_("No warping."),
   /* STID_START_LINE_SEG_DOTS */
   N_("Start line segment..."),
   /* STID_ENDT_LINE_SEG_DOTS */
   N_("End line segment..."),
   /* STID_ENTER_IMAGE_SIZE_IN_PIX_RC */
   N_("Please enter image size in pixels: [Col x Row]"),
   /* STID_FAIL_TO_EXECUTE_CMD */
   N_("Fail to execute '%s'."),
   /* STID_ENTER_CMD_OP_FOR_BGGEN */
   N_("Please enter command options for 'bggen' (e.g., 'blue magenta'):"),
   /* STID_NEW_XPM_WH_GENERATED */
   N_("New X11 Pixmap object (%1dx%1d) generated."),
   /* STID_ENTER_GRAY_LEVELS_222 */
   N_("Please enter number of gray levels (between 2 and 222):"),
   /* STID_INVALID_GIVEN_VALUE_ENTERED */
   N_("Invalid value '%s' entered."),
   /* STID_ENTER_COLOR_FOR_RECT_BGGEN */
   N_("Please enter a color for the rectangle (e.g., magenta, #808080):"),
   /* STID_CANNOT_PARSE_NAMED_COLOR */
   N_("Cannot parse color: '%s'."),
   /* STID_XPM_NOT_XFORMED_REGEN_ANYWAY */
   N_("Selected object is not stretched/rotated/sheared.\n\nWould you like to regenerate it anyway?"),
   /* STID_DRAG_A_RECT_TO_CROP */
   N_("Please drag out a rectangular area to crop..."),
   /* STID_START_CROP_IMAGE */
   N_("Start crop image"),
   /* STID_END_CROP_IMAGE */
   N_("End crop image"),
   /* STID_ABORT_CROP_IMAGE */
   N_("Abort crop image"),
   /* STID_SELECT_A_COLOR */
   N_("Select a color"),
   /* STID_SEL_A_COLOR_TO_USE_AS_CUR */
   N_("Please select a color to be used as the current color..."),
   /* STID_SEL_AREA_NOT_INTERSECT_IMAGE */
   N_("Selected area does not intersect selected image."),
   /* STID_SEL_PT_NOT_ON_IMAGE */
   N_("Selected point is not on the selected image."),
   /* STID_ONE_PRIM_FOR_IMAGEPROC_CMD */
   N_("Please select only one primitive object for the '%s' command."),
   /* STID_DARG_TO_FILL_AN_AREA */
   N_("Drag to fill an area..."),
   /* STID_SEL_A_COLOR_TO_BE_REPLACED */
   N_("Please select a color to be replaced by the current color..."),
   /* STID_SEL_A_COLOR_TO_REPLACE */
   N_("Select a color to replace"),
   /* STID_BTN1_FLOODFILL_BTN3_SET_COLOR */
   N_("<Btn1>: Flood-fill, <Btn3>: Set a pixel to current color."),
   /* STID_ESC_TO_FINISH */
   N_("<ESC> to finish."),
   /* STID_FLOOD_FILL */
   N_("Flood-fill"),
   /* STID_SET_A_PIXEL */
   N_("Set a pixel"),
   /* STID_NO_CONTOUR_CAN_BE_GEN_HERE */
   N_("No contour can be generated from here."),
   /* STID_SEL_A_COLOR_TO_BE_TRACED */
   N_("Please select a color to be traced..."),
   /* STID_START_CONTOUR */
   N_("Start contour"),
   /* STID_INVALID_XDEF_RNG_USE_ALT_VAL */
   N_("Invalid %s.%s: '%s', (must be between %1d and %1d), %1d is used instead."),
   /* STID_VAL_TOO_LARGE_IN_XDEF_USE_ALT */
   N_("Values too large in %s.%s: '%s', '%s' is used instead."),
   /* STID_VAL_TOO_SMALL_IN_XDEF_USE_ALT */
   N_("Values too small in %s.%s: '%s', '%s' is used instead."),
   /* STID_INVALID_GIVEN_PATH_MUST_FULL */
   N_("Invalid path: '%s'.  Must use a full path name."),
   /* STID_CANNOT_SET_LOCALE_ENV_VARS */
   N_("Cannot set to the locale specified by the environment variables."),
   /* STID_XT_CANNOT_SET_LOCALE_ENV_VARS */
   N_("Xtoolkit cannot set to the locale specified by the environment variables."),
   /* STID_XLIB_CANT_SET_LOCALE_ENV_VARS */
   N_("Xlib cannot set to the locale specified by the environment variables."),
   /* STID_CANT_SET_LOCALE_MODIFIERS */
   N_("Cannot set locale modifiers."),
   /* STID_LOCALE_DIR_NOT_EXIST_USE_XDEF */
   N_("Locale directory '%s' does not exist.  Please specify locale direory with %s.%s."),
   /* STID_WARN_CANT_PUTENV_GIVEN */
   N_("Warning: Cannot putenv(%s)."),
   /* STID_CANT_OPEN_DISPLAY_ABORT */
   N_("Could not open the default display.  Abort."),
   /* STID_CANT_OPEN_GIVEN_DPY_ABORT */
   N_("Could not open display '%s'.  Abort."),
   /* STID_CUR_EMPTY_FILE_IS */
   N_("Current (empty) file is '%s'."),
   /* STID_LEFT_STEP_MID_RUN_RIGHT_STOP */
   N_("Left:step.  Middle:run.  Right:stop."),
   /* STID_CANT_FIND_POLY_WITH_GIVEN_ID */
   N_("Cannot find poly with id=%1d."),
   /* STID_CANT_FIND_OBJ_WITH_GIVEN_ID */
   N_("Cannot find object with id=%1d."),
   /* STID_CANT_FIND_ATTR_NAME_AND_COLOR */
   N_("Cannot find attr name '%s' and color '%s'."),
   /* STID_BAD_FLTR_SPEC_DEF */
   N_("Invalid %s.%s: '%s'.\n\nFormat is '<name> <extensions> <filter spec>', e.g.,\n\n    JPEG jpg;jpeg djpeg -gif -colors 222 %%s | giftopnm | ppmtoxpm"),
   /* STID_BAD_FLTR_SPEC_MISS_PERC_S */
   N_("Invalid %s.%s: '%s' (missing %%s).\n\nFormat is '<name> <extensions> <filter spec>', e.g.,\n\n    JPEG jpg;jpeg djpeg -gif -colors 222 %%s | giftopnm | ppmtoxpm"),
   /* STID_BAD_FLTR_SPEC_MANY_PERC_S */
   N_("Invalid %s.%s: '%s' (too many %%s).\n\nFormat is '<name> <extensions> <filter spec>', e.g.,\n\n    JPEG jpg;jpeg djpeg -gif -colors 222 %%s | giftopnm | ppmtoxpm"),
   /* STID_BAD_XDEF_MUST_BE_GT_0 */
   N_("Invalid %s.%s: '%s'.  Value must be > 0."),
   /* STID_CANT_FIND_XDEF_CONT_LOOK_FLTR */
   N_("Cannot find %s.%s.\n\nWould you like to continue looking for filters?"),
   /* STID_SEL_XBM_FILE_TO_IMPORT */
   N_("Please select an X11 Bitmap file to IMPORT..."),
   /* STID_CANNOT_IMPORT_XBM_FILE */
   N_("Cannot import X11 Bitmap file '%s'."),
   /* STID_ENTER_GEOM_SPEC_ORIG_SIZE */
   N_("Please enter geometry spec: [[MAG=]WxH+X+Y] (original size is %1dx%1d)"),
   /* STID_GIVEN_XBM_SIZE_FILE_IMPORTED */
   N_("X11 Bitmap file (%1dx%1d) '%s' imported."),
   /* STID_GIVEN_XPM_SIZE_FILE_IMPORTED */
   N_("X11 Pixmap file (%1dx%1d) '%s' imported."),
   /* STID_SEL_XPM_FILE_TO_IMPORT */
   N_("Please select an X11 Pixmap file to IMPORT..."),
   /* STID_SEL_EPS_FILE_TO_IMPORT */
   N_("Please select an EPS file to IMPORT..."),
   /* STID_IMPORT_REM_FILE_EMBED_INSTEAD */
   N_("Link to a remote %s file is not supported.  Would you like to embed the %s file instead?"),
   /* STID_GIVEN_EPS_FILE_IMPORTED */
   N_("EPS file '%s' imported."),
   /* STID_SEL_GIF_FILE_TO_IMPORT */
   N_("Please select a GIF file to IMPORT..."),
   /* STID_CANNOT_IMPORT_GIVEN_GIF */
   N_("Cannot import GIF file '%s'."),
   /* STID_GIVEN_GIF_SIZE_FILE_IMPORTED */
   N_("GIF file (%1dx%1d) '%s' imported."),
   /* STID_CANT_FIND_ANY_IMP_FLTR_SPEC */
   N_("Cannot find any import filter specifications.\n\nImport filters are specified with %s.%s and %s.%s X resources."),
   /* STID_SEL_AN_IMPORT_FILTER */
   N_("Please select an import filter..."),
   /* STID_SEL_A_TYPE_FILE_TO_IMPORT */
   N_("Please select a %s file to IMPORT..."),
   /* STID_CANT_IMPORT_GIVEN_TYPE_FILE */
   N_("Cannot import %s file '%s'."),
   /* STID_GIVEN_TYPE_SIZE_FILE_IMPORTED */
   N_("%s file (%1dx%1d) '%s' imported."),
   /* STID_INVALID_FMT_IN_SHORTCUT_SPEC */
   N_("Invalid format in shortcut specification of %s(%s)."),
   /* STID_CANT_FIND_NAMED_IMP_FLTR */
   N_("Cannot find any import filter named '%s'."),
   /* STID_SEL_ANIM_GIF_FILE_TO_IMPORT */
   N_("Please select an animated GIF file to IMPORT..."),
   /* STID_CANT_EXEC_CMD_IMPORT_ANIM_GIF */
   N_("Cannot execute '%s'.\n\nAnimated GIF not imported."),
   /* STID_Q_LIKE_TO_USE_AN_IMP_FLTR_GIF */
   N_("Would you like to use an import filter when importing individual GIF files?"),
   /* STID_CANT_CREATE_NAMED_TMP_FILE */
   N_("Cannot create temporary file: '%s'."),
   /* STID_CANT_IMP_ANIM_GIF_TMP_DIR */
   N_("Cannot import GIF animation file '%s'.  Please make sure that you can write to the '%s' directory."),
   /* STID_CANT_IMP_GIVEN_ANIM_GIF */
   N_("Cannot import GIF animation file '%s'."),
   /* STID_ONLY_ONE_GIF_COMP_EXTRACTED */
   N_("Only one GIF component extracted from '%s'."),
   /* STID_ANIMATING_GIVEN */
   N_("Animating '%s'..."),
   /* STID_GIF_ANIMATION_STOPPED */
   N_("GIF animation stopped."),
   /* STID_PRESS_ESC_TO_STOP */
   N_("(press <ESC> to stop)"),
   /* STID_BROWSING_DIR_FOR_TYPE_FILES */
   N_("Browsing '%s' for %s files..."),
   /* STID_SKIP_GIVEN_FILE_SYM_LINK */
   N_("Skipping '%s' because it's a symbolic link."),
   /* STID_WORKING_DIR_IS_GIVEN */
   N_("Working directory: '%s'"),
   /* STID_MORE_THAN_ONE_CMAP_INSTALLED */
   N_("More than one colormap installed."),
   /* STID_FIRST_ONE_ON_LIST_USED */
   N_("First one on the list is used."),
   /* STID_SPECIFY_AREA */
   N_("Specify area"),
   /* STID_SPECIFY_DELAY_FULL_SCR_CAP */
   N_("Please specify a delay (in seconds) for full screen capture:"),
   /* STID_WILL_HIDE_TOOL_WHILE_CAPTURE */
   N_("Will hide %s during capturing."),
   /* STID_WILL_SHOW_TOOL_WHILE_CAPTURE */
   N_("Will leave %s alone during capturing."),
   /* STID_FUNC_SELECT_SYS_CALL_FAILED */
   N_("In %s: select() system call failed."),
   /* STID_FUNC_INVALID_RC_FOR_SELECT */
   N_("In %s: Invalid return code of %1d from the select() system call."),
   /* STID_CNTRL_C_ABORT_LAUNCH */
   N_("You have typed a ^C.  Do you want to abort the command that's being executed?  (If the command is running in an xterm, please try killing or destroying the xterm from the desktop first before doing an abort.)"),
   /* STID_CMD_ABORT_LAUNCH_CLOSE_TOOL */
   N_("The '%s' command is aborted.\n\nHowever, it's not safe to actually kill the command.  Therefore, please close %s at the next convenient point."),
   /* STID_INVALID_GIVEN_ATTR_SPEC */
   N_("Invalid attribute specification: '%s'."),
   /* STID_LAUNCH_NOT_AUTO_IN_HYPER_EXEC */
   N_("Launching is not automatic in hyperspace.  Do you want to execute the following command?\n\n    %s\n\n(If you are not sure about this, just click on NO.)"),
   /* STID_USER_ABORT_LAUNCH_IN_HYPER */
   N_("Launching in hyperspace aborted at user's request."),
   /* STID_MALFORMED_CMD_EXEC_ABORT */
   N_("Malformed '%s' command.\n\nCommand execution aborted."),
   NULL
};

static char *gaszEnglishStrTable700[] = {
   /* STID_NO_OBJ_SEL_WHILE_EXEC_CMD */
   N_("No object is selected while executing the '%s' command."),
   /* STID_CANT_FIND_NAMED_ATTR_EXEC */
   N_("Cannot find the '%s' attribute while executing the '%s' command."),
   /* STID_FILE_ATTR_NOT_ALLOWED_FOR_CMD */
   N_("File attribute '%s' is not appropriate for the '%s' command."),
   /* STID_CANT_FIND_NAMED_OBJ_EXEC */
   N_("Cannot find object named '%s' while executing the '%s' command."),
   /* STID_INVALID_ARG_WHILE_EXEC_CMD */
   N_("Invalid '%s' while executing the '%s' command."),
   /* STID_MODIFY_FILE_ATTR_IN_WB */
   N_("Cannot modify file attributes in WhiteBoard mode."),
   /* STID_USER_INTR_TOOL_WAIT_CLEANUP */
   N_("User interrupt received.  %s is waiting for clean up to complete.\n\nAre you sure you don't want to wait for clean up to complete (this may leave the scripts in an inconsistent state)?"),
   /* STID_FUNC_USER_INTR */
   N_("%s(): User interrupt."),
   /* STID_BAD_EVAL_INT_EXP_EXEC_CMD */
   N_("Invalid evaluation '%s' used (integer expected) while executing the '%s' command."),
   /* STID_BAD_EVAL_FLOAT_EXP_EXEC_CMD */
   N_("Invalid evaluation '%s' used (floating pointer number expected) while executing the '%s' command."),
   /* STID_FAIL_FUNC_CMD_EXEC_ABORT */
   N_("Fail to %s.  Command execution aborted."),
   /* STID_FUNC_ONLY_WORK_COMPOSITE_OBJ */
   N_("%s() only works with composite objects."),
   /* STID_FUNC_FAIL_TO_IMPORT_GIVEN */
   N_("%s(): Fail to import '%s'."),
   /* STID_FUNC_OBJ_NON_XBM_XPM_SUB_OBJS */
   N_("%s(): Object contains non-Bitmap/Pixmap sub-objects."),
   /* STID_WILL_EXPORT_PNG_FILE */
   N_("Will export PNG (needs xpm->png filter) file."),
   /* STID_CANNOT_IMPORT_GIVEN_PNG */
   N_("Cannot import PNG file '%s'."),
   /* STID_GIVEN_PNG_SIZE_FILE_IMPORTED */
   N_("PNG file (%1dx%1d) '%s' imported."),
   /* STID_SEL_PNG_FILE_TO_IMPORT */
   N_("Please select a PNG file to IMPORT..."),
   /* STID_FUNC_DIFF_SZ_XBM_XPM_SUBOBJS */
   N_("%s(): Different sizes Bitmap/Pixmap sub-objects."),
   /* STID_FUNC_MORE_1_XBM_XPM_SUBOBJS */
   N_("%s(): Must have > 1 Bitmap/Pixmap sub-objects."),
   /* STID_FUNC_ARG_RANGE */
   N_("%s(): '%s' must be between %1d and %1d (inclusive).\n\n%1d was specified."),
   /* STID_FUNC_UNDEFINED_ARG */
   N_("%s(): Undefined '%s'."),
   /* STID_INTERNAL_CMD_SELECT_FAILED */
   N_("%s(): select() system call failed."),
   /* STID_CANT_OPEN_FILE_READ_EXEC_CMD */
   N_("Cannot open '%s' for reading while executing the '%s' command."),
   /* STID_FAIL_TO_EXECUTE_CMD_EXEC_CMD */
   N_("Fail to execute '%s' while executing the '%s' command."),
   /* STID_UNEXP_ERR_EXEC_CMD_ABORT */
   N_("Unexpected error while executing the '%s' command.\n\nCommand execution aborted."),
   /* STID_BAD_NAME_TOP_LEVEL_ONLY_CMD */
   N_("Bad object name: '%s'.\n\nOnly top level object can be selected while executing the '%s' command."),
   /* STID_BAD_CURSOR_WHILE_EXEC_CMD */
   N_("Cannot create the '%s' cursor while executing the '%s' command."),
   /* STID_BAD_COLOR_NAME_WHILE_EXEC_CMD */
   N_("Fail to allocate the '%s' color while executing the '%s' command."),
   /* STID_WIDTH_RANGE_WHILE_EXEC_CMD */
   N_("Line width of '%s' is out of range while executing the '%s' command."),
   /* STID_SPLINE_TYPE_WHILE_EXEC_CMD */
   N_("Invalid spline type of '%s' used while executing the '%s' command."),
   /* STID_ARROW_STYLE_WHILE_EXEC_CMD */
   N_("Invalid arrow style of '%s' used while executing the '%s' command."),
   /* STID_DASH_INDEX_WHILE_EXEC_CMD */
   N_("Invalid dash index of '%s' used while executing the '%s' command."),
   /* STID_RCB_RAD_RANGE_WHILE_EXEC_CMD */
   N_("Rcbox radius of '%s' is out of range while executing the '%s' command."),
   /* STID_BAD_TEXT_JUST_WHILE_EXEC_CMD */
   N_("Invalid text justification of '%s' used while executing the '%s' command."),
   /* STID_BAD_FONT_NAME_WHILE_EXEC_CMD */
   N_("Invalid font name of '%s' used while executing the '%s' command."),
   /* STID_UNDO_BUF_FLUSHED_BY_CMD */
   N_("Undo buffer flushed by %s()."),
   /* STID_BAD_EVAL_NUM_EXP_EXEC_CMD */
   N_("Invalid evaluation '%s' used (numeric value expected) while executing the '%s' command."),
   /* STID_WRITE_CUT_BUF_WHILE_EXEC_CMD */
   N_("Writing to cut buffer failed while executing the '%s' command."),
   /* STID_CANT_FIND_DOT_WHILE_EXEC_CMD */
   N_("Cannot find '.' in '%s%c%s' while executing the '%s' command."),
   /* STID_BAD_WIDTH_WHILE_EXEC_CMD */
   N_("Invalid width of '%s' used while executing the '%s' command."),
   /* STID_BAD_HEIGHT_WHILE_EXEC_CMD */
   N_("Invalid height of '%s' used while executing the '%s' command."),
   /* STID_BAD_STYLE_WHILE_EXEC_CMD */
   N_("Invalid style of '%s' used while executing the '%s' command."),
   /* STID_TOO_MANY_OBJ_WHILE_EXEC_CMD */
   N_("Too many objects selected while executing the '%s' command."),
   /* STID_BAD_ATTR_NAME_WHILE_EXEC_CMD */
   N_("Illegal attribute name '%s' used while executing the '%s' command."),
   /* STID_END_AN_EDGE_AT_A_PORT */
   N_("Please end an edge at a port..."),
   /* STID_DRAW_AN_EDGE_BETWEEN_PORTS */
   N_("Please draw an edge between ports..."),
   /* STID_CANT_FIND_VRTX_WHILE_EXEC_CMD */
   N_("Cannot find vertex %1d for an object named '%s' while executing the '%s' command."),
   /* STID_POLY_NOR_PGON_WHILE_EXEC_CMD */
   N_("'%s' is neither a poly nor a polygon object while executing the '%s' command."),
   /* STID_BAD_REM_FNAME_WHILE_EXEC_CMD */
   N_("Invalid remote file name '%s' used while executing the '%s' command."),
   /* STID_CANT_OPEN_FILE_WRITE_EXEC_CMD */
   N_("Cannot open '%s' for writing while executing the '%s' command."),
   /* STID_GIVEN_CMD_EXECUTED */
   N_("%s() command executed."),
   /* STID_SQRT_NEG_NUM_WHILE_EXEC_CMD */
   N_("%s() domain error: Cannot take square-root of a negative number."),
   /* STID_CMD_NOT_AVAIL_ON_PLATFORM */
   N_("The %s() command is not available on this platform."),
   /* STID_BAD_SHORTCUT_WHILE_EXEC_CMD */
   N_("Invalid shortcut name '%s' used while executing the '%s' command."),
   /* STID_STAT_FAILED_WHILE_EXEC_CMD */
   N_("Fail to get file info for '%s' while executing the '%s' command."),
   /* STID_NON_GROUPED_WHILE_EXEC_CMD */
   N_("Non-compound object named '%s' specified while executing the '%s' command."),
   /* STID_BAD_SEP_LONG_WHILE_EXEC_CMD */
   N_("Invalid separator '%s' specified while executing the '%s' command.\n\n(Separator must be a single charactor.)"),
   /* STID_BAD_SEP_WHILE_EXEC_CMD */
   N_("Invalid separator '%s' specified while executing the '%s' command."),
   /* STID_MORE_THAN_ONE_WHILE_EXEC_CMD */
   N_("More than one object selected while executing the '%s' command."),
   /* STID_MUST_FULL_PATH_WHILE_EXEC_CMD */
   N_("Invalid path: '%s' specified while executing the '%s' command.\n\n(Must use an absolute path.)"),
   /* STID_OBJ_ALRDY_SEL_WHILE_EXEC_CMD */
   N_("Object named '%s' is already selected while executing the '%s' command."),
   /* STID_INVALID_FNAME_WHILE_EXEC_CMD */
   N_("Invalid file name '%s' specified while executing the '%s' command."),
   /* STID_INVALID_FMODE_WHILE_EXEC_CMD */
   N_("Invalid file mode '%s' specified while executing the '%s' command."),
   /* STID_INVALID_FNUM_WHILE_EXEC_CMD */
   N_("Invalid file number '%s' specified while executing the '%s' command."),
   /* STID_FNUM_ARDY_OPEN_WHILE_EXEC_CMD */
   N_("File number %1d is already opened while executing the '%s' command."),
   /* STID_CANT_OPEN_FMOD_WRITE_EXEC_CMD */
   N_("Cannot open '%s' in the '%s' mode for writing while executing the '%s' command."),
   /* STID_FNUM_NOT_OPEN_WHILE_EXEC_CMD */
   N_("File number %1d is not opened while executing the '%s' command."),
   /* STID_EOF_FOR_FNUM_WHILE_EXEC_CMD */
   N_("Cannot read beyond the end-of-file for file number %1d while executing the '%s' command."),
   /* STID_WRI_FNUM_FAIL_WHILE_EXEC_CMD */
   N_("Fail to write to file number %1d while executing the '%s' command."),
   /* STID_WRI_FNN_FAIL_WHILE_EXEC_CMD */
   N_("Fail to write to file number %1d ('%s') while executing the '%s' command."),
   /* STID_INVALID_PATH_WHILE_EXEC_CMD */
   N_("File '%s' does not exist while executing the '%s' command."),
   /* STID_APPEND_FAILED_WHILE_EXEC_CMD */
   N_("Appending '%s' to '%s' failed while executing the '%s' command."),
   /* STID_COPY_FAILED_WHILE_EXEC_CMD */
   N_("Copying '%s' to '%s' failed while executing the '%s' command."),
   /* STID_INVALID_FORMAT_WHILE_EXEC_CMD */
   N_("Invalid format '%s' specified while executing the '%s' command."),
   /* STID_INVALID_REC_WHILE_EXEC_CMD */
   N_("Invalid rectangle '(%d,%d,%d,%d)' specified while executing the '%s' command."),
   /* STID_CONVERT_FORMAT_WHILE_EXEC_CMD */
   N_("Fail to convert '%s' from the '%s' format to XPM format while executing the '%s' command."),
   /* STID_IMPORT_FAILED_WHILE_EXEC_CMD */
   N_("Fail to import '%s' while executing the '%s' command."),
   /* STID_BAD_VER_NUM_WHILE_EXEC_CMD */
   N_("Invalid version number '%1d' specified while executing the '%s' command."),
   /* STID_EMPTY_INI_SEC_WHILE_EXEC_CMD */
   N_("The [%s] section of '%s' is empty or non-existent while executing the '%s' command."),
   /* STID_BAD_LINENUM_WHILE_EXEC_CMD */
   N_("Invalid line number '%1d' specified while executing the '%s' command."),
   /* STID_BAD_SMALL_LNUM_WHILE_EXEC_CMD */
   N_("Invalid line number '%1d' (too small) specified while executing the '%s' command."),
   /* STID_BAD_EMPTY_STR_WHILE_EXEC_CMD */
   N_("Invalid empty string specified while executing the '%s' command."),
   /* STID_BAD_DIRECTION_WHILE_EXEC_CMD */
   N_("Invalid direction '%s' specified while executing the '%s' command."),
   /* STID_NO_START_VRTX_WHILE_EXEC_CMD */
   N_("No starting vertex specified while executing the '%s' command."),
   /* STID_TOO_FEW_VRTX_WHILE_EXEC_CMD */
   N_("Too few vertices (%1d) specified while executing the '%s' command."),
   /* STID_NO_START_OBJ_WHILE_EXEC_CMD */
   N_("No starting object specified by %s() while executing the '%s' command."),
   /* STID_CANT_FIND_MARK_WHILE_EXEC_CMD */
   N_("Cannot find the object marker for %s() while executing the '%s' command."),
   /* STID_NO_OBJ_GROUP_WHILE_EXEC_CMD */
   N_("No object to group while executing the '%s' command."),
   /* STID_CANT_GRP_SNGL_WHILE_EXEC_CMD */
   N_("Cannot group a single object while executing the '%s' command."),
   /* STID_UNRECOG_CMD_MAY_BE_BLANKS_ABT */
   N_("Unrecognized command: '%s' (may be caused by illegal trailing blanks in command name).\n\nCommand execution aborted."),
   /* STID_UNRECOG_CMD_DOWNLOAD_TOOL */
   N_("Unrecognized command: '%s'.\n\nCommand execution aborted.\n\nIt is possible that the command is supported in a newer version of %s.  Please visit '%s' to download the latest release."),
   /* STID_UNRECOG_CMD_ABORT */
   N_("Unrecognized command.  Command execution aborted."),
   /* STID_TOOL_NOT_DONE_WITH_PREV_FUNC */
   N_("%s is not finished with the previous %s yet."),
   /* STID_ENTER_INTERN_CMD_FILE_NAME */
   N_("Please specify the file name for a %s internal command file (enter '-' for standard input)."),
   /* STID_FINISH_EXEC_CMD_FROM_STDIN */
   N_("Finished executing commands from the standard input."),
   /* STID_FATAL_ERROR_IN_FUNC_PLUS_DESC */
   N_("Fatal Error in %s: %s"),
   /* STID_TOOL_ABORTED */
   N_("%s aborted."),
   /* STID_ICON_FILEVER_TOO_LARGE */
   N_("Icon file version too large (=%1d)."),
   /* STID_UNEQUAL_DEPTHS_IN_FUNC */
   N_("Unequal depths in %s."),
   NULL
};

static char *gaszEnglishStrTable800[] = {
   /* STID_BASELINE_YS_NOT_EQ_IN_FUNC */
   N_("Error: left_baseline_y != right_baseline_y in %s."),
   /* STID_LOCKED_OBJS_CANT_BE_MOVED */
   N_("Locked object(s) cannot be moved."),
   /* STID_HINT_CTL_MOVE_ATTR_ONLY */
   N_("(HINT: You can hold down <Cntrl> key to move only the attribute under the cursor.)"),
   /* STID_OUT_OF_VMEM_CANNOT_MALLOC */
   N_("Out of virtual memory and cannot malloc()."),
   /* STID_CANT_ALLOC_BITMAP_OF_SIZE */
   N_("Cannot allocate bitmap of size %1dx%1d."),
   /* STID_FAIL_TO_WRITE_TO_FILE */
   N_("Fail to write to '%s'.  File system may be full."),
   /* STID_SAFEST_SAVE_N_EXIT_REPRODUCE */
   N_("Safest thing to do is to save file and exit.\nPlease try to reproduce this error and\n\tsend bug report to %s."),
   /* STID_FATAL_UNEXPECTED_ERROR */
   N_("Fatal Unexpected Error:  %s%s%s\n\nSafest thing to do is to save file and exit.\n\nPlease try to reproduce this error and send bug report to %s."),
   /* STID_ENTER_FNAME_TO_WRITE_MSG_BUF */
   N_("Please enter a file name to write the message buffer content:"),
   /* STID_STDOUT_STDERR_ESC_CANCEL */
   N_("( \"stdout\", \"stderr\", <ESC>: cancel )"),
   /* STID_MSG_BUF_SAVED_INTO_GIVEN */
   N_("Message buffer saved into '%s'."),
   /* STID_FILE_LINE_MISS_FIELD_IN_OBJ */
   N_("%s, %d:  Missing %s in %s object."),
   /* STID_FILE_LINE_BAD_FIELD_IN_OBJ */
   N_("%s, %d:  Bad %s in %s object."),
   /* STID_NAMED_SIG_RECV */
   N_("%s signal received."),
   /* STID_ERR_COUNT_EXCEED_TOOL_ABORTED */
   N_("Error count exceeds %1d in %s.  %s aborted."),
   /* STID_WORKING_FILE_SAVED_TO */
   N_("Working file saved to '%s.%s'."),
   /* STID_UNABLE_TO_SAVE_WORKING_FILE */
   N_("Unable to save working file."),
   /* STID_X_ERROR_MSG */
   N_("X Error:  %s."),
   /* STID_BAD_COMPILE_OPT_USE_ALT_STR */
   N_("Invalid compiler option %s=\"%s\", \"%s\" is used instead."),
   /* STID_MISSING_DISPLAY_NAME */
   N_("Missing display name."),
   /* STID_MISSING_PAGE_NUMBER */
   N_("Missing page number."),
   /* STID_INVALID_GIVEN_PAGE_NUMBER_STR */
   N_("Invalid page number: '%s'."),
   /* STID_MISSING_ARG_FOR_CMDLINE_OPT */
   N_("Missing command line argument for '%s'."),
   /* STID_INVALID_ARG_FOR_CMDLINE_OPT */
   N_("Invalid command line argument for '%s': '%s'."),
   /* STID_TGIF_PATH_SHOULD_NOT_BE_USED */
   N_("Warning: -tgif_path should not be used!"),
   /* STID_PRINT_TO_PRINTER_INCOMPAT */
   N_("Printing to printer is incompatible with '%s'."),
   /* STID_BAD_EXEC_FNAME_DASH */
   N_("Invalid exec file name (cannot start with '-')."),
   /* STID_CANNOT_OPEN_DIR_FOR_READING */
   N_("Cannot open the '%s' directory for reading."),
   /* STID_FAIL_TO_STAT_GIVEN_FILE */
   N_("Fail to stat() '%s'."),
   /* STID_SET_WORKING_DIR */
   N_("SetWorkingDir"),
   /* STID_CLICK_SETWORKDIR_TO_SET_DIR */
   N_("Click on the 'SetWorkingDir' button to set Open/SaveAs directory."),
   /* STID_SETWORKDIR_NOT_SUP_FOR_STR */
   N_("SetWorkingDir not supported for '%s'."),
   /* STID_OPENSAVE_DIR_CHANGE_TO_REMAIN */
   N_("Current open/save directory change to '%s'.\n\n(Current import directory remains '%s'.)"),
   /* STID_IMPORT_DIR_CHANGED_TO_REMAIN */
   N_("Current import directory changed to '%s'.\n\n(Current open/save directory remains '%s'.)"),
   /* STID_GEN_LIST_OF_DOMAIN_NAMES_WAIT */
   N_("Generating list of domain names, please wait..."),
   /* STID_CANT_GEN_DOM_NAMES_INI_ENTRY */
   N_("Fail to generate a list of domain names.\n\nCannot find the '%s' entry in the [%s] section of '%s'."),
   /* STID_CANT_GEN_DOM_NAMES_XDEF */
   N_("Fail to generate a list of domain names.\n\nCannot find %s.%s%1d."),
   /* STID_CANT_GET_LIST_OF_DOM_NAMES */
   N_("Fail to get a list of domain names."),
   /* STID_PLEASE_SELECT_A_NEW_DOMAIN */
   N_("Please select a new DOMAIN..."),
   /* STID_CURRENT_DOMAIN_IS */
   N_("Current domain is '%s'."),
   /* STID_SYMBOL_PATH_SET_TO */
   N_("Symbol path set to '%s'."),
   /* STID_PLS_SEL_A_DIR_IN_GIVEN_DOMAIN */
   N_("Please select a DIRECTORY in the '%s' domain..."),
   /* STID_GEN_LIST_XBM_NAMES_WAIT */
   N_("Generating list of bitmap names, please wait..."),
   /* STID_GEN_LIST_XPM_NAMES_WAIT */
   N_("Generating list of pixmap names, please wait..."),
   /* STID_GEN_LIST_OBJ_NAMES_WAIT */
   N_("Generating list of object file names, please wait..."),
   /* STID_GEN_LIST_SYM_NAMES_WAIT */
   N_("Generating list of symbol names, please wait..."),
   /* STID_GEN_LIST_TYPE_FILE_NAMES_WAIT */
   N_("Generating list of %s file names, please wait..."),
   /* STID_NO_GIVEN_TYPE_FILE_FOUND */
   N_("No '%s' file found."),
   /* STID_NO_DIR_SEP_IN_FUNC_WORK_DIR */
   N_("Error: No directory-separator found in %s.\n\nCurrent working directory set to '%s'."),
   /* STID_NO_DIR_SEP_IN_FUNC_SYM_DIR */
   N_("Error: No directory-separator found in %s.\n\nCurrent symbol directory set to '%s'."),
   /* STID_NO_DIR_SEP_IN_FUNC_IMPORT_DIR */
   N_("Error: No directory-separator found in %s.\n\nCurrent import directory set to '%s'."),
   /* STID_EMPTY_PATH_DISALLOW_FOR_DOM */
   N_("Empty path is not allowed for the '%s' domain."),
   /* STID_TOOL_EDIT_DOM_PATHS */
   N_("%s - Edit Domain Paths"),
   /* STID_EDIT_DOMAIN_PATHS_DOTS */
   N_("Edit domain paths..."),
   /* STID_TOOL_SEL_DEFAULT_DOMAIN */
   N_("%s - Select A Default Domain"),
   /* STID_SEL_DEFAULT_DOMAIN_DOTS */
   N_("Please select a default domain..."),
   /* STID_PLS_SPEC_A_NEW_DOMAIN_NAME */
   N_("Please specify a new domain name:"),
   /* STID_GIVEN_DOMAIN_ALREADY_EXISTS */
   N_("Domain '%s' already exists!"),
   /* STID_GIVEN_DOM_ADDED_PATH_SET_TO */
   N_("Domain '%s' added.  Path set to \"%s\"."),
   /* STID_SEL_DOMAIN_TO_DELETE_DOTS */
   N_("Please select a domain to delete..."),
   /* STID_OK_TO_DELETE_NAMED_DOMAIN_YNC */
   N_("Okay to delete the '%s' domain? [ync](y)"),
   /* STID_OK_TO_RELOAD_DOM_INFO_FROM_X */
   N_("Okay to reload domain information from X defaults? [ync](y)"),
   /* STID_ERR_IN_PARSE_STR_FOR_2_VAL */
   N_("Error in parsing '%s' for 2 values."),
   /* STID_FILE_MOD_SAVE_BEFORE_BACK */
   N_("File modified, save file before going back? [ync](y)"),
   /* STID_NOTHING_TO_GO_BACK_TO */
   N_("Nothing to go back to."),
   /* STID_FILE_MOD_SAVE_BEFORE_FORWARD */
   N_("File modified, save file before going forward? [ync](y)"),
   /* STID_NOTHING_TO_GO_FORWARD_TO */
   N_("Nothing to go forward to."),
   /* STID_FAIL_TO_FIND_RECORD_IN_FUNC */
   N_("Fail to find record in AdjustNavigate()."),
   /* STID_FURTHER_NAVIGATE_MAY_CRASH */
   N_("Further navigating may crash."),
   /* STID_CANT_REFRESH_UNDEFINED_FILE */
   N_("Canot refresh an undefined file."),
   /* STID_FILE_MOD_SAVE_BEFORE_RELOAD */
   N_("File modified, save file before going reloading? [ync](y)"),
   /* STID_HOT_LIST_FILE_UNDEF_PLS_SPEC */
   N_("Hot list file undefined.  Please specify it in %s.%s."),
   /* STID_MALFORMED_NAMED_HOT_LIST_FILE */
   N_("Malformed hot list file '%s'."),
   /* STID_HOT_LIST_FILE_IS_EMPTY */
   N_("Hot list file is empty."),
   /* STID_Q_DEL_NAMED_FROM_HOT_LIST */
   N_("Would you like to delete '%s' from the hot list? [ync](y)"),
   /* STID_FILE_MOD_SAVE_BEFORE_OPEN_ANO */
   N_("File modified, save file before opening another file? [ync](y)"),
   /* STID_SESSION_HIST_NOT_ESTABLISHED */
   N_("Sesion history has not been established yet."),
   /* STID_TOOL_HOT_LIST */
   N_("%s - Hot List"),
   /* STID_TOOL_HISTORY */
   N_("%s - History"),
   /* STID_HOT_LIST_DOTS */
   N_("Hot list..."),
   /* STID_CANT_ADD_UNDEF_TO_HOT_LIST */
   N_("Cannot add an undefined file to hot list."),
   /* STID_HOT_LIST_FILE_UPDATED */
   N_("Hot list file updated."),
   /* STID_SESSION_HISTORY_DOTS */
   N_("Session history..."),
   /* STID_ENTER_HYPERSPACE_DOTS */
   N_("Enter hyperspace..."),
   /* STID_LEAVING_HYPERSPACE_DOTS */
   N_("Leaving hyperspace..."),
   /* STID_CANT_SLIDESHOW_IN_TILED_PAGE */
   N_("Cannot enter slideshow mode in TILED page mode.  Would you like to switch to STACKED page mode (and mark the file unsavable) then proceed? [ync](y)"),
   /* STID_FILE_MOD_SAVE_BEFORE_SLIDE */
   N_("File modified, save file before entering slideshow mode? [ync](y)"),
   /* STID_PAGE_NUM_MOD_IN_SLIDE_NOT_SAV - this string is not used! */
   N_("Page numbers will be modified by %s in slideshow mode.  It's probably a good idea NOT to do a Save() until you've re-opened the file."),
   /* STID_Q_PAGE_NUM_MOD_IN_SLIDE_YNC */
   N_("Page numbers will be modified by %s in slideshow mode.  The file will be marked unsavable if you continue.\n\nOkay to mark the file unsavable and enter slideshow mode? [ync](y)"),
   /* STID_ENTERING_SLIDESHOW_DOTS */
   N_("Entering slideshow..."),
   /* STID_LEAVING_SLIDESHOW_DOTS */
   N_("Leaving slideshow..."),
   /* STID_ENTER_A_COLOR_FOR_SLIDE_BRDR */
   N_("Please enter a color for the slideshow border:"),
   /* STID_PRESS_ENTER_FOR_DEF_COLOR */
   N_("( just press <ENTER> for the default color )"),
   /* STID_SLIDE_WILL_USE_DEF_BRDR_COLOR */
   N_("Slideshow will use default border color."),
   /* STID_SLIDE_BRDR_COLOR_SET_TO_NAMED */
   N_("Slideshow border color set to '%s'."),
   /* STID_ENTER_XY_OFFSET_FOR_SLIDE_WIN */
   N_("Please enter X and Y offsets for the slideshow window: [current: %1d,%1d]"),
   /* STID_SLIDE_WIN_XY_OFFSETS_SET_TO */
   N_("Slideshow window offsets set to: %1d, %1d."),
   /* STID_GRID_BE_VISIBLE_IN_SLIDE */
   N_("Grid will be visible in slideshow mode."),
   /* STID_GRID_NOT_BE_VISIBLE_IN_SLIDE */
   N_("Grid will not be visible in slideshow mode."),
   /* STID_CANT_DO_NEXTPAGE_IN_TILED */
   N_("Cannot do NextPage() in TILED page mode."),
   NULL
};

static char *gaszEnglishStrTable900[] = {
   /* STID_ALREADY_AT_LAST_PAGE */
   N_("Already at last page."),
   /* STID_CANT_DO_PREFPAGE_IN_TILED */
   N_("Cannot do PrevPage() in TILED page mode."),
   /* STID_ALREADY_AT_FIRST_PAGE */
   N_("Already at first page."),
   /* STID_TOOL_NAME_PAGES */
   N_("%s - Name Pages"),
   /* STID_TOOL_GOTO_PAGE */
   N_("%s - Goto Page"),
   /* STID_CANT_DO_GOTOPAGE_IN_TILED */
   N_("Cannot do GotoPage() in TILED page mode."),
   /* STID_GOTO_PAGE_DOTS */
   N_("Goto page..."),
   /* STID_NO_PAGE_SELECTED */
   N_("No page selected."),
   /* STID_INVALID_GIVEN_PAGE_NUMBER */
   N_("Invalid page number '%1d'."),
   /* STID_ALREADY_AT_GIVEN_PAGE */
   N_("Already at page %1d."),
   /* STID_CUR_PAGE_IS_GIVEN_PAGE_NUM */
   N_("Current page is page %1d."),
   /* STID_CANT_DO_NAMEPAGES_IN_TILED */
   N_("Cannot do NamePages() in TILED page mode."),
   /* STID_EDIT_PAGE_NAMES_DOTS */
   N_("Edige page names..."),
   /* STID_CANT_EDITPAGEFNAME_IN_TILED */
   N_("Cannot do EditPageFileNames() in TILED page mode."),
   /* STID_EDIT_PAGE_FILE_NAMES_DOTS */
   N_("Edige page file names..."),
   /* STID_PAGE_FNAME_NO_SPACE_TRUNC */
   N_("Page file name: '%s' must not contain space characters.  It is truncated to '%s'."),
   /* STID_OK_TO_FLUSH_UNDO_AND_PROCEED */
   N_("Okay to flush the undo buffer and proceed? [ync](y)"),
   /* STID_ADD_PAGE_BEFORE_GIVEN_FLUSH */
   N_("Adding a page before page %1d will cause the undo buffer to be flushed."),
   /* STID_ADD_PAGE_AFTER_GIVEN_FLUSH */
   N_("Adding a page after page %1d will cause the undo buffer to be flushed."),
   /* STID_DELETE_GIVEN_PAGE_CAUSE_FLUSH */
   N_("Deleting page %1d will cause the undo buffer to be flushed."),
   /* STID_DELETE_A_PAGE_CAUSE_FLUSH */
   N_("Deleting a page will cause the undo buffer to be flushed."),
   /* STID_DELETE_PAGES_PAGE_CAUSE_FLUSH */
   N_("Deleting pages will cause the undo buffer to be flushed."),
   /* STID_CHNG_PAGE_LAYOUT_FLUSH */
   N_("Changing the page layout mode will cause the undo buffer to be flushed."),
   /* STID_SET_PAPER_COLOR_CAUSE_FLUSH */
   N_("Setting paper color will cause the undo buffer to be flushed."),
   /* STID_CANT_DO_ADDPAGE_IN_TILED */
   N_("Cannot do AddPage() in TILED page mode."),
   /* STID_CANT_DO_DELETEPAGE_IN_TILED */
   N_("Cannot do DeleteCurrentPage() in TILED page mode."),
   /* STID_CANT_DEL_ONLY_PAGE_IN_TILED */
   N_("Cannot delete (the only) page in TILED page mode."),
   /* STID_CANT_DO_SPECDRAWSIZE_IN_TILED */
   N_("Cannot do SpecifyDrawingSize() in TILED page mode."),
   /* STID_CANT_DO_DELETEPAGES_IN_TILED */
   N_("Cannot do DeletePages() in TILED page mode."),
   /* STID_CANT_DO_POFPP_IN_TILED */
   N_("Cannot do PrintOneFilePerPage() in TILED page mode."),
   /* STID_CANT_DO_POFPP_TO_PRINTER */
   N_("Cannot do PrintOneFilePerPage() to printer."),
   /* STID_NAMED_TEMPLATE_LOADED */
   N_("Template loaded: '%s'."),
   /* STID_PROB_LOADING_NAMED_TEMPLATE */
   N_("Problem loading template file '%s'."),
   /* STID_GIVEN_PAGE_NUM_ADDED */
   N_("Page %1d added."),
   /* STID_ADDPAGEBEFORE_IN_WB */
   N_("AddPageBefore() is disabled in WhiteBoard mode."),
   /* STID_ADDPAGEAFTER_IN_WB */
   N_("AddPageAfter() is disabled in WhiteBoard mode."),
   /* STID_GIVEN_PAGE_NUM_DELETED */
   N_("Page %1d deleted."),
   /* STID_CANT_TPLS_IN_STACKED */
   N_("Cannot TogglePageLineShown() in STACKED page mode."),
   /* STID_ENTER_DRAW_SIZE_SPEC_WH */
   N_("Please enter drawing size specification: [W x H]"),
   /* STID_BAD_DRAW_SIZE_SPEC_WH */
   N_("Invalid drawing size specification.\n\nMust specify both Width and Height."),
   /* STID_BAD_DRAW_SIZE_SPEC_W_TIMES_H */
   N_("Invalid drawing size specification:  W times H should be >= %1d."),
   /* STID_INVALID_DRAW_SIZE_SPEC */
   N_("Invalid drawing size specified."),
   /* STID_CANT_PRINTONEPAGE_IN_FORMAT */
   N_("Cannot do PrintOnePage() in the '%s' format."),
   /* STID_BTN1_SEL_PAGE_TO_PRINT */
   N_("Left button selects a page to print,"),
   /* STID_OTHER_BTN_CANCEL_PRINTONEPAGE */
   N_("other buttons cancel printing one page."),
   /* STID_SELECT_A_PAGE_TO_PRINT */
   N_("Select a page to print"),
   /* STID_OPERATION_CANCEL_BY_USER */
   N_("Operation canceled by the user."),
   /* STID_MUST_SEL_A_PT_ON_THE_PAPER */
   N_("Must select a point on the paper."),
   /* STID_MUST_SEL_A_PT_IN_TOOL_WIN */
   N_("Must select a point within the %s window."),
   /* STID_NEW_PAPER_SIZE_IS_GIVEN */
   N_("New paper size: %.2fin x %.2fin (%.2fcm x %.2fcm)."),
   /* STID_INVALID_GIVEN_PAPER_SIZE_SPEC */
   N_("Invalid paper size specification: '%s'."),
   /* STID_CUR_PAPER_SIZE_IS_GIVEN */
   N_("( current paper size: '%s' )"),
   /* STID_ENTER_PAPER_SIZE_WH */
   N_("Please enter physical paper size: [W x H] (e.g., 8.5in x 11in)"),
   /* STID_CANT_DEL_ONLY_PAGE_IN_STACKED */
   N_("Cannot delete (the only) page."),
   /* STID_SPECIFY_PAGES_TO_DEL_EX */
   N_("Please specify pages to delete: (e.g., 2,3,7-9,10)"),
   /* STID_GIVEN_PAGE_NUM_IS_OUT_OF_RNG */
   N_("Page %1d is out of range."),
   /* STID_MALFORMED_SPECIFICATION_STR */
   N_("Malformed specification: '%s'."),
   /* STID_CANT_DEL_ALL_PAGES */
   N_("Cannot delete all pages."),
   /* STID_ONE_PAGE_DELETED */
   N_("1 page deleted."),
   /* STID_MANY_PAGES_DELETED */
   N_("%1d pages deleted."),
   /* STID_OBJ_OUTSIDE_PAGE_BNRDY_SWITCH */
   N_("There are objects outside the page boundary.\n\nCannot switch to TILED mode."),
   /* STID_SWTCH_TO_TILED_LOSE_ALL_NAMES */
   N_("Switching from TILED to STACKED mode loses all page names and page file names.\n\nOk to proceed? [ync](y)"),
   /* STID_CHANGE_PAGE_LAYOUT_MODE_IN_WB */
   N_("Changing page layout mode is disabled in WhiteBoard mode."),
   /* STID_PAGE_LAYOUT_MODE_IS_STACKED */
   N_("Page layout mode is STACKED."),
   /* STID_PAGE_LAYOUT_MODE_IS_TILED */
   N_("Page layout mode is TILED."),
   /* STID_SPECIFY_A_PAPER_COLOR */
   N_("Please specify a paper color:"),
   /* STID_SPECIFY_A_PAPER_COLOR_CUR_IS */
   N_("Please specify a paper color: [current: %s]"),
   /* STID_ENTER_NONE_TO_REMOVE_COLOR */
   N_("( Enter 'None' to remove the current paper color. )"),
   /* STID_PAPER_COLOR_REMOVED */
   N_("Paper color removed."),
   /* STID_PAPER_COLOR_SET_TO_GIVEN */
   N_("Paper color set to '%s'."),
   /* STID_GRAY_SCALE_USED_IN_PRINT_PAT */
   N_("Gray scale used in printing tiling patterns."),
   /* STID_NOTE_SLOW_PRINT_DUE_USE_PAT */
   N_("Note: slow printing due to the use of patterns."),
   /* STID_SUGGEST_USEGRAYSCALE_TO_SPEED */
   N_("You may want to try UseGrayScale() to speed things up."),
   /* STID_FILL_PEN_PAT_OPAQUE */
   N_("Fill and pen patterns are opaque."),
   /* STID_FILL_PEN_PAT_TRANSPARENT */
   N_("Fill and pen patterns are transparent."),
   /* STID_LINE_WIDTH_SET_TO_GIVEN_STR */
   N_("Line width set to %s."),
   /* STID_ENTER_LINE_WIDTH_OPT_AW_AH */
   N_("Please enter line width (arrow width and height are optional):"),
   /* STID_INVALID_GIVEN_STR_WIDTH */
   N_("Invalid width: '%s'."),
   /* STID_INVALID_GIVEN_STR_AW_AH */
   N_("Invalid arrow width and height: '%s %s'."),
   /* STID_NO_PINS_FOUND_IN_CUR_DRAWING */
   N_("No pins found in the current drawing."),
   /* STID_INPUT_PAIRS_OF_POINTS_POLY */
   N_("Please input pairs of points: (terminate with \"<Cntrl>d<CR>\" or \".<CR>\", continue to next poly with \";<CR>\")"),
   /* STID_READ_INT_ERROR_FOR_POLY_PTS */
   N_("Error reading integer for poly points."),
   /* STID_TOO_FEW_POINTERS_ENTERED */
   N_("Too few points entered."),
   /* STID_TOO_FEW_VER_TO_CLOSE_POLY */
   N_("Too few vertices to close a polyline/open-spline."),
   /* STID_HUH_WHERE_ARE_THE_VERTICES */
   N_("Huh?  Where are the vertices?"),
   /* STID_HUH_TOPVSEL_NE_BOTVSEL */
   N_("Huh?  How can topVSel != botVSel?"),
   /* STID_CMD_ONLY_AVAIL_IN_VERSEL_MODE */
   N_("This command is only available in vertex or select mode."),
   /* STID_SEL_2_ENDPOINT_VER_TO_JOIN */
   N_("Please select 2 end-point vertices to join."),
   /* STID_SEL_VER_FROM_POLY_OPEN_ONLY */
   N_("Please select vertices from polylines/open-splines only."),
   /* STID_SEL_2_POLYLINES_OPEN_SPLINES */
   N_("Please select 2 polylines/open-splines."),
   /* STID_CANT_JOIN_INTSPLINE_W_NONEINT */
   N_("Cannot join a interpolated spline with a non-interpolated polyline/open-spline."),
   /* STID_SEL_1_VERTEX_TO_CUT */
   N_("Please select 1 vertex to cut."),
   /* STID_SEL_A_VERTEX_FROM_POLY_OR_GON */
   N_("Please select a vertex from a polyline or polygon object."),
   /* STID_INVALID_SMOOTHHINGE_SPEC_POLY */
   N_("%s, %d:  Invalid smooth/hinge spec for a poly/polygon object.  Read aborted"),
   /* STID_INVALID_NUM_PTS_IN_POLY */
   N_("%s, %d:  Invalid number of points in a poly object.  Read aborted"),
   /* STID_INPUT_PAIRS_OF_POINTS_POLYGON */
   N_("Please input pairs of points: (terminate with \"<Cntrl>d<CR>\" or \".<CR>\", continue to next polygon with \";<CR>\")"),
   /* STID_READ_INT_ERR_FOR_POLYGON_PTS */
   N_("Error reading integer for polygon points."),
   /* STID_INVALID_NUM_PTS_IN_POLYGON */
   N_("%s, %d:  Invalid number of points in a polygon.  Read aborted"),
   /* STID_CANT_GET_XDEF_DEF_WIDTHS_USED */
   N_("Error in getting X default %s.%s.  Default widths used."),
   /* STID_ERR_SETCTM_CALLED_CTM_NONNULL */
   N_("Error: SetCTM() is called with ObjPtr->ctm != NULL."),
   NULL
};

static char *gaszEnglishStrTable1000[] = {
   /* STID_MEASUREMENT_SHOWN_IN_UNIT */
   N_("Measurement will be shown in '%s' (%s %s)."),
   /* STID_BAD_MEASUREMENT_UNIT_SPEC */
   N_("Bad measurement unit specification '%s'."),
   /* STID_SHOW_CROSSHAIR_ENABLED */
   N_("Showing cross-hair enabled."),
   /* STID_SHOW_CROSSHAIR_DISABLED */
   N_("Showing cross-hair disabled."),
   /* STID_SHOW_MEASUREMENT_ENABLED */
   N_("Showing measurement enabled."),
   /* STID_SHOW_MEASUREMENT_DISABLED */
   N_("Showing measurement disabled."),
   /* STID_CANNOT_EXECUTE_GIVEN_CMD */
   N_("Cannot execute '%s'."),
   /* STID_CONNECTION_INTERRUPTED */
   N_("%s: connection interrupted."),
   /* STID_CANT_CONN_TO_HOST_PORT */
   N_("%s: cannot connect to %s%s%s."),
   /* STID_FAIL_TO_GET_DATA_FROM_URL */
   N_("%s: fail to get data from '%s'."),
   /* STID_NETWORK_ERR_TALK_TO_HOST_PORT */
   N_("%s: network error occurred when talking to %s%s%s."),
   /* STID_FAILED_TO_LOGIN_TO_HOST_PORT */
   N_("%s: fail to login to %s%s%s."),
   /* STID_CANT_FIND_LOCATION_IN_HDR */
   N_("%s: Cannot find forwarding 'Location' information in message header while getting data from '%s'."),
   /* STID_LNK_FWD_5_TIMES_LOAD_ABORT */
   N_("%s: Links forwarded more than 5 times while getting data from '%s'.\n\nLoading is aborted."),
   /* STID_CANT_FIND_WWW_AUTHEN_IN_HDR */
   N_("%s: Cannot find 'WWW-Authenticate' information in message header while getting data from '%s'."),
   /* STID_AUTHEN_REQ_FOR_REALM */
   N_("%s: Authentication information for %s required..."),
   /* STID_ENTER_USERNAME_FOR_REALM */
   N_("Please enter UserName for %s:"),
   /* STID_ENTER_PASSWORD_FOR_REALM */
   N_("Please enter Password for %s:"),
   /* STID_FAIL_TO_PARSE_GIVEN_URL */
   N_("Fail to parse URL: '%s'."),
   /* STID_TOOL_NOT_KNOW_HOW_TALK_PROTO */
   N_("%s does not know how to talk using the '%s' protocol."),
   /* STID_LAUNCH_GIVEN_VIEWER */
   N_("Launch viewer: %s"),
   /* STID_CANT_OPEN_GIVEN_MAILCAP_FILE */
   N_("Cannot open mailcap file '%s'."),
   /* STID_MALFORMED_MAILCAP_ENTRY */
   N_("Malformed mailcap entry:\n\n%s/%s; %s"),
   /* STID_MALFORMED_MAILCAP_ENTRY_PARAM */
   N_("Malformed mailcap entry:\n\n%s/%s; %s; %s"),
   /* STID_TOOL_CANT_HANDLE_MP_MIME */
   N_("%s does not know how to handle Multipart MIME types."),
   /* STID_CANT_FIND_RT_BRACK_MC */
   N_("Cannot find matching '}' for mailcap entry:\n\n%s/%s; %s"),
   /* STID_CANT_FIND_RT_BRACK_MC_PARAM */
   N_("Cannot find matching '}' for mailcap entry:\n\n%s/%s; %s; %s"),
   /* STID_CANT_FIND_NAMED_INFO_IN_CT */
   N_("%s: cannot find the '%s' information in the message Content-Type."),
   /* STID_SAVE_GIVEN_STR_AS */
   N_("Save %s as:"),
   /* STID_NO_VIEWER_SAVE_GIVEN_STR_AS */
   N_("No viewer found.  Save %s as:"),
   /* STID_UNKNOWN_ERR_IN_COPYING_FILES */
   N_("Unknown error in copying files."),
   /* STID_CANT_GETHOSTNAME_USE_LOCALH */
   N_("Cannot gethostname(), 'localhost' is used instead."),
   /* STID_TURN_OFF_AUTO_PAN_SET_XDEF */
   N_("To turn off auto-panning, set %s.%s to false in X defaults."),
   /* STID_WILL_UPD_WIN_SMOOTH_SCROLL */
   N_("Will update canvas window smoothly while scrolling."),
   /* STID_WILL_UPD_WIN_JUMP_SCROLL */
   N_("Will jump update canvas window while scrolling."),
   /* STID_WILL_NOT_UPD_WIN_SCROLL */
   N_("Will not update canvas window while scrolling."),
   /* STID_INCONSIS_VERTEX_SEL_IN_FUNC */
   N_("Inconsistent vertex selection detected in %s."),
   /* STID_INVALID_NAMED_TELEPORT_DEST */
   N_("Invalid teleport destination '%s'."),
   /* STID_Q_FILE_NOT_EXIST_CREATE */
   N_("File '%s' does not exist.\n\nWould you like to create it?"),
   /* STID_CANT_FIND_PAGE_NAMED_TO_TEL */
   N_("Cannot find page named '%s' to teleport to."),
   /* STID_CANT_FIND_PAGE_NUM_TO_TEL */
   N_("Cannot find page %1d to teleport to."),
   /* STID_CANT_GROUP_IN_VERTEX_MODE */
   N_("Cannot group in vertex mode."),
   /* STID_CANT_GROUP_SINGLE_OBJECT */
   N_("Cannot group a single object."),
   /* STID_INVALID_XDEF_USE_ALT_OCTVALUE */
   N_("Invalid %s.%s: '%s', 0%03o is used instead."),
   /* STID_INVALID_XDEF_RNG_USE_ALT_STR */
   N_("Invalid %s.%s: '%s', (must be between %s and %s), %s is used instead."),
   /* STID_CANT_CREATE_NAMED_DIRECTORY */
   N_("Cannot create the '%s' directory."),
   /* STID_OPR_USING_NAMED_DIR_WILL_FAIL */
   N_("Operations using the '%s' directory (such as CopyProperties() and PasteProperties()) will fail."),
   /* STID_CREATE_A_SHAPE_FAILED */
   N_("Create a shape failed."),
   /* STID_CREATE_SHADOW_SHAPE_NO_FILL */
   N_("You are attempting to create a shadowed shape but the shape has no fill (the shadow will show through).\n\nWould you like to create such a shape without a shadow?"),
   /* STID_CREATE_SHADOW_SHAPE_TRAN_FILL */
   N_("You are attempting to create a shadowed shape but the shape has transparent fill (the shadow will show through).\n\nWould you like to create such a shape without a shadow?"),
   /* STID_ENTER_XY_OFFSET_FOR_SHP_SHDW */
   N_("Please enter X and Y offsets for the shape shadow: [current: %1d,%1d]"),
   /* STID_SHAPE_SHDW_XY_OFFSETS_SET_TO */
   N_("Shape shadow offsets set to: %1d, %1d."),
   /* STID_INVALID_SHORTCUT_ENTRY_SKIP */
   N_("Invalid shortcut entry: '%s'.  It is skipped."),
   /* STID_DUP_SHORTCUT_ENTRY_FOR_CHAR */
   N_("Warning: duplicate shortcut entry for '%c'."),
   /* STID_INVALID_SHORTCUT_ENTRY */
   N_("Invalid shortcut entry: '%s'."),
   /* STID_WARN_DUP_CMDID */
   N_("Warning: duplicate cmdid %1d."),
   /* STID_PUSH_IN_WB */
   N_("Push() is disabled in WhiteBoard mode."),
   /* STID_FILE_VER_ABORT_READ_SYMBOL */
   N_("File version (=%1d) too large.\n\nRead symbol aborted.\n\nYou may need a more recent version of %s.  Please check <URL:%s>."),
   /* STID_CANT_FIND_NAMED_OBJ_IN_FUNC */
   N_("Cannot find the '%s' text object in %s."),
   /* STID_SEL_A_SYM_TO_INSTAN_IN_DOMAIN */
   N_("Please select a symbol to INSTANTIATE in the '%s' domain..."),
   /* STID_CANT_INSTAN_FILE_UNKNOWN_EXT */
   N_("Cannot instantiate because '%s' has unknow file extension."),
   /* STID_CANT_MAKESYMBOLIC_VERTEX_MODE */
   N_("Cannot MakeSymbolic() in vertex mode."),
   /* STID_SEL_ONLY_ONE_FOR_MAKESYMBOLIC */
   N_("Please select only one object for MakeSymbolic()."),
   /* STID_PIN_CANT_MADE_INTO_SYMBOL */
   N_("A pin object cannot be made into a symbol object."),
   /* STID_SEL_OBJ_IS_NOW_SYMBOLIC */
   N_("Selected object is now SYMBOLIC."),
   /* STID_ENTER_NAME_FOR_THE_ICON */
   N_("Please enter name for the icon:"),
   /* STID_NAME_NOT_SPEC_ICON_NOT_CREATE */
   N_("Name not specified, icon not created."),
   /* STID_CANT_SAVE_OBJ_ICON_NOT_CREATE */
   N_("Cannot save as an object file, icon not created."),
   /* STID_CANT_FIND_DOT_IN_ICON_NAME_MI */
   N_("Cannot find '.' in icon_name in MakeIconic()."),
   /* STID_CANT_SAVE_PIN_ICON_NOT_CREATE */
   N_("Cannot save as a pin file, icon not created."),
   /* STID_CANT_OPEN_FOR_WRITE_ICON_CRET */
   N_("Cannot open '%s' for writing, icon not created."),
   /* STID_SEL_OBJ_IS_NOW_ICONIC */
   N_("Selected object is now ICONIC."),
   /* STID_SEL_ONLY_ONE_FOR_MAKEICONIC */
   N_("Please select one object to make it ICONIC."),
   /* STID_SEL_OBJS_ARE_NOW_GROUP */
   N_("Selected ICONIC objects are GROUP objects now."),
   /* STID_GIVEN_LINE_SKIP_NOT_AN_ATTR */
   N_("Line %1d in '%s' is skipped because it does not specify an attribute."),
   /* STID_GIVEN_LINE_SKIP_ILLEGAL_CHAR */
   N_("Line %1d in '%s' is skipped because attribute name contains illegal characters such as '!' or '.'."),
   /* STID_SEL_ONE_OBJ_FOR_IMPORTATTRS */
   N_("Please select only one object for ImportAttrs()."),
   /* STID_SEL_TEXT_FILE_OF_ATTR_TO_IMP */
   N_("Please select a text file containing attributes to import..."),
   /* STID_CANT_IMPORT_REMOTE_TEXT_FILE */
   N_("Importing remote text file not supported."),
   /* STID_SEL_ONE_OBJ_FOR_EXPORTATTRS */
   N_("Please select only one object for ExportAttrs()."),
   /* STID_SEL_OBJ_HAS_NO_ATTR_TO_EXPORT */
   N_("Select object has no attributes to export."),
   /* STID_ENTER_TXT_FILE_TO_EXPORT_CRES */
   N_("Please enter a text file name to export to: ( <CR>: accept, <ESC>: cancel )"),
   /* STID_ATTR_EXPORTED_TO_NAMED_FILE */
   N_("Attributes exported to '%s'."),
   /* STID_CANT_FIND_ATTR_FOR_MERGEWTBL */
   N_("Cannot find the '%s' attribute for MergeWithTable()."),
   /* STID_MALFORMED_ATTR_FOR_MERGEWTBL */
   N_("Malformed '%s' attribute for MergeWithTable()."),
   /* STID_SEL_ONE_OBJ_FOR_MERGEWTBL */
   N_("Please select only one object for MergeWithTable()."),
   /* STID_FILE_MOD_SAVE_BEFORE_MERGE */
   N_("File modified, save file before merge? [ync](y)"),
   /* STID_Q_MERGEWTBL_CANT_UNDO_PROCEED */
   N_("MergeWithTable cannot be undone.\n\nOk to proceed? [ync](y)"),
   /* STID_SEL_TEXT_FILE_FOR_MERGEWTBL */
   N_("Please select a text file for MergeWithTable()."),
   /* STID_CANT_MERGE_W_REMOTE_TEXT_FILE */
   N_("Merging remote text file not supported."),
   /* STID_FIND_COL_NAMES_IN_FILE_ABORT */
   N_("Cannot find column names in '%s'.\n\nMerge aborted."),
   /* STID_MALFORMED_COL_NAMES_ABORT_MRG */
   N_("Malformed column names in '%s'.\n\nMerge aborted."),
   /* STID_LF_BTN_PLACE_MRG_OTHER_CANCEL */
   N_("Left button places a merged object, other buttons cancel merging."),
   /* STID_MALFORMED_TBL_LINE_ABORT_MRG */
   N_("Malformed table at line %d in '%s'.\n\nMerge aborted."),
   /* STID_CANT_FND_NAMED_ATTR_ABORT_MRG */
   N_("Cannot find attribute named '%s'.\n\nMerge aborted."),
   /* STID_NUM_OBJECTS_GENERATED */
   N_("%1d object(s) generated."),
   /* STID_CANT_FND_TABLE_ATTRS_FILE_ATT */
   N_("Cannot find the \"!.TABLE_ATTRS\" file attribute.\n\nCannot proceed with ExportToTable()."),
   /* STID_NUM_OBJECTS_EXPORTED */
   N_("%1d object(s) exported."),
   /* STID_ATTRS_EXPORTED_TO_TBL_FILE */
   N_("Attributes exported to table file '%s'."),
   /* STID_SEL_ONE_ICON_OBJ_TO_PUSH */
   N_("Please select one ICON object to push into."),
   NULL
};

static char *gaszEnglishStrTable1100[] = {
   /* STID_CANT_FND_SYM_FILE_IN_PATH */
   N_("Cannot find '%s.%s' in %s."),
   /* STID_CANNOT_OPEN_FOR_READ_PUSH */
   N_("Cannot open '%s' for reading.  Icon not pushed into."),
   /* STID_FILE_VER_ABORT_PUSH */
   N_("File version (=%1d) too large.\n\nPush aborted.\n\nYou may need a more recent version of %s.  Please check <URL:%s>."),
   /* STID_POP_BACK_TO_PARENT_UNDEF */
   N_("Poping back to parent level.  Current file undefined."),
   /* STID_ALREADY_AT_TOP_LEVEL */
   N_("Already at top level."),
   /* STID_FILE_MOD_SAVE_BEFORE_POP */
   N_("File modified, save file before poping back to the parent level? [ync](y)"),
   /* STID_CANT_FND_SYM_IN_NAMED_TO_INST */
   N_("Cannot find a symbol object in '%s' to instantiate."),
   /* STID_CANT_FIND_XDEF */
   N_("Cannot find %s.%s."),
   /* STID_ENTER_USERAGENT_FOR_HTTP */
   N_("Please specify 'UserAgent' to be used when making an HTTP request."),
   /* STID_ENTER_USERAGENT_FOR_HTTP_CUR */
   N_("Please specify 'UserAgent' to be used when making an HTTP request (current value is '%s')."),
   /* STID_PRESS_ENTER_FOR_DEF_USERAGENT */
   N_("( press <ENTER> for the default 'UserAgent' )"),
   /* STID_WILL_USE_DEF_USERAGENT_HTTP */
   N_("Will use the default 'UserAgent' when making an HTTP request."),
   /* STID_WILL_USE_NAMED_USERAGENT_HTTP */
   N_("Will use '%s' as 'UserAgent' when making an HTTP request."),
   /* STID_ENTER_REFERRER_FOR_HTTP */
   N_("Please specify 'Referrer' to be used when making an HTTP request."),
   /* STID_ENTER_REFERRER_FOR_HTTP_CUR */
   N_("Please specify 'Referrer' to be used when making an HTTP request (current value is '%s')."),
   /* STID_PRESS_ENTER_FOR_NO_REFERRER */
   N_("( press <ENTER> for no 'Referrer' )"),
   /* STID_WILL_NOT_USE_REFERRER_HTTP */
   N_("Will not use 'Referrer' when making an HTTP request."),
   /* STID_WILL_USE_NAMED_REFERRER_HTTP */
   N_("Will use '%s' as 'Referrer' when making an HTTP request."),
   /* STID_WILL_USE_KEEP_ALIVE_HTTP */
   N_("Will use Keep-Alive HTTP connections."),
   /* STID_NOT_WILL_USE_KEEP_ALIVE_HTTP */
   N_("Will not use Keep-Alive HTTP connections."),
   /* STID_CANT_LOCATE_NAMED_SERVER_TRY */
   N_("%s: unable to locate the server '%s'.\n\nPlease check the server name and try again."),
   /* STID_FAIL_TO_MAKE_SOCKET_BLOCK */
   N_("Fail to make socket blocking."),
   /* STID_FAIL_TO_MAKE_SOCKET_NON_BLOCK */
   N_("Fail to make socket non-blocking."),
   /* STID_BROKEN_PIPE_CONTACT_HOST */
   N_("Broken pipe while contacting '%s'."),
   /* STID_WRITE_TO_SOCKET_FAILED */
   N_("Writing to socket failed."),
   /* STID_READ_FROM_SOCKET_FAILED */
   N_("Reading from socket failed."),
   /* STID_UNEXP_NETWORK_ERR_WRITE_SOCK */
   N_("Unexpected network error while writing to socket."),
   /* STID_UNEXP_NETWORK_ERR_READ_SOCK */
   N_("Unexpected network error while reading from socket."),
   /* STID_LOCKED_OBJS_CANT_BE_STRETCHED */
   N_("Locked object(s) cannot be stretched."),
   /* STID_CANT_STRETCH_OBJ_HAS_0_WIDTH */
   N_("Cannot stretch.  Object has ZERO width."),
   /* STID_CANT_STRETCH_OBJ_HAS_0_HEIGHT */
   N_("Cannot stretch.  Object has ZERO height."),
   /* STID_LOCKED_OBJS_CANT_BE_SCALED */
   N_("Locked object(s) cannot be scaled."),
   /* STID_ENTER_SCALING_FACTORS_XY */
   N_("Please enter scaling factors: [X:Y] or [scale]"),
   /* STID_ENTER_A_SCALING_FACTOR */
   N_("Please enter a scaling factor:"),
   /* STID_ONLY_INPUT_ONE_NUMERIC_VAL */
   N_("Please only input only one numeric value."),
   /* STID_SCALING_EVERYTHING_LARGE_SURE */
   N_("Scaling everything by %g times.  Are you sure?"),
   /* STID_DISABLE_ON_RESIZE_ROTATE */
   N_("The '%s' object has an on_resize() script.\n\nTo rotate it will disable the on_resize() script.  Is it okay to rotate it?"),
   /* STID_DISABLE_ON_RESIZE_SHEAR */
   N_("The '%s' object has an on_resize() script.\n\nTo shear it will disable the on_resize() script.  Is it okay to shear it?"),
   /* STID_DISABLE_ON_RESIZE_TRANSFORM */
   N_("The '%s' object has an on_resize() script.\n\nTo transform it will disable the on_resize() script.  Is it okay to transform it?"),
   /* STID_LOCKED_OBJS_CANT_BE_FLIPPED */
   N_("Locked object(s) cannot be flipped."),
   /* STID_LOCKED_OBJS_ARE_NOT_FLIPPED */
   N_("Locked object(s) are not flipped."),
   /* STID_LOCKED_OBJS_ARE_NOT_STRETCHED */
   N_("Locked object(s) are not stretched."),
   /* STID_LOCKED_OBJS_ARE_NOT_ROTATED */
   N_("Locked object(s) are not rotated."),
   /* STID_LOCKED_OBJS_ARE_NOT_SHEARED */
   N_("Locked object(s) are not sheared."),
   /* STID_LOCKED_OBJS_ARE_NOT_XFORMED */
   N_("Transformation matrix not changed for locked object(s)."),
   /* STID_FLIPPED_HORIZONTALLY */
   N_("Flipped horizontally."),
   /* STID_FLIPPED_VERTICALLY */
   N_("Flipped vertically."),
   /* STID_LOCKED_OBJS_CANT_BE_ROTATED */
   N_("Locked object(s) cannot be rotated."),
   /* STID_ROTATED_CLOCKWISE */
   N_("Rotated clockwise."),
   /* STID_ROTATED_COUNTER_CLOCKWISE */
   N_("Rotated counter-clockwise."),
   /* STID_ENTER_TEXT_ROT_IN_DEGREE_CUR */
   N_("Please enter text rotation in degrees: (current value is %s)"),
   /* STID_CANT_PARSE_ENTER_ONE_NUM_VAL */
   N_("Cannot parse '%s'.\n\nPlease enter a numeric value."),
   /* STID_INVALID_VAL_ENTERED_RNG_INC */
   N_("Invalid value entered: '%s'.  Please enter a numeric value between %1d and %1d (inclusive)."),
   /* STID_TEXT_ROTATION_SET_TO_GIVEN */
   N_("Text rotation set to %s."),
   /* STID_ENTER_ROT_INC_IN_DEGREE_CUR */
   N_("Please enter text rotation increment in degrees: (current value is %s)"),
   /* STID_INVALID_VAL_ENTERED_RNG_EXC */
   N_("Invalid value entered: '%s'.  Please enter a numeric value > %1d and < %1d."),
   /* STID_TEXT_ROT_INC_SET_TO_GIVEN */
   N_("Text rotation increment set to %s."),
   /* STID_LOCKED_OBJS_CANT_BE_XFORMED */
   N_("Cannot change transformation matrix for locked object(s)."),
   /* STID_SEL_OBJ_TOO_SMALL_ROT_ANOTHER */
   N_("Selected object is too small to rotate.\n\nPlease select another object to rotate."),
   /* STID_SEL_OBJ_TOO_SMALL_SHEAR_ANO */
   N_("Selected object is too small to shear.\n\nPlease select another object to shear."),
   /* STID_CANNOT_CREATE_GC */
   N_("Cannot create GC."),
   /* STID_READ_ONLY_TEXT_CANT_BE_EDITED */
   N_("Read-only text cannot be edited."),
   /* STID_TEXT_SZ_NOT_AVAIL_TRY_DIFF */
   N_("Text size=%1d not available.\n\nPlease try a different size."),
   /* STID_CANT_INS_NON_DB_CH_AT_CUR_REM */
   N_("Cannot insert a non-double-byte character at the text cursor position.  The non-double-byte character is removed."),
   /* STID_TOO_MANY_BYTES_IN_CH_IGNORED */
   N_("Cannot handle so many bytes in a character.  Character is ignored."),
   /* STID_CANT_TAB_OUT_OF_NON_ATTR_TEXT */
   N_("Cannot TAB out of a non-attribute text."),
   /* STID_COMPOUND_TEXT_BUF_IS_EMPTY */
   N_("Compound-text buffer is empty."),
   /* STID_CUT_BUFFER_IS_EMPTY */
   N_("Cut buffer is empty."),
   /* STID_AN_ESC_KEY_PRESS_IGNORED */
   N_("An <ESC> key press is ignored."),
   /* STID_GIVEN_NUM_LINES_PASTED */
   N_("%1d line(s) pasted."),
   /* STID_COPY_FAIL_SEL_STR_MAY_TOO_LNG */
   N_("Copy to cut buffer failed.  Selected string may be too long."),
   /* STID_UNEXPECTED_TOK_IN_ABORT_READ */
   N_("%s, %d: Unexpected token '%c' in %s.  Read aborted."),
   /* STID_ILLEGAL_FIELD_IN_ABORT_READ */
   N_("%s, %d: Illegal field '%s' in %s.  Read aborted."),
   /* STID_INVALID_GIVEN_VALUE_SPECIFIED */
   N_("Invalid value '%s' specified for script fraction."),
   /* STID_ENTER_SCRIPT_FRACTION_CUR_IS */
   N_("Please enter a fraction for the super/subscript size (current fraction is %s):"),
   /* STID_SCRIPT_FRACTION_SET_TO_STR */
   N_("Script fraction set to '%s'."),
   /* STID_ENTER_INT_WIDTH_FOR_THIN_SPC */
   N_("Please enter an integer width value for the thin space (a negative value indicates a negative think space):"),
   /* STID_ENTER_INT_VAL_FOR_VERT_OFFSET */
   N_("Please enter an integer value for the vertical offset (a negative value raises the current line):"),
   /* STID_FAIL_INSERT_SCRIPT_FONT_SZ */
   N_("Failed to insert super/subscript because font size is not available."),
   /* STID_TOOL_DONT_KNOW_HOW_RUN_GUNZIP */
   N_("%s does not know how to run gunzip."),
   /* STID_SPECIFY_DISPLAY_ON_CMD_LINE */
   N_("Please specify '-display $DISPLAY' on the command line."),
   /* STID_FILE_VER_ABORT_TOOL */
   N_("File version (=%1d) too large.\n\n%s aborted.\n\nYou may need a more recent version of %s.  Please check <URL:%s>."),
   /* STID_FILE_SKIP_CONTAINS_ONE_PAGE */
   N_("%s is skipped because it only contains 1 page."),
   /* STID_FILE_SKIP_CONTAINS_NUM_PAGE */
   N_("%s is skipped because it only contains %1d pages."),
   /* STID_FILE_NOT_CONTAIN_COLOR_ABORT */
   N_("File does not contain enough color information (it is not saved by tgif-3.0-p10 or beyond).  %s aborted."),
   /* STID_SPEC_FILE_WHEN_STDOUT_USED */
   N_("File name must be specified when -stdout is used."),
   /* STID_TOOL_FILE_NAME_TO_PRINT */
   N_("%s File Name to Print"),
   /* STID_CANT_ONEFPPAGE_IN_TILED_PAGE */
   N_("Cannot use -one_file_per_page in TILED page mode."),
   /* STID_ONE_FILE_ONLY_WITH_STDOUT */
   N_("Only one file name can be specified with -stdout."),
   /* STID_ONE_FILE_ONLY_RAW_PLUS_H */
   N_("Cannot process more than one file in -raw or -raw+h mode."),
   /* STID_ONE_FILE_ONLY_DOSEPSFILTER */
   N_("Cannot process more than one file in -dosepsfilter mode."),
   /* STID_CANT_PRTGIF_OFPP_TILED_PAGE */
   N_("Cannot use -one_file_per_page mode with a TILED page mode file."),
   /* STID_INVALID_SHARELIB_VER_IN_FILE */
   N_("Invalid shared library version in '%s'."),
   /* STID_FAIL_LOAD_SHARELIB */
   N_("Fail to load shared library '%s'."),
   /* STID_FAIL_GET_ENTRY_PT_IN_SHARELIB */
   N_("Fail to get entry point '%s' in shared lib '%s'."),
   /* STID_SHARELIB_TGEF_MSG_INIT_FAIL */
   N_("%s: TGEF_MSG_INIT failed with return code %ld."),
   /* STID_SHARELIB_CNFLCT_PROTOCOL_VER */
   N_("Conflicting protocol version %1d implemented by '%s'."),
   /* STID_SHARELIB_TGEF_MSG_LOAD_FAIL */
   N_("%s: TGEF_MSG_LOAD failed with return code %ld."),
   /* STID_SHARELIB_TGEF_MSG_NO_BMP_INFO */
   N_("%s: TGEF_MSG_LOAD returned with no bitmap info."),
   /* STID_ALLOC_BMP_FAIL_FOR_SHARELIB */
   N_("Failed to allocate bitmap for '%s'."),
   NULL
};

static char *gaszEnglishStrTable1200[] = {
   /* STID_UNSUP_PROTOCOL_VER_SHARELIB */
   N_("Unsupported protocol version %1d implemented by '%s'."),
   /* STID_SHARELIB_TGEF_MSG_DO_EXPORT_F */
   N_("%s: TGEF_MSG_DO_EXPORT failed with return code %ld."),
   /* STID_NEG_VAL_IN_EXP_PXL_TRM_ALT */
   N_("Error in parsing ExportPixelTrim values: negative values are not allowed.\n\n[%1d,%1d,%1d,%1d] used."),
   /* STID_EXP_PXL_TRM_VAL_SET_TO */
   N_("ExportPixelTrim values set to [%1d,%1d,%1d,%1d]."),
   /* STID_SPECIFY_TO_TRIM_EXP_PXL_TRM */
   N_("Please specify the number of pixels to trim when exporting (or printing) in bitmap-type format: [current: %1d,%1d,%1d,%1d]"),
   /* STID_ENTER_4_NUM_CUR_TOO_LARGE */
   N_("Please enter 4 numbers or type <CR> or <ESC> to use the current values (they are too large now):"),
   /* STID_ENTER_4_NUM_OR_CR_ESC */
   N_("Please enter 4 numbers or type <CR> or <ESC> to use the current values:"),
   /* STID_OBSOLETE_XDEF_USE_IGNORED */
   N_("Obsoleted %s.%s used.  Ignored."),
   /* STID_WILL_USE_FS_HALFTONE_BMP */
   N_("Will use Floyd-Steinberg half-tone method when exporting an X11 bitmap file."),
   /* STID_WILL_NOT_USE_FS_HALFTONE_BMP */
   N_("Will not use Floyd-Steinberg half-tone method when exporting an X11 bitmap file."),
   /* STID_WILL_USE_GIVE_SMPLE_THRESHOLD */
   N_("Will use simple thresholding method with a threshold of '%s' when exporting an X11 bitmap file."),
   /* STID_WILL_NOT_USE_SIMPLE_THRESHOLD */
   N_("Will not use simple thresholding method when exporting an X11 bitmap file."),
   /* STID_ENTER_BMP_THRESHOLD_CUR_VAL */
   N_("Please specify bitmap threshold when exporting an X11 bitmap file (current value is '%s')."),
   /* STID_INVALID_THRESHOLD_REMAINS */
   N_("Invalid threshold: '%s' specified.  Threshold remains at '%s'."),
   /* STID_USE_GIVEN_AS_BMP_THRESHOLD */
   N_("Will use '%s' as bitmap threshold when exporting an X11 bitmap file."),
   /* STID_XGETIMAGE_MAY_RUN_OUT_VMEM */
   N_("XGetImage() failed.  May have run out of virtual memory."),
   /* STID_NO_X11_BITMAP_OBJ_SELECTED */
   N_("No X11 Bitmap objects selected."),
   /* STID_CANT_CUT_AN_EPS_OBJECT */
   N_("Cannot cut an EPS object."),
   /* STID_XBM_CANT_HAVE_0_W_OR_H */
   N_("Bitmap cannot have 0 width or height."),
   /* STID_BAD_RED_MASK_TRUE_COLOR_DPY */
   N_("Unexpected red_mask value of 0x%06lx in the default visual for this TrueColor display.  Proceed pretending that the display is not TrueColor.  Please set %s.%s to FALSE to avoid this error message in the future."),
   /* STID_BAD_GREEN_MASK_TRUE_COLOR_DPY */
   N_("Unexpected green_mask value of 0x%06lx in the default visual for this TrueColor display.  Proceed pretending that the display is not TrueColor.  Please set %s.%s to FALSE to avoid this error message in the future."),
   /* STID_BAD_BLUE_MASK_TRUE_COLOR_DPY */
   N_("Unexpected blue_mask value of 0x%06lx in the default visual for this TrueColor display.  Proceed pretending that the display is not TrueColor.  Please set %s.%s to FALSE to avoid this error message in the future."),
   /* STID_NETPBM_PROGS_VISIT_HOME_PAGE */
   N_("(These programs are part of the netpbm package.  Please visit %s's home page at <URL:%s> regarding where to obtain netpbm.)"),
   /* STID_NETPBM_PNMTOPNG_VST_HOME_PAGE */
   N_("(These programs are part of the netpbm and pnmtopng packages.  Please visit %s's home page at <URL:%s> regarding where to obtain netpbm and pnmtopng.)"),
   /* STID_FNAME_FORMAT_ERROR_IN_FUNC */
   N_("File name format error in %s."),
   /* STID_NAMED_IMAGEMAP_FILE_GEN */
   N_("Imagemap file '%s' generated."),
   /* STID_CANT_FND_FATTR_IMGMAP_NOT_GEN */
   N_("Cannot find a '%s' file attribute.\n\nImagemap not generated."),
   /* STID_CANT_FND_FATTR_NO_IMGMAP_GIF */
   N_("Cannot find a '%s' file attribute.\n\nImagemap not generated.  Only a GIF file is generated."),
   /* STID_NAMED_HTML_FILE_GEN */
   N_("HTML file '%s' generated."),
   /* STID_LARGE_EXP_PXL_TRM_WH_0_USED */
   N_("The ExportPixelTrim values of [%1d,%1d,%1d,%1d] are too large for the image (%1dx%1d).  Zeroes are used."),
   /* STID_APPLY_EXP_PXL_TRM_VALS */
   N_("Applying ExportPixelTrim of [%1d,%1d,%1d,%1d]..."),
   /* STID_TOO_MANY_COLORS_LIMIT_IS */
   N_("Too many colors: %1d (limit is %1d) in the image."),
   /* STID_UNRECOG_GIVEN_PIXEL_VAL_PRINT */
   N_("Unrecognized pixel value at (%1d,%1d): %1d (0x%08lx).\n\nPrinting/exporting aborted."),
   /* STID_UNRECOG_GIVEN_PIXEL_VAL_1_USE */
   N_("Unrecognized pixel value: %1d (0x%08lx).\n\n1 used."),
   /* STID_FORMAT_FILE_WH_PRINTED_INTO */
   N_("%s file [%1dx%1d] printed into '%s'."),
   /* STID_SEL_XPM_DECK_TO_EXPORT */
   N_("Please select a deck of X Pixmap object to export."),
   /* STID_MANY_SZ_FAIL_XPM_DESK_EXPORT */
   N_("Not all X Pixmap objects in the deck are of size.  Expecting %1dx%1d.\n\nExport aborted."),
   /* STID_INVALID_CH_PER_PIX_IN_FUNC */
   N_("Invalid chars_per_pixel (%1d) in %s."),
   /* STID_XPM_CANT_HAVE_0_W_OR_H */
   N_("Pixmap cannot have 0 width or height."),
   /* STID_WARN_XPM_ALL_NON_BG_BE_BLACK */
   N_("Warning (xpm object): all non-background color will be treated as black."),
   /* STID_SAVED_FILE_MAY_BE_CORRUPTED */
   N_("Saved file may be corrupted."),
   /* STID_CANT_SAVE_XPM_ON_BW_DPY */
   N_("Cannot save X11 Pixmap objects on a Black & White display."),
   /* STID_CANT_IMPORT_GIVEN_FORMAT_XPM */
   N_("Does not know how to import an X11 Pixmap file with format: '%1d'."),
   /* STID_CANT_IMPORT_GIVEN_CPP_XPM */
   N_("Does not know how to import an X11 Pixmap file with chars_per_pixel: '%1d'."),
   /* STID_EDIT_UNNAME_ATTR_DOTS */
   N_("Edit an unnamed attribute..."),
   /* STID_EDIT_VAL_OF_ATTR_DOTS */
   N_("Edit the value for the '%s' attribute..."),
   /* STID_EDIT_UNNAME_ATTR_WITH_CMD */
   N_("Editing an unnamed attribute with '%s'..."),
   /* STID_EDIT_VAL_OF_ATTR_WITH_CMD */
   N_("Editing the value for the '%s' attribute with '%s'..."),
   /* STID_STAT_FAIL_EDIT_ATTR_VAL_SAME */
   N_("Fail to get file info for '%s' while editing an attribute.  Attribute value is unchanged."),
   /* STID_ENTER_HTML_TEMPLATE */
   N_("Please specify a file to be used as a template for generating HTML files:"),
   /* STID_ENTER_HTML_TEMPLATE_CUR_IS */
   N_("Please specify a file to be used as a template for generating HTML files (current template is '%s'):"),
   /* STID_HTML_TEMPLATE_UNCHANGED */
   N_("HTML export template file unchanged."),
   /* STID_HTML_TEMPLATE_SET_TO_GIVEN */
   N_("HTML export template file set to '%s'."),
   /* STID_NO_HTML_TEMPLATE_FILE */
   N_("Will not use an HTML export template file."),
   /* STID_Q_FILE_NOT_EXIST_USE_ANYWAY */
   N_("File '%s' does not exist.\n\nWould you like to use it anyway?"),
   /* STID_CANNOT_OPEN_HTML_TMPL_READ */
   N_("Cannot open HTML export template file '%s' for reading."),
   /* STID_OK_TO_FULL_SCREEN_CAPTURE_YNC */
   N_("Okay to proceed with full screen capture? [ync](y)"),
   /* STID_Q_FILE_UNSAVABLE_SAVE_NEW_YNC */
   N_("File is marked unsavable.\n\nWould you like to save to a new file name? [ync](y)"),
   /* STID_WONT_CONDENSE_READHEXSTRING */
   N_("Will not condense exported PS/EPS file due to 'readhexstring'."),
   /* STID_CMD_ONLY_AVAIL_IN_VERTEX_MODE */
   N_("This command is only available in vertex mode."),
   /* STID_CANNOT_MERGE_WITH_TILED_PAGE */
   N_("Does not know how to merge files with '%s' because it is in TILED page mode.  Merge aborted."),
   /* STID_SPECIFY_PAGES_TO_PRINT_EX */
   N_("Please specify pages to print: (e.g., 2,3,7-9,10 or '*' for all pages)"),
   /* STID_CANT_PRINT_PAGES_IN_TILED */
   N_("Cannot print pages in TILED page mode."),
   /* STID_CANT_PRINT_PAGES_IN_FORMAT */
   N_("Cannot print pages in the '%s' format."),
   /* STID_PRECISE_SCALE_EV_CAUSE_FLUSH */
   N_("Precise Scale Everything will cause the undo buffer to be flushed."),
   /* STID_GO_HYPERSPACE_IN_SLIDE */
   N_("HyperSpace mode will be entered upon entering slideshow mode."),
   /* STID_DONT_GO_HYPERSPACE_IN_SLIDE */
   N_("Freehand drawing mode will be entered upon entering slideshow mode."),
   /* STID_MOVE_EDIT_TEXT_BOX_ABORTED */
   N_("Move edit text box aborted."),
   /* STID_DRAG_MOUSE_MOVE_EDITTEXT_DOTS */
   N_("Drag the edit text box to move it around..."),
   /* STID_EXCEED_BUF_SIZE_LIMIT_IN_WB */
   N_("Error: There's a buffer size limitation of %1d bytes in the current implementation of the whiteboard.  The buffer size requirement (%1d bytes) for the operation you just performed has exceeded this limit.\n\nIn order to keep the whiteboard running, the operation in question is canceled."),
   /* STID_CANT_REPLACE_GRAPHIC_NOT_TGIF */
   N_("Cannot replace graphical objects because cut buffer does not contain %s objects."),
   /* STID_CANT_REPLACE_GRAPHIC_TOO_MANY */
   N_("Cannot replace graphical objects because cut buffer contain more than one objects."),
   /* STID_REPLACE_GRAPHIC_ICON_TO_GROUP */
   N_("Some iconic objects have been converted to grouped objects during Replace Graphic."),
   /* STID_REPLACE_GRAPHIC_DONE */
   N_("Graphical part of selected objects have been replaced and updated."),
   /* STID_TOO_MANY_ICON_REPLACE_GRAPHIC */
   N_("More than one iconic objects are selected for Replace Graphic."),
   /* STID_Q_CVT_ICON_TO_GRP_IN_REPLACE */
   N_("Replacing the graphic of an iconic object...\n\nWould you like to convert the object into a grouped object? [ync](y)\n\n(If you press the No button, you will be prompted with the name of a new symbol file the iconic object.  If you press Cancel, this object will be skipped for Replace Graphic.)"),
   /* STID_SHOW_MEASUREMENT_IN_TT_ENAB */
   N_("Showing measurement in tooltip enabled."),
   /* STID_SHOW_MEASUREMENT_IN_TT_DISB */
   N_("Showing measurement in tooltip disabled.  Measurement will be shown in the ruler window."),
   /* STID_SEL_JPEG_FILE_TO_IMPORT */
   N_("Please select a JPEG file to IMPORT..."),
   /* STID_CANNOT_IMPORT_GIVEN_JPEG */
   N_("Cannot import JPEG file '%s'."),
   /* STID_GIVEN_JPEG_SIZE_FILE_IMPORTED */
   N_("JPEG file (%1dx%1d) '%s' imported."),
   /* STID_WILL_EXPORT_JPEG_FILE */
   N_("Will export JPEG (needs xpm->jpg filter) file."),
   /* STID_NETPBM_JPEG_VST_HOME_PAGE */
   N_("(These programs are part of the netpbm and jpeg packages.  Please visit %s's home page at <URL:%s> regarding where to obtain netpbm and jpeg.)"),
   /* STID_CANT_GEN_PREVIEW_FOR_PS */
   N_("Cannot auto-generate a preview bitmap for '%s'."),
   /* STID_CANT_GEN_PREVIEW_FOR_EMPTY_PS */
   N_("The auto-generate preview bitmap for '%s' is empty.  Auto-generation skipped."),
   /* STID_WILL_AUTO_GEN_PREVIEW_BITMAP */
   N_("Will auto-generate a preview bitmap when importing/embedding a PS/EPS file."),
   /* STID_WONT_AUTO_GEN_PREVIEW_BITMAP */
   N_("Will not auto-generate a preview bitmap when importing/embedding a PS/EPS file."),
   /* STID_TMP_DIR_NOT_EXIST_USE_XDEF */
   N_("Temporary directory '%s' does not exist.  Please fix the %s.%s X default."),
   /* STID_INVALID_DIR_IN_X_DEFAULT */
   N_("Invalid directory '%s' specified by X default %s.%s."),
   /* STID_RACE_IN_CREATE_TMP_FILE */
   N_("Race condition detected in creating the '%s' temporary file.  Possible causes are network problems, security problems, etc.  This file will be removed before proceeding."),
   /* STID_INVALID_GEOM_USE_ALT_GEOM */
   N_("Invalid %s.%s: '%s', the geometry must be at least '%s', '%s' is used instead."),
   /* STID_REDUCE_BY_FACTOR */
   N_("Reducing by a factor of %.2f..."),
   /* STID_ENLARGE_BY_FACTOR */
   N_("Enlarging by a factor of %.2f..."),
   /* STID_CREATE_THUMBNAIL_CAUSE_FLUSH */
   N_("Create Thumbnails will cause the undo buffer to be flushed."),
   /* STID_CONNECT_PORTS_CANCEL_BY_USER */
   N_("Connect ports canceled by the user."),
   /* STID_Q_CNFLCT_SIG_NAME_OKAY_TO_CLR */
   N_("Conflicting signal names '%s' (at '%s') and '%s' (at '%s').\n\nOkay to clear both signal names? [ync](y)"),
   /* STID_PLS_ENT_SIG_NAME */
   N_("Please enter a signal name:"),
   /* STID_SIGNAL_NAMES_CLEARED */
   N_("Signal names at '%s' and '%s' have been cleared."),
   /* STID_PLACING_NAMED_SIGNAL */
   N_("Click to place signal name '%s'"),
   /* STID_PLS_ENT_SIG_NAME_FOR_PORT */
   N_("Please enter a signal name for the '%s' port:"),
   NULL
};

static char *gaszEnglishStrTable1300[] = {
   /* STID_NO_CUR_FILE_CANNOT_GEN_NETLST */
   N_("No current file.\n\nCannot generate a net list."),
   /* STID_NO_SIGNAME_FOUND_EMPTY_NETLST */
   N_("No signal name found.\n\nNet list not generated."),
   /* STID_WILL_EXPORT_NETLIST_FILE */
   N_("Will export net-list file."),
   /* STID_Q_OK_TO_CUT_INTO_SEGMENTS */
   N_("Okay to cut selected polyline/polygon objects into segments? [ync](y)\n\n(If you would like to cut at a specific vertex of the polyline/polygon, please use the vertex mode.)"),
   /* STID_NO_POLY_OBJ_SELECTED */
   N_("No polyline/polygon object selected."),
   /* STID_WILL_SHOW_WIRE_SIGNAL_NAME */
   N_("Will show (and place) wire signal name when connecting ports."),
   /* STID_WILL_HIDE_WIRE_SIGNAL_NAME */
   N_("Will hide wire signal name when connecting ports."),
   /* STID_TRY_AGAIN_AND_CLICK_IN_A_PORT */
   N_("Please try again and click in a port."),
   /* STID_TRY_AGAIN_AND_END_IN_A_PORT */
   N_("The last vertex did not end in a port, please try again."),
   /* STID_CANNOT_CREATE_ARC_WITH_RAD1 */
   N_("%s cannot create an arc with radius %1d (must be at least 1)."),
   /* STID_SEL_ONE_NONE_PORT_PLUS_COMP */
   N_("Please select at least one port object and a composite object."),
   /* STID_PORT_OBJ_HAS_EMPTY_NAME */
   N_("The port object has an empty name."),
   /* STID_CANNOT_FIND_INIT_ATTR_PORT_PA */
   N_("Cannot find the 'Initialization=' attribute of the '%s' object (parent of the port object)."),
   /* STID_CANNOT_FIND_VAR_IN_INIT_ATTR */
   N_("Cannot find the '%s' variable in the 'Initialization=' attribute of the '%s' object (parent of the port object)."),
   /* STID_RENUMBER_OBJ_IDS_CAUSE_FLUSH */
   N_("Renumber Object IDs will cause the undo buffer to be flushed."),
   /* STID_RENUMBER_OBJ_IDS_DONE */
   N_("Renumbering object IDs done."),
   /* STID_RENUMBER_OBJ_IDS_IN_WB */
   N_("RenumberObjectIDs() is disabled in WhiteBoard mode."),
   /* STID_SEL_ONE_NONE_PORT_PLUS_BRDCST */
   N_("Please select at least one port object and a broadcast wire object."),
   /* STID_CONFLICT_SIG_NAME_ENT_NEW */
   N_("Conflicting signal names detected.  Please entry a valid signal name for all the ports:"),
   /* STID_BAD_SIG_NAME_UNDO_AND_REDO */
   N_("Invalid or inconsistent signal names are given to the ports.  Please undo the previous operation and redo it again."),
   /* STID_WILL_USE_WORDWRAP_IMPORT_TEXT */
   N_("Will use word wrap when importing a multipage text file."),
   /* STID_NO_WORDWRAP_IMPORT_TEXT */
   N_("Will not use word wrap when importing a multipage text file."),
   /* STID_CUR_MARGINS_ARE_GIVEN */
   N_("( current margins (top, bottom, left, right) are: %s )"),
   /* STID_ENTER_MARGINS */
   N_("Please enter margins to be used when importing a multipage text file: [top,bottom,left,right] (e.g., %s -- the three commas are mandatory)"),
   /* STID_NEW_MARGINS_ARE_GIVEN */
   N_("New margins for import multipage text file are (top, bottom, left, right):"),
   /* STID_CANT_OBTAIN_TMP_FILE_NAME */
   N_("Cannot obtain a temporary file name using mkstemp() from the following request: '%s'.  Will use mktemp() instead."),
   /* STID_NO_OBJ_SPEC_WHILE_EXEC_CMD */
   N_("Non object specified while executing the '%s' command."),
   /* STID_UNRECOG_GIVEN_TRPIX_VAL_PRINT */
   N_("Unrecognized transparent pixel value at (%1d,%1d): %1d (0x%08lx).\n\nPrinting/exporting aborted."),
   /* STID_TGIF_DISTRUBITION */
   N_(" [distribution: %s]"),
   /* STID_WRITE_TO_SELF_PIPE_FAIL */
   N_("Fail to write to the pipe that talks to %s itself."),
   /* STID_CREATE_SELF_PIPE_FAIL */
   N_("Fail to create the pipe that talks to %s itself."),
   /* STID_READ_FROM_SELF_PIPE_FAIL */
   N_("Fail to read %1d byte from the pipe that talks to %s itself."),
   /* STID_ENTER_BEZIER_NUM_SEGS_CUR_IS */
   N_("Please enter the number of segments for the Convert To Bezier command (current value is %1d):"),
   /* STID_BEZIER_NUM_SEGS_SET_TO_INT */
   N_("Number of segments for the Convert To Bezier command set to %1d."),
   /* STID_ENT_VAL_RANGE_ENTER_GE_INT */
   N_("The value entered: '%s' is out of range.\n\nPlease enter a value >= %1d."),
   /* STID_ENTER_TICK_MARK_SIZE_CUR_IS */
   N_("Please enter the tick mark size for the various Add Tick Mark commands (current value is %1d):"),
   /* STID_TICK_MARK_SIZE_SET_TO_INT */
   N_("Tick mark size set to %1d."),
   /* STID_NO_SPLINE_SELECTED */
   N_("No polyline or simple spline objects selected."),
   /* STID_SPLINE_CONVERTED_TO_BEZIER */
   N_("Spline objects converted to Bezier curves."),
   /* STID_PEEK_DIM_LEFT */
   N_("The x-coordinate of the left most part of the selected object."),
   /* STID_PEEK_DIM_TOP */
   N_("The y-coordinate of the top most part of the selected object."),
   /* STID_PEEK_DIM_RIGHT */
   N_("The x-coordinate of the right most part of the selected object."),
   /* STID_PEEK_DIM_BOTTOM */
   N_("The y-coordinate of the bottom most part of the selected object."),
   /* STID_PEEK_DIM_WIDTH */
   N_("The width of the selected object."),
   /* STID_PEEK_DIM_HEIGHT */
   N_("The height of the selected object."),
   /* STID_PEEK_DIM_CX */
   N_("The x-coordinate of the center of the selected object."),
   /* STID_PEEK_DIM_CY */
   N_("The y-coordinate of the center of the selected object."),
   /* STID_NO_POLY_ETC_SELECTED */
   N_("No polyline/polygon/spline objects selected."),
   /* STID_SEL_A_SYM_TO_INSTAN_IN_NONE */
   N_("Please select a symbol to INSTANTIATE..."),
   /* STID_WRITE_NOT_AUTO_IN_HYPER_EXEC */
   N_("Writing into file is not automatic in hyperspace.  Do you want to allow writing into '%s'?\n\n(If you are not sure about this, just click on NO.)"),
   /* STID_USER_ABORT_WRITE_IN_HYPER */
   N_("Writing into file in hyperspace aborted at user's request."),
   /* STID_READ_FROM_SELF_PIPE_OVERFLOW */
   N_("Buffer overflow while reading the pipe that talks to %s itself."),
   /* STID_BAD_CMDLINE_OPT_USE_ALT_STR */
   N_("Invalid commandline option %s=\"%s\", \"%s\" is used instead."),
   /* STID_BAD_CMDLINE_OPT_IGNORED */
   N_("Invalid commandline option %s=\"%s\"; it is ignored."),
   /* STID_OBSOLETE_CMDLINE_ASSUME */
   N_("The %s commandline option is obsolete; assuming you mean %s."),
   /* STID_INVALID_XDEF_IGNORED */
   N_("Invalid %s.%s: '%s'.  Resource ignored."),
   /* STID_OBSOLETE_XDEF_REPLACED */
   N_("Obsoleted %s.%s used.  Assuming %s.%s to have the value of '%s' instead."),
   /* STID_FILE_IS_MARKED_UNSAVABLE */
   N_("File is marked unsavable."),
   /* STID_ONLYONE_DBIM_OR_SBIM */
   N_("Only one -dbim or -sbim can be used."),
   /* STID_DBIM_UNSUPPORTED */
   N_("The double-byte input method: '%s' is not supported."),
   /* STID_SBIM_UNSUPPORTED */
   N_("The single-byte input method: '%s' is not supported."),
   /* STID_CANNOT_OPEN_IN_SLIDESHOW */
   N_("Please exit the slideshow mode before opening a new file."),
   /* STID_TOGGLE_AUTO_SCR_IN_CHAT */
   N_("Toggle auto scroll mode in the chat list."),
   /* STID_TOGGLE_BOLD_IN_CHAT */
   N_("Toggle bold font in chat."),
   /* STID_TOGGLE_ITALIC_IN_CHAT */
   N_("Toggle italic font in chat."),
   /* STID_SEND_CHAT_TEXT */
   N_("Send text to all whiteboard participants."),
   /* STID_MALFORMED_CHAT_LINE */
   N_("Malformed chat line '%s'."),
   /* STID_UNRECOG_FIELD_IN_CHAT_LINE */
   N_("Unrecognized '%s' field in a chat line."),
   /* STID_UNRECOG_BYTE_FROM_SELF_PIPE */
   N_("Unrecognized byte value '%c' read from the pipe that talks to %s itself."),
   /* STID_JOIN_WB_IN_PROGRESS_IGNORED */
   N_("Joining a whiteboard session already in progress.  Request to open the '%s' file at startup is ignored."),
   /* STID_PEEK_AREA */
   N_("The area of the selected object (polygon must not be self-intersecting)."),
   /* STID_CANT_DO_SAVEPAGEAS_IN_TILED */
   N_("Cannot do SavePageAs() in TILED page mode."),
   /* STID_SPECIFY_PAGES_TO_SAVE_EX */
   N_("Please specify pages to save into a new file: (e.g., 2,3,7-9,10)"),
   /* STID_ONE_PAGE_SAVED_INTO_GIVEN */
   N_("1 page saved into '%s'."),
   /* STID_MANY_PAGES_SAVED_INTO_GIVEN */
   N_("%1d pages saved into '%s'."),
   /* STID_CANT_IMP_ANIM_GIF_DIR_OR_PATH */
   N_("Cannot import GIF animation file '%s'.  Please make sure that you can write to the '%s' directory.\n\nPlease also make sure that '%s' is installed properly."),
   /* STID_WILL_EXPORT_PPM_FILE */
   N_("Will export PPM file."),
   /* STID_NETPBM_VST_HOME_PAGE */
   N_("(This program is part of the netpbm packages.  Please visit %s's home page at <URL:%s> regarding where to obtain netpbm.)"),
   /* STID_SEL_PPM_FILE_TO_IMPORT */
   N_("Please select a PPM file to IMPORT..."),
   /* STID_CANNOT_IMPORT_GIVEN_PPM */
   N_("Cannot import PPM file '%s'."),
   /* STID_GIVEN_PPM_SIZE_FILE_IMPORTED */
   N_("PPM file (%1dx%1d) '%s' imported."),
   /* STID_IMPORT_FORMAT_WHILE_EXEC_CMD */
   N_("Fail to import '%s' in the '%s' format while executing the '%s' command."),
   /* STID_NO_OBJ_TO_IMP_WHILE_EXEC_CMD */
   N_("No object to import in '%s' while executing the '%s' command."),
   /* STID_CANNOT_LOAD_TGTWB5_FONT */
   N_("Cannot load the '%s' font.  Please specify a valid zh_TW.big5 font using the %s.%s X default (syntax is \"%s,FONTNAME\")."),
   /* STID_TOOL_BIG5_CHINESE_KEYPAD */
   N_("%s - Traditional Chinese (zh_TW.big5) Keypad"),
   /* STID_USE_ZYFH_KEYPAD_OR_PINYING */
   N_("Please select using zhu-yin-fu-hao keypad\nor enter pin-ying directly..."),
   /* STID_INVALID_PINYIN_SEQ */
   N_("The sequence: '%s', is not a valid sequence."),
   /* STID_ADD_ZYFH */
   N_("Add a zhu-yin-fu-hao symbol"),
   /* STID_FETCH_CHAR */
   N_("Fetch characters"),
   /* STID_BACKOUT_ZYFH */
   N_("Back out a zhu-yin-fu-hao symbol"),
   /* STID_SELECT_A_CHAR */
   N_("Select a character."),
   /* STID_SEND_A_CHAR_TO_DRAWING_AREA */
   N_("Select and send a character to the drawing area."),
   /* STID_CLICK_SEND_TO_SEND_CHAR */
   N_("Click on the Send button to send selected character to the drawig area."),
   /* STID_FETCH_CHARS_FROM_DB */
   N_("Fetch characters from the database."),
   /* STID_SEND_SEL_CHAR_TO_DRAWING_AREA */
   N_("Send selected character to the drawing area."),
   /* STID_SEND_CR_TO_DRAWING_AREA */
   N_("Send <CR> to the drawing area."),
   /* STID_OK_TO_RESET_GIVEN_IM */
   N_("Okay to reset the connection to the '%s' input method?"),
   /* STID_GIVEN_IM_RESETED */
   N_("Connection to the '%s' input method has been reset."),
   /* STID_WILL_EXPORT_PBM_FILE */
   N_("Will export PBM file."),
   /* STID_SEL_PBM_FILE_TO_IMPORT */
   N_("Please select a PBM file to IMPORT..."),
   NULL
};

static char *gaszEnglishStrTable1400[] = {
   /* STID_CANNOT_IMPORT_GIVEN_PBM */
   N_("Cannot import PBM file '%s'."),
   /* STID_GIVEN_PBM_SIZE_FILE_IMPORTED */
   N_("PBM file (%1dx%1d) '%s' imported."),
   /* STID_WILL_EXPORT_PGM_FILE */
   N_("Will export PGM file."),
   /* STID_SEL_PGM_FILE_TO_IMPORT */
   N_("Please select a PGM file to IMPORT..."),
   /* STID_CANNOT_IMPORT_GIVEN_PGM */
   N_("Cannot import PGM file '%s'."),
   /* STID_GIVEN_PGM_SIZE_FILE_IMPORTED */
   N_("PGM file (%1dx%1d) '%s' imported."),
   /* STID_CANNOT_IDEN_GS */
   N_("Problem identifying which version of ghostscript you are running ('%s')."),
   /* STID_TOOL_NOT_INTV_WHILE_EXEC_CMD */
   N_("%s is not running in the interactive mode while executing the '%s' command."),
   /* STID_CONVERT_TO_XBM_WHILE_EXEC_CMD */
   N_("Fail to convert '%s' from the '%s' format to XBM format while executing the '%s' command."),
   /* STID_FUNC_ARG_STRS_RANGE */
   N_("%s(): '%s' must be between %s and %s (inclusive).\n\n%s was specified."),
   /* STID_WARN_CANNOT_FIND_SOME_ATTRS */
   N_("Warning: some attributes in '%s' cannot be found.\n\nOkay to continue and edit the attributes which have been found? [ync](y)"),
   /* STID_BAD_LINE_MAY_HAVE_DEL_EQUAL */
   N_("Malformed line: '%s'.\n\nYou may have accidentally deleted a '=' symbol.  Please make a note of it."),
   /* STID_ATTR_NAME_CHANGED_IMPORT_Q */
   N_("You may have changed the name of an attribute.  The new attribute name is '%s'.  Do you want to import this new attribute? [ync](y)"),
   /* STID_CANT_IMPORT_ATTR_GROUP_RETRY */
   N_("Since there are malformed input lines, %s cannot import the attribute group.  Please fix the malformed lines and try again.\n\nIf you would like to abort editing this attribute group, please exit your editor without saving the content."),
   /* STID_TGIF_FLAGS */
   N_(" [flags: %s]"),
   /* STID_INVALID_COMPILER_OPT */
   N_("Malformed compiler option -D%s: '%s'."),
   /* STID_EXECUTING_GIVEN_SCRIPT */
   N_("Executing from '%s'..."),
   /* STID_STAT_FAILED_WHILE_IMPORT_LINK */
   N_("Fail to get file info for '%s' while importing/linking %s file."),
   /* STID_SEL_JPEG_FILE_TO_LINKTO */
   N_("Please select a JPEG file to link to..."),
   /* STID_INVALID_PARAM_LINKED_JPEG */
   N_("Invalid parameters encountered while reading a linked JPEG object."),
   /* STID_INVALID_PATH_WHILE_READ_JPEG */
   N_("File '%s' does not exist while reading a linked JPEG object."),
   /* STID_PRTGIF_LINK_JPEG_SKIPPED */
   N_("Linked JPEG object (linked to '%s') skipped.  Please consider running %s with the \"-display\" commandline option."),
   /* STID_DUP_SHORTCUT_FUNCTION_KEY */
   N_("Warning: duplicate shortcut entry for 'F%1d'."),
   /* STID_BAD_STYLE_NAME_WHILE_EXEC_CMD */
   N_("Invalid text style of '%s' used while executing the '%s' command."),
   /* STID_CANT_ACQUIRE_X_SELECTION */
   N_("Cannot acquire the X selection while performing a paste operation.  Will only copy to X cutbuffer."),
   /* STID_NO_TEXT_SELECTED_FOR_COPY */
   N_("No text selected for the copy/cut operation."),
   /* STID_PASTE_FAILED_CONV_REFUSED */
   N_("Paste failed because selection conversion was refused."),
   /* STID_BAD_XDEF_MISS_TOKEN_NAME_SKIP */
   N_("Invalid entry '%s' (missing token name) in %s.%s, skipped."),
   /* STID_BAD_XDEF_CANT_FIND_TOKEN_SKIP */
   N_("Invalid entry '%s' (cannot find %s) in %s.%s, skipped."),
   /* STID_BAD_XDEF_MISS_CHAR_NAME_SKIP */
   N_("Malformed entry '%s' (missing character name) in %s.%s, skipped."),
   /* STID_DUP_CHAR_CODE_XDEF_IGNORED */
   N_("Duplicate character code %1d (8#%03o) detected in %s.%s, new value ignored."),
   /* STID_FAIL_TO_PARSE_CHAR_CODE_XDEF */
   N_("Fail to parse '%s' for a character code value for taken '%s' while processing X default %s.%s."),
   /* STID_CHAR_CODE_OUT_OF_RANGE_XDEF */
   N_("Character code specified as '%s' for token '%s' is out of range in %s.%s.  The value must be between 161 and 255 (inclusive)."),
   /* STID_BAD_FAKE_FONT_NAME_NO_ALIAS */
   N_("In processing %s.%s, the fake font name '%s' is not valid because there is no corresponding entry in %s.%s."),
   /* STID_CANT_FIND_PS_CHAR_SUBS_READ */
   N_("Cannot find ps_char_subs(\"%s\",[...]) object while reading the input file."),
   /* STID_DUP_TOKEN_PS_CHAR_SUBS_READ */
   N_("Duplicate ps_char_subs(\"%s\",[...]) object detected reading the input file."),
   /* STID_FAIL_TO_PARSE_CHAR_CODE_FILE */
   N_("Fail to parse '%s' for a character code value for taken '%s' while processing the input file."),
   /* STID_CHAR_CODE_OUT_OF_RANGE_FILE */
   N_("Character code specified as '%s' for token '%s' is out of range while processing the input file.  The value must be between 161 and 255 (inclusive)."),
   /* STID_DUP_CHAR_CODE_FILE_IGNORED */
   N_("Duplicate character code %1d (8#%03o) detected while processing the input file, new value ignored."),
   /* STID_CMDLINE_ARG_GIVEN_TOO_LONG */
   N_("The commandline argument for '%s' is too long.  Please use a shorter one."),
   /* STID_CANT_IMPORT_GIVEN_PPM_MALFORM */
   N_("Failed to import PPM file '%s'.  The P6 format is expected but the file seems to be malformed."),
   /* STID_PPM6_DIM_NOT_MATCH */
   N_("The dimension (%1dx%1d) of the PPM file '%s' does not match the stored dimension (%1dx%1d)."),
   /* STID_UNSUP_PPM6_MAX_VAL */
   N_("The maximum color value (%1d) for the PPM file '%s' is not supported.  Only the value of 255 is supported."),
   /* STID_JPEG_DIM_NOT_MATCH_DELETED */
   N_("The dimension (%1dx%1d) of the linked JPEG file '%s' does not match the stored dimension (%1dx%1d).  The JPEG object is removed."),
   /* STID_ENTER_XY_OFFSET_OBJ_SHADOW */
   N_("Please enter X and Y offsets for generic object shadow: [current: %1d,%1d]"),
   /* STID_OBJ_SHADOW_XY_OFFSETS_SET_TO */
   N_("Generic object shadow offsets set to: %1d, %1d."),
   /* STID_ENTER_A_COLOR_FOR_OBJ_SHADOW */
   N_("Please enter a color for the generic object shadow: [current: %s]"),
   /* STID_OBJ_SHADOW_COLOR_SET_TO_NAMED */
   N_("Generic object shadow color set to '%s'."),
   /* STID_NO_OBJ_SHADOW_FOUND */
   N_("None of the selected object seem to have a generic object shadow."),
   /* STID_TRANS_XPM_TOO_MANY_COLORS_SKP */
   N_("Too many colors (%1d, which is > 255) in an xpm objects with transparent pixels.  This object is skipped during printing."),
   /* STID_NEED_DISABLE_UNDO_WILL_CRASH */
   N_("Referencing an off page object while undo is enabled.  If %s crashes, please modify your script to call \"disable_undo()\" before running it again."),
   /* STID_CANT_LOAD_INITIAL_FONT */
   N_("Warning: Cannot load the following initial font: '%s'.  To specify a different initial font, please use %s.InitialFont, %s.InitialFontStyle, and %s.InitialFontSize."),
   /* STID_CANT_CHNG_FONT_SUPSUB_CENTER */
   N_("Cannot switch font in a center-script object."),
   /* STID_JOIN_WB_IN_PROGRESS_FAILED */
   N_("Joining a whiteboard session already in progress has failed.  The state of the whiteboard may not be consistent with other whiteboards.  It's probably best to exit and restart."),
   /* STID_JOIN_WB_IN_FAILED_NO_CONTTYPE */
   N_("Joining a whiteboard session already in progress has failed - cannot find Content-Type.  The state of the whiteboard may not be consistent with other whiteboards.  It's probably best to exit and restart."),
   /* STID_INVALID_RMCAST_DLIB_PATH */
   N_("Invalid path: '%s' for the rmcast dynamic library.  %s will exit now."),
   /* STID_NO_INFO_LIBRMCAST_SO */
   N_("Fail to load shared library '%s'.  There is no information about where to find %s.  Please run %s with the -rmcastlibdir or the -rmcastlibpath commandline option.  %s will exit now."),
   /* STID_BAD_VER_RMCAST_DLIB */
   N_("Malformed interface version information: '%s' received from the rmcast dynamic library '%s'."),
   /* STID_CANNOT_HANDLE_VER_RMCAST_DLIB */
   N_("%s does not know how to handle interface version: '%s' for the rmcast dynamic library '%s'."),
   /* STID_MISSING_RMCASTDIR */
   N_("Directory for the '-rmcastlibdir' commandline argument is missing."),
   /* STID_NO_PTHREAD_CONTINUE */
   N_("%s is not compiled with the -DPTHREAD flag and will not be able to run in the tgwb2 mode.  Please restart %s without the -tgwb2 commandline option.  %s will exit now."),
   /* STID_INVALID_GIVEN_PARAM_GIVEN_OBJ */
   N_("Invalid %s parameters encountered while reading a %s object."),
   /* STID_CANNOT_INFLATE_GIVEN_OBJ */
   N_("Cannot uncompress %s object with zlib because either zlib is not supported on this machine or %s is compiled with the -D_NO_ZLIB compiler option."),
   /* STID_BAD_LINE_READING_GIVEN_OBJ */
   N_("Malformed input line while reading a %s object."),
   /* STID_INSUF_DATA_READING_GIVEN_OBJ */
   N_("Insufficient data while reading a %s object."),
   /* STID_PATTERNDIR_NOT_EXIST */
   N_("Customer pattern directory '%s' does not exist.  Will use the default patterns."),
   /* STID_MISSING_RMCASTLIB */
   N_("File path for the '-rmcastlibpath' commandline argument is missing."),
   /* STID_INVALID_XBM_CUST_PAT */
   N_("Invalid X11 bitmap file '%s'.  It cannot not be used as a customer pattern."),
   /* STID_TRANS_PIX_NOT_SUP_PPM_TRUE */
   N_("Transparent pixel is not supported for ppm_true objects."),
   /* STID_XPM_HAS_ATTR_IMGPROC */
   N_("The command you have just selected: '%s' can only work with a single X11 Pixmap object that does not have attached attributes.  Please detach its attributes and try again."),
   /* STID_PPM_TRUE_LOSSY_CONV_TO_XPM */
   N_("A ppm_true object has been converted to a regular XPM object (may have lost some colors)."),
   /* STID_SWITCHED_TO_AUTO_ROTATE_PIVOT */
   N_("Switched to using automatic rotation pivot."),
   /* STID_SWITCHED_TO_USER_ROTATE_PIVOT */
   N_("Switched to using user-specified rotation pivot."),
   /* STID_NEW_ROTATE_PIVOT_IS */
   N_("Rotation pivot is at: (%1d,%1d)."),
   /* STID_ENTER_ROT_PIVOT */
   N_("Please enter rotation pivot pixel coordinate: [X,Y]"),
   /* STID_ENTER_ROT_PIVOT_CUR_IS */
   N_("Please enter rotation pivot pixel coordinate: [X,Y] (current rotation pivot is at (%1d,%1d))"),
   /* STID_ROT_PIVOT_SET_TO */
   N_("Rotation pivot set to: (%1d,%1d)."),
   /* STID_VISIBLE_ROTATE_PIVOT */
   N_("User-specified rotation pivit will be visible in select mode."),
   /* STID_NOT_VISIBLE_ROTATE_PIVOT */
   N_("User-specified rotation pivit will not be visible in select mode."),
   /* STID_SELONLYONEPOLYOBJ */
   N_("Please make sure that you have only a poly/polygon object selected and that you are in the rotate/shear mode with user-specified rotation pivot enabled."),
   /* STID_SELONLYONEARCOBJ */
   N_("Please make sure that you have only an arc object selected and that you are in the rotate/shear mode with user-specified rotation pivot enabled."),
   /* STID_WILL_EXPORT_SVG_FILE */
   N_("Will export SVG (needs eps->svg filter) file."),
   /* STID_TOOL_COLOR_WHEEL */
   N_("%s - Choose A Color"),
   /* STID_COLOR_WHEEL_NOT_AVAIL */
   N_("Color wheel is not available for your display type (must be a TrueColor display)."),
   /* STID_GIVEN_VAL_MUST_BE_LE */
   N_("The %s value must be less than or equal to %1d)."),
   /* STID_TWO_VERTEX_POLY_ONLY */
   N_("Please select a two-vertex polyline."),
   /* STID_ENTER_MULT_FACTOR */
   N_("Please enter a multiplicative factor (1.0 means no adjustment and a negative value will extend the line segment in the opposite direction):"),
   /* STID_RES_LINESEG_HAVE_ZERO_LEN */
   N_("Resulting line segment will have a length of zero and cannot be used."),
   /* STID_WILL_USE_TIGHTER_SPLINES */
   N_("Will use tighter structured splines."),
   /* STID_WILL_USE_LOOSER_SPLINES */
   N_("Will use original (looser) splines tightness for structured splines."),
   /* STID_TWO_CONSEC_VERTICES_ONLY */
   N_("Please select two consecutive vertices of a polyline object."),
   /* STID_LINE_TYPE_IS_STRUCT_SPLINE */
   N_("Line type is 'structured spline'."),
   /* STID_ONE_V_STRUCT_SPLINE_IN_V_MODE */
   N_("Can only select one vertex for a structured spline object in vertex mode."),
   /* STID_SHUTTING_DOWN_WHITEBOARD */
   N_("Shutting down whiteboard"),
   /* STID_SOME_LT_FAILED_TO_CONVERT */
   N_("Some conversion of line types cannot be performed."),
   /* STID_CANNOT_CONVERT_SEL_LINETYPE */
   N_("Cannot convert the line type for selected objects."),
   /* STID_CANNOT_CONVERT_TO_STRUCT_SPLN */
   N_("Cannot convert selected objects to structures splines."),
   /* STID_ADD_HINGE_VERTEX */
   N_("Add hinge vertex"),
   /* STID_ADD_A_SMOOTH_VERTEX */
   N_("Add smooth vertex (for a hinge vertex)"),
   /* STID_CANNOT_ADD_SMOOTH_VERTEX */
   N_("Cannot add a smooth vertex at this vertex."),
   NULL
};

static char *gaszEnglishStrTable1500[] = {
   /* STID_LEFT_ADD_HINGE_MID_ADD_SMOOTH */
   N_("Drag left mouse button to add a HINGE point and middle mouse button to add a SMOOTH point."),
   /* STID_CLICK_RIGHT_BUTTON_TO_QUIT */
   N_("Click right mouse button to quit."),
   /* STID_SELECTION_CONV_REFUSED */
   N_("X selection conversion was refused (may not be a seerious problem)."),
   /* STID_TRY_PASTE_WITH_OLD_X_MECH */
   N_("Attempting to paste using an older X mechanism (may not be able to paste very large objects with this mechanism)..."),
   /* STID_CANNOT_TOGGLE_FOR_STRUCT_SPLN */
   N_("Cannot toggle Smooth/Hinge points for a structured spline object."),
   /* STID_SEL_2_COMPAT_END_PTS_TO_JOIN */
   N_("Please select 2 end-point vertices from lines/curves of similar types."),
   /* STID_CANNOT_CUT_AT_SMOOTH_PT */
   N_("Cannot cut a structured spline object at a smooth point.  Please select a hinge point and try again."),
   /* STID_RIGHT_MARGIN_IS */
   N_("Simple right margin is %s."),
   /* STID_RIGHT_MARGIN_DISABLED */
   N_("Simple right margin is off."),
   /* STID_ENTER_RIGHT_MARGIN */
   N_("Please enter (simple) right margin '<num> {pixel|cm|in}' (e.g., 1.5 inch, 2 cm, etc.):"),
   /* STID_CURRENT_RIGHT_MARGIN_IS */
   N_("( current right margin: '%s' )"),
   /* STID_ERR_PARSE_THRESH_COLORS */
   N_("Error in parsing threshold for flood fill and replace color: '%s'."),
   /* STID_FLOOD_REPLACE_ENABLED */
   N_("Threshold-based Flood Fill and Replace Color will be used with RGB thresholds of %1d,%1d,%1d (out of 255)."),
   /* STID_FLOOD_REPLACE_DISABLED */
   N_("Threshold-based Flood Fill and Replace Color will not be used."),
   /* STID_SET_FLOOD_REPLACE_THRESH */
   N_("Please enter either a single threshold value (between 0 and 255) to be applied to all color components or thress such values (separted by commas): [current: %1d,%1d,%1d]"),
   /* STID_FLOOD_REPLACE_THRESH_SET_TO */
   N_("Flood Fill and Replace Color threshold has been set to: %1d,%1d,%1d."),
   /* STID_BADPNG_SHORT_HEADER */
   N_("Malformed PNG file.  Cannot read the 8 bytes header."),
   /* STID_BADPNG_BAD_HEADER */
   N_("Malformed PNG file.  Invalid bytes in header."),
   /* STID_BADPNG_BAD_PALETTE_LEN */
   N_("Malformed PNG file.  Palette length is not a multiple of 3."),
   /* STID_BADPNG_BAD_CHUNK_LEN */
   N_("Malformed PNG file.  Invalid chunk length of %1d."),
   /* STID_BADPNG_SHORT_CHUNK_TYPE */
   N_("Malformed PNG file.  Cannot read 4 bytes of chunk type."),
   /* STID_BADPNG_SHORT_CRC */
   N_("Malformed PNG file.  Cannot read 4 bytes CRC."),
   /* STID_BAD_TRANS_COLOR_NO_TRANS */
   N_("Malformed transparent color in a PPM_TRUE object.  Transparency removed."),
   /* STID_CUR_COLOR_IS_TRANS_PIXEL */
   N_("The current color (%s) is the same as the color being used as transparent pixels in a PPM_TRUE object; therefore, it cannot be used for the %s command."),
   /* STID_BAD_CMD_FOR_TRANS_PPMTRUE */
   N_("Selected object is a PPM_TRUE object with a transparent color and cannot be used for running the %s command."),
   /* STID_SEL_ONE_XPM_TRANSPIX */
   N_("Please select at least one X11 Pixmap objects with a transparent pixel."),
   /* STID_CANNOT_FIND_GOOD_TRANSPIX */
   N_("Cannot find a suitable pixel to use as a transparent color pixel.  Abort!"),
   /* STID_CANNOT_SET_XDEF_TRUE_NO_ZLIB */
   N_("Cannot set %s.%s to 'true' because %s is not compile with zlib support.  The value of 'false' is used."),
   /* STID_BAD_GIVEN_LINE_WRITTEN */
   N_("Invalid '%s' line written into file.  This will be reported as a problem writing to the file."),
   /* STID_TWO_XDEF_ONE_IGNORED */
   N_("Conflicting %s.%s and %s.%s (both cannot be defined at the same time).  %s.%s is ignored."),
   /* STID_BAD_ENCODING_TGTWB5_FONT */
   N_("Unsupported font encoding for the '%s' font.  The only support font encodings are \"big5\" and \"gb2312.1980\".  Please specify a valid zh_TW.big5 or zh_CN.euc font using the %s.%s X default."),
   /* STID_TOOL_GB_CHINESE_KEYPAD */
   N_("%s - Simplified Chinese (zh_CN.euc) Keypad"),
   NULL
};

char *TgLoadString(nID)
   int nID;
   /*
    * Note: if ENABLE_NLS is defined, every one of this call will pay
    *       the panelty of gettext().  So, make sure these strings are
    *       not often used (such as error messages).
    * Note: the returned string is translated already.
    */
{
   if (nID >= MINNONCACHEDSTIDS && nID < MAXNONCACHEDSTIDS) {
      int index=(nID-MINNONCACHEDSTIDS);
      int which=(int)(index/100);

      switch (which) {
      case  0: return _(gaszEnglishStrTable[index]);
      case  1: return _(gaszEnglishStrTable100[index-100]);
      case  2: return _(gaszEnglishStrTable200[index-200]);
      case  3: return _(gaszEnglishStrTable300[index-300]);
      case  4: return _(gaszEnglishStrTable400[index-400]);
      case  5: return _(gaszEnglishStrTable500[index-500]);
      case  6: return _(gaszEnglishStrTable600[index-600]);
      case  7: return _(gaszEnglishStrTable700[index-700]);
      case  8: return _(gaszEnglishStrTable800[index-800]);
      case  9: return _(gaszEnglishStrTable900[index-900]);
      case 10: return _(gaszEnglishStrTable1000[index-1000]);
      case 11: return _(gaszEnglishStrTable1100[index-1100]);
      case 12: return _(gaszEnglishStrTable1200[index-1200]);
      case 13: return _(gaszEnglishStrTable1300[index-1300]);
      case 14: return _(gaszEnglishStrTable1400[index-1400]);
      default: return _(gaszEnglishStrTable1500[index-1500]);
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   fprintf(stderr, "Cannot find string table entry with ID=%1d.\n", nID);
#endif /* _TGIF_DBG */

   return NULL;
}

/* ===================== Special LoadString Routines ===================== */

/* --------------------- align.c --------------------- */

static char *distrDirectMenuStr[] = {
   N_("(none)"),
   N_("Distribute selected objects so that their top edges are equally spaced"),
   N_("Distribute selected objects so that their vertical centers are equally spaced"),
   N_("Distribute selected objects so that their bottom edges are equally spaced"),
   N_("Distribute selected objects so that they are equally spaced vertically"),
   N_("Distribute selected objects so that their left edges are equally spaced"),
   N_("Distribute selected objects so that their left and top edges are equally spaced"),
   N_("Distribute selected objects so that their left and vertical centers are equally spaced"),
   N_("Distribute selected objects so that their left and bottom edges are equally spaced"),
   N_("Distribute selected objects so that they are equally spaced vertically and their left edges are spaced equally"),
   N_("Distribute selected objects so that their horizontal centers are equally spaced"),
   N_("Distribute selected objects so that their horizontal centers and top edges are equally spaced"),
   N_("Distribute selected objects so that their horizontal and vertical centers are equally spaced"),
   N_("Distribute selected objects so that their horizontal centers and bottom edges are equally spaced"),
   N_("Distribute selected objects so that they are equally spaced vertically and their horizontal centers are spaced equally"),
   N_("Distribute selected objects so that their right edges are equally spaced"),
   N_("Distribute selected objects so that their right and top edges are equally spaced"),
   N_("Distribute selected objects so that their right and vertical centers are equally spaced"),
   N_("Distribute selected objects so that their right and bottom edges are equally spaced"),
   N_("Distribute selected objects so that they are equally spaced vertically and their right edges are spaced equally"),
   N_("Distribute selected objects so that they are equally spaced horizontally"),
   N_("Distribute selected objects so that they are equally spaced horizontally and their top edges are spaced equally"),
   N_("Distribute selected objects so that they are equally spaced horizontally and their vertical centers are spaced equally"),
   N_("Distribute selected objects so that they are equally spaced horizontally and their bottom edges are spaced equally"),
   N_("Distribute selected objects so that they are equally spaced horizontally and vertically"),
   NULL
};

char *DistrDirectLoadString(nIndex)
   int nIndex;
   /* well, this menu is hard to get to, no need to cache the strings */
{
   return _(distrDirectMenuStr[nIndex]);
}

char *AlignedLoadString(hori_align, vert_align)
   int hori_align, vert_align;
{
   switch ((hori_align<<ALIGN_SHIFT)|vert_align) {
   case ALIGN_NN: break;
   case ALIGN_NT: return _("top sides are aligned.");
   case ALIGN_NM: return _("vertical centers are aligned.");
   case ALIGN_NB: return _("bottom sides are aligned.");
   case ALIGN_NS: break;
   case ALIGN_LN: return _("left sides are aligned.");
   case ALIGN_LT: return _("left and top sides are aligned.");
   case ALIGN_LM: return _("left side and middle are aligned.");
   case ALIGN_LB: return _("left and bottom sides are aligned.");
   case ALIGN_LS: return _("left sides are aligned.");
   case ALIGN_CN: return _("horizontal centers are aligned.");
   case ALIGN_CT: return _("centers and top sides are aligned.");
   case ALIGN_CM: return _("centers and middle are aligned.");
   case ALIGN_CB: return _("centers and bottom sides are aligned.");
   case ALIGN_CS: return _("horizontal centers are aligned.");
   case ALIGN_RN: return _("right sides are aligned.");
   case ALIGN_RT: return _("right and top sides are aligned.");
   case ALIGN_RM: return _("right and middle are aligned.");
   case ALIGN_RB: return _("right and bottom sides are aligned.");
   case ALIGN_RS: return _("right sides are aligned.");
   case ALIGN_SN: break;
   case ALIGN_ST: return _("top sides are aligned.");
   case ALIGN_SM: return _("vertical centers are aligned.");
   case ALIGN_SB: return _("bottom sides are aligned.");
   case ALIGN_SS: break;
   }
   return NULL;
}

char *HoriAlignLoadString(hori_align)
   int hori_align;
{
   switch (hori_align) {
   case ALIGN_N: return _("Horizontal alignment set to NONE.");
   case ALIGN_L: return _("Will align on the LEFT.");
   case ALIGN_C: return _("Will align objects at the horizontal CENTER.");
   case ALIGN_R: return _("Will align on the RIGHT.");
   case ALIGN_S: return _("Will equally SPACE objects horizontally.");
   }
   return NULL;
}

char *VertAlignLoadString(vert_align)
   int vert_align;
{
   switch (vert_align) {
   case ALIGN_N: return _("Vertical alignment set to NONE.");
   case ALIGN_T: return _("Will align at the TOP.");
   case ALIGN_M: return _("Will align objects in the MIDDLE vertically.");
   case ALIGN_B: return _("Will align at the BOTTOM.");
   case ALIGN_S: return _("Will equally SPACE objects vertically.");
   }
   return NULL;
}

static char *alignDirectMenuStr[] = {
   N_("(none)"),
   N_("Align top edges of selected objects"),
   N_("Align vertical centers of selected objects"),
   N_("Align bottom edges of selected objects"),
   N_("Align left edges of selected objects"),
   N_("Align left and top edges of selected objects"),
   N_("Align left edges and vertical centers of selected objects"),
   N_("Align left and bottom edges of selected objects"),
   N_("Align horizontal centers of selected objects"),
   N_("Align horizontal centers and top edges of selected objects"),
   N_("Align horizontal and vertical centers of selected objects"),
   N_("Align horizontal centers and bottom edges of selected objects"),
   N_("Align right edges of selected objects"),
   N_("Align right and top edges of selected objects"),
   N_("Align right edges and vertical centers of selected objects"),
   N_("Align right and bottom edges of selected objects"),
   NULL
};

char *AlignDirectLoadString(nIndex)
   int nIndex;
   /* well, this menu is hard to get to, no need to cache the strings */
{
   return _(alignDirectMenuStr[nIndex]);
}

static char *alignDirectToGridMenuStr[] = {
   N_("(none)"),
   N_("Align top edges of selected objects to the grid"),
   N_("Align vertical centers of selected objects to the grid"),
   N_("Align bottom edges of selected objects to the grid"),
   N_("Align left edges of selected objects to the grid"),
   N_("Align left and top edges of selected objects to the grid"),
   N_("Align left edges and vertical centers of selected objects to the grid"),
   N_("Align left and bottom edges of selected objects to the grid"),
   N_("Align horizontal centers of selected objects to the grid"),
   N_("Align horizontal centers and top edges of selected objects to the grid"),
   N_("Align horizontal and vertical centers of selected objects to the grid"),
   N_("Align horizontal centers and bottom edges of selected objects to the grid"),
   N_("Align right edges of selected objects to the grid"),
   N_("Align right and top edges of selected objects to the grid"),
   N_("Align right edges and vertical centers of selected objects to the grid"),
   N_("Align right and bottom edges of selected objects to the grid"),
   NULL
};

char *AlignDirectToGridLoadString(nIndex)
   int nIndex;
   /* well, this menu is hard to get to, no need to cache the strings */
{
   return _(alignDirectToGridMenuStr[nIndex]);
}

static char *alignDirectToPageMenuStr[] = {
   N_("(none)"),
   N_("Align selected objects (as a group) to the top of the page"),
   N_("Align selected objects (as a group) to the vertical center of the page"),
   N_("Align selected objects (as a group) to the bottom of the page"),
   N_("Align selected objects (as a group) to the left side of the page"),
   N_("Align selected objects (as a group) to the left top corner of the page"),
   N_("Align selected objects (as a group) to the left vertical center of the page"),
   N_("Align selected objects (as a group) to the left bottom of the page"),
   N_("Align selected objects (as a group) to the horizontal center of the page"),
   N_("Align selected objects (as a group) to the top horizontal center of the page"),
   N_("Align selected objects (as a group) to the center of the page"),
   N_("Align selected objects (as a group) to the bottom horizontal center of the page"),
   N_("Align selected objects (as a group) to the right side of the page"),
   N_("Align selected objects (as a group) to the right top corner of the page"),
   N_("Align selected objects (as a group) to the right vertical center of the page"),
   N_("Align selected objects (as a group) to the right bottom corner of the page"),
   NULL
};

char *AlignDirectToPageLoadString(nIndex)
   int nIndex;
   /* well, this menu is hard to get to, no need to cache the strings */
{
   return _(alignDirectToPageMenuStr[nIndex]);
}

static char *alignVerticesDirectToPageMenuStr[] = {
   N_("(none)"),
   N_("Align selected vertices to the top of the page"),
   N_("Align selected vertices to the vertical center of the page"),
   N_("Align selected vertices to the bottom of the page"),
   N_("Align selected vertices to the left side of the page"),
   N_("Align selected vertices to the left top corner of the page"),
   N_("Align selected vertices to the left vertical center of the page"),
   N_("Align selected vertices to the left bottom corner of the page"),
   N_("Align selected vertices to the horizontal center of the page"),
   N_("Align selected vertices to the top horizontal center of the page"),
   N_("Align selected vertices to the center of the page"),
   N_("Align selected vertices to the bottom horizontal center of the page"),
   N_("Align selected vertices to the right side of the page"),
   N_("Align selected vertices to the right top corner of the page"),
   N_("Align selected vertices to the right vertical center of the page"),
   N_("Align selected vertices to the right bottom corner of the page"),
   NULL
};

char *AlignVerticesDirectToPageLoadString(nIndex)
   int nIndex;
   /* well, this menu is hard to get to, no need to cache the strings */
{
   return _(alignVerticesDirectToPageMenuStr[nIndex]);
}

/* --------------------- choice.c --------------------- */

static struct MouseStatusStrRec choiceMouseStatus[] = {
   /*
    * note: the m field is used for the localized strings
    */
   { N_("select/move/resize objects"),    NULL, NULL },
   { N_("enter text"),                    NULL, NULL },
   { N_("draw rectangles"),               NULL, NULL },
   { N_("draw ovals (corner to corner)"), NULL, NULL },
   { N_("draw ovals (center to corner)"), NULL, NULL },
   { N_("draw circles (edge to edge)"),   NULL, NULL },
   { N_("draw poly/open splines"),        NULL, NULL },
   { N_("draw polygon/closed splines"),   NULL, NULL },
   { N_("draw arcs (center first)"),      NULL, NULL },
   { N_("draw arcs (endpoints first)"),   NULL, NULL },
   { N_("draw rcboxes"),                  NULL, NULL },
   { N_("freehand poly/open splines"),    NULL, NULL },
   { N_("select/move vertices"),          NULL, NULL },
   { N_("rotate/shear objects"),          NULL, NULL },
   { NULL, NULL, NULL }
};

static
void CleanUpChoiceMouseStatusStrings()
{
   int choice=0;

   for (choice=0; choice < MAXCHOICES; choice++) {
      UtilFree(choiceMouseStatus[choice].m);
      choiceMouseStatus[choice].m = NULL;
   }
}

void SetCurChoiceMouseStatusStrings(choice, left_is_none, obj_under_mouse,
      cur_text_under_mouse, state)
   int choice, left_is_none, cur_text_under_mouse;
   struct ObjRec *obj_under_mouse;
   unsigned int state;
   /*
    * Note: once such a string is loaded, it will never change.
    *       So, don't put any data-dependent stuff here.
    */
{
   if (choiceMouseStatus[choice].m == NULL) {
      choiceMouseStatus[choice].m = UtilStrDup(_(choiceMouseStatus[choice].l));
      if (choiceMouseStatus[choice].m == NULL) FailAllocMessage();
   }
   if (choice == DRAWPOLY && connectingPortsByWire) {
      if (gstWiringInfo.num_ports_to_connect == 99) {
         /* rename signal_name */
         SetMouseStatus(TgLoadCachedString(CSTID_CLK_ON_PORT_TO_RENAME_SIGNAME),
               TgLoadCachedString(CSTID_ABORT),
               TgLoadCachedString(CSTID_ABORT));
      } else if (gstWiringInfo.num_ports_to_connect == 999) {
         /* clear signal_name */
         SetMouseStatus(TgLoadCachedString(CSTID_CLK_ON_PORT_TO_CLEAR_SIGNAME),
               TgLoadCachedString(CSTID_ABORT),
               TgLoadCachedString(CSTID_ABORT));
      } else {
         SetMouseStatus(TgLoadCachedString(CSTID_START_AN_EDGE_FROM_A_PORT),
               TgLoadCachedString(CSTID_ABORT),
               TgLoadCachedString(CSTID_ABORT));
      }
   } else if (cur_text_under_mouse && (state & ControlMask) == ControlMask) {
      if (obj_under_mouse == NULL) {
         /* if obj_under_mouse is NULL, we are at the boundary of curTextObj */
         SetMouseStatus(TgLoadCachedString(CSTID_MOVE_EDIT_TEXT_BOX),
               TgLoadCachedString(CSTID_MOVE_EDIT_TEXT_BOX),
               TgLoadCachedString(CSTID_CONTEXT_MENU));
      } else {
         SetMouseStatus((left_is_none ?
               TgLoadCachedString(CSTID_PARANED_NONE) :
               choiceMouseStatus[choice].m),
               TgLoadCachedString(CSTID_MOVE_EDIT_TEXT_BOX),
               TgLoadCachedString(CSTID_CONTEXT_MENU));
      }
   } else if (cur_text_under_mouse && obj_under_mouse == NULL) {
      /* if obj_under_mouse is NULL, we are at the boundary of curTextObj */
      SetMouseStatus(TgLoadCachedString(CSTID_MOVE_EDIT_TEXT_BOX),
            TgLoadCachedString(CSTID_MOVE_EDIT_TEXT_BOX),
            TgLoadCachedString(CSTID_CONTEXT_MENU));
   } else {
      SetMouseStatus((left_is_none ? TgLoadCachedString(CSTID_PARANED_NONE) :
            choiceMouseStatus[choice].m),
            (btn2PopupMainMenu ? TgLoadCachedString(CSTID_MAIN_MENU) :
            TgLoadCachedString(CSTID_PARANED_NONE)),
            (obj_under_mouse == NULL ? TgLoadCachedString(CSTID_MODE_MENU) :
            TgLoadCachedString(CSTID_CONTEXT_MENU)));
   }
}

/*
 * note: every other line is used for the localized strings
 */
struct MouseStatusStrRec colorMouseStatus[] = {
   { N_("next color"), N_("Color Menu"), N_("prev color") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec hAlignMouseStatus[] = {
   { N_("next horizontal align"), N_("HoriAlign Menu"), N_("prev horizontal align") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec vAlignMouseStatus[] = {
   { N_("next vertical align"), N_("VertAlign Menu"), N_("prev vertical align") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec pageMouseStatus[] = {
   { N_("next page"), N_("Page Menu"), N_("prev page") },
   { NULL, NULL, NULL },
   { N_("drawing size"), N_("Page Menu"), N_("drawing size") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec pageLayoutMouseStatus[] = {
   { N_("tiled page mode"), N_("PageLayout Menu"), N_("tiled page mode") },
   { NULL, NULL, NULL },
   { N_("stacked page mode"), N_("PageLayout Menu"), N_("stacked page mode") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec justMouseStatus[] = {
   { N_("center justified"), N_("TextStyle Menu"), N_("right justified") },
   { NULL, NULL, NULL },
   { N_("right justified"), N_("TextStyle Menu"), N_("left justified") },
   { NULL, NULL, NULL },
   { N_("left justified"), N_("TextStyle Menu"), N_("center justified") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec fontMouseStatus[] = {
   { N_("next font"), N_("Font Menu"), N_("prev font") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec vspaceMouseStatus[] = {
   { N_("inc text vspace"), N_("modify text vspace"), N_("dec text vspace") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec textSizeMouseStatus[] = {
   { N_("next text size"), N_("TextSize Menu"), N_("prev text size") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec lineWidthMouseStatus[] = {
   { N_("next line width"), N_("LineWidth Menu"), N_("prev line width") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec lineStyleMouseStatus[] = {
   { N_("next line style"), N_("LineStyle Menu"), N_("prev line style") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec lineTypeMouseStatus[] = {
   { N_("next line type"), N_("LineType Menu"), N_("prev line type") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec lineDashMouseStatus[] = {
   { N_("next line dash"), N_("LineDash Menu"), N_("prev line dash") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec rcbRadiusMouseStatus[] = {
   { N_("inc rcbox radius"), N_("modify rcbox radius"), N_("dec rcbox radius") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec zoomMouseStatus[] = {
   { N_("zoom in"), N_("modify zoom"), N_("zoom out") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec moveModeMouseStatus[] = {
   { N_("unconstrained move mode"), N_("MoveMode Menu"), N_("unconstrained move mode") },
   { NULL, NULL, NULL },
   { N_("constrained move mode"), N_("MoveMode Menu"), N_("constrained move mode") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec shapeMouseStatus[] = {
   { N_("(none)"), N_("Shape Menu"), N_("(none)") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec stretchableTextModeMouseStatus[] = {
   { N_("stretchable text mode"), N_("StretchText Menu"), N_("stretchable text mode") },
   { NULL, NULL, NULL },
   { N_("non-stretchable text mode"), N_("StretchText Menu"), N_("non-stretchable text mode") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec fileMouseStatus[] = {
   { N_("landscape"), N_("PageStyle Menu"), N_("landscape") },
   { NULL, NULL, NULL },
   { N_("portrait"), N_("PageStyle Menu"), N_("portrait") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec fillMouseStatus[] = {
   { N_("next fill pattern"), N_("Fill Menu"), N_("prev fill pattern") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec penMouseStatus[] = {
   { N_("next pen pattern"), N_("Pen Menu"), N_("prev pen pattern") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec transPatMouseStatus[] = {
   { N_("transparent pattern mode"), N_("TransPattern Menu"), N_("transparent pattern mode") },
   { NULL, NULL, NULL },
   { N_("opaque pattern mode"), N_("TransPattern Menu"), N_("opaque pattern mode") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};
struct MouseStatusStrRec autoRotatePivotMouseStatus[] = {
   { N_("Enter rotate/shear objects mode"), N_("AutoRotatePivot Submenu"), N_("(none)") },
   { NULL, NULL, NULL },
   { NULL, NULL, NULL }
};

static
void FreeChoiceQuickMouseStatusStrings(pmssr)
   struct MouseStatusStrRec *pmssr;
{
   UtilFree(pmssr->l);
   UtilFree(pmssr->m);
   UtilFree(pmssr->r);
   pmssr->l = pmssr->m = pmssr->r = NULL;
}

static
void CleanUpChoiceQuickMouseStatusStrings()
{
   FreeChoiceQuickMouseStatusStrings(&colorMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&hAlignMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&vAlignMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&pageMouseStatus[(PAGE_STACK<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&pageMouseStatus[(PAGE_TILE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&pageLayoutMouseStatus[(PAGE_STACK<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&pageLayoutMouseStatus[(PAGE_TILE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&justMouseStatus[(JUST_L<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&justMouseStatus[(JUST_C<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&justMouseStatus[(JUST_R<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&fontMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&vspaceMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&textSizeMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&lineWidthMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&lineStyleMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&lineTypeMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&lineDashMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&rcbRadiusMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&zoomMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&moveModeMouseStatus[(CONST_MOVE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&moveModeMouseStatus[(UNCONST_MOVE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&shapeMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&stretchableTextModeMouseStatus[(NO_STRETCHABLE_TEXT<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&stretchableTextModeMouseStatus[(STRETCHABLE_TEXT<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&fileMouseStatus[(PORTRAIT<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&fileMouseStatus[(LANDSCAPE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&fillMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&penMouseStatus[1]);
   FreeChoiceQuickMouseStatusStrings(&transPatMouseStatus[(NO_TRANSPAT_MODE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&transPatMouseStatus[(TRANSPAT_MODE<<1)+1]);
   FreeChoiceQuickMouseStatusStrings(&autoRotatePivotMouseStatus[1]);
}

void QuickSetMouseStatus(pmssr, index)
   struct MouseStatusStrRec *pmssr;
   int index;
{
   if (pmssr[(index<<1)+1].l == NULL) {
      pmssr[(index<<1)+1].l = UtilStrDup(_(pmssr[index<<1].l));
      pmssr[(index<<1)+1].m = UtilStrDup(_(pmssr[index<<1].m));
      pmssr[(index<<1)+1].r = UtilStrDup(_(pmssr[index<<1].r));
      if (pmssr[(index<<1)+1].l == NULL || pmssr[(index<<1)+1].m == NULL ||
            pmssr[(index<<1)+1].r == NULL) {
         FailAllocMessage();
      }
   }
   SetMouseStatus(pmssr[(index<<1)+1].l, pmssr[(index<<1)+1].m,
         pmssr[(index<<1)+1].r);
}

/* --------------------- color.c --------------------- */

static struct MouseStatusStrRec colorTabMouseStatus[] = {
   /*
    * note: the m field is used for the localized strings
    */
   { N_("Shift All Tabs Down"), NULL, NULL },
   { N_("Shift Tabs Down"),     NULL, NULL },
   { N_("Check All"),           NULL, NULL },
   { N_("Uncheck All"),         NULL, NULL },
   { N_("Shift Tabs Up"),       NULL, NULL },
   { N_("Shift All Tabs Up"),   NULL, NULL },
   { NULL, NULL, NULL }
};

static struct MouseStatusStrRec shiftColorTabMouseStatus[] = {
   /*
    * note: the m field is used for the localized strings
    */
   { N_("Shift All Tabs Down"), NULL, NULL },
   { N_("Shift 10 Tabs Down"),  NULL, NULL },
   { N_("Check All"),           NULL, NULL },
   { N_("Uncheck All"),         NULL, NULL },
   { N_("Shift 10 Tabs Up"),    NULL, NULL },
   { N_("Shift All Tabs Up"),   NULL, NULL },
   { NULL, NULL, NULL }
};

static
void CleanUpColorMouseStatusStrings()
{
   int index=0;

   for (index=0; index < 6; index++) {
      UtilFree(colorTabMouseStatus[index].m);
      colorTabMouseStatus[index].m = NULL;
   }
   for (index=0; index < 6; index++) {
      UtilFree(shiftColorTabMouseStatus[index].m);
      shiftColorTabMouseStatus[index].m = NULL;
   }
}

void SetColorMouseStatusStrings(shift_or_cntrl_down, index)
   int shift_or_cntrl_down, index;
   /*
    * Note: once such a string is loaded, it will never change.
    *       So, don't put any data-dependent stuff here.
    */
{
   if (shift_or_cntrl_down) {
      if (shiftColorTabMouseStatus[index].m == NULL) {
         shiftColorTabMouseStatus[index].m =
               UtilStrDup(_(shiftColorTabMouseStatus[index].l));
         if (shiftColorTabMouseStatus[index].m == NULL) FailAllocMessage();
      }
      SetMouseStatus(shiftColorTabMouseStatus[index].m,
            TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE));
   } else {
      if (colorTabMouseStatus[index].m == NULL) {
         colorTabMouseStatus[index].m =
               UtilStrDup(_(colorTabMouseStatus[index].l));
         if (colorTabMouseStatus[index].m == NULL) FailAllocMessage();
      }
      SetMouseStatus(colorTabMouseStatus[index].m,
            TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE));
   }
}

/* --------------------- cutpaste.c --------------------- */

struct PropInfoRec {
   long bit;
   char *localized;
   char *desc;
};
static struct PropInfoRec gstPropStringTable[] = {
   /*
    * note: the localized field is used for the localized strings
    */
   { PROP_MASK_AH,           NULL, N_("arrow height") },
   { PROP_MASK_AW,           NULL, N_("arrow width") },
   { PROP_MASK_ARROW_STYLE,  NULL, N_("arrow style") },
   { PROP_MASK_COLOR,        NULL, N_("color") },
   { PROP_MASK_DASH,         NULL, N_("dash") },
   { PROP_MASK_WIDTH,        NULL, N_("line width") },
   { PROP_MASK_TRANSPAT,     NULL, N_("pattern transparency") },
   { PROP_MASK_FILL,         NULL, N_("fill") },
   { PROP_MASK_PEN,          NULL, N_("pen") },
   { PROP_MASK_RCB_RADIUS,   NULL, N_("rcbox radius") },
   { PROP_MASK_CURVED,       NULL, N_("spline") },
   { PROP_MASK_TEXT_FONT,    NULL, N_("text font") },
   { PROP_MASK_TEXT_STYLE,   NULL, N_("text style (no use, part of text font)") },
   { PROP_MASK_TEXT_JUST,    NULL, N_("text justification") },
   { PROP_MASK_TEXT_SZ_UNIT, NULL, N_("text size") },
   { PROP_MASK_VSPACE,       NULL, N_("text vertical spacing") },
   { PROP_MASK_UNDERLINE_ON, NULL, N_("text underline") },
   { PROP_MASK_OVERLINE_ON,  NULL, N_("text overline") },
   { PROP_MASK_CTM,          NULL, N_("transformation matrix") },
   { PROP_MASK_WIDTH_INDEX,  NULL, N_("line width index") },
   { 0L, NULL, NULL }
};

static
void CleanUpPropertiesInfo()
{
   struct PropInfoRec *ppir=NULL;

   for (ppir=gstPropStringTable; ppir->bit != 0L; ppir++) {
      UtilFree(ppir->localized);
      ppir->localized = NULL;
   }
}

char *PropLoadString(lWhich)
   long lWhich;
   /*
    * Note: once such a string is loaded, it will never change.
    *       So, don't put any data-dependent stuff here.
    */
{
   struct PropInfoRec *ppir=NULL;

   for (ppir=gstPropStringTable; ppir->bit != 0L; ppir++) {
      if (ppir->bit == lWhich) {
         if (ppir->localized == NULL) {
            ppir->localized = UtilStrDup(_(ppir->desc));
            if (ppir->localized == NULL) FailAllocMessage();
         }
         return ppir->localized;
      }
   }
#ifdef _TGIF_DBG /* debug, do not translate */
   fprintf(stderr,
         "Cannot find properties string table entry with ID=0x%08lx.\n",
         lWhich);
#endif /* _TGIF_DBG */

   return NULL;
}

/* --------------------- grid.c --------------------- */

static struct MouseStatusStrRec exportFormatMouseStatus[] = {
   /*
    * note: the m field is used for the localized strings
    */
   { N_("Set printing mode to print to a printer"),              NULL, NULL },
   { N_("Set printing mode to generate EPS files"),              NULL, NULL },
   { N_("Set printing mode to generate PS files"),               NULL, NULL },
   { N_("Set printing mode to generate X11 Bitmap files"),       NULL, NULL },
   { N_("Set printing mode to generate X11 Pixmap files"),       NULL, NULL },
   { N_("Set printing mode to generate ASCII files"),            NULL, NULL },
   { N_("Set printing mode to generate EPSI files"),             NULL, NULL },
   { N_("Set printing mode to generate GIF/ISMAP files"),        NULL, NULL },
   { N_("Set printing mode to generate HTML files"),             NULL, NULL },
   { N_("Set printing mode to generate PDF files"),              NULL, NULL },
   { N_("Set printing mode to generate DOS/Windows EPSI files"), NULL, NULL },
   { N_("Set printing mode to generate PNG files"),              NULL, NULL },
   { N_("Set printing mode to generate JPEG files"),             NULL, NULL },
   { N_("Set printing mode to generate PBM files"),              NULL, NULL },
   { N_("Set printing mode to generate PPM files"),              NULL, NULL },
   { N_("Set printing mode to generate NetList files"),          NULL, NULL },
   { N_("Set printing mode to generate SVG files"),              NULL, NULL },
   { N_("Set printing mode to generate %s files"),               NULL, NULL },
   { NULL, NULL, NULL }
};

static
void CleanUpExportFormatMouseStatusStrings()
{
   int index=0;

   for (index=0; index <= MAXDEFWHERETOPRINT+1; index++) {
      UtilFree(exportFormatMouseStatus[index].m);
      exportFormatMouseStatus[index].m = NULL;
   }
}

char *ExportFormatMenuLoadString(where_to_print, color_dump)
   int where_to_print, color_dump;
   /* returned string has been translated */
{
   if (where_to_print == XBM_FILE) {
      if (color_dump) {
         if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
            exportFormatMouseStatus[where_to_print+1].m =
                  UtilStrDup(_(exportFormatMouseStatus[where_to_print+1].l));
            if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
               FailAllocMessage();
            }
         }
         return (exportFormatMouseStatus[where_to_print+1].m);
      } else {
         if (exportFormatMouseStatus[where_to_print].m == NULL) {
            exportFormatMouseStatus[where_to_print].m =
                  UtilStrDup(_(exportFormatMouseStatus[where_to_print].l));
            if (exportFormatMouseStatus[where_to_print].m == NULL) {
               FailAllocMessage();
            }
         }
         return (exportFormatMouseStatus[where_to_print].m);
      }
   } else if (where_to_print == PPM_FILE) {
      if (color_dump) {
         if (exportFormatMouseStatus[where_to_print+2].m == NULL) {
            exportFormatMouseStatus[where_to_print+2].m =
                  UtilStrDup(_(exportFormatMouseStatus[where_to_print+2].l));
            if (exportFormatMouseStatus[where_to_print+2].m == NULL) {
               FailAllocMessage();
            }
         }
         return (exportFormatMouseStatus[where_to_print+2].m);
      } else {
         if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
            exportFormatMouseStatus[where_to_print+1].m =
                  UtilStrDup(_(exportFormatMouseStatus[where_to_print+1].l));
            if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
               FailAllocMessage();
            }
         }
         return (exportFormatMouseStatus[where_to_print+1].m);
      }
   } else if (where_to_print < XBM_FILE) {
      if (exportFormatMouseStatus[where_to_print].m == NULL) {
         exportFormatMouseStatus[where_to_print].m =
               UtilStrDup(_(exportFormatMouseStatus[where_to_print].l));
         if (exportFormatMouseStatus[where_to_print].m == NULL) {
            FailAllocMessage();
         }
      }
      return (exportFormatMouseStatus[where_to_print].m);
   } else if (where_to_print < PPM_FILE) {
      if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
         exportFormatMouseStatus[where_to_print+1].m =
               UtilStrDup(_(exportFormatMouseStatus[where_to_print+1].l));
         if (exportFormatMouseStatus[where_to_print+1].m == NULL) {
            FailAllocMessage();
         }
      }
      return (exportFormatMouseStatus[where_to_print+1].m);
   } else if (where_to_print < MAXDEFWHERETOPRINT) {
      if (exportFormatMouseStatus[where_to_print+2].m == NULL) {
         exportFormatMouseStatus[where_to_print+2].m =
               UtilStrDup(_(exportFormatMouseStatus[where_to_print+2].l));
         if (exportFormatMouseStatus[where_to_print+2].m == NULL) {
            FailAllocMessage();
         }
      }
      return (exportFormatMouseStatus[where_to_print+2].m);
   }
   if (exportFormatMouseStatus[MAXDEFWHERETOPRINT+2].m == NULL) {
      exportFormatMouseStatus[MAXDEFWHERETOPRINT+2].m =
            UtilStrDup(_(exportFormatMouseStatus[MAXDEFWHERETOPRINT+2].l));
      if (exportFormatMouseStatus[MAXDEFWHERETOPRINT+2].m == NULL) {
         FailAllocMessage();
      }
   }
   return (exportFormatMouseStatus[MAXDEFWHERETOPRINT+2].m);
}

/* ===================== Init & CleanUp Routines ===================== */

void SetMouseStatusToAllNone()
{
   SetMouseStatus(TgLoadCachedString(CSTID_PARANED_NONE),
         TgLoadCachedString(CSTID_PARANED_NONE),
         TgLoadCachedString(CSTID_PARANED_NONE));
}

void CleanUpStrTable()
{
   int i=0;

   for (i=0; i < MAXCACHEDSTIDS; i++) {
      if (gaszCachedStrTable[i] != NULL) UtilFree(gaszCachedStrTable[i]);
   }
   for (i=0; i < MAXNONCACHEDSTIDS; i++) {
      if (gaszStrTable[i] != NULL) UtilFree(gaszStrTable[i]);
   }
   memset(gaszCachedStrTable, 0, sizeof(gaszCachedStrTable));
   memset(gaszStrTable, 0, sizeof(gaszStrTable));

   CleanUpChoiceMouseStatusStrings();
   CleanUpChoiceQuickMouseStatusStrings();

   CleanUpColorMouseStatusStrings();
   CleanUpPropertiesInfo();
   CleanUpExportFormatMouseStatusStrings();
}

void InitStrTable()
{
   memset(gaszStrTable, 0, sizeof(gaszStrTable));
}

