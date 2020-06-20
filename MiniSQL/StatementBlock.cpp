

#include "StatementBlock.h"
#include "CatalogManager.h"
#include "Exception.h"
#include "API.h"
#include "Interpreter.h"

#include <iostream>
#include <set>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace std;

void CreateTableBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at the end

	//if (pcb->isTableExist(table.getTableName()))
	if (pcb->TableExist(table.getTableName()))

		throw CatalogError("The table have already exist");

	set<string> attrSet;

	//auto tableVec = table.getTableVec();
	auto tableVec = table.getAttributes();

	for (auto &data : tableVec) {

		//attrSet.insert(data.getAttribute());
		attrSet.insert(data.getAttributeName());

	}

	if (attrSet.size() != tableVec.size())
		throw GrammarError("exist duplicated attributes");
	string s =primaryKeyName;

	//auto iter = find_if(tableVec.begin(), tableVec.end(), [=](const Data& d)->bool {
	auto iter = find_if(tableVec.begin(), tableVec.end(), [=](const Attribute& d)->bool {

		//return d.getAttribute() == s;
		return d.getAttributeName() == s;

		; });
	if (iter == tableVec.end())
		throw GrammarError("The name of primary key attribute does not exist");
	iter->setPrimary(true);
	table.setPrimaryKeyIndex(iter - tableVec.begin());

	//table.setTableVec(tableVec);
	table.setAttributes(tableVec);
	

	//delete pcb;
}

QueryResult CreateTableBlock::execute()
{
	auto api = API::getInstance();
	//auto api = new API(); // detele at the end

	//api->createTableCmd(table);
	QueryResult ret = api->CreateTable(table);  // TODO : here is a queryresult must to be returned , add a queryresult in statementblock

	//delete api;

	return ret;
}

void CreateTableBlock::print()
{
	cout <<"Table Name:"<< table.getTableName()<<endl;

	//auto v = table.getTableVec();
	auto v = table.getAttributes();

	for (auto& c : v) {
		//cout << "Attribute:" << c.getAttribute() << " " << " TYPE:"
		cout << "Attribute:" << c.getAttributeName() << " " << " TYPE:"

			<< c.getType() << " length:" << c.getLength();

		//if (c.isUnique() == true)
		if (c.isUniqueKey() == true)

			cout << " UNIQUE";
		if (c.isPrimary() == true)
			cout << " PRIMARY";
			cout << endl;
	}
	cout <<" Primary Key Name:"<< primaryKeyName << endl;
}

void CreateIndexBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb =  new CatalogManager();  // delete at hte end

	//if (pcb->isIndexExist(indexName))
	if (pcb->IndexExist(indexName))

		throw CatalogError("index(" + indexName + ") exists");

	//if (!pcb->isTableExist(tableName))
	if (!pcb->TableExist(tableName))

		throw CatalogError("table(" + tableName + ") does not exist");

	//auto table = pcb->getTable(tableName);
	auto table = Table(pcb->GetTableHeader(tableName));

	if (!table.hasAttribute(attributeName))
	//if (!table.isAttribute(attributeName))

		throw CatalogError("attribute("+ attributeName +") does not exist");

	auto d = table.getAttribute(attributeName);
	//auto d = table.getData(attributeName);

	if (d.isUniqueKey() || d.isPrimary())
	//if (d.isUnique() || d.isPrimary())

		return;
	else throw CatalogError("attribute(" + attributeName + ") is not unique or primary key");

	//delete pcb;
}

QueryResult CreateIndexBlock::execute()
{
	//auto api = new API();
	auto api = API::getInstance();

	QueryResult ret = api->CreateIndex(indexName, tableName, attributeName);
	//api->createIndexCmd(indexName, tableName, attributeName);

	//delete api;

	return ret;
}

void CreateIndexBlock::print()
{
	cout << "indexName:" << indexName
		<< " tableName:" << tableName
		<< " attributeName:" << attributeName
		<< endl;
}

void InsertTableBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	auto table = Table(pcb->GetTableHeader(tableName));
	//auto table = pcb->getTable(tableName);

	if (table.getTableName() != tableName)
		throw CatalogError("Table name(" + tableName + ") does not exist");

	auto& tableVec = table.getAttributes();
	//auto& tableVec = table.getTableVec();

	if (tableVec.size() != values.size())
		throw CatalogError("The number of attributes does not match");
	CheckType ct;
	string s;
	int size = tableVec.size();
	for (int i = 0; i < size; i++) {

		Type type = tableVec[i].getType();
		//TYPE type = tableVec[i].getType();

		if (!ct.isType(values[i], type)) 
			throw CatalogError("The type of " + values[i] + " does not match defined type ");
		if (type == CHAR) {
			if (values[i].length() <= tableVec[i].getLength() + 2)
				values[i] = string(values[i].begin() + 1, values[i].end() - 1);
			else
				throw CatalogError("The length of string does not match");
		}
	}
	
	//delete pcb;
}

