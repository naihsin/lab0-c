#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

struct list_head *q_merge(struct list_head *ptr);
struct list_head *q_mergefinal(struct list_head *left, struct list_head *right);

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *el, *safe;
    list_for_each_entry_safe (el, safe, l, list) {
        q_release_element(el);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ptr_ = malloc(sizeof(element_t)), *ptr = ptr_;
    if (!ptr)
        return false;

    ptr->value = strdup(s);

    list_add(&ptr->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ptr_ = malloc(sizeof(element_t)), *ptr = ptr_;
    if (!ptr) {
        return false;
    }

    ptr->value = strdup(s);

    list_add_tail(&ptr->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !q_size(head))
        return NULL;

    element_t *ptr = list_entry(head->next, element_t, list);

    if (sp) {
        strncpy(sp, ptr->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);

    return ptr;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !q_size(head))
        return NULL;

    element_t *ptr = list_entry(head->prev, element_t, list);

    if (sp) {
        strncpy(sp, ptr->value, bufsize);
        sp[bufsize - 1] = '\0';
    }


    list_del_init(head->prev);

    return ptr;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || !q_size(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next;
    while (fast->next != head->next && fast->next->next != head->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del_init(slow);
    q_release_element(list_entry(slow, element_t, list));

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!q_size(head))
        return false;

    struct list_head *cur = head->next;
    while (cur->next != head) {
        element_t *first = list_entry(cur, element_t, list);
        element_t *second = list_entry(cur->next, element_t, list);
        if (!strcmp(first->value, second->value)) {
            list_del_init(cur->next);
            q_release_element(second);
        } else {
            cur = cur->next;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *left = head->next, *right = left->next;
    while (left != head && right != head) {
        list_move(left, right);
        left = left->next;
        right = left->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    struct list_head *li, *safe;
    list_for_each_safe (li, safe, head) {
        list_move(li, head);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || !q_size(head) || q_size(head) == 1)
        return;
    head->prev->next = NULL;
    struct list_head *ptr = q_merge(head->next);
    head->next = ptr;
    ptr->prev = head;

    while (ptr->next) {
        ptr = ptr->next;
    }
    ptr->next = head;
    head->prev = ptr;
}

struct list_head *q_merge(struct list_head *ptr)
{
    if (!ptr || !ptr->next)
        return ptr;

    struct list_head *slow = ptr, *fast = ptr;
    while (fast->next != ptr->next && fast->next->next != ptr->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *left, *right;
    fast = slow->next;
    slow->next = NULL;
    left = q_merge(ptr);
    right = q_merge(fast);
    return q_mergefinal(left, right);
}

struct list_head *q_mergefinal(struct list_head *left, struct list_head *right)
{
    struct list_head *head = left, *prev = NULL, **ptr = &head, **node;

    for (node = NULL; left && right; *node = (*node)->next) {
        element_t *el = list_entry(left, element_t, list);
        element_t *el2 = list_entry(right, element_t, list);
        node = (strcmp(el->value, el2->value) < 0) ? &left : &right;
        (*node)->prev = prev;
        prev = *node;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = left ? left : right;
    (*ptr)->prev = prev;
    return head;
}