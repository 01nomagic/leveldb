#include <iostream>
#include "leveldb/db.h"

using namespace std;

int main() {
    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "./temp_my_db", &db);

    if (status.ok()) {
        cout << "数据库打开成功" << endl;
    }

    return 0;
}