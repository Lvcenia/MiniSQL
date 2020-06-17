#pragma once
#include<cstring>
#include<set>
#include<map>
#include "Generals.h"
#include "IndexManager.h"
#include "RecordManager.h"
#ifndef __CatalogManager__
#define __CatalogManager__
class IndexInfo 
{
public:
	std::string IndexName;
	std::string TableName;
	std::string AttrName;
	int RecordLenth;
	IndexInfo(std::string _TableName, std::string _AttrName, std::string _IndexName) :
		IndexName(_IndexName), TableName(_TableName), AttrName(_AttrName) {};
	IndexInfo(std::string _TableName, std::string _AttrName) :
		TableName(_TableName), AttrName(_AttrName) {};
	IndexInfo(std::string _IndexName) :
		IndexName(_IndexName) {};
	IndexInfo() {}
	bool valid() {
		return IndexName != "";
	}
};
class CatalogManager
{
public:
	CatalogManager();
	~CatalogManager();
	string PrimaryKeyIndex(TableHeader table);

	QueryResult CreateTableCatalog(const TableHeader& table);
	QueryResult DropTableCatalog(const std::string& TableName);
	QueryResult CreateIndexCatalog(std::string IndexName, std::string TableName, std::string AttributeName);
	QueryResult DropIndexCatalog(std::string IndexName);
	QueryResult DropTableIndex(std::string TableName);

	bool TableExist(const std::string& TableName);
	bool IndexExist(const std::string& IndexName);
	bool IndexExist(const std::string& TableName, const std::string AttrName);
	bool AttributeExist(const std::string& TableName, std::string AttrName);

	TableHeader GetTableHeader(const std::string& TableName);
	IndexInfo GetIndexInfo(const std::string& IndexName);
	IndexInfo GetIndexInfo(const std::string& TableName, std::string AttrName);
	static CatalogManager* getInstance();

	std::map<std::string, TableHeader> AllTables();
	std::map<std::string, IndexInfo> Allindexs();
private:
	const std::string TableCatalogDirection = "data/catalog/tables/";
	const std::string TableNameCatalog = "data/catalog/tables.cat";
	const std::string IndexCatalogDirection = "data/catalog/index.cat";
	QueryResult DropLineFromFile(std::string FileName, std::string ElementToDrop, int ElementOrder);
	bool LineExist(std::string FileName, std::string LineInfo,int element);
};

#endif // !__CatalogManager__

