
/* Generic linked bts_list routine.
 * Copyright (C) 1997, 2000 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>
#include "linklist.h"
#include "bts_linklist.h"
#include "memory.h"

#define USE_BTS_LIST 1

#if USE_BTS_LIST
#include "bts_list.h"
#endif

/* Allocate new bts_list. */
struct bts_list *
bts_list_new (void)
{

  return malloc(sizeof (struct bts_list));
}

/* Free bts_list. */
void
bts_list_free (struct bts_list *l)
{
  free(l);
}

/* Allocate new bts_listnode.  Internal use only. */
static struct bts_listnode *
bts_listnode_new (void)
{
	return malloc(sizeof (struct bts_listnode));
}

/* Free bts_listnode. */
static void
bts_listnode_free (struct bts_listnode *node)
{
	free(node);
}


/*
 * Add a node to the bts_list.  If the bts_list was sorted according to the
 * cmp function, insert a new node with the given val such that the
 * bts_list remains sorted.  The new node is always inserted; there is no
 * notion of omitting duplicates.
 */
void
bts_listnode_add_sort (struct bts_list *bts_list, void *val)
{
  struct bts_listnode *n;
  struct bts_listnode *new;
  
  assert (val != NULL);
  
  new = bts_listnode_new ();
  new->data = val;

  if (bts_list->cmp)
    {
      for (n = bts_list->head; n; n = n->next)
	{
	  if ((*bts_list->cmp) (val, n->data) < 0)
	    {	    
	      new->next = n;
	      new->prev = n->prev;

	      if (n->prev)
		n->prev->next = new;
	      else
		bts_list->head = new;
	      n->prev = new;
	      bts_list->count++;
	      return;
	    }
	}
    }

  new->prev = bts_list->tail;

  if (bts_list->tail)
    bts_list->tail->next = new;
  else
    bts_list->head = new;

  bts_list->tail = new;
  bts_list->count++;
}

void
bts_listnode_add_after (struct bts_list *bts_list, struct bts_listnode *pp, void *val)
{
  struct bts_listnode *nn;
  
  assert (val != NULL);
  
  nn = bts_listnode_new ();
  nn->data = val;

  if (pp == NULL)
    {
      if (bts_list->head)
	bts_list->head->prev = nn;
      else
	bts_list->tail = nn;

      nn->next = bts_list->head;
      nn->prev = pp;

      bts_list->head = nn;
    }
  else
    {
      if (pp->next)
	pp->next->prev = nn;
      else
	bts_list->tail = nn;

      nn->next = pp->next;
      nn->prev = pp;

      pp->next = nn;
    }
  bts_list->count++;
}

/* Move given bts_listnode to tail of the bts_list */
void
bts_listnode_move_to_tail (struct bts_list *l, struct bts_listnode *n)
{
  LISTNODE_DETACH(l,n);
  LISTNODE_ATTACH(l,n);
}

/* Delete specific date pointer from the bts_list. */
void
bts_listnode_delete (struct bts_list *bts_list, void *val)
{
  struct bts_listnode *node;

  assert(bts_list);
  for (node = bts_list->head; node; node = node->next)
    {
      if (node->data == val)
	{
	  if (node->prev)
	    node->prev->next = node->next;
	  else
	    bts_list->head = node->next;

	  if (node->next)
	    node->next->prev = node->prev;
	  else
	    bts_list->tail = node->prev;

	  bts_list->count--;
	  bts_listnode_free (node);
	  return;
	}
    }
}



/* Return first node's data if it is there.  */
void *
bts_listnode_head (struct bts_list *bts_list)
{
  struct bts_listnode *node;

  assert(bts_list);
  node = bts_list->head;

  if (node)
    return node->data;
  return NULL;
}

/* Delete all bts_listnode then free bts_list itself. */
void
bts_list_delete (struct bts_list *bts_list)
{
  assert(bts_list);
  bts_list_delete_all_node (bts_list);
  bts_list_free (bts_list);
}

/* Lookup the node which has given data. */
struct bts_listnode *
bts_listnode_lookup (struct bts_list *bts_list, void *data)
{
  struct bts_listnode *node;

  assert(bts_list);
  for (node = bts_listhead(bts_list); node; node = bts_listnextnode (node))
    if (data == bts_listgetdata (node))
      return node;
  return NULL;
}

/* Delete the node from bts_list.  For ospfd and ospf6d. */
void
bts_list_delete_node (struct bts_list *bts_list, struct bts_listnode *node)
{
  if (node->prev)
    node->prev->next = node->next;
  else
    bts_list->head = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    bts_list->tail = node->prev;
  bts_list->count--;
  bts_listnode_free (node);
}






/* ospf_spf.c */
void
bts_list_add_node_prev (struct bts_list *bts_list, struct bts_listnode *current, void *val)
{
  struct bts_listnode *node;
  
  assert (val != NULL);
  
  node = bts_listnode_new ();
  node->next = current;
  node->data = val;

  if (current->prev == NULL)
    bts_list->head = node;
  else
    current->prev->next = node;

  node->prev = current->prev;
  current->prev = node;

  bts_list->count++;
}

/* ospf_spf.c */
void
bts_list_add_node_next (struct bts_list *bts_list, struct bts_listnode *current, void *val)
{
  struct bts_listnode *node;
  
  assert (val != NULL);
  
  node = bts_listnode_new ();
  node->prev = current;
  node->data = val;

  if (current->next == NULL)
    bts_list->tail = node;
  else
    current->next->prev = node;

  node->next = current->next;
  current->next = node;

  bts_list->count++;
}

