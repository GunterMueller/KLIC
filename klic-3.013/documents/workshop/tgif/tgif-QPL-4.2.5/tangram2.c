/*
 * Author:      Daniel Ratton Figueiredo <ratton@land.ufrj.br>
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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tangram2.c,v 1.19 2011/05/25 16:45:49 cvsps Exp $
 */

#define _INCLUDE_FROM_TANGRAM2_C_

#include <ctype.h>

#include "tgifdefs.h"

#include "attr.e"
#include "auxtext.e"
#include "box.e"
#include "cmd.e"
#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "exec.e"
#include "menu.e"
#include "msg.e"
#include "miniline.e"
#include "move.e"
#include "obj.e"
#include "pattern.e"
#include "rcbox.e"
#include "select.e"
#include "setup.e"
#include "strtbl.e"
#include "tangram2.e"
#include "text.e"
#include "util.e"

char	cmdLineGenParser[MAXSTRING];

#define DO_CMD(cmd) ExecACommandFromBuffer(cmd,NULL)

static TgMenuItemInfo tangram2MenuItemInfo[] = {
   { "About Tangram-II", NULL, "Information about Tangram-II", NULL,
     CMDID_ABOUT_TANGRAM2 },
   { NULL, NULL, NULL, NULL, INVALID }
};
TgMenuInfo tangram2MenuInfo={ TGMUTYPE_TEXT, tangram2MenuItemInfo,
                              CreateTangram2Menu };

/* ======================= Init & CleanUp ======================= */

void CleanUpTangram2()
{
   CleanUpTangram2ShortCut();
}

int InitTangram2()
{
   InitTangram2ShortCut();

   return TRUE;
}

/* ======================= Tangram2 Shortcuts ======================= */

static struct ShortCutRec tangram2ShortCutXlateTbl[] = {
   { '\0', 0, "AboutTangram2()", 0, CMDID_ABOUT_TANGRAM2 },
   { '\0', 0, "Tangram2GenerateChain()", 1, CMDID_TANGRAM2_GENERATE_CHAIN },
   { '\0', 0, "Tangram2StartSimulator()", 1, CMDID_TANGRAM2_START_SIMULATOR },
   { '\0', 0, "Tangram2SimulationStep()", 0, CMDID_TANGRAM2_SIMULATION_STEP },
   { '\0', 0, "Tangram2EndSimulator()", 0, CMDID_TANGRAM2_END_SIMULATOR },
   { '\0', 0, "", 0, 0 }
};

static int tangram2ShortCutCmdIdIndex[MAXTANGRAM2CMDIDS-CMDID_TANGRAM2_BASE];

void CleanUpTangram2ShortCut()
{
}

int InitTangram2ShortCut()
{
   int i=0, num_cmdids=MAXTANGRAM2CMDIDS-CMDID_TANGRAM2_BASE;

   for (i=0; i < num_cmdids; i++) {
      tangram2ShortCutCmdIdIndex[i] = INVALID;
   }
   for (i=0; *(tangram2ShortCutXlateTbl[i].name) != '\0'; i++) {
      int cmdid=tangram2ShortCutXlateTbl[i].cmdid;

      if (cmdid != INVALID) {
         int index=cmdid-CMDID_TANGRAM2_BASE;

         if (tangram2ShortCutCmdIdIndex[index] == INVALID) {
            tangram2ShortCutCmdIdIndex[index] = i;
         } else {
            fprintf(stderr, "Warning:  duplicate cmdid %1d.\n", cmdid);
         }
      }
   }
   return TRUE;
}

int ValidTangram2CmdName(buf, len, pn_num_args)
   char *buf;
   int len, *pn_num_args;
   /*
    * returns 0 if no match
    * otherwise, returns (CMDID_TANGRAM2_BASE|index) where index is the
    *       index into tangram2ShortCutXlateTbl
    */
{
   int i;

   for (i=0; *(tangram2ShortCutXlateTbl[i].name) != '\0'; i++) {
      if (strncmp(tangram2ShortCutXlateTbl[i].name, buf, len) == 0) {
         *pn_num_args = tangram2ShortCutXlateTbl[i].num_args;

         return (i+CMDID_TANGRAM2_BASE);
      }
   }
   return 0;
}

int ValidTangram2CmdId(nCmdId)
   int nCmdId;
{
   int cmd_index=nCmdId-CMDID_TANGRAM2_BASE, xlate_index=INVALID;

   if (nCmdId >= MAXTANGRAM2CMDIDS || nCmdId < CMDID_TANGRAM2_BASE) {
      return FALSE;
   }
   xlate_index = tangram2ShortCutCmdIdIndex[cmd_index];
   if (xlate_index == INVALID ||
         tangram2ShortCutXlateTbl[xlate_index].num_args != 0) {
      return FALSE;
   }
   return TRUE;
}

int DoTangram2Cmd(nCmdId, args)
   int nCmdId;
   char *args;
   /* returns INVALID if the event can be caught by other windows */
{
   switch (nCmdId) {
   case CMDID_ABOUT_TANGRAM2: AboutTangram2(); break;
   case CMDID_TANGRAM2_GENERATE_CHAIN:  Tangram2GenerateChain(args);  break;
   case CMDID_TANGRAM2_START_SIMULATOR: Tangram2StartSimulator(args); break;
   case CMDID_TANGRAM2_SIMULATION_STEP: Tangram2SimulationStep();     break;
   case CMDID_TANGRAM2_END_SIMULATOR:   Tangram2EndSimulator();       break;
   default: break;
   }
   return BAD;
}

int FetchTangram2ShortCutNumArgs(index, pn_num_args) 
   int index, *pn_num_args;
{  
   *pn_num_args = tangram2ShortCutXlateTbl[index].num_args;
   
   return TRUE;
}

int DoTangram2ShortCut(index, args)
   int index;
   char *args;
   /* return FALSE if cannot execute shortcut */
{
   if (index < 0 || index >= MAXTANGRAM2CMDIDS-CMDID_TANGRAM2_BASE) {
      return FALSE;
   }
   if (tangram2ShortCutXlateTbl[index].num_args == 0) {
      DoTangram2Cmd(tangram2ShortCutXlateTbl[index].cmdid, NULL);
   } else {
      DoTangram2Cmd(tangram2ShortCutXlateTbl[index].cmdid, args);
   }
   return TRUE;
}

/* ======================= Tangram2 Internal Commands ======================= */

void ExecStartSimulator ARGS_DECL((struct ObjRec *, char*));
void ExecSimulateStep ARGS_DECL((struct ObjRec *, char*));
void ExecEndSimulator ARGS_DECL((struct ObjRec *, char*));

static
ExecInfo gTangram2ExecInfo[] = {
   { (NLFN*)ExecStartSimulator,      "start_simulator",                   0, 0},
   { (NLFN*)ExecSimulateStep,        "simulate_step",                     0, 0},
   { (NLFN*)ExecEndSimulator,        "end_simulator",                     0, 0},
   { NULL, NULL, 0, 0 }
};

ExecInfo *Tangram2GetExecInfo(func_name)
   char *func_name;
{
   ExecInfo *pei=NULL;

   for (pei=gTangram2ExecInfo; pei->pfunc != NULL; pei++) {
      if (strcmp(pei->func_name, func_name) == 0) {
         return pei;
      }
   }
   return NULL;
}

/* --------------- Tangram2 Internal Commands Implementation --------------- */

#define MAXCONNECTTRIES           3
#define PORT                   6743

static int simulator_socket=INVALID;
static struct ObjRec *objGC=NULL; /* Generate Chain   button */
static struct ObjRec *objSS=NULL; /* Start Simulation button */
static struct ObjRec *objSp=NULL; /* Simulation Step  button */

#define SP_OPT_HAS_VALUE      (1<<0)
#define SP_OPT_HAS_FORMAT     (1<<1)
#define SP_OPT_IN_ARRAY       (1<<2)
#define SP_OPT_FIRST_IN_ARRAY (1<<3)
#define SP_OPT_LAST_IN_ARRAY  (1<<4)

#define CT_CODE_INIT_SIMULATION   0
#define CT_CODE_END_OF_SIMULATION 1
#define CT_CODE_STEP_SIMULATION   2

