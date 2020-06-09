#pragma once
#include <map>
#include <string>
#include <list>
#include "BPTree/BPTree.h"
#include "Generals.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
using namespace std;

#define BLOCKSIZE 4096
#define HEADER_BLOCK_OFFSET 1*BLOCKSIZE//record offset
typedef int ADDRESS;

class IndexManager {
public:
	IndexManager();
	IndexManager(list<string> indexName);
	~IndexManager();
	const QueryResult& createIndex(const string &indexName, Attribute &attribute, const int &recordLength, const string &fileName);/*create Index of a relation Cautious, if you create an unique index on an integer or float, do not use IM to process where A<xx A>xx query*/
	const QueryResult& dropIndex(const string &indexName); /*delet/drop index indexfile and index in this function*/
	const QueryResult& deleteValues(const string &indexName, const list<string> &indexList, vector<Condition> conditions, const string &fileName, const int &recordLength);/*delete values specified by expression*/
	const QueryResult& deleteValues(const string &primaryIndexName, const list<string> &primaryKeyValues, const list<string> &indexList, const string &fileName, const int recordLength);/*delete values specified by list of key*/
	const QueryResult& deleteValuesAll(const string &indexName);/*delete all the values*/
	const QueryResult& selectValues(const string &indexName, const list<string> &attributes, Table& table, vector<Condition> conditions, const string &fileName);/*select values specified by expressions*/
	const QueryResult& insertValues(const string &indexName, string indexKey, const ADDRESS &recordOffset);/*insert indexkey to bplus tree after insertion with RM*/
	bool keyExists(const string &indexName, string keyValue, const int length);/*find if a key exists in index specified by indexName*/
	static IndexManager* getIndexManagerPtr(list<string> indexName) { static IndexManager im(indexName); return &im; }/*get an instance of IndexManager*/

private:
	map<string, BPTree*> indexDictionary;
	BufferManager* bufferManager;
	CatalogManager* catalogManager;


	void CreateFromFile(string name);
	void SaveToFile(string name);
	ADDRESS getEndOffset(const string &fileName);
	ADDRESS getNextToEndOffset(const string &fileName, const int &recordLength);
	void deleteRecordFromFile(const string &keyIndexName, const list<string> &indexList, const string &fileName, const ADDRESS &recordOffset, const int &recordLength);
	void renewEndOffset(const string &fileName, const int &recordLength);
};
