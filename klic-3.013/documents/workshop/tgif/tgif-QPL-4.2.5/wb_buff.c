/*
 * Author:      Magnos Martinello, <magnos@land.ufrj.br> in April, 1999.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb_buff.c,v 1.5 2009/10/02 15:31:24 william Exp $
 */

/*
 * TODO: rever a rotina de insercao. Analisar o buffer como sendo circular.
 *
 */


#define _INCLUDE_FROM_WB_BUFF_C_

#include "tgifdefs.h" 

#include "wb_buff.e"

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#include <pthread.h>
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

#include <stdio.h>
#include <stdlib.h>

/*
  Structure of each component from the buffer
*/

typedef struct item_s {
   void *data;
   int size; 
   struct item_s *prev;
   struct item_s *next;
} item_t;

/*******************************************************************
  Header:
   - Maximum number of items in the buffer
   - Size of each item
   - Number of items in the buffer
   - SORTED/UNSORTED insertion
   - Pointer to first item of the queue
   - Pointer to last item of the queue
     (Item is inserted in the start pointer and removed from the end pointer)
   - Function to compare two items of the buffer
*******************************************************************/
typedef struct {
   int n;
   int option;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
   pthread_mutex_t *mp;
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
   item_t *start;
   item_t *end;
   int (*compar)(const void *, const void *);
} header_t;


#define INITMAXTABLE 100

int MAXTABLE=INITMAXTABLE;  /* Maximum number of active buffers */

header_t **table=NULL;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
pthread_mutex_t table_mp;
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/***********************************************************************
 * This function creates a new buffer and returns its descriptor as    *
 * an integer                                                          *
 **********************************************************************/
