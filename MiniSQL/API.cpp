#include "API.h"

API::API()
{
}

API::~API()
{

}

void API::Init(CatalogManager * p_catalogManager, IndexManager * p_indexManager, RecordManager * p_recordManager)
{
	this->p_catalogManager = p_catalogManager;
	this->p_indexManager = p_indexManager;
	this->p_recordManager = p_recordManager;
}

QueryResult API::CreateTable(const Table & table)
{
	try
	{
		/*
		
		
		*/

	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::CreateDatabase(const string & databaseName)
{
	try
	{
		/*
		
		
		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::CreateIndex(const string & tableName, const string& indexName, const string& attributeName)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DropTable(const string & tableName)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DropDatabase(const string & databaseName)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DropIndex(const string indexName, const string & tableName, const string & attributeName)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::InsertValuesInto(const string & tableName, const vector<string>& values)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::Select(const list<string> attributes, const string & tableName)
{
	try
	{
		/*


		*/
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DeleteFromTable()
{
	try
	{
		/*
		

		*/
		
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}
