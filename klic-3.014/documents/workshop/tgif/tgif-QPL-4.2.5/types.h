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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/types.h,v 1.47 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGIF_TYPES_H_
#define _TGIF_TYPES_H_

#include "const.h"

typedef void *(NLFN)ARGS_DECL((void)); /* a NULL function template */
typedef void (VDFN)ARGS_DECL((void)); /* a void procedure template */

#define DO_ALL_X_EV 0
#define EXPOSE_AND_ESC_X_EV_ONLY 1

typedef int (AbortCallbackFunc)ARGS_DECL((void*)); /* returns TRUE for abort */

typedef struct BBRec {
   int	ltx, lty, rbx, rby;
} * BBRecPtr;

typedef struct PtRec {
   int	x, y;
   struct PtRec	* next;
} * PtRecPtr;

typedef struct {
   int	x, y;
} IntPoint;

typedef struct tagIntPointTriplet {
   int earlier_valid, later_valid;
   IntPoint hinge_pt;
   IntPoint earlier_smooth_pt;
   IntPoint later_smooth_pt; /* this point moves at first */
   double ratio;  /* segment len earlier = (segment len later * ratio) */
} IntPointTriplet;

typedef struct {
   double	x, y;
} DoublePoint;

typedef struct XfrmMtrxRec {
   double	m[4];
   int		t[2];
} * XfrmMtrxPtr;

typedef struct ObjRec {
   int			x, y, type, color, bg_color, id, dirty, hot_spot;
   int			invisible, trans_pat;
   int			rotation; /* degrees times 64 */
   short		marked, locked;
   struct BBRec		orig_obbox, obbox; /* object bounding box */
   struct BBRec		bbox; /* real bounding box */
   struct ObjRec	* next, * prev;
   struct AttrRec	* fattr, * lattr; /* first and last attributes */
   union {
      struct GroupRec	* r;
      struct PolyRec	* p;
      struct PolygonRec	* g;
      struct BoxRec	* b;
      struct OvalRec	* o;
      struct TextRec	* t;
      struct ArcRec	* a;
      struct RCBoxRec	* rcb;
      struct XBmRec	* xbm;
      struct XPmRec	* xpm;
      struct SSRec	* ss;
   } detail;
   struct ObjRec	* tmp_child; /* used temporarily */
   struct ObjRec	* tmp_parent; /* used temporarily */
   struct XfrmMtrxRec	* ctm;
   struct XfrmMtrxRec	* orig_ctm; /* not used, yet */
   char			color_str[40]; /* copy of the color name in file */
   char			bg_color_str[40]; /* copy of the color name in file */
   XPoint		rotated_obbox[5]; /* offsets */
   void			* userdata;
   /* for the multicast whiteboard */
   char	*creator_full_id; /* NULL means created locally */
   int	unavailable; /* involved in a long transaction of some sort */
} * ObjRecPtr;

typedef struct DynStrRec {
   char	*s;
   int	sz; /* size of the buffer, should be strlen(s)+1 */
} * DynStrPtr;

typedef struct SSRec {
   struct DynStrRec dyn_str;
   int double_byte, double_byte_mod_bytes, double_byte_vertical;
} SimpleString;

typedef struct AttrRec {
   struct DynStrRec	attr_name; /* attribute name */
   struct DynStrRec	attr_value; /* attribute value */
   short		shown; /* TRUE if the attribute is shown */
   short		nameshown; /* TRUE if the attr name is also shown */
   short		inherited; /* TRUE if attr was inherited */
   struct ObjRec	* obj; /* the OBJ_TEXT object that represent the attr */
   struct ObjRec        * owner; /* ptr to owner obj of the record */
   struct AttrRec	* next, * prev; /* next and prev attributes */
   void			* userdata;
   int			tgo_type; /* default to be TGO_STRING */
   void			* tgo_data;
} * AttrRecPtr;

/*
 * Pins are not implemented yet!
 */
typedef struct ConnRec {
   struct ObjRec	* poly_obj;
   int			at_start; /* TRUE if connect to start_conn of poly */
   struct ObjRec	* pin_obj;
   struct ConnRec	* next, * prev;
} * ConnRecPtr;

typedef struct GroupRec {
   struct ObjRec	* first, * last;
   char			s[MAXPATHLENGTH+1];
   int			rotate, flip, deck_index, pin_connected;
   			/*
   			 * For a pin object, if !pin_connected, first is used.
			 * If pin_connected, last is used.  This means that
			 * first is the un-connected view of a pin object
			 * and last is the connected view of a pin object.
			 *
			 * Please note that pins are not implemented yet!
   			 */
   struct ConnRec	* first_conn, * last_conn;
} * GroupRecPtr;

typedef struct ArrowStyleRec {
   int			arrow_style;
   int			aw, ah, aindent;
   char			aw_spec[40], ah_spec[40], aindent_spec[40];
} * ArrowStyleRecPtr;

