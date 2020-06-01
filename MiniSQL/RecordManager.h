#pragma once
#include "Generals.h"
#include "BufferManager.h"
#include <list>
#include <vector>
#include <map>

enum OP {
	Equal,
	NotEqual,
	GreaterThan,
	LessThan,
	GreaterEqual,
	LessEqual
};

//目前只考虑简单的 （属性 op 值），有时间再加
class Condition {
public:
	Condition(string attributeName, OP op, string value);
	virtual ~Condition();
	string getAttributeName();
	OP getOP();
	string getValue();
private:
	string attributeName;
	OP op;
	string value;
};

class RecordManager
{
public:
	RecordManager();
	virtual ~RecordManager();
	const QueryResult& createTable(string tableName, const vector<Attribute>& attributes);
	const QueryResult& dropTable(string tableName);
	const QueryResult& insertValues(string tableName, const vector<string>& values);//先只考虑全值插入，也就是values的个数等于属性个数
	const QueryResult& deleteValues(string tableName, const vector<Condition>& conditions);
	const QueryResult& selectValues(string tableName, const vector<Condition>& conditions);

private:
	BufferManager* bufferManager;
	map<string, Table*> tableDictionary;
};
