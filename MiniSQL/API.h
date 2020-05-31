#pragma once
#include "Generals.h"
#include "CatalogManager.h"
#include "IndexManager.h"
#include "RecordManager.h"
#include <vector>
#include <list>
using namespace std;

/*APIģ�鸺�����Interpreter��ָ�����룬��������ģ��Ľӿڲ�ͳһ���������Ϣ��������Ϣ��Intepreter�����մ��ݵ�������*/
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







