%
% Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
%
% Copyright (C) 2001-2009, William Chia-Wei Cheng.
%
% This file may be distributed under the terms of the Q Public License
% as defined by Trolltech AS of Norway and appearing in the file
% LICENSE.QPL included in the packaging of this file.
%
% THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
% THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
% PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
% INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
% FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
% NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
% WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
%
% @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgif.pl,v 1.7 2011/05/16 16:26:57 william Exp $
%

% 1) Call tgif_clean/0 each time before step (2).
% 2) Consult a '.obj' or a '.sym' file.
% 3) Call tgif_state/2.  This must be called before any other tgif goals
%    are called.
% 4) Trace any other goals.

%
% Note: tgif_pin() is not documented because its implementation is not done.
%       tgif_pin() first appears in file version 34.
%

internal_file_version(FileVersion) :-
    nl, write('Sorry, FileVersion '), write(FileVersion),
    write(' belongs to an internal release and it is not supported.'), nl.

check_file_version(34) :- internal_file_version(34), !, fail.
check_file_version(35) :- internal_file_version(35), !, fail.
check_file_version(36) :- internal_file_version(36), !, fail.
check_file_version(_).

tgif_state(FileVersion,Obj) :-
    tgif_state(FileVersion,Obj,_),
    !, check_file_version(FileVersion).

tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _GridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 3,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _GridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 7,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _GridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 8,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _GridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 11,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 12,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 13,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 18,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 19,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 21,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 26,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 27,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    % Mstsuda's Version
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PaperCol,_PaperRow,
        _CurPageNum,_LastPageNum),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 28,
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_Font,
        _TextStyle,_TextSize,_TextDPI,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 29,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,text_dpi=_TextDPI,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_FontName,
        _TextStyle,_FontSize,0,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 30,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font_name=_FontName,text_style=_TextStyle,
        font_size=_FontSize,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_FontName,
        _TextStyle,_FontSize,0,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode,_ColorDump),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 31,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font_name=_FontName,text_style=_TextStyle,
        font_size=_FontSize,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum,
        color_dump=_ColorDump],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_FontName,
        _TextStyle,_FontSize,0,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode,_ColorDump,
        _OnePageWidth,_OnePageHeight),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 32,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font_name=_FontName,text_style=_TextStyle,
        font_size=_FontSize,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum,
        color_dump=_ColorDump,one_page_width=_OnePageWidth,
        one_page_height=_OnePageHeight],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_FontName,
        _TextStyle,_FontSize,0,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode,_ColorDump,
        _OnePageWidth,_OnePageHeight,_StretchableText,_TextRotation,
        _RotationIncrement),
    tgif_chk_output(OutputObj,Obj),
    FileVersion =< 34,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font_name=_FontName,text_style=_TextStyle,
        font_size=_FontSize,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum,
        color_dump=_ColorDump,one_page_width=_OnePageWidth,
        one_page_height=_OnePageHeight,
        stretchable_text=_StretchableText,text_rotation=_TextRotation,
        rotation_increment=_RotationIncrement],
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).
tgif_state(FileVersion,Obj,Parms) :-
    (   var(Obj) -> OutputObj = true,
        current_predicate(state,
            state(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
    ;   OutputObj = false
    ),
    Obj = state(_PageStyle,FileVersion,_PrintMag,_OrigX,_OrigY,_Zoom,
        _EnglishGridSize,_Grid,_Color,_HoriAlign,_VertAlign,_LineWidth,
        _Spline,_LineStyle,_ObjFill,_PenPat,_TextJust,_FontName,
        _TextStyle,_FontSize,0,_Dash,_GridSystem,
        _MetricGridSize,_TextVSpace,_ZoomIn,_GridShown,_MoveMode,
        _TextRotate,_RCBoxRadius,_UseGray,_PageLayoutMode,_PageArg1,
        _PageArg2,_PageLineShownInTileMode,_ColorDump,
        _OnePageWidth,_OnePageHeight,_StretchableText,_TextRotation,
        _RotationIncrement,_TransPat),
    tgif_chk_output(OutputObj,Obj),
    FileVersion >= 35,
    (   (_PageLayoutMode == 0) -> % stack mode
        _CurPageNum = _PageArg1, _LastPageNum = _PageArg2,
        _PaperRow = 1, _PaperCol = 1
    ;   _PaperCol = _PageArg1, _PaperRow = _PageArg2,
        _CurPageNum = 1, _LastPageNum = 1
    ),
    Parms = [page_style=_PageStyle,file_version=FileVersion,
        print_mag=_PrintMag,
        orig_x=_OrigX,orig_y=_OrigY,zoom=_Zoom,
        english_grid_size=_GridSize,snap_on=_Grid,color=_Color,
        h_align=_HoriAlign,v_align=_VertAlign,line_width=_LineWidth,
        line_type=_Spline,
        line_style=_LineStyle,obj_fill=_ObjFill,pen_pat=_PenPat,
        text_just=_TextJust,font_name=_FontName,text_style=_TextStyle,
        _font_size_or_sz_unit=_FontSize,line_dash=_Dash,
        grid_system=_GridSystem,metric_grid=_MetricGridSize,
        text_v_space=_TextVSpace,zoom_in=_ZoomIn,grid_shown=_GridShown,
        move_mode=_ModeMode,text_rotate=_TextRotate,
        rcb_radius=_RCBoxRadius,use_gray=_UseGray,
        page_layout_mode=_PageLayoutMode,
        page_line_shown_in_tile_mode=_PageLineShownInTileMode,
        papge_col=_PaperCol,papge_row=_PaperRow,
        cur_page_num=_CurPageNum,last_page_num=_LastPageNum,
        color_dump=_ColorDump,one_page_width=_OnePageWidth,
        one_page_height=_OnePageHeight,
        stretchable_text=_StretchableText,text_rotation=_TextRotation,
        rotation_increment=_RotationIncrement,trans_pat=_TransPat],
    FileVersion >= 35,
    (   (FileVersion == 35) -> % stack mode
        _font_size_or_sz_unit = 'font_size'
    ;   _font_size_or_sz_unit = 'font_sz_unit'
    ),
    !, abolish(tgif_file_version/1),
    !, assert(tgif_file_version(FileVersion)).

% --------------------------------------------------------------------- %

tgif_text(Obj) :- tgif_text(Obj,_).

tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 2, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 6, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 7, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 9, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 10, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,_Asc,_Des,
        StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,asc=_Asc,des=_Des,
        strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 12, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,_Asc,_Des,
        _ObjFill,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,asc=_Asc,des=_Des,
        obj_fill=_ObjFill,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,_Asc,_Des,
        _ObjFill,_VSpace,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,asc=_Asc,des=_Des,
        obj_fill=_ObjFill,v_space=_VSpace,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,_Asc,_Des,
        _ObjFill,_VSpace,_Rotation,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,asc=_Asc,des=_Des,
        obj_fill=_ObjFill,v_space=_VSpace,rotation=_Rotation,
        strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 29, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_Font,_TextStyle,_TextSize,_NumLines,_TextJust,
        _TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,_TextDPI,_Asc,_Des,
        _ObjFill,_VSpace,_Rotation,_Locked,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font=_Font,text_style=_TextStyle,
        text_size=_TextSize,num_lines=_NumLines,text_just=_TextJust,
        text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
        bbox_h=_BBoxH,id=_Id,text_dpi=_TextDPI,asc=_Asc,des=_Des,
        obj_fill=_ObjFill,v_space=_VSpace,rotation=_Rotation,
        obj_locked=_Locked,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    % From version 30 on, the FontSize is the actual size;
    %    the TextSize in older versions is a size index.
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = text(_Color,_X,_Y,_FontName,_TextStyle,_FontSize,_NumLines,
        _TextJust,_TextRotate,_PenPat,_BBoxW,_BBoxH,_Id,0,_Asc,_Des,
        _ObjFill,_VSpace,_Rotation,_Locked,StrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,x=_X,y=_Y,font_name=_FontName,
        text_style=_TextStyle,font_size=_FontSize,num_lines=_NumLines,
        text_just=_TextJust,text_rotate=_TextRotate,pen_pat=_PenPat,
        bbox_w=_BBoxW,bbox_h=_BBoxH,id=_Id,asc=_Asc,des=_Des,
        obj_fill=_ObjFill,v_space=_VSpace,rotation=_Rotation,
        obj_locked=_Locked,strs=StrList],
    tgif_strs(StrList).
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = text(_Color,_X,_Y,_FontName,_TextStyle,_FontSize,
            _NumLines,_TextJust,_TextRotate,_PenPat,_BBoxW,_BBoxH,
            _Id,0,_Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,
            _UnderlineOn,_Underline,_MinLBearing,_MaxRExtra,
            _DoubleByte,_Direction,_CustomFontName,_Compressed,
            _Transformed,_Invisible,StrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,x=_X,y=_Y,font_name=_FontName,
            text_style=_TextStyle,font_size=_FontSize,
            num_lines=_NumLines,text_just=_TextJust,
            text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
            bbox_h=_BBoxH,id=_Id,asc=_Asc,des=_Des,
            obj_fill=_ObjFill,v_space=_VSpace,rotation=_Rotation,
            obj_locked=_Locked,underline_on=_UnderlineOn,
            underline=_Underline,min_lbearing=_MinLBearing,
            max_rextra=_MaxRExtra,double_byte=_DoubleByte,
            direction=_Direction,custom_font_name=_CustomFontName,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,strs=StrList]
    |   _Transformed = 1,
        Obj = text(_Color,_X,_Y,_FontName,_TextStyle,_FontSize,
            _NumLines,_TextJust,_TextRotate,_PenPat,_BBoxW,_BBoxH,
            _Id,0,_Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,
            _UnderlineOn,_Underline,_MinLBearing,_MaxRExtra,
            _DoubleByte,_Direction,_CustomFontName,_Compressed,
            _Transformed,_Invisible,TransformSpec,StrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_text_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,x=_X,y=_Y,font_name=_FontName,
            text_style=_TextStyle,font_size=_FontSize,
            num_lines=_NumLines,text_just=_TextJust,
            text_rotate=_TextRotate,pen_pat=_PenPat,bbox_w=_BBoxW,
            bbox_h=_BBoxH,id=_Id,asc=_Asc,des=_Des,
            obj_fill=_ObjFill,v_space=_VSpace,rotation=_Rotation,
            obj_locked=_Locked,underline_on=_UnderlineOn,
            underline=_Underline,min_lbearing=_MinLBearing,
            max_rextra=_MaxRExtra,double_byte=_DoubleByte,
            direction=_Direction,custom_font_name=_CustomFontName,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,transform_spec=TransformParms,
            strs=StrList]
    ),
    tgif_strs(StrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_text(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        (   Obj = text(_FillPatColor,_X,_Y,
                _NumLines,_TextJust,_PenPat,_BBoxW,_BBoxH,_Id,
                _Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,_Underline,
                _TextW,_TextH,_MinLBearing,_MaxRExtra,_CustomFontName,
                _Compressed,_Transformed,_Invisible,_TransPat,_BaselineY,
                MinilinesList),
                _BgColor = ''
        |   Obj = text(_FillPatColor,_X,_Y,
                _NumLines,_TextJust,_PenPat,_BBoxW,_BBoxH,_Id,
                _Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,_Underline,
                _TextW,_TextH,_MinLBearing,_MaxRExtra,_CustomFontName,
                _Compressed,_Transformed,_Invisible,_TransPat,_BaselineY,
                _BgColor,MinilinesList)
        ),
        tgif_chk_output(OutputObj,Obj),
        Parms = [fill_pat_color=_FillPatColor,x=_X,y=_Y,num_lines=_NumLines,
            text_just=_TextJust,pen_pat=_PenPat,bbox_w=_BBoxW,bbox_h=_BBoxH,
            id=_Id,asc=_Asc,des=_Des,obj_fill=_ObjFill,v_space=_VSpace,
            rotation=_Rotation,obj_locked=_Locked,underline=_Underline,
            text_w=_TextW,text_h=_TextH,min_lbearing=_MinLBearing,
            max_rextra=_MaxRExtra,custom_font_name=_CustomFontName,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,trans_pat=_TransPat,baseline_y=_BaselineY,
            bg_color=_BgColor,minilines=MinilinesList]
    |   _Transformed = 1,
        (   Obj = text(_FillPatColor,_X,_Y,
                _NumLines,_TextJust,_PenPat,_BBoxW,_BBoxH,_Id,
                _Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,_Underline,
                _TextW,_TextH,_MinLBearing,_MaxRExtra,_CustomFontName,
                _Compressed,_Transformed,_Invisible,_TransPat,_BaselineY,
                TransformSpec,MinilinesList),
                _BgColor = ''
        |   Obj = text(_FillPatColor,_X,_Y,
                _NumLines,_TextJust,_PenPat,_BBoxW,_BBoxH,_Id,
                _Asc,_Des,_ObjFill,_VSpace,_Rotation,_Locked,_Underline,
                _TextW,_TextH,_MinLBearing,_MaxRExtra,_CustomFontName,
                _Compressed,_Transformed,_Invisible,_TransPat,_BaselineY,
                _BgColor,TransformSpec,MinilinesList)
        ),
        tgif_chk_output(OutputObj,Obj),
        tgif_text_transform_spec(TransformSpec,TransformParms),
        Parms = [fill_pat_color=_FillPatColor,x=_X,y=_Y,num_lines=_NumLines,
            text_just=_TextJust,pen_pat=_PenPat,bbox_w=_BBoxW,bbox_h=_BBoxH,
            id=_Id,asc=_Asc,des=_Des,obj_fill=_ObjFill,v_space=_VSpace,
            rotation=_Rotation,obj_locked=_Locked,underline=_Underline,
            text_w=_TextW,text_h=_TextH,min_lbearing=_MinLBearing,
            max_rextra=_MaxRExtra,custom_font_name=_CustomFontName,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,trans_pat=_TransPat,baseline_y=_BaselineY,
            transform_spec=TransformParms,bg_color=_BgColor,
            minilines=MinilinesList]
    ),
    tgif_minilines_list(MinilinesList).

% --------------------------------------------------------------------- %

tgif_box(Obj) :- tgif_box(Obj,_).

tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 7, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat].
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 8, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Id,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Id,_Dash,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        id=_Id,line_dash=_Dash,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Id,_Dash,
        _Rotation,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        id=_Id,line_dash=_Dash,rotation=_Rotation,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Id,_Dash,
        _Rotation,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        id=_Id,line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Id,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            id=_Id,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            attrs=AttrList]
    |   _Transformed = 1,
        Obj = box(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Id,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            id=_Id,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_box(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = box(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Id,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_TransPat,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            id=_Id,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            trans_pat=_TransPat,attrs=AttrList]
    |   _Transformed = 1,
        Obj = box(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Id,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_TransPat,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            id=_Id,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            trans_pat=_TransPat,transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_oval(Obj) :- tgif_oval(Obj,_).

tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 7, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,_ObjFill,
        _LineWidth,_PenPat),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,rbx=_RightBotX,
        rby=_RightBotY,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat].
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 8, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,_ObjFill,
        _LineWidth,_PenPat,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,rbx=_RightBotX,
        rby=_RightBotY,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,_ObjFill,
        _LineWidth,_PenPat,_Id,_Dash,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,rbx=_RightBotX,
        rby=_RightBotY,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,id=_Id,line_dash=_Dash,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,_ObjFill,
        _LineWidth,_PenPat,_Id,_Dash,_Rotation,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,rbx=_RightBotX,
        rby=_RightBotY,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,id=_Id,line_dash=_Dash,rotation=_Rotation,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,_ObjFill,
        _LineWidth,_PenPat,_Id,_Dash,_Rotation,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,rbx=_RightBotX,
        rby=_RightBotY,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,id=_Id,line_dash=_Dash,rotation=_Rotation,
        obj_locked=_Locked,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,
            _ObjFill,_LineWidth,_PenPat,_Id,_Dash,_Rotation,
            _Locked,_Transformed,_Invisible,_LineWidthSpec,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,
            rbx=_RightBotX,rby=_RightBotY,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,id=_Id,
            line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,attrs=AttrList]
    |   _Transformed = 1,
        Obj = oval(_Color,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,
            _ObjFill,_LineWidth,_PenPat,_Id,_Dash,_Rotation,
            _Locked,_Transformed,_Invisible,_LineWidthSpec,
            TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_LeftTopX,lty=_LeftTopY,
            rbx=_RightBotX,rby=_RightBotY,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,id=_Id,
            line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_oval(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = oval(_Color,_BgColor,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,
            _ObjFill,_LineWidth,_PenPat,_Id,_Dash,_Rotation,
            _Locked,_Transformed,_Invisible,_LineWidthSpec,_TransPat,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_LeftTopX,lty=_LeftTopY,
            rbx=_RightBotX,rby=_RightBotY,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,id=_Id,
            line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,trans_pat=_TransPat,attrs=AttrList]
    |   _Transformed = 1,
        Obj = oval(_Color,_BgColor,_LeftTopX,_LeftTopY,_RightBotX,_RightBotY,
            _ObjFill,_LineWidth,_PenPat,_Id,_Dash,_Rotation,
            _Locked,_Transformed,_Invisible,_LineWidthSpec,_TransPat,
            TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_LeftTopX,lty=_LeftTopY,
            rbx=_RightBotX,rby=_RightBotY,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,id=_Id,
            line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,trans_pat=_TransPat,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_poly(Obj) :- tgif_poly(Obj,_).

tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 3, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 4, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 8, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,_Dash,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,line_dash=_Dash,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 16, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,_Dash,_Rotation,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,_Dash,_Rotation,_ArrowHeadW,_ArrowHeadH,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
        arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 30, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,_Dash,_Rotation,_ArrowHeadW,_ArrowHeadH,_Locked,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
        arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
        obj_locked=_Locked,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,_Spline,
        _ObjFill,_Dash,_Rotation,_ArrowHeadW,_ArrowHeadH,_Locked,
        _SmoothSpecStr,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,line_style=_LineStyle,
        line_width=_LineWidth,pen_pat=_PenPat,id=_Id,line_type=_Spline,
        obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
        arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
        obj_locked=_Locked,smooth_spec=_SmoothSpecStr,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,
            _Spline,_ObjFill,_Dash,_Rotation,_ArrowHeadW,
            _ArrowHeadH,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_ArrowHeadWSpec,_ArrowHeadHSpec,
            _SmoothSpecStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,
            line_style=_LineStyle,line_width=_LineWidth,
            pen_pat=_PenPat,id=_Id,line_type=_Spline,
            obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
            arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,
            smooth_spec=_SmoothSpecStr,attrs=AttrList]
    |   _Transformed = 1,
        Obj = poly(_Color,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,
            _Spline,_ObjFill,_Dash,_Rotation,_ArrowHeadW,
            _ArrowHeadH,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_ArrowHeadWSpec,_ArrowHeadHSpec,
            _SmoothSpecStr,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,
            line_style=_LineStyle,line_width=_LineWidth,
            pen_pat=_PenPat,id=_Id,line_type=_Spline,
            obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
            arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,
            smooth_spec=_SmoothSpecStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_poly(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = poly(_Color,_BgColor,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,
            _Spline,_ObjFill,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_TransPat,_LineCap,
            _SmoothSpecStr,_TensionSpec,LeftArrowSpec,RightArrowSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,num_vs=_NumVs,vs=_Vs,
            line_style=_LineStyle,line_width=_LineWidth,
            pen_pat=_PenPat,id=_Id,line_type=_Spline,
            obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            smooth_spec=_SmoothSpecStr,attrs=AttrList]
    |   _Transformed = 1,
        Obj = poly(_Color,_BgColor,_NumVs,_Vs,_LineStyle,_LineWidth,_PenPat,_Id,
            _Spline,_ObjFill,_Dash,_Rotation,_Locked,_Transformed,_Invisible,
            _LineWidthSpec,_TransPat,_LineCap,
            _SmoothSpecStr,_TensionSpec,LeftArrowSpec,RightArrowSpec,
            TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,num_vs=_NumVs,vs=_Vs,
            line_style=_LineStyle,line_width=_LineWidth,
            pen_pat=_PenPat,id=_Id,line_type=_Spline,
            obj_fill=_ObjFill,line_dash=_Dash,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            smooth_spec=_SmoothSpecStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_arrow(LeftArrowSpec),
    tgif_arrow(RightArrowSpec),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_polygon(Obj) :- tgif_polygon(Obj,_).

tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 3, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat].
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 7, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline].
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 8, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline,
        _Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline,
        id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline,
        _Id,_Dash,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline,
        id=_Id,line_dash=_Dash,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline,
        _Id,_Dash,_Rotation,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline,
        id=_Id,line_dash=_Dash,rotation=_Rotation,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 30, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline,
        _Id,_Dash,_Rotation,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline,
        id=_Id,line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,_Spline,
        _Id,_Dash,_Rotation,_Locked,_SmoothSpecStr,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
        line_width=_LineWidth,pen_pat=_PenPat,line_type=_Spline,
        id=_Id,line_dash=_Dash,rotation=_Rotation,obj_locked=_Locked,
        smooth_spec=_SmoothSpecStr,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,
            _Spline,_Id,_Dash,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_SmoothSpecStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,
            line_type=_Spline,id=_Id,line_dash=_Dash,
            rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,
            smooth_spec=_SmoothSpecStr,attrs=AttrList]
    |   _Transformed = 1,
        Obj = polygon(_Color,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,
            _Spline,_Id,_Dash,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_SmoothSpecStr,TransformSpec,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,num_vs=_NumVs,vs=_Vs,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,
            line_type=_Spline,id=_Id,line_dash=_Dash,
            rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,
            smooth_spec=_SmoothSpecStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_polygon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = polygon(_Color,_BgColor,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,
            _Spline,_Id,_Dash,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_TransPat,_SmoothSpecStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,num_vs=_NumVs,vs=_Vs,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_type=_Spline,id=_Id,line_dash=_Dash,
            rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,trans_pat=_TransPat,
            smooth_spec=_SmoothSpecStr,attrs=AttrList]
    |   _Transformed = 1,
        Obj = polygon(_Color,_BgColor,_NumVs,_Vs,_ObjFill,_LineWidth,_PenPat,
            _Spline,_Id,_Dash,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_TransPat,_SmoothSpecStr,TransformSpec,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,num_vs=_NumVs,vs=_Vs,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_type=_Spline,id=_Id,line_dash=_Dash,
            rotation=_Rotation,obj_locked=_Locked,
            transformed=_Transformed,invisible=_Invisible,
            line_width_spec=_LineWidthSpec,trans_pat=_TransPat,
            smooth_spec=_SmoothSpecStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_rcbox(Obj) :- tgif_rcbox(Obj,_).

tgif_rcbox(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = rcbox(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Dash,
        _Radius,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        line_dash=_Dash,radius=_Radius,id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_rcbox(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = rcbox(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Dash,
        _Radius,_Id,_Rotation,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_rcbox(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = rcbox(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,_Dash,
        _Radius,_Id,_Rotation,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
        line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
        obj_locked=_Locked,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_rcbox(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = rcbox(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Dash,_Radius,_Id,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            attrs=AttrList]
    |   _Transformed = 1,
        Obj = rcbox(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Dash,_Radius,_Id,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_rcbox(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = rcbox(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Dash,_Radius,_Id,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_TransPat,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            trans_pat=_TransPat,attrs=AttrList]
    |   _Transformed = 1,
        Obj = rcbox(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_LineWidth,_PenPat,
            _Dash,_Radius,_Id,_Rotation,_Locked,_Transformed,
            _Invisible,_LineWidthSpec,_TransPat,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,line_width=_LineWidth,pen_pat=_PenPat,
            line_dash=_Dash,radius=_Radius,id=_Id,rotation=_Rotation,
            obj_locked=_Locked,transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            trans_pat=_TransPat,transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_arc(Obj) :- tgif_arc(Obj,_).

tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,_Xc,_Yc,
        _X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
        xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,clock_wise=_Dir,
        major_axis=_W,minor_axis_H,angle1=_Angle1,angle2=_Angle2,
        id=_Id,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 15, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,_Xc,_Yc,
        _X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,_Rotation,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
        xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,clock_wise=_Dir,
        major_axis=_W,minor_axis_H,angle1=_Angle1,angle2=_Angle2,
        id=_Id,rotation=_Rotation,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 16, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,_Xc,_Yc,
        _X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,_Rotation,
        _Style,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
        xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,clock_wise=_Dir,
        major_axis=_W,minor_axis_H,angle1=_Angle1,angle2=_Angle2,
        id=_Id,rotation=_Rotation,line_style=_Style,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,_Xc,_Yc,
        _X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,_Rotation,
        _Style,_ArrowHeadW,_ArrowHeadH,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
        xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,clock_wise=_Dir,
        major_axis=_W,minor_axis_H,angle1=_Angle1,angle2=_Angle2,
        id=_Id,rotation=_Rotation,line_style=_Style,
        arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
        attrs=AttrList],
    tgif_attrs(AttrList).
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,_Xc,_Yc,
        _X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,_Rotation,
        _Style,_ArrowHeadW,_ArrowHeadH,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
        pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
        xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,clock_wise=_Dir,
        major_axis=_W,minor_axis_H,angle1=_Angle1,angle2=_Angle2,
        id=_Id,rotation=_Rotation,line_style=_Style,
        arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
        obj_locked=_Locked,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,
            _Xc,_Yc,_X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,
            _Rotation,_Style,_ArrowHeadW,_ArrowHeadH,_Locked,
            _Transformed,_Invisible,_LineWidthSpec,_ArrowHeadWSpec,
            _ArrowHeadHSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
            pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
            xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,
            clock_wise=_Dir,major_axis=_W,minor_axis_H,
            angle1=_Angle1,angle2=_Angle2,id=_Id,rotation=_Rotation,
            line_style=_Style,arrow_head_w=_ArrowHeadW,
            arrow_head_h=_ArrowHeadH,obj_locked=_Locked,
            transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,attrs=AttrList]
    |   _Transformed = 1,
        Obj = arc(_Color,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,
            _Xc,_Yc,_X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,
            _Rotation,_Style,_ArrowHeadW,_ArrowHeadH,_Locked,
            _Transformed,_Invisible,_LineWidthSpec,_ArrowHeadWSpec,
            _ArrowHeadHSpec,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,obj_fill=_ObjFill,line_width=_LineWidth,
            pen_pat=_PenPat,line_dash=_Dash,ltx=_LtX,lty=_LtY,
            xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,
            clock_wise=_Dir,major_axis=_W,minor_axis_H,
            angle1=_Angle1,angle2=_Angle2,id=_Id,rotation=_Rotation,
            line_style=_Style,arrow_head_w=_ArrowHeadW,
            arrow_head_h=_ArrowHeadH,obj_locked=_Locked,
            transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_arc(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = arc(_Color,_BgColor,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,
            _Xc,_Yc,_X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,
            _Rotation,_Style,_ArrowHeadW,_ArrowHeadH,_Locked,
            _Transformed,_Invisible,_LineWidthSpec,_ArrowHeadWSpec,
            _ArrowHeadHSpec,_TransPat,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,line_dash=_Dash,
            ltx=_LtX,lty=_LtY,xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,
            clock_wise=_Dir,major_axis=_W,minor_axis_H,
            angle1=_Angle1,angle2=_Angle2,id=_Id,rotation=_Rotation,
            line_style=_Style,arrow_head_w=_ArrowHeadW,
            arrow_head_h=_ArrowHeadH,obj_locked=_Locked,
            transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,trans_pat=_TransPat,
            attrs=AttrList]
    |   _Transformed = 1,
        Obj = arc(_Color,_BgColor,_ObjFill,_LineWidth,_PenPat,_Dash,_LtX,_LtY,
            _Xc,_Yc,_X1,_Y1,_X2,_Y2,_Dir,_W,_H,_Angle1,_Angle2,_Id,
            _Rotation,_Style,_ArrowHeadW,_ArrowHeadH,_Locked,
            _Transformed,_Invisible,_LineWidthSpec,_ArrowHeadWSpec,
            _ArrowHeadHSpec,_TransPat,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,obj_fill=_ObjFill,
            line_width=_LineWidth,pen_pat=_PenPat,line_dash=_Dash,
            ltx=_LtX,lty=_LtY,xc=_Xc,yc=_Yc,x1=_X1,y1=_Y1,x2=_X2,y2=_Y2,
            clock_wise=_Dir,major_axis=_W,minor_axis_H,
            angle1=_Angle1,angle2=_Angle2,id=_Id,rotation=_Rotation,
            line_style=_Style,arrow_head_w=_ArrowHeadW,
            arrow_head_h=_ArrowHeadH,obj_locked=_Locked,
            transformed=_Transformed,
            invisible=_Invisible,line_width_spec=_LineWidthSpec,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,trans_pat=_TransPat,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_xbm(Obj) :- tgif_xbm(Obj,_).

tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_BitmapStr,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,obj_fill=_ObjFill,
        id=_Id,xbm_str=_BitmapStr,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 22, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,_BitmapStr,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,obj_fill=_ObjFill,
        id=_Id,rotation=_Rotation,xbm_str=_BitmapStr,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 23, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,_ImageW,_ImageH,
        _Rotate,_Flip,_BitmapStr,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,obj_fill=_ObjFill,
        id=_Id,rotation=_Rotation,image_w=_ImageW,image_h=_ImageH,
        rotate=_Rotate,flip=_Flip,xbm_str=_BitmapStr,attrs=AttrList],
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _RealType = 0, _NoBitmap = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,
            xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Date,_File,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,
            date=_Date,file=_File,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 28, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _RealType = 0, _NoBitmap = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_Date,_File,_BitmapStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_Date,_File,_BitmapStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_Date,_File,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            date=_Date,file=_File,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _RealType = 0, _NoBitmap = 0, _SaveEPSF = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1, _SaveEPSF = 0,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Date,_File,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,
            date=_Date,file=_File,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 1,
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Date,_File,_NumEPSFLines,_EPSFLines,_BitmapStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        length(_EPSFLines,_NumEPSFLines),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,num_epsf_lines=_NumEPSFLines,
            epsf_lines=_EPSFLines,date=_Date,file=_File,
            xbm_str=_BitmapStr,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _RealType = 0, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 0,
        % Just XBM
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 0,
        % What type is this?
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1, _SaveEPSF = 0, _Transformed = 0,
        % Linked EPS
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 1, _Transformed = 0,
        % Embeded EPS
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_NumEPSFLines,_EPSFLines,_BitmapStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        length(_EPSFLines,_NumEPSFLines),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            num_epsf_lines=_NumEPSFLines,epsf_lines=_EPSFLines,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 0, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 1,
        % Just XBM
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_BitmapStr,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 1,
        % What type is this?
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_BitmapStr,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1, _SaveEPSF = 0, _Transformed = 1,
        % Linked EPS
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            date=_Date,file=_File,transform_spec=TransformParms,
            attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 1, _Transformed = 1,
        % Embeded EPS
        Obj = xbm(_Color,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,
            _Date,_File,_NumEPSFLines,_EPSFLines,_BitmapStr,
            TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        length(_EPSFLines,_NumEPSFLines),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,
            num_epsf_lines=_NumEPSFLines,epsf_lines=_EPSFLines,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_xbm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _RealType = 0, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 0,
        % Just XBM
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 0,
        % What type is this?
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_BitmapStr,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1, _SaveEPSF = 0, _Transformed = 0,
        % Linked EPS
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 1, _Transformed = 0,
        % Embeded EPS
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_NumEPSFLines,_EPSFLines,_BitmapStr,
            AttrList),
        tgif_chk_output(OutputObj,Obj),
        length(_EPSFLines,_NumEPSFLines),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            num_epsf_lines=_NumEPSFLines,epsf_lines=_EPSFLines,
            date=_Date,file=_File,xbm_str=_BitmapStr,attrs=AttrList]
    |   _RealType = 0, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 1,
        % Just XBM
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_BitmapStr,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='xbm',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 0, _Transformed = 1,
        % What type is this?
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_BitmapStr,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 1, _SaveEPSF = 0, _Transformed = 1,
        % Linked EPS
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            date=_Date,file=_File,transform_spec=TransformParms,
            attrs=AttrList]
    |   _RealType = 1, _NoBitmap = 0, _SaveEPSF = 1, _Transformed = 1,
        % Embeded EPS
        Obj = xbm(_Color,_BgColor,_X1,_Y1,_X2,_Y2,_ObjFill,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_RealType,_LLX,_LLY,
            _URX,_URY,_NoBitmap,_Locked,_SaveEPSF,
            _Compressed,_Transformed,_Invisible,_TransPat,
            _Date,_File,_NumEPSFLines,_EPSFLines,_BitmapStr,
            TransformSpec,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        length(_EPSFLines,_NumEPSFLines),
        Parms = [color=_Color,bg_color=_BgColor,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_ObjFill,id=_Id,rotation=_Rotation,
            image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,
            flip=_Flip,real_type='eps',llx=_LLX,lly=_LLY,
            urx=_URX,ury=_URY,no_bitmap=_NoBitmap,obj_locked=_Locked,
            save_epsf=_SaveEPSF,compressed=_Compressed,
            transformed=_Transformed,invisible=_Invisible,trans_pat=_TransPat,
            num_epsf_lines=_NumEPSFLines,epsf_lines=_EPSFLines,
            date=_Date,file=_File,xbm_str=_BitmapStr,
            transform_spec=TransformParms,attrs=AttrList]
    ),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_xpm(Obj) :- tgif_xpm(Obj,_).

tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 13, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,_Id,
        ColorNames,Pixels,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_UnUsedObjFill,num_colors=_NumColors,id=_Id,
        color_names=ColorNames,pixels=Pixels,attrs=AttrList],
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 14, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,_Id,
        _Rotation,ColorNames,Pixels,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_UnUsedObjFill,num_colors=_NumColors,id=_Id,
        rotation=_Rotation,color_names=ColorNames,pixels=Pixels,
        attrs=AttrList],
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 22, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
        _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,ColorNames,Pixels,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_UnUsedObjFill,num_colors=_NumColors,
        chars_per_pixel=_CharsPerPixel,
        first_pixel_is_bg=_FirstPixelIsBg,id=_Id,
        rotation=_Rotation,color_names=ColorNames,pixels=Pixels,
        attrs=AttrList],
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
        _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,_ImageW,_ImageH,
        _Rotate,_Flip,ColorNames,Pixels,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_UnUsedObjFill,num_colors=_NumColors,
        chars_per_pixel=_CharsPerPixel,
        first_pixel_is_bg=_FirstPixelIsBg,id=_Id,rotation=_Rotation,
        image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,flip=_Flip,
        color_names=ColorNames,pixels=Pixels,attrs=AttrList],
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
        _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,_ImageW,_ImageH,
        _Rotate,_Flip,_Locked,ColorNames,Pixels,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
        obj_fill=_UnUsedObjFill,num_colors=_NumColors,
        chars_per_pixel=_CharsPerPixel,
        first_pixel_is_bg=_FirstPixelIsBg,id=_Id,rotation=_Rotation,
        image_w=_ImageW,image_h=_ImageH,rotate=_Rotate,flip=_Flip,
        obj_locked=_Locked,color_names=ColorNames,pixels=Pixels,
        attrs=AttrList],
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
            _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_Locked,_Compressed,
            _Transformed,_Invisible,ColorNames,Pixels,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_UnUsedObjFill,num_colors=_NumColors,
            chars_per_pixel=_CharsPerPixel,
            first_pixel_is_bg=_FirstPixelIsBg,id=_Id,
            rotation=_Rotation,image_w=_ImageW,image_h=_ImageH,
            rotate=_Rotate,flip=_Flip,obj_locked=_Locked,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,color_names=ColorNames,
            pixels=Pixels,attrs=AttrList]
    |   _Transformed = 1,
        Obj = xpm(_Color,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
            _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_Locked,_Compressed,
            _Transformed,_Invisible,TransformSpec,ColorNames,
            Pixels,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,
            obj_fill=_UnUsedObjFill,num_colors=_NumColors,
            chars_per_pixel=_CharsPerPixel,
            first_pixel_is_bg=_FirstPixelIsBg,id=_Id,
            rotation=_Rotation,image_w=_ImageW,image_h=_ImageH,
            rotate=_Rotate,flip=_Flip,obj_locked=_Locked,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,transform_spec=TransformParms,
            color_names=ColorNames,pixels=Pixels,attrs=AttrList]
    ),
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).
%
% Note: FileVersions 34, 35, and 36 are skipped.  See check_file_version/1.
%
tgif_xpm(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   _Transformed = 0,
        Obj = xpm(_Color,_TransColor,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
            _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_Locked,_Compressed,
            _Transformed,_Invisible,ColorNames,Pixels,AttrList),
        tgif_chk_output(OutputObj,Obj),
        Parms = [color=_Color,trans_color=_TransColor,
            ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,obj_fill=_UnUsedObjFill,
            num_colors=_NumColors,chars_per_pixel=_CharsPerPixel,
            first_pixel_is_bg=_FirstPixelIsBg,id=_Id,
            rotation=_Rotation,image_w=_ImageW,image_h=_ImageH,
            rotate=_Rotate,flip=_Flip,obj_locked=_Locked,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,color_names=ColorNames,
            pixels=Pixels,attrs=AttrList]
    |   _Transformed = 1,
        Obj = xpm(_Color,_TransColor,_X1,_Y1,_X2,_Y2,_UnUsedObjFill,_NumColors,
            _CharsPerPixel,_FirstPixelIsBg,_Id,_Rotation,
            _ImageW,_ImageH,_Rotate,_Flip,_Locked,_Compressed,
            _Transformed,_Invisible,TransformSpec,ColorNames,
            Pixels,AttrList),
        tgif_chk_output(OutputObj,Obj),
        tgif_obj_transform_spec(TransformSpec,TransformParms),
        Parms = [color=_Color,trans_color=_TransColor,
            ltx=_X1,lty=_Y1,rbx=_X2,rby=_Y2,obj_fill=_UnUsedObjFill,
            num_colors=_NumColors,chars_per_pixel=_CharsPerPixel,
            first_pixel_is_bg=_FirstPixelIsBg,id=_Id,
            rotation=_Rotation,image_w=_ImageW,image_h=_ImageH,
            rotate=_Rotate,flip=_Flip,obj_locked=_Locked,
            compressed=_Compressed,transformed=_Transformed,
            invisible=_Invisible,transform_spec=TransformParms,
            color_names=ColorNames,pixels=Pixels,attrs=AttrList]
    ),
    tgif_color_info(ColorNames),
    tgif_pixels(Pixels),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_group(Obj) :- tgif_group(Obj,_).

tgif_group(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 20, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = group(ObjList,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_group(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = group(ObjList,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_group(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = group(ObjList,_Id,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,obj_locked=_Locked,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_group(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = group(ObjList,_Id,_Locked,_Invisible,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,obj_locked=_Locked,invisible=_Invisible,
        attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_sym(Obj) :- tgif_sym(Obj,_).

tgif_sym(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 20, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = sym(ObjList,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_sym(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = sym(ObjList,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_sym(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = sym(ObjList,_Id,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,obj_locked=_Locked,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_sym(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = sym(ObjList,_Id,_Locked,_Invisible,_AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [objs=ObjList,id=_Id,obj_locked=_Locked,invisible=_Invisible,
        attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).

% --------------------------------------------------------------------- %

tgif_icon(Obj) :- tgif_icon(Obj,_).

tgif_icon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 12, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = icon(ObjList,_Name,_Id,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [name=_Name,id=_Id,objs=ObjList,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_icon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 25, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = icon(ObjList,_Name,_Id,_Rotation,_Flip,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [name=_Name,id=_Id,rotation=_Rotation,objs=ObjList,
        attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_icon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = icon(ObjList,_Name,_Id,_Rotation,_Flip,_Locked,AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [name=_Name,id=_Id,rotation=_Rotation,obj_locked=_Locked,
        objs=ObjList,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).
tgif_icon(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = icon(ObjList,_Name,_Id,_Rotation,_Flip,_Locked,_Invisible,
        AttrList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [name=_Name,id=_Id,rotation=_Rotation,obj_locked=_Locked,
        invisible=_Invisible,objs=ObjList,attrs=AttrList],
    tgif_objs(ObjList),
    tgif_attrs(AttrList).

% ======================== support routines =========================== %

tgif_clean :- tgif_clean(state).

tgif_real_clean :-
    tgif_clean(state),
    tgif_clean(text), tgif_clean(box), tgif_clean(oval), tgif_clean(poly),
    tgif_clean(polygon), tgif_clean(rcbox), tgif_clean(arc),
    tgif_clean(xbm), tgif_clean(xpm),
    tgif_clean(group), tgif_clean(sym), tgif_clean(icon).

tgif_clean(Type) :-
    current_predicate(Type,Term), functor(Term,_F,A), abolish(Type,A), fail.
tgif_clean(_Type) :- !.

tgif_chk_output(true,Obj) :- !,
    functor(Obj,Functor,Arity), functor(DummyObj,Functor,Arity),
    current_predicate(Functor,DummyObj),
    !, call(Obj).
tgif_chk_output(_OutputObj,_Obj).

% --------------------------------------------------------------------- %

tgif_strs([]) :- !.
tgif_strs([Str|Strs]) :- atom_chars(_Line,Str), !, tgif_strs(Strs).

% --------------------------------------------------------------------- %

tgif_strseg(Obj) :- tgif_strseg(Obj,_).

tgif_strseg(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = str_seg(_Color,_FontName,_TextStyle,_FontSzUnit,
        _W,_Asc,_Des,_MinLBearing,_MaxRExtra,_UnderlineOn,
        _DoubleByte,_DBModBytes,_DBVertical,_Direction,Str),
    tgif_chk_output(OutputObj,Obj),
    Parms = [color=_Color,font_name=_FontName,text_style=_TextStyle,
        font_sz_unit=_FontSzUnit,w=_W,asc=_Asc,des=_Des,
        min_lbearing=_MinLBearing,max_rextra=_MaxRExtra,
        underline_on=_UnderlineOn,double_byte=_DoubleByte,
        db_mod_bytes=_DBModBytes,db_vertical=_DBVertical,direction=_Direction,
        str=Str],
    atom_chars(_Str,Str).

% --------------------------------------------------------------------- %

tgif_strblock_list([SimpleStrBlock]) :-
    !, tgif_simple_strblock(SimpleStrBlock).
tgif_strblock_list([SimpleStrBlock1,SimpleStrBlock2|StrBlocks]) :-
    tgif_simple_strblock(SimpleStrBlock1),
    tgif_simple_strblock(SimpleStrBlock2),
    !, tgif_strblock_list([SimpleStrBlock2|StrBlocks]).
tgif_strblock_list([SimpleStrBlock,ComplexStrBlock|StrBlocks]) :-
    tgif_simple_strblock(SimpleStrBlock),
    tgif_complex_strblock(ComplexStrBlock),
    !, tgif_strblock_list(StrBlocks).

tgif_simple_strblock(StrBlock) :- tgif_simple_strblock(StrBlock,_).

tgif_simple_strblock(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = str_block(0,_W,_Asc,_Des,_MinLBearing,_MaxRExtra,_,_,_,
        [_StrSeg]),
    tgif_chk_output(OutputObj,Obj),
    Parms = [w=_W,asc=_Asc,des=_Des,min_lbearing=_MinLBearing,
        max_rextra=_MaxRExtra,str_seg=_StrSeg],
    tgif_strseg(_StrSeg).

tgif_complex_strblock(StrBlock) :- tgif_complex_strblock(StrBlock,_).

tgif_complex_strblock(StrBlock,Parms) :-
    tgif_space_strblock(StrBlock,Parms), !.
tgif_complex_strblock(StrBlock,Parms) :-
    tgif_center_supsub_strblock(StrBlock,Parms), !.
tgif_complex_strblock(StrBlock,Parms) :-
    tgif_leftright_supsub_strblock(StrBlock,Parms), !.

tgif_space_strblock(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = str_block(4,_W,_,_,_,_,_SpaceW,0,0),
    !, tgif_chk_output(OutputObj,Obj),
    Parms = [type=4,w=_W,space_w=_SpaceW].

tgif_center_supsub_strblock(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = str_block(2,_W,_Asc,_Des,_MinLBearing,_MaxRExtra,_,1,1,
        [_SupMinilines,_SubMinilines,_StrSeg]),
    !, tgif_chk_output(OutputObj,Obj),
    Parms = [type=2,w=_W,asc=_Asc,des=_Des,min_lbearing=_MinLBearing,
        max_rextra=_MaxRExtra,sup_minilines=_SupMinilines,
        _sub_minilines=_SubMinilines,str_seg=_StrSeg],
    !, tgif_minilines(_SupMinilines),
    !, tgif_minilines(_SubMinilines),
    !, tgif_strseg(_StrSeg).

tgif_leftright_supsub_strblock(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = str_block(_Type,_W,_Asc,_Des,_MinLBearing,_MaxRExtra,_,1,1,
        [_SupMinilines,_SubMinilines]),
    (   _Type == 1 | _Type == 3 ),
    !, tgif_chk_output(OutputObj,Obj),
    Parms = [type=_Type,w=_W,asc=_Asc,des=_Des,min_lbearing=_MinLBearing,
        max_rextra=_MaxRExtra,sup_minilines=_SupMinilines,
        _sub_minilines=_SubMinilines],
    !, tgif_minilines(_SupMinilines),
    !, tgif_minilines(_SubMinilines).

% --------------------------------------------------------------------- %

tgif_miniline_list([]) :- !.
tgif_miniline_list([Miniline|MinilineList]) :-
    tgif_miniline(Miniline), !, tgif_miniline_list(MinilineList).

tgif_miniline(Obj) :- tgif_miniline(Obj,_).

tgif_miniline(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = mini_line(_W,_Asc,_Des,_MinLBearing,_MaxRExtra,_VGap,_StrBlockList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [w=_W,asc=_Asc,des=_Des,min_lbearing=_MinLBearing,
        max_rextra=_MaxRExtra,v_gap=_VGap,
        miniline_list=_StrBlockList],
    tgif_strblock_list(_StrBlockList).

% --------------------------------------------------------------------- %

tgif_minilines_list([]) :- !.
tgif_minilines_list([Minilines|MinilinesList]) :-
    tgif_minilines(Minilines), !, tgif_minilines_list(MinilinesList).

tgif_minilines(Obj) :- tgif_minilines(Obj,_).

tgif_minilines(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = minilines(_W,_H,_MinLBearing,_MaxRExtra,_Just,_VSpace,
        _BaselineOffset,_MiniLineList),
    tgif_chk_output(OutputObj,Obj),
    Parms = [w=_W,h=_H,min_lbearing=_MinLBearing,max_rextra=_MaxRExtra,
        just=_Just,v_space=_VSpace,baseline_offset=_BaselineOffset,
        miniline_list=_MiniLineList],
    tgif_miniline_list(_MiniLineList).

% --------------------------------------------------------------------- %

tgif_obj(Obj) :- var(Obj), !, tgif_obj(_,Obj).
tgif_obj(Obj) :- functor(Obj,Functor,_Arity), tgif_obj(Functor,Obj).

tgif_obj(text,Obj) :- tgif_text(Obj).
tgif_obj(box,Obj) :- tgif_box(Obj).
tgif_obj(oval,Obj) :- tgif_oval(Obj).
tgif_obj(poly,Obj) :- tgif_poly(Obj).
tgif_obj(polygon,Obj) :- tgif_polygon(Obj).
tgif_obj(rcbox,Obj) :- tgif_rcbox(Obj).
tgif_obj(arc,Obj) :- tgif_arc(Obj).
tgif_obj(xbm,Obj) :- tgif_xbm(Obj).
tgif_obj(xpm,Obj) :- tgif_xpm(Obj).
tgif_obj(group,Obj) :- tgif_group(Obj).
tgif_obj(sym,Obj) :- tgif_sym(Obj).
tgif_obj(icon,Obj) :- tgif_icon(Obj).

tgif_objs([]) :- !.
tgif_objs([Obj|Objs]) :- tgif_obj(Obj), !, tgif_objs(Objs).

% --------------------------------------------------------------------- %

tgif_attrs([]) :- !.
tgif_attrs([Attr|Attrs]) :- tgif_attr(Attr), !, tgif_attrs(Attrs).

tgif_attr(Attr) :- tgif_attr(Attr,_).

tgif_attr(Attr,Parms) :-
    Attr = attr(_Name,_Value,_ShowAll,_NameShown,_Inherited,TextObj),
    tgif_text(TextObj),
    Parms = [name=_Name,value=_Value,show_all=_ShowAll,
        name_shown=_NameShown,inherited=_Inherited,text_obj=TextObj],
    tgif_text(TextObj).

% --------------------------------------------------------------------- %

tgif_arrow(Obj) :- tgif_arrow(Obj,_).

tgif_arrow(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 37, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = [_ArrowHeadStyle,_ArrowHeadW,_ArrowHeadH,_ArrowHeadIndent,
        _ArrowHeadWSpec,_ArrowHeadHSpec,_ArrowHeadIndentSpec],
    tgif_chk_output(OutputObj,Obj),
    Parms = [arrow_head_style=_ArrowHeadStyle,
            arrow_head_w=_ArrowHeadW,arrow_head_h=_ArrowHeadH,
            arrow_head_indent=_ArrowHeadIndent,
            arrow_head_w_spec=_ArrowHeadWSpec,
            arrow_head_h_spec=_ArrowHeadHSpec,
            arrow_head_indent_spec=_ArrowHeadIndentSpec].

% --------------------------------------------------------------------- %

tgif_color_info([]) :- !.
tgif_color_info([ColorChar,ColorName,_Red,_Green,_Blue|ColorInfos]) :-
    atom_chars(_Char,ColorChar),
    atom_chars(_Color,ColorName),
    !, tgif_color_info(ColorInfos).

% --------------------------------------------------------------------- %

tgif_pixels([]) :- !.
tgif_pixels([_RowOfChar|Pixels]) :- !, tgif_pixels(Pixels).

% --------------------------------------------------------------------- %

tgif_obj_transform_spec(Spec,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    Spec = [_OrigX,_OrigY,_OrigLtX,_OrigLtY,_OrigRbX,_OrigRbY,
        _CTM_SX,_CTM_SIN,_CTM_MSIN,_CTM_SY,_CTM_TX,_CTM_TY],
    Parms = [orig_x=_OrigX,orig_x=_OrigY,orig_ltx=_OrigLtX,
        orig_lty=_OrigLtY,orig_rbx=_OrigRbX,orig_rby=_OrigRbY,
        ctm_sx=_CTM_SX,ctm_sin=_CTM_SIN,ctm_msin=_CTM_MSIN,
        ctm_sy=_CTM_SY,ctm_tx=_CTM_TX,ctm_ty=_CTM_TY].

tgif_text_transform_spec(Spec,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    Spec = [_OrigX,_OrigY,
        _OrigOBBoxLtX,_OrigOBBoxLtY,_OrigOBBoxRbX,_OrigOBBoxRbY,
        _CTM_SX,_CTM_SIN,_CTM_MSIN,_CTM_SY,_CTM_TX,_CTM_TY,
        _OrigBBoxLtX,_OrigBBoxLtY,_OrigBBoxRbX,_OrigBBoxRbY],
    Parms = [orig_x=_OrigX,orig_x=_OrigY,orig_obbox_ltx=_OrigOBBoxLtX,
        orig_obbox_lty=_OrigOBBoxLtY,orig_obbox_rbx=_OrigOBBoxRbX,
        orig_obbox_rby=_OrigOBBoxRbY,
        ctm_sx=_CTM_SX,ctm_sin=_CTM_SIN,ctm_msin=_CTM_MSIN,
        ctm_sy=_CTM_SY,ctm_tx=_CTM_TX,ctm_ty=_CTM_TY,
        orig_bbox_ltx=_OrigBBoxLtX,orig_bbox_lty=_OrigBBoxLtY,
        orig_bbox_rbx=_OrigBBoxRbX,orig_bbox_rby=_OrigBBoxRbY].

% ======================= non=graphical objects ======================= %

% --------------------------------------------------------------------- %

tgif_page(Obj) :- tgif_page(Obj,_).

tgif_page(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion =< 32, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = page(_PageNum,_PageName),
    tgif_chk_output(OutputObj,Obj),
    Parms = [page_num=_PageNum,page_name=_PageName].
tgif_page(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    (   Obj = page(_PageNum,_PageName,_)
    |   Obj = page(_PageNum,_PageName,_,_)
    ),
    tgif_chk_output(OutputObj,Obj),
    Parms = [page_num=_PageNum,page_name=_PageName].

% --------------------------------------------------------------------- %

tgif_unit(Obj) :- tgif_unit(Obj,_).

tgif_unit(Obj,Parms) :-
    current_predicate(tgif_file_version,tgif_file_version(_)),
    tgif_file_version(FileVersion),
    FileVersion >= 33, !,
    ( var(Obj) -> OutputObj = true ; OutputObj = false ),
    Obj = unit(_UnitSpec),
    tgif_chk_output(OutputObj,Obj),
    Parms = [unit_spec=_UnitSpec].
