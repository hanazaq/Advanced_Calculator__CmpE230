#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXTOKENS   256 
#define TOKENLENGTH 256 
#define N  1000   //postfix length

//global variables
//hashing
char hashmap_keys[256][256];
char hashmap_values[256][25];
int hashMapEmptiness[256]; // 1 if index is full, otherwise 0

//to postfix
char tokens[MAXTOKENS][TOKENLENGTH] ; //array of strings will be used in converting to postfix
int token_num;
char str[N] ;  //postfix result with be stored here
int  cur = 0 ; // keep track of the current token
int error=0 ; //if 1,-1,-2 print Error!
char* ptrto;

//evaluating
long long stack[256];
long long *sP = stack;
int stack_num=0;
char symbols[]={'+','-','*','&','|'};

//clean input
char splitArray[]={'+','-','*','&','|','(',')',' ',',','%','\t' };

//declartions
//hashing
void hash_map_add(char*, char*);
int hash_function(char*);
int hashmap_return_index(char*); 
char* hashmap_return_value(char*);

//BNF structure
int  expr(char *) ;
int  option(char *);
int  moreoptions(char *);
int  prop(char *);
int  moreprops(char *);
int  term(char *) ;
int  moreterms(char *) ;
int  factor(char *) ;
int  morefactors(char *) ;
int  is_integer(char *) ; 
int  var(char *);
int logic(char *);

//evaluating
long long evaluator(char *postfix);
long long xor(long long a, long long b);
long long not(long long a);
long long ls(long long a, long long i);
long long rs(long long a, long long i);
long long lr(long long n, long long d);
long long rr(long long n, long long d);
long long pop();
void push(long long new);
long long peek();

//clean input
void input_cleaner(char* line); 
void reset_for_next_line();
char* lltostr(long long );
//FUNCTIONS

//adds variable and its value to the hashmap
void hash_map_add(char *new_variable, char *value){
    int index=hash_function(new_variable);
    int count=0;
    while(1==1)
    {
    //if given index empty: add    
    if(hashMapEmptiness[index]==0){
        strcpy(hashmap_keys[index],new_variable);
        strcpy(hashmap_values[index],value);
        hashMapEmptiness[index]=1;
        break;
    }
    else{
        //Update the stored variable value
        if(strcmp(hashmap_keys[index],new_variable)==0){
            strcpy(hashmap_values[index],value);
            break;
        }
        else{
            index++; count++;
            if(index>256){index = index % 256;}
            //can also consider hash full case  
            if(count>257){printf("hashmap is full\n"); break;}  
        }
    }}
}   


// hashes the string 
int hash_function(char* current){
    int sum=0; //sum of variable alphabetic indixes
    int i=0;
    while(current[i]!='\0'){
        int a= 'A';
        int letter=current[i];
        sum+=(i+1)*(letter-a);
        i++;
    }
    //to stay inside the size of the hashmap
    if(sum>256){
        sum = sum % 256;
    }
    return sum;
}

    

//returns the index of the variable (search inside the hashmap)
//not found -1
int hashmap_return_index(char* current){
    int count=0;
    int index=hash_function(current);
    while(1==1){
        if(hashMapEmptiness[index]==0){ 
            return -1; //not found
        }
        //not empty
        if(strcmp(hashmap_keys[index],current)==0){
            return index; //found
        }
        else{
            index++; count++;
            if(index>256){index = index % 256;}
            if(count>257){return -1;} //have iterated thorough the whole full hashmap >> not found 
        }

    }
}

//return the value of the variable
//not found "empty" 
char* hashmap_return_value(char* current){
    int index= hashmap_return_index(current);
    if(index==-1){return "0";}
    else{
        return hashmap_values[index];
    }
}

///////////////////////////////////////////////////////////////////////

