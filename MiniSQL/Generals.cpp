#include "Generals.h"
#include <iostream>
#include "BPTree/BPTree.h"

Condition::Condition(string attributeName, OP op, string value)
{
	this->attributeName = attributeName;
	this->op = op;
	this->value = value;
}

Condition::~Condition()
{
}

string Condition::getAttributeName()
{
	return attributeName;
}

OP Condition::getOP()
{
	return op;
}

string Condition::getValue()
{
	return value;
}

bool Condition::FitCondition(string val, Type type)
{
	bool(*Comp)(const string & a, const string & b);
	switch (type)
	{
	case INT:
		Comp = BPTree::CompInt;
		break;
	case CHAR:
		Comp = BPTree::CompChar;
		break;
	case FLOAT:
		Comp = BPTree::CompFloat;
		break;
	default:
		throw exception("Type undifined!");
		break;
	}

	switch (op)
	{
	case Equal:
		return !Comp(val, value) && !Comp(value, val);
		break;
	case NotEqual:
		return Comp(val, value) || Comp(value, val);
		break;
	case GreaterThan:
		return Comp(value, val);
		break;
	case LessThan:
		return Comp(val, value);
		break;
	case GreaterEqual:
		return !Comp(val, value);
		break;
	case LessEqual:
		return !Comp(value, val);
		break;
	default:
		return false;
		break;
	}
}

bool Condition::GetMinimalConditoins(vector<Condition>& conditions, Type type)
{
	auto name = conditions[0].attributeName;
	bool lowFlag = false;
	bool highFlag = false;
	bool equalFlag = false;
	string lowVal;
	string highVal;
	bool lowInclude = false;
	bool highInclude = false;
	bool(*Comp)(const string & a, const string & b);
	switch (type)
	{
	case INT:
		Comp = BPTree::CompInt;
		break;
	case CHAR:
		Comp = BPTree::CompChar;
		break;
	case FLOAT:
		Comp = BPTree::CompFloat;
		break;
	default:
		throw exception("Type undifined!");
		break;
	}

	vector<string> exclusives;

	for (auto condition : conditions) {
		switch (condition.op)
		{
		case Equal:
			if (equalFlag) {
				if (Comp(condition.value, lowVal) || Comp(lowVal, condition.value)) {
					return false;
				}
				continue;
			}
			if (lowFlag) {
				if (lowInclude) {
					if (Comp(condition.value, lowVal)) {
						return false;
					}
				}
				else {
					if (!Comp(lowVal, condition.value)) {
						return false;
					}
				}
			}
			if (highFlag) {
				if (highInclude) {
					if (Comp(highVal, condition.value)) {
						return false;
					}
				}
				else {
					if (!Comp(condition.value, highVal)) {
						return false;
					}
				}
			}
			equalFlag = true;
			lowVal = condition.value;
			highVal = condition.value;
			break;
		case NotEqual:
			if (equalFlag) {
				if (Comp(condition.value, lowVal) || Comp(lowVal, condition.value)) {
					continue;
				}
				return false;
			}
			exclusives.push_back(condition.value);
			break;
		case GreaterThan:
			if (equalFlag) {
				if (!Comp(condition.value, lowVal)) {
					return false;
				}
				continue;
			}
			if (highFlag) {
				if (!Comp(condition.value, highVal)) {
					return false;
				}
			}
			if (lowFlag) {
				if (lowInclude) {
					if (!Comp(condition.value, lowVal)) {
						lowVal = condition.value;
						lowInclude = false;
					}
				}
				else {
					if (Comp(lowVal, condition.value)) {
						lowVal = condition.value;
					}
				}
			}
			else {
				lowVal = condition.value;
				lowInclude = false;
			}
			break;
		case LessThan:
			if (equalFlag) {
				if (!Comp(highVal, condition.value)) {
					return false;
				}
				continue;
			}
			if (lowFlag) {
				if (!Comp(lowVal, condition.value)) {
					return false;
				}
			}
			if (highFlag) {
				if (highInclude) {
					if (!Comp(highVal, condition.value)) {
						highVal = condition.value;
						highInclude = false;
					}
				}
				else {
					if (Comp(condition.value, highVal)) {
						highVal = condition.value;
					}
				}
			}
			else {
				highVal = condition.value;
				highInclude = false;
			}
			break;
		case GreaterEqual:
			if (equalFlag) {
				if (Comp(lowVal, condition.value)) {
					return false;
				}
				continue;
			}
			if (highFlag) {
				if (highInclude) {
					if (Comp(highVal, condition.value)) {
						return false;
					}
					if (!Comp(highVal, condition.value) && !Comp(condition.value, highVal)) {
						equalFlag = true;
						lowVal = highVal;
						continue;
					}
				}
				else {
					if (!Comp(condition.value, highVal)) {
						return false;
					}
				}
			}
			if (lowFlag) {
				if (Comp(lowVal, condition.value)) {
					lowVal = condition.value;
					lowInclude = true;
				}
			}
			else {
				lowVal = condition.value;
				lowInclude = true;
			}
			break;
		case LessEqual:
			if (equalFlag) {
				if (Comp(condition.value, lowVal)) {
					return false;
				}
				continue;
			}
			if (lowFlag) {
				if (lowInclude) {
					if (Comp(condition.value, lowVal)) {
						return false;
					}
					if (!Comp(lowVal, condition.value) && !Comp(condition.value, lowVal)) {
						equalFlag = true;
						highVal = lowVal;
						continue;
					}
				}
				else {
					if (!Comp(lowVal, condition.value)) {
						return false;
					}
				}
			}
			if (highFlag) {
				if (Comp(condition.value, highVal)) {
					highVal = condition.value;
					highInclude = true;
				}
			}
			else {
				highVal = condition.value;
				highInclude = true;
			}
			break;
		default:
			return false;
			break;
		}
	}

	conditions.clear();

	if (equalFlag) {
		for (auto ex : exclusives) {
			if (!Comp(ex, lowVal) && !Comp(lowVal, ex)) {
				return false;
			}
		}

		conditions.push_back(Condition(name, Equal, lowVal));
	}
	else {
		if (lowFlag) {
			conditions.push_back(Condition(name, GreaterEqual, lowVal));
			if (!lowInclude) {
				conditions.push_back(Condition(name, NotEqual, lowVal));
			}
		}
		if (highFlag) {
			conditions.push_back(Condition(name, LessEqual, highVal));
			if (!highInclude) {
				conditions.push_back(Condition(name, NotEqual, highVal));
			}
		}
		for (auto ex : exclusives) {
			bool flagHI = false;
			bool flagLO = false;
			if (highFlag) {
				if (highInclude) {
					if (!Comp(highVal, ex)) {
						flagHI = true;
					}
				}
				else {
					if (Comp(ex, highVal)) {
						flagHI = true;
					}
				}
			}
			else {
				flagHI = true;
			}
			if (lowFlag) {
				if (lowInclude) {
					if (!Comp(ex, lowVal)) {
						flagLO = true;
					}
				}
				else {
					if (Comp(lowVal, ex)) {
						flagLO = true;
					}
				}
			}
			else {
				flagLO = true;
			}
			if (flagHI && flagLO) {
				conditions.push_back(Condition(name, NotEqual, ex));
			}
		}
	}

	return true;
}

