#pragma once
#include "Generals.h"
#include "CatalogManager.h"
#include "IndexManager.h"
#include "RecordManager.h"
#include "Exception.h"
#include <vector>
#include <list>
#include <ctime>
using namespace std;

/*API模块负责接收Interpreter的指令输入，调用其他模块的接口并统一捕获错误信息、返回信息到Intepreter，最终传递到主函数*/
class API
{

public:
	API();
	~API();
	static API* getInstance();
	QueryResult CreateTable(Table& table);
	QueryResult CreateIndex(const string& indexName, const string& tableName, const string& attributeName);
	QueryResult DropTable(const string& tableName);
	QueryResult DropIndex(const string indexName,const string& tableName,const string& attributeName);
	QueryResult InsertValuesInto(const string& tableName,const vector<string>& values);
	QueryResult Select(const list<string> attributes,const string& tableName, const list<Expression>& exprs);
	//QueryResult Select(const string& tableName, const list<Expression>& exprs);
	QueryResult DeleteFromTable(const string& tableName);
	QueryResult DeleteFromTableWhere(const string& tableName, const list<Expression>& exprs);


private:
	CatalogManager* p_catalogManager;
	IndexManager* p_indexManager;
	RecordManager* p_recordManager;
	OP OPERATOR_to_OP(OPERATOR o);
	Condition expr_to_Condition(const Expression& expr);



};







