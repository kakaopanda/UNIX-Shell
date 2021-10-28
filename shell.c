#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define SIZE 100
#define ROW 100
#define COL 100

char history[ROW][COL];
int count = 0;

void printHistory()
{
	int index = 0;
    int j = 0;
    int count_h = count;

    for(int i = count_h; i > 0; i--)
    {
        printf("%d. ", ++index);
        while (history[i-1][j] != '\n' && history[i-1][j] != '\0')
        {
            printf("%c", history[i-1][j]);
            j++;
        }
        printf("\n");
        j = 0;
    }
    printf("\n");
}

int commandShell(char buf[], char *args[], int *flag)
{
    int length = length = read(STDIN_FILENO, buf, SIZE);
	int start = -1;
    int i;      
    int c = 0;

    if(length == 0)
        exit(0);

    if(length < 0)
    {
        printf("ERROR : Command Not Exist. \n");
        exit(-1);
    }

    for(i = 0; i < length; i++)
    {
        switch(buf[i])
        {
			case ' ':
			case '\t':
				if(start != -1)
				{
					args[c] = &buf[start];
					c++;
				}
				buf[i] = '\0';
				start = -1;
				break;

			case '\n':
				if(start != -1)
				{
					args[c] = &buf[start];
					c++;
				}
				buf[i] = '\0';
				args[c] = NULL;
				break;

			default:
				if(start == -1)
					start = i;
				if(buf[i] == '&')
				{
					*flag = 1;
					buf[i] = '\0';
				}
        }
    }

    args[c] = NULL;

    if(strcmp(args[0], "exit") == 0)
        exit(0);

    if(strcmp(args[0], "cd") == 0)
    {
        if(chdir(args[1]) != 0)
            printf("ERROR : Change Directory Fail. \n");
        return -1;
    }

    if(strcmp(args[0], "history") == 0)
    {
        if(count > 0)
            printHistory();
        else
            printf("ERROR : Empty Space. \n");
        return -1;
    }
    else if(args[0][0] == '!')
    {
		int x = args[0][1] - '0';
        int y = args[0][2] - '0';

		char data[2] = {args[0][1], args[0][2]};
		int index = atoi(data);

        if(index > count)
            printf("ERROR : Enter the correct command. \n");
        else
        {
            if(args[0][1] == '!')
				strcpy(buf, history[0]);

            else if(args[0][1] == '0')
                printf("ERROR : Enter the correct command. \n");

			else if(x >= 1)
			{
				if(y >= 0)
					strcpy(buf, history[count - (y+10*x)]);
				else
					strcpy(buf, history[count - x]);

				if((x >= 49 && (x <= 74)))
				{
					char temp[SIZE];
					for(int k=count; k>0; k--)
					{
						if(history[k-1][0]==args[0][1])
						{
							strcpy(temp,history[k-1]);
							break;
						}
					}
					strcpy(buf, temp);
				}
			}	     
        }
    }

    for(i = ROW-1; i > 0; i--)
        strcpy(history[i], history[i - 1]);

    strcpy(history[0], buf);
    count++;
    if(count > ROW-1)
        count = ROW-1;
}

int main()
{
	pid_t pid;
	int flag;
	
	char *args[SIZE];
	char buf[SIZE];
    char current[SIZE];          

    while(1)
    {
        flag = 0;
        if(getcwd(current, sizeof(current)) == NULL)
        {
            perror("ERROR : Current Working Directory Find Fail.");
            return 1;
        }
        printf("$ ");
        fflush(stdout);

        if(commandShell(buf, args, &flag) != -1)
        {
            pid = fork();
            if (pid < 0)
            {
                printf("ERROR : Fork Fail. \n");
                exit(1);
            }
            else if (pid == 0)
            {
                if(execvp(args[0], args) == -1)
                    printf("ERROR : Command Executing Fail. \n");
            }
            else if(flag == 0)
                wait(NULL);
        }
    }
    return 0;
}