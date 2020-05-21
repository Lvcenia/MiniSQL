#pragma once
/*包含一些全局范围内的数据类，比如查询结果*/
#include <string>
#include<cstring>
#include<vector>
#include<cstdio>

using namespace std;

enum QueryState
{
	Success,
	Fail
};

class QueryResult
{
public:
	QueryResult(const string& cont);
	~QueryResult();
	QueryState state;
	//结果的内容
	string content;
	//影响的行数
	int affectedRows;
	//query的执行时间
	double execTime;

};

