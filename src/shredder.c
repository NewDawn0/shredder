#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

// sign
char AUTHOR[] = "Author: NewDawn0";
char RELEASE_DATE[] = "Release Date: 03.01.2023";

//define
#define RESET "\x1b[0m"
#define BRED "\x1b[1;31m"
#define RED "\x1b[;31m"
#define YELLOW "\x1b[0;33m"
#define GREEN "\x1b[0;32m"
#define BLUE "\x1b[0;34m"
#define CYAN "\x1b[0;36m"
#define BCYAN "\x1b[1;36m"

// Typedefs
typedef unsigned int uint;

// globals
struct Globals {
    bool confirm;
    bool doConfirm;
    bool debug;
    struct Commands {
        char override[14];
        char shred[20];
    } commands;
};
// create global value to store globals
struct Globals globals = {
    .confirm = false,
    .doConfirm = true,
    .debug = false,
    .commands.override = "shred -v -n 1",
    .commands.shred = "shred -v -n 0 -z -u",
};
enum CommandType {File, Dir};

// fn def
bool confirm();
void *del(void *file);
void help();
int isFile(const char *file);
static void ls(const char *dir, enum CommandType cmdType);
static inline void shred(const char *path, const char *beginCmd);

// main
int main(int argc, char **argv) {
    // parse args //
    // prerun
    uint numOfFiles = 0;
    for (uint i=1; i<argc; i++) {
        if (strcmp(argv[i], "--no-confirm") == 0) {
            globals.doConfirm = false;
            globals.confirm = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            help(); return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            globals.debug = true;
        } else {
            // check if file exists
            if (access(argv[i], F_OK) != 0) {
                fprintf(stderr, "%sError%s File \'%s%s%s\' does not exist\n", BRED, RESET, YELLOW, argv[i], RESET);
                return 1;
            }
            numOfFiles++;
        }
    }
    // thread init stuff
    pthread_t threads[numOfFiles];
    int rc;
    int id = 1;

    // do arg stuff
    if (globals.debug) printf("%sDEBUG%s Looping over arguments\n", YELLOW, RESET);
    for (uint i=1; i<argc; i++) {
        // ignore if an arg
        if (strcmp(argv[i], "--no-confirm") == 0 || 
                strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || 
                strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
        } else {
            if (globals.doConfirm) {
                if (globals.debug) printf("%sDEBUG%s Calling the confirm function\n", YELLOW, RESET);
                if (!globals.confirm) globals.confirm = confirm();
            }
            if (globals.confirm) {
                rc = pthread_create(&threads[id], NULL, del, (void*)argv[i]);
                if (rc) {
                    fprintf(stderr, "%sError%s Unable to create deletion thread for \'%s%s%s\'", BRED, RESET, YELLOW, argv[i], RESET);
                }
                id++;
            } else {
                printf("Confirmation negated, exiting...\n");
                return 0;
            }
        }
    }
    pthread_exit(NULL);
    return 0;
}

// confirmation question
bool confirm() {
    char opt;
    printf("Do you really want to delete these files? %sY%s/%sN%s\n", GREEN, RESET, RED, RESET);
    printf("%sWarning%s this action can not be undone\n", YELLOW, RESET);
    opt = getchar();
    if (opt == 'Y' || opt == 'y') return true;
    return false;
}

// help fn
void help() {
    printf("%sshredder %s- Fast threaded secure file & folder deletion%s\n", BCYAN, CYAN, RESET);
    printf("%sshredder%s <%sfiles%s/%soptions%s>\n\t", CYAN, RESET, BLUE, RESET, BLUE, RESET);
    printf("%s-h%s\t\t Shows this menu\n\t", BLUE, RESET);
    printf("%s--help%s\t\t Shows this menu\n\t", BLUE, RESET);
    printf("%s-d%s\t\t Prints debug log\n\t", BLUE, RESET);
    printf("%s--debug%s\t\t Prints debug log\n\t", BLUE, RESET);
    printf("%s--no-confirm%s\t Skips the deletion confirmation\n", BLUE, RESET);
}

// delete file
void *del(void *file) {
    const char *f = (const char *)file;
    if (globals.debug) printf("%sDEBUG%s Deletion thread for file \'%s%s%s\' activated\n", YELLOW, RESET, YELLOW, f, RESET);
    if (isFile(f)) {
        ls(f, File);
        ls(f, Dir);
        rmdir(f);
    } else {
        shred(f, globals.commands.override);
        shred(f, globals.commands.shred);
    }
    if (globals.debug) printf("%sDEBUG%s Deletion thread for file \'%s%s%s\' finished\n", YELLOW, RESET, YELLOW, f, RESET);
    return NULL;
}

// list dirs
static void ls(const char *dir, enum CommandType cmdType) {
    DIR * d;
    d = opendir (dir);
    if (! d) {
        fprintf (stderr, "Cannot open directory '%s': %s\n", dir, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        const char * d_name;
        entry = readdir (d);
        if (! entry) {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        d_name = entry->d_name;
        // Print the name of the file and directory
        if (entry->d_type & DT_DIR) {
            if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
                path_length = snprintf (path, PATH_MAX, "%s/%s", dir, d_name);
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                // Recursively call "ls" with the new path
                ls(path, cmdType);
            }
        }
        if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
            char *fpath;
            asprintf(&fpath, "%s/%s", dir, d_name);
            if (entry->d_type & DT_DIR && cmdType == Dir) {
                rmdir(fpath);
                sync();
            } else if (!(entry->d_type & DT_DIR) && cmdType == File){
                shred(fpath, globals.commands.override);
                shred(fpath, globals.commands.shred);
            }
        }
    }
    // After going through all the entries, close the directory
    if (closedir(d)) {
        fprintf (stderr, "Could not close '%s': %s\n", dir, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

// returns true if a file, false if a dir
int isFile(const char *file) {
    struct stat statbuf;
    if (stat(file, &statbuf) != 0) {
        return true;
    }
    return S_ISDIR(statbuf.st_mode);
}

// delete file
static inline void shred(const char *path, const char *beginCmd) {
    char *cmd;
    if (globals.debug) {
        asprintf(&cmd, "%s %s", beginCmd, path);
    } else {
        asprintf(&cmd, "%s %s > /dev/null 2>/dev/null", beginCmd, path);
    }
    if (globals.debug) printf("%sDEBUG%s deleting \'%s%s%s\'\n", YELLOW, RESET, YELLOW, path, RESET);
    system(cmd);
    free(cmd);
    sync();
}
