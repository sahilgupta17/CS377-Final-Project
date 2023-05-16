#ifndef _SIMPLE_SHELL_H
#define _SIMPLE_SHELL_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

#include <map>
#include <vector>

using namespace std;

class simple_shell {
  private:
    map<string, string> aliases;
    vector<string> history;
  public:
    void parse_command(char* cmd, char** cmdTokens);
    void exec_command(char** argv);
    bool isQuit(char* cmd);
    void changeDirectory(char** argv);
    void setAlias(char** argv);
};

#endif