#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>



#if defined(__linux__)
    extern	char	*strdup(const char *str);
    extern	int	fileno(const FILE *fp);
#endif

// ----------------------------------------------------------------------

#define	DEFAULT_HOME		"/tmp"
#define	DEFAULT_PATH		"/bin:/usr/bin:/usr/local/bin:."
#define	DEFAULT_CDPATH		".:.."

#define COMMENT_CHAR		'#'	// comment character
#define HOME_CHAR		'~'	// home directory character

//  ----------------------------------------------------------------------

//  AN enum IN C99 'GENERATES' A SEQUENCE OF UNIQUE, ASCENDING CONSTANTS
typedef enum {
	CMD_COMMAND = 0,	// an actual command node itself
	CMD_SEMICOLON,		// as in   cmd1 ;  cmd2
	CMD_AND,		// as in   cmd1 && cmd2
	CMD_OR,			// as in   cmd1 || cmd2
	CMD_SUBSHELL,		// as in   ( cmds )
	CMD_PIPE,		// as in   cmd1 |  cmd2
	CMD_BACKGROUND		// as in   cmd1 &
} CMDTYPE;


typedef	struct sc {
    CMDTYPE	type;		// the type of the node, &&, ||, etc

    int		argc;		// the number of args iff type == CMD_COMMAND
    char	**argv;		// the NULL terminated argument vector

    char	*infile;	// as in    cmd <  infile
    char	*outfile;	// as in    cmd >  outfile
    bool	append;		// true iff cmd >> outfile

    struct sc	*left, *right;	// pointers to left and right sub-shellcmds
} SHELLCMD;


extern SHELLCMD	*parse_shellcmd(FILE *);	// in parser.c
extern void	free_shellcmd(SHELLCMD *);	// in parser.c
extern int	execute_shellcmd(SHELLCMD *);	// in execute.c
extern int	shellcmd(SHELLCMD *);	// in execute.c
extern int 	my_ls(int argc,char ** argv);
extern int 	my_cd(int argc,char ** argv);
extern int 	my_time();
extern void sig_term(int signo);
extern int daemon_init(void);


/* The global variable HOME points to a directory name stored as a
   character string. This directory name is used to indicate two things:

    - the directory used when the  cd  command is requested without arguments.
    - replacing the leading '~' character in args,  e.g.  ~/filename

   The HOME variable is initialized with the value inherited from the
   invoking environment (or DEFAULT_HOME if undefined).
 */

extern	char	*HOME;

/* The global variables PATH and CDPATH point to character strings representing
   colon separated lists of directory names.

   The value of PATH is used to search for executable files when a command
   name does not contain a '/'

   Similarly, CDPATH provides a colon separated list of directory names
   that are used in an attempt to chage the current working directory.
 */

extern	char	*PATH;
extern	char	*CDPATH;

extern	char	*argv0;		// The name of the shell, typically myshell
extern	bool	interactive;	// true if myshell is connected to a 'terminal'


//  ----------------------------------------------------------------------

//  TWO PRE-PROCESSOR MACROS THAT MAY HELP WITH DEBUGGING YOUR CODE.
//      check_allocation(p) ENSURES THAT A POINTER IS NOT NULL, AND
//      print_shellcmd(t)  PRINTS THE REQUESTED COMMAND-TREE
//  THE TWO ACTUAL FUNCTIONS ARE DEFINED IN globals.c

#define	check_allocation(p)	\
	check_allocation0(p, __FILE__, __func__, __LINE__)
extern	void check_allocation0(void *p, char *file, const char *func, int line);

#define	print_shellcmd(t)	\
	printf("called from %s, %s() line %i:\n", __FILE__,__func__,__LINE__); \
	print_shellcmd0(t)
extern	void print_shellcmd0(SHELLCMD *t);
extern void print_redirection(SHELLCMD *t);
