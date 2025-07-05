/*
 * Copyright (C) 1999-2005, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*********************************************************************
 *                                                                   *
 *   Visualization of transition-matrices of Tangram Models.         *
 *                                                                   *
 *********************************************************************/

/* General purpose header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h> /* difftime */

#include <getopt.h> /* parse command line */


/* X-Windows specific header files */
#include <X11/Xlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Core.h>
#include <X11/Shell.h>

/* stat specific header files */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



/* Data structures header files */
#include "stack.c"
#include "llist.c"

/* Extra header files */
#include "sparseMatrix.h"

/* Boolean values */
#define TRUE    1
#define FALSE   0

/* Buffer sizes */
#define MAXLINESIZE 1000

/* File Extensions */
#define EXT_VSTAT ".vstat"
#define EXT_PERM_STATES ".perm_states"
#define EXT_STATES ".states"
#define EXT_GENERATOR_MTX ".generator_mtx"
#define EXT_GENERATOR_MTX_PARAM ".generator_mtx.param"
#define EXT_REWARD ".rate_reward."
#define EXT_BLOCKS ".partition"
#define EXT_TGIF ".out.obj"
char EXT_ST_TRANS_PROB_MTX_DEFAULT[] = ".st_trans_prob_mtx"; /* this one can be changed */
char EXT_ST_TRANS_PROB_MTX[MAXLINESIZE] = ".st_trans_prob_mtx";

#define EXT_NEWVSTAT ".vstat.new"
#define EXT_NEWSTATES ".states.new"

char EXT_NEWST_TRANS_PROB_MTX_DEFAULT[] = ".st_trans_prob_mtx.new"; /* this one can be changed */
char *EXT_NEWST_TRANS_PROB_MTX = EXT_NEWST_TRANS_PROB_MTX_DEFAULT;

#define EXT_NEWGENERATOR_MTX ".generator_mtx.new"
#define EXT_NEWGENERATOR_MTX_PARAM ".generator_mtx.param.new"

/* Return Values for the Program */
#define OK_EXIT            0    /* Successful conclusion of the program */
#define ERR_PARAM         -1    /* Incorrect usage of parameters */
#define ERR_FILE          -2    /* File couldn't be opened */
#define ERR_MEMORY        -3    /* Memory allocation error */
#define ERR_PIPE          -4    /* Pipe opening error */

/* Color specific data */
#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define BLUE    4
#define YELLOW  5
#define CYAN    6
#define PURPLE  7

/* Zoom out strategies */
#define ZO_MAXIMUM 0
#define ZO_MINIMUM 1
#define ZO_AVERAGE 2

/* Useful macros */
#define memfree(x) if(x != NULL) { free(x); x = NULL; } /* No-SegFault free macro */
#define skip(x, y) while(*x == y) x++;                  /* Skips element y in vector x */
#define find(x, y) while(*x != y) x++;                  /* Finds element y in vector x */
#define max(a, b) (a > b ? a:b)
#define min(a, b) (a < b ? a:b)

/* Structure Definitions */
/* Structure for arguments information - keeps record of the option set by the user when
   the program was called */
struct argsinfo {
    char FileName[MAXLINESIZE];            /* The prefix for all file-opening operations */
    char PermutationString[MAXLINESIZE];   /* The string with the permutation of states */
    char ColorPermutationString[MAXLINESIZE];   /* The string with the permutation of states */
	
    int StartColor;            /* The low probability color */
    int FinalColor;            /* The high probability color */
    int BackgroundColor;       /* The zero probability color */
	int ColorDepth;            /* The color depth of the gradient */
    int GenerateOutputFiles;   /* The option to generate remapped files */
    int ShowMatrix;            /* The option to show the matrix on screen */
    int ZoomOutStrategy;       /* The zoom out strategy for the matrix visualization */
    char *ProgramPath;
	int PreColors;
	int UseSpecialColor;	   /* Use a special color for the elements defined as follow: */
	int SpecialColor;          /* SpecialColor is the color associated to the elements ... */
	float SpecialColorLowThr;    /* ... with probability higher than SpecialColorLowThr and ... */
	float SpecialColorUpThr;	   /* ... lower than SpecialColorUpThr. */
	int UsePermutationFile;
	int PrintScreen;
	char PrintScreenOutFile[MAXLINESIZE];
	char Legend[MAXLINESIZE];	
	
	
	float PreDefinedPixelsPerState;
};

/* State structure for keeping information about each state */
struct state {
    int id;     /* the state number */
    int *tuple; /* the tuple of variables that define the state */
	int old_id;
    int *old_tuple; /* the tuple of variables that define the state */	
};

/* State Variable structure */
struct varinfo {
    char *varname;  /* the variable name */
    int maxvalue;   /* from <model>.maxvalues */
};

/* Model Information structure - used during the whole program to retrieve */
/* model-related information */
struct modelinfo {
    char *ModelName;            /* the base name that prefixes all files */
    int nvars;                  /* the number of variables in the model */
    struct varinfo *Variables;  /* array state variables in the model */
    int *PermutationVector;     /* array with the permutation being used */
    int nstates;                /* the number of states in the model */
    struct state *ModelStates;  /* array with the states in the model */
    int *StatesMapVector;       /* array that maps old states into new ones */
    int ntrans;                 /* the number of transition in the matrix */
    struct matrixcell **Matrix; /* the matrix being shown */
    int isLiteral;              /* boolean `is the model literal?` */
    int hasPartition;           /* boolean `does the model have a partition?` */
    int nrewards;               /* the number of rewards found in the directory */
    char **Rewards;             /* array with reward names */
	struct argsinfo *argsinfo;  /* options set by the user when  the program was called */
    XColor *colors;	
	XColor *precolors;	
	int *ColorPermutationVector;
};

/* Matrix Cell structure - one position in the matrix, keeps record of one */
/* transition and its associated probability */
struct matrixcell {
    double value;
	int level;
    int state_id;
    struct matrixcell *next;
};

/* Color structure - keeps RGB values */
struct color {
    int red, green, blue;
};

/* Typedefs */
typedef struct argsinfo ArgsInfo;
typedef struct varinfo VarInfo;
typedef struct modelinfo ModelInfo;
typedef struct state State;
typedef struct matrixcell MatrixCell;
typedef struct color Color;



/* Function Prototypes */

void save_matrix(Display *display, XtPointer client);
void ColorAllocation(Display *display, Colormap cmap, Window matrix_window, ModelInfo *Model, GC **gc, GC **pregc);
void ColorFree(Display *display, ModelInfo *Model, GC **gc);



#define NOPREGCS 14

/* Strings with the main color names */
char ColorNames[][NOPREGCS] = { "BLACK", "WHITE", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "PURPLE", "LIGHTRED", "LIGHTGREEN", "LIGHTBLUE", "LIGHTYELLOW", "LIGHTCYAN", "LIGHTPURPLE" };



/* RGB data for the main colors */
Color ColorData[NOPREGCS] = { { 0x0000, 0x0000, 0x0000 },  /* Black */
                       { 0xFF00, 0xFF00, 0xFF00 },  /* White */
                       { 0xFF00, 0x0000, 0x0000 },  /* Red */
                       { 0x0000, 0xFF00, 0x0000 },  /* Green */
                       { 0x0000, 0x0000, 0xFF00 },  /* Blue */
                       { 0xFF00, 0xFF00, 0x0000 },  /* Yellow */
                       { 0x0000, 0xFF00, 0xFF00 },  /* Cyan */
                       { 0xFF00, 0x0000, 0xFF00 }, /* Purple */
                       { 0x9900, 0x0000, 0x0000 },  /* Light Red */
                       { 0x0000, 0x9900, 0x0000 },  /* Light Green */
                       { 0x0000, 0x0000, 0x9900 },  /* Light Blue */
                       { 0x9900, 0x9900, 0x0000 },  /* Light Yellow */
                       { 0x0000, 0x9900, 0x9900 }, /* Light Cyan */
                       { 0x9900, 0x0000, 0x9900 } /* Light Purple */
		        };  
					

void ParseArgs(ArgsInfo *Options, int argc, char **argv);
void ReadModel(ModelInfo *Model, ArgsInfo *Options);


int Compare2(const void *x, const void *y, int n);

#ifdef DEBUG
void ShowArgsInfo(ArgsInfo *Options);
void ShowModelInfo(ModelInfo *Model);
#endif

void WriteNewVStat(ModelInfo *Model, ArgsInfo *Options);
void WriteNewStates(ModelInfo *Model, ArgsInfo *Options);
void WriteNewGeneratorMtx(ModelInfo *Model, ArgsInfo *Options);
void WriteNewGeneratorMtxParam(ModelInfo *Model, ArgsInfo *Options);
void WriteNewStTransProbMtx(ModelInfo *Model, ArgsInfo *Options);
void WriteNewRewards(ModelInfo *Model, ArgsInfo *Options);

void UpdateFile(char *Name, char *Ext);
void UpdateRewards(ModelInfo *Model, ArgsInfo *Options);

void DrawMatrix(ModelInfo *Model, ArgsInfo *Options);

void UsageMessage(char *programname);

int Compare(const void *x, const void *y);
FILE *OpenFile(char *Name, char *Ext, char *mode);

void UpdateMenuItems(Widget *menuItems, ModelInfo *model, int state);

int PC=FALSE;

void ProbTableFree();

void LoadPartitions(ModelInfo *Model);


/**********************************************************************
 *                                                                    *
 *  Some global variables                                             *
 *                                                                    *
 **********************************************************************/



#define WINDOW_SIZE 400

int NOGCS = -1;
int NSTATES = -1;
int ZO = -1;
MatrixCell **MATRIX = NULL;
int size = 0;
GC *gc, *pregc, invert_gc;
typedef struct
{
    double prob;
    int weight;
	int level;
} ProbData;

ProbData **ProbTable = NULL; // auxiliar table to store probabilities


int x = 1, y = 1;


/**********************************************************************
 *                                                                    *
 *  Main Function                                                     *
 *                                                                    *
 **********************************************************************/
int main(int argc, char **argv)
{	
 	int i = 0;
    ArgsInfo Options;
    ModelInfo Model;

    /* Read command-line options */
    ParseArgs(&Options, argc, argv);

    /* Reading model information */
    ReadModel(&Model, &Options);

    /* If new output was requested for the whole model */
    if(Options.GenerateOutputFiles)
    {
    
        /* Create temporary files to keep the new data */
        WriteNewVStat(&Model, &Options);
        WriteNewStates(&Model, &Options);
        WriteNewStTransProbMtx(&Model, &Options);
        WriteNewGeneratorMtx(&Model, &Options);
        if(Model.isLiteral)
            WriteNewGeneratorMtxParam(&Model, &Options);
       
        WriteNewRewards(&Model, &Options);

        /* Update old files with the new ones */
        UpdateFile(Options.FileName, EXT_VSTAT);
        UpdateFile(Options.FileName, EXT_STATES);
        UpdateFile(Options.FileName, EXT_ST_TRANS_PROB_MTX);
        UpdateFile(Options.FileName, EXT_GENERATOR_MTX);
        if(Model.isLiteral)
            UpdateFile(Options.FileName, EXT_GENERATOR_MTX_PARAM);
        UpdateRewards(&Model, &Options);

        
    }  
	
    /* preliminar declarations of global variables */
	
	
    NSTATES = Model.nstates;
    MATRIX = Model.Matrix;
    ZO = Options.ZoomOutStrategy;
    size = Model.nstates;	
	x = y = 1;
	
    ProbTable = (ProbData **)malloc(WINDOW_SIZE * sizeof(ProbData *)); 
    for(i = 0; i < WINDOW_SIZE; i++)
        ProbTable[i] = (ProbData *)malloc(WINDOW_SIZE * sizeof(ProbData));
	
	
	
    /* Finally to the point... Draw the Matrix */
    if(Options.ShowMatrix)
	{
        DrawMatrix(&Model, &Options);
	
	}
	else if (Options.PrintScreen)
	{
		Display *display  = XOpenDisplay(NULL);
		int screen = DefaultScreen(display);
		Window matrix_window = RootWindow(display,screen);
		Colormap cmap =  XDefaultColormap(display,screen); 

		LoadPartitions(&Model);
		ColorAllocation(display, cmap, matrix_window, (ModelInfo *)&Model, (GC **)&gc, (GC **)&pregc);
		save_matrix(display, (XtPointer)&Model);
		ColorFree(display, &Model, &gc);			
		ProbTableFree();
	}
	
        


    return(OK_EXIT);
}

/**********************************************************************
 *                                                                    *
 *    ParseArgs(ArgsInfo *Options, int argc, char **argv)             *
 *        Reads the command-line options into the ArgsInfo structure  *
 *        Options - pointer to a previously allocated ArgsInfo struct *
 *        argc - the number of arguments in the args vector           *
 *        argv - the vector with string arguments                     *
 *                                                                    *
 **********************************************************************/