//<expr> := <option> <more options>
int expr(char *str)
{
   char str1[N], str2[N] ; 
   str1[0] = str2[0] = '\0' ; 
   if (! option(str1)) {  
      return(0) ; 
   } 
   if (! moreoptions(str2)) { 
      return(0) ; 
   }
   // have both option and moreoptions
   strcat(str1,str2) ;  
   strcpy(str,str1) ;
   return(1) ;
}
//<more options> := “|”  <option> <more options> | E
int moreoptions(char *str){
   char str1[N], str2[N], str3[N] ; 
   
   str1[0] = str2[0] = str3[0] = '\0' ; 
   
   if (strcmp(tokens[cur],"|") == 0 ) {
       strcpy(str1,tokens[cur]) ; 
       strcat(str1," ") ; 
       cur++ ;  
       if (!  option(str2)) {
          return(0) ; 
       } 
       if (!  moreoptions(str3)) {
         return(0) ; 
       }
   }
   strcat(str2,str1) ;
   strcat(str2,str3) ;
   strcpy(str,str2) ; 
   return(1) ;  
}
//<option>:= <prop> <more props>
int option(char *str){
 char str1[N], str2[N] ; 
   str1[0] = str2[0] = '\0' ; 
   if (! prop(str1)) {  
      return(0) ; 
   } 
   if (! moreprops(str2)) {
      return(0) ; 
   }
  
   strcat(str1,str2) ;  
   strcpy(str,str1) ; 
   return(1) ;
}
//<more props>:= & <prop> <more props> | E
int moreprops(char *str){
   char str1[N], str2[N], str3[N] ; 
   str1[0] = str2[0] = str3[0] = '\0' ; 
   if (strcmp(tokens[cur],"&") == 0 ) {
       strcpy(str1,tokens[cur]) ; 
       strcat(str1," ") ; 
       cur++ ;  
       if (!  prop(str2)) {
          return(0) ; 
       } 
       if (!  moreprops(str3)) {
         return(0) ; 
       }
   } 
   strcat(str2,str1) ;
   strcat(str2,str3) ;
   strcpy(str,str2) ; 
   return(1) ;  
}
// <prop>:= <term>  <more terms>
int prop(char *str){
   char str1[N], str2[N] ; 
   str1[0] = str2[0] = '\0' ; 
   if (!  term(str1)) {  
      return(0) ; 
   } 
   if (!  moreterms(str2)) {
      return(0) ; 
   }
   // have both term and more terms compine
   strcat(str1,str2) ;  
   strcpy(str,str1) ; 
   return(1) ;
}

//<more terms> := + <term> <more terms>
		    //  | - <term> <more terms>
		    //  | E
int moreterms(char *str)
{
   char str1[N], str2[N], str3[N] ; 
   str1[0] = str2[0] = str3[0] = '\0' ; 
   if ( (strcmp(tokens[cur],"+") == 0 ) || (strcmp(tokens[cur],"-") == 0 ) ) {
       strcpy(str1,tokens[cur]) ; 
       strcat(str1," ") ; 
       cur++ ;  
       if (!  term(str2)) {
          return(0) ; 
       } 
       if (!  moreterms(str3)) {
         return(0) ; 
       }
   }
   strcat(str2,str1) ;
   strcat(str2,str3) ;
   strcpy(str,str2) ; 
   return(1) ;  
}
// <term>:= <factor> <more factors>
int term(char *str)
{
   char str1[N], str2[N] ; 
   str1[0] = str2[0] = '\0' ; 
   if (!  factor(str1)) {
      return(0) ; 
   } 
   if (!  morefactors(str2)) {
      return(0) ; 
   }
   strcat(str1,str2) ; 
   strcpy(str,str1) ;
   return(1) ;  
}

// <more factors> := * <factor><more factors> | E
int morefactors(char *str)
{
   char str1[N], str2[N], str3[N] ; 
   
   str1[0] = str2[0] = str3[0] = '\0' ; 
   
   if ( (strcmp(tokens[cur],"*") == 0 ) || (strcmp(tokens[cur],"/") == 0 ) ) {
       strcpy(str1,tokens[cur]) ; 
       strcat(str1," ") ; 
       cur++ ;  
       if (!  factor(str2)) {
          return(0) ; 
       }
       if (!  morefactors(str3)) {
         return(0) ; 
       }
   } 
   strcat(str2,str3) ; 
   strcat(str2,str1) ;
   strcpy(str,str2) ;
   return(1) ;  
}