#define MAXOBJECTNAME           120
#define MAXSYMBOLNAME           120
#define MAXVALUESIZE            240
#define MAXFORMATSIZE           24
#define MAXEVENTNAME            512

#define SKIP_WHITESPACE(x) { while( isspace((int)(*(x))) ) (x)++; }

/* Control Packet */
typedef struct tagT_ControlPacket {
  int code;

  /* info to update TGIF's screen */
  int step;
  double elapsed_time;

  /* number of Symbol packets following */
  int symcount;

  /* name of latest event executed by the simulator */
  char last_event[MAXEVENTNAME];
} T_ControlPacket;

/* Symbol Packet */
typedef struct tagT_SymbolPacket {
  char object_name[MAXOBJECTNAME];
  char symbol_name[MAXSYMBOLNAME];
  int  options;                    /* available options: SP_OPT_HAS_VALUE
                                                         SP_OPT_HAS_FORMAT
                                                         SP_OPT_IN_ARRAY
                                                         SP_OPT_FIRST_IN_ARRAY
                                                         SP_OPT_LAST_IN_ARRAY */
  int  array_index;                /* if SP_OPT_IN_ARRAY this is the index */
  char value_string[MAXVALUESIZE]; /* if SP_OPT_HAS_VALUE this is the value */
  char value_format[MAXFORMATSIZE];
} T_SymbolPacket;

/* Function prototypes */
static int sendSymbolPacket( int, T_SymbolPacket * );
static int recvSymbolPacket( int, T_SymbolPacket * );

static int sendControlPacket( int, T_ControlPacket * );
static int recvControlPacket( int, T_ControlPacket * );

static void addSymToBuffer   ( T_SymbolPacket * );
static void sendAllSymPackets( int );

static T_SymbolPacket * packet_queue = NULL;
static int              packet_queue_size = 0,
                        packet_queue_max_size = 0;

static
int sendSymbolPacket( int soc, T_SymbolPacket * pack_out )
{
    int  bywrite;

    if( (bywrite = write( soc, pack_out, sizeof(T_SymbolPacket) )) < 0 )
    {
        perror( "write" );
        return(-1);
    }
    return(1);
}

static
int recvSymbolPacket( int soc, T_SymbolPacket * pack_in )
{
    int  byread;

    if( (byread = read( soc, pack_in, sizeof(T_SymbolPacket)) ) < 0 )
    {
        perror( "read" );
        return(-1);
    }
    return(1);
}

static
int sendControlPacket( int soc, T_ControlPacket * pack_out )
{
    int  bywrite;

    if( (bywrite = write( soc, pack_out, sizeof(T_ControlPacket) )) < 0 )
    {
        perror( "write" );
        return(-1);
    }
    return(1);
}

static
int recvControlPacket( int soc, T_ControlPacket * pack_in )
{
    int  byread;

    if( (byread = read( soc, pack_in, sizeof(T_ControlPacket)) ) < 0 )
    {
        perror( "read" );
        return(-1);
    }
    return(1);
}

static
void addSymToBuffer( T_SymbolPacket * packet )
{
    if( !packet_queue )
    {
        packet_queue_max_size = 8;
        packet_queue = malloc( packet_queue_max_size *
                               sizeof(T_SymbolPacket) );
    }
    else if( packet_queue_size+1 > packet_queue_max_size ) /* queue overflow */
    {
        packet_queue_max_size <<= 1;
        packet_queue = realloc( packet_queue,
                                packet_queue_max_size *
                                sizeof(T_SymbolPacket) );
    }

    memcpy( packet_queue+packet_queue_size, packet, sizeof(T_SymbolPacket) );
    packet_queue_size++;
}

static
void sendAllSymPackets( int soc )
{
    int i;

    for( i = 0; i < packet_queue_size; i++ )
        sendSymbolPacket( soc, packet_queue+i );

    if( packet_queue )
    {
        free( packet_queue );
        packet_queue = NULL;
        packet_queue_max_size = 0;
    }

    packet_queue_size = 0;
}

static
int BadAttr_Simulator(attr_name, cmd_name)
   char *attr_name, *cmd_name;
{
   char msg[MAXSTRING+1];

   sprintf(msg, "Can not find the '%s' %s '%s' command.",
         attr_name, "attribute while executing the", cmd_name);
   MsgBox(msg, TOOL_NAME, INFO_MB);
   return FALSE;
}

void ExecStartSimulator(obj_ptr, orig_cmd)
   char *orig_cmd;
   struct ObjRec *obj_ptr;
   /* start_simulator(); */
{
    char host[ 10 ];
    char *protocol = "udp";
    struct hostent     *phe;
    struct servent     *pse;
    struct protoent    *ppe;
    struct sockaddr_in  sin;
    int sin_size;
    int s, type, max_desc;
    char buffer;
    fd_set soc_pool;
    struct timeval timeout;
    int one = 1;
    char *service = "6743";

    strcpy( host, "localhost" );
    memset( (char *)&sin, 0, sizeof( sin ) );
    sin.sin_family = AF_INET;
    if( (pse = getservbyname( service, protocol )) != NULL )
        sin.sin_port = pse->s_port;
    else
        if( (sin.sin_port = htons( (u_short)atoi( service ) )) == 0 )
        {
            fprintf( stderr,"TGIF: Can't get \"%s\" service entry\n", service );
            return;
        }
    
    if( (phe = gethostbyname( host )) != NULL )
        memcpy( (char *)&sin.sin_addr, phe->h_addr, phe->h_length );
    else
    {
        sin.sin_addr.s_addr = inet_addr( host );
#ifdef linux
        if( sin.sin_addr.s_addr == INADDR_NONE )
        {
            fprintf( stderr, "TGIF: Can't get \"%s\" host entry\n", host );
            return;
        }
#endif /* linux */
    }
        
    if( (ppe = getprotobyname( protocol )) == NULL )
    {
        fprintf( stderr, "TGIF: Can't get \"%s\" protocol entry\n", protocol );
        return;
    }
    if( strcmp( protocol, "udp" ) == 0 )
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;
        
    if( (s = socket( PF_INET, type, ppe->p_proto )) < 0 )
    {
        fprintf( stderr, "TGIF: Can't create socket\n" );
        return;
    }
    
    if( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char *)&one,
        sizeof( one ) ) < 0 )
    {
        perror( "TGIF: REUSEADDR" );
        return;
    }
    
    if( bind( s, (struct sockaddr *)&sin, sizeof( sin ) ) < 0 )
    {
        perror( "TGIF: bin" );
        return;
    }
    
    if( type == SOCK_STREAM && listen( s, 0 ) < 0 )
    {
        fprintf( stderr, "TGIF: Can't listen on %s port\n", service );
        return;
    }
    fprintf(stdout,"TGIF: ready to receive data from the simulator.\n");
    fflush(stdout);
    
    FD_ZERO( &soc_pool );
    FD_SET( s, &soc_pool );
    max_desc = s + 1;
    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;
    if( select( max_desc, &soc_pool, NULL, NULL, &timeout ) < 0 )
    {
        perror( "TGIF: select" );
        return;
    }
    if( FD_ISSET( s, &soc_pool ) )
    {
        sin_size = sizeof( sin );
        if( recvfrom( s, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sin,
                      (void*)(&sin_size) ) < 0 )
        {
            perror( "TGIF: recv" );
            return;
        }
        
        if( connect( s, (struct sockaddr *)&sin, sin_size ) < 0 )
        {
            fprintf( stderr, "TGIF: Can't connect to %s.%s\n", host, service );
            return;
        }
    
        sleep( 1 );
        if( send( s, &buffer, sizeof(buffer), 0 ) < 0 )
        {
            perror( "TGIF: send" );
            return;
        }
    
    }
    else
    {
        fprintf( stderr, "TGIF: Timeout expired\n" );
        return;
    }

    simulator_socket = s;

    printf("TGIF: Client Connected.\n");
}

void ExecEndSimulator(obj_ptr, orig_cmd)
   char *orig_cmd;
   struct ObjRec *obj_ptr;
   /* end_simulator(); */
{
    T_ControlPacket pack_out;

    if (simulator_socket == INVALID) {
        perror("invalid simulator_socket");
        return;
    }
    /* monta o pacote */
    memset((char *)&pack_out, 0, sizeof(T_ControlPacket));
    pack_out.code = CT_CODE_END_OF_SIMULATION;

    /* envia o pacote ao simulador */
    sendControlPacket( simulator_socket, &pack_out );

    close(simulator_socket);
    printf("TGIF: Simulation finished.\n");
}

