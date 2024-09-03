#include "arraylist.h"
#include <pthread.h>
#include <stdlib.h>


inline size_t
arraylist_length(Arraylist arraylist)
{
    return arraylist.length;
}

Arraylist
arraylist_create(size_t capacity)
{
    Arraylist arraylist = (Arraylist){
        .capacity = capacity > 0 ? capacity : 1,
        .length   = 0,
        .data     = malloc(sizeof(element_t *) * capacity),
    };
    pthread_mutex_init(&arraylist.lock, NULL);
    return arraylist;
}

void
_arraylist_resize(Arraylist *arraylist)
{
    arraylist->capacity += AL_INCREMENT;
    arraylist->data =
    realloc(arraylist->data, arraylist->capacity * sizeof(element_t *));
}

size_t
_arraylist_append_nolock(Arraylist *arraylist, element_t element)
{
    int len;
    if (arraylist->length == arraylist->capacity)
        _arraylist_resize(arraylist);
    arraylist->data[arraylist->length]    = malloc(sizeof(element_t));
    *(arraylist->data[arraylist->length]) = element;
    len                                   = arraylist->length++;
    return len;
}

size_t
arraylist_append(Arraylist *arraylist, element_t element)
{
    pthread_mutex_lock(&arraylist->lock);
    size_t ret = _arraylist_append_nolock(arraylist, element);
    pthread_mutex_unlock(&arraylist->lock);
    return ret;
}

void
arraylist_modify(Arraylist *arraylist, size_t index, element_t element)
{
    pthread_mutex_lock(&arraylist->lock);
    if (index < arraylist->length)
        *(arraylist->data[index]) = element;
    pthread_mutex_unlock(&arraylist->lock);
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
    pthread_mutex_lock(&arraylist->lock);
    if (index < arraylist->length)
    {
        _arraylist_append_nolock(arraylist, element);
        for (int i = arraylist->length - 1; i > index; i--)
            _swap(arraylist, i, i - 1);
    }
    pthread_mutex_unlock(&arraylist->lock);
}

element_t
_arraylist_get_nolock(Arraylist arraylist, size_t index)
{
    if (index >= arraylist.length)
    {
        return ELEM_ERR;
    }
    return *(arraylist.data[index]);
}

element_t
arraylist_get(Arraylist arraylist, size_t index)
{
    pthread_mutex_lock(&arraylist.lock);
    element_t ret = _arraylist_get_nolock(arraylist, index);
    pthread_mutex_unlock(&arraylist.lock);
    return ret;
}

element_t
arraylist_remove(Arraylist *arraylist, size_t index)
{
    pthread_mutex_lock(&arraylist->lock);
    element_t temp;
    if (index >= arraylist->length)
    {
        pthread_mutex_unlock(&arraylist->lock);
        return ELEM_ERR;
    }
    temp = _arraylist_get_nolock(*arraylist, index);
    free(arraylist->data[index]);
    for (int i = index; i < arraylist->length; i++)
    {
        arraylist->data[i] = arraylist->data[i + 1];
    }
    --arraylist->length;
    pthread_mutex_unlock(&arraylist->lock);
    pthread_mutex_destroy(&arraylist->lock);

    return temp;
}
