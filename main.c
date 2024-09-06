#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashmap.h"

//
// Created by Emanuel on 05.09.2024.
//


typedef struct
{
    char *key;
} CustomKey;

char* generate_random_key(int length)
{
    constexpr char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char *key                = malloc(length + 1);
    if (key)
    {
        for (int i = 0; i < length; i++)
        {
            key[i] = charset[rand() % (sizeof(charset) - 1)];
        }
        key[length] = 0;
    }
    return key;
}

void benchmark_hashmap(int num_operations, int key_length)
{
    HashMap map = hm_create(num_operations, sizeof(CustomKey), sizeof(int), NULL);

    CustomKey *keys = malloc(num_operations * sizeof(CustomKey));
    int *values     = malloc(num_operations * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < num_operations; i++)
    {
        keys[i].key = generate_random_key(key_length);
        values[i]   = rand();
    }

    clock_t start, end;
    double time_taken;

    start = clock();
    for (int i = 0; i < num_operations; i++)
    {
        hm_put(map, &keys[i], &values[i]);
    }
    end        = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

	for (int i = 0; i < num_operations / 2; i++)
    {
        hm_remove(map, &keys[i]);
    }
	
    printf("Insertion time for %d elements: %.6f seconds\n", num_operations, time_taken);

    int null_count = 0;
    start          = clock();
    for (int i = 0; i < num_operations; i++)
    {
        int *value = hm_get(map, &keys[i]);
        if (value == NULL) null_count++;
    }
    end        = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Retrieval time for %d elements: %.6f seconds\n", num_operations, time_taken);
    printf("Number of null values: %d\n", null_count);

    start = clock();
    for (int i = 0; i < num_operations; i++)
    {
        int new_value = rand();
        hm_set(map, &keys[i], &new_value);
    }
    end        = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Update time for %d elements: %.6f seconds\n", num_operations, time_taken);


    start = clock();
    for (int i = 0; i < num_operations; i++)
    {
        hm_remove(map, &keys[i]);
    }
    end        = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Deletion time for %d elements: %.6f seconds\n", num_operations, time_taken);

    for (int i = 0; i < num_operations; i++)
    {
    free(keys[i].key);
    }
    free(keys);
    free(values);
    hm_destroy(map);
}

int main()
{
    int num_operations = 100000;
    int key_length     = 16;

    printf("Benchmarking HashMap with %d operations...\n", num_operations);
    benchmark_hashmap(num_operations, key_length);
   
    return 0;
}
