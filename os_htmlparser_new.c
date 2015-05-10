#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include "os_htmlbase_new.c"

int PrintResult(Node* nowNode);

//截取字符串,并返回截取后的新字符串
char* GetText(char* str,int len){
	if(strlen(str)<len)
		len = strlen(str); 
	char* text;
	text = (char*)malloc(len+1);
	memset(text,0,len+1);
	strncpy(text,str,len); 
	return text; 
}

//字符串指针进位
char* StrInc(char* str, int n){
	if(strlen(str)<=n)
		return NULL;
	int i = 0;
	while(*str&&(i<n)){	
		i++;
		str++;
	}
	return str;
}

//去掉字符串开头的空格
char* UnSpaceStr(char* str){
	char tmp;
	while(tmp = *str){
		if(isspace(tmp))
			str++;
		else	
			return str;
	}
	return NULL;
}


//初始化Prop
void InitProp(NodeProp* nowProp){
	nowProp->name = NULL;
	nowProp->value = NULL;
	nowProp->text = NULL;
	nowProp->nextProp = NULL;
}

//初始化Node
void InitNode(Node* nowNode){   	
	nowNode->type = NODE_UNKNOWN;
	nowNode->tagType = TAG_UNKNOWN;
	nowNode->tagName = NULL;
	nowNode->text = NULL;
	nowNode->comtext = NULL;
	nowNode->props = NULL;
	nowNode->comprops = NULL;
	nowNode->nextNode = NULL;
	NodeProp* newProp;
	NodeProp* newComprop;
	newProp =(NodeProp*)malloc(sizeof(NodeProp));
	newComprop =(NodeProp*)malloc(sizeof(NodeProp));
	InitProp(newProp);
	InitProp(newComprop);
	nowNode->props = newProp;
	nowNode->comprops = newComprop;
}

//添加一个Node
Node* AddNode(Node* rootNode,enum HtmlNodeType type){
	Node* newNode; 
	newNode =(Node *)malloc(sizeof(Node));
	InitNode(newNode);
	newNode->type = type; 
	rootNode->nextNode = newNode; 
	return newNode;
}

//添加一个Prop
NodeProp* AddProp(NodeProp* nowProp){
	NodeProp* newProp;
	newProp =(NodeProp*)malloc(sizeof(NodeProp));
	InitProp(newProp);
	nowProp->nextProp = newProp;
	return newProp;
}

//根据空格或 > 取标签名
int GetTagNameLen(char* str){
	char tmpChar;
	char* tmpStr;
	int bInQuotes1 = 0; 
	int bInQuotes2 = 0;
	tmpStr = str;
	while(tmpChar = *tmpStr){ 
		//判断是否是字符串
		if('\"' == tmpChar){
			if(bInQuotes1)
				bInQuotes1 = 0;
			else
				bInQuotes1 = 1;
		}
		else if('\'' == tmpChar){
			if(bInQuotes2)
				bInQuotes2 = 0;
			else
				bInQuotes2 = 1;
			}
		//取非字符串中的第一个空格或者 > 号
		if((!bInQuotes1)&&(!bInQuotes2)){
			if(isspace(tmpChar)||('>' == tmpChar))
				break;
		}
		tmpStr++;
	}
	return tmpStr - str;
}

//取字符位置，忽略掉字符串引号括起来的内容
int GetUnQuotedPos(char* str, char des){
	char tmpChar;
	char* tmpStr;
	int bInQuotes1 = 0; 
	int bInQuotes2 = 0;
	tmpStr = str;
	while(tmpChar = *tmpStr){ 
		//判断是否是字符串
		if('\"' == tmpChar){
			if(bInQuotes1)
				bInQuotes1 = 0;
			else
				bInQuotes1 = 1;
		}
		else if('\'' == tmpChar){
			if(bInQuotes2)
				bInQuotes2 = 0;
			else
				bInQuotes2 = 1;
			}
		//取非字符串中的第一个满足条件的字符
		if((!bInQuotes1)&&(!bInQuotes2)){
			if(des == tmpChar)
				break;
		}
		tmpStr++;
	}
	if(tmpStr)
		return tmpStr - str;
	else
		return 0;
}