QueryResult InsertTableBlock::execute()
{
	//auto api = new API();
	auto api = API::getInstance();

	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	vector<string> v;
	//list<string> v;

	for (auto& content : values) {
		v.push_back(content);
	}

	QueryResult ret = api->InsertValuesInto(tableName, v);
	//api->insertValuesCmd(v, pcb->getTable(tableName));

	//delete pcb;
	//delete api;

	return ret;
}

void InsertTableBlock::print()
{
	cout << "tableName:" << tableName << endl;
	cout << "values:";
	for (auto & s : values) {
		cout << s << " ";
	}cout << endl;
}

void QuitBlock::check()
{
	//no need to check
}

QueryResult QuitBlock::execute()
{
	throw Quit();
	// no return
}

void QuitBlock::print()
{
	cout << "quit" << endl;
}

void DropTableBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	if (!pcb->TableExist(tableName))
	//if (!pcb->isTableExist(tableName))

		throw CatalogError("The table does not exist");

	//delete pcb;
}

QueryResult DropTableBlock::execute()
{
	//auto api = new API();
	auto api = API::getInstance();

	QueryResult ret = api->DropTable(tableName);
	//api->dropTableCmd(tableName);


	return ret;
}

void DropTableBlock::print()
{
	cout << "drop " << tableName << endl;
}

void DropIndexBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	if (!pcb->IndexExist(indexName))
	//if (!pcb->isIndexExist(indexName))
		throw CatalogError("The index does not exist");

}

QueryResult DropIndexBlock::execute()
{
	auto api = API::getInstance();
	//auto api = API::getAPIPtr();

	QueryResult ret = api->DropIndex(indexName,"","");
	//api->dropIndexCmd(indexName);


	return ret;
}

void DropIndexBlock::print()
{
	cout << "drop " << indexName << endl;
}

void DeleteBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	if(!pcb->TableExist(tableName))
	//if(!pcb->isTableExist(tableName))

		throw CatalogError("The table does not exist");
	if (!flag)return;

	Table table = Table(pcb->GetTableHeader(tableName));
	//Table table = pcb->getTable(tableName);

	CheckType ct(table);
	list<Expression> tmpExps;
	for (auto &exp : exps) {
		string leftName = exp.leftOperand.operandName;
		string rightName = exp.rightOperand.operandName;

		Type typeLeft = ct.isWhatType(leftName);
		//TYPE typeLeft = ct.isWhatType(leftName);
		Type typeRight = ct.isWhatType(rightName);
		//TYPE typeRight = ct.isWhatType(rightName);

		if (typeLeft != typeRight)throw CatalogError("unmatched type");
		bool b1 = ct.isType(leftName, typeLeft), b2 = ct.isType(rightName, typeRight);
		
		exp.leftOperand.isAttribute = !b1;
		exp.rightOperand.isAttribute = !b2;
		//if they are all normal data
		if (b1&&b2) {
			if (compareExp(leftName, rightName, typeLeft, exp.op)){
			}
			else doNothingFlag = true;
		}
		else {
			if (b1&&typeLeft == CHAR)
				exp.leftOperand.operandName = string(leftName.begin() + 1, leftName.end() - 1);
			if (b2&&typeRight == CHAR)
				exp.rightOperand.operandName = string(rightName.begin() + 1, rightName.end() - 1);
			if (b1&&!b2)
				exp.swap();
			tmpExps.push_back(exp);
		}
	}
	if (tmpExps.empty()) {
		flag = true;
	}
	exps = tmpExps;
	//delete pcb;
}

QueryResult DeleteBlock::execute()
{
	if (doNothingFlag)return QueryResult();

	auto api = API::getInstance();
	//auto api = API::getAPIPtr();
	
	if (!flag) {
		QueryResult ret = api->DeleteFromTable(tableName);
		//api->deleteValuesCmd(tableName);

		

		return ret;
	}
	else {
		QueryResult ret = api->DeleteFromTableWhere(tableName, exps);
		//else api->deleteValuesCmd(tableName, exps);

		

		return ret;
	}
}

void DeleteBlock::print()
{
	cout << "Delete from " << tableName << " ";
	if (flag) {
		if (exps.size() != 0) {
			for (auto& exp : exps) {
				cout << "[" << exp.leftOperand.operandName << " " << exp.op << " " << exp.rightOperand.operandName << "]";
			}
		}
	}
	cout << endl;
}

