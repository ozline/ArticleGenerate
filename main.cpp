#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <fstream>
#include <map>

using namespace std;
typedef long long ll;

#define N 1000+10

ll prefixLong,suffixLong; //前缀长度和后缀长度
char launchWords[N]; //启动词
vector<string> dataRaw; //原始数据
multimap<string,string> words; //前后缀数据


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
//TODO:汉字占2个字符位，但是为什么这边只*2的话输出内容会出错？反而*3可以了？
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