static
void AppendLinesToAttr(attr_ptr, ppsz_lines)
   struct AttrRec *attr_ptr;
   char **ppsz_lines;
{
   char **ppsz=NULL;
   struct TextRec *text_ptr=attr_ptr->obj->detail.t;

   for (ppsz=ppsz_lines; *ppsz != NULL; ppsz++) {
      MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;

      CreateMiniLineFromString(*ppsz, &pFirstMiniLine, &pLastMiniLine);
      text_ptr->minilines.last->next = pFirstMiniLine;
      pFirstMiniLine->prev = text_ptr->minilines.last;
      text_ptr->minilines.last = pLastMiniLine;
      text_ptr->lines++;
   }
   RecalcTextMetrics(text_ptr, attr_ptr->obj->x, text_ptr->baseline_y);
   UpdTextBBox(attr_ptr->obj);
}


static void UpdateStateVars( soc, obj_ptr, ct_pack_in )
int soc;
struct ObjRec *obj_ptr;
T_ControlPacket * ct_pack_in;
{
    int i;
    int begin, end;
    int alignment_size, name_length;
    char line[ MAXSYMBOLNAME+MAXFORMATSIZE+MAXVALUESIZE ];
    char line_format[ MAXFORMATSIZE ];
    char var_value[ MAXSTRING + 1 ];
    char last_object_name[ MAXOBJECTNAME ];
    char name_string[ MAXSYMBOLNAME+MAXFORMATSIZE ];
    char value_string[ MAXVALUESIZE ];
    char att_name[ 15 ];
    struct ObjRec *owner_obj=NULL, *named_obj;
    struct ObjRec *att_owner_obj=NULL;
    struct AttrRec *attr_ptr;
    T_SymbolPacket pack_in;

    begin = 0;

    last_object_name[0] = 0;
    CleanTmpStr();
    alignment_size = 12;
    sprintf( line_format, "%%-%ds%%s", alignment_size );

    for( i = 0; i < ct_pack_in->symcount; i++ )
    {
        recvSymbolPacket( soc, &pack_in );

        /* if we are changing between objects, must update 'Watches='
           in the last object */
        if( last_object_name[0] != 0 &&
            strcmp( last_object_name, pack_in.object_name ) != 0 )
        {
            named_obj = FindObjWithName( botObj, obj_ptr, last_object_name,
                                         FALSE, FALSE, &owner_obj, NULL );

            strcpy( att_name, "Watches=" );
            attr_ptr  = FindAttrWithName( named_obj, att_name, &att_owner_obj );

            if( attr_ptr != NULL )
            { 
                char *count_buf = (char*)malloc( (strlen(att_name)+40) *
                                                 sizeof(char) );

                if( count_buf == NULL )
                    FailAllocMessage();

                sprintf( count_buf, "%s", att_name );

                if( PrependToTmpStr( count_buf ) )
                    ReplaceAttrAllValues( named_obj, attr_ptr, &topTmpStr,
                                          &botTmpStr );

                free(count_buf);
            }

            CleanTmpStr();
            alignment_size = 12;
            sprintf( line_format, "%%-%ds%%s", alignment_size );
        }
        strcpy( last_object_name, pack_in.object_name );

        /* handle new packet */

        /* packets without values report invalid variables */
        if( !(pack_in.options & SP_OPT_HAS_VALUE) )
        {
            continue;
        }

        if( !(pack_in.options & SP_OPT_IN_ARRAY) )
        {
            if( pack_in.options & SP_OPT_HAS_FORMAT )
                sprintf( name_string, "%s<%s>", pack_in.symbol_name,
                                                pack_in.value_format );
            else
                sprintf( name_string, "%s", pack_in.symbol_name );

            sprintf( value_string, "%s", pack_in.value_string );

            name_length = strlen( name_string );
            while( name_length >= alignment_size )
                alignment_size += 4;
            sprintf( line_format, "%%-%ds%%s", alignment_size );

            sprintf( line, line_format, name_string, value_string );
            AppendToTmpStr( line );
        }
        else
        {
            if( pack_in.options & SP_OPT_FIRST_IN_ARRAY )
            {
                begin = pack_in.array_index;
                end   = -1;
                strcpy( var_value, pack_in.value_string );
            }
            else
            {
                strcat( var_value, ", " );
                strcat( var_value, pack_in.value_string );
            }

            if( pack_in.options & SP_OPT_LAST_IN_ARRAY )
            {
                end = pack_in.array_index;

                if( pack_in.options & SP_OPT_HAS_FORMAT )
                {
                    if( end == begin )
                        sprintf( name_string, "%s[%d]<%s>", pack_in.symbol_name,
                                 begin, pack_in.value_format );
                    else
                        sprintf( name_string, "%s[%d:%d]<%s>",
                                 pack_in.symbol_name,
                                 begin, end, pack_in.value_format );
                }
                else
                {
                    if( end == begin )
                        sprintf( name_string, "%s[%d]", pack_in.symbol_name,
                                 begin );
                    else
                        sprintf( name_string, "%s[%d:%d]", pack_in.symbol_name,
                                 begin, end );
                }

                if( end == begin )
                    sprintf( value_string, "%s", var_value );
                else
                    sprintf( value_string, "[%s]", var_value );


                name_length = strlen( name_string );
                while( name_length >= alignment_size )
                    alignment_size += 4;
                sprintf( line_format, "%%-%ds%%s", alignment_size );

                sprintf( line, line_format, name_string, value_string );
                AppendToTmpStr( line );
            }
        }
    }

    /* if there was an object in the last packet, must update 'Watches=' */
    if( last_object_name[0] != 0 )
    {
        named_obj = FindObjWithName( botObj, obj_ptr, last_object_name,
                                     FALSE, FALSE, &owner_obj, NULL );

        strcpy( att_name, "Watches=" );
        attr_ptr  = FindAttrWithName( named_obj, att_name, &att_owner_obj );

        if( attr_ptr != NULL )
        { 
            char *count_buf = (char*)malloc( (strlen(att_name)+40) *
                                             sizeof(char) );

            if( count_buf == NULL )
                FailAllocMessage();

            sprintf( count_buf, "%s", att_name );

            if( PrependToTmpStr( count_buf ) )
                ReplaceAttrAllValues( named_obj, attr_ptr, &topTmpStr,
                                      &botTmpStr );

            free(count_buf);
        }

        CleanTmpStr();
    }
}

int ParseFormat( format )
char * format;
{
    char * pt;
    int n_params, open;

    /* Syntax for formats: [.]*%[.]*[dioxX eEfFgGaA][.]* */
    n_params = 0;
    open     = 0;
    pt       = format;

    do
    {
        /* look for a parameter beginning with '%' */
        while( *pt != '%' && *pt != '\0' ) pt++;
        if( *pt == '\0' )
            break;

        pt++;
        if( *pt == '%' )
        {
            pt++;
            continue;
        }

        open = 1;

        /* look for any of the acceptable characters for int's and doubles */
        pt = strpbrk( pt, "dioxXeEfFgGaA" );
        if( !pt )
            break;

        open = 0;
        n_params++;
        pt++;
    } while( 1 );

    if( n_params != 1 || open )
        return -1;

    return 0;
}

int ParseStateVariable( buffer, symname, symvalue, symformat,
                        options, initial_index, final_index )
char * buffer; char * symname; char * symvalue; char * symformat;
int * options; int * initial_index; int * final_index;
{
    int i;
    char * pt;

    /* Syntax for requests: symname([i(:f)])(<symformat>)=(symvalue) */

    SKIP_WHITESPACE(buffer);
    if( !isalpha((int)(*buffer)) ) return -1;

    *options = 0;

    /* getting the variable name */
    for( i = 0; isalpha((int)(*buffer)) ||
                isdigit((int)(*buffer)) ||
                *buffer == '_';
         buffer++, i++ )
    {
        symname[i] = *buffer;
    }
    symname[i] = 0;

    if( strlen( symname ) <= 0 ||
        strncmp( symname, "Watches", 10 ) == 0 )
        return(-1);

    SKIP_WHITESPACE(buffer);

    /* OPTIONAL: getting array indices */
    if( *buffer == '[' )
    {
        buffer++;

        *initial_index = strtol( buffer, &pt, 0 );
        if( pt == buffer )
            return(-1);
        buffer = pt;
        if( *initial_index < 0 )
            return(-1);

        *final_index = *initial_index;

        SKIP_WHITESPACE(buffer);

        if( *buffer == ':' )
        {
            buffer++;
            *final_index = strtol( buffer, &pt, 0 );

            if( pt == buffer )
                return(-1);

            buffer = pt;
            if( *final_index < *initial_index )
                return(-1);
        }

        SKIP_WHITESPACE(buffer);

        if( *buffer != ']' )
            return(-1);

        buffer++;

        *options |= SP_OPT_IN_ARRAY;
    }

    SKIP_WHITESPACE(buffer);

    /* OPTIONAL: getting format string */
    if( *buffer == '<' )
    {
        buffer++;

        pt = buffer;

        while( *pt != '>' && *pt != '\0' && *pt != '\n' )
            pt++;

        if( pt != buffer )
        {
            strncpy( symformat, buffer, pt-buffer );
            symformat[ pt - buffer ] = 0;
            if( ParseFormat( symformat ) < 0 )
                return(-1);
        }

        buffer = pt;

        if( *buffer == '>' )
        {
            buffer++;
            *options |= SP_OPT_HAS_FORMAT;
        }
    }

    SKIP_WHITESPACE(buffer);

    /* OPTIONAL: getting the '=' sign */
    if( *buffer == '=' )
    {
        buffer++;

        SKIP_WHITESPACE(buffer);

        /* OPTIONAL: getting the value */
        if( *buffer == '[' )
        {
            buffer++;
            pt = buffer;

            while( *pt != ']' && *pt != '\0' && *pt != '\n' )
                pt++;

            if( pt == buffer )
                return(-1);

            strncpy( symvalue, buffer, pt-buffer );
            symvalue[ pt - buffer ] = 0;
            buffer = pt;
            *options |= SP_OPT_HAS_VALUE;

            if( *buffer != ']' )
                return(-1);
        }
        else
        {
            strtod( buffer, &pt );

            if( pt != buffer )
            {
                strncpy( symvalue, buffer, pt-buffer );
                symvalue[ pt - buffer ] = 0;
                buffer = pt;
                *options |= SP_OPT_HAS_VALUE;
            }
        }
    }

    return(0);
}

static
char * getNextValue( char * values, char ** endpt )
{
    static char value[MAXVALUESIZE];
    char * buffer;

    buffer = values;

    SKIP_WHITESPACE(buffer);
    
    strtod( buffer, endpt );
    if( *endpt == buffer )
        return(NULL);

    strncpy( value, buffer, *endpt-buffer );
    value[ *endpt - buffer ] = 0;

    SKIP_WHITESPACE(*endpt);

    if( **endpt == ',' )
        (*endpt)++;
    else
        *endpt = NULL;

    return(value);
}

void SendStepInfo( soc, ct_pack_out )
int soc;
T_ControlPacket * ct_pack_out;
{
    int j;
    int need_to_free_tmp_buf;
    int initial_index, final_index;
    int options;
    char symvalue  [ MAXVALUESIZE ];
    char symbol_name[ MAXSYMBOLNAME ];
    char symformat [ MAXFORMATSIZE ];
    char * value_string, * pt;
    char *tmp_buf;
    struct ObjRec *first_obj_ptr, *optr;
    struct AttrRec *name_attr;
    struct AttrRec *stat_attr;
    MiniLineInfo *pMiniLine;

    T_SymbolPacket   sympack;

    CleanTmpStr();

    first_obj_ptr = topObj;
    for( optr = first_obj_ptr; optr != NULL; optr = optr->next )
    {
        stat_attr = FindAttrWithName(optr,"Watches=",NULL);
        if( stat_attr != NULL )
        {
            /* Found an object that has the Watches attribute */

            /* Query the object's name */
            name_attr = FindAttrWithName(optr,"name=",NULL);

            /* For each line in the Watches attribute */
            for( pMiniLine=stat_attr->obj->detail.t->minilines.first;
                 pMiniLine != NULL;
                 pMiniLine=pMiniLine->next) 
            {
                /* Convert attribute line text to a string */
                tmp_buf = ConvertMiniLineToString( pMiniLine,
                                                   &need_to_free_tmp_buf );

                /* if we can parse a valid state variable request */
                if( ParseStateVariable( tmp_buf, symbol_name, symvalue,
                                        symformat, &options,
                                        &initial_index, &final_index ) == 0 )
                {
                    /* initialize packet's fields */
                    sympack.object_name[0]  = 0;
                    sympack.symbol_name[0]  = 0;
                    sympack.options         = 0;
                    sympack.array_index     = -1;
                    sympack.value_string[0] = 0;
                    sympack.value_format[0] = 0;

                    /* state variable name */
                    strcpy( sympack.object_name, name_attr->attr_value.s );
                    strcpy( sympack.symbol_name, symbol_name );

                    if( options & SP_OPT_HAS_FORMAT )
                        strcpy( sympack.value_format, symformat );

                    /* if state variable is part of an array */
                    if( options & SP_OPT_IN_ARRAY )
                    {
                        pt = symvalue;

                        for( j = initial_index; j <= final_index; j++ )
                        {
                            /* initialize options to those parsed */
                            /* SP_OPT_IN_ARRAY   is always present
                               SP_OPT_HAS_FORMAT may or may not appear
                               SP_OPT_HAS_VALUE  may appear but is ignored */
                            sympack.options = options;

                            /* add SP_OPT_FIRST_IN_ARRAY */
                            if( j == initial_index )
                                sympack.options |= SP_OPT_FIRST_IN_ARRAY;

                            /* add SP_OPT_LAST_IN_ARRAY */
                            if( j == final_index )
                                sympack.options |= SP_OPT_LAST_IN_ARRAY;

                            /* add current index */
                            sympack.array_index = j;

                            /* check for value */
                            value_string = getNextValue( pt, &pt );
                            if( value_string )
                            {
                                sympack.options |= SP_OPT_HAS_VALUE;
                                strcpy( sympack.value_string, value_string );
                            }
                            else
                                sympack.options &= ~SP_OPT_HAS_VALUE;

                            /* add variable to packet queue */
                            addSymToBuffer( &sympack );

                            /* if next call to getNextValue will fail break */
                            if( !pt )
                            {
                                j++;
                                break;
                            }
                        }

                        /* add rest of variables without value */
                        for( ; j <= final_index; j++ )
                        {
                            /* initialize options to those parsed */
                            /* SP_OPT_IN_ARRAY   is always present
                               SP_OPT_HAS_FORMAT may or may not appear
                               SP_OPT_HAS_VALUE  is ignored */
                            sympack.options = options & ~SP_OPT_HAS_VALUE;

                            /* add SP_OPT_FIRST_IN_ARRAY */
                            if( j == initial_index )
                                sympack.options |= SP_OPT_FIRST_IN_ARRAY;

                            /* add SP_OPT_LAST_IN_ARRAY */
                            if( j == final_index )
                                sympack.options |= SP_OPT_LAST_IN_ARRAY;

                            /* add current index */
                            sympack.array_index = j;

                            /* add variable to packet queue */
                            addSymToBuffer( &sympack );
                        }
                    }
                    else /* !( options & SP_OPT_IN_ARRAY ) */
                    {
                        sympack.options = options;

                        /* check for value */
                        if( sympack.options & SP_OPT_HAS_VALUE )
                        {
                            pt = symvalue;
                            value_string = getNextValue( pt, &pt );
                            if( value_string )
                                strcpy( sympack.value_string, value_string );
                        }

                        /* add variable to packet queue */
                        addSymToBuffer( &sympack );
                    }
                }

                if( need_to_free_tmp_buf )
                    UtilFree(tmp_buf);
            }

            /* Clear the Watches= attribute after sending all symbols */
            tmp_buf = (char*)malloc( 20 * sizeof(char) );
            if( tmp_buf == NULL )
                FailAllocMessage();

            strcpy( tmp_buf, "Watches=" );

            AppendToTmpStr( tmp_buf );

            ReplaceAttrAllValues( optr, stat_attr, &topTmpStr, &botTmpStr );
            CleanTmpStr();

            free(tmp_buf);
        }
    }

    ct_pack_out->symcount = packet_queue_size;
    sendControlPacket( soc, ct_pack_out );
    sendAllSymPackets( soc );
}

