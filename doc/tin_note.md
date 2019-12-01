# leveldb阅读笔记

### 初始化全新的数据库

如果CURRENT文件都不存在，则初始化一个全新的数据库，而不是恢复一个已有的数据库

```
DB::Open
	new DBImpl
		创建versions_（VersionSet），含有一个Version
	DBImpl::Recover
		创建名称为dbname的目录
		DBImpl::NewDB
			创建新的VersionSet，设置比较方法的名称、LogNumber=0、LastSequence=0
			创建MANIFEST-000001文件，将VersionSet编码成字符串内容作为record写入到MANIFEST文件
			创建CURRENT文件
		VersionSet::Recover
			使用当前的Version（空的）创建新的Builder
			从MANIFEST中读取VersionEdit内容，解码后apply到builder中，生成一个新的Version，并添加到双向链表中
			
```

