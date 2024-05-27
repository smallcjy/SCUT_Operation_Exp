#include<stdio.h>
#include<sys/time.h>
#include<time.h>
#include<signal.h>
#include<unistd.h>
#include <stdlib.h>

#define  TRUE  1;
#define  FALSE  0;
//定义PCB结构体
unsigned int UNUSEDPID  = 0;
//定义时间片轮转时间
#define  TIMER  1; //秒

struct PCB
{
    unsigned int pid;
    struct PCB* nextProcess;
    time_t arrivalTime;
    time_t runTime; 
    time_t timeout;
    int isRunedState;
    float zzTime;
};

struct PCB* new(time_t run) {
    struct PCB* newPCB = (struct PCB*)malloc(sizeof(struct PCB));
    if (newPCB == NULL) {
        printf("Failed to allocate memory for new PCB\n");
        return NULL;
    }
    // 初始化newPCB的成员
    newPCB->pid = UNUSEDPID++;
    newPCB->arrivalTime = time(NULL);
    newPCB->runTime = run;
    newPCB->timeout = newPCB->arrivalTime + run;
    newPCB->isRunedState = FALSE;
    newPCB->nextProcess = NULL;
    newPCB->zzTime = 0;
    return newPCB;
}

// 定义队列节点
typedef struct Node {
    struct PCB* data;
    struct Node* next;
} Node;

// 定义队列
typedef struct List {
    Node* front;  // 指向队列前端的指针
    Node* rear;  // 指向队列后端的指针
} List;

// 创建一个新的队列
List* createList() {
    List* q = (List*)malloc(sizeof(List));
    q->front = q->rear = NULL;
    return q;
}

// 将一个PCB添加到队列的后端
void pushList(List* q, struct PCB* data) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
    if (q->front == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

// 从队列的前端移除一个PCB，并返回它
struct PCB* pollList(List* q) {
    if (q->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }
    Node* temp = q->front;
    struct PCB* item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
    return item;
}

//时间片轮转调度算法中初始化进程组
void setProcessInRR(int num, List* l, int timeRange) {
    srand(time(NULL));
    for(int i =0;i<num;i++) {
        time_t runTime = (time_t)rand() % timeRange + 1;
        struct PCB* newPCB =  new(runTime);
        pushList(l,newPCB);
    }
}


// 定义队列
typedef struct Queue {
    Node* front;
    Node* rear;
    int size;
} Queue;

// 初始化队列
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->size=0;
    return q;
}

// 入队操作，按照timeout从小到大排序
void enqueue(Queue* q, struct PCB* data) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
    if (q->front == NULL) {
        q->front = q->rear = temp;
        return;
    }
    if (q->front->data->timeout > data->timeout) {
        temp->next = q->front;
        q->front = temp;
    } else {
        Node* current = q->front;
        while (current->next != NULL && current->next->data->timeout < data->timeout) {
            current = current->next;
        }
        temp->next = current->next;
        current->next = temp;
        if (current == q->rear) {
            q->rear = temp;
        }
    }
    q->size++;
}

// 出队操作
struct PCB* dequeue(Queue* q) {
    if (q->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }
    Node* temp = q->front;
    struct PCB* item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
    q->size--;
    return item;
}



//初始化进程组,并推入调度队列（在最短作业优先算法中调用）
void setProcess(int num, Queue* q) {
    srand(time(NULL));
    for(int i =0;i<num;i++) {
        time_t runTime = (time_t)rand() % 10 + 1;
        struct PCB* newPCB =  new(runTime);
        enqueue(q, newPCB);
    }
}

//开始模拟最短作业优先方法
void SJF() {
    printf("********开始模拟最短优先作业算法********\n");
    Queue* process_schedule_queue = createQueue();
    //模拟5个进程
    setProcess(5, process_schedule_queue);
    for(;;) {
        if(process_schedule_queue->front==NULL) break;
        if(process_schedule_queue->front->data->timeout < time(NULL)) {
            //进程运行完成
            process_schedule_queue->front->data->isRunedState = TRUE;
            struct PCB* runedPCB = dequeue(process_schedule_queue);
            printf("进程%d运行完成，运行完成时间为%d\n", runedPCB->pid, (int)runedPCB->timeout);
        }else continue;
    }
    printf("********模拟最短优先作业算法完成********\n");
}

//设置一秒的定时器
void setTimer() {
    struct itimerval timer;
    timer.it_value.tv_sec = TIMER;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TIMER;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

List* process_schedule_list;

//设置用于时间片轮转调度的定时器和触发函数
void handle(int sig) {
    if(sig == SIGALRM) {
        struct PCB* pcb = pollList(process_schedule_list);
        //查看进程运行是否完成
        if(pcb->timeout<time(NULL)) {
            pcb->isRunedState = TRUE;
            printf("进程%d运行完成，运行完成时间为%d\n", pcb->pid, (int)pcb->timeout);
        } else {
            //进程未完成，更新timeout时间重新加入定时器列表中
            pcb->runTime -= (time_t)TIMER;
            pcb->arrivalTime = time(NULL);
            pcb->timeout = pcb->arrivalTime + pcb->runTime;
            pushList(process_schedule_list,pcb);
        }
    }

}
//开始模拟时间片轮转调度算法
void RR(){
    printf("********开始模拟时间片轮转调度作业算法********\n");
    
    process_schedule_list = createList();
    
    //模拟5个进程
    setProcessInRR(5, process_schedule_list, 10);
    signal(SIGALRM, handle);
    setTimer();
    while(1) {
        if(process_schedule_list->front==NULL) break;
        pause();
    }
    printf("********模拟时间片轮转调度作业算法完成！********\n");
}

int main()
{
    SJF();
    RR();
}

