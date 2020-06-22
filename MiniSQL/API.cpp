#include "API.h"

API::API()
{
	this->p_recordManager = RecordManager::getRecordMangerPtr();
	this->p_catalogManager = CatalogManager::getInstance();
	list<string> indexes;
	for (auto& index : p_catalogManager->Allindexs())
	{
		indexes.push_back(index.second.IndexName);
	}
	this->p_indexManager = IndexManager::getIndexManagerPtr(indexes);
	
}

API::~API()
{

}

API* API::getInstance()
{
		static API instance;
		return &instance;

}

QueryResult API::CreateTable(Table & table)
{
	try
	{
		cout << table.getRecordLength();
		auto start = clock();
		//创建table.data数据文件
		p_recordManager->createTable(table);
		//创建表的Catalog信息
		p_catalogManager->CreateTableCatalog(table.GetTableHeader());
		string tableName = table.getTableName();

		vector<Attribute> tableVec = table.getAttributes();
		//for each field on the table
		
		QueryResult res;
		for (auto attr : tableVec)
		{
			if (attr.isPrimary())
			{
				//在主键上建立索引
				string attributeName = attr.getAttributeName();
				p_indexManager->createIndex(p_catalogManager->PrimaryKeyIndex(table.GetTableHeader()), attr, table.getRecordLength(), tableName);
				//索引信息写入catalog
				p_catalogManager->CreateIndexCatalog(p_catalogManager->PrimaryKeyIndex(table.GetTableHeader()), tableName, attributeName);
			}
		}
		auto end = clock();
		auto time = (double)(end - start) / CLOCKS_PER_SEC;
		RecordBuffer rb;
		return QueryResult(Success, 0, time,rb);
		

	}
	catch (const std::exception& e)
	{
		cout << e.what();
		return QueryResult(Fail, e);
	}
}


