

#include<stdio.h>
#include<string.h>

#define NlenMax 20
#define FieldarrMax 50
#define FieldlenMax 20 
#define ValarrMax 50
#define VallenMax 20 


const char Primitives1[30]="Set_Packet_Structure";
const char Primitives2[30]="Select_Field";
const char Primitives3[30]="Set_Field_Value";

static int header_count=0;
static int ipv4_len=0;
static int udp_len=0;
static char next_type[10]="";


typedef struct hearder_t{
	char name[NlenMax];
	char field[FieldarrMax][FieldlenMax];
	char val[ValarrMax][VallenMax];
	int length;
}header;
static  header head[5];

void Packet_Structure(char *s,int len )
{
	printf("%s",s);
	int i;
	char temp[30]="";
	int j=0;
	for(i=2;i<=len;i++)
	{
		if(s[i]==']'){
			break;
		}
		else if(s[i]==','){
			temp[j]='\0';
//			printf("%s",temp);
			memcpy(head[header_count].name,temp,j);
			j=0;
			printf("%s\n",head[header_count].name);
			header_count++;
		}
		else{
			temp[j]=s[i];
			j++;
		}
	}
	temp[j]='\0';
//	printf("%s",temp);
	memcpy(head[header_count].name,temp,j);
	printf("%s\n",head[header_count].name);
	header_count++;
//	printf("%d\n",header_count);
	return ; 
}
void Select_Field(char *s,int len )
{
	printf("%s",s);
	int i;
	char temp[30]="";
	int j=0;
	int index=0;
	for(i=2;i<=len;i++)
	{
		if(s[i]==']'){
			break;
		}
		else if(s[i]=='.'){
			temp[j]='\0';
//			printf("%s",temp);
			int k;
			for(k=0;k<header_count;k++){
				if(strcmp(temp,head[k].name)==0)
				{
					index=k;
					break;
				}
			}
			temp[j]=s[i];
			j++;
		}
		else if(s[i]==',')
		{
			temp[j]='\0';
//			printf("%s",temp);
			int leng =head[index].length;
			memcpy(head[index].field[leng],temp,j);
			memcpy(head[index].val[leng],"",0);
			head[index].length++;
			j=0;
		}
		else{
			temp[j]=s[i];
			j++;
		}
	}
	temp[j]='\0';
	int leng =head[index].length;
	memcpy(head[index].field[leng],temp,j);
	memcpy(head[index].val[leng],"",0);
//	printf("%s\n",head[index].field[leng]);
	head[index].length++;
	return ;
 } 
 
 
void Set_Field_Value(char *s,int len )
{
	printf("%s\n",s);
//	printf("%d\n",header_count);
	int i;
	char temp[30]="";
	int counter[30];//记录相应的index值 
	char counter_field[30][30]; 
	int counter_i=0;
	int j=0;
	int index=0;
	int flag=0;
	for(i=2;i<len;i++){
		if(s[i]==']'&& !flag){
		//field
			temp[j]=s[i];
			int k;
			int p=0;
			char temp2[30]="";
			for(k=0;k<=j;k++){
				if(temp[k]==']'){
					break;
				} 
				else if(temp[k]==' '){
					continue;
				}
				else if(temp[k]=='.'){
					temp2[p]='\0';
					int l;
					for(l=0;l<header_count;l++){
						if(strcmp(temp2,head[l].name)==0)
						{
							index=l;
							counter[counter_i]=index;
//							printf("%d",counter[counter_i]);
//							counter_i++; 
							break;
						}						
					}
					temp2[p]=temp[k];
					p++;
				}
				else if(temp[k]==','){
					temp2[p]='\0';
					memcpy(counter_field[counter_i],temp2,p);
//					printf("%s.\n",counter_field[counter_i]);
					counter_i++; 
					p=0;										
				}
				else{
					temp2[p]=temp[k];
					p++;
				}
			}
			temp2[p]='\0';
//			printf("%s.\n",temp2);			
			memcpy(counter_field[counter_i],temp2,p);
			counter_field[counter_i][p]='\0'; 
//			printf("%s--.\n",counter_field[counter_i]);
			
			counter_i++; 
			j=0;
			flag=1;
			i+=2;//跳过[]之间的,空格 
		}
		else if(s[i]==']'&&flag){
		//value
			temp[j]=s[i];
//			printf("%s\n",temp);
			int k;
			int value_counter=0;
			int flag2=0;
			char temp2[30]="";
			int p=0;
			//跳过value的[ 
			for(k=1;k<=j;k++){
				if(temp[k]==','){	
					k++;
					continue;//跳过value里面的， 
				}else if(temp[k]=='"'&& !flag2){
					flag2=1;
					continue;
				}else if(temp[k]=='"' && flag2)
				{
					temp2[p]='\0';
					flag2=0;
					int leng=head[counter[value_counter]].length;
					strcpy(head[counter[value_counter]].field[leng],counter_field[value_counter]);
					strcpy(head[counter[value_counter]].val[leng],temp2);
					
					printf("%s-%s\n",counter_field[value_counter],temp2);
					printf("%s-%s\n",head[counter[value_counter]].field[leng],head[counter[value_counter]].val[leng]);
					head[counter[value_counter]].length++;
					value_counter++;
					p=0;
				}
				else{
					temp2[p]=temp[k];
					p++;	
				}
			}	
		}
		else{
			temp[j]=s[i];
			j++;
		}
	}
}

