#include <iostream>
#include "leveldb/db.h"

using namespace std;

int main() {
    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "./temp_my_db", &db);

    if (status.ok()) {
      db->Put(leveldb::WriteOptions(), leveldb::Slice("xxx"), "jjjjjj");
      db->Put(leveldb::WriteOptions(), leveldb::Slice("xxx2"), "jjjjjj");
      db->Put(leveldb::WriteOptions(), leveldb::Slice("xxx3"), "jjjjjj");
      cout << "OK" << endl;
    }

    return 0;
}