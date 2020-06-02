#pragma once
#include <map>
#include <string>
#include <list>
#include "BPTree/BPTree.h"
#include "Generals.h"
#include "BufferManager.h"
using namespace std;

#define BLOCKSIZE 4096
#define INT_STRING_SIZE 10
#define FLOAT_INTEGER_SIZE 10 //Integer part length of a float number
#define FLOAT_DECIMAL_SIZE 9//decimal part length of a float number
#define HEADER_BLOCK_OFFSET 1*BLOCKSIZE//record offset

typedef unsigned char BYTE;

struct INDEXFILEHEADER{
	BYTE indexName[32];
	Type type;
	int attributeLength;
	int offsetInRecord;
	int fanOut;
	int elementCount;
};

class IndexManager {
public:
	static IndexManager* getIndexManager(vector<string> indexNames);//使用该函数获取IndexManager对象

	IndexManager();
	IndexManager(list<string> indexNames);
	virtual ~IndexManager();
	const QueryResult& createIndex(string indexName, const Attribute& attributeInfo, const Table& tableInfo);
	const QueryResult& dropIndex(string name);

private:
	map<string, BPTree*> indexDictionary;
	BufferManager* bufferManager;

	void CreateFromFile(string name);
	void SaveToFile(string name);
	int GetEndOffset(string fileName);
	int GetNextToEndOffset(string fileName, int recordLength);
};