typedef struct PolyRec {
   int			n;	/* number of points in the polyline */
   IntPoint		* vlist; /* absolute */
   char			* smooth;
   int			ssn;	/* number of points in a structured spline */
   IntPoint		* ssvlist; /* absolute */
   char			* ssmooth;
   int			sn;	/* number of points in the spline polyline */
   XPoint		* svlist; /* offsets */
   int			asn;	/* spline polyline with arrows */
   XPoint		* asvlist; /* offsets */
   int			intn;	/* interpolated spline original spec */
   IntPoint		* intvlist; /* absolute */
   int			style, width, pen, curved, fill, dash, tension;
   char			width_spec[40];
   int			rotated_n, rotated_asn;
   XPoint		* rotated_vlist, * rotated_asvlist; /* offsets */
   struct ArrowStyleRec	start_asty, end_asty;
   			/*
   			 * Please note that pins are not implemented yet!
   			 */
   struct ConnRec	* start_conn; /* connection at vlist[0] */
   struct ConnRec	* end_conn; /* connection at vlist[n-1] */
   			/*
   			 * To be obsoleted!
   			 */
   int			aw, ah;
   char			aw_spec[40], ah_spec[40];
} * PolyRecPtr;

typedef struct PolygonRec {
   int			n;	/* number of points in the polygon */
   IntPoint		* vlist; /* absolute */
   char			* smooth;
   int			ssn;	/* number of points in a structured spline */
   IntPoint		* ssvlist; /* absolute */
   char			* ssmooth;
   int			sn;	/* number of points in the spline polygon */
   XPoint		* svlist; /* offsets */
   int			intn;	/* interpolated spline original spec */
   IntPoint		* intvlist; /* absolute */
   int			fill, width, pen, curved, dash, tension;
   char			width_spec[40];
   int			rotated_n;
   XPoint		* rotated_vlist; /* offsets */
} * PolygonRecPtr;

typedef struct BoxRec {
   int		fill, width, pen, dash;
   char		width_spec[40];
} * BoxRecPtr;

typedef struct OvalRec {
   int		fill, width, pen, dash;
   char		width_spec[40];
   int		rotated_n;
   XPoint	* rotated_vlist; /* offsets */
} * OvalRecPtr;

typedef struct KeyValRec {
   char *key;
   char *value;
   struct KeyValRec *next, *prev;
} KeyValInfo;

typedef struct StrRec {
   struct DynStrRec	dyn_str;
   struct StrRec	* next, * prev;
} * StrRecPtr;

typedef struct TextExtentsRec {
   char	*buf;
   int	len; /* strlen(buf) */
   int	w, lbearing, rextra; /* lbearing is always <= 0, rextra >= 0 */
   int	bbox_w, bbox_h, asc, des;
} TextExtentsInfo;

typedef struct RecalcMetricsRec {
   int	orig_x, orig_baseline_y;
   int	x, baseline_y;
   int	depth, pre_order;
} RecalcMetricsInfo;

typedef struct StrSegRec {
   int	color;
   char	color_str[40]; /* copy of the color name in file */
   int	font, style, sz_unit;
   int	real_sz_unit; /* only used if Tgif.EditTextSize is used */
   int	double_byte, double_byte_mod_bytes, double_byte_vertical, direction;
   int	underline_on, overline_on;
   int	w, asc, des, min_lbearing, max_rextra;
   int	read_only, orig_w, orig_h, orig_asc, orig_des; /* only if read_only */
   char	* font_name; /* only used in PRTGIF */
   struct DynStrRec	dyn_str;
   struct StrBlockRec	* owner;
   /*
    * for file version 38!
    */
   int	dontreencode;
} StrSegInfo;

#define SB_SIMPLE        0
#define SB_SUPSUB_LEFT   1
#define SB_SUPSUB_CENTER 2
#define SB_SUPSUB_RIGHT  3
#define SB_CHAR_SPACE    4 /* special_char_w is pixel offsets */

typedef struct StrBlockRec {
   int	w, asc, des, min_lbearing, max_rextra;
   int	read_only, orig_w, orig_h, orig_asc, orig_des; /* only if read_only */
   int	special_char_w;
   int	depth, pre_order; /* set in RecalcTextMetrics() */
   int	type; /* one of SB_* */
   StrSegInfo	*seg;
   struct MiniLinesRec	*sup, *sub;
   struct MiniLineRec	*owner_mini_line;
   struct StrBlockRec	*next, *prev;
   /*
    * used to determine what to repaint
    */
   int clean;
   struct BBRec clean_bbox, bbox;
} StrBlockInfo;

typedef struct MiniLineRec {
   int	w, asc, des, min_lbearing, max_rextra;
   int	read_only, orig_w, orig_h, orig_asc, orig_des; /* only if read_only */
   /*
    * For the first mini_line, v_gap is always 0.
    * For other mini_line, v_gap is relative to the mini_line
    *       immediately above it (after asc and des of the mini_line,
    *       and v_space of the minilines).
    */
   int	v_gap;
   struct StrBlockRec	*first_block, *last_block;
   struct MiniLineRec	*next, *prev;
   struct MiniLinesRec	*owner_minilines;
   /*
    * used to determine what to repaint
    */
   int clean;
   struct BBRec clean_bbox, bbox;
} MiniLineInfo;

