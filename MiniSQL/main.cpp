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

	cout << "*******************Bye!*******************" << endl;
	return 0;

}

*/