int buff_init (int trash1, int trash2, int option, int (*compar)(const void *,
const void *))
{
  header_t *header;
  int aux;
  static int last_used_desc = INITMAXTABLE - 1;
  static int is_first = TRUE;
  
  /* If option is invalid then return error */
  if (((option & SORTED)==SORTED) && ((option & UNSORTED)==UNSORTED))
    return(-1);

  /* If insertion is sorted and no comparison function has been assigned,
     then return error */
  if ((option == SORTED) && (compar == NULL))
    return(-1);

  if (is_first)
  {
		if ((table = (header_t**)malloc(MAXTABLE*sizeof(header_t*))) == NULL)
		{
			fprintf(stderr,"Allocation error. (buff_init)\n");
			exit(1);
		}
		is_first = FALSE;
  }
  
  /* Looks for an empty position in the Descriptor Table */
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_lock(&table_mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
  aux = last_used_desc;
  last_used_desc = (last_used_desc + 1) % MAXTABLE;
  while ((table[last_used_desc] != NULL) && (aux != last_used_desc))
    last_used_desc = (last_used_desc + 1) % MAXTABLE;
  if (!(table[last_used_desc] == NULL)){
  		last_used_desc = MAXTABLE;
		MAXTABLE += 10;
		table=(header_t**)realloc(table,MAXTABLE*sizeof(header_t*));
  }
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_unlock(&table_mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

  /* Create a new header and store its pointer in the descriptor table */
  header = (header_t *) malloc (sizeof(header_t));
  if (header == NULL)
    return(-1);
  memset(header, 0, sizeof(header_t));
  table[last_used_desc] = header;

  /* Set values for attributes in the header */
  header->n = 0;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  header->mp = malloc(sizeof(pthread_mutex_t));
  if (header->mp == NULL)
    return(-1);
  memset(header->mp, 0, sizeof(pthread_mutex_t));
  pthread_mutex_init(header->mp, NULL);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
  header->option = option;
  header->compar = compar;
  header->start = header->end = NULL;

  return last_used_desc;
}

void buff_cleanup ()
{
  if (table != NULL) free(table);
  table = NULL;
}

/***********************************************************************
 * This function inserts a new item in the buffer specified by its     *
 * descriptor. The insertion may be sorted or not, depending on the    *
 * option set during the buffer initialization. It returns a negative  *
 * integer if the insertion wasn't successful.                         *
 **********************************************************************/
int buff_ins (int bd, const void *buffer)
{
  header_t *header;
  item_t *item, *newitem, *auxitem;
 
  if (table[bd] == NULL)
    return(-1);

  header = table[bd];

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_lock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
  
  /* Create the new item */
  newitem = (item_t *) malloc (sizeof(item_t));
  if (newitem == NULL)
    return(-1);
  memset(newitem, 0, sizeof(item_t));
  newitem->size = strlen((char*)buffer)+1;
  newitem->data = (int*)malloc (newitem->size);
  if (newitem->data == NULL)
    return(-1);
  strcpy((char*)(newitem->data), (char*)buffer);

  /* Unsorted insertion or empty buffer */
  if (((header->option & UNSORTED) == UNSORTED) || (header->n == 0)){
  
  	 newitem->prev = header->start;
	 
	 if (newitem->prev != NULL)	
	 	 newitem->prev->next = newitem;
		 
	 header->start = newitem;
	 
	 if (header->end == NULL)
	 	header->end=newitem;

  }
  else /* Sorted buffer */
  if ((header->option & SORTED) == SORTED){

     /* Search position where new item will be inserted according to
       comparison function previously specified                     */
     /* Search is made from begining to end and from end to begining */
     item = header->end;
     auxitem = header->start;
	 
     while (((item != header->start) &&
            (header->compar(item->data,buffer)<= 0)) &&
           ((auxitem != header->end) &&
			(header->compar(auxitem->data,buffer)>0))){
      item = item->next;
      auxitem = auxitem->prev;
     }

     /* Decide wich pointer points to the correct position */
     if (header->compar(auxitem->data, buffer)<=0)
     {
	 	item = auxitem;
	 }
	 else
	 {
	 	if (item->prev!=NULL)
			item = item->prev; 
	 }

     /* front insertion */
     if (item == header->start)
	 {
	 	if (header->compar(header->start->data,buffer) < 0) {
    	    header->n++;
	 		newitem->next = NULL;
      	    newitem->prev = header->start;
			header->start->next = newitem;
          	header->start = newitem;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
          	pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
			return(0);
       	 } 
	 }
	 
	 /* rear insertion */
     if (header->compar(buffer, header->end->data) < 0)
	 {
		newitem->next = header->end;
		newitem->prev = NULL;
		header->end->prev = newitem;
        header->end = newitem;
       	header->n++;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
     	pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
		return(0);
	 }

	 /* is duplicated? */
    if ((header->option & DUPLICATED) != DUPLICATED)
      if (header->compar(item->data, buffer)==0){
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
        pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
 	     return(-1);
    }

    /* Set pointers */
    if (item->next!=NULL)
	 	(item->next)->prev = newitem;
		
    newitem->next = item->next;
    newitem->prev = item;
    item->next = newitem;
  }

  header->n++;
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
	
  return(0);
}

/***********************************************************************
 * This function removes an item from the buffer especified by its     *
 * descriptor. The data stored in the item is copied to a pointer      *
 * especified by the user. It returns a negative integer if an error   *
 * occurs                                                              *
 **********************************************************************/
int buff_rem (int bd, void **buffer)
{
  header_t *header;
  
  int buffsize =0;

  if (table[bd] == NULL)
    return(-1);

  header = table[bd];

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_lock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

  if (header->n == 0){
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
    pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
    return(-1);
  }
  
  if ((*buffer = malloc(header->end->size * sizeof(char)))==NULL)
  {
    fprintf(stderr,"Allocation error. (buff_rem)\n");
    exit(1);
  }
  
  buffsize = header->end->size;
  
  memcpy(*buffer, header->end->data, header->end->size);
  header->end = header->end->next;
  header->n--;
  
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

  return(buffsize);
}

/***********************************************************************
 * This function disposes memory occupied by the buffer.               *
 **********************************************************************/
int buff_destroy (int bd)
{
  item_t *item=NULL, *next_item=NULL;
  int i;

  if (table[bd] == NULL)
    return(-1);

  item = table[bd]->end;
  for (i = 0; item!=NULL; i++, item=next_item) {
    next_item = item->next;
    free(item->data);
    free(item);
  }

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_destroy(table[bd]->mp);
  free(table[bd]->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
  free(table[bd]);
  table[bd] = NULL;

  return(0);
}

/***********************************************************************
 * This function makes all items avaiable for insertion                *
 **********************************************************************/
int buff_empty (int bd)
{
  header_t *header;
  int aux;
  
  if (table[bd] == NULL)
    return(-1);

  header = table[bd];
  aux = header->n;
  
  if ( aux == buff_emptyn(bd,aux) )
  {
  	return(0);
  }
  else
  	return(-1);
  
}

/***********************************************************************
 * This function makes the first n items avaiable for insertion. It    *
 * returns the number of items disposed.                               *
 **********************************************************************/
int buff_emptyn (int bd, int n)
{
  header_t *header;
  int nitems, i;

  if (table[bd] == NULL)
    return(-1);
	 

  header = table[bd];
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_lock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */
  nitems = header->n;
  i = 0;
  while ( (header->n > 0) && (i < n)) {
    i++;
    header->n--;

	 free(header->end->data);
 	 free(header->end);
	 
	 header->end = header->end->next;
	 
	 if (header->end == header->start)
	 {
	 	header->start=NULL;
	 }
  }

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
#ifdef PTHREAD_MUTEX
  pthread_mutex_unlock(header->mp);
#endif /* PTHREAD_MUTEX */
#endif /* (defined(PTHREAD) || defined(HAVE_LIBPTHREAD)) */

  return((nitems<n?nitems:n));
}

/***********************************************************************
 * This function returns the number of active items in the buffer      *
 **********************************************************************/

int buff_items (int bd)  {

  if (table[bd] == NULL)
    return -1;
  else
    return table[bd]->n;
}

/***********************************************************************
 * This function concatenates two buffers, creating a new unsorted     *
 * insertion buffer                                                    *
 **********************************************************************/
int buff_conc (int bd1, int bd2)
{
  header_t *hd1, *hd2;
  item_t *item;
  int nbd, i, j;
  char *buffer=NULL;

  if ( (table[bd1] == NULL) || (table[bd2] == NULL))
    return(-1);

  hd1 = table[bd1];
  hd2 = table[bd2];

  nbd = buff_init (0, 0, UNSORTED, hd1->compar);

  item = hd1->end;
  i = 0;
  
  for (j=0;j<2;j++)
  {
    i = 0;
	 
 	 if (j==0) 
	 	item = hd1->end;
	 else
 	 	item = hd2->end;
		
	 while ((i < (j==0?hd1->n:hd2->n)))
	 {

		if (item == NULL)
		{
	 	  fprintf(stderr,"Internal error. (buff_conc)\n");
		  exit(1);
		}
   	    buffer = (char*) malloc (item->size);
   	    if (buffer == NULL)
	   	    return(-1);
   	    memset(buffer, 0, item->size);

   	    memcpy(buffer, item->data, item->size);
   	    buff_ins(nbd, buffer);
   	    item = item->next;

   	    i++;
	 }
  }

  if (buffer) free (buffer);

  return(nbd);
}

/***********************************************************************
 * This function sorts a buffer according to a comparison function     *
 * pointed to by compar                                                *
 **********************************************************************/
/*
int buff_sort (int bd, int (*compar)(const void *, const void *))
{
  header_t *header;
  char *buffer;
  int i, n;
  item_t *item;

  if (table[bd] == NULL)
    return(-1);

  header = table[bd];

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
  pthread_mutex_lock(header->mp);
#endif

  if ((header->option & SORTED) == SORTED){
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
    pthread_mutex_unlock(header->mp);
#endif
    return(0);
  }

  if (header->compar == NULL){
#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
    pthread_mutex_unlock(header->mp);
#endif
    return(-1);
  }

  n = header->n;
  buffer = (char*) malloc (n * header->size);
  if (buffer == NULL)
    return(-1);
  memset(buffer, 0, n*header->size);
  i = 0;
  item = header->end;
  while (i < n){
    memcpy(buffer + i * header->size, item->data, header->size);
    item = item->next;
    i++;
  }

  qsort (buffer, n, header->size, compar);

  i = 0;
  item = header->end;
  while (i < n){
    memcpy(item->data, buffer + i * header->size, header->size);
    item = item->next;
    i++;
  }

  free (buffer);

#if (defined(PTHREAD) || defined(HAVE_LIBPTHREAD))
  pthread_mutex_unlock(header->mp);
#endif

  return(0);
}
*/
int buff_show(int bd)
{
  int i;
  item_t *item = (table[bd])->end;
  fprintf(stderr,"Buffer info (descriptor: %d):\n",bd);
  fprintf(stderr,"\tn: %d\n\toption: %d\n\tstart: %p\n\tend: %p\n",
    (table[bd])->n,
    (table[bd])->option,
	(table[bd])->start,
    (table[bd])->end);
  fprintf(stderr,"\tDados: \n");	 
  for (i=0; i<(table[bd])->n; i++)
  {
  		fprintf(stderr,"\t%s (this: %p prev: %p next: %p) -> \n",(char*)item->data,
			item,item->prev,item->next);
		item = item->next;
  }	 
  fprintf(stderr,"\tis the first\n");
  return (0);
}