bool Condition::FitAllConditions(const vector<Condition>& conditions, Type type, string value)
{
	for (auto condition : conditions) {
		if (!condition.FitCondition(value, type)) {
			return false;
		}
	}
	return true;
}


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

string typeToString(Type t)
{
	switch (t)
	{
	case INT:return "int";
		break;
	case CHAR:return "char";
		break;
	case FLOAT:return "float";
		break;
	case UNDEFINEDTYPE:return "undefined";
		break;
	default:
		break;
	}
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
	if (content.empty()) {
		std:cout << "\n" << "No Records." << std::endl;
	}
	else {
		for (auto& info : content) {
			cout << info << endl;
		}
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

string Attribute::getAttributeName() const
{
	return name;
}

void Attribute::setAttributeName(string name)
{
	this->name = name;
}

Type Attribute::getType() const
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

int Attribute::getLength() const
{
	return length;
}

void Attribute::setLength(int length)
{
	this->length = length;
}

bool Attribute::isUniqueKey() const
{
	return isUnique;
}

void Attribute::setUnique(bool flag)
{
	this->isUnique = flag;
}

bool Attribute::isPrimary() const
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

int Attribute::getOffset() const
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
	this->attrCount = attributes.size();
	this->primaryKeyIndex = -1;
	for (int i = 0; i < attrCount; i++) {
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
	this->attrCount = header.attrCount;
	this->recordLength = header.recordLength;
	for (int i = 0; i < attrCount; i++) {
		this->attributes.push_back(Attribute(header.attributes[i]));
	}
}

Table::~Table()
{
}

string Table::getTableName() const
{
	return name;
}

void Table::setTableName(string tableName)
{
	this->name = tableName;
}

const vector<Attribute>& Table::getAttributes() const
{
	// TODO: 在此处插入 return 语句
	return this->attributes;
}

void Table::setAttributes(const vector<Attribute>& attributes)
{
	this->attributes = vector<Attribute>(attributes);
	this->attrCount = attributes.size();
}

int Table::getPrimaryKeyIndex() const
{
	return primaryKeyIndex;
}

void Table::setPrimaryKeyIndex(int primaryKeyIndex)
{
	this->primaryKeyIndex = primaryKeyIndex;
}

int Table::getAttributeCount() const
{
	return attrCount;
}

void Table::addAttribute(Attribute & attribute)
{
	this->attributes.push_back(attribute);
	this->attrCount++;
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

const Attribute& Table::getAttribute(string attributeName) const
{
	for (auto attribute : attributes) {
		if (attribute.getAttributeName() == attributeName) {
			return attribute;
		}
	}
	string buff = "Attribute \"";
	buff.append(attributeName);
	buff.append("\" Not Found!");
	throw exception(buff.c_str());
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
	header.attrCount = attrCount;
	header.recordCount = recordCount;
	header.recordLength = recordLength;
	for (i = 0; i < this->getAttributes().size(); i++) {
		header.attributes[i] = attributes[i].GetInfo();
	}
	return header;
}

int Table::getRecordLength() const
{
	return recordLength;
}

int Table::getRecordCount() const
{
	return recordCount;
}
