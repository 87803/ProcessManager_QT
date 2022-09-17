#include "ProcessManager.h"

PCB::PCB(int id, string name, int priority, int needTime, double memorySize)
{
    this->id = id;
    this->name = name;
    this->priority = priority;
    this->needTime = needTime;
    this->memorySize = memorySize;
    this->runTime = 0;
    this->state = 0;
    this->next = NULL;
    this->pt = NULL;
}

PCB::~PCB()
{
    delete pt;
}

ProcessManager::ProcessManager()
{
    id = 0;
    runningProcNum = 0;
    message = "";
    createList = NULL;
    readyList = NULL;
    blockList = NULL;
    runningList = NULL;
}

bool ProcessManager::create(string name, int priority, int needTime, double memorySize)
{
    if (name == "" || needTime <= 0 || memorySize <= 0)
    {
        message = "进程[name: " + name + "]未提交, 请提供正确的参数\n" + message;
        return false;
    }
    PCB *newPCB = new PCB(this->id++, name, priority, needTime, memorySize);
    if (!newPCB)
        return false;
    listInsert(this->createList, newPCB);
    message = "进程[name: " + name + "]已提交, 待分配内存...\n"+message;
    createToReady(); //调用函数为进程分配内存使之进入就绪状态
    schedule();
    return true;
}

bool ProcessManager::createToReady()
{
    PCB* temp = this->createList;
    while (temp)
    {
        if ((ceil(temp->memorySize / mem.pageSize) + 1.0) <= mem.pageNum)
        {
            PCB* temp2 = temp;
            temp = temp->next;
            int m1 = mem.getMemory();
            int* m2 = mem.getMemory(ceil(temp2->memorySize / mem.pageSize));
            temp2->pt = new pageTable(m1, m2); //为进程创建页表并分配内存
            temp2->state = 1;
            temp2 = listDelete(createList, temp2->id);
            listInsert(readyList, temp2);
            message = "进程[PID: " + std::to_string(temp2->id) + "]已就绪, 待调度...\n" + message;
        }
        else
            temp = temp->next;
    }
    return true;
}

bool ProcessManager::block(int pid)
{
    PCB *searchPro = listDelete(this->runningList, pid); //从运行队列中删除该进程
    if (searchPro)
    {
        searchPro->state = 2;						  //置为阻塞态
        listInsert(this->blockList, searchPro); //加入阻塞队列
        runningProcNum--;
        message = "进程[PID: " + std::to_string(searchPro->id) + "]已阻塞\n" + message;
        schedule();
        return true;
    }
    message = "进程[PID: " + std::to_string(pid) + "]不在运行队列中, 无法阻塞\n" + message;
    return false;
}

bool ProcessManager::wakeup(int pid)
{
    PCB *searchPro = listDelete(this->blockList, pid); //从阻塞队列中删除该进程
    if (searchPro)
    {
        searchPro->state = 1;						  //置为就绪态
        listInsert(this->readyList, searchPro); //加入就绪队列
        message = "进程[PID: " + std::to_string(searchPro->id) + "]已唤醒, 待调度...\n" + message;
        schedule();
        return true;
    }
    message = "进程[PID: " + std::to_string(pid) + "]不在阻塞队列中, 无法唤醒\n" + message;
    return false;
}

bool ProcessManager::kill(int pid)
{
    //寻找该进程所处队列
    PCB *searchPro = this->listDelete(this->createList, pid); //从创建队列中删除该进程
    if (searchPro)											   //在创建队列中，直接删除即可
    {
        message = "进程[PID: " + std::to_string(searchPro->id) + "]已杀死\n" + message;
        delete searchPro;
        return true;
    }
    searchPro = this->listDelete(this->runningList, pid); //从运行队列中删除该进程
    if (searchPro)
        goto L1;
    searchPro = this->listDelete(this->readyList, pid); //从就绪队列中删除该进程
    if (searchPro)
        goto L1;
    searchPro = this->listDelete(this->blockList, pid); //从阻塞队列中删除该进程
    if (searchPro)
        goto L1;
    message = "进程[PID: " + std::to_string(pid) + "]未找到\n" + message;
    return false;
L1:
    if (searchPro->state == 3)
        runningProcNum--;
    mem.releaseMomory(searchPro->pt->address);
    mem.releaseMomory(searchPro->pt->tableItem);
    message = "进程[PID: " + std::to_string(searchPro->id) + "]已杀死\n" + message;
    //this->pageNum += (ceil(this->createList->memorySize / this->pageSize) + 1); //收回分配的页面
    delete searchPro;
    this->createToReady(); //调用函数为进程分配内存使之进入就绪状态
    schedule();//释放PCB
    return true;
}

