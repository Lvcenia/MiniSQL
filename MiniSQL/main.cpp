// 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>
#include "API.h"
#include "Interpreter.h"
#include "Exception.h"
#include "BufferManager.h"

using namespace std;
BOOL WINAPI QuitHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		// 控制台将要被关闭,将内容写入到磁盘中
		auto bfm = BufferManager::getBufferManager();
		if (bfm != nullptr) {
			bfm->saveDataForcedly();
		}
		
		return FALSE;
	}

}



//主函数
int main()
{
	SetConsoleCtrlHandler(QuitHandler, TRUE);


	Interpreter interpreter;


	cout << "------------------------ Welcome to MiniSQL! -------------------------" << endl;

	string sql;
	bool changesentence = 1;
	while (1)
	{
		try {
			string line;
			if (changesentence) { 
				cout << "\nMiniSql->>"; 
				changesentence = 0;
			}
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
					sql = sql.substr(0, sql.length() - 1);
					interpreter.executeSql(sql); // 进入执行
					interpreter.print(); // 输出执行结果
				}
				sql.clear();
				changesentence = 1;
			}
			else
				cout << ">>>";

		}
		
		catch (Quit& q) {
			break;
		}


	}
	cout << "-------------------------------- Bye! --------------------------------" << endl;
	return 0;
}