void ExecSimulateStep(obj_ptr, orig_cmd)
   char *orig_cmd;
   struct ObjRec *obj_ptr;
{
   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   static char execDummyStr[MAXSTRING+1];

   struct ObjRec *owner_obj=NULL, *named_obj;
   static char obj_name[MAXSTRING+1];

   T_ControlPacket pack_out, pack_in;

   static int transitions = 0;
   static double tempo = 0;
   int step = 1;

   if (simulator_socket == INVALID) {
      perror("invalid simulator_socket");
      return;
   }

   /* pega o valor do atributo step */
   strcpy(execDummyStr, "Step=");
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { BadAttr_Simulator(execDummyStr, orig_cmd); return; }
   step = atoi(attr_ptr->attr_value.s);

   /*
    * step nao pode ser menor do que zero (se for zero entao esta com play_mode)
    */
   if (step <= 0) {
      step = 1;
   }

   /* monta o pacote */
   memset((char *)&pack_out, 0, sizeof(T_ControlPacket));
   pack_out.code = CT_CODE_STEP_SIMULATION;
   pack_out.step = step;

   /* send information to mark_find */
   SendStepInfo( simulator_socket, &pack_out );

   /* receive control information */
   if( recvControlPacket( simulator_socket, &pack_in ) < 0 )
      return;

   /* desmonta o pacote */
   tempo = pack_in.elapsed_time;
   transitions = step;

   /* set current text justification to center justified */
   DO_CMD("set_selected_text_just(\"left\")");
   /* set current text font to Helvetica Bold */
   DO_CMD("set_selected_text_font(\"Courier\")");
   /* set current text size to 14 */
   DO_CMD("set_selected_text_size(14)");
   /* set current text color to 'black' */
   DO_CMD("set_selected_obj_color(\"black\")");

   /* set o valor do step_time (tempo de duracao deste step) */
   strcpy(execDummyStr, "step_time=");
   attr_ptr = FindAttrWithName(obj_ptr, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { BadAttr_Simulator(execDummyStr, orig_cmd); return; }
   sprintf(execDummyStr, "%.3f", tempo);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* atualiza o valor do atributo Time do objeto "__START_SIMULATOR__" */
   strcpy(obj_name, "__START_SIMULATOR__");
   named_obj = FindObjWithName(botObj, obj_ptr, obj_name, FALSE, FALSE,
         &owner_obj, NULL);

   strcpy(execDummyStr, "Time=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { BadAttr_Simulator(execDummyStr, orig_cmd); return; }
   tempo += atof(attr_ptr->attr_value.s);
   sprintf(execDummyStr, "%.3f", tempo);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* atualiza o valor do atributo Transitions do mesmo objeto */
   strcpy(execDummyStr, "Transitions=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { BadAttr_Simulator(execDummyStr, orig_cmd); return; }
   transitions += atof(attr_ptr->attr_value.s);
   sprintf(execDummyStr, "%d", transitions);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* atualiza o valor do atributo Last_Event do mesmo objeto */
   strcpy(execDummyStr, "Last_Event=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { BadAttr_Simulator(execDummyStr, orig_cmd); return; }
   strcpy(execDummyStr, pack_in.last_event);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* Update state vars shown in screen */
   UpdateStateVars( simulator_socket, obj_ptr, &pack_in );
}

/* ======================= Tangram2 Menu ======================= */

int RefreshTangram2Menu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   return ok;
}

TgMenu *CreateTangram2Menu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshTangram2Menu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
   }
   return menu;
}

int Tangram2Menu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(tangram2MenuInfo.create_proc)(NULL, X, Y, &tangram2MenuInfo,
         INVALID);

   activeMenu = MENU_TANGRAM2;
   if (menu != NULL) {
      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

/* ========================= Tangram2 Menu Data ========================= */


static char *GenerateChainInit[] = {
  "get_current_file(\"str\");",
  "read_file_into_attr(\"| basename $(str) .obj\", \"basename\");",
  "strcpy(file_name, \"$(basename).parser\");",
  "strcpy(str, \" \");",
  "write_attr_into_file(str, $(file_name));",
  NULL
};

static char *GenerateChainExec[] = {
  "disable_undo();",
  "exec(init);",
  "exec(write_model);",
  "exec(write_global_rewards);",
  "exec(write_indep_chains);",
  "enable_undo();",
   NULL
};

static char *GenerateChainWriteModel[] = {
  "find_obj_names_on_all_pages(obj_list,\"Declaration=*\");",
  "get_line_in_attr(total_objs,obj_list,0);",
  "for_i(i,1,$(total_objs),1,write_obj);",
  NULL
};

static char *GenerateChainErrorParts[] = {
  "message_box(NULL,\"Objects must have these four parts: Variables_and_parameters, Events, Messages and Rewards. $(str)\",\"Mode Error\", \"stop\");",
  "exec(stop);",
  NULL
};


static char *GenerateChainStop[] = {
  "enable_undo();",
  "stop();",
  NULL
};


static char *GenerateChainAppendGlobalRewards[] = {
  "append_attr_into_file(__GLOBAL_REWARDS__.global_rewards, $(file_name));",
 NULL
};


static char *GenerateChainWriteIndependentChains[] = {
  "strcpy(str, \"Independent_Chains {\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result, __INDEP_CHAINS__.independent_chains);",
  "if($(result) != 0, append_indep_chains, NULL);",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  NULL
};

static char *GenerateChainAppendIndepChains[] = {
  "append_attr_into_file(__INDEP_CHAINS__.independent_chains, $(file_name));",
  NULL
};


static char *GenerateChainWriteGlobalRewards[] = {
  "strcpy(str, \"Global_Rewards {\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result, __GLOBAL_REWARDS__.global_rewards);",
  "if($(result) != 0, append_global_rewards, NULL);",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  NULL
};

static char *GenerateChainWriteMaxValues[] = {
  "strcpy(max_file_name, \"$(basename).maxvalues\");",
  "strcpy(str, \"$(__SETUP__.max_values)\");",
  "write_attr_into_file(__SETUP__.max_values, $(max_file_name));",
 NULL
};


static char *GenerateChainWriteObj[] = {
  "get_line_in_attr(obj_name,obj_list,$(i));",
  "strcpy(str, \"Object_Desc $(obj_name) (\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result,$(obj_name).Declaration);",
  "strcpy(str,\"Missing Declaration part in object: $(obj_name).\");",
  "if($(result) == 0,error_parts,NULL);",
  "strcpy(str, \"Declaration {\");",
  "append_attr_into_file(str,$(file_name));",
  "append_attr_into_file($(obj_name).Declaration,$(file_name));",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result,$(obj_name).Initialization);",
  "strcpy(str,\"Missing Initialization part in object: $(obj_name).\");",
  "if($(result) == 0,error_parts,NULL);",
  "strcpy(str, \"Initialization {\");",
  "append_attr_into_file(str,$(file_name));",
  "append_attr_into_file($(obj_name).Initialization,$(file_name));",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result,$(obj_name).Events);",
  "strcpy(str,\"Missing Events part in object: $(obj_name).\");",
  "if($(result) == 0,error_parts,NULL);",
  "strcpy(str, \"Events {\");",
  "append_attr_into_file(str,$(file_name));",
  "append_attr_into_file($(obj_name).Events,$(file_name));",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result,$(obj_name).Messages);",
  "strcpy(str,\"Missing Messages part in object: $(obj_name).\");",
  "if($(result) == 0,error_parts,NULL);",
  "strcpy(str, \"Messages {\");",
  "append_attr_into_file(str,$(file_name));",
  "append_attr_into_file($(obj_name).Messages,$(file_name));",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  "is_attr(result,$(obj_name).Rewards);",
  "strcpy(str,\"Missing Rewards part in object: $(obj_name).\");",
  "if($(result) == 0,error_parts,NULL);",
  "strcpy(str, \"Rewards {\");",
  "append_attr_into_file(str,$(file_name));",
  "append_attr_into_file($(obj_name).Rewards,$(file_name));",
  "strcpy(str, \"}\");",
  "append_attr_into_file(str,$(file_name));",
  "strcpy(str, \")\");",
  "append_attr_into_file(str,$(file_name));",
  NULL
};
/*-----------------------------------------------------------------------*/
static char *StartSimulatorSetModeB[] = {
  NULL
};

