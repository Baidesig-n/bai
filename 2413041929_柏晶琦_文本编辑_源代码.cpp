#include <stdio.h>	//提供输入输出函数(如printf, scanf, fopen, fprintf等) 
#include <stdlib.h>	//提供内存管理函数(如malloc, free, exit等)
#include <string.h>	//提供字符串处理函数 (如strcpy, strlen, strcmp等)
#include <ctype.h>	//提供字符判断函数 

#define MAX_LEN 200	//每行文本的最大长度 

//链表节点结构体，每个节点保存一行文本和指向下一节点的指针 
typedef struct LineNode {
    char line[MAX_LEN];		//保存一行文本 
    struct LineNode* next;	//指向下一节点 
} LineNode;

//创建一个新节点，复制字符串内容 
LineNode* create_node(const char* str) {
    LineNode* node = (LineNode*)malloc(sizeof(LineNode));	//动态分配一个节点空间
    if(node == NULL){
    	exit(EXIT_FAILURE);		//分配失败直接退出程序
	} 
	strcpy(node->line, str);	//复制字符串到节点的line字段 
    node->next = NULL;			//初始化指针为 NULL 
    return node;				//返回新节点指针 
}

//将新节点追加到链表尾部 
void append_node(LineNode** head, const char* str) {
    LineNode* new_node = create_node(str);	 // 创建新节点
    if (*head == NULL) {
        *head = new_node;	// 如果链表为空，头指针直接指向新节点 
    } else {
        LineNode* curr = *head;
        while (curr->next) curr = curr->next;	// 找到链表尾部-
        curr->next = new_node;	// 尾部节点指向新节点
    }
}

//从文件读取文本内容并构建链表
LineNode* input_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");	// 以只读方式打开文件
    if (!fp) {	//打开文件失败处理 
        printf("无法打开文件 %s\n", filename);
        return NULL;
    }
    LineNode* head = NULL;	//初始化链表头为NULL 
    char buffer[MAX_LEN];	//临时存储读取的行 
    //逐行读取文件，去除换行符，加入链表 
    while (fgets(buffer, MAX_LEN, fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';	//去掉末尾换行 
        append_node(&head, buffer);		//将当前行添加到链表 
    }
    fclose(fp);
    return head;	//返回链表头指针 
}

//从终端读取多行输入，直到输入"#END"结束 
LineNode* input_from_terminal() {
    LineNode* head = NULL;	//初始化链表头位NULL 
    char buffer[MAX_LEN];	//临时存储输入的行 
    printf("请输入文本内容，每行一条，输入#END结束输入：\n");
    while (1) {
        fgets(buffer, MAX_LEN, stdin);			//循环读取，直到用户输入#END 
        buffer[strcspn(buffer, "\n")] = '\0';	//去除换行符 
        if (strcmp(buffer, "#END") == 0) break;	//遇到结束标志退出 
        append_node(&head, buffer);				//将输入行添加到链表 
    }
    return head;
}

//统计链表中的文本的字母、数字、空格、总字符数 
void count_statistics(LineNode* head, int* letter, int* digit, int* space, int* total) {
    *letter = *digit = *space = *total = 0;		//初始化所有计数器为0 
    for (LineNode* p = head; p; p = p->next) {	//遍历链表中的每个节点（行） 
        for (int i = 0; p->line[i]; i++) {		//遍历当前行的每个字符 
            char c = p->line[i];			//获取当前字符 
            if (isalpha(c)) (*letter)++;	//字母计数 
            else if (isdigit(c)) (*digit)++;	//数字计数 
            else if (c == ' ') (*space)++;	//空格计数 
            (*total)++;	//总字符计数 
        }
    }
}

//构建KMP算法的next数组，用于模式匹配中的跳转 
void build_kmp_next(const char* pattern, int* next) {
    int m = strlen(pattern);	//模式串长度 
    next[0] = 0;	//第一个位置固定为0 
    for (int i = 1, j = 0; i < m;) { //i遍历模式串，j记录前缀匹配长度 
        if (pattern[i] == pattern[j]) {
            next[i++] = ++j;	//匹配成功，前缀长度加1 
        } else if (j > 0) {
            j = next[j - 1];	//回溯到上一个匹配位置 
        } else {
            next[i++] = 0;	//		无匹配，设置为0 
        }
    }
}

//使用KMP算法在一行文本中查找字符出现次数 (针对单行) 
int kmp_search(const char* text, const char* pattern, int* next) {
    int n = strlen(text), m = strlen(pattern);	//文本长度，模式串长度 
    int i = 0, j = 0, count = 0;		//i遍历文本，j遍历模式串，count记录匹配次数		
    while (i < n) {
        if (text[i] == pattern[j]) { //字符匹配 
            i++; j++;				 //i和j后移 
        }
        if (j == m) {	//找到一次完整匹配 
            count++;				//匹配次数+1 
            j = next[j - 1];		//继续查找下一个匹配 
        } else if (i < n && text[i] != pattern[j]) {
            if (j != 0) j = next[j - 1];	//不匹配时跳转 
            else i++;
        }
    }
    return count;
}

