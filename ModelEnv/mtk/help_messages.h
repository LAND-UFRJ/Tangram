#ifndef HELP_MESSAGES_H
#define HELP_MESSAGES_H

#define SET_HELP_MESSAGE \
    "Used to set a program attribute to a specific value.              \n"\
    "    Usage: set <attribute> <value>                                \n"\
    "    Where <attribute> can be one of:                              \n"\
    "        active   - name of the currently active object.           \n"\
    "        terminal - can be either 'file' or 'screen'.              \n"\
    "        output   - name of the output file if terminal is 'file'. \n"

#define SHOW_HELP_MESSAGE \
    "Used to show the value of a program attribute.                    \n"\
    "    Usage: show <attribute>                                       \n"\
    "    Where <attribute> can be one of:                              \n"\
    "        active   - name of the currently active object.           \n"\
    "        terminal - can be either 'file' or 'screen'.              \n"\
    "        output   - name of the output file if terminal is 'file'. \n"\
    "        version  - version of the Modelling ToolKit being run.    \n"

#define LIST_HELP_MESSAGE \
    "Lists either plugins or objects.                                  \n"\
    "    Usage: list (plugins|objects)                                 \n"\
    "    Where:                                                        \n"\
    "        plugins - lists all the plugins currently loaded and      \n"\
    "                  available for object creation.                  \n"\
    "        objects - lists the names of objects created.             \n"

#define HELP_HELP_MESSAGE \
    "Used to get help messages like this one.                          \n"\
    "    Usage: help <command>                                         \n"\
    "       or: help <plugin>                                          \n"\
    "       or: help <plugin>.<command>                                \n"\
    "    Where:                                                        \n"\
    "        <command> - the name of the command to get help for.      \n"\
    "        <plugin>  - the plugin to get help for.                   \n"

#define QUIT_HELP_MESSAGE \
    "Finishes the Modeling ToolKit session.                            \n"\
    "    Usage: quit                                                   \n"


#endif /* HELP_MESSAGES_H */
