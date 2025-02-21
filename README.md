# AlCo - Allocator collection

## Overview

> **Disclaimer:**  
> The author is not liable for any malfunctions that may occur while using the provided allocators.

AlCo - collection of simple memory allocation algorithms.  
Common API for allocators:
- `init` - initialize new allocator object
- `alloc` - return allocated memory
- `free` - free memory by pointer
- `free_all` - reset allocator and all getted memory

## Table

- [Arena](#arena)
- [Pool](#pool)

### Arena

Variable-length allocation without random-order free.

Allocation:
Return cuurent value of `head` and after add size of allocated memory. When free all arena, assing `head` value of `base`.

#### Start state
```
base/head                                           tail
V                                                   V
+---+---+---+---+---+---+---+---+---+---+---+---+---+
|                    free memory                    |
+---+---+---+---+---+---+---+---+---+---+---+---+---+
```

#### Allocate one `int`:
``` c
int* i = (int*)alco_arena_alloc(/* pointer to arena object */, sizeof *i);
```
```
base            head                                tail
V               V                                   V
+---+---+---+---+---+---+---+---+---+---+---+---+---+
|       i       |            free memory            |
+---+---+---+---+---+---+---+---+---+---+---+---+---+
```

#### Allocate three `short`:
``` c
short* s3 = (short*)alco_arena_alloc(/* pointer to arena object */, sizeof *s3 * 3);
```
```
base                                    head        tail
V                                       V           V
+---+---+---+---+---+---+---+---+---+---+---+---+---+
|       i       | s3[0]   s3[1]   s3[2] | free  mem |
+---+---+---+---+---+---+---+---+---+---+---+---+---+
```

### Pool

Fixed-size block allocation without guarantee contiguous memory. [Free list](https://en.wikipedia.org/wiki/Free_list) - data structure for store places of free blocks.

Allocation:
Return current value of `head` and after assing value of field `next`. When free pointer, create a new 'header' structure with `next` equal current value of `head` and assing `head` value of getted pointer.  
As example use pool for `int` type.

#### Chunk structure:
```
|<---------- chunk_size --------->|
|  ptr to next chunk  |  padding  |
```

`chunk_size` = max(sizeof pointer, sizeof type)

#### Start state:
```
head/chunks
| .---------. .---------. .---------. .---> NULL
V |         V |         V |         V |
+---+-------+---+-------+---+-------+---+-------+
| n |  pad  | n |  pad  | n |  pad  | n |  pad  |
+---+-------+---+-------+---+-------+---+-------+
```

#### Allocate one `int`:
``` c
int* a = (int*)alco_pool_alloc(/* pointer to pool object */);
```
```
chunks      head
|           | .---------. .---------. .---> NULL
V           V |         V |         V |
+-----------+---+-------+---+-------+---+-------+
| alloc mem | n |  pad  | n |  pad  | n |  pad  |
+-----------+---+-------+---+-------+---+-------+
```

#### Allocate again:
``` c
int* b = (int*)alco_pool_alloc(/* pointer to pool object */);
```
```
chunks                  head
|                       | .---------. .---> NULL
V                       V |         V |
+-----------------------+---+-------+---+-------+
|   allocated  memory   | n |  pad  | n |  pad  |
+-----------------------+---+-------+---+-------+
```

#### Free pointer `a`:
``` c
alco_pool_free(/* pointer to pool object */, a);
```
```
chunks/a                head
|                       | .---------. .---> NULL
V                       V |         V |
+-----------------------+---+-------+---+-------+
|   allocated  memory   | n |  pad  | n |  pad  |
+-----------------------+---+-------+---+-------+
```
```
chunks/head
| .---------------------. .---------. .---> NULL
V |                     V |         V |
+---+-------+-----------+---+-------+---+-------+
| n |  pad  | alloc mem | n |  pad  | n |  pad  |
+---+-------+-----------+---+-------+---+-------+
```

#### Free pointer `b`:
``` c
alco_pool_free(/* pointer to pool object */, b);
```
```
chunks/head
| .---------------------. .---------. .---> NULL
V |                     V |         V |
+---+-------+-----------+---+-------+---+-------+
| n |  pad  | alloc mem | n |  pad  | n |  pad  |
+---+-------+-----------+---+-------+---+-------+
            ^
            |
            b
```
```
chunks
| ,---------------------. ,---------. ,---> NULL
V |                     V |         V |
+---+-------+---+-------+---+-------+---+-------+
| n |  pad  | n |  pad  | n |  pad  | n |  pad  |
+---+-------+---+-------+---+-------+---+-------+
^           ^ |
'-------------'
            |
            head
```