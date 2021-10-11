#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#define BUFF_SIZE 1
#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"

int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);
int sh_num();
int sh_launch(char**);

char *builtin_str[]={
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **)={
    &sh_cd,
    &sh_help,
    &sh_exit
};

int sh_cd(char **args){
     if (args[1]==NULL)
     {
         fprintf(stderr,"sh: expected argument to cd \n");
     }
     else{
         if (chdir(args[1])!=0)
         {
            perror("sh");
         }
    }

    return 1;
     
}

int sh_help(char **args){
    int i;
    printf("This is the help page for my shell\n");
    printf("Built in functions are\n");

    for(i=0;i<sh_num();i++){
        printf("%s \n",builtin_str[i]);
    }
}

int sh_exit(char **args){
    return 0;
}

int sh_num(){
    return sizeof(builtin_str)/sizeof(char*);
}


char* sh_read_line(){
    int position=0;
    int size=BUFF_SIZE;
    char c;
    char *buffer;
    buffer=(char*)malloc(sizeof(char)*size);
    if(!buffer){
        fprintf(stderr,"sh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while(c!='\n'){
        c=getchar(); 
        
        buffer[position]=c;
    

        position++;

        if(position>=BUFF_SIZE){
            size+=BUFF_SIZE;
            buffer=realloc(buffer,size);
        
            if(!buffer){
                fprintf(stderr,"sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    buffer[position]='\0';
    return buffer;
}

char** sh_split_line(char *line){
    int buff_size=SH_TOK_BUFSIZE;
    int position=0;
    char *token;
    char **tokens;
    tokens=(char**)malloc(buff_size*sizeof(char*));
    if(!tokens){
        fprintf(stderr,"sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token=strtok(line,SH_TOK_DELIM);
    while (token!=NULL)
    {
        tokens[position]=token;
        position++;

        if (position>=buff_size)
        {   buff_size+=SH_TOK_BUFSIZE;
            tokens=realloc(tokens,buff_size*sizeof(char*));
            if (!tokens)
            {   fprintf(stderr,"sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token=strtok(NULL,SH_TOK_DELIM);
        
    }

    tokens[position]=NULL;
    return tokens;   
}

int sh_execute(char **args){
    if(!args[0]){
        return 1;
    }

    for (int i = 0; i <sh_num(); i++)
    {
        if(!strcmp(args[0],builtin_str[i])){
            return (*builtin_func[i])(args);
        }
    }

    return sh_launch(args);
    
}


int sh_launch(char **args){
    int status;
    pid_t pid,wpid;
    pid=fork();
    if(pid==0){
        if(execvp(args[0],args)==-1){
            perror("sh");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid<0){
        perror("sh");
    }
    else{
        do
        {
            wpid=waitpid(pid,&status,WUNTRACED);
        } while (!WIFEXITED(status)&&!WIFSIGNALED(status));
        
    }
    return 1;
}


int main(){
    char *line;
    char **args;
    int status;
    
    do{
        printf(">> ");
        line=sh_read_line();
        args=sh_split_line(line);
        status=sh_execute(args);
        free(line);
        free(args);
    }while(status);
    return 0;
}
