// 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "API.h"
#include "Interpreter.h"
#include "BufferManager.h"
using namespace std;

/*
//主函数
int main()
{
	RecordBuffer recordBuffer();

	//创建几个Manager对象并各自初始化
	CatalogManager catalogManager;
	IndexManager indexManager;
	RecordManager recordManager;
	BufferManager bufferManager;
	Interpreter interpreter;

	API api;
	api.Init(&catalogManager, &indexManager, &recordManager);
	
    cout << "*******************Welcome to MiniSQL!*******************" << endl;

	string userInput;
	//接收输入、调用Interpreter处理输入、显示处理结果的循环

	while (1)
	{
		cout << "MiniSql->>";
		getline(cin, userInput, ';');
		//QueryResult res =  interperter.execSql(userInput);
		//cout << res.content;

	}

	string sql;
	while (1)
	{
		string line;
		getline(cin, line);

		sql += (line + ' ');
		//find a semicolon
		if (line.find_first_of(';') < line.size())
		{
			while (sql.back() == ' ' || sql.back() == '\t')
				sql = sql.substr(0, sql.length() - 1);
			if (sql.back() != ';' || sql.find_first_of(';') < sql.size() - 1)
				cout << "ERROR: There is some characters at the end of the sql after the semicolon;" << endl;
			else
			{
				//QueryResult res =  interperter.executeSql(sql);
				//cout << res.content;
			}
			cout << "MiniSql->>";
			sql.clear();
		}
		else
			cout << ">>>";

	cout << "*******************Bye!*******************" << endl;
	return 0;

}

*/