static char *StartSimulatorSetModeI[] = {
 "strcpy(Transitions, \"0\");",
 "strcpy(Time, \"0.0\");",
 "start_simulator();",
  NULL
};

static char *StartSimulatorErrorMode[] = {
 "message_box(NULL,\"Please, Mode has to be I(Interactive) or B(Batch)!\",\"Mode Error\", \"stop\");",
  NULL
};

static char *StartSimulatorExec[] = {
 "disable_undo();",
 "show_attr(Transitions);",
 "show_attr(Time);",
 "exec(__GENERATE_CHAIN__.init);",
 "if(\"$(Mode)\"!=\"I\"&&\"$(Mode)\"!=\"B\",error_mode,NULL);"
 "strcpy(file_name, \"$(__GENERATE_CHAIN__.file_name)\");",
 "strcpy(str, \" \");",
 "write_attr_into_file(str, $(file_name));",
 "exec(__GENERATE_CHAIN__.write_model);"
 "exec(__GENERATE_CHAIN__.write_global_rewards);"
 "exec(__GENERATE_CHAIN__.write_indep_chains);",
 "if(\"$(Mode)\"==\"I\",set_mode_I,NULL);",
 "if(\"$(Mode)\"==\"B\",set_mode_B,NULL);",
 "enable_undo();",
 NULL
};
/*-----------------------------------------------------------------------*/
static char *SimulationStepLoopAnimation[] = {
 "for_i(Ani_C_Step,1,$(Ani_T_Steps),1,Do_Animation);",
 "round(sleep_time,$(Time_Scale)*$(step_time)*1000);",
 "sleep(NULL,$(sleep_time));",
  NULL
};

static char *SimulationStepDoAnimation[] = {
 "find_obj_names(obj_list,\"\",\"Animation=*\");",
 "get_line_in_attr(no_objs,obj_list,0);",
 "for_i(obj_index,1,$(no_objs),1,Animate_Obj);",
  NULL
};

static char *SimulationStepAnimateObj[] = {
 "get_line_in_attr(cur_obj,obj_list,$(obj_index));",
 "exec(\"$(cur_obj).Animation\");",
  NULL
};


static char *SimulationStepCallSimulator[] = {
  "simulate_step();",
  NULL
};

static char *SimulationStepPlayOneStep[] = {
 "set_allow_interrupt(FALSE);",
 "exec(step_simulation);",
 "set_allow_interrupt(TRUE);",
 "sleep(NULL,$(Delay));",
  NULL
};

static char *SimulationStepStop[] = {
  "stop();",
  NULL
};

static char *SimulationStepExec[] = {
 "disable_undo();",
 "if(\"$(Step)\"==\"0\",play_simulation,step_simulation);",
 "enable_undo();",
 "redraw_drawing_area();",
  NULL
};

static char *SimulationStepPlaySimulation[] = {
  "while(1,play_one_step);",
  NULL
};

static char *SimulationStepStepSimulation[] = {
 "exec(call_simulator);",
 "if(\"$(Animation)\"==\"ON\",Loop_Animation,NULL);",
  NULL
};

/**
static char *SimulationStepParseObj[] = {
  NULL
};

static char *SimulationStepWriteVars[] = {
  NULL
};

static char *SimulationStepWriteVarLine[] = {
  NULL
};

static char *SimulationStepUpdateObjects[] = {
  NULL
};

static char *SimulationStepUpdateVars[] = {
  NULL
};
**/
/* ====================== Tangram2 Auxiliar Functions ======================= */