bool ProcessManager::schedule()
{
    if (runningProcNum == 4)
        return false;
    while (runningProcNum < 4 && readyList) //就绪调入运行
    {
        PCB *temp = listDelete(readyList, getHighestPriorityProc()); //从就绪队列中删除最高优先级的进程
        temp->state = 3;
        runningProcNum++;
        listInsert(runningList, temp); //将其加入运行队列
        message = "进程[PID: " + std::to_string(temp->id) + "]已调入, 运行中...\n" + message;
    }
    return true;
}

void ProcessManager::procRunning()
{
    PCB *temp = this->runningList;
    while (temp)
    {
        temp->runTime++;
        if (temp->runTime >= temp->needTime) //运行正常结束
        {
            PCB* temp2 = temp;
            temp = temp->next;
            mem.releaseMomory(temp2->pt->address);
            mem.releaseMomory(temp2->pt->tableItem);
            //this->pageNum += (ceil(this->createList->memorySize / this->pageSize) + 1); //收回分配的页面
            PCB *del = listDelete(this->runningList, temp2->id);
            message = "进程[PID: " + std::to_string(temp2->id) + "]已正常结束运行\n" + message;
            delete del;
            runningProcNum--;
        }
        else
            temp = temp->next;
    }
    this->createToReady(); //调用函数为进程分配内存使之进入就绪状态
    schedule();
}

int ProcessManager::getHighestPriorityProc()
{
    PCB *temp = this->readyList;
    PCB *highest = this->readyList;
    while (temp)
    {
        if (temp->priority > highest->priority)
            highest = temp;
        temp = temp->next;
    }
    if (highest)
        return highest->id;
    else
        return -1;
}

bool ProcessManager::listInsert(PCB *&list, PCB *node)
{
    if (list == NULL)
        list = node;
    else
    {
        PCB *tail = list;
        while (tail->next)
            tail = tail->next;
        tail->next = node;
    }
    return true;
}

PCB *ProcessManager::listDelete(PCB *&list, int pid)
{
    if (list && list->id == pid) //头结点就是要找的进程
    {
        PCB *temp = list;
        list = list->next;
        temp->next = NULL;	//该结点从中删除时，要删除原来的关系
        return temp;
    }
    else if (list) //得到待找进程的前驱
    {
        PCB *searchPro = list;
        while (searchPro->next && searchPro->next->id != pid) //找到待找进程的前驱
        {
            searchPro = searchPro->next;
        }
        if (searchPro->next && searchPro->next->id == pid) //找到该进程
        {

            PCB *temp = searchPro->next;
            searchPro->next = temp->next;
            temp->next = NULL;
            return temp;
        }
        return NULL;
    }
    return NULL;
}

const PCB* ProcessManager::getCreateList()
{
    return createList;
}

const PCB* ProcessManager::getReadyList()
{
    return readyList;
}

const PCB* ProcessManager::getBlockList()
{
    return blockList;
}

const PCB* ProcessManager::getRunningList()
{
    return runningList;
}

int ProcessManager::getPageNum()
{
    return mem.pageNum;
}

int ProcessManager::getRunningProcNum()
{
    return runningProcNum;
}

Memory::Memory()
{
    pageSize = 1024;
    pageNum = 32;
}

int Memory::getMemory()
{
    for (int i = 0; i < 32; i++)
    {
        if (memoryInfo[i] == 0)
        {
            memoryInfo[i] = 1;
            pageNum--;
            return i;
        }
    }
    return -1;
}

int *Memory::getMemory(int num)
{
    int *temp = new int[num];
    int j = 0;
    for (int i = 0; j < num && i < 32; i++)
    {
        if (memoryInfo[i] == 0)
        {
            memoryInfo[i] = 1;
            temp[j++] = i;
        }
    }
    pageNum -= num;
    return temp;
}

void Memory::releaseMomory(int i)
{
    memoryInfo[i] = 0;
    pageNum++;
}

void Memory::releaseMomory(int *table)
{
    int length = int(_msize(table)) / sizeof(table[0]);//动态数组的长度
    for (int i = 0; i < length; i++)
        memoryInfo[table[i]] = 0;
    pageNum += length;
}

pageTable::pageTable(int add, int *table)
{
    address = add;
    tableItem = table;
}

pageTable::~pageTable()
{
    delete[] tableItem;
}
