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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/testdrive.c,v 1.6 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_TESTDRIVE_C_

#include "tgifdefs.h"

#include "mainloop.e"
#include "msg.e"
#include "obj.e"
#include "setup.e"

/*
 * extern int	malloc_debug ARGS_DECL((int));
 */

int	lastFile=TRUE;
short	*pDrawFontAsc=NULL;
short	*pDrawFontDes=NULL;

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
void PrintObjId(ObjPtr, Level)
   struct ObjRec *ObjPtr;
   int Level;
{
   register int i;
   int id=ObjPtr->id;
   struct ObjRec *obj_ptr;
   struct AttrRec *attr_ptr;

   for (i = 0; i < Level; i++) printf("   ");
   switch (ObjPtr->type) {
   /* these are all the tgif object types */
   case OBJ_POLY: printf("poly: %1d\n", id); break;
   case OBJ_BOX: printf("box: %1d\n", id); break;
   case OBJ_OVAL: printf("oval: %1d\n", id); break;
   case OBJ_TEXT: printf("text: %1d\n", id); break;
   case OBJ_ARC: printf("arc: %1d\n", id); break;
   case OBJ_RCBOX: printf("rcbox: %1d\n", id); break;
   case OBJ_XBM: printf("xbm: %1d\n", id); break;
   case OBJ_XPM: printf("xpm: %1d\n", id); break;
   case OBJ_POLYGON: printf("polygon: %1d\n", id); break;
   case OBJ_GROUP: printf("group: %1d\n", id); break;
   case OBJ_SYM: printf("sym: %1d\n", id); break;
   case OBJ_ICON: printf("icon: %1d\n", id); break;
   case OBJ_PIN: printf("pin: %1d\n", id); break;
   }
   if (ObjPtr->type == OBJ_GROUP || ObjPtr->type == OBJ_SYM ||
         ObjPtr->type == OBJ_ICON || ObjPtr->type == OBJ_PIN) {
      /* for these composite objects, each on maintains a LIST of objects */
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr=obj_ptr->prev) {
         PrintObjId(obj_ptr, Level+1);
      }
   }
   /* all object can have a list of attributes; the head    */
   /*     of the list is pointed to by the fattr and the    */
   /*     tail of the list is pointed to by the lattr field */
   if ((attr_ptr=ObjPtr->lattr) != NULL) {
      for (i = 0; i < Level+1; i++) printf("   ");
      printf("attrs:\n");

      /* each attribute entry contains a TEXT object */
      for ( ; attr_ptr != NULL; attr_ptr=attr_ptr->prev) {
         PrintObjId(attr_ptr->obj, Level+2);
      }
   }
}

#define DO_CMD(cmd) ExecACommandFromBuffer(cmd,NULL)

static
void ScriptDemo()
{
   MakeQuiescent(); /* select nothing, edit nothing */

   if (BeginExecCommandsFromBuffer()) {
      int i=0;

      /* set current color to 'blue' */
      DO_CMD("set_selected_obj_color(\"blue\")");
      /* create a 200 pixels by 100 pixels blue box */
      DO_CMD("create_box_obj(100,100,300,200)");
      /* select the top object, which is the box object */
      DO_CMD("select_top_obj()");
      /* name the box object "the_box" */
      DO_CMD("add_attr_to_selected_obj(\"name\",\"the_box\",100,100)");
      /* hide all the attributes of the box object */
      DO_CMD("call_simple_shortcut(\"HideAttr\")");
      /* unselect everything */
      DO_CMD("unselect_all_obj()");

      /* set current text justification to center justified */
      DO_CMD("set_selected_text_just(\"center\")");
      /* set current text font to Helvetica Bold */
      DO_CMD("set_selected_text_font(\"Helvetica-Bold\")");
      /* set current text size to 14 */
      DO_CMD("set_selected_text_size(14)");
      /* create a text object that sits on top the blue box */
      DO_CMD("create_text_obj(200,100,\"My Box\")");

      /* select the box object */
      DO_CMD("select_obj_by_name(\"the_box\")");
      /* sleep for 500ms */
      DO_CMD("sleep(NULL,500)");
      /* change the box to 'green' */
      DO_CMD("set_selected_obj_color(\"green\")");

      /* cycle through all the fill patterns */
      for (i=31; i >= 0; i--) {
         char buf[80];

         sprintf(buf, "set_selected_obj_fill(%1d)", i);
         DO_CMD(buf);
      }
      /* sleep for another 500ms */
      DO_CMD("sleep(NULL,500)");
      /* change the box to 'red' */
      DO_CMD("set_selected_obj_color(\"red\")");
      /* sleep for another 500ms */
      DO_CMD("sleep(NULL,500)");

      /* lock the box */
      DO_CMD("call_simple_shortcut(\"Lock\")");

      /* select the top object, which is the text object */
      DO_CMD("select_top_obj()");
      /* also select the box */
      DO_CMD("select_additional_obj(\"the_box\")");
      /* align the text object so that it sits in the middle of the box */
      DO_CMD("call_simple_shortcut(\"AlignObjsMiddle\")");
      /* unlock the box */
      DO_CMD("call_simple_shortcut(\"UnLock\")");
      /* unselect everything */
      DO_CMD("unselect_all_obj()");

      EndExecCommandsFromBuffer();
   }
}

