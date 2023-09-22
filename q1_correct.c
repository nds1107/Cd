#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct token
{
    char lexeme[64];
    int row,col;
};
struct table_entry{
    int sno;
    char lexeme_name[100];
    char dtype[100],retype[100],tname[100];
    int num_args;
}Table[100];
 
static int row=1,col=1;
char buf[2048];
char dbuf[200];
const char specialsymbols[]={'?',';',':',','};
const char *keywords[] = {"const", "char", "int","return","for", "while", "do",
                            "switch", "if", "else","unsigned", "case", "break","float"};
const char *datatypes[]={"int","char","float","double"};
 
const char arithmeticsymbols[]={'*'};
static int ind=1;
int isKeyword(const char *str)
{
    for(int i=0;i<sizeof(keywords)/sizeof(char*);i++)
    {
    if(strcmp(str,keywords[i])==0)
      {
      return 1;
      }
    }
    return 0;
}
 
int charBelongsTo(int c,const char *arr)
{
    int len;
 
  if(arr==specialsymbols)
  {
      len=sizeof(specialsymbols)/sizeof(char);
    }
 
  else if(arr==arithmeticsymbols)
    {
      len=sizeof(arithmeticsymbols)/sizeof(char);
    }
 
  for(int i=0;i<len;i++)
    {
      if(c==arr[i])
      {
        return 1;
      }
    }
 
  return 0;
}
 
void fillToken(struct token *tkn,char c,int row,int col)
{
    tkn->row=row;
    tkn->col=col;
    tkn->lexeme[0]=c;
    tkn->lexeme[1]='\0';
}
 
void newLine()
{
    ++row;
    col=1;
}

int searchTable(const char* lexeme_name){
    if(ind==1)
        return 0;
    int i=0;
    while(i<ind){
        if(strcmp(Table[i].lexeme_name,lexeme_name)==0)
            return 1;
        i++;
    }
    return 0;
}
void insertIntoTable(struct token* tkn,int n_args,const char* type){
    if(!strcmp(type,"variable")){
        Table[ind-1].sno=ind;
        strcpy(Table[ind-1].lexeme_name,tkn->lexeme);
        strcpy(Table[ind-1].dtype,dbuf);
        strcpy(Table[ind-1].tname,"var");
        ++ind;
    }
    else if(!strcmp(type,"function")){
        Table[ind-1].sno=ind;
        strcpy(Table[ind-1].lexeme_name,tkn->lexeme);
        strcpy(Table[ind-1].retype,dbuf);
        strcpy(Table[ind-1].tname,"func");
        Table[ind-1].num_args=n_args;
        ++ind;
    }
    else
        printf("Token cannot be inserted\n");
}
void printTable(int n){
    for(int i=0;i<n;i++){
        if(!strcmp(Table[i].tname,"var"))
            printf("%d\t\t%s\t\t%s\t\t-\t\t-\t\t\t%s\n",Table[i].sno,Table[i].lexeme_name,Table[i].dtype,Table[i].tname);
        if(!strcmp(Table[i].tname,"func"))
            printf("%d\t\t%s\t\t-\t\t%s\t\t%d\t\t\t%s\n",Table[i].sno,Table[i].lexeme_name,Table[i].retype,Table[i].num_args,Table[i].tname);
    }
}
int isFuncName(FILE* f,char c){
    int n_args=0;
    if(c=='('){
        char d;
        int chars=0;
        while((d=fgetc(f))!=EOF&&d!=')'){
            if(d==',')
                n_args++;
            chars++;
        }
        if(n_args!=0)
            n_args++;
        fseek(f,(0-chars-1),SEEK_CUR);
    }
    else{
        n_args=-1;
        fseek(f,0,SEEK_CUR);
    }
    return n_args;
}
int isDtype(const char* lexeme){
    for(int i=0;i<sizeof(datatypes)/sizeof(char*);i++){
        if(strcmp(lexeme,datatypes[i])==0)
            return 1;
    }
    return 0;
}
 
struct token getNextToken(FILE *f1)
{
    char c;
    struct token tkn;
    tkn.row=-1;
 
  int gotToken=0;
 
