# ADTS

### Strings
- The llvm::StringRef class maintains a reference to data, which doesn’t need to be null-terminated like traditional C/C++ strings. It essentially holds a pointer to a data block and the block’s size, making the object’s effective size 16 bytes.

- Another class used for string manipulation in LLVM is llvm::Twine, which is particularly useful when concatenating several objects into one

### Maps

- It’s noteworthy that these containers utilize a quadratically probed hash table mechanism. This method is effective for hash collision resolution because the cache isn’t recomputed during element lookups. This is crucial for performance-critical applications, such as compilers.

- Unlike many other data structures in LLVM, llvm::StringMap<> does not store a copy of the string key. Instead, it keeps a reference to the string data, so it’s crucial to ensure the string data outlives the map to prevent undefined behavior.