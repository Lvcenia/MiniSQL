#include "Generals.h"
#include <iostream>




OPERATOR reverseOperator(OPERATOR op)
{
	switch (op) {
	case GREATER: return LESS; break;
	case LESS:return GREATER; break;
	case EQUAL: return EQUAL; break;
	case NOTEQUAL:return NOTEQUAL; break;
	case GREATER_AND_EQUAL:return LESS_AND_EQUAL; break;
	case LESS_AND_EQUAL:return GREATER_AND_EQUAL; break;
	default:return UNDEFINED;
	}
}
void Expression::swap()
{
	std::swap(this->leftOperand, this->rightOperand);
	op = reverseOperator(op);
}

OPERATOR stringToOperator(std::string s)
{
	if (s == "==")
		return EQUAL;
	else if (s == ">")
		return GREATER;
	else if (s == "<")
		return LESS;
	else if (s == "!=" || s == "<>")
		return NOTEQUAL;
	else if (s == ">=")
		return GREATER_AND_EQUAL;
	else if (s == "<=")
		return LESS_AND_EQUAL;
	else return OPERATOR::UNDEFINED;
}


Type stringToType(std::string s)
{
	if (s == "int")
		return INT;
	else if (s == "float")
		return FLOAT;
	else if (s == "char")
		return CHAR;
	else
		return Type::UNDEFINEDTYPE;
}


RecordBuffer::RecordBuffer()
{
}

RecordBuffer::RecordBuffer(const vector<string>& content)
{
	this->content = vector<string>(content);
}

RecordBuffer::~RecordBuffer()
{
}

void RecordBuffer::output()
{
	for (auto info : content) {
		cout << info << endl;
	}
}

void RecordBuffer::addContent(string info)
{
	this->content.push_back(info);
}

void RecordBuffer::clearContent()
{
	this->content.clear();
}

const vector<string>& RecordBuffer::getContent()
{
	return this->content;
}

int RecordBuffer::getLineCount()
{
	return content.size();
}

Attribute::Attribute()
{
}

Attribute::Attribute(const std::string & name, Type type, int length, bool isUnique, bool isPrimary, int offset)
{
	this->name = name;
	this->type = type;
	this->length = length;
	this->isUnique = isUnique;
	this->isPrimaryKey = isPrimary;
	this->offset = offset;
}

Attribute::Attribute(const AttributeInfo & info)
{
	this->name = string(info.name);
	this->type = info.type;
	this->length = info.length;
	this->isUnique = info.isUnique;
	this->isPrimaryKey = info.isPrimaryKey;
	this->offset = info.offset;
}

Attribute::~Attribute(){}

string Attribute::getAttributeName()
{
	return name;
}

void Attribute::setAttributeName(string name)
{
	this->name = name;
}

Type Attribute::getType()
{
	return type;
}

void Attribute::setType(Type type)
{
	this->type = type;
	if (type == INT)
		this->length = sizeof(int);
	else if (type == FLOAT)
		this->length = sizeof(float);
}

int Attribute::getLength()
{
	return length;
}

void Attribute::setLength(int length)
{
	this->length = length;
}

bool Attribute::isUniqueKey()
{
	return isUnique;
}

void Attribute::setUnique(bool flag)
{
	this->isUnique = flag;
}

bool Attribute::isPrimary()
{
	return isPrimaryKey;
}

void Attribute::setPrimary(bool value)
{
	this->isPrimaryKey = value;
}

void Attribute::setOffset(int off)
{
	this->offset = off;
}

int Attribute::getOffset()
{
	return offset;
}

AttributeInfo Attribute::GetInfo()
{
	AttributeInfo info;
	int i;
	for (i = 0; i < (int)this->name.length(); i++) {
		info.name[i] = this->name[i];
	}
	info.name[i] = '\0';
	info.length = length;
	info.offset = offset;
	info.type = type;
	info.isPrimaryKey = isPrimaryKey;
	info.isUnique = isUnique;

	return info;
}

Table::Table()
{
}

Table::Table(string tableName, const vector<Attribute>& attributes)
{
	this->recordCount = 0;
	this->recordLength = 0;
	this->name = tableName;
	this->attributes = vector<Attribute>(attributes);
	for (auto attribute : attributes) {
		this->recordLength += attribute.getLength();
	}
	this->rowLength = attributes.size();
	this->primaryKeyIndex = -1;
	for (int i = 0; i < rowLength; i++) {
		if (this->attributes[i].isPrimary()) {
			this->primaryKeyIndex = i;
			break;
		}
	}
}

Table::Table(const TableHeader & header)
{
	this->recordCount = header.recordCount;
	this->name = string(header.tableName);
	this->primaryKeyIndex = header.primaryKeyIndex;
	this->rowLength = header.rowLength;
	this->recordLength = header.recordLength;
	for (int i = 0; i < rowLength; i++) {
		this->attributes.push_back(Attribute(header.attributes[i]));
	}
}

Table::~Table()
{
}

string Table::getTableName()
{
	return name;
}

void Table::setTableName(string tableName)
{
	this->name = tableName;
}

const vector<Attribute>& Table::getAttributes()
{
	// TODO: 在此处插入 return 语句
	return this->attributes;
}

void Table::setAttributes(const vector<Attribute>& attributes)
{
	this->attributes = vector<Attribute>(attributes);
}

int Table::getPrimaryKeyIndex()
{
	return primaryKeyIndex;
}

void Table::setPrimaryKeyIndex(int primaryKeyIndex)
{
	this->primaryKeyIndex = primaryKeyIndex;
}

int Table::getRowLength()
{
	return rowLength;
}

void Table::addAttribute(Attribute & attribute)
{
	this->attributes.push_back(attribute);
}

bool Table::hasAttribute(string attributeName)
{
	for (auto attribute : attributes) {
		if (attribute.getAttributeName() == attributeName) {
			return true;
		}
	}
	return false;
}

const Attribute& Table::getAttribute(string attributeName)
{
	for (auto attribute : attributes) {
		if (attribute.getAttributeName() == attributeName) {
			return attribute;
		}
	}
	throw exception("Attribute Not Found!");
}

TableHeader Table::GetTableHeader()
{
	TableHeader header;
	int i;
	for (i = 0; i < (int)name.length(); i++) {
		header.tableName[i] = name[i];
	}
	header.tableName[i] = '\0';
	header.primaryKeyIndex = primaryKeyIndex;
	header.rowLength = rowLength;
	header.recordCount = recordCount;
	header.recordLength = recordLength;
	for (i = 0; i < rowLength; i++) {
		header.attributes[i] = attributes[i].GetInfo();
	}
	return header;
}

int Table::getRecordLength()
{
	return recordLength;
}

int Table::getRecordCount()
{
	return recordCount;
}
