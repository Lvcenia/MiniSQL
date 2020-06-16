#include "API.h"

API::API()
{
	this->p_recordManager = RecordManager::getRecordMangerPtr();
	this->p_catalogManager = CatalogManager::getInstance();
	list<string> indexNames;
	for (auto tableName : p_catalogManager->AllTables)
	{
		vector<string> indexName = p_catalogManager->getIndexVecFromTableName(tableName);;
		for (auto name : indexName)
			indexNames.insert(indexNames.end(), name);
	}
	this->p_indexManager = IndexManager::getIndexManagerPtr(indexNames);
	
	
}

API::~API()
{

}

API* API::getInstance()
{
		static API instance;
		return &instance;

}

void API::Init(CatalogManager * p_catalogManager, IndexManager * p_indexManager, RecordManager * p_recordManager)
{

}

QueryResult API::CreateTable(const Table & table)
{
	try
	{
		p_catalogManager->CreateTableCatalog(table);
		string tableName = table.getTableName();

		vector<Attribute> tableVec = table.getAttributes();
		//for each field on the table
		QueryResult res;
		
		for (auto attr : tableVec)
		{
			if (attr.isPrimary())
			{
				//create an index on the primary key
				string tableName = table.getTableName();
				string attributeName = attr.getAttributeName();
				QueryResult r1 =  p_indexManager->createIndex(tableName + attributeName + "idx", attr, table.getRecordLength(), tableName);
				//write the index info into catalog
				QueryResult r2 = p_catalogManager->CreateIndexCatalog(tableName + attributeName + "idx", tableName, attributeName);
			}
		}
		return QueryResult()
		

	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

/*不要求*/
//QueryResult API::CreateDatabase(const string & databaseName)
//{
//	try
//	{
//		/*
//		
//		
//		*/
//	}
//	catch (const std::exception& e)
//	{
//		return QueryResult(Fail, e);
//	}
//}

QueryResult API::CreateIndex(const string & tableName, const string& indexName, const string& attributeName)
{
	try
	{
		Table table = p_catalogManager->GetTable(tableName);
		for (auto attr : table.getAttributes())
		{
			//the attribute you create index on
			if (attributeName == attr.getAttributeName() )
			{
				if (p_catalogManager->IndexExist(tableName, attributeName))
				{
					string idxname = p_catalogManager->GetIndexInfo(tableName, attributeName).IndexName;
					if (idxname[0] == '$')
					{
						p_indexManager->dropIndex(idxname);
						p_catalogManager->DropIndexCatalog(idxname);
						//create the index via index manager
						p_indexManager->createIndex(indexName, attr, table.getRecordLength(), tableName);
						//write the index info into catalog
						p_catalogManager->CreateIndexCatalog(indexName, tableName, attributeName);
					}
					else
					{
						//throw IndexOnTheSameAttributeException();
					}
				}
				else
				{
					//create the index via index manager
					p_indexManager->createIndex(indexName, attr, table.getRecordLength(), tableName);
					//write the index info into catalog
					p_catalogManager->CreateIndexCatalog(indexName, tableName, attributeName);
				}
			}
		}
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
		Table table = p_catalogManager->GetTable(tableName);
		for (auto indexname : p_catalogManager->GetIndexsFromTable(tableName))
		{
			p_indexManager->dropIndex(indexname);
			p_catalogManager->DropIndexCatalog(indexname);
		}
		p_catalogManager->DropTableCatalog(tableName);
		p_recordManager->dropTable(table);
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

//不要求
//QueryResult API::DropDatabase(const string & databaseName)
//{
//	try
//	{
//		/*
//
//
//		*/
//	}
//	catch (const std::exception& e)
//	{
//		return QueryResult(Fail, e);
//	}
//}

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

QueryResult API::Select(const list<string> attributes, const string& tableName, const list<Expression>& exprs)
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

QueryResult API::Select(const string& tableName, const list<Expression>& exprs)
{
	return QueryResult();
}

QueryResult API::DeleteFromTable(const string& tableName)
{
	try
	{
		/*


		*/
		table = p_catalogManager->GetTable(tableName);
		p_recordManager->deleteValues(table)
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DeleteFromTableWhere(const string& tableName, const list<Expression>& exprs)
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