//计算ipv4_len next_type udp_len
void count_len(){
	int i=0;
	//计算字节    
	//ipv4 头部最小=20字节  最大60字节  暂时使用20 
	//udp 头部=8字节  
	int ipv4_headlen = 20;
	int nexttype_len=0;
	int udp_headlen = 8;
	int payload_len=0;
	for(i=0;i<header_count;i++)
	{
		if(strcmp(head[i].name,"IPv4")==0)
		{
			ipv4_len+=ipv4_headlen;
			if(i+1 < header_count){
				strcpy(next_type,head[i+1].name);
				//若要扩充协议 需要注意修改 
				if(strcmp(next_type,"UDP")==0){
					nexttype_len+=8;
				}else if(strcmp(next_type,"TCP")==0){
					nexttype_len+=20;
				}
			}
		}else if(strcmp(head[i].name,"UDP")==0)
		{
			udp_len+=udp_headlen;
		}else if(strcmp(head[i].name,"Payload")==0)
		{
			int j=0;
			for(j=0;j<head[i].length;j++)
			{
				payload_len+=strlen(head[i].val);
			}
		}
	}
	udp_len+=payload_len; 
	ipv4_len+=payload_len; 
	printf("########################################\n");
	return ; 
}

void judge(char *s,int len)
{
//	printf("%s",s);
	char temp[200]="";
	int j=-1;
	int i;
	for(i=0;i<len;i++)
	{
		if(s[i]=='(')
		{
			j++;
		}
		if(j!=-1)
		{
			temp[j]=s[i];
			j++;
		}
	}
	temp[j]='\0';
	if(s[4]=='P'){
		Packet_Structure(temp,j);
	}
	else if(s[2]=='l'){
		Select_Field(temp,j);
	}
	else{
		Set_Field_Value(temp,j);
	}
	return ;
}

void text_analyze(){
	char ch;
	while(1){
		char temp[200]="";
		int i=0;
		int flag=0;
		while((ch=getchar())!='\n')		{
			if(flag==1){
				temp[i]=ch;
				i++;
			}
			else if(ch=='='){
				flag=1;
			}	
		}
		if(temp[0]=='+')
			break; 
		temp[i]='\0';
		judge(temp,i);
		int p=0;
		for(p=0;p<header_count;p++)
		{
			printf("%s	%d\n",head[p].name,head[p].length);
			int j=0;
			for(j=0;j<head[p].length;j++)
			{
				printf("%s	%s\n",head[p].field[j],head[p].val[j]);
			}
		}
		count_len();
		printf("ipv4_len:%d,udp_len:%d,next_type:%s\n",ipv4_len,udp_len,next_type);
	}
}
/*
int main()
{
	char ch;
	while(1){
		char temp[200]="";
		int i=0;
		int flag=0;
		while((ch=getchar())!='\n')		{
			if(flag==1){
				temp[i]=ch;
				i++;
			}
			else if(ch=='='){
				flag=1;
			}	
		}
		if(temp[0]=='+')
			break; 
		temp[i]='\0';
		judge(temp,i);
		int p=0;
		for(p=0;p<header_count;p++)
		{
			printf("%s	%d\n",head[p].name,head[p].length);
			int j=0;
			for(j=0;j<head[p].length;j++)
			{
				printf("%s	%s\n",head[p].field[j],head[p].val[j]);
			}
		}
		count_len();
		printf("ipv4_len:%d,udp_len:%d,next_type:%s\n",ipv4_len,udp_len,next_type);
	}
	
	return 0;
} */