typedef struct MiniLinesRec {
   int	w, h, min_lbearing, max_rextra;
   int	read_only, orig_w, orig_h, orig_asc, orig_des; /* only if read_only */
   int	just, v_space;
   /*
    * For the minilines of a text_ptr, baseline_offset always 0.
    * For the sup and sub of a str_block, baseline_offset is relative to
    *       the baseline of the str_block, which is the baseline of the
    *       mini_line which owns the str_block.  This offsets the mini_lines
    *       in it all together.
    */
   int	baseline_offset;
   /*
    * owner_block is NULL for the top minilines;
    */
   struct StrBlockRec	*owner_block;
   struct MiniLineRec	*first, *last;
   /*
    * used to determine what to repaint
    */
   int clean;
   struct BBRec clean_bbox, bbox;
} MiniLinesInfo;

typedef struct TextRec {
   /*
    * The height of a line is determined by the font height of the first
    * substr.  V_space is also the v_space of the first substr.
    */
   int			lines, pen, fill;
   int			w, h, min_lbearing, max_rextra;
   struct AttrRec       *attr; /* ptr to attr record if text obj is an attr */
   Pixmap		cached_bitmap, cached_pixmap, cached_bg_bitmap;
   int			cached_zoom, cached_zoomed;
   struct XfrmMtrxRec	cached_ctm;
   struct BBRec		orig_bbox;
   int			underline_y_offset, overline_y_offset;
   /* these are new */
   /*
    * baseline_y is the real thing!  y of struct ObjRec is derived from it
    *       by subtracting minilines->asc
    */
   int	baseline_y;
   int	read_only, orig_w, orig_h, orig_asc, orig_des; /* only if read_only */
   struct MiniLinesRec	minilines;
} * TextRecPtr;

typedef struct tagStretchStructuredSplineInfo {
   int hinge; /* TRUE if a hinge point is selected, FALSE otherwise */
   int orig_hinge_index; /* this index into vlist */
   int earlier_smooth_selected;
   int prev_valid; /* FALSE if the first point of a poly is selected */
   int next_valid; /* FALSE if the last point of a poly is selected */
   IntPointTriplet ipt_prev, ipt, ipt_next;
   /* for drawing */
   int num_vs, num_vs2;
   IntPoint vs[5], vs2[5];
   char smooth[5], smooth2[5];
   XPoint *sv, *sv2, *saved_sv, *saved_sv2;
   int sn, sn2, saved_sn, saved_sn2;
   /* for moving a vertex -- only one vertex can be selected */
   int orig_abs_x, orig_abs_y, new_abs_x, new_abs_y, abs_dx, abs_dy;
   int rotated_orig_abs_x, rotated_orig_abs_y;
   int rotated_new_abs_x, rotated_new_abs_y;
   int rotated_abs_dx, rotated_abs_dy;
} StretchStructuredSplineInfo;

typedef struct SelRec {
   struct ObjRec	* obj;
   struct SelRec	* next, * prev;
} * SelRecPtr;

typedef struct tagMoveVertexInfo {
   int orig_abs_x, orig_abs_y, new_abs_x, new_abs_y, abs_dx, abs_dy;
   int rotated_orig_abs_x, rotated_orig_abs_y;
   int rotated_new_abs_x, rotated_new_abs_y;
   int rotated_abs_dx, rotated_abs_dy;
} MoveVertexInfo;

typedef struct VSelRec {
   struct ObjRec		* obj;
   int				n, max_v, * v_index, * x, * y;
   struct VSelRec		* next, * prev;
   /* for moving a vertex -- normally pmvi is NULL */
   MoveVertexInfo		* pmvi;
} * VSelRecPtr;

typedef struct MoveSubCmdRec {
   int	dx, dy;
} * MoveSubCmdRecPtr;

typedef struct tagTextFormatInfo {
   int color_index, font_style;
   char color_str[40];
} TextFormatInfo;

#define CHAT_STATE_NORMAL     0
#define CHAT_STATE_HELLO      1
#define CHAT_STATE_DISCONNECT 2

typedef struct ChatSubCmdRec {
   int type;
   TextFormatInfo tfi;
   char nick_name[MAXSTRING];
   char encoding[MAXSTRING];
   char *buf;
} * ChatSubCmdRecPtr;

typedef struct SlideShowSubCmdRec {
   int into_slideshow; /* TRUE if going into slideshow mode */
} * SlideShowSubCmdRecPtr;

typedef struct ClearAllSubCmdRec {
   int page_style;
   float print_mag;
} * ClearAllSubCmdRecPtr;

