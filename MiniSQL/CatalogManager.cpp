#include "CatalogManager.h"
#include "Exception.h"
#include<fstream>
#include<vector>
#include<exception>
#include<ctime>
#include<direct.h>
#include<io.h>
using std::exception;
CatalogManager* CatalogManager::getInstance()
{
    static CatalogManager ctm;
    return &ctm;
}
CatalogManager::CatalogManager() {
    if (_access("data/", 0) == -1) {
        _mkdir("data/");
    }
    if (_access("data/catalog/", 0) == -1) {
        _mkdir("data/catalog/");
    }
    if (_access("data/catalog/tables/", 0) == -1) {
        _mkdir("data/catalog/tables/");
    }
}
CatalogManager::~CatalogManager() {

}
/*******************************/
string CatalogManager::PrimaryKeyIndex(TableHeader table) {
    return "PRIKEY_" + (std::string)table.tableName;
}


/**************************/
QueryResult CatalogManager::CreateTableCatalog(const TableHeader& table) {
    std::ofstream Cata_File;
    double start = clock();
    try {
        Cata_File.open(TableCatalogDirection + table.tableName);
        Cata_File << table.tableName << endl;
        Cata_File << table.recordCount << endl;
        Cata_File << table.recordLength << endl;
        Cata_File << table.rowLength << endl;
        Cata_File << table.primaryKeyIndex << endl;
        int cnt = 0;
        for (const auto& attr : table.attributes) ++cnt;
        Cata_File << cnt << endl;
        for (const auto& attr : table.attributes) {
            Cata_File << attr.isPrimaryKey << ' ' << attr.isUnique << ' ' << attr.length << ' ' << attr.name << ' ' << attr.offset << ' ' << attr.type << endl;
        }
        Cata_File.close();
        ofstream TableNameCatalogFile;
        TableNameCatalogFile.open(TableNameCatalog,std::ios_base::app);
        TableNameCatalogFile << table.tableName;
        TableNameCatalogFile.close();
        CreateIndexCatalog(PrimaryKeyIndex(table), table.tableName, 
            table.attributes[table.primaryKeyIndex].name);
    }
    catch (...) {
        CatalogError CE("error in CreateTableCatalog");
        throw CE;
        return QueryResult(Fail, CE);

    }
    double end = clock();
    RecordBuffer BF;
    return QueryResult(Success, 0, end - start, BF);
}

/*********************************/
QueryResult CatalogManager::DropTableCatalog(const std::string& TableName) {
    QueryResult P;
    double start = clock();
    try {
        DropTableIndex(TableName);
        DropLineFromFile(TableNameCatalog, TableName, 1);
        remove((TableCatalogDirection + TableName).c_str());
    }
    catch (...) {
        CatalogError CE("error in DropTableCatalog");
        throw CE;
        return QueryResult(Fail, CE);
    }
    double end = clock();
    RecordBuffer BF;
    return QueryResult(Success, 0, end - start, BF);
}
/**************************************/
QueryResult CatalogManager::CreateIndexCatalog(std::string IndexName, std::string TableName, std::string AttributeName) {
    ofstream IndexCataloge;
    double start = clock();
    try {
        IndexCataloge.open(IndexCatalogDirection, std::ios_base::app);
        IndexCataloge << IndexName << ' ' << TableName << ' ' << AttributeName << endl;
        IndexCataloge.close();
    }
    catch (...) {
        CatalogError CE("error in CreateIndexCatalog");
        throw CE;
        return QueryResult(Fail, CE);
    }
    double end = clock();
    RecordBuffer BF;
    return QueryResult(Success, 0, end - start, BF);
}
/***************/
QueryResult CatalogManager::DropIndexCatalog(std::string IndexName) {
    return DropLineFromFile(IndexCatalogDirection, IndexName, 1);
}

/**********************/
QueryResult CatalogManager::DropTableIndex(std::string TableName) {
    return DropLineFromFile(IndexCatalogDirection, TableName, 2);
}


