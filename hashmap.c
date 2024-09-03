#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hashmap.h"

//
// Created by Emanuel on 02.09.2024.
//

#define MIN_CAPACITY (1 << 4)
#define MAX_CAPACITY (1 << 30)
#define LOAD_FACTOR 0.75

typedef enum { ACTIVE, TOMBSTONE } BucketStatus;

typedef struct Bucket
{
    void *key;
    void *value;
    size_t hash;
    struct Bucket *next;
    BucketStatus status;
} Bucket;

struct HashMap
{
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t value_size;

    hash hash_func;
    Bucket **buckets;
};

Bucket* create_bucket(const void *key, const void *value, const size_t key_size, const size_t value_size,
                      const size_t hash)
{
    Bucket *bucket = malloc(sizeof(Bucket));
    assert(bucket != NULL);

    bucket->key = malloc(key_size);
    assert(bucket->key != NULL);
    memcpy(bucket->key, key, key_size);

    bucket->value = malloc(value_size);
    assert(bucket->value != NULL);
    memcpy(bucket->value, value, value_size);

    bucket->hash   = hash;
    bucket->status = ACTIVE;
    bucket->next   = NULL;

    return bucket;
}

size_t _hash(const void *key, const size_t key_size)
{
    //DJB2 hash func
    const char *str = key;
    size_t hash     = 5381;
    int c;

    while ((c = *str++))
        hash  = ((hash << 5) + hash) + c;

    return hash;
}

double calc_load_fac(HashMap hm)
{
    return (double) hm->size / hm->capacity;
}

int hm_resize(HashMap hm)
{
    size_t new_capacity = hm->capacity * 2;
    if (new_capacity > MAX_CAPACITY)
        new_capacity = MAX_CAPACITY;
    if (new_capacity >= MAX_CAPACITY * LOAD_FACTOR)
        return 1;
    Bucket **new_buckets = realloc(hm->buckets, new_capacity * sizeof(Bucket *));
    assert(new_buckets != NULL);
    memset(new_buckets + hm->capacity, 0, (new_capacity - hm->capacity) * sizeof(Bucket *));
    hm->buckets = new_buckets;

    for (size_t i = 0; i < hm->capacity; i++)
    {
        Bucket *bucket = new_buckets[i];
        new_buckets[i] = NULL;
        while (bucket != NULL)
        {
            Bucket *next = bucket->next;
            if (bucket->status == ACTIVE)
            {
                const size_t hash = hm->hash_func(bucket->key, hm->key_size) % new_capacity;
                bucket->next      = new_buckets[hash];
                new_buckets[hash] = bucket;
            }
            else
            {
                free(bucket->key);
                free(bucket->value);
                free(bucket);
            }
            bucket = next;
        }
    }
    hm->capacity = new_capacity;

    return 0;
}

HashMap hm_create_ch(const size_t hm_capacity, const size_t key_size, const size_t value_size,
                     hash hash_func)
{
    const HashMap hm = hm_create(hm_capacity, key_size, value_size);
    hm->hash_func    = hash_func;
    return hm;
}

HashMap hm_create(const size_t hm_capacity, const size_t key_size, const size_t value_size)
{
    const HashMap hm = malloc(sizeof(*hm));
    assert(hm != NULL);
    hm->hash_func = _hash;
    hm->capacity  = (hm_capacity < MIN_CAPACITY) ? MIN_CAPACITY : hm_capacity;
    hm->buckets   = calloc(hm->capacity, sizeof(Bucket *));
    assert(hm->buckets != NULL);
    hm->size       = 0;
    hm->key_size   = key_size;
    hm->value_size = value_size;

    return hm;
}

int hm_destroy(HashMap hm)
{
    for (size_t i = 0; i < hm->capacity; ++i)
    {
        for (Bucket *bucket = hm->buckets[i]; bucket != NULL;)
        {
            Bucket *next = bucket->next;
            free(bucket->key);
            free(bucket->value);
            free(bucket);
            bucket = next;
        }
    }
    free(hm->buckets);
    free(hm);
    return 0;
}

void* hm_get(HashMap hm, const void *key)
{
    const size_t hash = hm->hash_func(key, hm->key_size) % hm->capacity;
    for (Bucket *bucket = hm->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (bucket->status == ACTIVE && memcmp(bucket->key, key, hm->key_size) == 0)
        {
            return bucket->value;
        }
    }
    return NULL;
}

int hm_set(HashMap hm, const void *key, const void *value)
{
    const size_t hash = hm->hash_func(key, hm->key_size) % hm->capacity;
    for (Bucket *bucket = hm->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (memcmp(bucket->key, key, hm->key_size) == 0)
        {
            free(bucket->value);
            bucket->value = malloc(hm->value_size);
            assert(bucket->value != NULL);
            memcpy(bucket->value, value, hm->value_size);
            bucket->status = ACTIVE;
            return 0;
        }
    }

    return 1;
}

int hm_put(HashMap hm, const void *key, const void *value)
{
    if (calc_load_fac(hm) > LOAD_FACTOR)
        if (hm_resize(hm) != 0)
            return 1;

    const size_t hash = hm->hash_func(key, hm->key_size) % hm->capacity;
    for (Bucket *bucket = hm->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (memcmp(bucket->key, key, hm->key_size) == 0)
        {
            free(bucket->value);
            bucket->value = malloc(hm->value_size);
            assert(bucket->value != NULL);
            memcpy(bucket->value, value, hm->value_size);
            bucket->status = ACTIVE;
            return 0;
        }
    }

    Bucket *bucket    = create_bucket(key, value, hm->key_size, hm->value_size, hash);
    bucket->next      = hm->buckets[hash];
    hm->buckets[hash] = bucket;
    hm->size++;

    return 0;
}

size_t hm_size(HashMap hm)
{
    return hm->size;
}

int hm_remove(HashMap hm, const void *key)
{
    const size_t hash = hm->hash_func(key, hm->key_size) % hm->capacity;
    Bucket *prev      = NULL;
    for (Bucket *bucket = hm->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (memcmp(bucket->key, key, hm->key_size) == 0)
        {
            if (prev == NULL)
            {
                hm->buckets[hash] = bucket->next;
            }
            else
            {
                prev->next = bucket->next;
            }
            bucket->status = TOMBSTONE;
            hm->size--;
            return 0;
        }
        prev = bucket;
    }

    return 1;
}