// <factor>:= (<expr>) | <var> | <num> | <logic>
int factor(char *str)
{
   char str1[N] ; 
   str1[0] = '\0' ;
    if ( is_integer(tokens[cur])  ) { 
       strcpy(str,tokens[cur]) ; 
       strcat(str," ") ;
       cur++ ; 
       return(1) ; 
    } 
   
    if ( strcmp(tokens[cur],"(") == 0 ) {
       cur++ ; 
       if ( ! expr(str1) ) {
          return(0) ;    
       }
       if ( strcmp(tokens[cur] ,")" )!=0) { 
          error=1;
        //   printf("Error!\n") ; 
          return(0) ;
       }
       cur++ ; 
       strcpy(str,str1) ; 
       return(1) ; 
    }
    if(logic(str1)){
        strcpy(str,str1) ; 
        strcat(str," ") ;  
        return (1);
      }
    if ( var(tokens[cur])  ) { 
        strcpy(str,hashmap_return_value(tokens[cur])) ; 
        strcat(str," ") ; 
        cur++ ; 
        return(1) ; 
    }
    error=1;
    return(0) ; 
}
//<logic> := xor( <expr>, <expr>) | lr( <expr>, <expr>) | … | not (<expr>)
// char logicGates[]="xor ls rs lr rr not";
int logic(char *str)
{
   char str1[N] ; 
   char str2[N] ;
   char str3[N] ; //logic gate 
   str1[0] =str2[0]= str3[0]='\0' ; 
    if ( strcmp( tokens[cur],"xor")!=0 &&  strcmp( tokens[cur],"ls")!=0 
    && strcmp( tokens[cur],"rs")!=0 && strcmp( tokens[cur],"lr")!=0 &&
    strcmp( tokens[cur],"rr")!=0 && strcmp( tokens[cur],"not")!=0  ) { 
       return 0; 
    } 
    strcpy(str3, tokens[cur]);
    cur++;
    if ( strcmp(tokens[cur],"(") == 0 ) {
       cur++ ; 
       if ( ! expr(str1) ) {
         return(0) ;    
       }
      if(strcmp(str3, "not")!=0)
      {     
         if(strcmp(tokens[cur],",") != 0){
            error=1;
            return 0;
         }

         cur++ ; 
         if ( ! expr(str2) ) {
            return(0) ;    
         }
      }
       if ( strcmp(tokens[cur] ,")" )!=0) { 
          error=1;
          return(0) ;
       }
     
       cur++ ;
       strcat(str1,str2) ; 
       strcat(str1,str3) ;
       strcpy(str,str1) ;
       return(1) ; 
    }
    error=1;
    return(0) ; 
}
//numbers
int is_integer(char *token) 
{
    int isnumber = 1 ;
    char *q ; 

    for(q = token ; *q != '\0' ; q++) {
        isnumber = isnumber && isdigit(*q) ;
    }
    return(isnumber) ; 
} 
//letters  <var>:= [a-zA-Z] 
int  var( char * token){
    //reserved word
    if ( strcmp( token,"xor")==0 ||  strcmp( token,"ls")==0 
    || strcmp( token,"rs")==0 || strcmp( token,"lr")==0 ||
    strcmp( token,"rr")==0 || strcmp( token,"not")==0  ) { 
       return 0; 
    }
    int isaletter = 1 ;
    char *q ; 
     for(q = token ; *q != '\0' ; q++) {
        isaletter = isaletter && isalpha(*q) ;
    }
    return(isaletter) ; 
}

