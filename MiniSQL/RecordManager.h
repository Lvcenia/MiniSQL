#pragma once
#include "Generals.h"
#include "BufferManager.h"
#include "IndexManager.h"
#include <list>
#include <vector>
#include <map>

//使用CatalogManager建表
//结果返回后一定要更新TableInfo.recordLength
class RecordManager
{
public:
	RecordManager();
	virtual ~RecordManager();
	const QueryResult& dropTable(const Table& tableInfo);
	const QueryResult& insertValues(const Table& tableInfo, const vector<string>& values);//先只考虑全值插入，意思values的个数等于属性个数
	const QueryResult& deleteValues(const Table& tableInfo, const vector<Condition>& conditions);
	const QueryResult& selectValues(const Table& tableInfo, const vector<Condition>& conditions);
	const QueryResult& selectValues(const vector<string>& attributes, const Table& tableInfo, const vector<Condition>& conditions);

private:
	BufferManager* bufferManager;
};