int main(argc, argv)
   int argc;
   char *argv[];
   /* All these strangeness with strings are related to */
   /*    Prolog's foreign function interface. */
{
   register int i;
   char op_name[80], file_name[(MAXPATHLENGTH<<1)+1];
   char *sp[6], *func_strp;
/*
 * char	color_name[80], val_name[80];
 * char	attr_name[80], speed_name[80], id_name[80];
 */

/*
 * malloc_debug (1);
 */

   if (!ProcessTgifOptions(argc, argv, file_name, sizeof(file_name))) return 1;

   if (file_name[0] == '\0') {
      MainLoop("init", "", &func_strp,
            &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
   } else {
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

      if (strcmp(s, "Quit") == 0) {
         *file_name = '\0';
         MainLoop("quit", file_name, &func_strp,
               &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
         DeallocStrings(&func_strp,&sp[0],&sp[1],&sp[2],&sp[3],&sp[4],&sp[5]);
         break;
      } else if (strcmp(s, "Solv") == 0) {
         struct ObjRec *obj_ptr;

         printf("==============\n");
         printf("Listing IDs...\n");
         printf("==============\n");
         /* botObj points to the last top-level object, and */
         /*     topObj points to the first top-level object */
         for (obj_ptr = botObj; obj_ptr != NULL; obj_ptr = obj_ptr->prev) {
            PrintObjId(obj_ptr, 0);
         }
         printf("\n");
      } else if (strcmp(s, "Anim") == 0) {
         ScriptDemo();
      }

      Msg("Returned from basic driver.");
/*
 *    Prompt2 ("Input an operation and a sub command.\n",op_name,file_name);
 *
 *    if (strcmp (op_name, "animate") == 0)
 *    {
 *       Prompt3 ("Input poly_id, speed, color.\n", id_name, speed_name,
 *             color_name);
 *       Animate (file_name, id_name, speed_name, color_name, &func_strp);
 *       printf ("Animate RETURNs --> %s %s %s\n", func_strp, sp[0], sp[1]);
 *    }
 *    if (strcmp (op_name, "upd_attr_val") == 0)
 *    {
 *       Prompt3 ("Input attrname, color and value.\n", attr_name, color_name,
 *             val_name);
 *       UpdAttrVal (file_name, attr_name, color_name, val_name, &func_strp);
 *       printf ("UpdAttrVal RETURNs --> %s %s %s\n", func_strp, sp[0], sp[1]);
 *    }
 */
      *op_name = *file_name = '\0';
      MainLoop(op_name, file_name, &func_strp,
            &sp[0], &sp[1], &sp[2], &sp[3], &sp[4], &sp[5]);
/*
 *    printf ("RETURN --> %s ", func_strp);
 *    for (i = 0; i < 6; i++)
 *       if (strcmp (sp[i], "") != 0)
 *          printf ("%s ", sp[i]);
 *       else
 *          break;
 *    printf ("\n");
 */
   }
   return 0;
}