///////////////////////////////////////////////////////////////////////// 
//sends all to the stack and evaluates the postfix expression
long long evaluator(char *postfix){
    //when there is an operator pops the last two thing and evaluates and adds it again
    // int counter=0;
    char *token = strtok(postfix, " ");
    while(token != NULL){
        if(strstr(symbols,token)!=NULL){
            long long x1=pop();
            long long x2=pop();
            if(error==-1){
                return 0;
            }
            if(strcmp(token,"+")==0){
                push(x1+x2);
            }
            else if(strcmp(token,"-")==0){
               push(x2-x1); 
            }
            else if(strcmp(token,"*")==0){
                push(x1*x2);
            }
            else if(strcmp(token,"&")==0){
                push(x2&x1);
            }
            else if(strcmp(token,"|")==0){
                push(x2|x1);
            }

        }
    
        else if (strcmp(token,"xor")==0)
        {
            long long x1=pop();
            long long x2=pop(); 
            if(error==-1){
                return 0;
            } 
            push(xor(x2,x1));
                      
        }
        else if (strcmp(token,"ls")==0)
        {
            long long x1=pop();
            long long x2=pop();   
            if(error==-1){
                return 0;
            }
            push(ls(x2,x1));         
        }
        else if (strcmp(token,"rs")==0)
        {            
            long long x1=pop();
            long long x2=pop();
            if(error==-1){
                return 0;
            }
            push(rs(x2,x1)); 
        }
        else if (strcmp(token,"lr")==0)
        {            
            long long x1=pop();
            long long x2=pop();
            if(error==-1){
                return 0;
            }
            push(lr(x2,x1)); 
        }
        else if (strcmp(token,"rr")==0)
        {
            long long x1=pop();
            long long x2=pop();
            if(error==-1){
                return 0;
            }
            push(rr(x2,x1)); 
        }
        else if (strcmp(token,"not")==0)
        {
            long long x1=pop();
            if(error==-1){
                return 0;
            }
            push(not(x1));  
        }
        else{        
            long long x;
            x=atoll(token);
            push(x);
        }
        token = strtok(NULL, " ");
    
    }
    if(stack_num==0){
        error=-2;
        return 0;
    }


    if(stack_num!=1){
        error=-1;
        return 0;
    }
    return pop();
} 

long long xor(long long a, long long b){return a ^ b; }
long long not(long long a){return ~a;} //Returns bitwise complement of a.
long long ls(long long a, long long i){return a <<i;} //Returns the result of a shifted i bits to the left.
long long rs(long long a, long long i){return a >>i;} //Returns the result of a shifted i bits to the right.
long long lr(long long n, long long d){return (n << d)|(n >> (64 - d));} //Returns the result of a rotated i times to the left.
long long rr(long long n, long long d){  return (n >> d)|(n << (64 - d));} //Returns the result of a rotated i times to the right

//stack implementation

