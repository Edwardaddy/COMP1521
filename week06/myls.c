// myls.c ... my very own "ls" implementation

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
# include <bsd/string.h>
#endif
#include <sysexits.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#define MAXDIRNAME 256
#define MAXFILEPATH 1024
#define MAXNAME 24

char *rwxmode (mode_t, char *);
char *username (uid_t, char *);
char *groupname (gid_t, char *);

struct FileListNode {
    char *cwd;
    char *rwxmdoe;
    char *username;
    char *groupname;
    long long size;
    char *filename;
    char *modi_time;
    struct FileListNode *next;
};
typedef struct FileListNode *FileList;

struct FileListRep {
    size_t nitems;   /**< count of items in list */
    FileList first;  /**< first node in list */
    FileList curr;   /**< current node in list */
    FileList last;   /**< last node in list */
};
typedef struct FileListRep *FileListRep;

FileListRep newFileList (void);
static FileList newFileListNode (char *cwd, char *rwxmode, char *username, char *groupname, long long size, char *filename, char *modi_time);
FileListRep sortFileList(FileListRep fl);

int main (int argc, char *argv[])
{
    FILE * fp;
    fp = fopen ("file_info.txt", "w"); assert(fp!=NULL && "fp must not be NULL");
    fp = fopen ("file_info.txt", "a"); assert(fp!=NULL && "fp must not be NULL");

    // string buffers for various names
    char uname[MAXNAME+1]; // UNCOMMENT this line
    char gname[MAXNAME+1]; // UNCOMMENT this line
    char mode[MAXNAME+1]; // UNCOMMENT this line

    // collect the directory name, with "." as default
    char dirname[MAXDIRNAME] = ".";
    if (argc >= 2)
        strlcpy (dirname, argv[1], MAXDIRNAME);

    // check that the name really is a directory
    struct stat info;
    if (stat (dirname, &info) < 0)
        err (EX_OSERR, "%s", dirname);

    if (! S_ISDIR (info.st_mode)) {
        errno = ENOTDIR;
        err (EX_DATAERR, "%s", dirname);
    }

    // open the directory to start reading
    // DIR *df; // UNCOMMENT this line
    // TODO
    FileListRep file_info = NULL;

    DIR *df; // UNCOMMENT this line
    df = opendir(dirname);
    if (df == NULL) {
        printf("Cannot open directory %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    // read directory entries
    struct dirent *entry;       // UNCOMMENT this line
    while((entry = readdir(df)) != NULL) {
        if(entry->d_name[0] == '.')continue;
        chdir(dirname);
        lstat(entry->d_name, &info);

        if(file_info == NULL)
        {
            file_info = newFileList();
            file_info->first = file_info->last = file_info->curr = newFileListNode(
                    getcwd(dirname,MAXFILEPATH),
                    rwxmode(info.st_mode, mode),
                    username(info.st_uid, uname),
                    groupname(info.st_gid, gname),
                    (long long)info.st_size,
                    entry->d_name,
                    strtok(ctime(&info.st_mtime), "\n")
                    );
            file_info->nitems++;
        }
        else
        {
            file_info->curr->next = newFileListNode(
                    getcwd(dirname,MAXFILEPATH),
                    rwxmode(info.st_mode, mode),
                    username(info.st_uid, uname),
                    groupname(info.st_gid, gname),
                    (long long)info.st_size,
                    entry->d_name,
                    strtok(ctime(&info.st_mtime), "\n")
                    );
            file_info->nitems++;
            file_info->curr = file_info->curr->next;
            file_info->last = file_info->curr;
        }

//        printf("%s  %s  %-8.8s %-8.8s %8lld %s\n",
//               //getcwd(dirname,MAXFILEPATH),
//               strtok(ctime(&info.st_mtime), "\n"),
//               rwxmode(info.st_mode, mode),
//               username(info.st_uid, uname),
//               groupname(info.st_gid, gname),
//               (long long)info.st_size,
//               entry->d_name
//               );
    }

    sortFileList(file_info);
    file_info->curr = file_info->first;
    while (file_info->curr != NULL)
    {
        printf("%s  %s  %-8.8s %-8.8s %8lld %s\n",
                //getcwd(dirname,MAXFILEPATH),
               file_info->curr->modi_time,
               file_info->curr->rwxmdoe,
               file_info->curr->username,
               file_info->curr->groupname,
               file_info->curr->size,
               file_info->curr->filename
        );
        file_info->curr = file_info->curr->next;
    }

    // finish up
    closedir(df); // UNCOMMENT this line

    return EXIT_SUCCESS;
}


// convert octal mode to -rwxrwxrwx string
char *rwxmode (mode_t mode, char *str)
{
    // Initiate str with '-'
    for(int i = 0; i < 10; i++) str[i]= '-';

    if ( S_ISDIR(mode) ) str[0] = 'd';    // directory
    else if ( S_ISLNK(mode) ) str[0] = 'l';    //symbolic link

    // USER rights
    if ( mode & S_IRUSR ) str[1] = 'r';
    if ( mode & S_IWUSR ) str[2] = 'w';
    if ( mode & S_IXUSR ) str[3] = 'x';

    // GROUP rights
    if ( mode & S_IRGRP ) str[4] = 'r';
    if ( mode & S_IWGRP ) str[5] = 'w';
    if ( mode & S_IXGRP ) str[6] = 'x';

    // OTHER GROUP rights
    if ( mode & S_IROTH ) str[7] = 'r';
    if ( mode & S_IWOTH ) str[8] = 'w';
    if ( mode & S_IXOTH ) str[9] = 'x';

    str[10] = '\0';
    return str;
}

// convert user id to user name
char *username (uid_t uid, char *name)
{
    struct passwd *uinfo = getpwuid (uid);

    if (uinfo != NULL) snprintf (name, MAXNAME, "%s", uinfo->pw_name);
    else snprintf (name, MAXNAME, "%d?", (int) uid);

    return name;
}

// convert group id to group name
char *groupname (gid_t gid, char *name)
{
    struct group *ginfo = getgrgid (gid);

    if (ginfo != NULL) snprintf (name, MAXNAME, "%s", ginfo->gr_name);
    else snprintf (name, MAXNAME, "%d?", (int) gid);

    return name;
}

FileListRep newFileList (void)
{
    FileListRep new = malloc (sizeof *new);
    if (new == NULL) err (EX_OSERR, "couldn't allocate FileListRep");
    new->curr = new->first = new->last = NULL;
    new->nitems = 0;
    return new;
}

static FileList newFileListNode (char *cwd, char *rwxmode, char *username, char *groupname, long long size, char *filename, char* modi_time)
{
    FileList new = malloc (sizeof *new);
    if (new == NULL) err (EX_OSERR, "couldn't allocate newFileListNode");
    new->size = size;
    new->groupname = groupname;
    new->cwd = cwd;
    new->rwxmdoe = rwxmode;
    new->username = username;
    new->filename = filename;
    new->modi_time = modi_time;

    new->next = NULL;
    return new;
}

/** Sort FileLis alphabeticallyt */
FileListRep sortFileList(FileListRep fl)
{
    fl->curr = fl->first;
    for (int i = 0; i < fl->nitems; i++)    // bubble sort
    {
        for (int j = i+1; j < fl->nitems; j++)
        {
            if(strcmp(fl->curr->filename,fl->curr->next->filename)>0){
                char* str;
                long long temp;
                str = fl->curr->filename;
                fl->curr->filename = fl->curr->next->filename;
                fl->curr->next->filename = str;

                str = fl->curr->modi_time;
                fl->curr->modi_time = fl->curr->next->modi_time;
                fl->curr->next->modi_time = str;

                str = fl->curr->username;
                fl->curr->username = fl->curr->next->username;
                fl->curr->next->username = str;

                str = fl->curr->rwxmdoe;
                fl->curr->rwxmdoe = fl->curr->next->rwxmdoe;
                fl->curr->next->rwxmdoe = str;

                str = fl->curr->cwd;
                fl->curr->cwd = fl->curr->next->cwd;
                fl->curr->next->cwd = str;

                str = fl->curr->groupname;
                fl->curr->groupname = fl->curr->next->groupname;
                fl->curr->next->groupname = str;

                temp = fl->curr->size;
                fl->curr->size = fl->curr->next->size;
                fl->curr->next->size = temp;
            }
            fl->curr = fl->curr->next;
        }
        fl->curr = fl->first;
    }
    return fl;
}
