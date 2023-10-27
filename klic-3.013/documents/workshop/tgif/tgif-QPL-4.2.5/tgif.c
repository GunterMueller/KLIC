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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgif.c,v 1.17 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TGIF_C_

#include "tgifdefs.h"

#include "color.e"
#include "dialog.e"
#include "exec.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "mainloop.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "page.e"
#include "raster.e"
#include "setup.e"
#include "strtbl.e"
#include "tangram2.e"
#include "util.e"
#include "version.e"

/*
 * extern int	malloc_debug ARGS_DECL((int));
 */

int	lastFile=TRUE;

short pDrawFontAsc[] = {
    8, 10, 12, 14, 17, 22, 
    8, 10, 12, 14, 17, 22, 
    8, 10, 12, 14, 17, 23, 
    8, 10, 12, 14, 17, 22, 
    8,  9, 11, 13, 15, 19, 
    7,  9, 11, 12, 15, 20, 
    7,  9, 11, 13, 15, 19, 
    7,  9, 11, 12, 15, 20, 
    9, 11, 12, 14, 18, 24, 
    9, 11, 12, 14, 17, 24, 
    9, 11, 12, 14, 17, 24, 
    9, 11, 12, 14, 17, 24, 
    8, 11, 12, 14, 18, 23, 
    8, 11, 12, 15, 18, 24, 
    8, 11, 12, 14, 16, 23, 
    8, 11, 12, 14, 16, 24, 
    8, 10, 12, 14, 18, 24, 
    9, 14, 15, 18, 23, 30, 
   10, 14, 16, 17, 23, 30, 
    9, 13, 15, 17, 22, 30, 
    9, 13, 15, 18, 22, 30, 
    9, 12, 14, 15, 19, 26, 
    9, 12, 14, 15, 21, 26, 
    9, 12, 14, 15, 19, 26, 
    9, 12, 14, 15, 20, 26, 
   11, 14, 16, 18, 24, 31, 
   11, 14, 16, 18, 24, 31, 
   11, 14, 16, 18, 24, 31, 
   11, 14, 16, 18, 24, 31, 
   11, 14, 16, 19, 24, 32, 
   11, 15, 16, 19, 24, 33, 
   11, 14, 16, 18, 23, 32, 
   11, 15, 16, 19, 24, 32, 
   11, 12, 13, 14, 19, 27 
};
short pDrawFontDes[] = {
    2,  3,  3,  4,  4,  6, 
    2,  3,  3,  4,  4,  6, 
    2,  3,  3,  4,  5,  6, 
    2,  3,  3,  3,  4,  6, 
    2,  2,  3,  3,  4,  5, 
    2,  2,  3,  4,  5,  5, 
    2,  2,  3,  3,  4,  5, 
    2,  2,  3,  4,  5,  5, 
    2,  2,  3,  3,  4,  5, 
    2,  2,  3,  3,  5,  5, 
    2,  2,  3,  3,  5,  5, 
    2,  2,  3,  3,  5,  5, 
    2,  2,  3,  3,  4,  5, 
    2,  2,  3,  3,  4,  5, 
    2,  2,  3,  3,  6,  5, 
    2,  2,  3,  3,  6,  5, 
    3,  4,  4,  6,  7,  8, 
    3,  3,  4,  4,  6,  7, 
    3,  3,  4,  4,  6,  7, 
    3,  4,  4,  5,  6,  7, 
    3,  3,  4,  4,  6,  7, 
    2,  3,  3,  4,  5,  6, 
    2,  4,  3,  5,  5,  7, 
    2,  3,  4,  4,  5,  6, 
    2,  4,  4,  5,  5,  7, 
    2,  3,  4,  4,  5,  7, 
    2,  3,  4,  5,  5,  7, 
    2,  3,  4,  5,  5,  7, 
    2,  3,  4,  5,  5,  7, 
    2,  3,  3,  4,  5,  7, 
    2,  3,  3,  4,  5,  7, 
    2,  3,  3,  4,  5,  7, 
    2,  3,  3,  4,  5,  7, 
    4,  3,  4,  5,  5,  7 
};

