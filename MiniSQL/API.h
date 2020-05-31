#pragma once
#include "Generals.h"
#include "CatalogManager.h"
#include "IndexManager.h"
#include "RecordManager.h"
#include <vector>
#include <list>
using namespace std;

/*API模块负责接收Interpreter的指令输入，调用其他模块的接口并统一捕获错误信息、返回信息到Intepreter，最终传递到主函数*/
class API
{

public:
	API();
	~API();
	void Init(CatalogManager* p_catalogManager, IndexManager* p_indexManager, RecordManager* p_recordManager);
	QueryResult CreateTable(const Table& table);
	QueryResult CreateDatabase(const string& databaseName);
	QueryResult CreateIndex(const string& tableName, const string& indexName, const string& attributeName);
	QueryResult DropTable(const string& tableName);
	QueryResult DropDatabase(const string& databaseName);
	QueryResult DropIndex(const string indexName,const string& tableName,const string& attributeName);
	QueryResult InsertValuesInto(const string& tableName,const vector<string>& values);
	QueryResult Select(const list<string> attributes,const string& tableName);
	QueryResult DeleteFromTable();
	//QueryResult Update();

private:
	CatalogManager* p_catalogManager;
	IndexManager* p_indexManager;
	RecordManager* p_recordManager;



};







