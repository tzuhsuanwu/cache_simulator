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

## Data structure

### fourawy_FIFO

使用map<string, deque<string>>結構儲存，map的key代表的是index，value的deque儲存一個set，在此例fourway中最多會有4個element，每個element中儲存的就是該筆資料的tag，當輸入一個index時會先檢查map中有沒有該index，若無則在該index新增一個deque，並新增tag至deque的尾端（此時亦為首端），並輸出-1；若有找到該index，對其deque搜尋tag，若在deque中找到該tag則為hit，輸出-1，若沒有找到則先判斷deque是否已滿，若未滿則直接將tag新增至deque尾端，若已滿則將首端（即first in element）釋出，然後將tag新增至尾端，以達成FIFO的置換策略。

### fourway_LRU

結構與搜尋方式大致與fourway_FIFO一致，不同的地方在於，此方法若於deque搜尋時找到tag（即hit），則會將該tag移至deque尾端，這樣在deque內的資料從首端到尾端就是依照近期被訪問的順序排序，首端為近期最不常用到的element，若須置換時只要將首端釋出，再將新的tag新增至尾端，即達成LRU的置換策略。

### fourway_LFU

使用map<string, deque<vector<string>>>結構儲存，map的key代表的是index，value的deque儲存一個set，在此例fourway中最多會有4個element，每個element中儲存一個vector，每個vector會有兩筆資料，第一筆儲存tag，第二筆儲存一個counter，這個counter會記錄該tag總共被使用的次數，每次新增的tag都有一個初始counter，值為1，當hit發生時，hit的tag其counter就會加1，在需要置換的時候，則搜尋set內所有tag的counter，將counter數最少的（即使用次數最少的）釋出，並新增新的tag，已達成LFU的置換策略。

### fully_FIFO_cache

使用較為簡略的deque<string>結構儲存，由於fully不會有index，所以只需要使用一個deque來儲存tag即可，當要置換時只需要釋出首端的element，在新增tag至尾端即可達成FIFO的置換策略。

### fully_LRU_cache;

結構同fully_FIFO，置換方法如同上述fourway_LRU所提到的，hit發生時將該tag至於尾端，須置換時只要將首端釋出，再將新的tag新增至尾端，即達成LRU的置換策略。

### fully_LFU_cache

使用deque<vector<string>>儲存，結構與上述兩種fully相似，唯獨其內部存的是一個vector，同上述fourway_LFU所提到的，vector所儲存的資料為tag即counter，置換策略也與fourway_LFU提到的相同。


---

## LRU

在fourway及fully的LRU演算法都是一樣的，由於是使用deque來儲存，當deque未滿時，若hit則將該tag放置於尾端，若沒hit則直接新增tag至尾端；當deque滿時，若hit則將該tag放置於尾端，若沒hit則將首端element釋出，並新增tag至尾端。


---

## LFU

LFU(Least Frequently Used)，即最近最不常使用之置換策略，實際的做法為對每一筆資料都新增一個計數器，每當這筆資料被使用時，計數器就會加1，當需要置換時，只要搜尋set內最少使用(fourway)或是cache中最少使用(fully)的資料將其釋出即可。
