#include <stdio.h>	//�ṩ�����������(��printf, scanf, fopen, fprintf��) 
#include <stdlib.h>	//�ṩ�ڴ������(��malloc, free, exit��)
#include <string.h>	//�ṩ�ַ��������� (��strcpy, strlen, strcmp��)
#include <ctype.h>	//�ṩ�ַ��жϺ��� 

#define MAX_LEN 200	//ÿ���ı�����󳤶� 

//����ڵ�ṹ�壬ÿ���ڵ㱣��һ���ı���ָ����һ�ڵ��ָ�� 
typedef struct LineNode {
    char line[MAX_LEN];		//����һ���ı� 
    struct LineNode* next;	//ָ����һ�ڵ� 
} LineNode;

//����һ���½ڵ㣬�����ַ������� 
LineNode* create_node(const char* str) {
    LineNode* node = (LineNode*)malloc(sizeof(LineNode));	//��̬����һ���ڵ�ռ�
    if(node == NULL){
    	exit(EXIT_FAILURE);		//����ʧ��ֱ���˳�����
	} 
	strcpy(node->line, str);	//�����ַ������ڵ��line�ֶ� 
    node->next = NULL;			//��ʼ��ָ��Ϊ NULL 
    return node;				//�����½ڵ�ָ�� 
}

//���½ڵ�׷�ӵ�����β�� 
void append_node(LineNode** head, const char* str) {
    LineNode* new_node = create_node(str);	 // �����½ڵ�
    if (*head == NULL) {
        *head = new_node;	// �������Ϊ�գ�ͷָ��ֱ��ָ���½ڵ� 
    } else {
        LineNode* curr = *head;
        while (curr->next) curr = curr->next;	// �ҵ�����β��-
        curr->next = new_node;	// β���ڵ�ָ���½ڵ�
    }
}

//���ļ���ȡ�ı����ݲ���������
LineNode* input_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");	// ��ֻ����ʽ���ļ�
    if (!fp) {	//���ļ�ʧ�ܴ��� 
        printf("�޷����ļ� %s\n", filename);
        return NULL;
    }
    LineNode* head = NULL;	//��ʼ������ͷΪNULL 
    char buffer[MAX_LEN];	//��ʱ�洢��ȡ���� 
    //���ж�ȡ�ļ���ȥ�����з����������� 
    while (fgets(buffer, MAX_LEN, fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';	//ȥ��ĩβ���� 
        append_node(&head, buffer);		//����ǰ����ӵ����� 
    }
    fclose(fp);
    return head;	//��������ͷָ�� 
}

//���ն˶�ȡ�������룬ֱ������"#END"���� 
LineNode* input_from_terminal() {
    LineNode* head = NULL;	//��ʼ������ͷλNULL 
    char buffer[MAX_LEN];	//��ʱ�洢������� 
    printf("�������ı����ݣ�ÿ��һ��������#END�������룺\n");
    while (1) {
        fgets(buffer, MAX_LEN, stdin);			//ѭ����ȡ��ֱ���û�����#END 
        buffer[strcspn(buffer, "\n")] = '\0';	//ȥ�����з� 
        if (strcmp(buffer, "#END") == 0) break;	//����������־�˳� 
        append_node(&head, buffer);				//����������ӵ����� 
    }
    return head;
}

//ͳ�������е��ı�����ĸ�����֡��ո����ַ��� 
void count_statistics(LineNode* head, int* letter, int* digit, int* space, int* total) {
    *letter = *digit = *space = *total = 0;		//��ʼ�����м�����Ϊ0 
    for (LineNode* p = head; p; p = p->next) {	//���������е�ÿ���ڵ㣨�У� 
        for (int i = 0; p->line[i]; i++) {		//������ǰ�е�ÿ���ַ� 
            char c = p->line[i];			//��ȡ��ǰ�ַ� 
            if (isalpha(c)) (*letter)++;	//��ĸ���� 
            else if (isdigit(c)) (*digit)++;	//���ּ��� 
            else if (c == ' ') (*space)++;	//�ո���� 
            (*total)++;	//���ַ����� 
        }
    }
}