typedef struct SubCmdRec {
   union {
      struct MoveSubCmdRec	move;
      struct ChatSubCmdRec	chat;
      struct SlideShowSubCmdRec	slideshow;
      struct ClearAllSubCmdRec	clearall;
   } detail;
} * SubCmdRecPtr;

typedef struct CmdRec {
   int			type, undone;
   int			include_tgif_obj, new_colormap, logical_clock;
   char			* sender_process_id;
   int			* pos_before, count_before, pos_before_has_ids;
   int			* pos_after, count_after;
   struct SelRec	* top_before, * bot_before;
   struct SelRec	* top_after, * bot_after;
   struct CmdRec	* next, * prev;
   struct CmdRec	* first, * last;
   /*
    * if serialized is TRUE, pos_before should be typecasted to (char**)
    *       and each entry is <creator_full_id> which is "id", "/",
    *       "creator_pid", ":", and "creator_ip" concatenated together.
    */
   int			serialized;
   char			* serialization_buf;
   int			first_redo_after_deserialize;
   int			skipped;
   struct SubCmdRec	* subcmd;
} * CmdRecPtr;

typedef struct PageRec {
   struct ObjRec	* top, * bot;
   struct PageRec	* next, * prev;
   char			* name;
   char			* page_file_name;
   int			draw_orig_x, draw_orig_y, zoom_scale, zoomed_in;
   int			layer_on;
} * PageRecPtr;

typedef struct StkRec {
   struct ObjRec	* first, * last, * sel;
   struct StkRec	* next;
   struct CmdRec	* first_cmd, * last_cmd, * cur_cmd;
   int			history_count;
   int			name_valid, file_mod, id, page_style;
   int			orig_x, orig_y, zoom, zoomed;
   int			grid_system, english_grid, metric_grid, snap_on;
   int			color, h_align, v_align, line_w, line_s;
   int			fill, pen, dash, just, font, f_style, f_sz_unit;
   int			trans_pat;
   float		print_mag;
   int			v_space, grid_shown, move_mode;
   int			rcb_radius, underline_on, underline_y_offset;
   int			overline_on, overline_y_offset;
   char			name[MAXPATHLENGTH+1], domain[MAXPATHLENGTH+1];
   char			dir[MAXPATHLENGTH+1], sym_dir[MAXPATHLENGTH+1];
   char			* saved_comments;
   int			saved_comments_len;
   struct AttrRec	* first_file_attr, * last_file_attr;
   struct PageRec	* first_page, * last_page, * cur_page;
   int			cur_page_num, last_page_num, cols, rows;
   int			page_layout_mode, color_dump;
   int			one_page_width, one_page_height;
   int			file_bg_pixel, file_fg_pixel;
   int			file_bg_pixmap_w, file_bg_pixmap_h;
   char			*file_bg_color_str, *file_fg_color_str;
   char			*file_bg_gif_url;
   Pixmap		file_bg_pixmap;
} * StkRecPtr;

typedef struct ArcRec {
   int		fill, width, pen, dash, style, aw, ah;
   char		width_spec[40], aw_spec[40], ah_spec[40];
   int		xc, yc, x1, y1, x2, y2, dir;
   int		ltx, lty, w, h, angle1, angle2;
   int		a_angle1, a_angle2;
   int		rotated_n, rotated_asn;
   XPoint	* rotated_vlist, * rotated_asvlist; /* offsets */
} * ArcRecPtr;

typedef struct RCBoxRec {
   int		fill, width, pen, dash, radius;
   char		width_spec[40];
   int		rotated_n;
   XPoint	* rotated_vlist; /* offsets */
} * RCBoxRecPtr;

typedef struct XBmRec {
   int			fill, real_type, flip, image_w, image_h;
   int			eps_w, eps_h;
   char			* data;
   char			* filename;
   char			* * epsflines;
   char			write_date[32];
   int			num_epsf_lines, epsf_level, save_epsf;
   int			llx, lly, urx, ury; /* these values are x1000 */
   XImage		* image;
   Pixmap		bitmap;
   Pixmap		cached_bitmap;
   int			cached_zoom, cached_zoomed, cached_flip;
   int			cached_w, cached_h;
   struct XfrmMtrxRec	cached_ctm;
} * XBmRecPtr;

typedef struct XPmRec {
   int			fill, real_type, flip, image_w, image_h;
   int			linked_jpeg; /* TRUE if JPEG is linked */
   char			* filename; /* only valid if JPEG is linked */
   char			tmp_ppm6_fname[MAXPATHLENGTH+1]; /* used temporarily */
   int			ncolors;
   int			chars_per_pixel;
   int			first_pixel_is_bg;
   int			* pixels, * red, * green, * blue;
   char			* color_char;
   char			* * color_str;
   char			* data;
   XImage		* image, * bitmap_image;
   Pixmap		pixmap, bitmap;
   Pixmap		cached_pixmap, cached_bitmap;
   int			cached_zoom, cached_zoomed, cached_flip;
   int			cached_w, cached_h, cached_color;
   Pixmap		clip_mask;
   struct XfrmMtrxRec	cached_ctm;
   /*
    * If real_type is PPM_TRUE, color_char, color_str, pixels, and data are
    *         all NULL and ncolors is 0.
    * The following are only valid if real_type is PPM_TRUE.
    */
   int			ppm_data_compress; /* 0x1: jpeg, 0x2: ppm.z */
   char			* ppm_data; /* can be raw (jpeg) or deflated (ppm.z) */
   unsigned int		ppm_data_size;
   char			* ppm_mask_data; /* not used, for future transparency */
   unsigned int		ppm_mask_size; /* not used, for future transparency */
   int			has_transparent_color;
   unsigned char	transparent_color[3]; /* r, g, b */
   void			* userdata;
} * XPmRecPtr;

