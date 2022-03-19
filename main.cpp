#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <fstream>
#include <map>
#include <locale>
#include <cassert>
#include <ctime>

using namespace std;
typedef long long ll;

#define N 1000+10

ll prefixLong,suffixLong; //前缀长度和后缀长度
char launchWords[N]; //启动词
vector<string> dataRaw; //原始数据
multimap<string,string> words; //前后缀数据

/*

该代码写于MacOS 12.3 arm64

内核信息：
Darwin OZLIINEXs-ThinkPad.local 21.4.0 Darwin Kernel Version 21.4.0: Mon Feb 21 20:35:58 PST 2022; root:xnu-8020.101.4~2/RELEASE_ARM64_T6000 arm64

在Windows下（以dev-c++为例）要正常运行需要作如下修改：

1.data.txt的编码由UTF-8改为ANSI
2.编译器中编译参数添加：-finput-charset=UTF-8 -fexec-charset-GBK
3.代码103行，main()函数中调用的divideWords()函数，其中prefixLong和suffixLong的*3均改为*2


实现原理上，实际上用的是一个类似链表的功能，但尚未实现：

1.前后缀长度不一（例如prefixLong为3，suffixLong为2）时的映射关系，但考虑到模糊搜索会大幅增加CPU负载，没有考虑修复这个bug，实际上任务的定义也很模糊，我认为如果存在前后缀关系的话，这篇文章不会非常的乱，更多的会像是这份代码生成的那样，可能是前后句之间没有任何逻辑，但阅读下来还是可以的。不过我也认为是样本数不够，仅仅100条样本无法构建足够多的映射

2.处理不同字节的字符，我在README.pdf中要求data.txt中必须是2字节字符，实际上是可以做到识别字符字节长度的，比如网上一个比较高端的就是 if(~(p>>8)!=0)，其中p是字符

3.不加入模糊搜索还有一个原因：加入后文章只会更乱，连基本的逻辑都可能不存在，这对于一个文章生成器来说不是一个好事。


文章生成原理：

根据启动词，查找对应映射-》将映射作为新的启动词（prefix），查找它的下一个映射（suffix）-》如此循环，直到找不到新的映射为止。

由于全部使用了vector、multimap库自带的函数，因此代码量小、可读性高，但同时也存在无法模糊搜索的问题

    */


//取随机数，闭区间
ll getRandNum(ll min, ll max) {
    return (min + rand() %(max - min +1));
}

//读取数据
void initData(vector<string>& res,string pathname){
    ifstream infile;
    infile.open(pathname.data());
    assert(infile.is_open()); //失败则输出错误信息并终止
    string str;
    while(getline(infile,str)){
        res.push_back(str);
        str.clear();
    }
    infile.close();
}

//获取前后缀，考虑到data.txt里只可能全都是2字节字符，就不考虑1字节了
//在MacOS下，一个汉字占3个字节位置（似乎是Unicode编码不同的问题），因此要*3，而在Windows下，一个汉字占2个字节位置，因此只需要*2
void divideWords(vector<string>& array,multimap<string,string>& res,ll prefixLong, ll suffixLong){
    multimap<string,string>::iterator items;
    ll arraySize=array.size(),count;
    for(ll i=0;i<arraySize;i++){
        ll strlen = array[i].length();
        for(ll j=0;j+prefixLong<=strlen;j+=prefixLong){
            string prefix = array[i].substr(j,prefixLong);
            string suffix = array[i].substr(j+prefixLong,suffixLong);

            //这一部分负责实现去重功能，也就是，如果存在：a->b，那么就不能添加b->a，同时不允许存在多个a->b
            items = res.find(suffix);
            for(int i=0;i<res.count(suffix);i++,items++){
                if(items->second==prefix) continue;
                if(items->second==suffix) continue;
            }

            res.insert(make_pair(prefix,suffix));
        }
    }
}

string generateArticle(multimap<string,string>& res,char* launchWords){
    string ans=launchWords,wordsNow=launchWords;
    ll count=res.count(wordsNow),index;
    multimap<string,string>::iterator items;
    while(count){
        items = res.find(wordsNow);

        //这一部分负责实现随机的要求，因为Multimap可以允许一个键对应多个值,但是之所以是这么写是因为似乎他没有实现+=的operator，只能死板的用while位移，应该有别的优化方法
        index=getRandNum(0,count-1);
        while(index--) items++;

        wordsNow = items->second;
        // printf("当前搜索词：%s\n",wordsNow.c_str());
        ans+=wordsNow;
        count=res.count(wordsNow);
    }
    return ans==launchWords? "文章生成失败" : ans;
}

int main(){
    setlocale(LC_ALL,"en_US.UTF-8");
    puts("正在初始化程序\n");
    srand(time(NULL));//置随机种子
    initData(dataRaw,"./data.txt"); //初始化数据，首先进行以保证数据存在
    puts("按顺序输入前缀长度、后缀长度和启动词，用空格或回车隔开：\n");
    scanf("%lld %lld %s",&prefixLong,&suffixLong,launchWords);
    puts("\n正在处理前后缀\n");
    divideWords(dataRaw,words,prefixLong*3,suffixLong*3);
    puts("生成的10篇文章:");
    for(int i=0;i<10;i++) printf("\n%s\n",generateArticle(words,launchWords).c_str());
    return 0;
}