/****************************************/
bool CatalogManager::TableExist(const std::string& TableName) {
    return LineExist(TableNameCatalog, TableName, 1);
}
/*************************************/
bool CatalogManager::IndexExist(const std::string& IndexName) {
    return LineExist(IndexCatalogDirection, IndexName, 1);
}
/*******************************/
bool CatalogManager::IndexExist(const std::string& TableName, const std::string AttrName) {
    return GetIndexInfo(TableName, AttrName).valid();
}
/***********************/
bool CatalogManager::AttributeExist(const std::string& TableName, std::string AttrName) {
    TableHeader _tableheader = GetTableHeader(TableName);
    for (const auto attr : _tableheader.attributes) {
        if ((std::string)attr.name == AttrName){
            return true;
        }
    }
    return false;
}
/*******************************/
TableHeader CatalogManager::GetTableHeader(const std::string& TableName) {
    TableHeader table;
    ifstream TableCatalogFile;
    try {
        TableCatalogFile.open(TableCatalogDirection + TableName);
        TableCatalogFile >> table.tableName >> table.recordCount >> table.recordLength >> table.rowLength >> table.primaryKeyIndex;
        int NumOfAttr;
        TableCatalogFile >> NumOfAttr;
        for (int i = 0; i < NumOfAttr; ++i) {
            string S_type;
            TableCatalogFile >> table.attributes[i].isPrimaryKey>>table.attributes[i].isUnique
                >>table.attributes[i].length>>table.attributes[i].name>>table.attributes[i].offset>>S_type;
            table.attributes[i].type = stringToType(S_type);
        }
    }catch(CatalogError e){

    }
    return table;
}

/***************************/
IndexInfo CatalogManager::GetIndexInfo(const std::string& IndexName) {
    ifstream IndexCata;
    IndexCata.open(IndexCatalogDirection);
    string Line, _tablename, _indexname, _attrname;
    for (; getline(IndexCata, Line);) {
        istringstream LineStream(Line);
        LineStream >> _indexname >> _tablename >> _attrname;
        if (_indexname == IndexName) return IndexInfo(_tablename, _attrname, _indexname);
    }
    return IndexInfo();
}
/********************************/
IndexInfo CatalogManager::GetIndexInfo(const std::string& TableName, std::string AttrName) {
    ifstream IndexCata;
    IndexCata.open(IndexCatalogDirection);
    string Line, _tablename, _indexname, _attrname;
    for (; getline(IndexCata, Line);) {
        istringstream LineStream(Line);
        LineStream >> _indexname >> _tablename >> _attrname;
        if (TableName==_tablename && _attrname==AttrName) return IndexInfo(_tablename, _attrname, _indexname);
    }
    return IndexInfo();
}
/**********************************/
map<std::string, TableHeader> CatalogManager::AllTables(){
    ifstream TableNameFile;
    TableNameFile.open(TableNameCatalog);
    string _TableName;
    map<string, TableHeader> result;
    for (; getline(TableNameFile, _TableName);) {
        result.insert(std::pair<std::string, TableHeader>(_TableName, GetTableHeader(_TableName)));
    }
    return result;
}
/*************************************/
std::map<std::string, IndexInfo> CatalogManager::Allindexs(){
    map<std::string, IndexInfo> result;
    ifstream IndexCata;
    IndexCata.open(IndexCatalogDirection);
    string Line, _tablename, _indexname, _attrname;
    for (; getline(IndexCata, Line);) {
        istringstream LineStream(Line);
        LineStream >> _indexname >> _tablename >> _attrname;
        result.insert(std::pair<string, IndexInfo>(_indexname, IndexInfo(_tablename, _attrname, _indexname)));
    }
}
/***************************************/
QueryResult CatalogManager::DropLineFromFile(std::string FileName, std::string ElementToDrop, int ElementOrder) {
    ifstream INFILE;
    ofstream OUTFILE;
    double start = clock();
    try {
        INFILE.open(FileName);
        OUTFILE.open(FileName + ".tmp");
        string Line;
        for (; getline(INFILE, Line);) {
            istringstream LineStream(Line);
            string _tmp;
            for (int i = 0; i < ElementOrder; ++i) {
                LineStream >> _tmp;
            }
            if (_tmp != ElementToDrop)OUTFILE << Line << endl;
        }
        INFILE.close(), OUTFILE.close();
        remove(FileName.c_str());
        rename((FileName + ".tmp").c_str(), FileName.c_str());
    }
    catch (...) {
        CatalogError CE("error in DropLineFromFile");
        throw CE;
        return QueryResult(Fail, CE);
    }
    double end = clock();
    RecordBuffer BF;
    return QueryResult(Success, 0, end - start, BF);
}
/*********************************/
bool CatalogManager::LineExist(std::string FileName, std::string LineInfo,int element) {
    ifstream ___File;
    ___File.open(FileName);
    string Line,__TMP;
    for (; getline(___File, Line);) {
        istringstream LineStream(Line);
        for (int i = 0; i < element; ++i) {
            
            LineStream >> __TMP;
            if (__TMP == LineInfo)return 1;
        }
    }
    return 0;
}