typedef struct WinInfoRec {
   Window	window;
   int		mapped;
   int		raise;
   int		(*ev_handler)ARGS_DECL((XEvent*, struct WinInfoRec *));
   void		(*expose_handler)ARGS_DECL((XEvent*, struct WinInfoRec *));
   void		(*cleanup)ARGS_DECL((struct WinInfoRec *));
   void		*userdata;
} * WinInfoPtr;

typedef struct tagKeyOrBtnEvInfo {
   int		type;
   Bool		send_event;
   Display	*display;
   Window	window;
   Time		time;
   int		x, y;
   unsigned int	keycode_or_button;
   unsigned int	state;
} KeyOrBtnEvInfo;

typedef struct MtrxRec {
   float	m[2][2], rev_m[2][2], h_scale, v_scale;
   float	image_w, image_h, w, h, transformed_w, transformed_h;
   float	dump_h_scale, dump_v_scale;
   int		rotate, flip, degree;
} * MtrxPtr;

typedef struct _DspList {
   char			itemstr[MAXPATHLENGTH+1];
   char			pathstr[MAXPATHLENGTH+1];
   int			directory;
   struct _DspList	* next;
} DspList;

typedef struct MouseStatusStrRec {
   char	* l, * m, * r;
} MouseStatusStrPtr;

typedef struct tagMouseOverStatusInfo {
   char btn_str[MAX_STATUS_BTNS][MAXSTRING+1];
   char one_line_str[MAXSTRING+1];
   int one_line_status;
   struct tagMouseOverStatusInfo *next;
} MouseOverStatusInfo;

typedef struct VRec {
   int vtype;
   union {
      int i;
      double d;
      char *s;
   } val;
} VRecPtr;

struct URLCacheRec {
   int remote_buf_sz, is_html;
   char *remote_buf, *content_type, *simple_url_name;
   struct URLCacheRec *next, *prev;
};

typedef struct PushedFontRec {
   int font;
   int sz_unit;
   int style;
   int just;
   int pen;
   int fill;
   int trans_pat;
   int v_space;
   int color;
   int underline_on;
   int underline_y_offset;
   int overline_on;
   int overline_y_offset;
   int double_byte, double_byte_mod_bytes, double_byte_vertical;
   struct PushedFontRec *next;
} * PushedFontRecPtr;

typedef struct ObjListRec {
   struct ObjRec *top_obj, *bot_obj;
   struct SelRec *top_sel, *bot_sel;
   int count;
} ObjListInfo;

typedef struct tagSimpleWinInfo {
   int x, y, w, h;
} SimpleWinInfo;

typedef struct tagDirtyBBoxInfo {
   int valid;
   int force_redraw_all;
   struct BBRec bbox;
} DirtyBBoxInfo;

typedef struct tagTextHighlightInfo {
   /* all these values are screen offsets */
   int start_x, start_min_y, start_max_y, start_baseline_y;
   int end_x, end_min_y, end_max_y, end_baseline_y;
   StrBlockInfo *start_str_block_ptr, *end_str_block_ptr;
   int start_index, end_index;
   int highlighting; /* used in PaintMiniLines() */
   int mode; /* one of PAINT_* above */
} TextHighlightInfo;

typedef struct tagCopyUTF8Info {
   int single_byte_valid, double_byte_valid;
   StrSegInfo single_byte_seg;
   StrSegInfo double_byte_seg;
} CopyUTF8Info;

typedef struct tagDrawTextInfo {
   /* the following is from miniline.e */

   TextHighlightInfo text_highlight_info;

   int text_orig_x, text_orig_y, text_orig_baseline_y;
   int text_abs_x, text_abs_y, text_abs_baseline_y;

   int text_cur_x, text_cur_y, text_cur_baseline_y;
   int text_cur_index;
   int text_end_x, text_end_y, text_end_baseline_y;
   int text_end_index;

   int text_abs_min_lbearing, text_abs_max_rextra;
   int text_w, text_h;

   int text_highlight;

   struct ObjRec *cur_text_obj;

   struct BBRec cur_text_obbox, cur_text_bbox;

   StrBlockInfo *cur_str_block;
   StrBlockInfo *end_str_block;
   MiniLineInfo *first_miniLine, *last_miniLine;

   int esc_pressed;

   int cur_text_outline_w, cur_text_outline_half_w;

   /* the following is from text.e */

   int text_drawn;
   int cur_text_modified;
   int text_vspace;

   int text_just;
   int text_cursor_shown;
   int text_cursor_h;
   int editing_text;
   int cur_text_is_new;

   int edit_text_size;

   /* the following is from struct PushedFontRec */

   struct PushedFontRec font_info;
} DrawTextInfo;

