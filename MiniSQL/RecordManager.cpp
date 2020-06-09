#include "RecordManager.h"

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
	bool(*Comp)(const string& a, const string& b);
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
	bool(*Comp)(const string& a, const string& b);
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
			conditions.push_back(Condition(name, lowInclude ? GreaterEqual : GreaterThan, lowVal));
		}
		if (highFlag) {
			conditions.push_back(Condition(name, highInclude ? LessEqual : LessThan, highVal));
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