//����KMP�㷨��next���飬����ģʽƥ���е���ת 
void build_kmp_next(const char* pattern, int* next) {
    int m = strlen(pattern);	//ģʽ������ 
    next[0] = 0;	//��һ��λ�ù̶�Ϊ0 
    for (int i = 1, j = 0; i < m;) { //i����ģʽ����j��¼ǰ׺ƥ�䳤�� 
        if (pattern[i] == pattern[j]) {
            next[i++] = ++j;	//ƥ��ɹ���ǰ׺���ȼ�1 
        } else if (j > 0) {
            j = next[j - 1];	//���ݵ���һ��ƥ��λ�� 
        } else {
            next[i++] = 0;	//		��ƥ�䣬����Ϊ0 
        }
    }
}

//ʹ��KMP�㷨��һ���ı��в����ַ����ִ��� (��Ե���) 
int kmp_search(const char* text, const char* pattern, int* next) {
    int n = strlen(text), m = strlen(pattern);	//�ı����ȣ�ģʽ������ 
    int i = 0, j = 0, count = 0;		//i�����ı���j����ģʽ����count��¼ƥ�����		
    while (i < n) {
        if (text[i] == pattern[j]) { //�ַ�ƥ�� 
            i++; j++;				 //i��j���� 
        }
        if (j == m) {	//�ҵ�һ������ƥ�� 
            count++;				//ƥ�����+1 
            j = next[j - 1];		//����������һ��ƥ�� 
        } else if (i < n && text[i] != pattern[j]) {
            if (j != 0) j = next[j - 1];	//��ƥ��ʱ��ת 
            else i++;
        }
    }
    return count;
}

//����������ִ���Ӵ�����
int search_substring_kmp(LineNode* head, const char* pattern) {
    int next[1000];	//�洢next���飨ģʽ���Ϊ1000�ַ�) 
    build_kmp_next(pattern, next);	//����next���� 
    int count = 0;	//��ʼ��ƥ����� 
    for (LineNode* p = head; p; p = p->next) {	//���������е�ÿһ�� 
        count += kmp_search(p->line, pattern, next);	//�ۼ�ÿ�е�ƥ����� 
    }
    return count;
}

//ʹ��KMP�㷨ɾ�����������г��ֵ��Ӵ� 
void delete_substring_kmp(LineNode* head, const char* pattern) {
    int next[1000];
    build_kmp_next(pattern, next);
    int m = strlen(pattern);
    for (LineNode* p = head; p; p = p->next) {	//���������е�ÿһ��
        char* text = p->line;	//��ǰ�ı��� 
        char temp[MAX_LEN] = "";	//��ʱ�洢�������ı� 
        int i = 0, j = 0, k = 0;	//i����ԭ�ı���j����ģʽ����kд��temp 
        while (text[i]) {			//����ԭ�ı������ַ� 
            if (text[i] == pattern[j]) {
                i++; j++;
                if (j == m) {
                    j = 0;	//ƥ�������Ӵ���������д�룬ʵ��ɾ��
                }
            } else {
                if (j == 0) {
                    temp[k++] = text[i++];	//û���κ�ƥ�䣬ֱ�Ӹ���
                } else {
                	// ����ƥ��ʧ�ܣ�д�뵱ǰ��ƥ��ǰ׺
                    for (int t = 0; t < j; t++) temp[k++] = pattern[t];
                    j = next[j - 1];	 // ʹ�� KMP ��ת���ƻ��� j
                }
            }
        }
        for (int t = 0; t < j; t++) temp[k++] = pattern[t];	//����ĩβ�����Ĳ���ƥ��ǰ׺ 
        temp[k] = '\0';			//����ַ��������� 
        strcpy(p->line, temp);	//���ƻ�����ڵ� 
    }
}

