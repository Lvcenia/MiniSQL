#pragma once
#include "Generals.h"
#include "BufferManager.h"
#include "IndexManager.h"
#include <list>
#include <vector>
#include <map>
#define DeleteInfo "Deleted"

class RecordIterator
{
public:
	RecordIterator(int len, int tail) { nowOffset = BLOCKSIZE; recordLength = len; this->tail = tail; }
	RecordIterator(int off, int len, int tail) { nowOffset = off; recordLength = len; this->tail = tail; }
	void setTail(int tail) { this->tail = tail; }
	RecordIterator& next() {
		nowOffset += recordLength;
		if (nowOffset % BLOCKSIZE + recordLength > BLOCKSIZE)
			nowOffset = (nowOffset / BLOCKSIZE + 1) * BLOCKSIZE;
		return *this;
	}
	bool hasNext() {
		if ((nowOffset + recordLength) <= tail)
			return true;
		return false;
	}
	int value() { return nowOffset; }
private:
	int nowOffset;
	int recordLength;
	int tail;
};

//使用CatalogManager建表
//结果返回后一定要更新TableInfo.recordLength
class RecordManager
{
public:
	RecordManager();
	virtual ~RecordManager();
	QueryResult createTable(Table& tableInfo);
	QueryResult dropTable(const Table& tableInfo);
	ADDRESS insertValues(const Table& tableInfo, const vector<string>& values);//先只考虑全值插入，意思values的个数等于属性个数
	QueryResult deleteValues(const Table& tableInfo, const vector<Condition>& conditions);
	QueryResult selectValues(const Table& tableInfo, const vector<Condition>& conditions);
	QueryResult selectValues(const vector<string>& attributes, const Table& tableInfo, const vector<Condition>& conditions);
	static RecordManager* getRecordMangerPtr() { static RecordManager rm; return &rm; }
private:
	BufferManager* bufferManager;
};