struct ObjRec *createGenerateChainButton( dbglevel, maxstates, output )
char *dbglevel;
char *maxstates;
char *output;
{
   struct ObjRec *tmp_box_obj=NULL;

   CreateBoxObj(0,0,10,10,TRUE);
   tmp_box_obj = topObj;

   /* Note: AddAttrByNameAndValue() adds the new object at tmp_box_obj->fattr */
   AddAttrByNameAndValue(tmp_box_obj, "i=", "");
   AddAttrByNameAndValue(tmp_box_obj, "dbglevel=", dbglevel);
   AddAttrByNameAndValue(tmp_box_obj, "maxstates=", maxstates);
   AddAttrByNameAndValue(tmp_box_obj, "output=", output);
   AddAttrByNameAndValue(tmp_box_obj, "processID=", "");
   AddAttrByNameAndValue(tmp_box_obj, "name=", "__GENERATE_CHAIN__");
   AddAttrByNameAndValue(tmp_box_obj, "basename=", "");
   AddAttrByNameAndValue(tmp_box_obj, "total_objs=", "");
   AddAttrByNameAndValue(tmp_box_obj, "file_name=", "");
   AddAttrByNameAndValue(tmp_box_obj, "command=", "");
   AddAttrByNameAndValue(tmp_box_obj, "str=", "");
   AddAttrByNameAndValue(tmp_box_obj, "result=", "");
   AddAttrByNameAndValue(tmp_box_obj, "obj_name=", "");
   AddAttrByNameAndValue(tmp_box_obj, "max_file_name=", "");
   AddAttrByNameAndValue(tmp_box_obj, "obj_list=", "");

   AddAttrByNameAndValue(tmp_box_obj, "init=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainInit);

   AddAttrByNameAndValue(tmp_box_obj, "exec=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainExec);

   AddAttrByNameAndValue(tmp_box_obj, "write_model=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainWriteModel);

   AddAttrByNameAndValue(tmp_box_obj, "write_obj=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainWriteObj);

   AddAttrByNameAndValue(tmp_box_obj, "stop=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainStop);

   AddAttrByNameAndValue(tmp_box_obj, "error_parts=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainErrorParts);

   AddAttrByNameAndValue(tmp_box_obj, "write_max_values=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainWriteMaxValues);

   AddAttrByNameAndValue(tmp_box_obj, "write_global_rewards=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainWriteGlobalRewards);

   AddAttrByNameAndValue(tmp_box_obj, "append_global_rewards=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainAppendGlobalRewards);

   AddAttrByNameAndValue(tmp_box_obj, "write_indep_chains=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainWriteIndependentChains);

   AddAttrByNameAndValue(tmp_box_obj, "append_indep_chains=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, GenerateChainAppendIndepChains);

   AddNewSelObj(tmp_box_obj);

   return( tmp_box_obj );
}


struct ObjRec *createStartSimulatorButton( runs, event, trans, time, mode )
char *runs;
char *event;
char *trans;
char *time;
char *mode;
{
   struct ObjRec *tmp_box_obj = NULL;
   int y = 5;

   CreateRCBoxObj(4,y,500,y+55,TRUE);
   tmp_box_obj = topObj;

   tmp_box_obj->color = 9;
   if (mainDisplay != NULL) {
      UtilStrCpyN(tmp_box_obj->color_str, sizeof(tmp_box_obj->color_str),
            colorMenuItems[tmp_box_obj->color]);
   }
   tmp_box_obj->detail.rcb->width = 2;
   y += 5;

   AddAttrByNameAndValue(tmp_box_obj, "Runs=", runs);

   AddAttrByNameAndValue(tmp_box_obj, "Time=", time);
   tmp_box_obj->fattr->shown = TRUE;
   MoveObj(tmp_box_obj->fattr->obj, 20, y - tmp_box_obj->fattr->obj->obbox.lty);
   y += 14;

   AddAttrByNameAndValue(tmp_box_obj, "Transitions=", trans);
   tmp_box_obj->fattr->shown = TRUE;
   MoveObj(tmp_box_obj->fattr->obj, 20, y - tmp_box_obj->fattr->obj->obbox.lty);
   y += 14;

   AddAttrByNameAndValue(tmp_box_obj, "Last_Event=", event);
   tmp_box_obj->fattr->shown = TRUE;
   MoveObj(tmp_box_obj->fattr->obj, 20, y - tmp_box_obj->fattr->obj->obbox.lty);
   y += 14;

   AddAttrByNameAndValue(tmp_box_obj, "Mode=", mode);
   AddAttrByNameAndValue(tmp_box_obj, "file_name=", "");
   AddAttrByNameAndValue(tmp_box_obj, "command=", "");
   AddAttrByNameAndValue(tmp_box_obj, "simulating=", "");
   AddAttrByNameAndValue(tmp_box_obj, "str=", "");
   AddAttrByNameAndValue(tmp_box_obj, "name=", "__START_SIMULATOR__");

   AddAttrByNameAndValue(tmp_box_obj, "set_mode_B=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, StartSimulatorSetModeB);

   AddAttrByNameAndValue(tmp_box_obj, "set_mode_I=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, StartSimulatorSetModeI);

   AddAttrByNameAndValue(tmp_box_obj, "error_mode=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, StartSimulatorErrorMode);

   AddAttrByNameAndValue(tmp_box_obj, "exec=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, StartSimulatorExec);

   AdjObjBBox(tmp_box_obj);

   AddNewSelObj(tmp_box_obj);
   RedrawDrawWindow(botObj);

   return( tmp_box_obj );
}

struct ObjRec *createSimulationStepButton( time, anim, delay, step )
char *time;
char *anim;
char *delay;
char *step;
{
   struct ObjRec *tmp_box_obj = NULL;

   CreateBoxObj(0,0,10,10,TRUE);
   tmp_box_obj = topObj;

   /* Note: AddAttrByNameAndValue() adds the new object at tmp_box_obj->fattr */
   AddAttrByNameAndValue(tmp_box_obj, "Time_Scale=", time);
   AddAttrByNameAndValue(tmp_box_obj, "Animation=", anim);
   AddAttrByNameAndValue(tmp_box_obj, "Delay=", delay);
   AddAttrByNameAndValue(tmp_box_obj, "Step=", step);
   AddAttrByNameAndValue(tmp_box_obj, "state_var_str=", "1");
   AddAttrByNameAndValue(tmp_box_obj, "line_size=", "7");
   AddAttrByNameAndValue(tmp_box_obj, "obj_list=", "");
   AddAttrByNameAndValue(tmp_box_obj, "sleep_time=", "2049");
   AddAttrByNameAndValue(tmp_box_obj, "step_time=", "2049");
   AddAttrByNameAndValue(tmp_box_obj, "name=", "__SIMULATION_STEP__");
   AddAttrByNameAndValue(tmp_box_obj, "cur_obj=", "");
   AddAttrByNameAndValue(tmp_box_obj, "obj_index=", "");
   AddAttrByNameAndValue(tmp_box_obj, "no_objs=", "");
   AddAttrByNameAndValue(tmp_box_obj, "Ani_T_Steps=", "10");
   AddAttrByNameAndValue(tmp_box_obj, "Ani_C_Step=", "11");
   AddAttrByNameAndValue(tmp_box_obj, "step_index=", "");
   AddAttrByNameAndValue(tmp_box_obj, "no_state_vars=", "3");
   AddAttrByNameAndValue(tmp_box_obj, "state_var_value=", "8");
   AddAttrByNameAndValue(tmp_box_obj, "state_var_name=", "8");
   AddAttrByNameAndValue(tmp_box_obj, "obj_name=", "");
   AddAttrByNameAndValue(tmp_box_obj, "new_line=", "");
   AddAttrByNameAndValue(tmp_box_obj, "buffer=", "");
   AddAttrByNameAndValue(tmp_box_obj, "state_var_line=", "");
   AddAttrByNameAndValue(tmp_box_obj, "state_var_list=", "");
   AddAttrByNameAndValue(tmp_box_obj, "state_var_tok=", "");
   AddAttrByNameAndValue(tmp_box_obj, "var_index=", "");

   AddAttrByNameAndValue(tmp_box_obj, "Loop_Animation=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepLoopAnimation);

   AddAttrByNameAndValue(tmp_box_obj, "Animate_Obj=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepAnimateObj);

   AddAttrByNameAndValue(tmp_box_obj, "Do_Animation=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepDoAnimation);

   AddAttrByNameAndValue(tmp_box_obj, "call_simulator=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepCallSimulator);

   AddAttrByNameAndValue(tmp_box_obj, "play_one_step=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepPlayOneStep);

   AddAttrByNameAndValue(tmp_box_obj, "stop=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepStop);

   AddAttrByNameAndValue(tmp_box_obj, "step_simulation=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepStepSimulation);

   AddAttrByNameAndValue(tmp_box_obj, "play_simulation=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepPlaySimulation);
/**
   AddAttrByNameAndValue(tmp_box_obj, "update_objects=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepUpdateObjects);

   AddAttrByNameAndValue(tmp_box_obj, "update_vars=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepUpdateVars);

   AddAttrByNameAndValue(tmp_box_obj, "parse_obj=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepParseObj);

   AddAttrByNameAndValue(tmp_box_obj, "write_vars=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepWriteVars);

   AddAttrByNameAndValue(tmp_box_obj, "write_var_line=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepWriteVarLine);
**/
   AddAttrByNameAndValue(tmp_box_obj, "exec=", "");
   AppendLinesToAttr(tmp_box_obj->fattr, SimulationStepExec);

   AddNewSelObj(tmp_box_obj);

   return( tmp_box_obj );
}

/******************************************************************************/
/* This function extracts the args that are comma separated in 'args' and     */
/* stores them in 'dst'.                                                      */
/* return 0 if failed, no_args if success.                                    */
/* arguments:                                                                 */
/*    args: comma separated list of tokens                                    */
/*    dst : char vector pointer where the tokens will be stored               */
/*    no_args: number of tokens in 'args'                                     */
/*    max_arg_length: max token's size                                        */
/******************************************************************************/
int parseArgs( args, dst, no_args, max_arg_length )
char *args;
char *dst;
int no_args;
int max_arg_length;
{
    int i, j;
    char *ptr;

    memset( dst, 0, no_args * max_arg_length );    
    ptr = args;
    /* Remove leading " and spaces */
    while( *ptr == '\"' || *ptr == ' ' )
        ptr++;
        
    for( i = 0; i < no_args; i++ )
    {
        j = 0;
        while( *ptr != '\0' && *ptr != ';' && *ptr != '\"' &&
               j < max_arg_length )
            *(dst + max_arg_length * i + j++) = *ptr++;
            
        if( *ptr == ';' || *ptr == '\"' )
            ptr++;
        else
            if( *ptr != '\0' )
                break;
    }
    
    return( i == no_args );
}
/* ======================= Tangram2 Menu Functions ======================= */

void AboutTangram2()
{
   sprintf(gszMsgBox, "%s\n\n%s %s.",
         "Welcome to Tangram-II!",
         "Brought to you by the Tangram-II team at the",
         "Universidade Federal do Rio de Janeiro");
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
}

/******************************************************************************/
/* This function creates a invisible button, executes this button and removes */
/* it. This function also needs to extract three arguments that are colapsed  */
/* in arg. This is done because tgif supports only one argument functions.    */
/******************************************************************************/
void Tangram2GenerateChain( args )
char *args; /* semi-colon separated list (arg1;arg2;arg3) */
{
   int saved_fill=objFill, saved_pen=penPat;
   struct ObjRec *tmp_box_obj=NULL;
   struct AttrRec *exec_attr=NULL;
   char arg[3][1024];

   /* Extracts parameters from args */
   if( !parseArgs( args, arg, 3, 1024 ) )
   {
        fprintf( stderr, "TGIF: Usage Tangram2GenerateChain( \"arg1;arg2;arg3\" )\n" );
        return;
   }

   MakeQuiescent(); /* select nothing, edit nothing */

   objFill = penPat = NONEPAT;

   tmp_box_obj = createGenerateChainButton( arg[0], arg[1], arg[2] );

   objFill = saved_fill;
   penPat = saved_pen;

   exec_attr = FindAttrWithName(topSel->obj, "exec=", NULL);
   if (exec_attr != NULL) {
      DoExecLoop(topSel->obj, exec_attr);
   }
   MakeQuiescent();
   AddNewSelObj(tmp_box_obj);
   PrepareToRecord(CMD_DELETE, topSel, botSel, numObjSelected);
   UnlinkObj(topSel->obj);
   FreeObj(topSel->obj);
   RemoveAllSel();
   RecordCmd(CMD_DELETE, NULL, NULL, NULL, 0);

}

void Tangram2StartSimulator( args )
char *args;
{
   int saved_fill=objFill, saved_pen=penPat;
   struct AttrRec *exec_attr=NULL;
   char arg[12][1024]; /* first 3 for generate_chain */

   struct AttrRec *attr_ptr;
   struct ObjRec *attr_owner_obj=NULL;
   static char execDummyStr[MAXSTRING+1];

   struct ObjRec *owner_obj=NULL, *named_obj;
   static char obj_name[MAXSTRING+1];

   T_ControlPacket pack_out, pack_in;



   /* Extracts parameters from args */
   if( !parseArgs( args, arg, 12, 1024 ) )
   {
        return;
   }

   MakeQuiescent(); /* select nothing, edit nothing */
   objFill = penPat = NONEPAT;

   /* set current text justification to center justified */
   DO_CMD("set_selected_text_just(\"left\")");
   /* set current text font to Helvetica Bold */
   DO_CMD("set_selected_text_font(\"Courier\")");
   /* set current text size to 14 */
   DO_CMD("set_selected_text_size(14)");
   /* set current text color to 'black' */
   DO_CMD("set_selected_obj_color(\"black\")");

   objGC = createGenerateChainButton( arg[0], arg[1], arg[2] );
   objSp = createSimulationStepButton( arg[8], arg[9], arg[10], arg[11] );

   objFill = saved_fill;
   penPat = saved_pen;
   objSS = createStartSimulatorButton( arg[3], arg[4], arg[5], arg[6], arg[7] );

   exec_attr = FindAttrWithName(objSS, "exec=", NULL);
   if (exec_attr != NULL) {
      DoExecLoop(objSS, exec_attr);
   }

   /* send initial simulation request packet */
   memset((char *)&pack_out, 0, sizeof(T_ControlPacket));
   pack_out.code = CT_CODE_INIT_SIMULATION;

   /* send information to mark_find */
   SendStepInfo( simulator_socket, &pack_out );

   /* receive control information */
   if( recvControlPacket( simulator_socket, &pack_in ) < 0 )
      return;

   /* atualiza o valor do atributo Time do objeto "__START_SIMULATOR__" */
   strcpy(obj_name, "__START_SIMULATOR__");
   named_obj = FindObjWithName(botObj, topObj, obj_name, FALSE, FALSE,
         &owner_obj, NULL);

   strcpy(execDummyStr, "Time=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { return; }
   strcpy( execDummyStr, "0" );
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* atualiza o valor do atributo Transitions do mesmo objeto */
   strcpy(execDummyStr, "Transitions=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { return; }
   strcpy( execDummyStr, "0" );
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* atualiza o valor do atributo Last_Event do mesmo objeto */
   strcpy(execDummyStr, "Last_Event=");
   attr_ptr = FindAttrWithName(named_obj, execDummyStr, &attr_owner_obj);
   if (attr_ptr == NULL) { return; }
   strcpy(execDummyStr, pack_in.last_event);
   ReplaceAttrFirstValue(attr_owner_obj, attr_ptr, execDummyStr);

   /* Update state vars shown in screen */
   UpdateStateVars( simulator_socket, topObj, &pack_in );
}

void Tangram2SimulationStep()
{
   struct AttrRec *exec_attr=NULL;
   
   exec_attr = FindAttrWithName(objSp, "exec=", NULL);
   if (exec_attr != NULL) {
      DoExecLoop(objSp, exec_attr);
   }
}

void Tangram2EndSimulator()
{
   ExecEndSimulator( NULL, NULL );

   /* Delete all created buttons */
   MakeQuiescent();
   AddNewSelObj(objGC);
   AddNewSelObj(objSS);
   AddNewSelObj(objSp);
   DelAllSelObj();

   RedrawDrawWindow(botObj);
}

/* ======================= Tangram2 Misc Functions ======================= */

void Tangram2UpdateInitAttr(port_obj, signal_name)
   struct ObjRec *port_obj;
   char *signal_name;
{
   struct AttrRec *attr_ptr=FindAttrWithName(port_obj, "name=", NULL);
   MiniLinesInfo *minilines=NULL;
   MiniLineInfo *pMiniLine=NULL;
   struct TextRec *text_ptr=NULL;
   char *port_name=NULL, *new_line=NULL, *parent_obj_name=NULL;
   int port_name_len=0;

   if (attr_ptr == NULL) return;
   port_name = attr_ptr->attr_value.s;
   port_name_len = strlen(port_name);
   if (port_name_len == 0) {
      MsgBox(TgLoadString(STID_PORT_OBJ_HAS_EMPTY_NAME), TOOL_NAME, INFO_MB);
      return;
   }
   attr_ptr = FindAttrWithName(port_obj->tmp_parent, "type=", NULL);
   if (attr_ptr != NULL &&
         strcmp(attr_ptr->attr_value.s, "tgBroadcastWire") == 0) {
      /* a tgBroadcastWire object is really not a Tangram-II object */
      return;
   }
   attr_ptr = FindAttrWithName(port_obj->tmp_parent, "name=", NULL);
   if (attr_ptr != NULL) {
      parent_obj_name = attr_ptr->attr_value.s;
   }
   attr_ptr = FindAttrWithName(port_obj->tmp_parent, "Initialization=", NULL);
   if (attr_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_INIT_ATTR_PORT_PA),
            (parent_obj_name==NULL ? TgLoadCachedString(CSTID_PARANED_UNKNOWN) :
            parent_obj_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   text_ptr = attr_ptr->obj->detail.t;
   minilines = (&text_ptr->minilines);
   for (pMiniLine=minilines->first->next; pMiniLine != NULL;
         pMiniLine=pMiniLine->next) {
      int need_to_free_tmp_buf=FALSE, blank_count=0;
      char *tmp_buf=ConvertMiniLineToString(pMiniLine, &need_to_free_tmp_buf);
      char *attr_value=UtilStrDup(tmp_buf), *psz=NULL, *start=NULL;

      if (attr_value == NULL) FailAllocMessage();
      for (start=attr_value; *start == ' ' || *start == '\t'; start++) {
         blank_count++;
      }
      if (*start != '\0') {
         char *prefix=NULL;

         if (blank_count > 0) {
            char saved_ch=(*start);

            *start = '\0';
            prefix = UtilStrDup(attr_value);
            if (prefix == NULL) FailAllocMessage();
            *start = saved_ch;
         }
         psz = strchr(start, '=');
         if (psz != NULL) {
            *psz = '\0';
            if (strncmp(port_name, start, port_name_len) == 0 &&
                  (start[port_name_len] == ' ' ||
                  start[port_name_len] == '\t' ||
                  start[port_name_len] == '\0')) {
               /* found it */
               int misc_len=strlen(&start[port_name_len]);
               int len=blank_count+port_name_len+(misc_len<<1)+1+
                     strlen(signal_name);

               new_line = (char*)malloc(len);
               if (new_line == NULL) FailAllocMessage();
               sprintf(new_line, "%s%s%s=%s%s",
                     (prefix==NULL ? "" : prefix), port_name,
                     &start[port_name_len], &start[port_name_len],
                     signal_name);
               break;
            }
            *psz = '=';
         }
         UtilFree(prefix);
      }
      UtilFree(attr_value);
      if (need_to_free_tmp_buf) UtilFree(tmp_buf);
   }
   if (pMiniLine == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_VAR_IN_INIT_ATTR),
            port_name, (parent_obj_name==NULL ?
            TgLoadCachedString(CSTID_PARANED_UNKNOWN) : parent_obj_name));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   FreeStrBlockList(pMiniLine->first_block->next);
   pMiniLine->first_block->next = NULL;
   pMiniLine->last_block = pMiniLine->first_block;
   DynStrSet(&pMiniLine->first_block->seg->dyn_str, new_line);
   RecalcTextMetrics(text_ptr, attr_ptr->obj->x, text_ptr->baseline_y);
   UpdTextBBox(attr_ptr->obj);
}