typedef struct tagColorBytes {
   int valid;
   unsigned char r, g, b;
} ColorBytes;

typedef struct tagTrueColorInfo {
   unsigned long r_mask, g_mask, b_mask;
   unsigned int r_shift, g_shift, b_shift;
   double dr_maxval, dg_maxval, db_maxval;
   /* dr_maxval_div255 = dr_maxval/255, etc. */
   double dr_maxval_div255, dg_maxval_div255, db_maxval_div255;
   unsigned int num_r_bits, num_g_bits, num_b_bits;
} TrueColorInfo;

/* ------------------ imageproc stuff ------------------ */

typedef void (ImageMapColorFunc)ARGS_DECL((int nColorIndex, XColor*));
typedef int (ConvolveFunc)ARGS_DECL((int x, int y));

#define TRAP_NODE_LEAF 0
#define TRAP_NODE_PT   1
#define TRAP_NODE_SEG  2

#define TRAP_PT_L 0
#define TRAP_PT_T 1
#define TRAP_PT_R 2
#define TRAP_PT_B 3
#define TRAP_PT_C 4

#define TRAP_SEG_L 0
#define TRAP_SEG_T 1
#define TRAP_SEG_R 2
#define TRAP_SEG_B 3

#define TRAP_LEAF_LT 0
#define TRAP_LEAF_RT 1
#define TRAP_LEAF_LB 2
#define TRAP_LEAF_RB 3

typedef struct TrapMapSegRec {
   double m, b; /* y = mx + b */
} TrapMapSeg;

typedef struct TrapMapRec {
   int type; /* TRAP_NODE_* */
   union {
      struct {
         int which; /* TRAP_LEAF_* */
         int *data; /* points to one of gaTrapMapLeaf */
      } leaf;
      struct {
         int which; /* TRAP_PT_* */
         IntPoint *data; /* points to one of gaTrapMapPt */
         struct TrapMapRec *left, *right;
      } pt;
      struct {
         int which; /* TRAP_SEG_* */
         TrapMapSeg *data; /* points to one of gaTrapMapSeg */
         struct TrapMapRec *above, *below;
      } seg;
   } detail;
} TrapMap;

typedef struct TrapMapSpecRec {
   int type;
   int which;
} TrapMapSpec;

/* ------------------ copy and paste properties stuff ------------------ */

typedef struct CheckArrayRec {
   int num_cols;
   int num_rows;
   int **value; /* value[0..(col_index-1)][0..(row_index-1)] */
   char **col_name; /* col_name[0..col_index] */
} * CheckArrayRecPtr;

typedef int (GetEntriesFunc)ARGS_DECL((DspList**, char***, int *pn_num_entries,
      int *pn_marked_index, struct CheckArrayRec **, char *inbuf, void*));
typedef int (AfterLoopFunc)ARGS_DECL((DspList**, char***, int *pn_num_entries,
      int *pn_marked_index, struct CheckArrayRec **, char *cur_buf,
      int btn_id, int selected_index, void*));
typedef int (CheckUpdateFunc)ARGS_DECL((DspList**, char***,
      int *pn_num_entries, int *pn_marked_index, struct CheckArrayRec **,
      char *cur_buf, int col, int row, void*));

typedef struct PropertiesRec {
   int			color, width, aw, ah, width_index;
   char			color_str[40], width_spec[40], aw_spec[40], ah_spec[40];
   int			fill, pen, dash, arrow_style, curved, rcb_radius;
   int			trans_pat, text_just, text_sz_unit, v_space;
   int			underline_on, overline_on;
   int			double_byte, text_font, text_style;
   int			transformed;
   struct XfrmMtrxRec	ctm;
   void			* userdata;
} * PropertiesRecPtr;

/* ------------------ pixmap button stuff ------------------ */

#define TGBS_NORMAL 0 /* button state is enabled */
#define TGBS_GRAYED 1 /* button state is disabled */
#define TGBS_RAISED 2 /* button state is mouse-overed */
#define TGBS_LOWRED 3 /* button state is pressed */

typedef struct TgPxMpBtnRec {
   int		flags; /* if (1<<TGBS_* != 0) then pixmap[TGBS_*] is valid */
   int		state;
   Pixmap	pixmap[4];
} TgPxMpBtn;

/* ------------------ menu stuff ------------------ */

