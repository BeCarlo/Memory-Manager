# 📦 Memory Manager

A custom memory manager designed to efficiently handle both small and large object allocations.  
The implementation takes inspiration from the techniques of Alexandrescu, with some additional heuristics for performance improvements.

---

## 🧩 Small Object Allocation

For small objects, the system uses a strategy similar to Alexandrescu’s small object allocator, with the addition of some heuristics:

1. **Empty Chunk List**  
   A list of pointers to empty chunks is maintained.  
   This allows **O(1)** insertion time when an empty chunk is available.

2. **Chunk Tree Structure**  
   All chunks are stored in a **red–black tree**, allowing pointer lookups in **O(log n)** time.

This approach combines fast allocation for frequent small objects with efficient search and reuse of freed memory.

---

## 🏗️ Big Object Allocation

For larger objects, the allocator uses:

- A **free list** to manage available chunks.
- A **red–black tree** to efficiently handle and search for large memory blocks.

This provides a good balance between memory fragmentation control and allocation speed for larger objects.

---

## Result

(result.png)