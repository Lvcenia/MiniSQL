#ifndef _ARRAYLIST_
#define _ARRAYLIST_

#define BLOCKSIZE 4096
#define BLOCKNUM 10

class ArrayList
{
private:
	int head, tail;
	struct {
		int last;
		int next;
	}arraylist[BLOCKNUM];
public:
	ArrayList();
	~ArrayList();
	void moveToTail(int index);
	int getHead();
};

class ArrayList
{
public:
	ArrayList();
	~ArrayList();
};
#endif // !_ARRAYLIST_



