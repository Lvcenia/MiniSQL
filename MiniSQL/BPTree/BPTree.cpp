#include "BPTree.h"
#include "BPTLeafNode.h"
#include "BPTNoneLeafNode.h"
#define BLOCKSIZE 4096

bool BPTree::CompInt(const string& a, const string& b)
{
	return atoi(a.c_str()) < atoi(b.c_str());
}

bool BPTree::CompChar(const string& a, const string& b)
{
	return a < b;
}

bool BPTree::CompFloat(const string& a, const string& b)
{
	return atof(a.c_str()) < atof(b.c_str());
}

BPTree::BPTree(int m, Type type)
{
	root = NULL;
	maxKeyCount = m;
	this->type = type;
	switch (type)
	{
	case Int:
		Comp = CompInt;
		break;
	case Char:
		Comp = CompChar;
		break;
	case Float:
		Comp = CompFloat;
		break;
	default:
		Comp = NULL;
		throw exception("Type Undefined!");
		break;
	}
}

BPTree::BPTree(int m, Type type, int attributeLength, int offsetInRecord) : BPTree(m, type)
{
	length = attributeLength;
	offset = offsetInRecord;
}

BPTree::BPTree(string name, Attribute & attributeInfo, string tableName)
{
	root = NULL;
	this->name = name;
	this->attributeName = attributeInfo.getAttributeName();
	this->tableName = tableName;
	this->type = attributeInfo.getType();
	switch (type)
	{
	case Int:
		Comp = CompInt;
		break;
	case Char:
		Comp = CompChar;
		break;
	case Float:
		Comp = CompFloat;
		break;
	default:
		Comp = NULL;
		throw exception("Type Undefined!");
		break;
	}
	this->offset = attributeInfo.getOffset();
	this->length = attributeInfo.getLength();
	this->elementCount = 0;
	this->maxKeyCount = (BLOCKSIZE - 4) / (attributeInfo.getLength() + sizeof(int)) - 1;
}

BPTree::BPTree(const IndexHeader & header) : BPTree(header.fanOut - 1, header.type)
{
	this->length = header.attributeLength;
	this->offset = header.offsetInRecord;
	this->name = string(header.indexName);
	this->attributeName = string(header.attributeName);
	this->tableName = string(header.tableName);
	this->elementCount = header.elementCount;
}

BPTree::~BPTree()
{
	delete root;
}

void BPTree::SetRoot(BPTNode* root)
{
	this->root = root;
}

void BPTree::Insert(string key, int address)
{
	if (root == NULL) {
		root = new BPTLeafNode(Comp, maxKeyCount, this);
	}
	try {
		root->AddKey(key, address);
		elementCount++;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}

void BPTree::Delete(string key)
{
	if (root != NULL) {
		try
		{
			root->DeleteKey(key);
			elementCount--;
		}
		catch (const std::exception& e)
		{
			throw e;
		}
	}
}

int BPTree::FindAddress(string key)
{
	try
	{
		return root->FindKey(key);
	}
	catch (const std::exception& e)
	{
		return -1;
	}
}

int BPTree::GetMaxKeyCount()
{
	return maxKeyCount;
}

int BPTree::GetOffset()
{
	return offset;
}

int BPTree::GetLength()
{
	return length;
}

int BPTree::GetElementCount()
{
	return elementCount;
}

Type BPTree::GetType()
{
	return type;
}

BPTLeafNode * BPTree::GetLeafWithKey(string key)
{
	try
	{
		return root->FindLeaf(key);
	}
	catch (const std::exception& e)
	{
		return NULL;
	}
}

BPTLeafNode * BPTree::GetFirstLeaf()
{
	if (root == NULL || root->GetKeyCount() == 0) {
		return NULL;
	}
	return root->FindFirstLeaf();
}

const IndexHeader & BPTree::GetHeader()
{
	IndexHeader header;
	header.attributeLength = length;
	header.elementCount = elementCount;
	header.fanOut = maxKeyCount + 1;
	header.offsetInRecord = offset;
	header.type = type;
	int i;
	for (i = 0; i < (int)name.length(); i++) {
		header.indexName[i] = name[i];
	}
	header.indexName[i] = '\0';
	for (i = 0; i < (int)attributeName.length(); i++) {
		header.attributeName[i] = attributeName[i];
	}
	header.attributeName[i] = '\0';
	for (i = 0; i < (int)tableName.length(); i++) {
		header.tableName[i] = tableName[i];
	}
	header.tableName[i] = '\0';

	return header;
}

void BPTree::Clear()
{
	delete root;
	root = NULL;
}

void BPTree::Debug()
{
	vector<BPTNode*> buffer;
	if (root != NULL) {
		buffer.push_back(root);
	}

	while (buffer.size() > 0)
	{
		int count = buffer.size();

		for (int i = 0; i < count; i++) {
			buffer[i]->Debug();
			if (!buffer[i]->IsLeaf()) {
				for (auto child : *((BPTNoneLeafNode*)(buffer[i]))->GetChildren()) {
					buffer.push_back(child);
				}
			}
		}

		cout << endl;
		buffer = vector<BPTNode*>(buffer.begin() + count, buffer.end());
	}
}