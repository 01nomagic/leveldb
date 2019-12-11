## 静态接口：`leveldb::DB::Open(...)`

`include/leveldb/db.h`中定义了DB类
`db/db_impl.h`定义了DBImpl类，继承了DB类
`db/db_impl.cc`实现了DBImpl类和DB类
`SanitizeOptions`对传入的options进行检查和校准

`memtable.h, memtable.cc`: memtable定义，Add和Get方法

```
leveldb::DB::Open
  
```

暴露在外面的类型：`Options, DB, Status, WriteBatch, WriteOptions, ReadOptions, Iterator`

MANIFEST-000001:

