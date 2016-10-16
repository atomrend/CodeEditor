#include "MainWindow.h"
#include <QApplication>
#include <QStringList>

#ifndef INLINE
#include <execinfo.h>
#include <fstream>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

void
segfaultHandler(int, siginfo_t*, void*)
{
    void *stackPtrs[256];
    size_t stackCount = backtrace(stackPtrs, 256);
    char *homePath = getenv("HOME");
    char *filePath = (char*) malloc(strlen(homePath) + 14);
    std::ofstream file;
    char **stackNames;
    char indexStr[16];
    unsigned int index;

    strcpy(filePath, homePath);
    strcpy(filePath + strlen(homePath), "/.config/codedit_stack");
    file.open(filePath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        stackNames = backtrace_symbols(stackPtrs, stackCount);
        for (index = 2; index < stackCount - 2; ++index) {
            memset(indexStr, '\0', 16 * sizeof(char));
            sprintf(indexStr, "%i: ", index - 2);
            file << indexStr << stackNames[index] << '\n';
        }
        free(stackNames);
        file.close();
    }

    exit(EXIT_FAILURE);
}
#endif // INLINE

int
main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QString argument;
    QString preferencePath;
    QString directoryPath;
    QStringList files;
    int index = 1;

#ifndef INLINE
    struct sigaction sigact;
    sigact.sa_sigaction = segfaultHandler;
    sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sigact, (struct sigaction*) NULL);
#endif // INLINE

    while (index < argc) {
        argument = argv[index];
        if (argument.indexOf("--pref") == 0 || argument.indexOf("-p") == 0) {
            if (argument.indexOf("=") == -1) {
                if (index + 1 < argc) {
                    preferencePath = argv[index + 1];
                }
            } else {
                preferencePath = argument;
                preferencePath.remove(0, argument.indexOf("="));
            }
        } else  if (argument.indexOf("--dir") == 0 ||
                argument.indexOf("-d") == 0) {
            if (argument.indexOf("=") == -1) {
                if (index + 1 < argc) {
                    ++index;
                    directoryPath = argv[index];
                }
            } else {
                directoryPath = argument;
                directoryPath.remove(0, argument.indexOf("="));
            }
        } else {
            files << QString(argv[index]);
        }
        ++index;
    }

    app.setActiveWindow(new MainWindow(files));

    return app.exec();
}