#define TGMUITEM_SEPARATOR         ((char*)(-1))
#define TGMUITEM_BREAK             ((char*)(-2))
#define TGMUITEM_SUBMENU           ((char*)(-3))
#define TGMUITEM_PINNABLESUBMENU   ((char*)(-4))

                                  /* flasg */
#define TGMU_SEPARATOR            0x00000001
#define TGMU_BREAK                0x00000002 /* does not handle this, yet */
#define TGMU_HAS_CHECK            0x00000004 /* put a check mark if checked */
#define TGMU_HAS_RADIO            0x00000008 /* put a dot mark if checked */
#define TGMU_HAS_SUBMENU          0x00000010
#define TGMU_SUBMENU_PINNABLE     0x00000020
#define TGMU_HAS_SHORTCUT         0x00000040
#define TGMU_MULTICOLOR           0x00000080
#define TGMU_DISABLED             0x00000100
                                  /* masks */
#define TGMU_MASK_STATE           0x00010000 /* set/get state */
#define TGMU_MASK_CMDID           0x00020000 /* set/get cmdid */
#define TGMU_MASK_MULTICOLOR      0x00040000 /* set/get multicolor_pixel */
#define TGMU_MASK_PXMPBTN1        0x00100000 /* set/get checked_pxmpbtn */
#define TGMU_MASK_PXMPBTN2        0x00200000 /* set/get unchecked_pxmpbtn */
#define TGMU_MASK_CHECK           0x00400000 /* set/get check for check mark */
#define TGMU_MASK_RADIO           0x00800000 /* set/get check for radio mark */
#define TGMU_MASK_MENUSTR         0x01000000 /* set/get menu_str */
#define TGMU_MASK_STATUSSTR       0x02000000 /* set/get status_str */
#define TGMU_MASK_SUBMENU         0x04000000 /* set/get status_str */
#define TGMU_MASK_PINNABLESUBMENU 0x08000000 /* set/get status_str */
#define TGMU_MASK_SHORTCUTSTR     0x10000000 /* set/get status_str */
#define TGMU_MASK_LTXY            0x20000000 /* set/get status_str */
#define TGMU_MASK_USERDATA        0x40000000 /* set/get bbox.ltx and bbox.lty */
#define TGMU_MASK_RAWSTATUSSTR    0x80000000 /* need to gettext() before copy */

typedef struct TgMenuItemRec {
   int		flags; /* TGMU_* */
   int		state; /* one of TGBS_* */
   int		checked; /* valid if TGMU_HAS_CHECK or TGMU_HAS_RADIO */
   int		cmdid; /* command identifier */
   int		multicolor_pixel; /* valid only if TGMU_MULTICOLOR */
   struct BBRec	bbox;
   TgPxMpBtn	*checked_pxmpbtn; /* valid only if TGMU_HAS_PIXMAP */
   TgPxMpBtn	*unchecked_pxmpbtn; /* valid only if TGMU_HAS_PIXMAP */
   int		menu_str_allocated; /* need to free(menu_str) on destroy */
   char		*menu_str;
   char		*status_str;
   void		*userdata;
   union {
      struct TgMenuRec	*submenu;
      char		*shortcut_str;
   } detail;
   struct TgMenuItemInfoRec	*submenu_create_info;
} TgMenuItem;

#define TGMUTYPE_TEXT   0
#define TGMUTYPE_COLOR  1
#define TGMUTYPE_BITMAP 2

#define TGMUTYPE_MASK      0x0f
#define TGMUTYPE_CANSCROLL 0x10

typedef int (RefreshMenuFunc)ARGS_DECL((void *menu));

typedef struct TgMenuRec {
   int			type; /* one of TGMUTYPE_* */
   int			num_items;
   int			selected_index;
   struct BBRec		bbox;
   Window		window;
   int			padding; /* 0 if not 3D */
   int			brdr_w; /* 0 if 3D */
   int			is_main_menu;
   int			track_menubar;
   int			track_parent_menu;
   int			disallow_pinning;
   int			check_start, str_start; /* for TGMUTYPE_TEXT */
   int			shortcut_start, arrow_start; /* for TGMUTYPE_TEXT */
   /* can_scroll -- can have a scroll bar */
   int			can_scroll;
   int			scroll_start; /* 0: no scrollbar */
   int			first_index; /* 0: scroll all the way to top or left */
   /* multicolumn -- for TGMUTYPE_COLOR or TGMUTYPE_BITMAP */
   int			image_w, image_h, num_rows, num_cols;
   TgMenuItem		*menuitems; /* for TGMUTYPE_TEXT */
   void			*userdata;
   struct TgMenuRec	*parent_menu;
   RefreshMenuFunc	*refresh_proc; /* must set this for a submenu */
} TgMenu;

