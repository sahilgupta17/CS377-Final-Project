#include <tsh.h>
#include <iostream>
#include <sstream>
#include <cstring>

#include <cstdlib>

using namespace std;

string concatenateArguments(char** argv, int idx=0) {
    stringstream ss;
    
    for (int i = idx; argv[i] != nullptr; ++i) {
        ss << argv[i];
        if (argv[i + 1] != nullptr) {
            ss << ' ';  // Append space between arguments
        }
    }
    
    return ss.str();
}

void simple_shell::parse_command(char* cmd, char** cmdTokens) {
    // TODO: tokenize the command string into arguments
    char delimiter = ' ';
    stringstream ss(cmd);
    string token;
    int i = 0;
    while (getline(ss, token, delimiter)) { 
        // splitting on each word
        if (!token.empty()) {
            // removing the \n character from the token
            if (token.back() == '\n'){
                token.pop_back();
            } 
            // adding a new char* token to the token list
            cmdTokens[i] = new char[token.length() + 1]; 
            // converting the string token to char* and storing it in the token list
            strcpy(cmdTokens[i], token.c_str());  
            i++;
        }
    }
    cmdTokens[i] = NULL; 
}

void simple_shell::exec_command(char** argv) {

    // Check for the pipe symbol '|'
    bool isPipeline = false;
    char** argv1 = new char*[100];
    char** argv2 = new char*[100];
    int splitIdx = -1;

    for (int i = 0; argv[i] != NULL; ++i) {
        if (strcmp(argv[i], "|") == 0) {
            isPipeline = true;
            splitIdx = i;
            break;
        }
    }

    if (isPipeline) {
        // Split the command into two separate commands
        for (int i = 0; i < splitIdx; ++i) {
            argv1[i] = argv[i];
        }
        argv1[splitIdx] = NULL;

        int j = 0;
        for (int i = splitIdx + 1; argv[i] != NULL; ++i) {
            argv2[j] = argv[i];
            j++;
        }
        argv2[j] = NULL;

        exec_pipeline(argv1, argv2);
    } else{
        // TODO: fork a child process to execute the command.
        // parent process should wait for the child process to complete and reap it
        int pid = fork(); // forking the parent to get the child process
        if( pid < 0){ 
            cout << "Fork failed to execute" << endl;
            exit(1);
        }else if (pid == 0) {
            // child process
            if (strcmp(argv[0], "cd") == 0) {
                // Execute the cd command
                changeDirectory(argv);
            }else if(strcmp(argv[0], "history") == 0){
                // handlind the history command
                for (const string& cmd : history) {
                    cout << cmd << endl;
                }
            }else if(strcmp(argv[0], "alias") == 0){
                setAlias(argv);
            }else if (aliases.count(argv[0]) > 0) {
                // execute alias command
                string aliasedCmd = aliases[argv[0]];
                char* aliasedCmdTokens[100];
                parse_command(const_cast<char*>(aliasedCmd.c_str()), aliasedCmdTokens);
                exec_command(aliasedCmdTokens);
                for (int i = 0; aliasedCmdTokens[i] != nullptr; ++i) {
                    delete[] aliasedCmdTokens[i];
                }
            }else{
                execvp(argv[0], argv);
            }
        } else {
            // parent process waiting for child process to finish
            int wait = waitpid(pid, NULL, 0); 
        }
    }
    history.push_back(concatenateArguments(argv));
}

bool simple_shell::isQuit(char* cmd) {
    // TODO: check for the command "quit" that terminates the shell
    return (strcmp(cmd, "quit") == 0);
}

void simple_shell::changeDirectory(char** argv){
    if (argv[1] == NULL) {
        // If no directory is specified, go to the home directory
        chdir(getenv("HOME"));
    } else {
        if (chdir(argv[1]) != 0) {
            cout << "Error: no such file or directory: "  << argv[1] << endl;
        }
    }
}

void simple_shell:: setAlias(char** argv){
    string originalCommand = concatenateArguments(argv, 1);
    string aliasName;
    string command;
    bool isAliasName = true;
    bool isInQuotes = false;
    for (char c : originalCommand) {
        if (c == '\'' || c == '"') {
            isInQuotes = !isInQuotes;
        } else if (isAliasName && !isInQuotes && c == '=') {
            isAliasName = false;
        } else {
            if (isAliasName) {
                aliasName += c;
            } else {
                command += c;
            }
        }
    }
    // cout << aliasName << endl;
    // cout << command << endl;
    aliases[aliasName] = command;
}

// void simple_shell::exec_pipeline(char** argv1, char** argv2) {
//     int pipefd[2];
//     pid_t pid;

//     // Create a pipe
//     if (pipe(pipefd) == -1) {
//         cout << "Failed to create pipe." << endl;
//         return;
//     }

//     // Fork a child process for the first command
//     pid = fork();
//     if (pid < 0) {
//         cout << "Fork failed." << endl;
//         return;
//     }

//     if (pid == 0) {
//         // Child process (first command)
//         // Close the read end of the pipe
//         close(pipefd[0]);

//         // Redirect stdout to the write end of the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);

//         execvp(argv1[0], argv1);

//         // If execvp returns, it means an error occurred
//         cout << "Failed to execute command1." << endl;
//         exit(1);
//     } else {
//         // Parent process
//         // Fork another child process for the second command
//         pid_t pid2 = fork();
//         if (pid2 < 0) {
//             cout << "Fork failed." << endl;
//             return;
//         }

//         if (pid2 == 0) {
//             // Second child process (second command)
//             // Close the write end of the pipe
//             close(pipefd[1]);

//             // Redirect stdin to the read end of the pipe
//             dup2(pipefd[0], STDIN_FILENO);
//             close(pipefd[0]);

//             execvp(argv2[0], argv2);

//             // If execvp returns, it means an error occurred
//             cout << "Failed to execute command2." << endl;
//             exit(1);
//         } else {
//             // Parent process
//             close(pipefd[0]);
//             close(pipefd[1]);

//             // Wait for both child processes to finish
//             waitpid(pid, nullptr, 0);
//             waitpid(pid2, nullptr, 0);
//         }
//     }
// }

void simple_shell::exec_pipeline(char** argv1, char** argv2) {
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        cout << "Failed to create pipe." << endl;
        return;
    }

    // Fork a child process for the first command
    pid = fork();
    if (pid < 0) {
        cout << "Fork failed." << endl;
        return;
    }

    if (pid == 0) {
        // Child process (first command)
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execvp(argv1[0], argv1);

        // If execvp returns, it means an error occurred
        cout << "Failed to execute command1." << endl;
        exit(1);
    } else {
        // Parent process
        // Fork another child process for the second command
        pid_t pid2 = fork();
        if (pid2 < 0) {
            cout << "Fork failed." << endl;
            return;
        }

        if (pid2 == 0) {
            // Second child process (second command)
            // Close the write end of the pipe
            close(pipefd[1]);

            // Redirect stdin to the read end of the pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            execvp(argv2[0], argv2);

            // If execvp returns, it means an error occurred
            cout << "Failed to execute command2." << endl;
            exit(1);
        } else {
            // Parent process
            close(pipefd[0]);
            close(pipefd[1]);

            // Wait for both child processes to finish
            waitpid(pid, nullptr, 0);
            waitpid(pid2, nullptr, 0);
        }
    }
}


