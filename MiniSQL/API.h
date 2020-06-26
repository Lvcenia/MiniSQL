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
	API();//构造函数和初始化
	~API();
	static API* getInstance();//单例的接口
	QueryResult CreateTable(Table& table);//创建表
	QueryResult CreateIndex(const string& indexName, const string& tableName, const string& attributeName);//创建索引
	QueryResult DropTable(const string& tableName);//删除表
	QueryResult DropIndex(const string indexName,const string& tableName,const string& attributeName);//删除索引
	QueryResult InsertValuesInto(const string& tableName,const vector<string>& values);//插入数据
	QueryResult Select(const list<string> attributes,const string& tableName, const list<Expression>& exprs);//选择
	QueryResult DeleteFromTable(const string& tableName);//全表删除
	QueryResult DeleteFromTableWhere(const string& tableName, const list<Expression>& exprs);//条件删除


private:
	CatalogManager* p_catalogManager;
	IndexManager* p_indexManager;
	RecordManager* p_recordManager;
	OP OPERATOR_to_OP(OPERATOR o);//两个操作符枚举的转换
	Condition expr_to_Condition(const Expression& expr);//Expression类到Condition类的转换

};







