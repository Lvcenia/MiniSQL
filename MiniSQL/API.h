#pragma once
#include "Generals.h"
using namespace std;

class API
{
	API(); 
	~API();
public:
	QueryResult CreateTable();
	QueryResult CreateDatabase();
	QueryResult CreateIndex();
	QueryResult DropTable();
	QueryResult DropDatabase();
	QueryResult DropIndex();
	QueryResult InsertValuesInto();
	QueryResult Select();
	QueryResult DeleteFromTable();


















	



};