//去掉引号
char* UnQuotedStr(char* str){
	if(!str) return NULL;
	char* tmpStr;
	int len;
	len = strlen(str);
	tmpStr = str;
	//判断第一位是否为引号
	if(('\"' == *tmpStr)||('\'' == *tmpStr)){
		tmpStr ++;
		len--;
	}
	if(('\"'==tmpStr[len-1])||('\''==tmpStr[len-1])){
		len--;
	} 
	if(len == 0) 
		return NULL;
	else 
		return  GetText(tmpStr,len);
}

//去掉括号
char* UnParenStr(char* str){
	char* tmpStr;
	int len;
	len = strlen(str);
	tmpStr = str;
	if('('==*tmpStr){
		tmpStr ++;
		len--;
	}
	if(')'==tmpStr[len-1]){
		len--;
	} 
	return UnQuotedStr(GetText(tmpStr,len));
}

//返回以目标字符串开始的后续字符串
//类似于 GetUnQuotedPos,只是返回值不同
char* FoundUtilStr(char* str, char des){
	char tmpChar;
	char* tmpStr;
	int bInQuotes1 = 0; 
	int bInQuotes2 = 0;
	tmpStr = str;
	while(tmpChar = *tmpStr){ 
		//判断是否是字符串
		if('\"' == tmpChar){
			if(bInQuotes1)
				bInQuotes1 = 0;
			else
				bInQuotes1 = 1;
		}
		else if('\'' == tmpChar){
			if(bInQuotes2)
				bInQuotes2 = 0;
			else
				bInQuotes2 = 1;
			}
		//取非字符串中的第一个满足条件的字符
		if((!bInQuotes1)&&(!bInQuotes2)){
			if(des == tmpChar)
				break;
		}
		tmpStr++;
	}
	return tmpStr ;
}

//计算Node的类型
void CalNodeType(Node* nowNode){
	char tmp;
	tmp = *(nowNode->tagName);
	if('/' == tmp){
		nowNode->type = NODE_CLOSE_TAG;
		nowNode->tagName ++;
		nowNode->tagType = GetTagTypeByName(nowNode->tagName);
		}
	else{
		nowNode->tagType = GetTagTypeByName(nowNode->tagName);
		nowNode->type = GetNodeType(nowNode->tagType);
	}			
}

//根据标签名，获取关闭标签的内容
char* GetCloseTag(char* tagName){
	char* str;
	str = (char*)malloc(strlen(tagName)+4);
	memset(str,0,strlen(tagName)+4);
	str[0] = '<';
	str[1] = '/'; 
	str = strcat(str,tagName); 
	str = strcat(str,">");
	return str;
}

//拆分属性   name1=value1 
void ParseProp(NodeProp* nowProp){
	if((!nowProp)||(!nowProp->text))
		return;
	char tmpChar;
	char* text;
	char* startText;
	text = nowProp->text;
	startText = text; 
	while(tmpChar=*text){
		if('=' == tmpChar){
			nowProp->name = GetText(nowProp->text,text-nowProp->text);
			text++;
			text = GetText(text,strlen(text));
			nowProp->value =UnQuotedStr(text);
			free(text);
//			nowProp->value =UnQuotedStr( GetText(text,strlen(text)));			
			return;
		}
	text++;	
	}
}

//拆分CSS(style)标签内容
int parserComtextForCSS(Node* nowNode){
	int i = 0;
	char tmpChar;
	char* keyWords;
	char* comtext; 
	char* urlText;
	int urlLen;
	NodeProp* nowProp;
	nowProp = nowNode->comprops;
	while(i < KEY_WORDS_CSS){
		keyWords = GetCSSKeyWords(i); 
		comtext = nowNode->comtext;
		while(*comtext){
			comtext = strstr(comtext,keyWords); 
			if(!comtext)	break;
			comtext = StrInc(comtext,strlen(keyWords));
			tmpChar = *comtext; 
			if('('==tmpChar){ 
				nowProp->name = "url"; 
				urlLen = GetUnQuotedPos(comtext,')');
				urlText = GetText(comtext,urlLen+1);
				nowProp->text = urlText;
				urlText = UnParenStr(urlText);
				nowProp->value = urlText;
				nowProp = AddProp(nowProp);
//				nowProp = nowProp->nextProp; 			
			}
			comtext ++;		
		}
		i++;
	}
}

