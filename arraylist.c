#include "arraylist.h"
#include <stdlib.h>


Arraylist
arraylist_create(size_t capacity)
{
    return (Arraylist){
        .capacity = capacity > 0 ? capacity : 1,
        .length   = 0,
        .data     = malloc(sizeof(element_t *) * capacity),
    };
}

void
_arraylist_resize(Arraylist *arraylist)
{
    arraylist->capacity += AL_INCREMENT;
    arraylist->data =
    realloc(arraylist->data, arraylist->capacity * sizeof(element_t *));
}

size_t
arraylist_append(Arraylist *arraylist, element_t element)
{
    if (arraylist->length == arraylist->capacity)
        _arraylist_resize(arraylist);
    arraylist->data[arraylist->length]    = malloc(sizeof(element_t));
    *(arraylist->data[arraylist->length]) = element;
    return arraylist->length++;
}

void
arraylist_modify(Arraylist *arraylist, size_t index, element_t element)
{
    if (index >= arraylist->length)
        return;
    *(arraylist->data[index]) = element;
}

// i and j MUST be 0<=i,j<length
void
_swap(Arraylist *arraylist, size_t i, size_t j)
{
    element_t *temp    = arraylist->data[i];
    arraylist->data[i] = arraylist->data[j];
    arraylist->data[j] = temp;
}

void
arraylist_insert(Arraylist *arraylist, size_t index, element_t element)
{
    if (index >= arraylist->length)
        return;
    arraylist_append(arraylist, element);
    for (int i = arraylist->length - 1; i > index; i--)
        _swap(arraylist, i, i - 1);
}


element_t
arraylist_remove(Arraylist *arraylist, size_t index)
{
    element_t temp;
    if (index >= arraylist->length)
        return ELEM_ERR;
    temp = arraylist_get(*arraylist, index);
    free(arraylist->data[index]);
    for (int i = index; i < arraylist->length; i++)
    {
        arraylist->data[i] = arraylist->data[i + 1];
    }
    --arraylist->length;
    return temp;
}


element_t
arraylist_get(Arraylist arraylist, size_t index)
{
    if (index >= arraylist.length)
        return ELEM_ERR;
    return *((element_t *) arraylist.data[index]);
}
