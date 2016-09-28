#define GEN7_MOCS (struct GEN7_MEMORY_OBJECT_CONTROL_STATE) {  \
   .GraphicsDataTypeGFDT                        = 0,           \
   .LLCCacheabilityControlLLCCC                 = 0,           \
   .L3CacheabilityControlL3CC                   = 1,           \
}

#define GEN75_MOCS (struct GEN75_MEMORY_OBJECT_CONTROL_STATE) {  \
   .LLCeLLCCacheabilityControlLLCCC             = 0,           \
   .L3CacheabilityControlL3CC                   = 1,           \
}

#define GEN8_MOCS (struct GEN8_MEMORY_OBJECT_CONTROL_STATE) {  \
      .MemoryTypeLLCeLLCCacheabilityControl = WB,              \
      .TargetCache = L3DefertoPATforLLCeLLCselection,          \
      .AgeforQUADLRU = 0                                       \
   }

/* Skylake: MOCS is now an index into an array of 62 different caching
 * configurations programmed by the kernel.
 */

#define GEN9_MOCS (struct GEN9_MEMORY_OBJECT_CONTROL_STATE) {  \
      /* TC=LLC/eLLC, LeCC=WB, LRUM=3, L3CC=WB */              \
      .IndextoMOCSTables                           = 2         \
   }

#define GEN9_MOCS_PTE {                                 \
      /* TC=LLC/eLLC, LeCC=WB, LRUM=3, L3CC=WB */       \
      .IndextoMOCSTables                           = 1  \
   }
