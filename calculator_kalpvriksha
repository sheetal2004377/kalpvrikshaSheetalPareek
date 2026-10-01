#include<stdio.h>
int main(){
    char str[100];
    int pos=0;
    int answer=0;
    int val=0;
    int curr_val=0;
    char op='+';
    int need_number=1;

    printf("Enter the expression: ");
    fgets(str,sizeof(str),stdin);

    while(str[pos]!='\0'){
        if(str[pos]==' '||str[pos]=='\n'){
            pos++;
            continue;
        }
        if(str[pos]>='0'&&str[pos]<='9'){
             if(need_number==0){      
                printf("Error: Invalid expression.\n");
                return 0;
            }
            need_number=0;
            val=0;
            while(str[pos]>='0'&&str[pos]<='9'){
                val=val*10+(str[pos]-'0');
                pos++;
            }
            if(op=='*'){
                curr_val=val*curr_val;
            }
            else if(op=='/'){
                if(val==0){
                    printf("Error:Division by zero.\n");
                    return 0;
                }
                curr_val=curr_val/val;
            }
            else{
                answer=answer+curr_val;
                if(op=='-'){
                    curr_val=-val;
                }
                else{
                    curr_val=val;
                }
            }
        }
        else if(str[pos]=='+'||str[pos]=='-'||str[pos]=='*'||str[pos]=='/'){
            if(need_number){        
                printf("Error: Invalid expression.\n");
                return 0;
            }
            need_number=1;
            op=str[pos];
            pos++;
        }
        else{
            printf("Error:Invalid expression.\n");
            return 0;
        }
    }
     if(need_number){                
        printf("Error: Invalid expression.\n");
        return 0;
    }
    answer=answer+curr_val;
    printf("%d\n",answer);
    return 0;

}