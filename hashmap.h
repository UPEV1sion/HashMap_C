//
// Created by Emanuel on 02.09.2024.
//

#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct HashMap *HashMap;
typedef size_t (*hash)(const void *key, size_t key_size);

/**
 * @brief creates a HashMap
 * @param hm_capacity the initial capacity of the hashmap
 * @param key_size the sizeof value of the key
 * @param value_size the sizeof value of the value
 * @return HashMap
 */
HashMap hm_create(size_t hm_capacity, size_t key_size, size_t value_size);

/**
 * @brief creates a HashMap with a custom hash function
 * @param hm_capacity the initial capacity of the hashmap
 * @param key_size the sizeof value of the key
 * @param value_size the sizeof value of the value
 * @param hash_func the custom hash function
 * @return HashMap
 */
HashMap hm_create_ch(size_t hm_capacity, size_t key_size, size_t value_size, hash hash_func);

/**
 * @brief destroys the hashmap
 * @param hm the hashmap
 * @return success status
 */
int hm_destroy(HashMap hm);

/**
 * @brief gets the value at the specified key
 * @param hm the hashmap
 * @param key the key
 * @return the value at the key
 */
void *hm_get(HashMap hm, const void *key);

/**
 * @brief sets the value at the specified key
 * @param hm the hashmap
 * @param key the key
 * @param value the value
 * @return success status
 */
int hm_set(HashMap hm, const void *key, const void *value);

/**
 * @brief adds an item to the hashmap
 * @param hm the hashmap
 * @param key the key
 * @param value the value
 * @return success status
 */
int hm_put(HashMap hm, const void *key, const void *value);

/**
 * @brief removes an item from the hashmap
 * @param hm the hashmap
 * @param key the key
 * @return success status
 */
int hm_remove(HashMap hm, const void *key);

#endif //HASHMAP_H