//对整个链表执行子串查找
int search_substring_kmp(LineNode* head, const char* pattern) {
    int next[1000];	//存储next数组（模式串最长为1000字符) 
    build_kmp_next(pattern, next);	//构建next数组 
    int count = 0;	//初始化匹配次数 
    for (LineNode* p = head; p; p = p->next) {	//遍历链表中的每一行 
        count += kmp_search(p->line, pattern, next);	//累加每行的匹配次数 
    }
    return count;
}

//使用KMP算法删除链表中所有出现的子串 
void delete_substring_kmp(LineNode* head, const char* pattern) {
    int next[1000];
    build_kmp_next(pattern, next);
    int m = strlen(pattern);
    for (LineNode* p = head; p; p = p->next) {	//遍历链表中的每一行
        char* text = p->line;	//当前文本行 
        char temp[MAX_LEN] = "";	//临时存储处理后的文本 
        int i = 0, j = 0, k = 0;	//i遍历原文本，j遍历模式串，k写入temp 
        while (text[i]) {			//遍历原文本所有字符 
            if (text[i] == pattern[j]) {
                i++; j++;
                if (j == m) {
                    j = 0;	//匹配完整子串，跳过不写入，实现删除
                }
            } else {
                if (j == 0) {
                    temp[k++] = text[i++];	//没有任何匹配，直接复制
                } else {
                	// 部分匹配失败，写入当前已匹配前缀
                    for (int t = 0; t < j; t++) temp[k++] = pattern[t];
                    j = next[j - 1];	 // 使用 KMP 跳转机制回退 j
                }
            }
        }
        for (int t = 0; t < j; t++) temp[k++] = pattern[t];	//处理末尾残留的部分匹配前缀 
        temp[k] = '\0';			//添加字符串结束符 
        strcpy(p->line, temp);	//复制回链表节点 
    }
}

//输出原始文本和统计信息
void output_result(FILE* fp, LineNode* head, int letter, int digit, int space, int total) {
    fprintf(fp, "【原始文本内容】：\n");
    for (LineNode* p = head; p; p = p->next) {
        fprintf(fp, "%s\n", p->line);
    }

    fprintf(fp, "\n【统计信息】：\n");
    fprintf(fp, "字母数：%d\n", letter);	//输出统计标题 
    fprintf(fp, "数字数：%d\n", digit);		//输出字母数量 
    fprintf(fp, "空格数：%d\n", space);		//输出空格数量 
    fprintf(fp, "总字数：%d\n", total);		//输出总字符数 
}

//输出删除子串后的文本内容 
void output_deleted_text(FILE* fp, LineNode* head) {
    fprintf(fp, "\n【删除子串后的文本】：\n");
    for (LineNode* p = head; p; p = p->next) {	//输出遍历输出处理后的每行文本 
        fprintf(fp, "%s\n", p->line);
    }
}

// 释放链表所占内存
void free_list(LineNode* head){
	LineNode* p;
	while(head){
		p = head;
		head = head ->next;
		free(p);
	}
} 

//主函数入口 
int main() {
    LineNode* text = NULL;		         //文本链表头指针 
    int letter, digit, space, total;	//统计结果变量 
    char substr[100];					//存储到用户输入的字符 
    int choice_input, choice_output;	//输入输出方式的选择变量 
	
    printf("请选择输入方式：\n1. 从文件读取（D:\\data\\mytext.txt）\n2. 从终端输入\n选择：");
    scanf("%d", &choice_input);
    getchar();  // 吸收输入后的换行符

    if (choice_input == 1) {
        text = input_from_file("D:\\data\\mytext.txt");	//从文件读取 
    } else {
        text = input_from_terminal();					//从终端读取 
    }

    if (!text) {										//输入失败处理 
        printf("读取文本失败！\n");
        return 1;
    }

    printf("请选择输出方式：\n1. 输出到屏幕\n2. 输出到文件 output.txt\n选择：");
    scanf("%d", &choice_output);
    getchar();	//吸收输入后的换行符			

    FILE* fout = NULL;
    if (choice_output == 1) {
        fout = stdout;  // 标准输出
    } else {
        fout = fopen("output.txt", "w");
        if (!fout) {
            printf("无法创建输出文件\n");
            return 1;
        }
    }

	//统计文本信息并输出 
    count_statistics(text, &letter, &digit, &space, &total);
    output_result(fout, text, letter, digit, space, total);

	//搜索子串并输出结果 
    printf("请输入要查找的子串：");
    scanf("%s", substr);
    int found = search_substring_kmp(text, substr);
    fprintf(fout, "\n查找子串 \"%s\" 出现次数：%d 次\n", substr, found);

	//删除子串并数出结果 
    printf("请输入要删除的子串：");
    scanf("%s", substr);
    if(strlen(substr) == 0){
    	printf("子串不能为空！\n");
    	free_list(text);
    	return 1;
	}
    delete_substring_kmp(text, substr);
    output_deleted_text(fout, text);
    
	//关闭文件（如果输出到文件） 
    if (choice_output == 2 && fout != NULL) {
        fclose(fout);
        printf("程序执行完毕，结果已保存至 output.txt\n");
    } else {
        printf("程序执行完毕，结果已显示在屏幕上\n");
    }

	//释放内存
	free_list(text); 
    return 0;	//程序正常退出 
}

