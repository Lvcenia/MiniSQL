#include "API.h"

API::API()
{
	this->p_recordManager = RecordManager::getRecordMangerPtr();
	this->p_catalogManager = CatalogManager::getInstance();
	list<string> indexNames;
	//for (auto tableName : p_catalogManager->AllTables)
	//{
	//	vector<string> indexName = p_catalogManager->getIndexVecFromTableName(tableName);;
	//	for (auto name : indexName)
	//		indexNames.insert(indexNames.end(), name);
	//}
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

QueryResult API::CreateTable(Table & table)
{
	try
	{
		p_recordManager->createTable(table);
		cout << table.GetTableHeader().tableName << " " << table.GetTableHeader().attributes[0].name;
		cout << table.GetTableHeader().tableName << " " << table.getAttributes()[0].getAttributeName();
		p_catalogManager->CreateTableCatalog(table.GetTableHeader());
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
				QueryResult r1 =  p_indexManager->createIndex("$"+tableName +"$"+attributeName, attr, table.getRecordLength(), tableName);
				res = r1;
				//write the index info into catalog
				QueryResult r2 = p_catalogManager->CreateIndexCatalog("$" + tableName + "$" + attributeName, tableName, attributeName);
				res.execTime += r2.execTime;
			}
		}
		return res;
		

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
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
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
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		//for (auto indexname : p_catalogManager->DropTableIndex)
		//{
		//	p_indexManager->dropIndex(indexname);
		//	p_catalogManager->DropIndexCatalog(indexname);
		//}
		p_catalogManager->DropTableIndex(tableName);
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
		//drop the index file
		p_indexManager->dropIndex(indexName);
		//delete the index info from catalog
		p_catalogManager->DropIndexCatalog(indexName);
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
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		vector<string>::const_iterator it = values.cbegin();

		for (auto attr : table.getAttributes())
		{
			if (attr.isPrimary() || attr.isUniqueKey())
			{
				if (!p_catalogManager->IndexExist(table.getTableName()))
				{
					string indexName = "$" + table.getTableName() + "$" + attr.getAttributeName();
					p_indexManager->createIndex(indexName, attr, table.getRecordLength(), table.getTableName());
					p_catalogManager->CreateIndexCatalog(indexName, table.getTableName(), attr.getAttributeName());
					if (p_indexManager->keyExists("$" + table.getTableName() + "$" + attr.getAttributeName(), *it))
					{
					}//throw PriOrUniqExistException(field.getAttribute());
				}
				else
				{
					if (p_indexManager->keyExists(p_catalogManager->GetIndexInfo(table.getTableName(),attr.getAttributeName()).IndexName, *it))
					{
					}//throw PriOrUniqExistException(field.getAttribute());
				}
			}
			it++;
		}

		p_recordManager->insertValues(table, values);

		it = values.cbegin();
		//for (auto field : table.getTableVec())
		//{
		//	if (field.isPrimary() || field.isUnique())
		//		imPtr->insertValues(cmPtr->getIndexName(field.getAttribute(), table.getTableName()), *it, address);
		//	it++;
		//}
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
		vector<string> attrVec;
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		for (const auto& item : attrVec)
		{
			attrVec.push_back(item);
		}
		vector<Condition> conditions;
		for (auto expr : exprs) {

			conditions.push_back(expr_to_Condition(expr));
		}
		QueryResult res = p_recordManager->selectValues(attrVec, table, conditions);
		res.showRocords = true;
		return res;
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::Select(const string& tableName, const list<Expression>& exprs)
{
	try
	{
		Table table = Table(p_catalogManager->GetTableHeader(tableName));

		vector<Condition> conditions;
		for (auto expr : exprs) {

			conditions.push_back(expr_to_Condition(expr));
		}
		QueryResult res = p_recordManager->selectValues(table, conditions);
		res.showRocords = true;
		return res;
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

QueryResult API::DeleteFromTable(const string& tableName)
{
	try
	{
		/*


		*/
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		QueryResult res = p_recordManager->deleteValues(table,vector<Condition>());
		
		return res;
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
		vector<Condition> conditions;
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		for (auto expr : exprs) {
			
			conditions.push_back(expr_to_Condition(expr));
		}
		
		QueryResult res = p_recordManager->deleteValues(table,conditions);
		return res;
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}

OP API::OPERATOR_to_OP(OPERATOR o)
{
	switch (o)
	{
	case GREATER:return GreaterThan;
		break;
	case LESS:return LessThan;
		break;
	case EQUAL:return Equal;
		break;
	case NOTEQUAL:return NotEqual;
		break;
	case GREATER_AND_EQUAL:return GreaterEqual;
		break;
	case LESS_AND_EQUAL:return LessEqual;
		break;
	case UNDEFINED:
		break;
	default:
		break;
	}
}

Condition API::expr_to_Condition(const Expression& expr)
{
	OP op = OPERATOR_to_OP(expr.op);
	return Condition(expr.leftOperand.operandName,op, expr.rightOperand.operandName);

}