QueryResult API::CreateIndex(const string & indexName, const string& tableName, const string& attributeName)
{
	try
	{
		auto start = clock();
		Table table = Table(p_catalogManager->GetTableHeader(tableName));

		for (auto attr : table.getAttributes())
		{
			//the attribute you create index on
			if (attributeName == attr.getAttributeName() )
			{
				if(! (attr.isPrimary() || attr.isUniqueKey()))
					return QueryResult(Fail, CatalogError("Attribute " + attributeName + " is not unique"));
				if (p_catalogManager->IndexExist(tableName, attributeName))
				{
					string idxname = p_catalogManager->GetIndexInfo(tableName, attributeName).IndexName;
					//如果已经有了系统自动创建的索引 删掉重创
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
						return QueryResult(Fail, CatalogError("Index on the same attribute exists"));
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
		auto end = clock();
		auto time = (double)(end - start) / CLOCKS_PER_SEC;
		RecordBuffer rb;
		return QueryResult(Success, 0, time, rb);
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
		auto start = clock();
		int  affectedRows = Table(p_catalogManager->GetTableHeader(tableName)).getRecordCount();
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		for (auto indexinfo : p_catalogManager->GetIndexInfoByTableName(tableName))
		{
			p_indexManager->dropIndex(indexinfo.IndexName);
		}
		p_catalogManager->DropTableIndex(tableName);
		p_catalogManager->DropTableCatalog(tableName);
		p_recordManager->dropTable(table);
		auto end = clock();
		auto time = (double)(end - start) / CLOCKS_PER_SEC;
		RecordBuffer rb;
		return QueryResult(Success, affectedRows, time, rb);
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
		auto start = clock();

		//delete the index info from catalog
		p_catalogManager->DropIndexCatalog(indexName);
		//drop the index file
		p_indexManager->dropIndex(indexName);
		
		auto end = clock();
		auto time = (double)(end - start) / CLOCKS_PER_SEC;
		RecordBuffer rb;
		return QueryResult(Success, 0, time, rb);
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
		auto start = clock();
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		vector<string>::const_iterator it = values.cbegin();
		string tableName = table.getTableName();

		//在数据表中插入，通过已有的索引保证所有值不重复
		for (auto attr : table.getAttributes())
		{
			if (attr.isPrimary() || attr.isUniqueKey())
			{
				string indexName = "$" + table.getTableName() + "$" + attr.getAttributeName();
				//如果是唯一值或主键，并且没有创建索引，就为它创建索引
				if (!p_catalogManager->IndexExist(tableName,attr.getAttributeName()))
				{
					p_indexManager->createIndex(indexName, attr, table.getRecordLength(), tableName);
					p_catalogManager->CreateIndexCatalog(indexName, table.getTableName(), attr.getAttributeName());
					if (p_indexManager->keyExists(indexName, *it))
					{
						return QueryResult(Fail, CatalogError("Duplicate primary or unique key"));
					} 
				}
				//如果已经有索引，保证值不重复
				else
				{
					string idxName = p_catalogManager->GetIndexInfo(table.getTableName(), attr.getAttributeName()).IndexName;
					if (p_indexManager->keyExists(idxName, *it))
					{
						return QueryResult(Fail, CatalogError("Duplicate primary or unique key"));
					}
				}
			}
			it++;
		}


		auto address = p_recordManager->insertValues(table, values);

		//更新索引中的信息
		it = values.cbegin();
		for (auto attr : table.getAttributes())
		{
			if (attr.isPrimary() || attr.isUniqueKey())
			{
				IndexInfo indexinfo = p_catalogManager->GetIndexInfo(tableName, attr.getAttributeName());
				if (indexinfo.valid())
				{
					p_indexManager->insertValues(indexinfo.IndexName, *it, address);
				}
				
			}
			it++;
		}
		auto end = clock();
		auto time = (double)(end - start) / CLOCKS_PER_SEC;
		RecordBuffer rb;
		return QueryResult(Success,1, time, rb);
		
	}
	catch (const std::exception& e)
	{
		cout << e.what();
		return QueryResult(Fail, e);
	}
}

/*Select 
1.不使用索引：没有condition，或者condition中attribute没有索引
2.使用索引：condition中的attribute有索引

*/
QueryResult API::Select(const list<string> attributes, const string& tableName, const list<Expression>& exprs)
{
	try
	{
		auto start = clock();
		vector<string> attrVec;
		vector<string> indexedAttrsInCondition;
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		bool hasIndexedAttr = false;
		//所有要select的attribute
		for (const auto& item : attributes)
		{
			attrVec.push_back(item);
		}
		vector<Condition> conditions;
		for (auto expr : exprs) {
			if (expr.leftOperand.isAttribute)
			{
				//如果这个属性上有索引
				if (p_catalogManager->GetIndexInfo(tableName, expr.leftOperand.operandName).valid())
				{
					indexedAttrsInCondition.push_back(expr.leftOperand.operandName);
				}
			}
			if (expr.rightOperand.isAttribute)
			{
				//如果这个属性上有索引
				if (p_catalogManager->GetIndexInfo(tableName, expr.rightOperand.operandName).valid())
				{
					indexedAttrsInCondition.push_back(expr.leftOperand.operandName);
				}
			}
			conditions.push_back(expr_to_Condition(expr));
		}
		//如果没有condition 或者where里的attr没有索引 直接用recordManager
		if (conditions.size()<1 || indexedAttrsInCondition.size()< 1)
		{
			QueryResult res = p_recordManager->selectValues(attrVec, table, conditions);
			auto end = clock();
			auto time = ((double)end - (double)start) / CLOCKS_PER_SEC;
			res.execTime = time;
			res.showRocords = true;
			//res.PrintRecords();

			return res;
		}
		//否则使用索引来select,如果有多个带索引的attribute，用第一个
		auto useIndexInfo = p_catalogManager->GetIndexInfo(tableName, indexedAttrsInCondition[0]);
		QueryResult res = p_indexManager->selectValues(useIndexInfo.IndexName,attributes,table,conditions,tableName);


		auto end = clock();
		auto time = ((double)end - (double)start) / CLOCKS_PER_SEC;
		res.execTime = time;
		res.showRocords = true;

		return res;
	}
	catch (const std::exception& e)
	{
		cout << e.what();
		return QueryResult(Fail, e);
	}
}

//这个Select废弃
//QueryResult API::Select(const string& tableName, const list<Expression>& exprs)
//{
//	try
//	{
//		Table table = Table(p_catalogManager->GetTableHeader(tableName));
//
//		vector<Condition> conditions;
//		for (auto expr : exprs) {
//
//			conditions.push_back(expr_to_Condition(expr));
//		}
//		QueryResult res = p_recordManager->selectValues(table, conditions);
//		res.showRocords = true;
//		return res;
//	}
//	catch (const std::exception& e)
//	{
//		return QueryResult(Fail, e);
//	}
//}


//删除表中所有值
QueryResult API::DeleteFromTable(const string& tableName)
{
	try
	{
		auto start = clock();
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		//删除record中的值
		QueryResult res = p_recordManager->deleteValues(table,vector<Condition>());
		//删除索引中所有值
		for (auto& indexinfo : p_catalogManager->GetIndexInfoByTableName(tableName))
		{
			p_indexManager->deleteValuesAll(indexinfo.IndexName);
		}

		auto end = clock();
		auto time = ((double)end - (double)start) / CLOCKS_PER_SEC;
		res.execTime = time;
		
		return res;
	}
	catch (const std::exception& e)
	{
		return QueryResult(Fail, e);
	}
}


/*条件删除
数据文件中的内容要删
表内所有索引中的内容也要相应地删掉
*/
QueryResult API::DeleteFromTableWhere(const string& tableName, const list<Expression>& exprs)
{
	try
	{
		auto start = clock();
		vector<string> attrVec;
		vector<string> indexedAttrsInCondition;
		Table table = Table(p_catalogManager->GetTableHeader(tableName));
		bool hasIndexedAttr = false;

		list<string> indexlist;
		for (auto& indexinfo : p_catalogManager->GetIndexInfoByTableName(tableName))
		{
			indexlist.push_back(indexinfo.IndexName);
		}

		string primaryKey = table.getAttributes()[table.getPrimaryKeyIndex()].getAttributeName();

		vector<Condition> conditions;
		for (auto expr : exprs) {
			if (expr.leftOperand.isAttribute)
			{
				//如果这个属性上有索引
				if (p_catalogManager->GetIndexInfo(tableName, expr.leftOperand.operandName).valid())
				{
					indexedAttrsInCondition.push_back(expr.leftOperand.operandName);
				}
			}
			if (expr.rightOperand.isAttribute)
			{
				//如果这个属性上有索引
				if (p_catalogManager->GetIndexInfo(tableName, expr.rightOperand.operandName).valid())
				{
					indexedAttrsInCondition.push_back(expr.leftOperand.operandName);
				}
			}
			conditions.push_back(expr_to_Condition(expr));
		}
		//如果没有condition 或者where里的attr没有索引 索引用主键删
		if (conditions.size() < 1 || indexedAttrsInCondition.size() < 1)
		{
			p_indexManager->deleteValues(p_catalogManager->GetIndexInfo(tableName, primaryKey).IndexName, list<string>(), indexlist, tableName, table.getRecordLength());
			QueryResult res = p_recordManager->selectValues(attrVec, table, conditions);
			auto end = clock();
			auto time = ((double)end - (double)start) / CLOCKS_PER_SEC;
			res.execTime = time;
			res.showRocords = true;

			return res;
		}
		//否则用第一个索引来删
		auto useIndexname = *indexlist.begin();
		QueryResult res = p_indexManager->deleteValues(useIndexname, indexlist, conditions, tableName, table.getRecordLength());

		auto end = clock();
		auto time = ((double)end - (double)start) / CLOCKS_PER_SEC;
		res.execTime = time;
		res.showRocords = true;

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



