#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct command{
	int num;
	char body[1024];
};

void split_by_space(char* c_line, char** p_args, int max_words){
    int i;
    for(i=0; i< max_words; i++){
        p_args[i] = strsep(&c_line, " ");
        if(p_args[i] == NULL){
            break;
        }
    }
}
int main()
{
    printf("Do not enter extra spaces anywhere! \n");
    char c_line[1024];
    char* p_args[1024];
    int max_words = 20;
    int command_number = 0;
    int mntn_his_num = 9; /*It is actually 10 i.e from 0 to 9*//*Dear TA! if you want to scale this i.e maintain history of more than 10 commands then one might need to see command !x, I have made it only work for maintaing hsitory of smaller or equal to 10 commands.*/
    struct command com_his[mntn_his_num];
    int i;
    for(i = 0; i<= mntn_his_num; i++){
	com_his[i].num = -1;
    }



    while(1){
        printf("osh #%d>", command_number+1);
	
        fgets(c_line, 1024, stdin);
	
	if(c_line[strlen(c_line) - 1] == '\n'){
            c_line[strlen(c_line) - 1] = '\0';
        }
	if(strcmp("history", c_line) == 0){
		if(command_number == 0){printf("No commands in history\n");}
		
		int i;
		for(i= mntn_his_num; i >= 0; i--){
			if(com_his[i].num != -1){
		    		printf("%d-%s \n", i+1, com_his[i].body);
			}					
		}

		continue;
	}
	if(strcmp("!!", c_line) == 0){
		if(command_number == 0){printf("No commands in history\n"); continue;}
		/*storing in com_his is reverse i.e 0 -> 9*/
		if(command_number > mntn_his_num){strcpy(c_line, com_his[mntn_his_num].body);}
		else{strcpy(c_line, com_his[command_number-1].body);}
		printf("command run: %s \n", c_line);
		//c_line = com_his[mntn_his_num - (command_number-1) - 1].body;
	}else if(c_line[0] == '!' && isdigit(c_line[1])){
		int red_flag = 0;
		int index_req = 0;
		if((int)(c_line[1] - '0') == 0){red_flag = 1;}
		if(c_line[2] == '0'){
			if(c_line[1] != '1' ){red_flag = 1;}
			index_req = 9;		
		}else{
			index_req = (int)(c_line[1] - '0');
			index_req--;		
		}
		
		if(c_line[2] != '0' && isdigit(c_line[2])){red_flag = 1;}
		if(isdigit(c_line[3])){red_flag = 1;}		
		if(index_req >= command_number || red_flag == 1)
		{printf("No such command in history.\n");continue;}

		strcpy(c_line, com_his[index_req].body);
		printf("command run: %s \n", c_line);
	}
	


	/*putting command in com_his array*/
	//printf("changed command is: %s", c_line);
	if(command_number > mntn_his_num){
		int i;
		for(i = 0; i< mntn_his_num; i++){
			com_his[i].num = com_his[i+1].num;
			strcpy(com_his[i].body , com_his[i+1].body); 
		}
	}


	int cn = command_number;
	if(command_number > mntn_his_num){cn = mntn_his_num;}
	com_his[ cn ].num = command_number;
	strcpy(com_his[ cn ].body , c_line);	
	//com_his[(command_number-1)%10].body = c_line;	
	
	
	
        /*trailing new line characters omitted*/
        if(c_line[strlen(c_line) - 1] == '\n'){
            c_line[strlen(c_line) - 1] = '\0';
        }


	
		
        int do_wait = 0;	
	char* ind;
	if((ind=strchr(c_line, '&')) != NULL){
		do_wait = 1;
		*ind = '\0';
	}





	if(c_line[strlen(c_line) -1] == ' '){
		c_line[strlen(c_line) -1] = '\0';	
	}		
	split_by_space(c_line, p_args, max_words);
	

		
	
        pid_t pid = fork();
	command_number++;
        if(pid == 0){

            execvp(p_args[0], p_args);
            printf("enter proper command. \n");
		
        }
        else if( pid < 0){
            printf("Error in forking child process.");
		return 0;
        }
        else{
            int status;
		if(do_wait == 1){
            		waitpid(pid, &status, 0);
 
		}
           //return 0;
        }
    }
    return 0;
}
