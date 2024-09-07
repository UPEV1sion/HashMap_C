#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hashmap.h"

//
// Created by Emanuel on 04.09.2024.
//

#define MIN_CAPACITY (1 << 4)
#define MAX_CAPACITY (1 << 30)
#define LOAD_FACTOR 0.75

typedef enum { TOMBSTONE, ACTIVE } BucketStatus;

typedef struct Bucket
{
    size_t hash;
    BucketStatus status;
    char payload[];
} Bucket;

struct HashMap
{
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t value_size;
    size_t bucket_size;

    hash hash_func;
    Bucket *buckets;
};

static size_t default_hash(const void *key, const size_t key_size)
{
    // DJB2 hash func variation
    const unsigned char *data = key;
    size_t hash               = 5381;

    for (size_t i = 0; i < key_size; ++i)
    {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

static inline double calc_load_fac(const HashMap hm)
{
    return (double) hm->size / hm->capacity;
}

static inline Bucket* get_bucket(const HashMap hm, const size_t hash)
{
    return (Bucket *) ((char *) hm->buckets + hash * hm->bucket_size);
}

static int hm_resize(const HashMap hm)
{
    size_t new_capacity = hm->capacity * 2;
    if (new_capacity > MAX_CAPACITY) new_capacity = MAX_CAPACITY;
    if (hm->size >= MAX_CAPACITY * LOAD_FACTOR)
        return 1;

    Bucket *new_buckets = calloc(new_capacity, hm->bucket_size);
    assert(new_buckets != NULL);

    for (size_t i = 0; i < hm->capacity; ++i)
    {
        Bucket *bucket = get_bucket(hm, i);
        if (bucket->status == ACTIVE)
        {
            size_t new_hash    = hm->hash_func(bucket->payload, hm->key_size) % new_capacity;
            Bucket *new_bucket = (Bucket *) ((char *) new_buckets + new_hash * hm->bucket_size);
            while (new_bucket->status == ACTIVE)
            {
                new_hash   = (new_hash + 1) % new_capacity;
                new_bucket = (Bucket *) ((char *) new_buckets + new_hash * hm->bucket_size);
            }
            memcpy(new_bucket, bucket, hm->bucket_size);
        }
    }

    free(hm->buckets);
    hm->buckets  = new_buckets;
    hm->capacity = new_capacity;

    return 0;
}

HashMap hm_create(const size_t hm_capacity, const size_t key_size, const size_t value_size, const hash hash_func)
{
    HashMap hm = malloc(sizeof(*hm));
    assert(hm != NULL);

    hm->hash_func   = (hash_func == NULL) ? default_hash : hash_func;
    hm->capacity    = (hm_capacity < MIN_CAPACITY) ? MIN_CAPACITY : hm_capacity;
    hm->bucket_size = sizeof(Bucket) + key_size + value_size;
    hm->buckets     = malloc(hm->bucket_size * hm->capacity);
    assert(hm->buckets != NULL);

    hm->size       = 0;
    hm->key_size   = key_size;
    hm->value_size = value_size;

    for (size_t i = 0; i < hm->capacity; ++i)
    {
        Bucket *bucket = get_bucket(hm, i);
        bucket->status = TOMBSTONE;
        bucket->hash   = 0;
    }

    return hm;
}

int hm_destroy(const HashMap hm)
{
    free(hm->buckets);
    free(hm);
    return 0;
}

void* hm_get(const HashMap hm, const void *key)
{
    size_t hash             = hm->hash_func(key, hm->key_size) % hm->capacity;
    const size_t start_hash = hash;
    Bucket *bucket          = get_bucket(hm, hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->payload, key, hm->key_size) == 0)
        {
            return bucket->payload + hm->key_size;
        }
        hash   = (hash + 1) % hm->capacity;
        bucket = get_bucket(hm, hash);
        if (bucket == get_bucket(hm, start_hash))
        {
            break;
        }
    }

    return NULL;
}

int hm_set(const HashMap hm, const void *key, const void *value)
{
    size_t hash    = hm->hash_func(key, hm->key_size) % hm->capacity;
    Bucket *bucket = get_bucket(hm, hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->payload, key, hm->key_size) == 0)
        {
            memcpy(bucket->payload + hm->key_size, value, hm->value_size);
            return 0;
        }
        hash   = (hash + 1) % hm->capacity;
        bucket = get_bucket(hm, hash);
    }

    return 1;
}

int hm_put(const HashMap hm, const void *key, const void *value)
{
    if (calc_load_fac(hm) > LOAD_FACTOR)
        if (hm_resize(hm) != 0)
            return 1;

    size_t hash             = hm->hash_func(key, hm->key_size) % hm->capacity;
    const size_t start_hash = hash;
    Bucket *bucket          = get_bucket(hm, hash);

    while (bucket->status != TOMBSTONE && memcmp(bucket->payload, key, hm->key_size) != 0)
    {
        hash   = (hash + 1) % hm->capacity;
        bucket = get_bucket(hm, hash);
        if (bucket == get_bucket(hm, start_hash))
        {
            return 1;
        }
    }

    if (bucket->status == ACTIVE && memcmp(bucket->payload, key, hm->key_size) == 0)
    {
        memcpy(bucket->payload + hm->key_size, value, hm->value_size);

        return 0;
    }

    bucket->status = ACTIVE;
    bucket->hash   = hash;
    memcpy(bucket->payload, key, hm->key_size);
    memcpy(bucket->payload + hm->key_size, value, hm->value_size);
    hm->size++;

    return 0;
}

bool hm_contains(const HashMap hm, const void *key)
{
    size_t hash             = hm->hash_func(key, hm->key_size) % hm->capacity;
    const size_t start_hash = hash;
    Bucket *bucket          = get_bucket(hm, hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->payload, key, hm->key_size) == 0)
        {
            return true;
        }
        hash   = (hash + 1) % hm->capacity;
        bucket = get_bucket(hm, hash);
        if (bucket == get_bucket(hm, start_hash))
        {
            break;
        }
    }

    return false;
}

size_t hm_size(const HashMap hm)
{
    return hm->size;
}

int hm_remove(const HashMap hm, const void *key)
{
    const size_t init_hash = hm->hash_func(key, hm->key_size) % hm->capacity;
    size_t current_hash    = init_hash;
    Bucket *bucket         = get_bucket(hm, current_hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->payload, key, hm->key_size) == 0)
        {
            bucket->status = TOMBSTONE;
            hm->size--;

            size_t next_hash    = (current_hash + 1) % hm->capacity;
            Bucket *next_bucket = get_bucket(hm, next_hash);

            while (next_bucket->status != TOMBSTONE)
            {
                const size_t next_original_hash = hm->hash_func(next_bucket->payload, hm->key_size) % hm->capacity;

                if ((next_original_hash <= current_hash && current_hash < next_hash) ||
                    (next_hash < next_original_hash && next_original_hash <= current_hash))
                {
                    memcpy(bucket, next_bucket, hm->bucket_size);

                    next_bucket->status = TOMBSTONE;

                    current_hash = next_hash;
                    bucket       = get_bucket(hm, current_hash);
                }

                next_hash   = (next_hash + 1) % hm->capacity;
                next_bucket = get_bucket(hm, next_hash);
            }

            return 0;
        }

        current_hash = (current_hash + 1) % hm->capacity;
        bucket       = get_bucket(hm, current_hash);

        if (current_hash == init_hash)
        {
            break;
        }
    }

    return 1;
}
