# NCKU CSIE Computer Organization HW6 - cache simulator

## Homework Specification
* Width of memory (line size) : 32bits
*  Unit of memory : byte.
*  Cache size : Read from test cases. With variable cache size from up to 256KB.
* Block size : Read from test cases. From 16B to 256B.
* Associativity : Read from test cases. Various associativity from direct-mapped to fully associative.
* Replaced Algorithm : Read from test cases. Will be FIFO or LRU.
* All cache is initialized 0.

## Input file format
* We will give .txt file which contains the test cases. The file contains a
trace of memory accesses executed from some benchmark program.
* The 1st line specifies the cache size (kb).
* The 2nd line specifies the block size (byte).
* The 3rd line specifies the associativity. 0 represents direct-mapped, 1
represents four-way set associative, 2 represents fully associative.
The 4th line specifies the Replace algorithm. 0 represents FIFO, 1
represents LRU and 2 represents your policy.
* The rest of the test case is a trace of memory accesses executed from
some benchmark program.

## Sample Input: trace1.txt

```
1024 // cache size (KB)
16 // block size (Byte)
0 // associativity
0 // FIFO=0 , LRU=1, Your Policy=2
0xbfa437cc // No. 1
0xbfa437c8 // No. 2 ……
0xbfa437c4
0xbfa437c0
0xbfa437bc
0xbfa437b8
0xb80437b8
0xb8043794
0xb80437c8
0xb80437cc
```

## Output of your cache simulator
Take trace1 for example, output to file named trace1.out, and output
the following information (you need to output which tag is the victim
for each request, if there is not any victim then output -1).

## Sample output: trace1.out

```
Sample Output
-1
-1
-1
-1
-1
-1
3066
-1
3066
-1
```

## Run and verify
```
$ make verify REQ_FILE=<trace.txt> VIC_FILE=<trace.out>
```