static
void SetProgramName(FileName)
   char *FileName;
{
   char *c_ptr=UtilStrRChr(FileName, DIR_SEP);

   if (c_ptr != NULL && *(++c_ptr) != '\0') {
      UtilStrCpyN(progName, sizeof(progName), c_ptr);
   } else {
      UtilStrCpyN(progName, sizeof(progName), TOOL_NAME);
      UtilStrLower(progName);
   }
}

static
int PrTgifLoad(FileName)
   char *FileName;
{
   struct ObjRec *obj_ptr=NULL;
   char full_name[MAXPATHLENGTH+1], gzipped_fname[MAXPATHLENGTH+1];
   char tmp_filename[MAXPATHLENGTH+1], tmp_filefullpath[MAXPATHLENGTH+1];
   int read_status, obj_file=TRUE;
   FILE *fp=NULL;
   int tmp_linenum=0, gzipped=FALSE;

   if (strcmp(FileName, "-") == 0) {
      return TRUE;
   }
   *gzipped_fname = '\0';
   if (FileNameHasExtension(FileName, OBJ_FILE_TYPE, &gzipped, NULL)) {
      if (gzipped) {
         if (cmdLineOpenDisplay) {
            char *tmp_fname=NULL;

            if ((tmp_fname=GunzipFileIntoTemp(FileName)) == NULL) {
               return FALSE;
            } else {
               strcpy(gzipped_fname, FileName);
               strcpy(full_name, tmp_fname);
               free(tmp_fname);
            }
         } else {
            fprintf(stderr, TgLoadString(STID_TOOL_DONT_KNOW_HOW_RUN_GUNZIP),
                  TOOL_NAME);
            fprintf(stderr, "\n");
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_SPECIFY_DISPLAY_ON_CMD_LINE));
            return FALSE;
         }
      } else {
         strcpy(full_name, FileName);
      }
   } else if (FileNameHasExtension(FileName, SYM_FILE_TYPE, NULL, NULL)) {
      strcpy(full_name, FileName);
      obj_file = FALSE;
   } else if (FileNameHasExtension(FileName, PIN_FILE_TYPE, NULL, NULL)) {
      strcpy(full_name, FileName);
      obj_file = FALSE;
   } else {
      sprintf(full_name, "%s.%s", FileName, OBJ_FILE_EXT);
   }
   if ((fp=fopen(full_name, "r")) == NULL) {
      fprintf(stderr, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            full_name);
      fprintf(stderr, "\n");
      if (*gzipped_fname != '\0') unlink(full_name);
      return FALSE;
   }

   strcpy(tmp_filefullpath, scanFileFullPath);
   strcpy(tmp_filename, scanFileName);
   tmp_linenum = scanLineNum;
   UtilStrCpyN(scanFileFullPath, sizeof(scanFileFullPath), full_name);
   strcpy(scanFileName, full_name);
   scanLineNum = 0;

   if (!(PRTGIF && cmdLineStdOut)) {
      if (!cmdLineQuiet) {
         fprintf(stderr, TgLoadCachedString(CSTID_READING_FILE_DOTS),
               full_name);
         fprintf(stderr, "\n");
      }
   }
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         AdjForOldVersion(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
      }
   }

   strcpy(scanFileFullPath, tmp_filefullpath);
   strcpy(scanFileName, tmp_filename);
   scanLineNum = tmp_linenum;

   fclose(fp);

   if (read_status == INVALID) {
      fprintf(stderr, TgLoadString(STID_FILE_VER_ABORT_TOOL),
            fileVersion, TOOL_NAME, TOOL_NAME, homePageURL);
      if (*gzipped_fname != '\0') unlink(full_name);
      return FALSE;
   }
   if (*gzipped_fname != '\0') {
      unlink(full_name);
      strcpy(full_name, gzipped_fname);
   }
   if (cmdLineOpenDisplay || *cmdLineGenParser != '\0') {
      char file_path[MAXPATHLENGTH+1];

      if (*full_name == DIR_SEP) {
         strcpy(file_path, full_name);
      } else {
         sprintf(file_path, "%s%c%s", curDir, DIR_SEP, full_name);
      }
      curFileDefined = TRUE;
      if (obj_file) {
         SetCurDir(file_path);
         *curSymDir = '\0';
      } else {
         SetCurSymDir(file_path);
      }
   }
   if (cmdLineHasPageNum) {
      if (cmdLinePageNum > lastPageNum) {
         if (lastPageNum == 1) {
            fprintf(stderr, TgLoadString(STID_FILE_SKIP_CONTAINS_ONE_PAGE),
                  FileName);
         } else {
            fprintf(stderr, TgLoadString(STID_FILE_SKIP_CONTAINS_NUM_PAGE),
                  FileName, lastPageNum);
         }
         fprintf(stderr, "\n");
         return FALSE;
      }
      GotoPageNum(cmdLinePageNum);
   }
   if (cmdLineColor && colorMenuItems == NULL) {
      fprintf(stderr, TgLoadString(STID_FILE_NOT_CONTAIN_COLOR_ABORT),
            TOOL_NAME);
      fprintf(stderr, "\n");
      return FALSE;
   }
   if (loadedCurPageNum > 0 && curPage != NULL && lastPageNum > 1) {
      /*
       * Well, it should go to loadedCurPageNum because it was the page
       * at which the file was saved.  But we go to lastPageNum to stay
       * compatible with Tangram-II since it was expected for tgif to
       * behave this way.
       */
      GotoPageNum(lastPageNum);
   }
   return TRUE;
}