  while(!gotToken && (c=fgetc(f1))!=EOF)
  {
        if(charBelongsTo(c,specialsymbols))
        {
             fillToken(&tkn,c,row,col);
             gotToken=1;
             ++col;
        }
 
    else if(charBelongsTo(c,arithmeticsymbols))
        {
          fillToken(&tkn,c,row,col);
          gotToken=1;
             ++col;
        }
 
    else if(c=='(')
        {
          fillToken(&tkn,c,row,col);
             gotToken=1;
          ++col;
        }
 
    else if(c==')')
      {
          fillToken(&tkn,c,row,col);
             gotToken=1;
          ++col;
    }
 
    else if(c=='{')
        {
          fillToken(&tkn,c,row,col);
             gotToken=1;
             ++col;
        }
 
    else if(c=='}')
        {
          fillToken(&tkn,c,row,col);
             gotToken=1;
             ++col;
        }
 
    else if(c=='+')
        {
             int d=fgetc(f1);
 
      if(d!='+')
          {
                 fillToken(&tkn,c,row,col);
                 gotToken=1;
                 ++col;
               fseek(f1,-1,SEEK_CUR);
             }
 
      else
          {
               fillToken(&tkn,c,row,col);
               strcpy(tkn.lexeme,"++");
               gotToken=1;
               col+=2;
             }
         }
 
     else if(c=='-')
         {
              int d=fgetc(f1);
 
            if(d!='-')
              {
                  fillToken(&tkn,c,row,col);
                  gotToken=1;
                  ++col;
                  fseek(f1,-1,SEEK_CUR);
                 }
 
            else{
                      fillToken(&tkn,c,row,col);
                      strcpy(tkn.lexeme,"--");
                        gotToken=1;
                      col+=2;
                  }
        }
 
      else if(c=='=')
             {
                 int d=fgetc(f1);
 
        if(d!='=')
                 {
                   fillToken(&tkn,c,row,col);
                   gotToken=1;
                   ++col;
                   fseek(f1,-1,SEEK_CUR);
                 }
 
        else
                 {
                  fillToken(&tkn,c,row,col);
                  strcpy(tkn.lexeme,"==");
                  gotToken=1;
                  col+=2;
                 }
        }
 
     else if(isdigit(c))
         {
             tkn.row=row;
             tkn.col=col++;
             tkn.lexeme[0]=c;
 
       int k=1;
 
       while((c=fgetc(f1))!=EOF && isdigit(c))
            {
                tkn.lexeme[k++]=c;
                col++;
            }
 
            tkn.lexeme[k]='\0';
             strcpy(tkn.lexeme,"num");
             gotToken=1;
             fseek(f1,-1,SEEK_CUR);
     }
 
     else if(c == '#')
       {
             while((c = fgetc(f1)) != EOF && c != '\n');
             newLine();
         }
 
     else if(c=='\n')
         {
             newLine();
             c = fgetc(f1);
 
       if(c == '#')
             {
                 while((c = fgetc(f1)) != EOF && c != '\n');
                    newLine();
             }
 
       else if(c != EOF)
             {
                fseek(f1, -1, SEEK_CUR);
             }
         }
 
    else if(isspace(c))
      {
            ++col;
        }
 
    else if(isalpha(c)||c=='_')
      {
            tkn.row=row;
            tkn.col=col++;
            tkn.lexeme[0]=c;
            int k=1;
 
      while((c=fgetc(f1))!= EOF && isalnum(c))
        {
            tkn.lexeme[k++]=c;
            ++col;
        }
 
        tkn.lexeme[k]='\0';
 
        if(!isKeyword(tkn.lexeme)){
            if(!searchTable(tkn.lexeme)){
                int n_args=isFuncName(f1,c);
                if(n_args==-1){
                    insertIntoTable(&tkn,n_args,"variable");
                }
                else{
                    insertIntoTable(&tkn,n_args,"function");
                }
            }
            strcpy(tkn.lexeme,"id");
        }
        else{
            if(isDtype(tkn.lexeme))
                strcpy(dbuf,tkn.lexeme);
        }
 
      gotToken=1;
            fseek(f1,-1,SEEK_CUR);
         }
 
    else if(c=='/')
      {
         int d=fgetc(f1);
         ++col;//Do we check EOF here?
 
     if(d=='/')
         {
             while((c=fgetc(f1))!= EOF && c!='\n')
             {
               ++col;
             }
 
       if(c=='\n')
             {
               newLine();
             }
            }
 
      else if(d=='*')
            {
              do
              {
                if(d=='\n')
                {
                     newLine();
                }
 
          while((c==fgetc(f1))!= EOF && c!='*')
                {
                       ++col;
 
            if(c=='\n')
                  {
                         newLine();
                     }
                   }
 
          ++col;
              }while((d==fgetc(f1))!= EOF && d!='/' && (++col));
              ++col;
             }
 
      else
            {
              fillToken(&tkn,c,row,--col);
              gotToken=1;
              fseek(f1,-1,SEEK_CUR);
            }
        }
 
   else if(c == '"')
     {
                tkn.row = row;
                tkn.col = col;
                int k = 1;
                tkn.lexeme[0] = '"';
 
        while((c = fgetc(f1)) != EOF && c != '"')
                {
                    tkn.lexeme[k++] = c;
                    ++col;
                }
 
                tkn.lexeme[k++] = '"';
                tkn.lexeme[k]='\0';
                gotToken = 1;
        }
 
    else if(c == '<' || c == '>' || c == '!')
         {
                fillToken(&tkn, c, row, col);
                ++col;
                int d = fgetc(f1);
                if(d == '=')
                {
                    ++col;
                    strcat(tkn.lexeme, "=");
                }
 
        else
                {
 
                    fseek(f1, -1, SEEK_CUR);
                 }
 
         gotToken = 1;
            }
 
    else if(c == '&' || c == '|')
        {
                int d = fgetc(f1);
 
        if(c == d)
                {
                    tkn.lexeme[0] = tkn.lexeme[1] = c;
                    tkn.lexeme[2] = '\0';
                    tkn.row = row;
                    tkn.col = col;
                    ++col;
                    gotToken = 1;
                }
 
        else
                {
                    fseek(f1, -1, SEEK_CUR);
                }
                ++col;
        }
 
    else
        {
            ++col;
        }
    }
    return tkn;
}
 
int main()
{
    char filename[100];
    printf("Enter the filename to open for reading:\n");
    scanf("%s", filename);

    FILE *f1=fopen(filename,"r");
    if(f1==NULL)
    {
      printf("Error! File cannot be opened!\n");
      return 0;
    }
 
    printf("\n\nTOKENS:\n\n");

    struct token tkn;
    while((tkn=getNextToken(f1)).row!=-1){
        printf("<%s, %d, %d>\n",tkn.lexeme, tkn.row,tkn.col);
    }

    printf("\n\nTABLE:\n\n");

    printf("S.No.\t\t LexemeName   DataType\t ReturnType\t\tNumArgs\t\tTokenName\n");
    int num_entries=ind;
    printTable(num_entries);
    fclose(f1);
}