//���ԭʼ�ı���ͳ����Ϣ
void output_result(FILE* fp, LineNode* head, int letter, int digit, int space, int total) {
    fprintf(fp, "��ԭʼ�ı����ݡ���\n");
    for (LineNode* p = head; p; p = p->next) {
        fprintf(fp, "%s\n", p->line);
    }

    fprintf(fp, "\n��ͳ����Ϣ����\n");
    fprintf(fp, "��ĸ����%d\n", letter);	//���ͳ�Ʊ��� 
    fprintf(fp, "��������%d\n", digit);		//�����ĸ���� 
    fprintf(fp, "�ո�����%d\n", space);		//����ո����� 
    fprintf(fp, "��������%d\n", total);		//������ַ��� 
}

//���ɾ���Ӵ�����ı����� 
void output_deleted_text(FILE* fp, LineNode* head) {
    fprintf(fp, "\n��ɾ���Ӵ�����ı�����\n");
    for (LineNode* p = head; p; p = p->next) {	//����������������ÿ���ı� 
        fprintf(fp, "%s\n", p->line);
    }
}

// �ͷ�������ռ�ڴ�
void free_list(LineNode* head){
	LineNode* p;
	while(head){
		p = head;
		head = head ->next;
		free(p);
	}
} 

//��������� 
int main() {
    LineNode* text = NULL;		         //�ı�����ͷָ�� 
    int letter, digit, space, total;	//ͳ�ƽ������ 
    char substr[100];					//�洢���û�������ַ� 
    int choice_input, choice_output;	//���������ʽ��ѡ����� 
	
    printf("��ѡ�����뷽ʽ��\n1. ���ļ���ȡ��D:\\data\\mytext.txt��\n2. ���ն�����\nѡ��");
    scanf("%d", &choice_input);
    getchar();  // ���������Ļ��з�

    if (choice_input == 1) {
        text = input_from_file("D:\\data\\mytext.txt");	//���ļ���ȡ 
    } else {
        text = input_from_terminal();					//���ն˶�ȡ 
    }

    if (!text) {										//����ʧ�ܴ��� 
        printf("��ȡ�ı�ʧ�ܣ�\n");
        return 1;
    }

    printf("��ѡ�������ʽ��\n1. �������Ļ\n2. ������ļ� output.txt\nѡ��");
    scanf("%d", &choice_output);
    getchar();	//���������Ļ��з�			

    FILE* fout = NULL;
    if (choice_output == 1) {
        fout = stdout;  // ��׼���
    } else {
        fout = fopen("output.txt", "w");
        if (!fout) {
            printf("�޷���������ļ�\n");
            return 1;
        }
    }

	//ͳ���ı���Ϣ����� 
    count_statistics(text, &letter, &digit, &space, &total);
    output_result(fout, text, letter, digit, space, total);

	//�����Ӵ��������� 
    printf("������Ҫ���ҵ��Ӵ���");
    scanf("%s", substr);
    int found = search_substring_kmp(text, substr);
    fprintf(fout, "\n�����Ӵ� \"%s\" ���ִ�����%d ��\n", substr, found);

	//ɾ���Ӵ���������� 
    printf("������Ҫɾ�����Ӵ���");
    scanf("%s", substr);
    if(strlen(substr) == 0){
    	printf("�Ӵ�����Ϊ�գ�\n");
    	free_list(text);
    	return 1;
	}
    delete_substring_kmp(text, substr);
    output_deleted_text(fout, text);
    
	//�ر��ļ������������ļ��� 
    if (choice_output == 2 && fout != NULL) {
        fclose(fout);
        printf("����ִ����ϣ�����ѱ����� output.txt\n");
    } else {
        printf("����ִ����ϣ��������ʾ����Ļ��\n");
    }

	//�ͷ��ڴ�
	free_list(text); 
    return 0;	//���������˳� 
}