static
void PrTgifDump(inbuf, tile_page_err_msg_id)
   char *inbuf;
   int tile_page_err_msg_id;
{
   if (inbuf != NULL && strcmp(inbuf, "-") != 0) {
      UtilStrCpyN(cmdLineOpenFile, sizeof(cmdLineOpenFile), inbuf);
   }
   if (cmdLineNoShowPageInEPS) showPageInEPS = FALSE;
   if (*cmdLineFileToExec != '\0') {
      if (!cmdLineQuiet) {
         fprintf(stderr, TgLoadString(STID_EXECUTING_GIVEN_SCRIPT),
               cmdLineFileToExec);
         fprintf(stderr, "\n");
      }
      StartExecCmdsFromFile();
   } else {
      colorDump = cmdLineColor;
      if (cmdLineOpenDisplay) {
         whereToPrint = cmdLineWhereToPrint;
      }
      if (cmdLineOneFilePerPage) {
         if (pageLayoutMode == PAGE_TILE) {
            fprintf(stderr, "%s\n", TgLoadString(tile_page_err_msg_id));
         } else {
            if (cmdLineHasPageNum) {
               sprintf(cmdLinePageNumStr, "%1d", cmdLinePageNum);
               GotoPageNum(cmdLinePageNum);
               if (inbuf != NULL) Dump(inbuf);
            } else {
               cmdLineHasPageNum = TRUE;
               for (cmdLinePageNum=1; cmdLinePageNum <= lastPageNum;
                     cmdLinePageNum++) {
                  sprintf(cmdLinePageNumStr, "%1d", cmdLinePageNum);
                  GotoPageNum(cmdLinePageNum);
                  if (inbuf != NULL) Dump(inbuf);
               }
               cmdLineHasPageNum = FALSE;
            }
         }
      } else {
         if (inbuf != NULL) Dump(inbuf);
      }
   }
   *cmdLineOpenFile = '\0';
   DelAllObj();
   if (cmdLineColor) CleanUpColors();
   if (cmdLineOpenDisplay) {
      CleanUp();
   }
}

