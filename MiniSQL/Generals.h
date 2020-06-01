#pragma once
/*包含一些全局范围内的数据类，比如查询结果*/
#include <string>
#include<cstring>
#include<vector>
#include<cstdio>
#include <sstream>


using namespace std;

enum QueryState
{
	Success,
	Fail
};

//记录一次用户操作是否成功的数据类，由API或interpreter生成并传递到最外层，在主循环中显示
//如果操作成功，不用填第四个参数
class RecordBuffer;
class QueryResult
{
public:
	QueryResult() {};
	//成功时用这个
	QueryResult(QueryState state, int affcRows, double exectime,RecordBuffer& rb)
	{
		this->state = state;
		this->affectedRows = affcRows;
		this->execTime = exectime;
		stringstream fmt;
		switch (this->state)
		{
		case Success:fmt << "Query Successfully ended in "<< this->execTime <<  "s, " <<this->affectedRows <<" rows Affected";
			this->content = fmt.str();
		default:
			break;
		}
	}
	//出错时用这个
	QueryResult(QueryState state, exception e = exception(""))
	{
		stringstream fmt;
		switch (this->state)
		{
		case Fail:fmt << "Query Failed " << e.what();
			this->content = fmt.str();
		default:
			break;
		}
	}


	~QueryResult() {};
	QueryState state;
	//结果的内容
	string content;
	//影响的行数
	int affectedRows;
	//query的执行时间
	double execTime;
	
};


class Table
{
public:
	Table();
	~Table();
	string tableName;


private:

};





//记录查询结果的缓存类，全局唯一
class RecordBuffer
{
public:
	RecordBuffer();
	~RecordBuffer();

private:

};