typedef struct TgMenuItemInfoRec {
   /*
    * for TGMUTYPE_TEXT, menu_str is (char*)
    * for TGMUTYPE_COLOR, menu_str is (int*)
    * for TGMUTYPE_BITMAP, menu_str is (Pixmap*)
    * menu_str can also be just TGMUITEM_SEPARATOR
    */
   char *menu_str;
   /*
    * for TGMUTYPE_TEXT, shortcut_str is (char*) or TGMUITEM_SUBMENU
    * for TGMUTYPE_BITMAP, shortcut_str NULL
    */
   char *shortcut_str;
   char *status_str;
   struct TgMenuInfoRec	*submenu_info;
   int cmdid; /* INVALID means no cmdid */
} TgMenuItemInfo;

typedef TgMenu *(CreateMenuFunc)ARGS_DECL((TgMenu *parent_menu, int x, int y,
      struct TgMenuInfoRec *menuinfo, int status_str_xlated));

typedef struct TgMenuInfoRec {
   int			type; /* one of TGMUTYPE_* */
   TgMenuItemInfo	*items;
   CreateMenuFunc	*create_proc;
} TgMenuInfo;

/* ------------------ shortcut stuff ------------------ */

#ifdef __DARWIN__
#ifdef _DARWIN_MOD1_AS_META
#define METAMASK (Mod1Mask)
#else /* ~_DARWIN_MOD1_AS_META */
#define METAMASK (Mod2Mask)
#endif /* _DARWIN_MOD1_AS_META */
#else /* ~__DARWIN__ */
#define METAMASK (Mod1Mask)
#endif /* __DARWIN__ */

#define CNTRL_ONLY (ControlMask)
#define META_ONLY (METAMASK)
#define CNTRL_META (ControlMask|METAMASK)

typedef struct ShortCutRec {
   char         code;
   unsigned int state;
   char         name[80];
   short        num_args;
   short        cmdid;
} ShortCutInfo;

/* ------------------ edit attr stuff ------------------ */

typedef struct tagEditAttrRec {
   int	num_attrs;
   char	**attr_names;
   char	**attr_values;
   char	**attr_strings;
   char	**status_strings;
   int	*fore_colors;
   int	*attr_indices;
} EditAttrInfo;

/* ------------------ scroll stuff ------------------ */

typedef int (ScrollBtnCallbackFunc)ARGS_DECL((void*));
typedef int (ScrollingCallbackFunc)ARGS_DECL((XMotionEvent*, void*));

typedef struct tagScrollBtnCallbackInfo {
   int ms;
   void *pv_userdata;
   ScrollBtnCallbackFunc *pf_scroll_btn_callback;
} ScrollBtnCallbackInfo;

/* ------------------ exec stuff ------------------ */

typedef int (ExecFunc)ARGS_DECL((char **argv, struct ObjRec *obj_ptr,
                                 char *orig_cmd));
typedef int (RawExecFunc)ARGS_DECL((char **argv, char **raw_argv,
                                    struct ObjRec *obj_ptr, char *orig_cmd));
typedef void (SimpleExecFunc)ARGS_DECL((struct ObjRec *obj_ptr,
                                        char *orig_cmd));

typedef struct tagExecInfoRec {
   NLFN *pfunc;
   char *func_name;
   int func_argc;
         /*
          * if (func_argc == 0) {
          *    pfunc is (SimpleExecFunc)();
          * } else if (func_argc < 0) {
          *    pfunc is (RawExecFunc)();
          * } else {
          *    pfunc is (ExecFunc)();
          * }
          */
   int double_quotes_for_null;
         /*
          * If TRUE, then if $(foo) evaluates to the empty string, "" is
          *       passed.  This is mainly used by commands with expression
          *       evaluations.
          */
} ExecInfo;

/* ------------------ page stuff ------------------ */

typedef struct tagSpecifyPagesInfoRec {
   int num_pages_specified;
   int *page_specified;
} SpecifyPagesInfo;

typedef struct tagPageInfoRec {
   int page_layout_mode, paper_col, paper_row;
   int cur_page_num, last_page_num;
   struct PageRec *first_page, *last_page, *cur_page;
} PageInfo;

/* ------------------ progress stuff ------------------ */

typedef struct tagProgressInfoRec {
   int total_size;
   int target_percent;

   char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
   int one_line_status;
} ProgressInfo;

/* ------------------ generated_by stuff ------------------ */

typedef struct tagGeneratedByInfoRec {
   char name[40];
   int version; /* version for tgif */
   char version_str[40]; /* version of the tool */
} GenerateByInfo;

/* ------------------ pngtrans stuff ------------------ */

typedef struct tagPngHeaderInfo {
    int valid;
    char fname[MAXPATHLENGTH];
    FILE *fp;
    uint32_t image_w;
    uint32_t image_h;
    unsigned char bit_depth;
    unsigned char color_type;
    unsigned char compression_method;
    unsigned char filter_method;
    unsigned char interlace_method;
    uint32_t palette_sz;
    unsigned char *palette;
    int trans_color_pixel_found;
    unsigned char trans_color_pixel_red;
    unsigned char trans_color_pixel_green;
    unsigned char trans_color_pixel_blue;
} PngHeaderInfo;

#endif /*_TGIF_TYPES_H_*/