/* ospf_spf.c */
void
bts_list_add_list (struct bts_list *l, struct bts_list *m)
{
  struct bts_listnode *n;

  for (n = bts_listhead (m); n; n = bts_listnextnode (n))
    bts_listnode_add (l, n->data);
}





#if USE_BTS_LIST
bts_listnode_t *
bts_listnode_lookup_by_key (bts_list_t *bts_list, void *key)
{
  int rv = 0;
  bts_listnode_t *node = NULL;

  struct list_head *pos = NULL, *next =NULL;

  assert(bts_list);
  assert(bts_list->cmp);

  list_for_each_safe(pos, next,  &(bts_list->bucket_head))
  {
     
     node  = list_entry(pos, bts_listnode_t, node);

     if(node->data == NULL)
     {
        continue;
     }
     
     rv = bts_list->cmp(node->data, key);
    
     if(0 == rv)
     {
        return node;
     }   
  } 
  return NULL;
}

/* Delete specific date pointer from the bts_list. */
void bts_listnode_delete_by_key (struct bts_list *bts_list, void *key)
{
    struct bts_listnode *node = NULL;
    struct list_head *pos = NULL, *next = NULL;
    assert(bts_list);
    assert(bts_list->cmp);
    
    list_for_each_safe(pos, next, &(bts_list->bucket_head))
    {
         node  = list_entry(pos, bts_listnode_t, node);
         if(!bts_list->cmp(node->data, key))
         {
             __list_del(pos->prev, pos->next);
             free(node->data);
             free(node);
             node = NULL;
             bts_list->count--;
             return ;
         }
        
    } 
    return;
}

void
bts_listnode_add (struct bts_list *bts_list, void *val)
{
    struct bts_listnode *node = NULL;
    struct list_head *pos = NULL, *next = NULL;
    assert(bts_list);
    assert(bts_list->cmp);
    int find = 0;
    
    list_for_each_safe(pos, next, &(bts_list->bucket_head))
    {
         node  = list_entry(pos, bts_listnode_t, node);
         if(!bts_list->cmp(node->data, val))
         {
            find = 1;
            break;
         }
        
    } 
    if(find)
    {
        free(node->data);
        node->data = val;
        
    }
    else
    {
        node = bts_listnode_new();
        if (NULL == node)
        {
			return;
		}
        node->data = val;
        list_add_tail(&(node->node), &(bts_list->bucket_head));
        bts_list->count++;
        
    }
    return;
}


void
bts_list_delete_all_node (struct bts_list *bts_list)
{
    struct bts_listnode *node = NULL;
    struct list_head *pos = NULL, *next = NULL;
    assert(bts_list);

    
    list_for_each_safe(pos, next, &(bts_list->bucket_head))
    {
        node  = list_entry(pos, bts_listnode_t, node);
        free(node->data);
        free(node);
        list_del(pos);
    } 
    return;
}


void 
bts_list_all_element_lo(struct bts_list *bts_list, bts_iter_func func, void *param)
{
	struct bts_listnode *node = NULL;
	struct list_head *pos = NULL, *next = NULL;
	assert(bts_list);

	list_for_each_safe(pos, next, &(bts_list->bucket_head))
    {
        node  = list_entry(pos, bts_listnode_t, node);
		if (node->data)
		{
			(*func)(node->data, param);
		}

    } 
    return;

}


#else

/* Delete all bts_listnode from the bts_list. */
void
bts_list_delete_all_node (struct bts_list *bts_list)
{
  struct bts_listnode *node;
  struct bts_listnode *next;

  assert(bts_list);
  for (node = bts_list->head; node; node = next)
    {
      next = node->next;
      if (bts_list->del)
	(*bts_list->del) (node->data);
      bts_listnode_free (node);
    }
  bts_list->head = bts_list->tail = NULL;
  bts_list->count = 0;
}

/* Add new data to the bts_list. */
void
bts_listnode_add (struct bts_list *bts_list, void *val)
{
	struct bts_listnode *node;

	assert (val != NULL);

	node = bts_listnode_new ();

	node->prev = bts_list->tail;
	node->data = val;

	if (bts_list->head == NULL)
	bts_list->head = node;
	else
	bts_list->tail->next = node;
	bts_list->tail = node;

	bts_list->count++;
}


/* Delete specific date pointer from the bts_list. */
void
bts_listnode_delete_by_key (struct bts_list *bts_list, void *key)
{
    struct bts_listnode *node;

    assert(bts_list);
    assert(bts_list->cmp);
    for (node = bts_list->head; node; node = node->next)
    {
        if (bts_list->cmp(key, node->data))
        {
            if (node->prev)
                node->prev->next = node->next;
            else
                bts_list->head = node->next;

            if (node->next)
                node->next->prev = node->prev;
            else
                bts_list->tail = node->prev;

            bts_list->count--;
            bts_listnode_free (node);
            return;
        }
    }
}

/* Lookup the node which has given data. */
bts_listnode_t *
bts_listnode_lookup_by_key (bts_list_t *bts_list, void *key)
{
  bts_listnode_t *node = NULL;
  void *dkey = NULL;

  assert(bts_list);
  assert(bts_list->cmp);
  
  for (node = bts_listhead(bts_list); node; node = bts_listnextnode (node)){
      if (!bts_list->cmp(key, node->data))
          return node;
  }
  return NULL;
}


#endif