//拆分每个Node的内容，feng ge Props属性
int parserProp(Node* nowNode){
	if(!nowNode->text) 
		return 0;
//	char* nodeText;
	char tmpChar; 
	char* startText;
	char* Text;
	int bInQuotes1 = 0; 
	int bInQuotes2 = 0;
	NodeProp* nowProp;
//	nodeText = nowNode->text;
	nowProp = nowNode->props;

	startText = nowNode->text;
	Text = nowNode->text; 
	while(tmpChar = *Text){ 
		//判断是否是字符串
		if('\"' == tmpChar){
			if(bInQuotes1)
				bInQuotes1 = 0;
			else
				bInQuotes1 = 1;
		}
		else if('\'' == tmpChar){
			if(bInQuotes2)
				bInQuotes2 = 0;
			else
				bInQuotes2 = 1;
			}
		//不在字符串中
		if(!bInQuotes1 && !bInQuotes2){
			//是空格，取第一个空格前的内容
			if(isspace(tmpChar)&&(Text > startText)){
				nowProp->text = GetText(startText,Text-startText);
				ParseProp(nowProp);
				nowProp = AddProp(nowProp);
				startText = Text + 1;
			}
		}
	Text++;
	}
	//循环结束，如果仍有剩余数据，则处理最后的数据
	if(Text > startText){
		nowProp->text = GetText(startText,Text-startText);
		ParseProp(nowProp);
		nowProp = AddProp(nowProp); 
		startText = Text + 1; 
	}
}

//拆分网页源码，分为每个Node
static void parseHtml(char* htmlText){
	if(!htmlText) return;
   Node rootNode;    
	Node* nowNode;
//	NodeProp* nowProp;    
	char *text;
	char * tmpText;
	char* comtext;
	char firstStr;
	int tagNameLen,tagBodyLen,tagSkipLen; //tag的各个长度值

	text = htmlText;
	InitNode(&rootNode);
	nowNode = &rootNode; 
	while(*text){ 
		tagNameLen = 0; tagBodyLen = 0; tagSkipLen = 0;
		//取 < 所在位置，找不到则退出
		text = FoundUtilStr(text,'<');
		if(!text) break; 
		//从 < 的后一位开始
		text++;
		//取tagName
		tagNameLen = GetTagNameLen(text);
		nowNode->tagName = GetText(text,tagNameLen); 
		//根据tagName计算类型 		
		CalNodeType(nowNode); 
		//简单的节点类型,例如 Link,A 只需要解析节点内属性就可以了
		if(NODE_START_TAG == nowNode->type){ 
			text = StrInc(text,tagNameLen); 
			text = UnSpaceStr(text);
			tagBodyLen = GetUnQuotedPos(text,'>');
			if(tagBodyLen){ 
				//qu zuihou yiwei,panduan shifou shi zifenbi jiedian
				tmpText = StrInc(text,tagBodyLen-1);
				firstStr = *tmpText;
				if('/' == firstStr) 
					tagSkipLen =1;
				//
				nowNode->text = GetText(text,tagBodyLen-tagSkipLen);		
				parserProp(nowNode);
				text = StrInc(text,tagBodyLen+1);	 
			}	
			else{
				nowNode->text = NULL;
				text++;
			}
		}
		else if(NODE_CLOSE_TAG == nowNode->type){
			nowNode->text = NULL;
			text = StrInc(text,tagNameLen+1);  
		}
		//需要考虑节点间内容的，例如  style, script
		else if(NODE_NEEDCLOSE_TAG == nowNode->type){		
			text = StrInc(text,tagNameLen);
			text = UnSpaceStr(text);
			tagBodyLen = GetUnQuotedPos(text,'>');
			if(tagBodyLen){ 
				nowNode->text = GetText(text,tagBodyLen); 
				text = StrInc(text,tagBodyLen+1);
				parserProp(nowNode);	
			}	
			else{
				nowNode->text = NULL;
				text++;
			}
			comtext = GetCloseTag(nowNode->tagName);  
			tagSkipLen = strlen(comtext);
			comtext = strstr(text,comtext);	
			tagBodyLen = comtext- text ;
			nowNode->comtext = GetText(text,tagBodyLen); 
			parserComtextForCSS(nowNode);
			text = StrInc(text,tagBodyLen+tagSkipLen);	
		}
		text = UnSpaceStr(text);
		if(!text) break;
		nowNode = AddNode(nowNode,NODE_UNKNOWN);
	}
	//循环输出结果 
	PrintResult(&rootNode);

}

