leveldb Log format（leveldb文件格式）
==================
> Tin注：所有的文件都是以下面这种方式存储的，不只是log文件。所以标题我翻译为文件格式，而不是日志文件格式。

The log file contents are a sequence of 32KB blocks.  The only exception is that
the tail of the file may contain a partial block.

文件内容由一个个32KB大小（32768字节）的block组成。唯一的例外是文件结尾可能是一个小于32KB的块。

Each block consists of a sequence of records:

每个块由一个个record组成：

    block := record* trailer?
    record :=
      checksum: uint32     // crc32c of type and data[] ; little-endian
      length: uint16       // little-endian
      type: uint8          // One of FULL, FIRST, MIDDLE, LAST
      data: uint8[length]

A record never starts within the last six bytes of a block (since it won't fit).
Any leftover bytes here form the trailer, which must consist entirely of zero
bytes and must be skipped by readers.

一个record永远不会从一个block的最后6个字节开始（因为太小无法存储）。这些剩余的字符将被0值填充，并且会被reader忽略。

Aside: if exactly seven bytes are left in the current block, and a new non-zero
length record is added, the writer must emit a FIRST record (which contains zero
bytes of user data) to fill up the trailing seven bytes of the block and then
emit all of the user data in subsequent blocks.

注意：如果block只剩余7个字节，这时如果添加一个非空的record，writer首先将用一个FIRST类型的record（包含0字节的用户数据）填充block剩余的那七个字节，然后将用户数据写入接下来的几个block。

More types may be added in the future.  Some Readers may skip record types they
do not understand, others may report that some data was skipped.

未来可能加入更多的record类型。一些reader可能会忽略无法理解的record，另一些reader可能会提示某些数据被忽略了。

    FULL == 1
    FIRST == 2
    MIDDLE == 3
    LAST == 4

The FULL record contains the contents of an entire user record.

FULL类型的record包含了用户record的全部内容。

FIRST, MIDDLE, LAST are types used for user records that have been split into
multiple fragments (typically because of block boundaries).  FIRST is the type
of the first fragment of a user record, LAST is the type of the last fragment of
a user record, and MIDDLE is the type of all interior fragments of a user
record.

FIRST、MIDDLE、LAST三种类型的record用于表示用户record被分成了多个片段存储（由于block大小的限制导致的）。FIRST record是用户record的第一个片段，LAST record是用户record的最后一个片段，MIDDLE record是用户record的中间那些片段。

Example: consider a sequence of user records:

示例：比如下面几个用户record：

    A: length 1000
    B: length 97270
    C: length 8000

**A** will be stored as a FULL record in the first block.

A将作为一个FULL record存储在第一个block。

**B** will be split into three fragments: first fragment occupies the rest of
the first block, second fragment occupies the entirety of the second block, and
the third fragment occupies a prefix of the third block.  This will leave six
bytes free in the third block, which will be left empty as the trailer.

B将分成三个片段：第一个片段是第一个block的剩余空间，第二个片段占满第二个block，第三个片段占了第三个block的前面部分。第三个block将只剩余6个字节，作为trailer保持为空。

**C** will be stored as a FULL record in the fourth block.

C将作为一个FULL record存储在第四个block

```
Tin解释：

1 block == 32768bytes

block 1:
	// A FULL record
  0-6: 				7bytes			A header
  7-1006: 		1000bytes		A data
  // B FIRST RECORD
  1007-1013: 	7bytes			B header1
  1014-32767: 31754bytes	B data1 

block 2:
	// B MIDDLE record
	0-6:				7bytes			B header2
	7-32767:		32761bytes	B data2

block 3:
	// B LAST record
	0-6:				7bytes			B header3
	7-32761			32755bytes	B data3
	// 剩余的6个字节，不足以作为一个record，用0填充
	32762-32767	6bytes			as trailer

block 4:
	// C FULL record
	0-6:				7bytes			C header
	7-8006			8000bytes		C data
	// 留给后面的user record使用
	8006-32767	24761bytes  empty
```





----

## Some benefits over the recordio format（上述格式的一些优点）:

1. We do not need any heuristics for resyncing - just go to next block boundary
   and scan.  If there is a corruption, skip to the next block.  As a
   side-benefit, we do not get confused when part of the contents of one log
   file are embedded as a record inside another log file.

   我们不需要任何启发式的读取方式，只需要移动到下一个block然后扫描即可。如果某个block格式错误，跳过该block。作为额外的优点，当一个文件中的内容作为用户record写入到另外一个文件时，也可以清晰的读取，不会造成干扰。
   
2. Splitting at approximate boundaries (e.g., for mapreduce) is simple: find the
   next block boundary and skip records until we hit a FULL or FIRST record.

   对record进行近似地分割（比如用于map和reduce）比较容易：找到下一个block，然后一直跳过record，直到找到一个FULL或FIRST类型的record。

3. We do not need extra buffering for large records.

   对于巨大的record不需要额外的缓冲区。

## Some downsides compared to recordio format（上述格式的一些缺点）:

1. No packing of tiny records.  This could be fixed by adding a new record type,
   so it is a shortcoming of the current implementation, not necessarily the
   format.

   对于非常小的record没有进行批量打包。这可以通过添加新的record类型解决，因此它是当前实现上的缺点，而不是上述格式的缺点。
   
2. No compression.  Again, this could be fixed by adding new record types.

   没有压缩。同样的，这也可以通过添加新的record类型解决。