static
void DoPrTgif(argc, argv, from_prtgif)
   int argc, from_prtgif;
   char *argv[];
{
   char inbuf[MAXSTRING+1];
   int len, argc_to_be;

   *scanFileFullPath = *scanFileName = '\0';
   scanLineNum = 0;

   if ((argc_to_be=ProcessPrTgifOptions(argc, argv, from_prtgif)) == INVALID) {
      return;
   }
   while (argc > argc_to_be) {
      argc--; argv++;
   }
   InitPaperSize();
   InitTmpDir();
   InitPatFill();
   if (PRTGIF && !cmdLineOpenDisplay) {
      InitDontReencode(NULL);
      InitPSFontCharSubs();
   }
   if (*cmdLineGenParser != '\0') {
      if (cmdLineTgrm2) {
         InitNames();
      }
   }
   if (argc <= 0) {
      if (cmdLineStdOut) {
         fprintf(stderr, "\n%s\n",
               TgLoadString(STID_SPEC_FILE_WHEN_STDOUT_USED));
         return;
      } else if (strcmp(cmdLineFileToExec, "-") == 0) {
         if (cmdLineOpenDisplay) {
            JustInit(NULL, NULL);
            Init2PatFill();
         }
         PrTgifDump(NULL, STID_CANT_ONEFPPAGE_IN_TILED_PAGE);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_TOOL_FILE_NAME_TO_PRINT),
               TOOL_NAME);
         fprintf(stderr, "\n%s> ", gszMsgBox);
         fflush(stderr);
         while (fgets(inbuf, MAXSTRING, stdin) != NULL) {
            len = strlen(inbuf);
            if (len > 0) {
               if (inbuf[--len] == '\n') inbuf[len] = '\0';
               if (*cmdLineGenParser != '\0') {
                  if (cmdLineTgrm2) {
                     if (PrTgifLoad(inbuf)) {
                        Tangram2GenerateChain(cmdLineGenParser);
                        DelAllObj();
                     } else {
                        fprintf(stderr, "\n");
                     }
                  }
               } else if (cmdLineDumpURL) {
                  UrlDump(inbuf);
               } else if (cmdLineDosEpsFilter) {
                  UtilStrCpyN(cmdLineOpenFile, sizeof(cmdLineOpenFile), inbuf);
                  DosEpsFilter(inbuf);
               } else {
                  if (cmdLineOpenDisplay) {
                     JustInit(NULL, NULL);
                  }
                  if (PrTgifLoad(inbuf)) {
                     PrTgifDump(inbuf, STID_CANT_ONEFPPAGE_IN_TILED_PAGE);
                  } else {
                     fprintf(stderr, "\n");
                  }
               }
            }
            sprintf(gszMsgBox, TgLoadString(STID_TOOL_FILE_NAME_TO_PRINT),
                  TOOL_NAME);
            fprintf(stderr, "\n%s> ", gszMsgBox);
            fflush(stderr);
         }
         fprintf(stderr, "\n");
         fflush(stderr);
      }
   } else {
      if (cmdLineStdOut && argc > 2) {
         fprintf(stderr, "\n%s\n",
               TgLoadString(STID_ONE_FILE_ONLY_WITH_STDOUT));
         return;
      }
      for ( ; argc > 0; argc--, argv++) {
         lastFile = (argc == 1);
         if (*cmdLineGenParser != '\0') {
            if (cmdLineTgrm2) {
               if (PrTgifLoad(*argv)) {
                  Tangram2GenerateChain(cmdLineGenParser);
                  DelAllObj();
               } else {
                  fprintf(stderr, "\n");
               }
            }
         } else if (cmdLineDumpURL) {
            UrlDump(*argv);
            if (argc > 1) {
               fprintf(stderr, "%s\n",
                     TgLoadString(STID_ONE_FILE_ONLY_RAW_PLUS_H));
            }
            break;
         } else if (cmdLineDosEpsFilter) {
            UtilStrCpyN(cmdLineOpenFile, sizeof(cmdLineOpenFile), *argv);
            DosEpsFilter(*argv);
            if (argc > 1) {
               fprintf(stderr, "%s\n",
                     TgLoadString(STID_ONE_FILE_ONLY_DOSEPSFILTER));
            }
            break;
         } else {
            if (cmdLineOpenDisplay) {
               JustInit(NULL, NULL);
            }
            if (PrTgifLoad(*argv)) {
               PrTgifDump(*argv, STID_CANT_PRTGIF_OFPP_TILED_PAGE);
            } else {
               fprintf(stderr, "\n");
            }
         }
      }
   }
   if (PRTGIF && !cmdLineOpenDisplay) CleanUpFonts();
   CleanUpPage();
   CleanUpResiduals();
   CleanUpPatFill();
   CleanUpStrTable();
}