int PrintResult(Node* nowNode){
	NodeProp* nowProp;
	while(nowNode&&nowNode->tagName){
		if((nowNode->type == NODE_START_TAG)){//&&(nowNode->props->text)&&(nowNode->tagType==TAG_STYLE))
			printf("标签类型:%s\n",nowNode->tagName);
			printf("标签内容:%s\n",nowNode->text); 
			nowProp = nowNode->props;
			while(nowProp&&nowProp->name){
				printf("属性内容:%s\n",nowProp->text); 
		//		if((!strcmp(nowProp->name,"src"))||(!strcmp(nowProp->name,"href"))){
				printf("节点属性:%s\n",nowProp->name); 
				printf("节点属性值:%s\n",nowProp->value); 
		//		}
				nowProp = nowProp->nextProp;
			}  
		}
		else if(nowNode->type == NODE_CLOSE_TAG){ 
			printf("tag_close_Name:%s\n",nowNode->tagName);
		}
		else if(nowNode->type == NODE_NEEDCLOSE_TAG){
			printf("标签类型:%s\n",nowNode->tagName);
			printf("标签内容:%s\n",nowNode->text); 
			printf("标签jiaoben内容:%s\n",nowNode->comtext); 
			nowProp = nowNode->props;
			while(nowProp&&nowProp->name){
				printf("属性内容:%s\n",nowProp->text);
			//	if((!strcmp(nowProp->name,"src"))||(!strcmp(nowProp->name,"href"))){
				printf("节点属性:%s\n",nowProp->name); 
				printf("节点属性值:%s\n",nowProp->value);
			//	} 
				nowProp = nowProp->nextProp;
			}  
			nowProp = nowNode->comprops;
			while(nowProp&&nowProp->text){
				printf("属性内容:%s\n",nowProp->text); 
				printf("脚本属性:%s\n",nowProp->name); 
				printf("脚本性值:%s\n",nowProp->value); 
				nowProp = nowProp->nextProp;
			}  
		}
		nowNode = nowNode->nextNode;		
	}

}


int main(){
  

	//读取文件
	FILE *html;
	char tmpChar;
	char htmlText[2000000];
	char *p;
	memset(htmlText,0,2000000);
	p=htmlText;
	html = fopen("test.txt","r");
	if(html){
		while((tmpChar =fgetc(html))!=EOF){
			*p++ = tmpChar;		
		}
	}
	fclose(html);
	html = NULL;

 	//通过网址直接获取源代码
/*	FILE *fp;
    char htmlText[200000];
    if (NULL != (fp = popen("curl http://www.baidu.com", "r")))
        fread(htmlText, 200000, 1, fp);
    else {
        fprintf(stderr, "popen error...\n");
        exit(1);
  	  } */

	printf("-----------------------------------------------------------------------\n");
	printf("-----------------------------------------------------------------------\n");
//	printf("htmlText:%s\n",htmlText);
//	printf("-------------------------------------------------------------------\n");
	parseHtml(htmlText);	
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
}
