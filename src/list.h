#ifndef _LIST_H
#define _LIST_H

#define INIT_LIST_NODE(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = NULL; (ptr)->prev = (ptr); \
} while (0)

#define list_for_each(pos, head)    \
        for(pos = (head); pos != NULL; \
                pos = pos->next)    
				
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next) 

static __inline__ void list_add_tail(ListHead *head,
									 ListHead *add)
{
	add->prev = head;
	add->next = head->next;
	head->next = add;
}

static __inline__ void __list_add(ListHead * new,
                                  ListHead * prev,
                                  ListHead * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static __inline__ void list_add(ListHead *new, ListHead *head)
{
	__list_add(new, head, head->next);
}

static __inline__ void __list_del(ListHead * prev, ListHead * next)
{
	next->prev = prev;
	prev->next = next;
}

static __inline__ void list_del(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = entry->prev = 0;
}

static __inline__ void list_del_init(ListHead *entry)
{
	__list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static __inline__ int list_empty(ListHead *head)
{
	return head->next == head;
}

static __inline__ void list_splice(ListHead *list, ListHead *head)
{
	ListHead *first = list->next;

	if (first != list)
	{
		ListHead *last = list->prev;
		ListHead *at = head->next;
		
		first->prev = head;
		head->next = first;
		
		last->next = at;
		at->prev = last;
	}
}

#endif
