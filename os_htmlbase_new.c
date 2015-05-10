#include <stdio.h>

#define KEY_WORDS_CSS 2

enum HtmlNodeType
{
	NODE_UNKNOWN = 0,
	NODE_START_TAG,
	NODE_CLOSE_TAG,
	NODE_NEEDCLOSE_TAG,
	NODE_CONTENT,
	NODE_SOFT_LINE,
	NODE_ROOT,
};

enum HtmlTagType 
{
TAG_UNKNOWN = 0,
TAG_A,TAG_DIV,TAG_FONT,TAG_IMG, //4
TAG_P,TAG_SPAN,TAG_BR,TAG_B,TAG_I,//9
TAG_HR,TAG_COLOR,TAG_BGCOLOR,TAG_LINK,TAG_STYLE,//14 
TAG_SCRIPT,
};

typedef struct HtmlNodeProp
{
	char* name; 
	char* value; 
	char* text;
	struct HtmlNodeProp* nextProp;
}NodeProp;

typedef struct HtmlNode 
{
	enum HtmlNodeType type; 
	enum HtmlTagType tagType; 
	char* tagName; 
	char* text; 
	char* comtext;
	struct HtmlNodeProp* props;
	struct HtmlNodeProp* comprops;
	struct HtmlNode* nextNode;
}Node;

static enum HtmlTagType GetTagTypeByName(char* name){
	char *tmp;
	char *tmp1;
	int result;
	tmp = (char*)malloc(strlen(name)+1);
	tmp = strcpy(tmp,name);
	tmp1 = tmp; 
	while(*tmp){
		*tmp = toupper(*tmp);
		tmp++;
	} 
	if(!strcmp(tmp1,"A"))
		result = TAG_A; 
	else if(!strcmp(tmp1,"DIV"))
		result = TAG_DIV;  
	else if(!strcmp(tmp1,"FONT"))
		result = TAG_FONT;  
	else if(!strcmp(tmp1,"IMG"))
		result = TAG_IMG;  
	else if(!strcmp(tmp1,"P"))
		result = TAG_P;  
	else if(!strcmp(tmp1,"SPAN"))
		result = TAG_SPAN;  
	else if(!strcmp(tmp1,"BR"))
		result = TAG_BR;  
	else if(!strcmp(tmp1,"B"))
		result = TAG_B;  
	else if(!strcmp(tmp1,"I"))
		result = TAG_I;  
	else if(!strcmp(tmp1,"HR"))
		result = TAG_HR;  
	else if(!strcmp(tmp1,"COLOR"))
		result = TAG_COLOR; 
	else if(!strcmp(tmp1,"BGCOLOR"))
		result = TAG_BGCOLOR;
	else if(!strcmp(tmp1,"LINK"))
		result = TAG_LINK;
	else if(!strcmp(tmp1,"STYLE"))
		result = TAG_STYLE;
	else if(!strcmp(tmp1,"SCRIPT"))
		result = TAG_SCRIPT;
	else
		result = TAG_UNKNOWN;
	free(tmp1);
	return result;
}

static enum HtmlNodeType GetNodeType(enum HtmlTagType tagType){
	if(tagType < 14)
		return NODE_START_TAG;
	else
		return NODE_NEEDCLOSE_TAG;
}

static char* GetCSSKeyWords(int i){
	switch(i){
		case 0 : return " url";
		case 1 : return ":url";
	}
}


