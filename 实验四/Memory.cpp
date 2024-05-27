#include<iostream>
#include<queue>
#include<vector>
#include <iomanip>
using namespace std;

//定义虚页结构
struct virtualPage
{
    /* data */
    int pn; //虚页号
    int pfn; //转换后的实页号
    int time;
};

//定义实页结构
struct physicalPage
{
    /* data */
    int pn; //虚页号
    int pfn; //转换后的实页号
};

//定义虚拟储存空间
class virtualSpace {
private:
    vector<virtualPage> page;
    int length = 0;
public:
    friend class physicalSpace;
    virtualSpace(int num) {
        for (int i = 0; i< num; i++) {
            virtualPage newv;
            newv.pn = i;
            newv.pfn = -1;
            page.push_back(newv);
        }
        this->length = num;
    }
    void Print() {
        cout<<"虚拟储存空间有： ";
        for (int i = 0;i<this->length; i++) {
            cout<<"虚页"<<page[i].pn<<" ";
        }
        cout<<endl;
    }
    ~virtualSpace() {}
};

//定义实页空间
class physicalSpace{
private:
    queue<physicalPage*> busyPages;
    queue<physicalPage*> freePages;
public:
    friend class virtualSpace;
    physicalSpace(int num){
        //初始化free列表
        for(int i=0; i<num; i++){
            physicalPage* page = new physicalPage;
            page->pn = -1;
            page->pfn = i;
            freePages.push(page);
        }
    }

    void getSpace() {
        queue<physicalPage*> copy_b_pages = busyPages;
        queue<physicalPage*> copy_f_pages = freePages;
        //打印busyPages和freePage中的虚页号
        cout << "当前物理内存的状态是： " << endl;
        if(!copy_b_pages.empty()) {
            cout<<"当前使用的物理内存的虚页号为：| ";
            while (!copy_b_pages.empty())
            {
                /* code */
                cout<<copy_b_pages.front()->pn<< " | ";
                copy_b_pages.pop();
            }
            
        }

        if(!copy_f_pages.empty()) {
            cout<<"当前未使用的物理内存的实页号为：| ";
            while (!copy_f_pages.empty())
            {
                /* code */
                cout<<copy_f_pages.front()->pfn<< " | ";
                copy_f_pages.pop();
            }
            
        }
        cout << endl;
        cout << endl
             << endl;
    }

    //实现先进先出,返回hit次数
    int FIFO(vector<int> sequence, const int len, virtualSpace a) {
        int count = 0;
        for(int i = 0; i<len; i++){
            if(a.page[sequence[i]].pfn == -1) {
                //not hit
                if(freePages.size()==0) {
                    //开始置换
                    physicalPage* replacePage = busyPages.front();
                    busyPages.pop();
                    replacePage->pn = a.page[sequence[i]].pn;
                    a.page[sequence[i]].pfn = replacePage->pfn;
                    busyPages.push(replacePage);
                }
                else{
                    freePages.front()->pn = a.page[sequence[i]].pn;
                    a.page[sequence[i]].pfn = freePages.front()->pfn;
                    busyPages.push(freePages.front());
                    freePages.pop();
                }
            }else {
                //hit
                count++;
            }
            getSpace();
        }
        return count;
    }
    //在busyPages找到最小的访问时间的虚页的实页号
    int findOldPage(virtualSpace a) {
        queue<physicalPage*> copy = this->busyPages;
        int time = a.page[copy.front()->pn].time;
        int oldPfn = a.page[copy.front()->pn].pfn;
        copy.pop();
        while(!copy.empty()){
            if(a.page[copy.front()->pn].time<time) {
                time = a.page[copy.front()->pn].time;
                oldPfn = a.page[copy.front()->pn].pfn;
                copy.pop();
            }
            else {
                copy.pop();
            }
        }
        return oldPfn;
    }
    //在busyPages找到某实页号的page删除后并返回
    physicalPage* popMatchPage(int matchPfn) {
        queue<physicalPage*> newPages;
        physicalPage* matchPage;
        while(!this->busyPages.empty()){
            if(this->busyPages.front()->pfn == matchPfn) {
            matchPage = busyPages.front();
            busyPages.pop();
        }
        else {
            newPages.push(busyPages.front());
            busyPages.pop();
        }
        }
        this->busyPages = newPages;
        return matchPage;
    }

    int LRU(vector<int> sequence, const int len, virtualSpace a){
        int count = 0;
        int countTime = 0;
        for(int i=0;i<len;i++) {
            countTime++;
            if(a.page[sequence[i]].pfn == -1){
                //no hit
                if(freePages.empty()) {
                    physicalPage* replace = popMatchPage(findOldPage(a));
                    replace->pn = a.page[sequence[i]].pn;
                    a.page[sequence[i]].pfn = replace->pfn;
                    //set time
                    a.page[sequence[i]].time = countTime;
                    busyPages.push(replace);
                }
                else {
                    freePages.front()->pn = a.page[sequence[i]].pn;
                    a.page[sequence[i]].pfn = freePages.front()->pfn;
                    busyPages.push(freePages.front());
                    freePages.pop();
                }
            }
            else {
                count++;
            }
            getSpace();
        }
        return count;
    }
};

void printSequence(vector<int> sequence, int len)
{
    cout << "Virtual page sequence: ";
    for (int i = 0; i < len; ++i)
    {
        cout << sequence[i] << " ";
    }
    cout << endl << endl;
}

vector<int> generateRandomArray(int len, int min, int max) {
    srand(time(0)); // Set random seed
    vector<int> arr(len);
    for (int i = 0; i < len; ++i) {
        arr[i] = min + rand() % (max - min + 1); // Generate random number between min and max
    }

    return arr;
}


int main(){
    int num1,num2;
    int sequenceLen = 10;
    vector<int> sequence = generateRandomArray(sequenceLen,0,14);
    num1 = 15;
    cout<< "输入实页的数量： "<<endl;
    cin>>num2;
    while(num2>num1) {
        cout<<"实页数需要小于虚页数"<<endl;
        cin>>num2;
    }
    virtualSpace virtualspace1 = virtualSpace(num1);
    physicalSpace physicalspace1 = physicalSpace(num2);
    printSequence(sequence, sequenceLen);
    cout<<endl<<endl;
    cout << "************************Using FIFO************************" << endl;
    cout << "调度过程: " << endl;
    double count = physicalspace1.FIFO(sequence,sequenceLen,virtualspace1);
    cout << "hit次数: " << count << endl;
    cout << "Hit rate: " << fixed << setprecision(2) << count / sequenceLen;
    cout << endl << endl;

    virtualSpace virtualspace2 = virtualSpace(num1);
    physicalSpace physicalspace2 = physicalSpace(num2);
    cout << "************************Using LRU************************" << endl;
    cout << "Scheduling process: " << endl;
    count = physicalspace2.LRU(sequence, sequenceLen, virtualspace2);
    cout << "hit次数: " << fixed << setprecision(0) << count << endl;
    cout << "Hit rate: " << fixed << setprecision(2) << count / sequenceLen;
}