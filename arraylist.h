#include "window.h" // Window
#include <stddef.h> // size_t

/* As this is going to be used for a few
 * elements, once the array grows it only
 * add 1 to previous size */
#define AL_INCREMENT 1;
#define ELEM_ERR     0;

// type declarations
typedef Window *element_t;
typedef struct __araylist
{
    element_t **data;
    size_t      capacity;
    size_t      length;
} Arraylist;

// ---------- function declarations ---------- //
// create a list
Arraylist arraylist_create(size_t capacity);
// modify the element in a valid index
void arraylist_modify(Arraylist *arraylist, size_t index, element_t element);
// append an element to the end of the list
size_t arraylist_append(Arraylist *arraylist, element_t element);
// insert an element at index (quite inefficient)
void arraylist_insert(Arraylist *arraylist, size_t index, element_t element);
// remove an element at index
element_t arraylist_remove(Arraylist *arraylist, size_t index);
// get the element at some index
element_t arraylist_get(Arraylist arraylist, size_t index);
// just for 'debugging'
void arraylist_print(Arraylist arraylist);
