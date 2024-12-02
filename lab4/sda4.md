
    Text/Code: Lowest addresses
    Global Variables: Medium-low addresses
    Heap: Medium addresses
    Stack: Highest addresses (often starting with 0x7...)
    Sequential Patterns: Variables in the same segment will have similar address patterns


Let me break this down by explaining each segment and their ending addresses in detail:

1. **etext (End of Text Segment)**
- The text segment contains the actual program code (instructions)
- This is typically read-only memory
- Contains compiled machine code of your program
- `etext` marks the first memory address after this segment ends
- Example: If your program code occupies addresses 0x1000 to 0x2000, then etext would be 0x2001

2. **edata (End of Data Segment)**
- The data segment contains initialized global and static variables
- These are variables that have a defined value when the program starts
- `edata` marks the first address after this initialized data segment
- Example:
```c
int globalVar = 42;        // Goes in data segment
static int staticVar = 10; // Goes in data segment
```
- If data segment occupies 0x2001 to 0x3000, then edata would be 0x3001

3. **end (End of BSS Segment)**
- BSS (Block Started by Symbol) contains uninitialized global and static variables
- These variables are automatically initialized to zero/null
- `end` marks the first address after this segment
- Example:
```c
int globalUninitialized;        // Goes in BSS segment
static int staticUninitialized; // Goes in BSS segment
```
- If BSS segment occupies 0x3001 to 0x4000, then end would be 0x4001

Visual Representation:
```
Memory Layout:
Low Address
+------------------+
| Text Segment     |
| (Program Code)   |
+------------------+ <- etext
| Data Segment     |
| (Initialized)    |
+------------------+ <- edata
| BSS Segment      |
| (Uninitialized)  |
+------------------+ <- end
| Heap            |
| (Dynamic Memory) |
+------------------+
| ...             |
+------------------+
| Stack           |
+------------------+
High Address
```

These addresses are useful for:
- Memory management
- Debugging
- Understanding program layout
- Memory optimization
- Security checks

You can access these addresses in C using external variables:
```c
extern char etext, edata, end;
```