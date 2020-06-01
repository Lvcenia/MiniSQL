#include "Generals.h"

Attribute::Attribute()
{
}

Attribute::Attribute(const std::string & name, Type type, int length, bool isUnique, bool isPrimary)
{
	this->name = name;
	this->type = type;
	this->length = length;
	this->isUnique = isUnique;
	this->isPrimaryKey = isPrimary;
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
}

int Attribute::getLength()
{
	return length;
}

void Attribute::setLength(int length)
{
	this->length = length;
}

bool Attribute::isUnique()
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

Table::Table()
{
}

Table::Table(string tableName, const vector<Attribute>& attributes)
{
	this->name = tableName;
	this->attributes = vector<Attribute>(attributes);
	this->rowLength = attributes.size();
	this->primaryKeyIndex = -1;
	for (int i = 0; i < rowLength; i++) {
		if (this->attributes[i].isPrimary()) {
			this->primaryKeyIndex = i;
			break;
		}
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