//pop from stack
long long pop(){
    if(stack_num<1){
        error=-1;
        return 0;
    }
    long long num=stack[stack_num-1];
    stack_num-=1;
    return num;
    
}
//push to stack
void push(long long new){
    stack[stack_num] = new;
    stack_num+=1;
}
//show the last element in the stack
long long peek(){
    return stack[stack_num-1];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//do not send line directly copy it 
//cleans the input and adds the tokens to the array
void input_cleaner(char* line){
    token_num=0;
    int counter=0;
    int length=strlen(line)-1;
    line[length]=' ';
    line[length+1]='\0';
    length+=1;

    while(1){
        //when we reached the end of the file 
        if (length==0){
            if(strchr(splitArray, line[counter-1]) == NULL){
                strcpy(tokens[token_num],line);
                token_num+=1;
                break;
            }
            else{
                break;
            }
            
        }
        char current=line[counter];
        char currentStr[2];
        currentStr[0]=current;
        currentStr[1]='\0';
        //to save the variable name 
        if(strchr(splitArray, current) == NULL){
            counter+=1;
            length-=1;
            continue;
        }
        //to divide if there is a symbol that should be splitted
        else{
            //if counter has 0 index there are nothing to store beforehand so 
            if(counter==0){
                 if(current==' '||current=='\t' ){
                    line=line+1;
                    counter=0;
                            
                }
                else if(current=='%'){
                    break;
                }
                else{
                    line=line+1;
                    counter=0;  
                    strcpy(tokens[token_num],currentStr);
                    token_num+=1;
                }
            }
        //if counter has not 0 index, there are something to store beforehand so 
            else{
                if(current==' '||current=='\t' ){
                    char *bin=line;
                    bin[counter]='\0';
                    line+=counter+1;
                    counter=0;
                    strcpy(tokens[token_num],bin);
                    token_num+=1;
                                 
                }
                else if(current=='%'){
                    char *bin=line;
                    bin[counter]='\0';
                    strcpy(tokens[token_num],bin);
                    break;  
                }
                else{
                    
                    char *bin=line;
                    bin[counter]='\0';
                    line+=counter+1;
                    counter=0;
                    strcpy(tokens[token_num],bin);
                    strcpy(tokens[token_num+1],currentStr);
                    token_num+=2;
                }
            }
        }
        length-=1;
        
    
    }
 
}
///////////////////////
//send the copy to this it juct cleans the spaces at the beginning and end if there are spaces at the middle does not change
//cleans the input when it is an assignment
char* input_cleaner_second(char* line){  
    int length=strlen(line);
    char* s=line+length;
    int first=1;
    int last=1;
    //basically it checks if there is a non-whitespace element, when finds it makes first or last 0 so it stops moving if it could not keep searching
    while(1){
        if(first){
            if(*(line)==' '||*(line)=='\t' ){
                line=line+1;
            }
            else{
                first=0;
            }
        }
        if(last){
            if(*(s)==' '|| *(s)=='\t'||*(s)=='\0'||*(s)=='\n'){
                *s='\0';
                s=s-1;
            }
            else{
                last=0;
            }
        }
        if(first==0 &&  last==0){
            break;
        }
        if(s==line){
            if(*(line)==' '||*(line)=='\t'){
               error=1; 
            //    printf("Error!\n");//when there are only x  lines in our code
               line="\0";
            return line; 
            }
            else{
                break;
            }
        }
        
    }
    return line;
    

}

//convert long long to string
char* lltostr(long long value){
  char str[25], *p;  //created a pointer to keep lon long as str
  unsigned long long absValue;
  
  if(value<0)
  {
    absValue=-value;
    }
  else{
    absValue=value;
    }
  p = str + 24;//starting pointer from arrays last index
  *p='\0';
  int length=1;
  p--;
  int a=1;
  while(a){
    if(10>absValue){
        a=0;
    }
    *p= '0' + (absValue%10);
    p--;
    absValue=absValue/ 10;
    length++;
  }
    if(value < 0) {
    *p = '-';
    length++;
    }
    else{
    p ++;//skip sign part
    }
    char *s = (char*)malloc(sizeof(char) * (length + 1));
    strncpy(s, p, length);
     
   return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reset_for_next_line(){
    cur=0; error=0;
    ptrto=realloc(NULL, N); 
    for(int i=0; i<MAXTOKENS; i++){
        strcpy(tokens[i],"");
    }
}

//divides assignments from = sign
char* equal(char* p){
    int length=strlen(p);
    *(p+length+1)='\0';
    char* s;
    int index=0;
    while(1){
        if(*(p+index)=='='){
            *(p+index)='\0';
            s=p+index+1;
            return s;
        }
        else{
            index+=1;
        }
    }
}

//when there is a comment symbol stops the line on that
void comment_seperator(char* p){
    int length=strlen(p);
    for(int i=0;i<length;i++){
        if(*(p+i)=='%'){
            *(p+i)='\n';
            *(p+i+1)='\0';
            break;
        }
    }
}

int main()
{ 
    
   char line[256 +1] = ""; // add termantıng  
    ptrto = (char*)calloc(N, sizeof(char));
    printf(">");
    while (fgets(line, sizeof(line), stdin)){
        error=0;
        reset_for_next_line(); 
        comment_seperator(line);
        if(line == NULL){
            break;
        }

        //expression 
        if(strchr(line, '=') == NULL){
            input_cleaner(line); // will fill the array of tokens
            expr(ptrto);                      // will fill str according to array of the tokens
            // 5 5 5 num of token=3 but cur=1
            if(error==1 || token_num>cur){printf("Error!\n"); printf(">");continue;}
            long long x=evaluator(ptrto);
            if(error!=-1&&error!=-2){
              printf("%s\n", lltostr(x)); printf(">"); continue;      //evaluator for postfix str give long long result  
            }
            else if(error==-2L){
                printf(">"); continue;
            }
            else{
                printf("Error!\n"); printf(">");continue;
            }   
        }
        else{
            //assignement token=expression
        char temp[257];
            strcpy(temp, line);
            char* expression= equal(temp);
            char* x=input_cleaner_second(temp);
            if(var(x)==1){
                input_cleaner(expression);
                expr(ptrto);
                if(error==1 || token_num>cur){printf("Error!\n");printf(">");continue;}
                long long res=evaluator(ptrto);
                if(error==-1||error==-2){
                     printf("Error!\n"); 
                     printf(">");continue;
                     }
                
                hash_map_add(x,lltostr(res));
                printf(">");continue;
            }
            else{
                printf("Error!\n");
                printf(">");continue;
            }
        }
    
    printf(">");
    }
    
    
    return 0;
}