#ifndef __BTS_LINKLIST_H__
#define __BTS_LINKLIST_H__


#include "bts_list.h"
//#include "bts_hashtable.h"
/* bts_listnodes must always contain data to be valid. Adding an empty node
 * to a bts_list is invalid
 */


typedef struct  bts_listnode
{
  struct bts_listnode *next;
  struct bts_listnode *prev;    
  /* private member, use getdata() to retrieve, do not access directly */
  void *data;
  struct list_head  node;
} bts_listnode_t;



typedef int (*bts_list_cmp_func)(void *val1, void *val2); 
typedef void (*bts_list_del_func)(void *val);

typedef struct bts_list
{
  struct bts_listnode *head;
  struct bts_listnode *tail;
  struct list_head   bucket_head;
  //pthread_mutex_t mutex;

  
  /* invariant: count is the number of bts_listnodes in the bts_list */
  unsigned int count;
    
  /*
   * Returns -1 if val1 < val2, 0 if equal?, 1 if val1 > val2.
   * Used as definition of sorted for bts_listnode_add_sort
   */
  bts_list_cmp_func cmp;

  /* callback to free user-owned data when bts_listnode is deleted. supplying
   * this callback is very much encouraged!
   */
  bts_list_del_func del;
} bts_list_t;

#define bts_listnextnode(X) ((X) ? ((X)->next) : NULL)
#define bts_listhead(X) ((X) ? ((X)->head) : NULL)
#define bts_listtail(X) ((X) ? ((X)->tail) : NULL)
#define bts_listcount(X) ((X)->count)
#define bts_list_isempty(X) ((X)->head == NULL && (X)->tail == NULL)
#define bts_listgetdata(X) (assert((X)->data != NULL), (X)->data)

/* Prototypes. */
extern struct bts_list *bts_list_new(void); /* encouraged: set bts_list.del callback on new bts_lists */
extern void bts_list_free (struct bts_list *);

extern void bts_listnode_add (struct bts_list *, void *);
extern void bts_listnode_add_sort (struct bts_list *, void *);
extern void bts_listnode_add_after (struct bts_list *, struct bts_listnode *, void *);
extern void bts_listnode_move_to_tail (struct bts_list *, struct bts_listnode *);
extern void bts_listnode_delete (struct bts_list *, void *);
extern struct bts_listnode *bts_listnode_lookup (struct bts_list *, void *);
extern void *bts_listnode_head (struct bts_list *);

extern void bts_list_delete (struct bts_list *);
extern void bts_list_delete_all_node (struct bts_list *);

/* For ospfd and ospf6d. */
extern void bts_list_delete_node (struct bts_list *, struct bts_listnode *);

/* For ospf_spf.c */
extern void bts_list_add_node_prev (struct bts_list *, struct bts_listnode *, void *);
extern void bts_list_add_node_next (struct bts_list *, struct bts_listnode *, void *);
extern void bts_list_add_bts_list (struct bts_list *, struct bts_list *);

extern bts_listnode_t *bts_listnode_lookup_by_key (bts_list_t *bts_list, void *key);

typedef void (*bts_iter_func)(void *data, void *param);

extern void bts_list_all_element_lo(struct bts_list *bts_list, bts_iter_func func, void *param);



/* List iteration macro. 
 * Usage: for (ALL_BTS_LIST_ELEMENTS (...) { ... }
 * It is safe to delete the bts_listnode using this macro.
 */
#define ALL_BTS_LIST_ELEMENTS(bts_list,node,nextnode,data) \
  (node) = bts_listhead(bts_list), ((data) = NULL); \
  (node) != NULL && \
    ((data) = bts_listgetdata(node),(nextnode) = node->next, 1); \
  (node) = (nextnode), ((data) = NULL)

/* read-only bts_list iteration macro.
 * Usage: as per ALL_BTS_LIST_ELEMENTS, but not safe to delete the bts_listnode Only
 * use this macro when it is *immediately obvious* the bts_listnode is not
 * deleted in the body of the loop. Does not have forward-reference overhead
 * of previous macro.
 */
#define ALL_BTS_LIST_ELEMENTS_RO(bts_list,node,data) \
  (node) = bts_listhead(bts_list), ((data) = NULL);\
  (node) != NULL && ((data) = bts_listgetdata(node), 1); \
  (node) = bts_listnextnode(node), ((data) = NULL)

/* these *do not* cleanup bts_list nodes and referenced data, as the functions
 * do - these macros simply {de,at}tach a bts_listnode from/to a bts_list.
 */
 
/* List node attach macro.  */
#define BTS_LISTNODE_ATTACH(L,N) \
  do { \
    (N)->prev = (L)->tail; \
    (N)->next = NULL; \
    if ((L)->head == NULL) \
      (L)->head = (N); \
    else \
      (L)->tail->next = (N); \
    (L)->tail = (N); \
    (L)->count++; \
  } while (0)

/* List node detach macro.  */
#define BTS_LISTNODE_DETACH(L,N) \
  do { \
    if ((N)->prev) \
      (N)->prev->next = (N)->next; \
    else \
      (L)->head = (N)->next; \
    if ((N)->next) \
      (N)->next->prev = (N)->prev; \
    else \
      (L)->tail = (N)->prev; \
    (L)->count--; \
  } while (0)

#endif /* __BTS_LINKLIST_H__ */