void SelectBlock::check()
{
	auto pcb = CatalogManager::getInstance();
	//auto pcb = new CatalogManager();  // delete at hte end

	if (!pcb->TableExist(tableName))
		throw CatalogError("The table does not exist");

	Table table = Table(pcb->GetTableHeader(tableName));
	std::cout << "table: " + tableName;
	//Table table = pcb->getTable(tableName);


	vector<Attribute> tableVec = table.getAttributes();
	//vector<Data> tableVec = table.getTableVec();
	
	if (star) {
		//select *
		attributes.clear();
		for (auto& data : tableVec) {

			attributes.push_back(data.getAttributeName());
			//attributes.push_back(data.getAttribute());
		}
	}
	else {
		for (auto& attr : attributes) {

			if (!table.hasAttribute(attr))
			//if (!table.isAttribute(attr))

				throw CatalogError("The attribute does not exist");
		}
	}
	if (exps.size() == 0)return;
	CheckType ct(table);
	
	std::list<Expression> expTmp;
	for (auto &exp : exps) {
		string leftName = exp.leftOperand.operandName;
		string rightName = exp.rightOperand.operandName;

		Type typeLeft = ct.isWhatType(leftName);
		//TYPE typeLeft = ct.isWhatType(leftName);
		Type typeRight = ct.isWhatType(rightName);
		//TYPE typeRight = ct.isWhatType(rightName);

		if (typeLeft != typeRight)throw CatalogError("unmatched type");
		bool b1 = ct.isType(leftName, typeLeft), b2 = ct.isType(rightName, typeRight);

		exp.leftOperand.isAttribute = !b1;
		exp.rightOperand.isAttribute = !b2;
		//if they are all normal data
		if (b1&&b2) {
			if (compareExp(leftName, rightName, typeLeft, exp.op)) {
			}
			else doNothingFlag = true;
		}
		else {
			if (b1&&typeLeft == CHAR)
				exp.leftOperand.operandName = string(leftName.begin() + 1, leftName.end() - 1);
			if (b2&&typeRight == CHAR)
				exp.rightOperand.operandName = string(rightName.begin() + 1, rightName.end() - 1);
			if (b1&&!b2)
				exp.swap();
			expTmp.push_back(exp);
		}
	}
	exps = expTmp;

	//delete pcb;
}

QueryResult SelectBlock::execute()
{
	if (doNothingFlag)return QueryResult();

	auto api = API::getInstance();
	//auto api = API::getAPIPtr();

	//if (exps.size() == 0) {

	//	QueryResult ret = api->Select(attributes, tableName);
	//	//api->selectValuesCmd(attributes, tableName, ip->getRecordBuffer());

	//	

	//	return ret;
	//}
	//else {

		QueryResult ret = api->Select(attributes, tableName, exps);
		//api->selectValuesCmd(attributes, tableName, exps, ip->getRecordBuffer());

		

		return ret;
	//}
}

void SelectBlock::print()
{
	cout << "select ";
	if (star)cout << "* ";\
	else {
		for (auto & s : attributes) {
			cout << s << ",";
		}
	}
	cout << "from " << tableName << " ";
	if (exps.size() != 0) {
		for (auto& exp : exps) {
			cout << "[" << exp.leftOperand.operandName << " " << exp.op << " " << exp.rightOperand.operandName << "]";
		}
	}
	cout << endl;
}

bool CheckType::isType(const std::string & s, Type type) {
	switch (type) {
	case INT:
		return isInt(s);
		break;
	case FLOAT:
		return isFloat(s);
		break;
	case CHAR:
		return isString(s);
		break;
	default:return false;
	}
}

bool CheckType::isString(const std::string & s) {
	return *(s.begin()) == '\''&&*(s.end() - 1) == '\'';
}

bool CheckType::isFloat(const std::string & s) {
	if (std::regex_match(s, std::regex("^-?([1-9]\\d*\\.\\d*|0\\.\\d*)$")))
		return true;
	else return false;
}

bool CheckType::isInt(const std::string & s) {
	if (std::regex_match(s, std::regex("^-?([1-9]\\d*|0)$")))
		return true;
	return false;
}

bool CheckType::isAttribute(const std::string & s)
{
	return table.hasAttribute(s);
	//return table.isAttribute(s);
}

//TYPE CheckType::isWhatType(const std::string & s)
Type CheckType::isWhatType(const std::string & s)
{	
	if (isInt(s))
		return INT;
	if (isString(s)) 
		return CHAR;
	if (isFloat(s))
		return FLOAT;

	vector<Attribute> v = table.getAttributes();
	//vector<Data> v = table.getTableVec();

	auto iter = std::find_if(v.begin(), v.end(), [=](const Attribute& d) {
	//auto iter = std::find_if(v.begin(), v.end(), [=](const Data& d) {

		return d.getAttributeName() == s;
		//return d.getAttribute() == s;

	});
	if(iter == v.end())
		throw CatalogError("no type matches " + s);
	return iter->getType();
}

//bool compareExp(const std::string& left, const std::string& right, TYPE type, OPERATOR op)
bool compareExp(const std::string& left, const std::string& right, Type type, OPERATOR op)
{
	switch (type) {
	case INT:
		return compareFunc<int>(op)(stoi(left),stoi(right));
	case FLOAT:
		return compareFunc<float>(op)(stof(left), stof(right));
	case CHAR:
		return compareFunc<std::string>(op)(left, right);
	}

	return false;
}

QueryResult execBlock::execute()
{
	string file = fileName;
	
	ip->gettPtr() = ip->getVsb()[0];
	ip->getVsb().clear();
	ip->executeFile(fileName);
	ip->getVsb().push_back(ip->gettPtr());
	return QueryResult(); // ¿Õ½á¹û
}
