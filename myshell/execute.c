#include "myshell.h"



// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
    FILE * stream;
    if(t == NULL) {			// hmmmm, that's a problem
	exitstatus	= EXIT_FAILURE;
    }
    else {				// normal, exit commands
    //对命令进行分类，参考函数print_shellcmd0()    
    switch (t->type) {
    case CMD_COMMAND :
	//print_redirection(t);
	//file redirection; output and input; append or not append
	if(t->infile != NULL)
            printf("< %s ", t->infile);
        if(t->outfile != NULL) {
            if(t->append == false)
		stream = freopen(t->outfile, "w", stdout );
            else
		stream = freopen(t->outfile, "a", stdout );
        }

	exitstatus = shellcmd(t);
	if(NULL != stream)
	{
	    fclose(stdout);
	    freopen("/dev/tty","w",stdout);
	}
	break;

    case CMD_SEMICOLON :
        execute_shellcmd(t->left);
        execute_shellcmd(t->right);
	//print_shellcmd0(t->left); printf("; "); print_shellcmd0(t->right);
	break;

    case CMD_AND :
	exitstatus = execute_shellcmd(t->left);
	if (0 == exitstatus)
		exitstatus = execute_shellcmd(t->right);
	
	//print_shellcmd0(t->left); printf("&& "); print_shellcmd0(t->right);
	break;

    case CMD_OR :
	exitstatus = execute_shellcmd(t->left);
	if (0 != exitstatus)
		exitstatus = execute_shellcmd(t->right);
	//print_shellcmd0(t->left); printf("|| "); print_shellcmd0(t->right);
	break;

    case CMD_SUBSHELL :
	printf("( "); print_shellcmd0(t->left); printf(") ");
	print_redirection(t);
	break;

    case CMD_PIPE :
	execute_shellcmd(t->left);
	print_shellcmd0(t->left); printf("| "); print_shellcmd0(t->right);
	break;
	//background command execute
    case CMD_BACKGROUND :
        execute_shellcmd(t->left);

	if(daemon_init() == -1)
	{
	    printf("can't fork self\n");
    	    exit(0);
        }
        openlog("daemontest", LOG_PID, LOG_USER);
        syslog(LOG_INFO, "program started.");
        signal(SIGTERM, sig_term); // arrange to catch the signal 
        while(1)
        {
            execute_shellcmd(t->right);
        }

	//print_shellcmd0(t->left); printf("& "); print_shellcmd0(t->right);
	break;

    default :
	fprintf(stderr, "%s: invalid CMDTYPE in print_shellcmd0()\n", argv0);
	exit(EXIT_FAILURE);
	break;
    }
	
	//exitstatus	= EXIT_SUCCESS;
    }

    return exitstatus;
}

//单独执行一个命令
int shellcmd(SHELLCMD * t)
{
    int  exitstatus = EXIT_SUCCESS;

    if ( !strcmp(t->argv[0],"quit") || !strcmp(t->argv[0],"exit"))
    {
	fprintf(stderr,"Exit MyShell, Goodbye!\n\n"); 
        exit (0);
    }
    else if(!strcmp(t->argv[0],"ls"))
	my_ls(t->argc,t->argv);
    else if(!strcmp(t->argv[0],"time"))
	my_time();
    else if(t->argv[0][0] == '/')
	execv(t->argv[0],t->argv);
    else if(!strcmp(t->argv[0],"cd"))
	my_cd(t->argc,t->argv);
    else
	{fprintf(stderr, "%s: command not fount!\n", t->argv[0]);
	exitstatus = EXIT_FAILURE;}
    return exitstatus;

}

//list directroy; e.g. ls; ls /opt
int my_ls(int argc , char ** argv)
{

    DIR *dirptr = NULL;
    struct dirent *entry;
    char dirpath[100];
    if(argc<2)
        strcpy(dirpath ,".");
    else
        strcpy(dirpath, argv[1]);
    if(argc>2)
    {
        printf("the program can only deal with one dir at once\n");
        return 1;
    }
    if((dirptr = opendir(dirpath)) == NULL)
    {
        printf("open dir error !\n");
        return 1;
    }
    else
    {
        while (entry = readdir(dirptr))
        {
        printf("%s ", entry->d_name);
        }
    printf("\n");
    closedir(dirptr);
    }
}

//change directory; e.g. cd; cd /opt
int my_cd(int argc, char ** argv)
{
        int dirptr;
        char dirpath[100];
        if (argv[1] == NULL)
                strcpy(dirpath, "/tmp");
        else
                strcpy(dirpath, argv[1]);
        dirptr = chdir(dirpath);
        
        return 0;
}

//time function; e.g time
int my_time()
{
    struct  timeval    tv;
    gettimeofday(&tv,NULL);

    printf("tv_sec:%d\n",tv.tv_sec);
    printf("tv_usec:%d\n",tv.tv_usec);
    return 0;
}
//background function
int daemon_init(void)
{
    pid_t pid;
    if((pid = fork()) < 0)
        return(-1);
    else if(pid != 0)
        exit(0); // parent exit 
    // child continues 
    setsid(); 
    chdir("/"); 
    umask(0); 
    close(0); 
    close(1); 
    close(2); 
    return(0);
}

void sig_term(int signo)
{
    if(signo == SIGTERM)
    // catched signal sent by kill(1) command
    {
        syslog(LOG_INFO, "program terminated.");
        closelog();
        exit(0);
    }
}

