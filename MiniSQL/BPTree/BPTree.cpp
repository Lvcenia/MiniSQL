#include "BPTree.h"
#include "BPTLeafNode.h"
#include "BPTNoneLeafNode.h"

bool BPTree::CompInt(const string& a, const string& b)
{
	return atoi(a.c_str()) < atoi(b.c_str());
}

bool BPTree::CompChar(const string& a, const string& b)
{
	return a.compare(b);
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
	case INT:
		Comp = CompInt;
		break;
	case CHAR:
		Comp = CompChar;
		break;
	case FLOAT:
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
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

void BPTree::Delete(string key)
{
	if (root != NULL) {
		try
		{
			root->DeleteKey(key);
		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
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
		cout << e.what() << endl;
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
		cout << e.what() << endl;
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
