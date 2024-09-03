# HashMap implementation in C

A very lightweight, performant `HashMap` implementation in C.

> **_NOTE:_**  This `HashMap` implementation solely relies on a "Array of `Linked List` implementation. Testing with "treeifying" a node after a certain threshold yielded a negative performance impact from the extensive testing needed.

This document provides an overview and detailed description of the functions available in this `HashMap` implementation.

## Overview

This `HashMap` implementation provides a flexible hash map with functions to create, manipulate, and destroy hash maps. It supports both default and custom hash functions.

## Typedefs
```c
typedef struct HashMap *HashMap;
typedef size_t (*hash)(const void *key, size_t key_size);
```


## Function Overview
```c
HashMap hm_create(size_t hm_capacity, size_t key_size, size_t value_size);
HashMap hm_create_ch(size_t hm_capacity, size_t key_size, size_t value_size, hash hash_func);
int hm_destroy(HashMap hm);
void *hm_get(HashMap hm, const void *key);
int hm_set(HashMap hm, const void *key, const void *value);
int hm_put(HashMap hm, const void *key, const void *value);
size_t hm_size(HashMap hm);
int hm_remove(HashMap hm, const void *key);
```
## Function Reference

```c 
HashMap hm_create(size_t hm_capacity, size_t key_size, size_t value_size);
```

**Description**: Creates a new `HashMap` with a generic hash function.

- **Parameters**:
  - `hm_capacity`: The initial capacity of the `HashMap`.
  - `key_size`: The size of the key.
  - `value_size`: The size of the value.
  
- **Returns**: A new `HashMap`.

---

```c
HashMap hm_create_ch(size_t hm_capacity, size_t key_size, size_t value_size, hash hash_func);
```

**Description**: Creates a new `HashMap` with a custom hash function.

- **Parameters**:
  - `hm_capacity`: The initial capacity of the `HashMap`.
  - `key_size`: The size of the key.
  - `value_size`: The size of the value.
  - `hash_func`: A custom hash function to be used by the `HashMap`.
  
- **Returns**: A new `HashMap`.

---

```c
int hm_destroy(HashMap hm);
```

**Description**: Destroys the `HashMap`.

- **Parameters**:
  - `hm`: The `HashMap` to be destroyed.
  
- **Returns**: A success code.

---

```c
void *hm_get(HashMap hm, const void *key);
```

**Description**: Retrieves the value associated with the specified key.

- **Parameters**:
  - `hm`: The `HashMap`.
  - `key`: The key whose associated value is to be retrieved.
  
- **Returns**: A pointer to the value at the specified key, or `NULL` if the key is not found.

---

```c
int hm_set(HashMap hm, const void *key, const void *value);
```

**Description**: Updates the value associated with the specified key.

- **Parameters**:
  - `hm`: The `HashMap`.
  - `key`: The key whose value is to be updated.
  - `value`: The new value to be associated with the key.
  
- **Returns**: A success code.

---

```c
int hm_put(HashMap hm, const void *key, const void *value);
```

**Description**: Adds a new key-value pair to the `HashMap`.

- **Parameters**:
  - `hm`: The `HashMap`.
  - `key`: The key to be added.
  - `value`: The value to be associated with the key.
  
- **Returns**: A success code.

---

```c
size_t hm_size(HashMap hm);
```

**Description**: Returns the size of the HashMap.

- **Parameters**:
  - `hm`: The `HashMap`.
  
- **Returns**: The size.
  
---

```c
int hm_remove(HashMap hm, const void *key);
```

**Description**: Removes the key-value pair associated with the specified key.

- **Parameters**:
  - `hm`: The `HashMap`.
  - `key`: The key of the key-value pair to be removed.
  
- **Returns**: A success code.

---

## Error Codes

- `0`: Success.
- Non-zero values indicate errors.

---

## Example

Here is a basic example demonstrating how to use the `HashMap` functions:

```c
#include "hashmap.h"

int main() {
    // Create a HashMap
    HashMap map = hm_create(10, sizeof(int), sizeof(int));
    
    // Add a key-value pair
    int key = 1;
    int value = 100;
    hm_put(map, &key, &value);
    
    // Retrieve the value
    int *retrieved_value = hm_get(map, &key);
    
    // Destroy the HashMap
    hm_destroy(map);
    
    return 0;
}
