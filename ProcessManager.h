#pragma once
#include <string>
#include <bitset>
using namespace std;

class pageTable
{
public:
    pageTable(int, int*);
    ~pageTable();
    //进程的一张页表
    int address;	//该页表所在的物理块
    int* tableItem; //页表项，存储分配的物理块号
};

class PCB
{
public:
    PCB(int id, string name, int priority, int needTime, double memorySize);
    ~PCB();
    int id;			//进程标识符
    string name;	//进程名
    int state;		//进程状态 0创建 1就绪 2阻塞 3运行
    int priority;	//优先级
    int runTime;	//运行时间
    int needTime;	//需要时间
    double memorySize; //所需内存大小
    pageTable *pt;	//该进程的页表
    PCB *next;
};

class Memory
{
public:
    Memory();
    int getMemory();
    int *getMemory(int);
    void releaseMomory(int);
    void releaseMomory(int *);
    int pageSize;
    int pageNum;

private:
    bitset<32> memoryInfo;
};

class ProcessManager
{
public:
    ProcessManager();

    bool create(string, int, int, double);
    bool block(int);
    bool wakeup(int);
    bool kill(int);
    bool schedule();
    void procRunning();
    int getHighestPriorityProc();
    bool listInsert(PCB *&, PCB *);
    PCB *listDelete(PCB *&, int);

    //用于GUI调用
    const PCB* getCreateList();
    const PCB* getReadyList();
    const PCB* getBlockList();
    const PCB* getRunningList();
    int getPageNum();
    int getRunningProcNum();
    string message;	//用于GUI显示提示信息
private:
    bool createToReady(); //该函数在create或有内存释放时调用
    PCB *createList;
    PCB *readyList;
    PCB *blockList;
    PCB *runningList;
    int id; //用于分配PID
    int runningProcNum;
    Memory mem;
};