/*
 * static
 * void Prompt2 (PromptStr, OpName, FileName)
 *    char	* PromptStr, * OpName, * FileName;
 * {
 *    char	inbuf[80];
 * 
 *    printf (PromptStr);
 *    fgets (inbuf, 80, stdin);
 *    sscanf (inbuf, "%s%s", OpName, FileName);
 * }
 *
 * static
 * void Prompt3 (PromptStr, AttrName, ColorName, ValName)
 *    char	* PromptStr, * AttrName, * ColorName, * ValName;
 * {
 *    char	inbuf[80];
 * 
 *    printf (PromptStr);
 *    fgets (inbuf, 80, stdin);
 *    sscanf (inbuf, "%s%s%s", AttrName, ColorName, ValName);
 * }
 */

static
void DoTgif(argc, argv)
   int argc;
   char *argv[];
   /* All these strangeness with strings are related to */
   /*    Prolog's foreign function interface. */
{
/*
 * register int i;
 */
   char op_name[80], file_name[(MAXPATHLENGTH<<1)+1];
   char *sp[6], *func_strp;
/*
 * char	color_name[80], val_name[80];
 * char	attr_name[80], speed_name[80], id_name[80];
 */

   if (!ProcessTgifOptions(argc, argv, file_name, sizeof(file_name))) return;

   if (file_name[0] == '\0') {
      /* do not translate -- program constants */
      MainLoop("init", "", &func_strp,
            &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
   } else {
      /* do not translate -- program constants */
      MainLoop("init", file_name, &func_strp,
            &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
   }
/*
 * for (i = 0; i < 6; i++)
 *    if (strcmp (sp[i], "") != 0)
 *       printf ("%s ", sp[i]);
 *    else
 *       break;
 * printf ("\n");
 */

   while (TRUE) {
      char s[80];

      strcpy(s, func_strp);
      s[4] = '\0';

      DeallocStrings(&func_strp,&sp[0],&sp[1],&sp[2],&sp[3],&sp[4],&sp[5]);

      /* do not translate -- program constants */
      if (strcmp(s, "Quit") == 0) {
         *file_name = '\0';
         MainLoop("quit", file_name, &func_strp,
               &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
         DeallocStrings(&func_strp,&sp[0],&sp[1],&sp[2],&sp[3],&sp[4],&sp[5]);
         break;
      }

      Msg(TgLoadCachedString(CSTID_RETURNED_FROM_BASIC_DRIVER));
/*
 *    Prompt2("Input an operation and a sub command.\n",op_name,file_name);
 *
 *    if (strcmp(op_name, "animate") == 0)
 *    {
 *       Prompt3("Input poly_id, speed, color.\n", id_name, speed_name,
 *             color_name);
 *       Animate(file_name, id_name, speed_name, color_name, &func_strp);
 *       printf("Animate RETURNs --> %s %s %s\n", func_strp, sp[0], sp[1]);
 *    }
 *    if (strcmp(op_name, "upd_attr_val") == 0)
 *    {
 *       Prompt3("Input attrname, color and value.\n", attr_name, color_name,
 *             val_name);
 *       UpdAttrVal(file_name, attr_name, color_name, val_name, &func_strp);
 *       printf("UpdAttrVal RETURNs --> %s %s %s\n", func_strp, sp[0], sp[1]);
 *    }
 */
      *op_name = *file_name = '\0';
      MainLoop(op_name, file_name, &func_strp,
            &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
/*
 *    printf("RETURN --> %s ", func_strp);
 *    for (i = 0; i < 6; i++)
 *       if (strcmp(sp[i], "") != 0)
 *          printf("%s ", sp[i]);
 *       else
 *          break;
 *    printf("\n");
 */
   }
}

int main(argc, argv)
   int argc;
   char *argv[];
{
   register int i, from_prtgif;

   origArgC = argc;
   origArgV = argv;

   PRTGIF = FALSE;
   from_prtgif = FALSE;
   SetProgramName(*argv);
   for (i = 1; i < argc; i++) {
      /* do not translate -- program constants */
      if (strcmp(argv[i], "-prtgif")==0) {
         PRTGIF = TRUE;
         from_prtgif = TRUE;
         break;
      } else if ((strcmp(argv[i], "-print")==0) ||
            (strcmp(argv[i], "-PRINT")==0)) {
         PRTGIF = TRUE;
         break;
      }
   }
/*
 * malloc_debug (1);
 */
   if (PRTGIF) {
      DoPrTgif(argc, argv, from_prtgif);
   } else {
      DoTgif(argc, argv);
   }
   return 0;
}
