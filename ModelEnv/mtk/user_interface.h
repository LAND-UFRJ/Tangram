#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <string>

#include "types.h"
#include "command.h"

class UserInterface
{
public:
    // public functions
    static bool createUserInterface ( void );
    static bool destroyUserInterface( void );

    static UserInterface * getUserInterface( void ); // instance retrieval

    void runMainLoop   ( void );         // main loop function
    bool runCommandFile( const Name & ); // execute commands from a file

    void quit   ( void ); // signal quit command
    bool history( int );  // show command history

private:
    // private data types
    typedef enum
    {
        INPUT_MODE_ONLINE,
        INPUT_MODE_BATCH
    } InputMode;

    typedef struct
    {
        InputMode mode;

        const char * file_name;

        FILE * file;
        int    line_number;

        bool end_of_input;
        bool quit_command_received;
        bool command_error;
    } InputContext;

    // private data members
    static UserInterface * the_user_interface; // pointer to Singleton

    char user_prompt[16];
    int command_count;
    int n_cmds;

    InputContext context;

    // private functions
    UserInterface( void );  // private constructor
    ~UserInterface( void ); // private destructor

    // loop control methods
    bool endOfCommands( void );
    bool endOfMainLoop( void );

    bool getUserInput    ( CommandList & );
    void updateUserPrompt( void );
};

#endif /* USER_INTERFACE_H */
