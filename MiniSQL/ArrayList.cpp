#include "ArrayList.h"

ArrayList::ArrayList() :
	head(0), tail(BLOCKNUM - 1)
{
	for (int i = 0; i < BLOCKNUM; i++) {
		arraylist[i].last = i - 1;
		arraylist[i].next = i + 1;
	}
	arraylist[tail].next = -1;
}


ArrayList::~ArrayList()
{
}
/*
	获取队列头部编号
*/
int ArrayList::getHead()
{
	return head;
}

/*
	作用:将指定编号移到队列尾部
*/
void ArrayList::moveToTail(int index)
{
	if (index == tail)
		return;
	int last = arraylist[index].last;
	int next = arraylist[index].next;

	arraylist[tail].next = index;
	arraylist[last].next = next;
	arraylist[next].last = last;
	arraylist[index].next = -1;
	arraylist[index].last = tail;
	tail = index;
	if (index == head)
		head = next;
}