void ParseArgs(ArgsInfo *Options, int argc, char **argv)
{
       int c, i, j;
       char *ptaux, path[MAXLINESIZE+1], colorname[MAXLINESIZE+1];

       /* Set some default options */
       Options->FileName[0] = 0;
       Options->PermutationString[0] = 0;
       memset(Options->ColorPermutationString,0,sizeof(Options->ColorPermutationString));
       Options->StartColor = BLUE;
       Options->FinalColor = RED;
       Options->BackgroundColor = WHITE;
       Options->ColorDepth = 8;
       Options->GenerateOutputFiles = FALSE;
       Options->ZoomOutStrategy = ZO_AVERAGE;
       Options->ShowMatrix = TRUE;
	   Options->UseSpecialColor = 0;
	   Options->UsePermutationFile = 0;
	   Options->PrintScreen = 0;
	   Options->PrintScreenOutFile[0] = 0;
	   Options->Legend[0] = 0;	   
	   Options->PreDefinedPixelsPerState = 0;
	   
       /* Points to the end of the string */
	   
	   strncpy(path, argv[0], MAXLINESIZE);
	   
       ptaux = path + strlen(path);

       while ((*ptaux != '/')&&(ptaux!=path))
           ptaux--;

       if (*ptaux == '/') 
       {
           int lsize;

           lsize = ptaux - path + 1;
           Options->ProgramPath = (char *)malloc(lsize + 1);
           strncpy(Options->ProgramPath, path, lsize);
           Options->ProgramPath[lsize] = '\0';
       }
       else
       {
           FILE *fpwhich;
           char *cmdline, line[MAXLINESIZE+1];

           cmdline = (char *)malloc(strlen("which ")+strlen(argv[0])+1);

           strcpy(cmdline,"which ");
           strcat(cmdline, argv[0]);

           Options->ProgramPath = NULL;
           if ((fpwhich = popen(cmdline,"r"))!=NULL)
           {
		   
               while(fgets(line, MAXLINESIZE, fpwhich) != NULL)
               {        
                   if(strncmp(line, "Warning:", strlen("Warning:")) == 0)
                       continue;
                   if(strncmp(line, "alias", strlen("alias")) == 0)
                       continue;

                   for (i=strlen(line)-1; (i >= 0) && (line[i]!='/'); i--)
                       ;

                   if((i == 0) && (line[0] != '/'))
                       continue;

                   line[i+1]='\0';

                   Options->ProgramPath = (char*)malloc(strlen(line)+1);
                   strcpy(Options->ProgramPath,line);
               }
           }
           if(Options->ProgramPath == NULL)
           {
               Options->ProgramPath = (char *)malloc(3);
               strcpy(Options->ProgramPath, "./");
               fprintf(stderr,"Couldn't find resource file. Using default visual.\n");
           }
           memfree(cmdline);
       }

     
       while (1)
        {
           static struct option long_options[] =
             {
               {"fn",  required_argument, 0, 'f'}, /* File name - necessary parameter */
               {"ps",  required_argument, 0, 'p'}, /* Permutation String - optional parameter */
               {"pf",    no_argument,     0, 'u'}, /* Use Permutation File */
               {"ex",    required_argument, 0, 'e'},  /* Probabilities file extension  */
               {"zo",    required_argument, 0, 'z'},  /* Zoom out strategy  */
               {"gf",    no_argument, 0, 'g'},  /* Output flag  */
               {"ng",    no_argument, 0, 'n'}, 		  /* Don't show matrix */
               {"cd",    required_argument, 0, 'c'},  /* Color depth  */
               {"sc",    required_argument, 0, 's'},  /* Low probability color */
               {"fc",    required_argument, 0, 'i'},  /* High probability color */
               {"bc",    required_argument, 0, 'b'},  /* Background color */
               {"spec",    required_argument, 0, 'a'},
               {"print",    required_argument, 0, 't'},
               {"legend",    required_argument, 0, 'l'},			   
               {"pixelst",    required_argument, 0, 'x'},		   
			   {"pc", no_argument, 0, 'w'},
			   {"cs", required_argument, 0, 'y'},			   
               {0, 0, 0, 0}
             };
			 
           /* getopt_long stores the option index here. */
           int option_index = 0;
     
           c = getopt_long_only (argc, argv, "f:p:u:e:z:g:nc:s:i:b:a:t:l:",
                            long_options, &option_index);
     
           /* Detect the end of the options. */
           if (c == -1)
             break;
     
           switch (c)
             {
             case 0:
               /* If this option set a flag, do nothing else now. */
               if (long_options[option_index].flag != 0)
                 break;
               printf ("option %s", long_options[option_index].name);
               if (optarg)
                 printf (" with arg %s", optarg);
               printf ("\n");
               break;


			 case 'f':  /* File name - necessary parameter */
            	 strncpy(Options->FileName,optarg, MAXLINESIZE);
				 break;
				 
			 case 'p': /* Permutation String  */
			  	 strncpy(Options->PermutationString,optarg, MAXLINESIZE);
			 	 break;
				 
			 case 'y': /* Color Permutation String  */
			  	 strncpy(Options->ColorPermutationString,optarg, MAXLINESIZE);
			 	 break;
				
				 
				
			 case 'u': /* Use Permutation File */
				 Options->UsePermutationFile = 1;
				 break;
				 
			 case 'e':  /* Probabilities file extension  */

            	 strncpy(EXT_ST_TRANS_PROB_MTX, optarg, MAXLINESIZE);
            	 EXT_NEWST_TRANS_PROB_MTX = (char *)malloc(strlen(EXT_ST_TRANS_PROB_MTX)+strlen(".new")+1);
            	 strcpy(EXT_NEWST_TRANS_PROB_MTX, EXT_ST_TRANS_PROB_MTX);
            	 strcat(EXT_NEWST_TRANS_PROB_MTX, ".new");
            	 break;
				 
			 case 'z': /* Zoom out strategy  */
				 
            	 Options->ZoomOutStrategy = atoi(optarg);
            	 break;
			 
			 case 'g': /* Output flag */
			 
            	 fprintf(stderr, "WARNING: Due to the particular way in which markfind generates\n");
            	 fprintf(stderr, "         the model states there is no guarantee whatsoever that\n");
            	 fprintf(stderr, "         your initial information (matrices, etc) represents a\n");
            	 fprintf(stderr, "         permutation.\n");
            	 fprintf(stderr, "         Once you determine a permutation of states with this program\n");
            	 fprintf(stderr, "         and convert it with the new states tuples, the only way\n");
            	 fprintf(stderr, "         to retrieve the original states is re-generating the model\n");
            	 fprintf(stderr, "         chain.\n");
            	 Options->GenerateOutputFiles = TRUE;
				 break;

			 case 'n': /* Don't show matrix */
			 
 				 Options->ShowMatrix = FALSE;
				 break;
			 
			 case 'c':    /* Color depth  */
			 
            	 Options->ColorDepth = atoi(optarg);
            	 break;

			 case 's':   /* Low probability color  */

            	 for(j = BLACK; j <= PURPLE; j++)
            	 {
                	 if(strcasecmp(optarg, ColorNames[j]) == 0)
                	 {
                    	 Options->StartColor = j;
                    	 break;
                	 }
            	 }
            	 break;

			case 'i':     /* High probability color  */

            	 for(j = BLACK; j <= PURPLE; j++)
            	 {
                	 if(strcasecmp(optarg, ColorNames[j]) == 0)
                	 {
                    	 Options->FinalColor = j;
                    	 break;
                	 }
            	 }
            	 break;
				 
			 case 'b':   /* Background color  */

            	 for(j = BLACK; j <= PURPLE; j++)
            	 {
                	 if(strcasecmp(optarg, ColorNames[j]) == 0)
                	 {
                    	 Options->BackgroundColor = j;
                    	 break;
                	 }
            	 }
            	 break;
			
			 case 'a':
			 
			 	 if (sscanf(optarg, " %s %f %f ", colorname, &Options->SpecialColorLowThr, &Options->SpecialColorUpThr)!=3)
				 {
        				UsageMessage(argv[0]);
        				exit(ERR_PARAM);
				 }

            	 for(j = BLACK; j <= PURPLE; j++)
            	 {
                	 if(strcasecmp(colorname, ColorNames[j]) == 0)
                	 {
                    	 Options->SpecialColor = j;
                    	 break;
                	 }
            	 }


				 Options->UseSpecialColor = 1;


				 break;
				
			 case 't': 

				 Options->PrintScreen = 1;
				 
            	 strncpy(Options->PrintScreenOutFile, optarg, MAXLINESIZE);		
				 
				 break;
				 
			 case 'x' :
			 			
            	 Options->PreDefinedPixelsPerState = atof(optarg);
				 
				 break;	 
				 
			 case 'l': 				 
			 
			 	 strncpy(Options->Legend, optarg, MAXLINESIZE);		
			 
			 	 break;
			
			 case 'w':
			     
		  	     Options->PreColors = TRUE;
			     PC = Options->PreColors;
				 
 			 	 break;

            default:
              abort ();
            }
        }

     
       /* Print any remaining command line arguments (not options). */
       if (optind < argc)
         {
           printf ("non-option ARGV-elements: ");
           while (optind < argc)
             printf ("%s ", argv[optind++]);
           putchar ('\n');
         }
     



    /* If the user didn't provide a filename */
    if(Options->FileName[0] == 0)
    {
        fprintf(stderr, "File Name missing\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }

    /* If the user didn't provide a permutation string */
    if(Options->PermutationString[0] == 0)
    {
        fprintf(stderr, "Reading states information without any permutation\n");
    }

    /* If any of the colors are invalid */
    if((Options->StartColor < BLACK) || (Options->StartColor > PURPLE))
    {
        fprintf(stderr, "Invalid Starting Color\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }
    if((Options->FinalColor < BLACK) || (Options->FinalColor > PURPLE))
    {
        fprintf(stderr, "Invalid Final Color\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }
    if((Options->BackgroundColor < BLACK) || (Options->BackgroundColor > PURPLE))
    {
        fprintf(stderr, "Invalid Background Color\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }

    /* If color depth is invalid */
    if((Options->ColorDepth < 1) || (Options->ColorDepth > 8))
    {
        fprintf(stderr, "Invalid Color Depth\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }

    /* If zoom out strategy is invalid */
    if((Options->ZoomOutStrategy < 0) || (Options->ZoomOutStrategy > 2))
    {
        fprintf(stderr, "Invalid Zoom Out Strategy\n");
        UsageMessage(argv[0]);
        exit(ERR_PARAM);
    }

#ifdef DEBUG
    ShowArgsInfo(Options);
#endif


}



#ifdef DEBUG
/**********************************************************************
 *                                                                    *
 *  ShowArgsInfo(const void *x, const void *y)                        *
 *      Prints out the information about command-line parsed          *
 *      Options - the arguments information                           *
 *                                                                    *
 **********************************************************************/
void ShowArgsInfo(ArgsInfo *Options)
{
    fprintf(stderr, "Command-line arguments:\n");
    fprintf(stderr, "   Options = {\n");
    fprintf(stderr, "       FileName             = %s\n", Options->FileName);
    fprintf(stderr, "       PermutationString    = %s\n", (Options->PermutationString[0] ? Options->PermutationString : "NONE"));
    fprintf(stderr, "       StartColor           = %s\n", ColorNames[Options->StartColor]);
    fprintf(stderr, "       FinalColor           = %s\n", ColorNames[Options->FinalColor]);
    fprintf(stderr, "       BackgroundColor      = %s\n", ColorNames[Options->BackgroundColor]);
    fprintf(stderr, "       ColorDepth           = %d\n", Options->ColorDepth);
    fprintf(stderr, "       GenerateOutputFiles  = %s\n", (Options->GenerateOutputFiles ? "TRUE":"FALSE"));
    fprintf(stderr, "       ShowMatrix           = %s\n", (Options->ShowMatrix ? "TRUE":"FALSE"));
    fprintf(stderr, "       ZoomOutStrategy      = %s\n",
            (Options->ZoomOutStrategy == 0 ? "MAXIMUM":(Options->ZoomOutStrategy == 1 ? "MINIMUM":"AVERAGE")));
    fprintf(stderr, "       UseSpecialColor      = %d\n", Options->UseSpecialColor);
	if (Options->UseSpecialColor)
	{
	    fprintf(stderr, "              SpecialColor      = %s\n", ColorNames[Options->SpecialColor]);	
	    fprintf(stderr, "              Lower Threshold   = %f\n", Options->SpecialColorLowThr);			
	    fprintf(stderr, "              Upper Threshold   = %f\n", Options->SpecialColorUpThr);					
	}
	
	fprintf(stderr, "      UsePermutationFile = %d\n", Options->UsePermutationFile);
	fprintf(stderr, "      PrintScreen = %d\n", Options->PrintScreen);	
	fprintf(stderr, "      PrintScreenOutFile = %s\n", Options->PrintScreenOutFile);		

    fprintf(stderr, "   }\n");
}
#endif

/**********************************************************************
 *                                                                    *
 *    UsageMessage(char *programname)                                 *
 *        Prints out a message to instruct on how to use this program *
 *        programname - a string holding the program's name           *
 *                                                                    *
 **********************************************************************/
void UsageMessage(char *programname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s -fn <model name> -ps \"<permutation string>\" [OPTIONS]\n", programname);
    fprintf(stderr, "\n");
    fprintf(stderr, "    -fn <model name> - self-explanatory\n");
    fprintf(stderr, "    -ps <permutation string> - the permutation of states (e.g.: \"3 0 1 2\")\n");
    fprintf(stderr, "[OPTIONS]:\n");
    fprintf(stderr, "    -sc <starting color> - the low-probability color value      (default: BLUE)\n");
    fprintf(stderr, "    -fc <final color> - the high-probability color value        (default: RED)\n");
    fprintf(stderr, "    -bc <background color> - the zero-probability color value   (default: WHITE)\n");
    fprintf(stderr, "    -cd <color depth> - the color depth within the range 1 to 8 (default: 8)\n");
    fprintf(stderr, "    -ex <extension> - the extension for the probabilities file  (default: .st_trans_prob_mtx)\n");
    fprintf(stderr, "    -zo <strategy> - the zoom out strategy                      (default: 2)\n");
    fprintf(stderr, "    -gf - generate output files\n");
    fprintf(stderr, "    -ng - don't show the matrix\n");
    fprintf(stderr, "    -spec <color lower threshold upper threshold> - special color to mark \n");
	fprintf(stderr, "          the cells which have probabilities in the interval between the lower \n");
	fprintf(stderr, "          and the upper threshold  (e.g., \"WHITE  0 0.001\")   (default: NONE)\n");	
	fprintf(stderr, "    -pf - use permutation input file  (.perm_states)            (default: NONE)\n");
	fprintf(stderr, "    -print <file name> - print the matrix to a tgif .obj file   (default: NONE)\n");
	fprintf(stderr, "    -legend <legend> - the legend to print in the .obj file     (default: NONE)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Obs(1): Color values are strings that can be chosen from the following set:\n");
    fprintf(stderr, "        { BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, PURPLE }\n");
    fprintf(stderr, "Obs(2): The color depth parameter specifies the actual number of colors\n");
    fprintf(stderr, "        in the gradient from the starting color the final color\n");
    fprintf(stderr, "Obs(3): The zoom out strategy is one of the following:\n");
    fprintf(stderr, "        { 0 = MAXIMUM, 1 = MINIMUM, 2 = AVERAGE }\n");
    fprintf(stderr, "Obs(4): The permutation file has 3 columns.  The first 2 columns follow the\n");
	fprintf(stderr, "        standard of *.states.  The last column gives the original state number.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "WARNING: Due to the particular way in which markfind generates\n");
    fprintf(stderr, "         the model states there is no guarantee whatsoever that\n");
    fprintf(stderr, "         your initial information (matrices, etc) represents a\n");
    fprintf(stderr, "         permutation.\n");
    fprintf(stderr, "         Once you determine a permutation of states with this program\n");
    fprintf(stderr, "         and convert it with the new states tuples, the only way\n");
    fprintf(stderr, "         to retrieve the original states is re-generating the model\n");
    fprintf(stderr, "         chain.\n");
}


/**********************************************************************
 *                                                                    *
 *  ReadStates(ModelInfo *Model, int where)                           *
 *      Loads the states into the memory (*.EXT_STATES file)          *
 *      Model - the modelinfo struct to be filled                     *
 *      where - where to write the information (1=old_tuple, 0=tuple) *
 *                                                                    *
 **********************************************************************/

int ReadStates(ModelInfo *Model, int where)
{
	    FILE *fpin = NULL;
		int i = 0, count = 0, j = 0;
	    char line[MAXLINESIZE],  *ptstart, *ptend;
		
		
		
    	fpin = OpenFile(Model->argsinfo->FileName, EXT_STATES, "r");

    	while(!feof(fpin))
    	{
        	if(fgets(line, MAXLINESIZE, fpin) == NULL)
        	   break;

        	if(line[strlen(line)-1] == '\n')
            	line[strlen(line)-1] = '\0';

        	if(line[0] == '\0') /* Blank line */
            	 break;
			
			if (where == 0)
			{

        		Model->nstates++;
        		Model->ModelStates = (State *)realloc(Model->ModelStates, (Model->nstates+1) * sizeof(State));
	        	Model->ModelStates[Model->nstates].tuple = (int *)malloc(Model->nvars * sizeof(int));
			}
			else
			{
				count++;			
	        	Model->ModelStates[count].old_tuple = (int *)malloc(Model->nvars * sizeof(int));			

			}

        	ptstart = ptend = line;
        	find(ptend, '(');
        	*ptend = '\0';

			if (where == 0)
	        	Model->ModelStates[Model->nstates].id = atoi(ptstart); /* Model->nstates; */

        	for(i = 0; i < Model->nvars; i++)
        	{
            	ptstart = ++ptend;

            	while(isdigit(*ptend))
                	ptend++;

            	*ptend = '\0';
				
				if (where == 0)
				{
	            	Model->ModelStates[Model->nstates].tuple[Model->PermutationVector[i]] = atoi(ptstart);
				}
				else
				{
	            	Model->ModelStates[count].old_tuple[i] = atoi(ptstart);
				}
        	}
			
			
			/* integrity check */
			
			if (where == 1)
			{
			
				for (j = 1; j <= Model->nstates; j ++)
				{
					if (Compare2(&(Model->ModelStates[j]), &(Model->ModelStates[count]), Model->nvars) == 0)
					{
						if ( Model->ModelStates[j].old_id != count )
						{
							fprintf(stderr,"integrity error: old id, %d new id, %d\n\n", Model->ModelStates[j].old_id , count );
							exit(1);
						}
						else
						{
							break;
						}
					}

				}

			}
			
			/* end of integrity check */			
			
    	}

    	fclose(fpin);
		
		
		return 0;
}

/**********************************************************************
 *                                                                    *
 *  int ReadPermStates(ModelInfo *Model)                              *
 *      Loads the permutation of states into the memory               *
 *      Model - the modelinfo struct to be filled                     *
 *                                                                    *
 **********************************************************************/

int ReadPermStates(ModelInfo *Model)
{
	    FILE *fpin = NULL; int i = 0;  char *ptstart, *ptend,  line[MAXLINESIZE];

		Model->StatesMapVector	= NULL;

    	fpin = OpenFile(Model->argsinfo->FileName, EXT_PERM_STATES, "r");
		
		Model->nstates = 0;

    	while(!feof(fpin))
    	{
		
			
        	if(fgets(line, MAXLINESIZE, fpin) == NULL)
        	   break;

        	if(line[strlen(line)-1] == '\n')
            	line[strlen(line)-1] = '\0';

        	if(line[0] == '\0') /* Blank line */
            	 break;
				 

        	Model->nstates++;
        	Model->ModelStates = (State *)realloc(Model->ModelStates, (Model->nstates+1) * sizeof(State));
			

        	Model->ModelStates[Model->nstates].tuple = (int *)malloc(Model->nvars * sizeof(int));

        	ptstart = ptend = line;
			

			
        	find(ptend, '(');
        	*ptend = '\0';

        	Model->ModelStates[Model->nstates].id = atoi(ptstart); /* Model->nstates; */
			

			
        	for(i = 0; i < Model->nvars; i++)
        	{
            	ptstart = ++ptend;

            	while(isdigit(*ptend))
                	ptend++;

            	*ptend = '\0';
            	Model->ModelStates[Model->nstates].tuple[i] = atoi(ptstart);
        	}
			
			ptend++;
			
			sscanf( ptend, " %d ", &(Model->ModelStates[Model->nstates].old_id) );
			

    	}
		return 0;
}


/**********************************************************************
 *                                                                    *
 *  ReadModel(ModelInfo *Model, ArgsInfo *Options)                    *
 *      Loads a model into the memory                                 *
 *      Model - the modelinfo struct to be filled                     *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void ReadModel(ModelInfo *Model, ArgsInfo *Options)
{
    int  prob_mtx_needs_update = 0;
    FILE *fpin=NULL, *fpin1=NULL, *fpout=NULL;
    char line[MAXLINESIZE], line1[MAXLINESIZE], *dirstring, *modelreward, *ptaux, *filename;
    char outFileName[ 256 ];
    char transname[ 256 ];
    DIR *dir_stream;
    struct dirent *dirinfo;
    int isize, i, origstate, deststate, deststate1, origstate1, level;
    double prob;
    double lambda;
    MatrixCell *ptcell;
    Matrix *P, *Q;
	
	
    Model->argsinfo = Options;		
    
    
    /*****************************************
     * Look for .generator_mtx.param         *
     *****************************************/
    filename = (char*)malloc(strlen(Options->FileName)+strlen(EXT_GENERATOR_MTX_PARAM)+1);
    strcpy(filename, Options->FileName);
    strcat(filename, EXT_GENERATOR_MTX_PARAM);
    fpin = fopen(filename, "r");

    if(fpin != NULL)
    {
        Model->isLiteral = TRUE;
        fclose(fpin);
    }
    else
        Model->isLiteral = FALSE;

    memfree(filename);
    

    /*********************************
     * Read the state variables      *
     *********************************/
    Model->nvars = 0;
    Model->Variables = NULL;

    ptaux = Options->FileName + strlen(Options->FileName);
    while((*ptaux != '/') && (ptaux != Options->FileName))
        ptaux--;

    if(*ptaux == '/')
        ptaux++;

    Model->ModelName = ptaux;

    fpin = OpenFile(Options->FileName, EXT_VSTAT, "r");

    while(!feof(fpin))
    {    
        if(fgets(line, MAXLINESIZE, fpin) == NULL) 
            break;

        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';

        if(line[0] == '\0') /* Blank line */
             break;

        Model->nvars++;
        Model->Variables = realloc(Model->Variables, Model->nvars * sizeof(VarInfo));

        Model->Variables[(Model->nvars)-1].varname = (char *)malloc((strlen(line) + 1)*sizeof(char));

        strcpy(Model->Variables[Model->nvars-1].varname, line);
    }
    
    fclose(fpin);

    Model->ColorPermutationVector = (int *)malloc(NOPREGCS * sizeof(int));
    for(i = 0; i < NOPREGCS; i++)
        Model->ColorPermutationVector[i] = i;


    if(Options->ColorPermutationString[0] != 0)
    {


        ptaux = Options->ColorPermutationString;		
				
        for(i = 0 ;  ; i++)
        {
            while(((*ptaux)!=0) && !isdigit(*ptaux))
			{
				fprintf(stderr, "%c ok\n", *ptaux);
                ptaux++;         
			}
		                                   /* Finds a digit */
			if ((*ptaux)==0)
				break;
			
			fprintf(stderr, "%c ok\n", *ptaux);
				
            sscanf(ptaux, "%d", &(Model->ColorPermutationVector[i]));    /* Reads one number */
			
			if ((Model->ColorPermutationVector[i] < 0) || (Model->ColorPermutationVector[i] >= NOPREGCS))
			{
				fprintf(stderr,"Invalid permutation string (error: %s).\n",ptaux);
				exit(1);
			}
			
            while(isdigit(*ptaux))
                ptaux++;                                            /* Skips digits */
        }		
	}
    
    
    if(Options->PermutationString[0] != 0)
    {
        /*********************************
         * Read the permutation          *
         *********************************/
        ptaux = Options->PermutationString;
        Model->PermutationVector = (int *)malloc(Model->nvars * sizeof(int));


        for(i = 0 ; i < Model->nvars ; i++)
        {
            while(!isdigit(*ptaux))
                ptaux++;                                            /* Finds a digit */
            sscanf(ptaux, "%d", &(Model->PermutationVector[i]));    /* Reads one number */
			
			if ((Model->PermutationVector[i] < 0) || (Model->PermutationVector[i] >= Model->nvars))
			{
				fprintf(stderr,"Invalid permutation string (error: %s).\n",ptaux);
				exit(1);
			}
			
            while(isdigit(*ptaux))
                ptaux++;                                            /* Skips digits */
        }
    }
    else
    {
        Model->PermutationVector = (int *)malloc(Model->nvars * sizeof(int));
        for(i = 0; i < Model->nvars; i++)
            Model->PermutationVector[i] = i;
    }

    /*********************************
     * Read the states' tuples       *
     *********************************/
    Model->nstates = 0;
    Model->ModelStates = (State *)malloc(sizeof(State));
    Model->ModelStates[0].id = 0;
    Model->ModelStates[0].tuple = NULL;
	
	
	if (!Options->UsePermutationFile)
	{
	
		ReadStates(Model,0);


    	if(Options->PermutationString[0] != 0)
    	{
        	/*****************************************
        	 * Sorts the states by their tuples      *
        	 *****************************************/
        	qsort(Model->ModelStates + 1, Model->nstates, sizeof(State), Compare);

    	}
    	Model->StatesMapVector = (int *)malloc((Model->nstates+1) * sizeof(int));
    	Model->StatesMapVector[0] = -1;

    	for(i = 1; i <= Model->nstates; i++)
        	Model->StatesMapVector[Model->ModelStates[i].id] = i;
	}
	else
	{

        ReadPermStates(Model);		
		
		ReadStates(Model,1);					
		
    	Model->StatesMapVector = (int *)malloc((Model->nstates+1) * sizeof(int));
    	Model->StatesMapVector[0] = -1;

    	for(i = 1; i <= Model->nstates; i++)
        	Model->StatesMapVector[Model->ModelStates[i].old_id] = i;

	}

    /*****************************************
     * Reads the probability matrix          *
     *****************************************/
     
    
    sprintf( transname, "%s%s", Options->FileName, EXT_ST_TRANS_PROB_MTX );
    fpin = fopen( transname, "r" );
    
    if ( fpin != NULL )
    {
        /* verifying if the probability matrix needs to be updated 
        
           ( if it's older than generator matrix, it needs to be updated ) */
    
        char *rate_mtx_filename = NULL;
        struct stat prob_mtx_stat, rate_mtx_stat;
    
        rate_mtx_filename = (char *)malloc(strlen(Options->FileName)+strlen(EXT_GENERATOR_MTX)+1);
        memset(rate_mtx_filename,0,strlen(Options->FileName)+strlen(EXT_GENERATOR_MTX)+1);
        strcpy(rate_mtx_filename, Options->FileName);
        strcat(rate_mtx_filename, EXT_GENERATOR_MTX);

        if (fstat( fileno(fpin), &prob_mtx_stat ) == -1)
        {
            perror("");
            
        } else if (stat( rate_mtx_filename , &rate_mtx_stat ) == -1)
        {
            fprintf(stderr,"Warning! Probability matrix was found, but rate matrix was not.\n\n");
            
            prob_mtx_needs_update = 0;
            
        } else if (difftime(prob_mtx_stat.st_mtime, rate_mtx_stat.st_mtime)<=0)
        {
            prob_mtx_needs_update = 1;
        }
    }
    
    if( ( fpin == NULL ) || prob_mtx_needs_update )
    {
        fpin = OpenFile( Options->FileName, EXT_GENERATOR_MTX, "r" );
        sprintf( outFileName, "%s%s", Options->FileName, EXT_ST_TRANS_PROB_MTX );
        fpout = fopen( outFileName, "w" );
        if( fpout != NULL )
        {
            Q = get_trans_matrix( fpin );
            generate_diagonal( 'Q', Q );
            P = uniformize_matrix( Q, &lambda );
            print_matrix( P, fpout );
            fclose( fpout );
            
        }  else  {
        
            if ( prob_mtx_needs_update )
            {
                fprintf(stderr,"Warning! Probability matrix may be out of date.  ");
                fprintf(stderr,"The probability matrix is older than the generator matrix, ");
                fprintf(stderr,"but could not be updated.\n\n");  
            
            } else {

                fprintf(stderr,"Warning! Probability matrix not found and could not be generated.");
                fprintf(stderr,"\n\n");  
                
                exit( 33 );
            }
        }
    }

    fclose( fpin );

    Model->Matrix = (MatrixCell **)malloc((Model->nstates+1) * sizeof(MatrixCell *));
    for(i = 0; i <= Model->nstates; i++)
        Model->Matrix[i] = NULL;

    fpin = OpenFile(Options->FileName, EXT_ST_TRANS_PROB_MTX, "r");
    
	    
    if (Model->isLiteral)
    {
	    fpin1 = OpenFile(Options->FileName, EXT_GENERATOR_MTX_PARAM, "r");	
    }

    Model->ntrans = 0;

    if (fgets(line, MAXLINESIZE, fpin)==NULL)
    {
      ; /* error */
    }
    
    if (Model->isLiteral)
    {
	   if (fgets(line, MAXLINESIZE, fpin1) == NULL)
	   {
	     ;  /* error */
	   }
    }
    	
    if(sscanf(line, "%d %d %lf", &origstate, &deststate, &prob) == 3)
        rewind(fpin);
#ifdef DEBUG
    else
    {
        /* Admitting that the line contains the number of states in the model */
        if(atoi(line) != Model->nstates)
           fprintf(stderr, ".st_trans_prob_mtx reports %d states against %d found in .states\n", atoi(line), Model->nstates);
    }
#endif

    while(!feof(fpin))
    {
        if(fgets(line, MAXLINESIZE, fpin) == NULL)
           break;
            
        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';

        if(line[0] == '\0') /* Blank line */
             break;

        sscanf(line, "%d %d %lf", &origstate, &deststate, &prob);

        Model->ntrans++;

        ptcell = (MatrixCell *)malloc(sizeof(MatrixCell));
        ptcell->next = Model->Matrix[Model->StatesMapVector[deststate]];
        ptcell->value = prob;
		ptcell->level = -1;
        ptcell->state_id = Model->StatesMapVector[origstate];
        Model->Matrix[Model->StatesMapVector[deststate]] = ptcell;
    }

    if (Model->isLiteral)
    {
	    while(!feof(fpin1))
	    {
        	if(fgets(line1, MAXLINESIZE, fpin1) == NULL)
        	   break;


        	if(line1[strlen(line1)-1] == '\n')
        	    line1[strlen(line1)-1] = '\0';


        	sscanf(line1, "%d %d [%d]", &origstate1, &deststate1, &level);	

			ptcell=Model->Matrix[Model->StatesMapVector[deststate1]];

			for (;ptcell!=NULL;ptcell=ptcell->next)
			{
				if (ptcell->state_id == Model->StatesMapVector[origstate1])
				{
					ptcell->level= level;
					break;
				}
			}

	    }
	    fclose(fpin1);
    }
    
    fclose(fpin);


    /*****************************************
     * Look for .partition                   *
     *****************************************/
    filename = (char*)malloc(strlen(Options->FileName)+strlen(EXT_BLOCKS)+1);
    strcpy(filename, Options->FileName);
    strcat(filename, EXT_BLOCKS);
    fpin = fopen(filename, "r");

    if(fpin != NULL)
    {
        Model->hasPartition = TRUE;
        fclose(fpin);
    }
    else
        Model->hasPartition = FALSE;

    memfree(filename);

    /*****************************************
     * Look for rewards                      *
     *****************************************/
    ptaux = Options->FileName + strlen(Options->FileName);
    
    while ((*ptaux != '/')&&(ptaux!=Options->FileName))
        ptaux--;
    
    if(*ptaux == '/') 
    {   
        isize = ptaux - Options->FileName + 1;
        dirstring = (char *)malloc(isize + 1);
        strncpy(dirstring, Options->FileName, isize);
        dirstring[isize] = '\0';
    }
    else
    {
        dirstring = (char *)malloc(3);
        strcpy(dirstring, "./");
    }    

    dir_stream = opendir(dirstring);

    isize = strlen(Model->ModelName) + strlen(EXT_REWARD);

    modelreward = (char *)malloc(isize+1);
    strcpy(modelreward, Model->ModelName);
    strcat(modelreward, EXT_REWARD);

    Model->nrewards = 0;
    Model->Rewards = NULL;

    while((dirinfo = readdir(dir_stream)) != NULL)
    {
        if(strncmp(modelreward, dirinfo->d_name, isize) == 0)
        {
            Model->nrewards++;
            Model->Rewards = realloc(Model->Rewards, Model->nrewards * sizeof(char *));

            ptaux = dirinfo->d_name + strlen(modelreward);

            filename = (char *)malloc(strlen(ptaux)+1);
            strcpy(filename, ptaux);

            Model->Rewards[Model->nrewards-1] = filename;
        }
    }

    closedir(dir_stream);
    memfree(dirstring);
    memfree(modelreward);
	
	


#ifdef DEBUG
    ShowModelInfo(Model);
#endif
}

/**********************************************************************
 *                                                                    *
 *  Compare(const void *x, const void *y)                             *
 *      The comparision function required by qsort()                  *
 *      Return: -1 if x is supposed to be before y, 1 on the contrary,*
 *                      0 if it doesn't matter                        *
 *      x - first element                                             *
 *      y - second element                                            *
 *      Obs: Although it is the last chance return value, 0 will      *
 *           never be returned since there aren't two distinct states *
 *           defined by the same tuple.                               *
 *                                                                    *
 **********************************************************************/
int Compare(const void *x, const void *y)
{
    int i;

    for (i = 0; ; i++)
    {
        if ( (((State*)x)->tuple[i]) != (((State*)y)->tuple[i]) )
            return ( ((State*)x)->tuple[i] > ((State*)y)->tuple[i] ? 1:-1);
    }
    return 0;
}

int Compare2(const void *x, const void *y, int n)
{
    int i;
	
    for (i = 0; i < n ; i++)
    {	
        if ( (((State*)x)->tuple[i]) != (((State*)y)->old_tuple[i]) )
            return ( ((State*)x)->tuple[i] > ((State*)y)->old_tuple[i] ? 1:-1);
    }
    return 0;
}


#ifdef DEBUG
/**********************************************************************
 *                                                                    *
 *  ShowModelInfo(const void *x, const void *y)                       *
 *      Prints out the information about the model                    *
 *      Model - the model information                                 *
 *                                                                    *
 **********************************************************************/
void ShowModelInfo(ModelInfo *Model)
{
    int i;
#ifdef SHOWSTATES
    int j;
#endif
#ifdef SHOWSUM
    MatrixCell *pt;
    double *sumvector;
#endif

    fprintf(stderr, "Model Information:\n");
    fprintf(stderr, "   Model = {\n");
    fprintf(stderr, "       ModelName         = %s\n", Model->ModelName);
    fprintf(stderr, "       nvars             = %d\n", Model->nvars);
    fprintf(stderr, "       Variables         = {\n");
    for(i = 0; i < Model->nvars; i++)
        fprintf(stderr, "           %s\n", Model->Variables[i].varname);
    fprintf(stderr, "       }\n");
    fprintf(stderr, "       PermutationVector = {");
    for(i = 0; i < Model->nvars; i++)
        fprintf(stderr, " %d", Model->PermutationVector[i]);
    fprintf(stderr, " }\n");
    fprintf(stderr, "       nstates           = %d\n", Model->nstates);
#ifdef SHOWSTATES
    fprintf(stderr, "       ModelStates       = {\n");
    for(i = 1; i <= Model->nstates; i++)
    {
        fprintf(stderr, "           %5d (", Model->ModelStates[i].id);
        for(j = 0; j < Model->nvars-1; j++)
            fprintf(stderr, "%3d,", Model->ModelStates[i].tuple[j]);
        fprintf(stderr, "%3d)\n", Model->ModelStates[i].tuple[j]);
    }
    fprintf(stderr, "       }\n");
#endif
    fprintf(stderr, "       ntrans            = %d\n", Model->ntrans);
    fprintf(stderr, "       isLiteral         = %s\n", (Model->isLiteral ? "TRUE":"FALSE"));
    fprintf(stderr, "       hasPartition      = %s\n", (Model->hasPartition ? "TRUE":"FALSE"));
    fprintf(stderr, "       nrewards          = %d\n", Model->nrewards);
    if(Model->nrewards > 0)
    {
        fprintf(stderr, "       Rewards           = {\n");
        for(i = 0; i < Model->nrewards; i++)
            fprintf(stderr, "           %s\n", Model->Rewards[i]);
        fprintf(stderr, "       }\n");
    }

#ifdef SHOWSUM
    sumvector = (double *)malloc(Model->nstates *sizeof(double));
    for(i = 0; i < Model->nstates; i++)
        sumvector[i] = 0.0;

    for(i = 0; i < Model->nstates; i++)
    {
        pt = Model->Matrix[i+1];
        while(pt != NULL)
        {
            sumvector[pt->state_id-1] += pt->value;
            pt = pt->next;
        }
    }

    for(i = 0; i < Model->nstates; i++)
        if(fabs(sumvector[i] - 1.0) > 1e-10)
            fprintf(stderr, "Matrix line %d added to %.10f\n", i+1, sumvector[i]);
    memfree(sumvector);
#endif

    fprintf(stderr, "   }\n");
}
#endif

/**********************************************************************
 *                                                                    *
 *  OpenFile(char *Name, char *Ext, char *mode)                       *
 *      Opens one of the model files for reading checking for errors  *
 *      Return: a pointer to the new file stream                      *
 *      Name - a string containing the modelname                      *
 *      Ext - a string with the file extension                        *
 *      mode - the opening mode (r,w)                                 *
 *                                                                    *
 **********************************************************************/
FILE *OpenFile(char *Name, char *Ext, char *mode)
{
    char *filename;
    FILE *fp;

    filename = (char*)malloc(strlen(Name)+strlen(Ext)+1);
    strcpy(filename, Name);
    strcat(filename, Ext);
    fp = fopen (filename, mode);
    if(fp == NULL)
    {
        fprintf(stderr, "\nFile %s couldn't be opened (mode: %s).\n", filename, mode);
        exit(ERR_FILE);
    }
    memfree(filename);
    return(fp);
}

/**********************************************************************
 *                                                                    *
 *  UpdateFile(Name, Ext)                                             *
 *      Literally `mv -f Name.Ext.new Name.Ext`                       *
 *      Name - a string containing the modelname                      *
 *      Ext - a string with the file extension                        *
 *                                                                    *
 **********************************************************************/
void UpdateFile(char *Name, char *Ext)
{
    char *cmdline, *filename;

    filename = (char *)malloc(strlen(Name)+strlen(Ext)+1);
    strcpy(filename, Name);
    strcat(filename, Ext);
    cmdline = (char *)malloc(2*strlen(filename)+12);

    strcpy(cmdline, "mv -f ");
    strcat(cmdline, filename);
    strcat(cmdline, ".new");
    strcat(cmdline, " ");
    strcat(cmdline, filename);
#ifdef DEBUG
    fprintf(stderr, "%s\n", cmdline);
#endif

    if (system(cmdline)!=0)
    {
	; /* error */
    }

    memfree(filename);
    memfree(cmdline);
}

/**********************************************************************
 *                                                                    *
 *  WriteNewStates(ModelInfo *Model, ArgsInfo *Options)               *
 *      Prints out all the states in the model with the permutation   *
 *             of variables already applied to their tuples           *
 *      Model - a pointer to the model containing the states          *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewStates(ModelInfo *Model, ArgsInfo *Options)
{
    FILE *fpout;
    int i, j;
    char *filename, *cmdline;

#ifdef DEBUG
    fprintf(stderr, "Writing .states.new\n");
#endif
    fpout = OpenFile(Options->FileName, EXT_NEWSTATES, "w");

    for(i = 1; i <= Model->nstates; i++)
    {
        fprintf(fpout, "%d (", Model->StatesMapVector[Model->ModelStates[i].id]);
        for(j = 0; j < Model->nvars-1 ; j++)
        {
            fprintf(fpout, "%d,", Model->ModelStates[i].tuple[j]);
        }
        fprintf(fpout, "%d)\n", Model->ModelStates[i].tuple[j]);
    }

    fclose(fpout);

    filename = (char *)malloc(strlen(Options->FileName) + strlen(EXT_NEWSTATES) + 1);
    strcpy(filename, Options->FileName);
    strcat(filename, EXT_NEWSTATES);

    cmdline = (char *)malloc(2 * strlen(filename) + 15);

    strcpy(cmdline, "sort -n -o ");
    strcat(cmdline, filename);
    strcat(cmdline, " ");
    strcat(cmdline, filename);

#ifdef DEBUG
    fprintf(stderr, "%s\n", cmdline);
#endif

    if (system(cmdline)!=0)
    {
	; /* error */
    }

    memfree(cmdline);
    memfree(filename);
}

/**********************************************************************
 *                                                                    *
 *  WriteNewVStat(ModelInfo *Model, ArgsInfo *Options)                *
 *      Prints out the .vstat file with the order of the current      *
 *             permutation                                            *
 *      Model - a pointer to the model containing the variables       *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewVStat(ModelInfo *Model, ArgsInfo *Options)
{
    FILE *fpout;
    int i, j;

#ifdef DEBUG
    fprintf(stderr, "Writing .vstat.new\n");
#endif
    fpout = OpenFile(Options->FileName, EXT_NEWVSTAT, "w");

    for(i = 0; i < Model->nvars; i++)
    {
        /* state translation */
        
        for ( j = 0; Model->PermutationVector[j] != i ; j ++ )
            ;
        fprintf(fpout, "%s\n", Model->Variables[/* Model->PermutationVector[i] */ j ].varname);
    }

    fclose(fpout);
}

/**********************************************************************
 *                                                                    *
 *  WriteNewGeneratorMtxParam(ModelInfo *Model, ArgsInfo *Options)    *
 *      Updates the .generator_mtx.param for the model with the new   *
 *             permutation                                            *
 *      Model - a pointer to the model information                    *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewGeneratorMtxParam(ModelInfo *Model, ArgsInfo *Options)
{
    int i, orig_state, dest_state;
    FILE *fpin,*fpout, *fpsort;
    char line[MAXLINESIZE],*pt, *cmdline;

    fpin = OpenFile(Options->FileName, EXT_GENERATOR_MTX_PARAM, "r");   
    
    /* fpin = OpenFile(Options->FileName, EXT_NEWVSTAT, "r"); ??? */
    
#ifdef DEBUG
    fprintf(stderr, "Writing .generator_mtx.param.new\n");
#endif
    fpout = OpenFile(Options->FileName, EXT_NEWGENERATOR_MTX_PARAM, "w");
    
    
    cmdline = (char*)malloc(strlen(Options->FileName)+ strlen(EXT_NEWGENERATOR_MTX_PARAM) + 15);
    strcpy(cmdline, "sort -g >> ");
    strcat(cmdline, Options->FileName);
    strcat(cmdline, EXT_NEWGENERATOR_MTX_PARAM);
    
    
    if ((fpsort = popen(cmdline, "w")) == NULL )
    {
        perror("sort error\n");
        exit(ERR_PIPE);
    }

    while(!feof(fpin))
    {
        if(fgets(line, MAXLINESIZE, fpin) == NULL) 
            break;

        if(sscanf(line, "%d %d", &orig_state, &dest_state)<2)
        { 
             if ((*line!='\0') && (*line!='\n'))
                 fputs(line, fpout);
             continue;
        }
        pt = line;
        


        find(pt, '[');
        pt++; 
        


        sscanf(pt, "%d", &i);

        orig_state = Model->StatesMapVector[orig_state];
        dest_state = Model->StatesMapVector[dest_state];

        fprintf(fpsort, "%d %d [%d]\n", orig_state, dest_state, i);
        

    }

    fclose(fpin);
    fclose(fpout);
    pclose(fpsort);
    

}

/**********************************************************************
 *                                                                    *
 *  WriteNewGeneratorMtx(ModelInfo *Model, ArgsInfo *Options)         *
 *      Updates the .generator_mtx for the model with the new         *
 *             permutation                                            *
 *      Model - a pointer to the model information                    *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewGeneratorMtx(ModelInfo *Model, ArgsInfo *Options)
{
    int orig_state, dest_state;
    FILE *fpin,*fpout, *fpsort;
    char line[MAXLINESIZE], *cmdline;
    double rate;
   
#ifdef DEBUG
    fprintf(stderr, "Writing .generator_mtx.new\n");
#endif
    fpin = OpenFile(Options->FileName, EXT_GENERATOR_MTX, "r");
    fpout = OpenFile(Options->FileName, EXT_NEWGENERATOR_MTX, "w");
    
    cmdline = (char*)malloc(strlen(Options->FileName)+ strlen(EXT_NEWGENERATOR_MTX) + 15);
    strcpy(cmdline, "sort -g >> ");
    strcat(cmdline, Options->FileName);
    strcat(cmdline, EXT_NEWGENERATOR_MTX);

    if ((fpsort = popen(cmdline, "w")) == NULL )
    {
        perror("sort error\n");
        exit(ERR_PIPE);
    }

    while(!feof(fpin))
    {
        if(fgets(line, MAXLINESIZE, fpin) == NULL) 
            break;

        if(sscanf(line, "%d %d %lf", &orig_state, &dest_state, &rate)<3)
        { 
             if((*line!='\0') && (*line!='\n'))
                 fputs(line, fpout);
             continue;
        }

        orig_state = Model->StatesMapVector[orig_state];
        dest_state = Model->StatesMapVector[dest_state];

        fprintf(fpsort, "%d %d %.10E\n", orig_state, dest_state, rate);
    }

    fclose(fpin);
    fclose(fpout);
    pclose(fpsort);
}
    
/**********************************************************************
 *                                                                    *
 *  WriteNewStTransProbMtx(ModelInfo *Model, ArgsInfo *Options)       *
 *      Updates the .st_trans_prob_mtx for the model with the new     *
 *             permutation                                            *
 *      Model - a pointer to the model information                    *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewStTransProbMtx(ModelInfo *Model, ArgsInfo *Options)
{
    int orig_state, dest_state;
    FILE *fpin,*fpout, *fpsort;
    char line[MAXLINESIZE], *cmdline;
    double prob;
   
#ifdef DEBUG
    fprintf(stderr, "Writing .NM.st_trans_prob_mtx.new\n");
#endif
    fpin = OpenFile(Options->FileName, EXT_ST_TRANS_PROB_MTX, "r");
    fpout = OpenFile(Options->FileName, EXT_NEWST_TRANS_PROB_MTX, "w");
    
    cmdline = (char*)malloc(strlen(Options->FileName)+ strlen(EXT_NEWST_TRANS_PROB_MTX) + 15);
    strcpy(cmdline, "sort -g >> ");
    strcat(cmdline, Options->FileName);
    strcat(cmdline, EXT_NEWST_TRANS_PROB_MTX);

    if ((fpsort = popen(cmdline, "w")) == NULL )
    {
        perror("sort error\n");
        exit(ERR_PIPE);
    }

    while(!feof(fpin))
    {
        if(fgets(line, MAXLINESIZE, fpin) == NULL) 
            break;

        if(sscanf(line, "%d %d %lf", &orig_state, &dest_state, &prob) < 3)
        { 
             if((*line!='\0') && (*line!='\n'))
                 fputs(line, fpout);
             continue;
        }

        orig_state = Model->StatesMapVector[orig_state];
        dest_state = Model->StatesMapVector[dest_state];

        fprintf(fpsort, "%d %d %.10E\n", orig_state, dest_state, prob);
    }

    fclose(fpin);
    fclose(fpout);
    pclose(fpsort);
}

/**********************************************************************
 *                                                                    *
 *  WriteNewRewards(ModelInfo *Model, ArgsInfo *Options)              *
 *      Writes remapped files for all the rewards in the model        *
 *      Model - a pointer to the model information                    *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void WriteNewRewards(ModelInfo *Model, ArgsInfo *Options)
{
    char *extension, *cmdline;
    FILE *fpin, *fpout, *fpsort;
    int state, i;
    double reward;
    char line[MAXLINESIZE];

    for(i = 0; i < Model->nrewards; i++)
    {
        
        extension = (char *)malloc(strlen(EXT_REWARD)+strlen(Model->Rewards[i])+strlen(".new")+1);
        strcpy(extension, EXT_REWARD);
        strcat(extension, Model->Rewards[i]);
        fpin = OpenFile(Options->FileName, extension, "r");

        strcat(extension, ".new");

#ifdef DEBUG
        fprintf(stderr, "Writing %s\n", extension);
#endif
        fpout = OpenFile(Options->FileName, extension, "w");

        cmdline = (char*)malloc(strlen(Options->FileName)+ strlen(extension) + 15);
        strcpy(cmdline, "sort -g >> ");
        strcat(cmdline, Options->FileName);
        strcat(cmdline, extension);

        if ((fpsort = popen(cmdline, "w")) == NULL )
        {
            perror("sort error\n");
            exit(ERR_PIPE);
        }

        if (fgets(line, MAXLINESIZE, fpin)==NULL)
	{
	  ;
	}
	
        
        /*
        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0'; */

        if( ( sscanf(line, "%d %lf", &state, &reward) < 2 ) || 
            ( state == -1 ) )
        {
            fputs(line, fpout);
            /* fclose(fpout); */
        }
        else
        {
            /* fclose(fpout); */
            fprintf(fpsort, "%d %.10E\n", Model->StatesMapVector[state], reward);
        }

        while(!feof(fpin))
        {
            if(fgets(line, MAXLINESIZE, fpin) == NULL) 
                break;

            if ( ( sscanf(line, "%d %lf", &state, &reward) < 2 ) || 
                 ( state == -1 ) )
            {
                fputs(line, fpout);
            } else {
                fprintf(fpsort, "%d %.10E\n", 
                    Model->StatesMapVector[state], reward);
            }
        }
        
        fclose(fpout);

        fclose(fpin);
        pclose(fpsort);
        memfree(extension);
        memfree(cmdline);
    }
}

/**********************************************************************
 *                                                                    *
 *  UpdateRewards(ModelInfo *Model, ArgsInfo *Options)                *
 *      Overwrites the reward files with the new remapped files       *
 *      Model - a pointer to the model information                    *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void UpdateRewards(ModelInfo *Model, ArgsInfo *Options)
{
    char *extension;
    int i;

    for(i = 0; i < Model->nrewards; i++)
    {
        extension = (char *)malloc(strlen(EXT_REWARD)+strlen(Model->Rewards[i])+1);
        strcpy(extension, EXT_REWARD);
        strcat(extension, Model->Rewards[i]);

        UpdateFile(Options->FileName, extension);
    }
}

/****************************************************************************
 *                                                                          *
 *  Graphical Functions                                                     *
 *                                                                          *
 ****************************************************************************/



/* More global variables (see the top of the file for others)... */


double StatesPerPixel;

/* Interface Widgets - Organized to show their hierarchy in the interface */
Widget topLevel,
         frmMain,
           coreMatrix,
           frmZoomInfo,
             labZoom, labLevel, labLevelVal,
             hboxZoom,
               labX, ascX,
               labY, ascY,
               labSize, ascSize,
               butSetZoom,
           vboxSideBar,
             frmToolBar,
               radZoom, radView, butP, togBlock,
			   labLegend, ascLegend,
			   labOutFile, ascOutFile,
             vboxBlockInfo,
               frmBlock,
               labBlock,
               labInitialState, ascInitialState,
               labBlockSize, ascBlockSize,
               labNBlocks, ascNBlocks,
               butRemoveAllBlocks, butGenerateOutput, butAddBlock, butRemoveBlock,
               labListTitle,
               lstBlock, vpBlockList,
           hboxTransitions,
             labTransition,
             butOriginState,
                smeMenuOrigin,
                *smeMenuOriginItems,
             labArrow,
             butDestState,
                smeMenuDest,
                *smeMenuDestItems,
             labProb,
             coreScale,
           boxStatusBar,
             labStatus,
           butQuit;



String EMPTY_LIST[] = { "No blocks defined", NULL };
static String *listitems = NULL; // list of blocks
    
Stack S;        // stack of window blocks used in zoom in/out operations
LList LL;       // linked list of matrix blocks


int RectangleX, RectangleY, RectangleSize, Pressed,
    StartX, StartY, FinalX, FinalY;


int ScalePointerLevel;
int CurrentOriginState, CurrentDestState;


int quit;

char StatusMessage[100];

#define MAXTUPLESIZE 80

#define LISTITEMSWIDTH 140

#define SCALEHEIGHT 17
#define SCALEWIDTH 150

#define VPBLOCKLIST_WIDTH 156
#define VPBLOCKLIST_HEIGHT 80

#define STATEBUTTON_WIDTH 70


/* the following pixmaps were created using the bitmap tool */

#define icon_bitmap_width 20
#define icon_bitmap_height 20
static char icon_bitmap_bits[] = {
   0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x24, 0x00, 0x00, 0x48, 0x00, 0x00,
   0x90, 0x04, 0x00, 0x24, 0x09, 0x00, 0x48, 0x12, 0x00, 0x90, 0x04, 0x00,
   0x20, 0x49, 0x00, 0x40, 0x12, 0x00, 0x90, 0x20, 0x01, 0x20, 0x41, 0x02,
   0x40, 0x12, 0x00, 0x00, 0x04, 0x01, 0x00, 0x49, 0x00, 0x00, 0x80, 0x04,
   0x00, 0x24, 0x09, 0x00, 0x08, 0x02, 0x00, 0x80, 0x04, 0x00, 0x00, 0x09};

#define magnify_width 30
#define magnify_height 30
static char magnify_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00,
   0xf8, 0xf8, 0x00, 0x00, 0x38, 0xe0, 0x00, 0x00, 0x1c, 0xc0, 0x01, 0x00,
   0x0c, 0x82, 0x01, 0x00, 0x0e, 0x82, 0x03, 0x00, 0x86, 0x0f, 0x03, 0x00,
   0x06, 0x02, 0x03, 0x00, 0x06, 0x02, 0x03, 0x00, 0x0e, 0x80, 0x03, 0x00,
   0x8c, 0x8f, 0x01, 0x00, 0x1c, 0xc0, 0x01, 0x00, 0x38, 0xe0, 0x01, 0x00,
   0xf8, 0xf8, 0x01, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x80, 0x0f, 0x0f, 0x00,
   0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7c, 0x00,
   0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0xe0, 0x03,
   0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00, 0x00, 0x1f,
   0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00};

#define view_width 30
#define view_height 30
static char view_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
   0x00, 0x07, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00,
   0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
   0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x07, 0x00,
   0x00, 0x3f, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00,
   0x00, 0xe3, 0x00, 0x00, 0x00, 0xe1, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00,
   0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x80, 0x03, 0x00,
   0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0e, 0x00,
   0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x1c, 0x00,
   0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define p_width 30
#define p_height 30
static char p_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00,
   0x70, 0x70, 0x00, 0x00, 0x70, 0xe0, 0x00, 0x00, 0x70, 0xe0, 0x00, 0x00,
   0x70, 0xe0, 0x00, 0x00, 0x70, 0xe0, 0x00, 0x00, 0x70, 0xf0, 0xc0, 0x00,
   0xf0, 0x7f, 0xe0, 0x03, 0xf0, 0x3f, 0xe0, 0x03, 0x70, 0x80, 0xc3, 0x00,
   0x70, 0x80, 0xcf, 0x00, 0x70, 0x80, 0xcf, 0x00, 0x70, 0x80, 0xc3, 0x00,
   0x70, 0x80, 0xc3, 0x03, 0x70, 0x80, 0x83, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void redisplayscale_event(w, client, ev)
Widget w;
XtPointer client;
XExposeEvent *ev;
{
    Display *display = XtDisplay(w);
    Window winScale = XtWindow(w);

    int i;
    float ScaleFact;

    ScaleFact = ((float)SCALEWIDTH / (float)NOGCS);
   
    for (i=0; i < NOGCS; i++)
    {
        XFillRectangle(display, winScale, gc[i], ((int)i*ScaleFact),0,(int)ScaleFact+1, SCALEHEIGHT);
    }

    XDrawLine(display, XtWindow(coreScale), invert_gc,
              ScalePointerLevel, 0, ScalePointerLevel, SCALEHEIGHT);
}



char tgif_template1[1024]="%TGIF 4.1.43-QPL\n\
state(0,37,178.000,0,0,0,32,1,1,1,1,0,0,1,1,1,0,'Courier',0,80640,0,0,0,10,0,0,1,1,0,16,0,0,1,1,1,1,1088,1408,1,0,2880,0).\n\
%\n\
% @(#)$Header: /export/CVS/tangram2/ModelEnv/mtx_vis-st_permut/matrix.c,v 1.6 2011/12/07 18:58:01 gdjaime Exp $\n\
% %W%\n\
%\n\
unit(\"1 pixel/pixel\").\n\
color_info(";

char tgif_template2[1024]=",65535,0,[\n\
	\"magenta\", 65535, 0, 65535, 65535, 0, 65535, 1,\n\
	\"red\", 65535, 0, 0, 65535, 0, 0, 1,\n\
	\"green\", 0, 65535, 0, 0, 65535, 0, 1,\n\
	\"blue\", 0, 0, 65535, 0, 0, 65535, 1,\n\
	\"yellow\", 65535, 65535, 0, 65535, 65535, 0, 1,\n\
	\"pink\", 65535, 49344, 52171, 65535, 49344, 52171, 1,\n\
	\"cyan\", 0, 65535, 65535, 0, 65535, 65535, 1,\n\
	\"CadetBlue\", 24415, 40606, 41120, 24415, 40606, 41120, 1,\n\
	\"white\", 65535, 65535, 65535, 65535, 65535, 65535, 1,\n\
	\"black\", 0, 0, 0, 0, 0, 0, 1,\n\
	\"DarkSlateGray\", 12079, 20303, 20303, 12079, 20303, 20303, 1";
	
char tgif_template3[1024]="\n]).\n\
script_frac(\"0.6\").\n\
fg_bg_colors('red','white').\n\
dont_reencode(\"FFDingbests:ZapfDingbats\").\n\
page(1,\"\",1,'').\n";

char tgif_polystr[1024]="poly('black','',2,[\n\t%d,%d,%d,%d],0,1,1,0,0,0,0,0,0,0,0,'1',0,0,\n\
\"0\",\"\",[\n\
\t0,8,3,0,'8','3','0'],[0,8,3,0,'8','3','0'],[\n\
]).\n";  


char tgif_text[1024]="text('black',17,409,1,0,1,18,21,165,16,5,0,0,0,0,2,18,21,0,0,\"\",0,0,0,0,425,'',[\n\
minilines(18,21,0,0,0,0,0,[\n\
mini_line(18,16,5,0,0,0,[\n\
str_block(0,18,16,5,0,-1,0,0,0,[\n\
str_seg('black','Helvetica',0,92160,18,16,5,0,-1,0,0,0,0,0,\n\
	\"%s\")])\n\
])\n\
])]).";


void save_matrix(Display *display, XtPointer client)
{
	int id = 0, toggstate = 0;
    MatrixCell *pt = NULL;
    double prob; int i = 0;
    int level, j;
    char *filename = NULL;
	FILE *fpout;
    ModelInfo *model = (ModelInfo *)client;	
	
	
    filename = (char *)malloc(strlen((char *)model->argsinfo->FileName) 
		+ strlen((char *)model->argsinfo->PrintScreenOutFile) + 2 
		+ strlen(EXT_TGIF) + 1);
    strcpy(filename, (char *)model->argsinfo->FileName);
	
	if (strlen(model->argsinfo->PrintScreenOutFile) > 0)
	{
	
		strcat(filename, ".");	
		strcat(filename, model->argsinfo->PrintScreenOutFile);		
	}
	
	strcat(filename, EXT_TGIF);
    fpout = fopen(filename, "w");
    memfree(filename);
	
	
	
	fprintf(fpout, "%s", tgif_template1);
	fprintf(fpout, "%d", 11+NOGCS+2);
	
	fprintf(fpout, "%s", tgif_template2);		
	
	
	for (level = 0; level <= NOGCS+1; level ++)
	{
		
		fprintf(fpout, ",\n\t\"#%02X%02X%02X\", %d, %d, %d, %d, %d, %d, 1", 
		
			model->colors[level].red>>8, model->colors[level].green>>8, model->colors[level].blue>>8,
			model->colors[level].red, model->colors[level].green, model->colors[level].blue, 
			model->colors[level].red, model->colors[level].green, model->colors[level].blue		);	
	}
	
	

	fprintf(fpout, "%s", tgif_template3);		
	
	if (model->argsinfo->BackgroundColor == WHITE)
	{
		fprintf(fpout,"box('black','',-1,0,400,400,0,1,1,154,0,0,0,0,0,'1',0,[\n]).\n");
	}
	else
	{
		fprintf(fpout, "box('#%02X%02X%02X','',-1,0,400,400,1,1,2,154,0,0,0,0,0,'1',0,[\n]).\n",
								((unsigned int)model->colors[NOGCS+1].red)>>8,
								((unsigned int)model->colors[NOGCS+1].green)>>8, 
								((unsigned int)model->colors[NOGCS+1].blue)>>8);
	}
	
	
	

    StatesPerPixel = (double)size/(double)WINDOW_SIZE;

    if( ( StatesPerPixel <= 1.0 ) ) // ||  ( Options->PreDefinedPixelsPerState ) )
    {
		/*
		if ( PreDefinedPixelsPerState )
		{
			StatesPerPixel = 1/(float)PreDefinedPixelsPerState;
		} */
		
        for(i = 0; i < size; i++)
        {


            pt = MATRIX[x+i];
            while(pt != NULL)
            {
                int bsize = (int)ceil(1.0/StatesPerPixel);
                if ( (pt->state_id >= y) && (pt->state_id < y+size) )
                {
				
				
						if (PC==FALSE)
					    {



							if (  (model->argsinfo->UseSpecialColor) && 
					    		  (model->argsinfo->SpecialColorLowThr < pt->value) && 
					    		  (model->argsinfo->SpecialColorUpThr > pt->value) )
							{
								level = NOGCS;
							}
							else
							{
                    			level = (int)rint(floor(pt->value*(double)(NOGCS)));

		            			if ( level == NOGCS ) level --;
							}


							fprintf(fpout, "box('#%02X%02X%02X','',%f,%f,%f,%f,1,1,0,%d,0,0,0,0,0,'1',0,[\n]).\n", 
									((unsigned int)model->colors[level].red)>>8,
									((unsigned int)model->colors[level].green)>>8, 
									((unsigned int)model->colors[level].blue)>>8,
									(i/(float)StatesPerPixel),
                            		((pt->state_id-y)/(float)StatesPerPixel),
                            		bsize+(i/(float)StatesPerPixel),
                            		bsize+((pt->state_id-y)/(float)StatesPerPixel),id++);								}
						else
						{
		
							
		                	    level = pt->level;

							    if ( level == NOPREGCS ) level --;

		        						    
							    if (level < NOPREGCS)
			    				{
		                		  /*  XFillRectangle(display, matrix_window, pregc[level+2],
        	                		    (int)floor(i/StatesPerPixel),
            	            		    (int)floor((pt->state_id-y)/StatesPerPixel),
                	        		    bsize,
                    	    		    bsize);		     */
							    					
								
 							    fprintf(fpout, "box('#%02X%02X%02X','',%f,%f,%f,%f,1,1,0,%d,0,0,0,0,0,'1',0,[\n]).\n", 
									((unsigned int)model->precolors[model->ColorPermutationVector[level+2]].red)>>8,
									((unsigned int)model->precolors[model->ColorPermutationVector[level+2]].green)>>8, 
									((unsigned int)model->precolors[model->ColorPermutationVector[level+2]].blue)>>8,
									(i/(float)StatesPerPixel),
                            		((pt->state_id-y)/(float)StatesPerPixel),
                            		bsize+(i/(float)StatesPerPixel),
                            		bsize+((pt->state_id-y)/(float)StatesPerPixel),id++);										}
							
						}							


                }

                pt = pt->next;
            }
        }
    }
    else
    {


        int coord1, coord2;
		

        /* Initializes the probabilities table to zero */
        for(i = 0; i < WINDOW_SIZE; i++)
            for(j = 0; j < WINDOW_SIZE; j++)
            {
                ProbTable[i][j].prob = 0.0;
                ProbTable[i][j].level = 0;				
                ProbTable[i][j].weight = 0;
            }
			
		
        /* Fills the probabilities table */                    
        for(i = 0; i < size; i++)
        {
            coord1 = (int)floor(i/StatesPerPixel);
        
            pt = MATRIX[x+i];
        
            while(pt != NULL)
            {
                if( (pt->state_id >= y) && (pt->state_id < (y+size)) )
                {                    
                    coord2 = (int)floor(((pt->state_id-y))/StatesPerPixel);
#ifdef DEBUG
                    if (coord1 >= WINDOW_SIZE || coord1 < 0 ||
                        coord2 >= WINDOW_SIZE || coord2 < 0)
                    {
                        fprintf(stderr, "\ncoord1 = %d coord2 = %d", coord1, coord2);
                    }    
#endif

                    switch(ZO)
                    {
                        case ZO_MAXIMUM:
                            if(pt->value > ProbTable[coord1][coord2].prob)
                                ProbTable[coord1][coord2].prob = pt->value;
                        break;
                        case ZO_MINIMUM:
                            if((pt->value < ProbTable[coord1][coord2].prob) ||
                               (ProbTable[coord1][coord2].prob == 0.0))
                                ProbTable[coord1][coord2].prob = pt->value;
                        break;
                        case ZO_AVERAGE:
                            ProbTable[coord1][coord2].prob *= ProbTable[coord1][coord2].weight;
                            ProbTable[coord1][coord2].prob += pt->value;
                            ProbTable[coord1][coord2].weight++;
                            ProbTable[coord1][coord2].prob /= ProbTable[coord1][coord2].weight;
                        break;
                    }

#ifdef DEBUG
                    if(ProbTable[coord1][coord2].prob > 1.0)
                    {
                        fprintf(stderr, "prob = %g\n", ProbTable[coord1][coord2].prob);
                        break;
                    }
#endif
                }
                pt = pt->next;
            }
        }
        /* Plots the states on the window */
        for(i=0; i<WINDOW_SIZE; i++)
        {
            for(j = 0; j<WINDOW_SIZE; j++)
            {
                prob = ProbTable[i][j].prob;
				
                    if(prob != 0.0)
                    {
					
 					    if (  (model->argsinfo->UseSpecialColor) && 
					      (model->argsinfo->SpecialColorLowThr < prob) && 
					      (model->argsinfo->SpecialColorUpThr > prob) )

						{
							level = NOGCS;
						}
						else
						{
                    		level = (int)rint(floor(prob*(double)(NOGCS)));

		            		if ( level == NOGCS ) level --;
						}
						

						fprintf(fpout, "box('#%02X%02X%02X','',%d,%d,%d,%d,1,1,0,%d,0,0,0,0,0,'1',0,[\n]).\n", 
								((unsigned int)model->colors[level].red)>>8,
								((unsigned int)model->colors[level].green)>>8, 
								((unsigned int)model->colors[level].blue)>>8,
								i,
                            	j,
                            	1+i,
                            	1+j,id++);

                    }
            }
        }
    }
	
	
	
	if ( model->argsinfo->ShowMatrix )
	{
		XtVaGetValues(togBlock, XtNstate, &toggstate, NULL);	
	}
	
	if ( !model->argsinfo->ShowMatrix || toggstate )
    {
	    LLNode *ptllnode;		
        int SideLength, coordcolpt1, coordcolpt2, coordlinpt1, coordlinpt2;

        LList_SearchGreater(&LL, max(x, y), &ptllnode);

        while(ptllnode != NULL)
        {
#ifdef DEBUG
            MatrixBlock_Show(&ptllnode->info);
#endif
            coordcolpt1 = (int)floor((ptllnode->info.initialState + ptllnode->info.size - x)/StatesPerPixel);
            coordlinpt1 = (int)floor((ptllnode->info.initialState + ptllnode->info.size - y)/StatesPerPixel);
            SideLength = (int)ceil(ptllnode->info.size/StatesPerPixel);

            coordcolpt2 = coordcolpt1 - SideLength;
            coordlinpt2 = coordlinpt1 - SideLength;

            if(coordcolpt2 < 0) coordcolpt2 = -1;
            if(coordlinpt2 < 0) coordlinpt2 = -1;
            if(coordcolpt2 > WINDOW_SIZE) coordcolpt2 = WINDOW_SIZE + 1;
            if(coordlinpt2 > WINDOW_SIZE) coordlinpt2 = WINDOW_SIZE + 1;                        

            if(coordcolpt1 < 0) coordcolpt1 = -1;
            if(coordlinpt1 < 0) coordlinpt1 = -1;
            if(coordcolpt1 > WINDOW_SIZE) coordcolpt1 = WINDOW_SIZE + 1;
            if(coordlinpt1 > WINDOW_SIZE) coordlinpt1 = WINDOW_SIZE + 1;                        

            if(min(coordcolpt2, coordlinpt2) < WINDOW_SIZE)
            {
				fprintf(fpout, tgif_polystr, 
						coordcolpt1, coordlinpt1, coordcolpt1, coordlinpt2);									


				fprintf(fpout, tgif_polystr, 
						coordcolpt1, coordlinpt1, coordcolpt2, coordlinpt1);									



				fprintf(fpout, tgif_polystr, 
						coordcolpt2, coordlinpt1, coordcolpt2, coordlinpt2);		


				fprintf(fpout, tgif_polystr, 
						coordcolpt1, coordlinpt2, coordcolpt2, coordlinpt2);									
						
            }
            else
                break;
            ptllnode = ptllnode->next;
        }
    }
	
	if ( model->argsinfo->Legend[0] != 0 )
		fprintf(fpout, tgif_text, model->argsinfo->Legend);

	
	fclose(fpout);    
}



void redisplay_event(w, client, ev)
Widget w;
XtPointer client;
XExposeEvent *ev;
{
    Boolean toggstate;
    LLNode *ptllnode;
    Display *display;
    Window matrix_window;
    MatrixCell *pt = NULL;
    double prob;
    int level, i, j;
    char buffer[20];
	
	static int first = 1;
	

	 
    ModelInfo *model = (ModelInfo *)client;
	
	


    if(ev->count != 0)
        return;

    display = XtDisplay(w);
    matrix_window = XtWindow(w);

    XClearWindow(display, matrix_window);

    StatesPerPixel = (double)size/(double)WINDOW_SIZE;

    if(StatesPerPixel <= 1.0)
    {
        
        for(i = 0; i < size; i++)
        {

            pt = MATRIX[x+i];
            while(pt != NULL)
            {
                int bsize = (int)ceil(1.0/StatesPerPixel);
                if ( (pt->state_id >= y) && (pt->state_id < y+size) )
                {
					
					if (PC==FALSE)
					{
				
						if (  (model->argsinfo->UseSpecialColor) && 
					    	  (model->argsinfo->SpecialColorLowThr < pt->value) && 
					    	  (model->argsinfo->SpecialColorUpThr > pt->value) )
						{
							level = NOGCS;
						}
						else
						{
                    		level = (int)rint(floor(pt->value*(double)(NOGCS)));

		            		if ( level == NOGCS ) level --;
						}


                    	XFillRectangle(display, matrix_window, gc[level],
                            	(int)floor(i/StatesPerPixel),
                            	(int)floor((pt->state_id-y)/StatesPerPixel),
                            	bsize,
                            	bsize);
					}
					else
					{
                	    level = pt->level;

			    		if ( level == NOPREGCS ) level --;

			    		if (level < NOPREGCS)
			    		{
                		    		XFillRectangle(display, matrix_window, pregc[model->ColorPermutationVector[level+2]],
                        		    		(int)floor(i/StatesPerPixel),
                        		    		(int)floor((pt->state_id-y)/StatesPerPixel),
                        		    		bsize,
                        		    		bsize);		    
			    		}
					}
                }

                pt = pt->next;
            }
        }
    }
    else
    {
        int coord1, coord2;
		

        /* Initializes the probabilities table to zero */
        for(i = 0; i < WINDOW_SIZE; i++)
            for(j = 0; j < WINDOW_SIZE; j++)
            {
                ProbTable[i][j].prob = 0.0;
                ProbTable[i][j].weight = 0;
            }

        /* Fills the probabilities table */                    
        for(i = 0; i < size; i++)
        {
            coord1 = (int)floor(i/StatesPerPixel);
        
            pt = MATRIX[x+i];
        
            while(pt != NULL)
            {
                if( (pt->state_id >= y) && (pt->state_id < (y+size)) )
                {                    
                    coord2 = (int)floor(((pt->state_id-y))/StatesPerPixel);
#ifdef DEBUG
                    if (coord1 >= WINDOW_SIZE || coord1 < 0 ||
                        coord2 >= WINDOW_SIZE || coord2 < 0)
                    {
                        fprintf(stderr, "\ncoord1 = %d coord2 = %d", coord1, coord2);
                    }    
#endif

                    switch(ZO)
                    {
                        case ZO_MAXIMUM:
                            if(pt->value > ProbTable[coord1][coord2].prob)
                                ProbTable[coord1][coord2].prob = pt->value;
                        break;
                        case ZO_MINIMUM:
                            if((pt->value < ProbTable[coord1][coord2].prob) ||
                               (ProbTable[coord1][coord2].prob == 0.0))
                                ProbTable[coord1][coord2].prob = pt->value;
                        break;
                        case ZO_AVERAGE:
                            ProbTable[coord1][coord2].prob *= ProbTable[coord1][coord2].weight;
                            ProbTable[coord1][coord2].prob += pt->value;
                            ProbTable[coord1][coord2].weight++;
                            ProbTable[coord1][coord2].prob /= ProbTable[coord1][coord2].weight;
                        break;
                    }

#ifdef DEBUG
                    if(ProbTable[coord1][coord2].prob > 1.0)
                    {
                        fprintf(stderr, "prob = %g\n", ProbTable[coord1][coord2].prob);
                        break;
                    }
#endif
                }
                pt = pt->next;
            }
        }
		
		
        /* Plots the states on the window */
        for(i=0; i<WINDOW_SIZE; i++)
        {
            for(j = 0; j<WINDOW_SIZE; j++)
            {
			
				
                prob = ProbTable[i][j].prob;
				
                    if(prob != 0.0)
                    {
					
 					    if (  (model->argsinfo->UseSpecialColor) && 
					      (model->argsinfo->SpecialColorLowThr < prob) && 
					      (model->argsinfo->SpecialColorUpThr > prob) )

						{
							level = NOGCS;
						}
						else
						{
                    		level = (int)rint(floor(prob*(double)(NOGCS)));

		            		if ( level == NOGCS ) level --;
						}
					
                        XDrawPoint(display, matrix_window, gc[level], i, j);
						
                    }
            }
        }
    }
	
    
    /* Draw blocks on screen */    
    XtVaGetValues(togBlock, XtNstate, &toggstate, NULL);

    if(toggstate)
    {
        int SideLength, coordcolpt1, coordcolpt2, coordlinpt1, coordlinpt2;

        LList_SearchGreater(&LL, max(x, y), &ptllnode);

        while(ptllnode != NULL)
        {
#ifdef DEBUG
            MatrixBlock_Show(&ptllnode->info);
#endif
            coordcolpt1 = (int)floor((ptllnode->info.initialState + ptllnode->info.size - x)/StatesPerPixel);
            coordlinpt1 = (int)floor((ptllnode->info.initialState + ptllnode->info.size - y)/StatesPerPixel);
            SideLength = (int)ceil(ptllnode->info.size/StatesPerPixel);

            coordcolpt2 = coordcolpt1 - SideLength;
            coordlinpt2 = coordlinpt1 - SideLength;

            if(coordcolpt2 < 0) coordcolpt2 = -1;
            if(coordlinpt2 < 0) coordlinpt2 = -1;
            if(coordcolpt2 > WINDOW_SIZE) coordcolpt2 = WINDOW_SIZE + 1;
            if(coordlinpt2 > WINDOW_SIZE) coordlinpt2 = WINDOW_SIZE + 1;                        

            if(coordcolpt1 < 0) coordcolpt1 = -1;
            if(coordlinpt1 < 0) coordlinpt1 = -1;
            if(coordcolpt1 > WINDOW_SIZE) coordcolpt1 = WINDOW_SIZE + 1;
            if(coordlinpt1 > WINDOW_SIZE) coordlinpt1 = WINDOW_SIZE + 1;                        

            if(min(coordcolpt2, coordlinpt2) < WINDOW_SIZE)
            {
                XDrawLine(display, matrix_window, invert_gc,
                    coordcolpt1, coordlinpt1, coordcolpt1, coordlinpt2);
                XDrawLine(display, matrix_window, invert_gc,
                    coordcolpt1, coordlinpt1, coordcolpt2, coordlinpt1);

                XDrawLine(display, matrix_window, invert_gc,
                    coordcolpt2, coordlinpt1, coordcolpt2, coordlinpt2);
                XDrawLine(display, matrix_window, invert_gc,
                    coordcolpt1, coordlinpt2, coordcolpt2, coordlinpt2);
            }
            else
                break;
            ptllnode = ptllnode->next;
        }
    }
    
    sprintf(buffer, "%.3g:1", 1.0/StatesPerPixel);
    XtVaSetValues(labLevelVal, XtNlabel, buffer, NULL);


	if (first && model->argsinfo->PrintScreen)	
		save_matrix(XtDisplay(w), client);

	first = 0;
	
	
}

void buttonpress_event(w, client, ev)
Widget w;
XtPointer client;
XButtonEvent *ev;
{
    XExposeEvent e;
    Display *display = XtDisplay(w);
    Window matrix_window = XtWindow(w);
    char buffer[50];
    MatrixCell *pt;
    double prob;
    char outputstr[50];
    int i, j;
    Boolean ToggleState;
    WindowBlock wb;
    ModelInfo *model = (ModelInfo *)client;
   
    if((ev->x >= 0) && (ev->x < WINDOW_SIZE) &&
       (ev->y >= 0) && (ev->x < WINDOW_SIZE))
    {
        XtVaGetValues(radZoom, XtNstate, &ToggleState, NULL);
        if(ToggleState == 1) /* Zoom is the current selection */
        {
            if(ev->button == 1) /* left button pressed - zoom in */
            {
                StartX = ev->x;
                StartY = ev->y;

                Pressed = 1;

                RectangleX = StartX;
                RectangleY = StartY;
                RectangleSize = 1;

                XDrawRectangle(display, matrix_window, invert_gc,
                               RectangleX, RectangleY, RectangleSize, RectangleSize);
            }
            else if(ev->button == 3) /* right button - zoom out */
            {
                Stack_Pop(&S, NULL);  /* pops the current window block */

                if (Stack_GetFirst(&S,&wb)!=-1) 
                {
                    x = wb.x; y = wb.y; size = wb.size;
                }
                else /* if the stack is empty ... */
                {
                    x = 1; y = 1; size = NSTATES;
                }

                /*************************************************
                    Call the Exposure Event Handler
                 **************************************************/
                e.count = 0;
                redisplay_event(w, (XtPointer)model, &e);

                sprintf(buffer, "%d", x);
                XtVaSetValues(ascX, XtNstring, buffer, NULL);
                sprintf(buffer, "%d", y);
                XtVaSetValues(ascY, XtNstring, buffer, NULL);
                sprintf(buffer, "%d", size);
                XtVaSetValues(ascSize, XtNstring, buffer, NULL);
            }
        }
        else if(ToggleState == 0) /* Check state transition is the current selection */
        {
            XDrawLine(display, XtWindow(coreScale), invert_gc,
                    ScalePointerLevel, 0, ScalePointerLevel, SCALEHEIGHT);

            i = (int)floor(ev->x*StatesPerPixel)+x;
            j = (int)floor(ev->y*StatesPerPixel)+y;

            if((i < 1) || (i > NSTATES)) return;
            if((j < 1) || (j > NSTATES)) return;

            pt = MATRIX[i];
            prob = 0.0;
            while(pt != NULL)
            {
                if(pt->state_id == j)
                {
                    prob = (double)pt->value;
                    break;
                }
                pt = pt->next;
            }

            sprintf(outputstr, "%d", j);
            XtVaSetValues(butOriginState, XtNlabel, outputstr, XtNwidth, STATEBUTTON_WIDTH, NULL);
            UpdateMenuItems(smeMenuOriginItems, model, j);
            CurrentOriginState = j;

            sprintf(outputstr, "%d", i);
            XtVaSetValues(butDestState, XtNlabel, outputstr, XtNwidth, STATEBUTTON_WIDTH, NULL);
            UpdateMenuItems(smeMenuDestItems, model, i);
            CurrentDestState = i;

            sprintf(outputstr, "= %.10E", prob);
            XtVaSetValues(labProb, XtNlabel, outputstr, NULL);

            ScalePointerLevel = (int)rint(prob * (double)(SCALEWIDTH-1));
   
            XDrawLine(display, XtWindow(coreScale), invert_gc,
                    ScalePointerLevel, 0, ScalePointerLevel, SCALEHEIGHT);
        }
    }
	

}

void motion_event(w, client, ev)
Widget w;
XtPointer client;
XMotionEvent *ev;
{
    Display *display = XtDisplay(w);
    Window matrix_window = XtWindow(w);

    if(Pressed)
    {
        XDrawRectangle(display, matrix_window, invert_gc,
                       RectangleX, RectangleY, RectangleSize, RectangleSize);

        FinalX = ev->x;
        FinalY = ev->y;

        if(FinalX < 0)
            FinalX = 0;
        else if(FinalX >= WINDOW_SIZE)
            FinalX = WINDOW_SIZE-1;
        if(FinalY < 0)
            FinalY = 0;
        else if(FinalY >= WINDOW_SIZE)
            FinalY = WINDOW_SIZE-1;

        RectangleSize = min(abs(StartX-FinalX), abs(StartY-FinalY));
        if(FinalX > StartX)
        {
            if(FinalY > StartY)
            {
                RectangleX = StartX;
                RectangleY = StartY;
            }
            else
            {
                RectangleX = StartX;
                RectangleY = StartY - RectangleSize;
            }
        }
        else
        {
            if(FinalY > StartY)
            {
                RectangleX = StartX - RectangleSize;
                RectangleY = StartY;
            }
            else
            {
                RectangleX = StartX - RectangleSize;
                RectangleY = StartY - RectangleSize;
            }                
        }

        XDrawRectangle(display, matrix_window, invert_gc,
                       RectangleX, RectangleY, RectangleSize, RectangleSize);
    }
}

void buttonrelease_event(w, client, ev)
Widget w;
XtPointer client;
XButtonEvent *ev;
{
    Display *display = XtDisplay(w);
    Window matrix_window = XtWindow(w);
    XExposeEvent e;
    char buffer[50];
    WindowBlock wb;
	
    ModelInfo *model = (ModelInfo *)client;	

    if(Pressed)
    {
        XDrawRectangle(display, matrix_window, invert_gc,
                       RectangleX, RectangleY, RectangleSize, RectangleSize);

        x = (int)floor(RectangleX*StatesPerPixel)+x;
        y = (int)floor(RectangleY*StatesPerPixel)+y;
        size = (int)ceil(RectangleSize*StatesPerPixel);

        wb.x = x; wb.y = y; wb.size = size;

        Stack_Push(&S,&wb);

        /*************************************************
            Call the Exposure Event Handler
         **************************************************/
        e.count = 0;
        redisplay_event(w, (XtPointer)model, &e);

        sprintf(buffer, "%d", x);
        XtVaSetValues(ascX, XtNstring, buffer, NULL);
        sprintf(buffer, "%d", y);
        XtVaSetValues(ascY, XtNstring, buffer, NULL);
        sprintf(buffer, "%d", size);
        XtVaSetValues(ascSize, XtNstring, buffer, NULL);

        sprintf(StatusMessage, "Zoom level has been set.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);

        Pressed = 0;
    }
}

void click_butQuit(Widget w, XtPointer client_data, XtPointer call_data)
{

    quit = 1;
}


void click_butP(Widget w, XtPointer client_data, XtPointer call_data)
{
	char *string;
    ModelInfo *model = (ModelInfo *)client_data;
    XExposeEvent e;
    Boolean toggstate;
    XtVaGetValues(w, XtNstate, &toggstate, NULL);

    XtVaGetValues(ascOutFile, XtNstring, &string,  NULL);
	strncpy(model->argsinfo->PrintScreenOutFile, string, MAXLINESIZE);
	
    XtVaGetValues(ascLegend, XtNstring, &string, NULL);
	strncpy(model->argsinfo->Legend, string, MAXLINESIZE);
	
	save_matrix(XtDisplay(w), client_data);
	
    redisplay_event(coreMatrix, (XtPointer)model, &e);
    
}


void toggle_togBlock(Widget w, XtPointer client_data, XtPointer call_data)
{
    ModelInfo *model = (ModelInfo *)client_data;
    XExposeEvent e;
    Boolean toggstate;
    XtVaGetValues(w, XtNstate, &toggstate, NULL);

    if(toggstate)
    {
        XtVaSetValues(w, XtNlabel, "Show Blocks: On", NULL);
        sprintf(StatusMessage, "Blocks are now being shown.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }
    else
    {
        XtVaSetValues(w, XtNlabel, "Show Blocks: Off", NULL);
        sprintf(StatusMessage, "Blocks are now being hidden.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }
    
    /*************************************************
        Call the Exposure Event Handler
     **************************************************/     
    e.count = 0;
    redisplay_event(coreMatrix, (XtPointer)model, &e);
    
}

void click_butRemoveAllBlocks(Widget w, XtPointer client_data, XtPointer call_data)
{
    XExposeEvent e;
    ModelInfo *model = (ModelInfo *)client_data;	

    if(LL.nelem == 0)
    {
        sprintf(StatusMessage, "There are no blocks to be removed.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    while(LL.nelem > 0)
        LList_RemovePos(&LL, 0);

    XawListChange(lstBlock, EMPTY_LIST, 1, LISTITEMSWIDTH, TRUE);

    XtVaSetValues(vpBlockList, XtNwidth, VPBLOCKLIST_WIDTH, XtNheight, VPBLOCKLIST_HEIGHT, NULL); // sets the view port size

    sprintf(StatusMessage, "All blocks have been removed.");
    XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);

    /*************************************************
        Call the Exposure Event Handler
     **************************************************/
    e.count = 0;
    redisplay_event(coreMatrix, (XtPointer)model, &e);
}

void click_butRemoveBlock(Widget w, XtPointer client_data, XtPointer call_data)
{
    XawListReturnStruct *listret;
    XExposeEvent e;
    LLNode *pt; 
    int i=0;
    ModelInfo *model = (ModelInfo *)client_data;	

    listret = XawListShowCurrent(lstBlock);
    if(listret == NULL)
        return;
    if(listret->list_index == XAW_LIST_NONE)
    {
        sprintf(StatusMessage, "You must select a valid block before trying to remove it.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }
    if(LList_RemovePos(&LL, listret->list_index) == -1)
    {
        sprintf(StatusMessage, "You must select a valid block before trying to remove it.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    listitems = (String*)realloc(listitems, (LL.nelem)*(sizeof(String)));
    for(pt = LL.ptfirst->next; pt != NULL; pt = pt->next)
    {
        listitems[i] = (char*)malloc(20*sizeof(char));
        sprintf(listitems[i++], "%7d   %7d", pt->info.initialState, pt->info.size);
    }

    if(LL.nelem > 0)
    {
        XawListChange(lstBlock, listitems, LL.nelem, LISTITEMSWIDTH, TRUE);
    }
    else
    {
        XawListChange(lstBlock, EMPTY_LIST, 1, -1, TRUE);
    }

    sprintf(StatusMessage, "Block removed.");
    XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);

    XtVaSetValues(vpBlockList, XtNwidth, VPBLOCKLIST_WIDTH, XtNheight, VPBLOCKLIST_HEIGHT, NULL); // sets the view port size

    /*************************************************
        Call the Exposure Event Handler
     **************************************************/
    e.count = 0;
    redisplay_event(coreMatrix, (XtPointer)model, &e);
}

void click_butAddBlock(Widget w, XtPointer client_data, XtPointer call_data)
{    
    XExposeEvent e;
    LLNode *pt; 

    MatrixBlock mb;
    char *string;
    int tempInitialState, tempSize;
    long nblocks, blocksadded, i=0;
	
    ModelInfo *model = (ModelInfo *)client_data;	

    XtVaGetValues(ascInitialState, XtNstring, &string, NULL);
    tempInitialState = atoi(string);
    if( ( tempInitialState <= 0 ) || ( tempInitialState > NSTATES ) )
    {
        sprintf(StatusMessage, "Invalid initial state. Cannot add blocks.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    XtVaGetValues(ascBlockSize, XtNstring, &string, NULL);
    tempSize = atoi(string);
    if(tempSize <= 0)
    {
        sprintf(StatusMessage, "Invalid size. Cannot add blocks.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    if( (tempInitialState + tempSize - 1) > NSTATES )
    {
        sprintf(StatusMessage, "Invalid combination of initial state and size. Cannot add blocks.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    XtVaGetValues(ascNBlocks, XtNstring, &string, NULL);
    nblocks = atol(string);
    if(nblocks <= 0)
    {
        sprintf(StatusMessage, "Invalid number of blocks. Cannot add blocks.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    mb.initialState = tempInitialState;
    mb.size = tempSize;

    blocksadded = 0;
    for(i = 0; i < nblocks; i++)
    {
        if((mb.initialState + mb.size - 1) <= NSTATES)
        {
            if(LList_Insert(&LL, &mb) == -1)
                break;            /* block can't be added because it overlaps another block */
            blocksadded++;
        }
        else
        {
            /* block can't be added because it's out of the matrix bounds */
            mb.size = NSTATES - mb.initialState + 1;
            if(mb.size > 0)
                LList_Insert(&LL, &mb);
            break;
        }
        /* if no error is reported increases initialState and try another block */
        mb.initialState += tempSize;
    }

    if(blocksadded == 0) /* no blocks inserted entirely */
    {
        sprintf(StatusMessage, "No block has been completely inserted.");
        if(mb.size != tempSize) /* a smaller block was added */
        {
            strcat(StatusMessage, " Space filled with a smaller block.");
        } /* or else no block was really added at all */
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }
    else if(blocksadded == nblocks) /* added all requested blocks */
    {
        sprintf(StatusMessage, "All blocks have been completely inserted.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }
    else
    {
        sprintf(StatusMessage, "Only %ld block%s completed.", blocksadded, (blocksadded == 1 ? " was":"s were"));
        if(mb.size != tempSize)
        {
            strcat(StatusMessage, "Remaining space filled with a smaller block.");
        }
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }

    listitems = (String*)realloc(listitems, (LL.nelem)*(sizeof(String)));
    for(i = 0, pt=LL.ptfirst->next; pt!=NULL; pt=pt->next)
    {
        listitems[i] = (char*)malloc(20*sizeof(char));
        sprintf(listitems[i++], "%7d   %7d", pt->info.initialState, pt->info.size);
    }

    XawListChange(lstBlock, listitems, LL.nelem, LISTITEMSWIDTH, TRUE);

    XtVaSetValues(vpBlockList, XtNwidth, VPBLOCKLIST_WIDTH, XtNheight, VPBLOCKLIST_HEIGHT, NULL); // sets the view port size

    e.count = 0;
	
	
    redisplay_event(coreMatrix, (XtPointer)model, &e);
}

void click_butSetZoom(Widget w, XtPointer client_data, XtPointer call_data)
{
    XExposeEvent e;
    char *string;
    int tempx, tempy, tempsize;
	
    ModelInfo *model = (ModelInfo *)client_data;	

    XtVaGetValues(ascX, XtNstring, &string, NULL);
    tempx = atoi(string);
    if((tempx < 1) || (tempx > NSTATES))
    {
        sprintf(StatusMessage, "Invalid x coordinate. Cannot set zoom.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    XtVaGetValues(ascY, XtNstring, &string, NULL);
    tempy = atoi(string);
    if((tempy < 1) || (tempy > NSTATES))
    {
        sprintf(StatusMessage, "Invalid y coordinate. Cannot set zoom.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    XtVaGetValues(ascSize, XtNstring, &string, NULL);
    tempsize = atoi(string);
    if((tempsize < 1) || ((tempx+tempsize-1) > NSTATES) || ((tempy+tempsize-1) > NSTATES))
    {
        sprintf(StatusMessage, "Invalid size. Cannot set zoom.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    x = tempx;
    y = tempy;
    size = tempsize;

    sprintf(StatusMessage, "Zoom level has been set.");
    XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);

    /*************************************************
        Call the Exposure Event Handler
     **************************************************/
    e.count = 0;
    redisplay_event(coreMatrix, (XtPointer)model, &e);
}

void click_butGenerateOutput(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *filename;
    FILE *fpout;
    LLNode *pt;
	
    if(LL.nelem == 0)
    {
        sprintf(StatusMessage, "There are no blocks defined. Cannot generate output.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    pt = LL.ptfirst;
    if(pt->next->info.initialState != 1)
    {
        sprintf(StatusMessage, "The matrix is not completely filled with blocks. Cannot generate output.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    pt = pt->next;

    while(pt->next != NULL)
    {
        if(pt->next->info.initialState != (pt->info.initialState + pt->info.size))
        {
            sprintf(StatusMessage, "The matrix is not completely filled with blocks. Cannot generate output.");
            XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
            return;
        }
        pt = pt->next;
    }

    if((pt->info.initialState + pt->info.size - 1) != NSTATES)
    {
        sprintf(StatusMessage, "The matrix is not completely filled with blocks. Cannot generate output.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }
        
    filename = (char *)malloc(strlen((char *)client_data) + strlen(EXT_BLOCKS) + 1);
    strcpy(filename, (char *)client_data);
    strcat(filename, EXT_BLOCKS);
    fpout = fopen(filename, "w");
    memfree(filename);

    if(fpout == NULL)
    {
        sprintf(StatusMessage, "Couldn't open file for writing. Output has not been generated.");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
        return;
    }

    fprintf(fpout, "%d\n", LL.nelem);

    for(pt = LL.ptfirst->next; pt != NULL; pt = pt->next)
    {
        LLNode *lookahead;
        lookahead = pt;
        while((lookahead->next != NULL) && (lookahead->next->info.size == pt->info.size))
            lookahead = lookahead->next;
        /* when the loop ends lookahead must point to the last block in the sequence */
        pt = lookahead;
        fprintf(fpout, "s %d %d\n", pt->info.size, (pt->info.initialState + pt->info.size - 1));
    }

    fclose(fpout);

    sprintf(StatusMessage, "Output has been generated.");
    XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
}

void UpdateMenuItems(Widget *menuItems, ModelInfo *model, int state)
{
    int i;
    int templen, maxlen = -1;
    char varstring[80], formatstring[20], valuestring[40];

    for(i = 0; i < model->nvars; i++)
    {
        templen = strlen(model->Variables[i].varname);
        if(templen > maxlen)
            maxlen = templen;
    }

    sprintf(formatstring, "%%-%ds  %%7s", maxlen);

    for(i = 0; i < model->nvars; i++)
    {
        int aux = 0;
        
        if(state == -1)
            sprintf(valuestring, "?");
        else
            sprintf(valuestring, "%d", (model->ModelStates[state]).tuple[i]);

        /* state translation */            
            
        for ( aux = 0; model->PermutationVector[aux] != i; aux ++)
            ;

        sprintf(varstring, formatstring, /* model->Variables[model->PermutationVector[i]].varname, */
            model->Variables[ aux ].varname,
            valuestring);
        XtVaSetValues(menuItems[i], XtNlabel, varstring, NULL);
    }
}

void getTupleString( char *retstring, int state, ModelInfo *model )
{
    int i;
    char valuestring[20];

    retstring[0] = '\0';

    for(i = 0; i < model->nvars-1; i++)
    {
        sprintf(valuestring, "%d", (model->ModelStates[state]).tuple[i]);
        if( strlen(valuestring) + 5 <= MAXTUPLESIZE - strlen(retstring) )
        {
            strcat(retstring, valuestring);
            strcat(retstring, ", ");
        }
        else
        {
            strcat(retstring, "...");
            return;
        }
    }
    sprintf(valuestring, "%d", (model->ModelStates[state]).tuple[i]);
    if( strlen(valuestring) <= MAXTUPLESIZE - strlen(retstring) )
    {
        strcat(retstring, valuestring);
    }
    else
    {
        strcat(retstring, "...");
    }    
}

void enter_butState(Widget w, XtPointer client_data, XtPointer call_data)
{
    char tuplestring[MAXTUPLESIZE+1];
    int state = -1;
    ModelInfo *model = (ModelInfo *)client_data;

    if( w == butOriginState )
        state = CurrentOriginState;
    else if( w == butDestState )
        state = CurrentDestState;

    if( state > 0 )
    {
        sprintf(StatusMessage, "State %d - <", state);
        getTupleString(tuplestring, state, model);
        strcat(StatusMessage, tuplestring);
        strcat(StatusMessage, ">");
        XtVaSetValues(labStatus, XtNlabel, StatusMessage, NULL);
    }
}

void ProbTableFree()
{
	int i;
    for(i = 0; i < WINDOW_SIZE; i++)
        memfree(ProbTable[i]);
    memfree(ProbTable);
}

void ColorFree(Display *display, ModelInfo *Model, GC **gc)
{
	int i;
	
    for(i = 0; i < NOGCS; i++)
        XFreeGC(display, (*gc)[i]);

    memfree(*gc);
	
	memfree(Model->colors);
}

void ColorAllocation(Display *display, Colormap cmap, Window matrix_window, ModelInfo *Model, GC **gc, GC **pregc)
{
    XGCValues values;
    unsigned long mask;
    int rinc, ginc, binc, i;
		
    NOGCS = 1<<Model->argsinfo->ColorDepth;
	

	
    *gc = (GC *)malloc((NOGCS + 1) * sizeof(GC));
    *pregc = (GC *)malloc((NOPREGCS + 1) * sizeof(GC));
    if(*gc == NULL || *pregc == NULL)
    {
        fprintf(stderr, "Couldn't allocate memory for resources: Graphic Contexts\n");
        exit(ERR_MEMORY);
    }
	




	/* Color Allocation */
	
	
	
	
	Model->colors = malloc((NOGCS+2)*sizeof(XColor));

	Model->precolors = malloc((NOPREGCS+2)*sizeof(XColor));
	


    /* Trying to allocate background color */
    Model->colors[NOGCS+1].red = ColorData[Model->argsinfo->BackgroundColor].red;
    Model->colors[NOGCS+1].green = ColorData[Model->argsinfo->BackgroundColor].green;
    Model->colors[NOGCS+1].blue = ColorData[Model->argsinfo->BackgroundColor].blue;
    if(XAllocColor(display, cmap, &(Model->colors[NOGCS+1])) == 0)
    {
        fprintf(stderr, "Color #%02X%02X%02X couldn't be allocated in this display.\n", Model->colors[NOGCS+1].red>>8, Model->colors[NOGCS+1].green>>8, Model->colors[NOGCS+1].blue>>8);
    }
	
	

    values.line_width = 1;
    values.function = GXinvert;

    mask = GCFunction | GCLineWidth; /* mask for the inverting GC */
    invert_gc = XCreateGC(display, matrix_window, mask, &values);

    mask = GCForeground | GCLineWidth; /* mask for all the other GCs */

    /* Selecting Colors */
    if((ColorData[Model->argsinfo->FinalColor].red - ColorData[Model->argsinfo->StartColor].red) > 0)
        rinc = (255/(NOGCS-1))<<8;
    else if((ColorData[Model->argsinfo->FinalColor].red - ColorData[Model->argsinfo->StartColor].red) < 0)
        rinc = -((255/(NOGCS-1))<<8);
    else
        rinc = 0;

    if((ColorData[Model->argsinfo->FinalColor].green - ColorData[Model->argsinfo->StartColor].green) > 0)
        ginc = (255/(NOGCS-1))<<8;
    else if((ColorData[Model->argsinfo->FinalColor].green - ColorData[Model->argsinfo->StartColor].green) < 0)
        ginc = -((255/(NOGCS-1))<<8);
    else
        ginc = 0;

    if((ColorData[Model->argsinfo->FinalColor].blue - ColorData[Model->argsinfo->StartColor].blue) > 0)
        binc = (255/(NOGCS-1))<<8;
    else if((ColorData[Model->argsinfo->FinalColor].blue - ColorData[Model->argsinfo->StartColor].blue) < 0)
        binc = -((255/(NOGCS-1))<<8);
    else
        binc = 0;

    for(i = 0; i < NOGCS; i++)
    {

        Model->colors[i].red   = ColorData[Model->argsinfo->StartColor].red   + (i*rinc);
        Model->colors[i].green = ColorData[Model->argsinfo->StartColor].green + (i*ginc);
        Model->colors[i].blue  = ColorData[Model->argsinfo->StartColor].blue  + (i*binc);

        if(XAllocColor(display, cmap, &(Model->colors[i])) == 0)
            fprintf(stderr, "Color #%02X%02X%02X couldn't be allocated in this display.\n", Model->colors[i].red>>8, Model->colors[i].green>>8, Model->colors[i].blue>>8);

        values.foreground = Model->colors[i].pixel;
        (*gc)[i] = XCreateGC(display, matrix_window, mask, &values);
    }

	if (Model->argsinfo->UseSpecialColor)
	{

    	Model->colors[NOGCS].red   = ColorData[Model->argsinfo->SpecialColor].red;
    	Model->colors[NOGCS].green = ColorData[Model->argsinfo->SpecialColor].green;
    	Model->colors[NOGCS].blue  = ColorData[Model->argsinfo->SpecialColor].blue;


    	if(XAllocColor(display, cmap, &(Model->colors[NOGCS])) == 0)
        	fprintf(stderr, "Color #%02X%02X%02X couldn't be allocated in this display.\n", 
			  Model->colors[NOGCS].red>>8, Model->colors[NOGCS].green>>8, Model->colors[NOGCS].blue>>8);

    	values.foreground = Model->colors[NOGCS].pixel;	
    	(*gc)[NOGCS] = XCreateGC(display, matrix_window, mask, &values);	

	}


    for(i = 0; i < NOPREGCS; i++)
    {

        Model->precolors[i].red   = ColorData[i].red   ;
        Model->precolors[i].green = ColorData[i].green ;
        Model->precolors[i].blue  = ColorData[i].blue  ;

        if(XAllocColor(display, cmap, &(Model->precolors[i])) == 0)
            fprintf(stderr, "Color #%02X%02X%02X couldn't be allocated in this display.\n", Model->precolors[i].red>>8, Model->precolors[i].green>>8, Model->precolors[i].blue>>8);

        values.foreground = Model->precolors[i].pixel;
        (*pregc)[i] = XCreateGC(display, matrix_window, mask, &values);
		
    }

	

}


void LoadPartitions(ModelInfo *Model)
{
    LList_InitLList(&LL);
	


    if(Model->hasPartition)
    {
        FILE *fpin;
        MatrixBlock mb;
        char line[MAXLINESIZE], *pt;
        int nblocks, i, j, fs, qty, laststate;
		ArgsInfo *Options = Model->argsinfo;		

        fpin = OpenFile(Options->FileName, EXT_BLOCKS, "r");

        if (fgets(line, MAXLINESIZE, fpin)==NULL)
	{
	      fclose(fpin);
	      return;
	}

        nblocks = atoi(line);
        laststate = 0;
        for(i = 0; i < nblocks; i++)
        {
            if(fgets(line, MAXLINESIZE, fpin) == NULL)
                break;
            if(line[strlen(line)-1] == '\n')
                line[strlen(line)-1] = '\0';
            if(line[0] == '\0')
                break;

            pt = line;
            while(!isdigit(*pt)) pt++;

            sscanf(pt, "%d %d", &mb.size, &fs);

            if(fs <= laststate)
            {
                fprintf(stderr, "WARNING: Integrity error in partition file.\n");
                while(LL.nelem > 0)
                {
                    LList_RemovePos(&LL, 0);
                }
                break;
            }
            qty = (fs - laststate)/mb.size;

            mb.initialState = laststate+1;
            for(j = 0; j < qty; j++)
            {
                if(LList_Insert(&LL, &mb) == -1)
                {
                    fprintf(stderr, "WARNING: Error inserting block from partition file.\n");
                    while(LL.nelem > 0)
                    {
                        LList_RemovePos(&LL, 0);
                    }
                    break;
                }
                mb.initialState += mb.size;
            }
            laststate = mb.initialState - 1;
        }
        fclose(fpin);
    }
	

}



/**********************************************************************
 *                                                                    *
 *  DrawMatrix(ModelInfo *Model, ArgsInfo *Options)                   *
 *      Draws the transition matrix on an X window                    *
 *      Model - the model information                                 *
 *      Options - the program options                                 *
 *                                                                    *
 **********************************************************************/
void DrawMatrix(ModelInfo *Model, ArgsInfo *Options)
{
    XtAppContext app_context;
    Display *display;
    Pixmap icon_pixmap, magnify_pixmap, view_pixmap, p_pixmap;
    XEvent event;
    Colormap cmap;
    Window matrix_window;
		
    int i, templen, maxlen;

    int narg = 0;
    char strsize[20], *xenv, *envval;
    char titlestr[80], menutitle[80], formatstring[40];
    String trans_checkButton =
"<EnterWindow>: highlight(Always)\n\
<LeaveWindow>: unhighlight()\n\
<Btn1Down>,<Btn1Up>: set() notify()";
    String trans_menu =
"<EnterWindow>:\n\
<LeaveWindow>: unhighlight()\n\
<BtnMotion>:\n\
<BtnUp>: MenuPopdown() notify() unhighlight()\n";
    String trans_menuButton =
"<EnterWindow>: highlight() set() notify() unset()\n\
<LeaveWindow>: reset()\n\
<BtnDown>: reset() PopupMenu()\n";



    envval = (char *)malloc(strlen("XENVIRONMENT=")+strlen(Options->ProgramPath)+strlen("MtxVisStPermut")+1);
    strcpy(envval, "XENVIRONMENT=");
    strcat(envval, Options->ProgramPath);
    strcat(envval, "MtxVisStPermut");
    xenv = getenv("XENVIRONMENT");
    putenv(envval);

    maxlen = -1;
    for(i = 0; i < Model->nvars; i++)
    {
        templen = strlen(Model->Variables[i].varname);
        if(templen > maxlen)
            maxlen = templen;
    }
    sprintf(formatstring, "%%-%ds  %%7s", maxlen);
    sprintf(menutitle, formatstring, "State Variable", "Value");

    CurrentOriginState = CurrentDestState = -1;


    ScalePointerLevel = 0;
    
    strcpy(titlestr, "Matrix Visualization & States Permutation - ");
    strcat(titlestr, Model->ModelName);
    
    x = 1;
    y = 1;


    quit = 0;
    Pressed = 0;

    Stack_InitStack(&S);
	
	LoadPartitions(Model);

	
    topLevel = XtVaAppInitialize(&app_context, "MtxVisStPermut", NULL, 0, &narg, NULL,
        NULL, NULL);

        /* Horizontal box */
        frmMain = XtVaCreateManagedWidget("frmMain", formWidgetClass, topLevel,
            XtNborderWidth, 0, NULL);

            coreMatrix = XtVaCreateManagedWidget("coreMatrix", coreWidgetClass, frmMain,
                XtNwidth, WINDOW_SIZE, XtNheight, WINDOW_SIZE, NULL);

            /* Vertical box */
            frmZoomInfo = XtVaCreateManagedWidget("frmZoomInfo", formWidgetClass, frmMain,
                XtNfromVert, coreMatrix, XtNwidth, WINDOW_SIZE, NULL);

                labZoom = XtVaCreateManagedWidget("labZoom", labelWidgetClass, frmZoomInfo,
                    XtNlabel, "Zoom Information", XtNjustify, XtJustifyLeft,
                    XtNwidth, 240, XtNborderWidth, 0, NULL);

                labLevel = XtVaCreateManagedWidget("labLevel", labelWidgetClass, frmZoomInfo,
                    XtNlabel, "Level", XtNjustify, XtJustifyLeft, XtNborderWidth, 0,
                    XtNwidth, 60, XtNfromHoriz, labZoom, NULL);

                labLevelVal = XtVaCreateManagedWidget("labLevelVal", labelWidgetClass, frmZoomInfo,
                    XtNlabel, "1:1", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                    XtNwidth, 80, XtNfromHoriz, labLevel, NULL);

                hboxZoom = XtVaCreateManagedWidget("hboxZoom", boxWidgetClass, frmZoomInfo,
                    XtNorientation, XtorientHorizontal, XtNfromVert, labZoom, NULL);

                    labX = XtVaCreateManagedWidget("labX", labelWidgetClass, hboxZoom,
                        XtNlabel, "x:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                        XtNwidth, 20, NULL);

                    ascX = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, hboxZoom,
                        XtNtype, XawAsciiString, XtNeditType, XawtextEdit, XtNstring, "1",
                        XtNwidth, 60, NULL);

                    labY = XtVaCreateManagedWidget("labY", labelWidgetClass, hboxZoom,
                        XtNlabel, "y:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                        XtNwidth, 20, NULL);

                    ascY = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, hboxZoom,
                        XtNtype, XawAsciiString, XtNeditType, XawtextEdit, XtNstring, "1",
                        XtNwidth, 60, NULL);

                    labSize = XtVaCreateManagedWidget("labSize", labelWidgetClass, hboxZoom,
                        XtNlabel, "Size:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                        XtNwidth, 50, NULL);

                    sprintf(strsize, "%d", size);
                    ascSize = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, hboxZoom,
                        XtNtype, XawAsciiString, XtNeditType, XawtextEdit, XtNstring, strsize,
                        XtNwidth, 60, NULL);

                    butSetZoom = XtVaCreateManagedWidget("button", commandWidgetClass, hboxZoom,
                        XtNlabel, "Set Zoom", XtNwidth, 80, NULL);

            /* Vertical box */
            vboxSideBar = XtVaCreateManagedWidget("vboxSideBar", boxWidgetClass, frmMain,
                XtNfromHoriz, coreMatrix, XtNwidth, 150, XtNheight, 500, XtNborderWidth, 0,
                XtNhSpace, 0, NULL);

                frmToolBar = XtVaCreateManagedWidget("frmToolBar", formWidgetClass, vboxSideBar,
                    XtNwidth, 150, XtNborderWidth, 1, NULL);

                    radZoom = XtVaCreateManagedWidget("radZoom", toggleWidgetClass, frmToolBar,
                        XtNstate, 1, XtNtranslations, XtParseTranslationTable(trans_checkButton), 
                        XtNwidth, 35, XtNheight, 35, NULL);

                    radView = XtVaCreateManagedWidget("radView", toggleWidgetClass, frmToolBar,
  		        XtNstate, 0,
                        XtNradioGroup, radZoom, XtNtranslations, XtParseTranslationTable(trans_checkButton),
                        XtNfromHoriz, radZoom, XtNwidth, 35, XtNheight, 35, NULL);
						
						
					butP = XtVaCreateManagedWidget("butP", commandWidgetClass, frmToolBar,
                        XtNstate, 0, 
                        XtNwidth, 35, XtNheight, 35, XtNresize, FALSE, XtNfromHoriz, radView, NULL);

                    togBlock = XtVaCreateManagedWidget("button", toggleWidgetClass, frmToolBar,
                        XtNstate, FALSE, XtNlabel, "Show Blocks: Off", XtNwidth, 156, XtNresize, FALSE,
                        XtNfromVert, radZoom, NULL);
						
                    labLegend = XtVaCreateManagedWidget("labLegend", labelWidgetClass, frmToolBar,
                        XtNlabel, "Legend:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                        XtNwidth, 60, XtNfromVert, togBlock, NULL);

                    ascLegend = XtVaCreateManagedWidget("ascLegend", asciiTextWidgetClass, frmToolBar,
                        XtNfromHoriz, labLegend,  XtNfromVert, togBlock, XtNtype, XawAsciiString,
                        XtNeditType, XawtextEdit, XtNwidth, 82, NULL);

                    labOutFile = XtVaCreateManagedWidget("labOutFile", labelWidgetClass, frmToolBar,
                        XtNlabel, "File:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                        XtNwidth, 60, XtNfromVert, labLegend, NULL);

                    ascOutFile = XtVaCreateManagedWidget("ascOutFile", asciiTextWidgetClass, frmToolBar,
                        XtNfromHoriz, labOutFile, XtNtype, XawAsciiString,
                        XtNeditType, XawtextEdit, XtNwidth, 82, XtNfromVert, labLegend, NULL);

						

                /* Vertical box */
                vboxBlockInfo = XtVaCreateManagedWidget("vboxBlockInfo", boxWidgetClass, vboxSideBar, XtNwidth, 190, NULL);

                    labBlock = XtVaCreateManagedWidget("labBlock", labelWidgetClass, vboxBlockInfo,
                        XtNlabel, "Block Information", XtNwidth, 158, XtNborderWidth, 0, NULL);

                    frmBlock = XtVaCreateManagedWidget("frmBlock", formWidgetClass, vboxBlockInfo, NULL);

                        labInitialState = XtVaCreateManagedWidget("labInitialState", labelWidgetClass, frmBlock,
                            XtNlabel, "Initial\nState:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                            XtNwidth, 60, NULL);

                        ascInitialState = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, frmBlock,
                            XtNfromHoriz, labInitialState, XtNtype, XawAsciiString,
                            XtNeditType, XawtextEdit, XtNwidth, 82, NULL);

                        labBlockSize = XtVaCreateManagedWidget("labBlockSize", labelWidgetClass, frmBlock,
                            XtNlabel, "Size:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                            XtNfromVert, labInitialState, XtNwidth, 60, NULL);

                        ascBlockSize = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, frmBlock,
                            XtNfromHoriz, labBlockSize, XtNfromVert, labInitialState, XtNtype, XawAsciiString,
                            XtNeditType, XawtextEdit, XtNwidth, 82, NULL);

                        labNBlocks = XtVaCreateManagedWidget("labNBlocks", labelWidgetClass, frmBlock,
                            XtNlabel, "Qty.:", XtNjustify, XtJustifyRight, XtNborderWidth, 0,
                            XtNfromVert, ascBlockSize, XtNwidth, 60, NULL);

                        ascNBlocks = XtVaCreateManagedWidget("ascText", asciiTextWidgetClass, frmBlock,
                            XtNfromHoriz, labNBlocks, XtNfromVert, ascBlockSize, XtNtype, XawAsciiString,
                            XtNeditType, XawtextEdit, XtNwidth, 82, NULL);

                        butAddBlock = XtVaCreateManagedWidget("button", commandWidgetClass, frmBlock,
                            XtNfromVert, ascNBlocks, XtNlabel, "Add Blocks", XtNwidth, 146, NULL);
                    
                    labListTitle = XtVaCreateManagedWidget("labListTitle", labelWidgetClass, vboxBlockInfo,
                            XtNlabel, "Init. State   Size", XtNborderWidth, 0, XtNjustify, XtJustifyLeft,
                            XtNwidth, VPBLOCKLIST_WIDTH, NULL);        

                    vpBlockList = XtVaCreateManagedWidget("vpBlockList", viewportWidgetClass, vboxBlockInfo,
                        XtNforceBars, TRUE,  XtNallowVert, TRUE, XtNuseRight, TRUE, XtNwidth, VPBLOCKLIST_WIDTH,
                        XtNheight, VPBLOCKLIST_HEIGHT, NULL);

                        lstBlock = XtVaCreateManagedWidget("lstBlock", listWidgetClass, vpBlockList,
                            XtNlist, EMPTY_LIST, XtNdefaultColumns, 1, XtNforceColumns, TRUE,
                            XtNborderWidth, 0, XtNheight, 1000, NULL);

                    butRemoveBlock = XtVaCreateManagedWidget("button", commandWidgetClass, 
                        vboxBlockInfo, XtNlabel, "Remove Block", XtNwidth, 156, NULL);

                    butRemoveAllBlocks = XtVaCreateManagedWidget("button", commandWidgetClass, 
                        vboxBlockInfo, XtNlabel, "Remove All Blocks", XtNwidth, 156, NULL);

                    butGenerateOutput = XtVaCreateManagedWidget("button", commandWidgetClass, 
                        vboxBlockInfo, XtNlabel, "Generate Output", XtNwidth, 156, NULL);

                butQuit = XtVaCreateManagedWidget("button", commandWidgetClass, vboxSideBar,
                    XtNwidth, 166, XtNlabel, "Quit", NULL);

        /* Horizontal box */
        hboxTransitions = XtVaCreateManagedWidget("hboxTransitions", boxWidgetClass, frmMain,
            XtNorientation, XtorientHorizontal, XtNfromVert, frmZoomInfo, NULL);

            labTransition = XtVaCreateManagedWidget("labTransition", labelWidgetClass, hboxTransitions,
                XtNwidth, 80, XtNjustify, XtJustifyLeft, XtNresize, 0,
                XtNlabel, "Transition:", XtNborderWidth, 0, NULL);

            butOriginState = XtVaCreateManagedWidget("butOriginState", menuButtonWidgetClass, hboxTransitions,
                XtNlabel, "", XtNwidth, STATEBUTTON_WIDTH, XtNmenuName, "menuOrigin",
                XtNtranslations, XtParseTranslationTable(trans_menuButton), NULL );

                smeMenuOrigin = XtVaCreatePopupShell("menuOrigin", simpleMenuWidgetClass, topLevel,
                    XtNlabel, menutitle,
                    XtNtranslations, XtParseTranslationTable(trans_menu), NULL);

                smeMenuOriginItems = (Widget *)malloc(Model->nvars * sizeof(Widget));
                for(i = 0; i < Model->nvars; i++)
                    smeMenuOriginItems[i] = XtVaCreateManagedWidget("smeBSB", smeBSBObjectClass, smeMenuOrigin,
                        NULL);
                UpdateMenuItems(smeMenuOriginItems, Model, -1);

            labArrow = XtVaCreateManagedWidget("labArrow", labelWidgetClass, hboxTransitions,
                XtNwidth, 18, XtNjustify, XtJustifyLeft, XtNresize, 0,
                XtNlabel, "->", XtNborderWidth, 0, NULL);

            butDestState = XtVaCreateManagedWidget("butDestState", menuButtonWidgetClass, hboxTransitions,
                XtNlabel, "", XtNwidth, STATEBUTTON_WIDTH, XtNmenuName, "menuDest",
                XtNtranslations, XtParseTranslationTable(trans_menuButton), NULL );

                smeMenuDest = XtVaCreatePopupShell("menuDest", simpleMenuWidgetClass, topLevel,
                    XtNlabel, menutitle,
                    XtNtranslations, XtParseTranslationTable(trans_menu), NULL);

                smeMenuDestItems = (Widget *)malloc(Model->nvars * sizeof(Widget));
                for(i = 0; i < Model->nvars; i++)
                    smeMenuDestItems[i] = XtVaCreateManagedWidget("smeBSB", smeBSBObjectClass, smeMenuDest,
                        NULL);
                UpdateMenuItems(smeMenuDestItems, Model, -1);

            labProb = XtVaCreateManagedWidget("labProb", labelWidgetClass, hboxTransitions,
                XtNwidth, 150, XtNjustify, XtJustifyLeft, XtNresize, 0,
                XtNlabel, "", XtNborderWidth, 0, NULL);

            coreScale = XtVaCreateManagedWidget("coreScale", coreWidgetClass, hboxTransitions,
                XtNwidth, 150, XtNheight, 17, XtNjustify, XtJustifyRight, XtNresize, 0,
                NULL);
        /* Horizontal box */
        boxStatusBar = XtVaCreateManagedWidget("statusBar", boxWidgetClass, frmMain,
            XtNfromVert, hboxTransitions, NULL);

            labStatus = XtVaCreateManagedWidget("labStatus", labelWidgetClass, boxStatusBar,
                XtNwidth, 564, XtNjustify, XtJustifyLeft, XtNresize, 0,
                XtNlabel, "", XtNborderWidth, 0, NULL);
    /* Vertical box */

    /* Callbacks and Event Handlers */
    XtAddCallback(butSetZoom, XtNcallback, click_butSetZoom, (XtPointer)Model); 
    XtAddCallback(togBlock, XtNcallback, toggle_togBlock, (XtPointer)Model);
    XtAddCallback(butAddBlock, XtNcallback, click_butAddBlock, (XtPointer)Model);
    XtAddCallback(butRemoveBlock, XtNcallback, click_butRemoveBlock, (XtPointer)Model);
    XtAddCallback(butRemoveAllBlocks, XtNcallback, click_butRemoveAllBlocks, (XtPointer)Model);
    XtAddCallback(butQuit, XtNcallback, click_butQuit, (XtPointer)Model);
    XtAddCallback(butGenerateOutput, XtNcallback, click_butGenerateOutput, Options->FileName);
    XtAddCallback(butOriginState, XtNcallback, enter_butState, (XtPointer)Model);
    XtAddCallback(butDestState, XtNcallback, enter_butState, (XtPointer)Model);
    XtAddCallback(butP, XtNcallback, click_butP, (XtPointer)Model);	

    XtAddEventHandler(coreMatrix, ExposureMask, FALSE, redisplay_event, (XtPointer)Model);
    XtAddEventHandler(coreScale, ExposureMask, FALSE, redisplayscale_event, (XtPointer)Model);
    XtAddEventHandler(coreMatrix, ButtonPressMask, FALSE, buttonpress_event, (XtPointer)Model);
    XtAddEventHandler(coreMatrix, ButtonReleaseMask, FALSE, buttonrelease_event, (XtPointer)Model);
    XtAddEventHandler(coreMatrix, PointerMotionMask, FALSE, motion_event, (XtPointer)Model);
	
	XtRealizeWidget(topLevel);

    /* Resources Allocation */
    display = XtDisplay(coreMatrix);
    matrix_window = XtWindow(coreMatrix);
    cmap = DefaultColormapOfScreen(XtScreen(coreMatrix));

    icon_pixmap = XCreateBitmapFromData(display, XtWindow(topLevel),
                icon_bitmap_bits, icon_bitmap_width, icon_bitmap_height);

    XtVaSetValues(topLevel, XtNiconPixmap, icon_pixmap, XtNtitle, titlestr, NULL);

    magnify_pixmap = XCreateBitmapFromData(display, XtWindow(radZoom),
                magnify_bits, magnify_width, magnify_height);

    XtVaSetValues(radZoom, 
        XtNbackground, BlackPixelOfScreen(XtScreen(radZoom)),
        XtNforeground, WhitePixelOfScreen(XtScreen(radZoom)), NULL);

    XtVaSetValues(radZoom, XtNbitmap, magnify_pixmap, NULL);

    view_pixmap = XCreateBitmapFromData(display, XtWindow(radView),
                view_bits, view_width, view_height);

    XtVaSetValues(radView, XtNforeground, WhitePixelOfScreen(XtScreen(radView)),
        XtNbackground, BlackPixelOfScreen(XtScreen(radView)), NULL);

    XtVaSetValues(radView, XtNbitmap, view_pixmap, NULL);



    p_pixmap = XCreateBitmapFromData(display, XtWindow(butP),
                p_bits, p_width, p_height);

    XtVaSetValues(butP, XtNforeground, WhitePixelOfScreen(XtScreen(butP)),
        XtNbackground, BlackPixelOfScreen(XtScreen(butP)), NULL);

    XtVaSetValues(butP, XtNbitmap, p_pixmap, NULL);




    if(Model->hasPartition)
    {
        LLNode *pt; 
        listitems = (String*)realloc(listitems, (LL.nelem)*(sizeof(String)));
        for(i = 0, pt=LL.ptfirst->next; pt!=NULL; pt=pt->next)
        {
            listitems[i] = (char*)malloc(20*sizeof(char));
            sprintf(listitems[i++], "%7d   %7d", pt->info.initialState, pt->info.size);
        }
        XawListChange(lstBlock, listitems, LL.nelem, LISTITEMSWIDTH, TRUE);

        XtVaSetValues(vpBlockList, XtNwidth, VPBLOCKLIST_WIDTH, XtNheight, VPBLOCKLIST_HEIGHT, NULL); // sets the view port size
    }
	
		
	
	ColorAllocation(display, cmap, matrix_window, Model, &gc, &pregc);	
	
    XtVaSetValues(coreMatrix, XtNbackground, Model->colors[NOGCS+1].pixel, NULL);	


/*

	for(i = 0; i < NOPREGCS; i++)
    {
        color.red   = ColorData[i].red;
        color.green = ColorData[i].green;
        color.blue  = ColorData[i].blue;

        if(XAllocColor(display, cmap, &color) == 0)
            fprintf(stderr, "Color #%02X%02X%02X couldn't be allocated in this display.\n", color.red>>8, color.green>>8, color.blue>>8);

        values.foreground = color.pixel;
        pregc[i] = XCreateGC(display, matrix_window, mask, &values);
    }
    
*/
		

#ifdef DEBUG
    fprintf(stderr, "Entering event loop\n");
#endif /* DEBUG */

    do
    {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }
    while(!quit);
	
#ifdef DEBUG
    fprintf(stderr, "Leaving event loop\n");
#endif /* DEBUG */

	
	ProbTableFree();
	
    XFreeGC(display, invert_gc);
	
	ColorFree(display, Model, &gc);	

    XFreePixmap(display, icon_pixmap);
    XFreePixmap(display, magnify_pixmap);
    XFreePixmap(display, p_pixmap);	
    XFreePixmap(display, view_pixmap);

    Stack_Flush(&S);
    LList_Flush(&LL);
/*
    if(xenv)
    {
        envval = (char *)malloc(strlen("XENVIRONMENT=")+strlen(xenv)+1);
        strcpy(envval, "XENVIRONMENT=");
        strcat(envval, xenv);
        putenv(envval);
    }
    else
    {
        envval = (char *)malloc(strlen("XENVIRONMENT=")+1);
        strcpy(envval, "XENVIRONMENT=");
        putenv(envval);
    }*/
}
