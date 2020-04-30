#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

// <TASK 1>
char* substring(char str[], int pos, int length) {
    int k = 0;
    char* substr = (char *)calloc(512, sizeof(char));
    while (k < length) {
        substr[k] = str[pos + k - 1];
        k++;
    }
    substr[k] = '\0';
    return substr;
}

void list_content(char* path, int recursive, int size, char* name) {
    DIR* dir;
    struct dirent* entry;
    struct stat statbuf;
    char fullPath[512];

    dir = opendir(path);

    if (recursive == 0) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..")) {
                snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0) {
                    if (strcmp(name, "\0") == 0) {
                        if (size != -1) {
                            if (S_ISREG(statbuf.st_mode)) {
                                    if (statbuf.st_size < size) {
                                        printf("%s\n", fullPath);
                                    }
                                }
                        } else {
                            printf("%s\n", fullPath);
                        }
                    } else {
                        if (size != -1) {
                            if (strcmp(fullPath + strlen(fullPath) - strlen(name), name) == 0) {
                                if (S_ISREG(statbuf.st_mode)) {
                                        if (statbuf.st_size < size) {
                                            printf("%s\n", fullPath);
                                        }
                                    }
                                }
                        } else {
                            if (strcmp(fullPath + strlen(fullPath) - strlen(name), name) == 0) {
                                printf("%s\n", fullPath);
                            }
                        }
                    }
                }
            }
        } 
    } else {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..")) {
                snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0) {
                    if (strcmp(name, "\0") == 0) {
                        if (size != -1) {
                            if (S_ISREG(statbuf.st_mode)) {
                                if (statbuf.st_size < size) {
                                    printf("%s\n", fullPath);
                                }
                            }
                        } else {
                            printf("%s\n", fullPath);
                        }
                    } else {
                        if (size != -1) {
                            if (strcmp(fullPath + strlen(fullPath) - strlen(name), name) == 0) {
                                if (S_ISREG(statbuf.st_mode)) {
                                    if (statbuf.st_size < size) {
                                        printf("%s\n", fullPath);
                                    }
                                }
                            }
                        } else {
                            if (strcmp(fullPath + strlen(fullPath) - strlen(name), name) == 0) {
                                printf("%s\n", fullPath);
                            }
                        }
                    }
                    if (S_ISDIR(statbuf.st_mode)) {
                        list_content(fullPath, 1, size, name);
                    }
                }
            }
        }
    }
    closedir(dir);
}
// </TASK 1>

// <TASK 2>

typedef struct section {
    char name[15];
    short type;
    int offset;
    int size;
} SECTION;

void parseSF(char* path) {
    int fd = -1;
    char magic[4];
    short header_size = 0;
    short version = 0;
    char no_of_sections;
    SECTION* sections;

    fd = open(path, O_RDONLY);
    read(fd, &magic, 4);
    read(fd, &header_size, 2);
    read(fd, &version, 2);
    read(fd, &no_of_sections, 1);
    
    if ((strcmp(magic, "KKOB") == 0)) {
        if ((version >= 30) && (version <= 125)) {
            if (((int)no_of_sections >= 4) && ((int)no_of_sections <= 11)) {
                sections = (SECTION *)calloc(no_of_sections + 1, sizeof(SECTION));
                for (int i = 1; i <= no_of_sections; ++i) {
                    read(fd, &sections[i].name, 15);
                    read(fd, &sections[i].type, 2);
                    read(fd, &sections[i].offset, 4);
                    read(fd, &sections[i].size, 4);
                }
                for (int i = 1; i <= no_of_sections; ++i) {
                    if (sections[i].type == 62 || sections[i].type == 94 || sections[i].type == 90 || sections[i].type == 80){
                        continue;
                    } else {
                        printf("ERROR\nwrong sect_types\n");
                        free(sections);
                        return;
                    }
                }
                printf("SUCCESS\n");
                printf("version=%d\n", version);
                printf("nr_sections=%d\n", (int) no_of_sections);
                for (int i = 1; i <= no_of_sections; ++i) {
                    printf("section%d: %s %d %d\n", i, sections[i].name, sections[i].type, sections[i].size);
                }
            } else {
                printf("ERROR\nwrong sect_nr\n");
                return;
            }
        } else {
            printf("ERROR\nwrong version\n");
            return;
        }
    } else {
        printf("ERROR\nwrong magic\n");
        return;
    }

    free(sections);
    close(fd);
}
// </TASK 2>

// <TASK 3>
void extract(char* path, int section, int line) { 
    int fd = -1;
    char magic[4];
    short header_size = 0;
    short version = 0;
    char no_of_sections;
    int current_line = 0;
    int nrLines = 0;
    SECTION* sections;

    fd = open(path, O_RDONLY);
    read(fd, &magic, 4);
    read(fd, &header_size, 2);
    read(fd, &version, 2);
    read(fd, &no_of_sections, 1);
    
    if ((strcmp(magic, "KKOB") == 0)) {
        if ((version >= 30) && (version <= 125)) {
            if (((int)no_of_sections >= 4) && ((int)no_of_sections <= 11)) {
                sections = (SECTION *)calloc(no_of_sections + 1, sizeof(SECTION));
                for (int i = 1; i <= no_of_sections; ++i) {
                    read(fd, &sections[i].name, 15);
                    read(fd, &sections[i].type, 2);
                    read(fd, &sections[i].offset, 4);
                    read(fd, &sections[i].size, 4);
                }
                for (int i = 1; i <= no_of_sections; ++i) {
                    if (sections[i].type == 62 || sections[i].type == 94 || sections[i].type == 90 || sections[i].type == 80){
                        continue;
                    } else {
                        printf("ERROR\ninvalid file\n");
                        free(sections);
                        return;
                    }
                }
            } else {
                printf("ERROR\ninvalid file\n");
                return;
            }
        } else {
            printf("ERROR\ninvalid file\n");
            return;
        }
    } else {
        printf("ERROR\ninvalid file\n");
        return;
    }

    if (section <= no_of_sections) {
        lseek(fd, 0, SEEK_SET);
        lseek(fd, sections[section].offset, SEEK_SET);

        for (int i = 0; i < sections[section].size; ++i) {
            char c;
            read(fd, &c, 1);
            if (c == '\n') {
                ++nrLines;
            }
        } 
        if (line <= nrLines) {
        printf("SUCCESS\n");
        lseek(fd, sections[section].offset, SEEK_SET);
        for (int i = 0; i < sections[section].size; ++i) { 
            char c;
            read(fd, &c, 1);
            if (c == '\n') {
                current_line += 1;
            }
            if (current_line == nrLines - line + 1) {
                if (c != '\n') {
                    printf("%c", c);
                }
            } else if (current_line > nrLines - line + 1) {
                printf("\n");
                break;
            }
        }
        } else {
            printf("ERROR\ninvalid line\n");
        }
    } else {
        printf("ERROR\ninvalid section\n");
    }
    
    free(sections);
    close(fd);
}
// </TASK 3>

// <TASK 4> 

int is_a_valid_section_file(char* path) {
    int fd = -1;
    char magic[4];
    short header_size = 0;
    short version = 0;
    char no_of_sections;
    SECTION* sections;

    fd = open(path, O_RDONLY);
    read(fd, &magic, 4);
    read(fd, &header_size, 2);
    read(fd, &version, 2);
    read(fd, &no_of_sections, 1);

    if ((strcmp(magic, "KKOB") == 0)) {
        if ((version >= 30) && (version <= 125)) {
            if (((int)no_of_sections >= 4) && ((int)no_of_sections <= 11)) {
                sections = (SECTION *)calloc(no_of_sections + 1, sizeof(SECTION));
                for (int i = 1; i <= no_of_sections; ++i) {
                    read(fd, &sections[i].name, 15);
                    read(fd, &sections[i].type, 2);
                    read(fd, &sections[i].offset, 4);
                    read(fd, &sections[i].size, 4);
                }
                for (int i = 1; i <= no_of_sections; ++i) {
                    if (sections[i].type == 62 || sections[i].type == 94 || sections[i].type == 90 || sections[i].type == 80){
                        if (sections[i].size <= 1454) {
                            continue;
                        } else {
                            free(sections);
                            return 0;
                        }
                    } else {
                        free(sections);
                        return 0;
                    }
                }
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }
    free(sections);
    return 1;
}

void findall(char* path) {
    DIR* dir = NULL;
    struct dirent* entry;
    struct stat statbuf;
    char fullPath[512];

    dir = opendir(path);
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..")) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if (lstat(fullPath, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    if (is_a_valid_section_file(fullPath)) {
                        printf("%s\n", fullPath);
                    }
                }
                if (S_ISDIR(statbuf.st_mode)) {
                    findall(fullPath);
                }
            }
            
        }
    }
                    
    closedir(dir);
}
// </TASK 4>

int main(int argc, char **argv){

    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("21460\n");
        } else if (strcmp(argv[1], "list") == 0) {
            int recursive = 0;
            int size = -1;
            char* name = (char *)calloc(512, sizeof(char));
            char* path = (char *)calloc(512, sizeof(char));
            for (int i = 1; i < argc; ++i) {
                if (strcmp(argv[i], "recursive") == 0) {
                    recursive = 1;
                } else if (strstr(argv[i], "size_smaller=") != NULL) {
                    size = atoi(substring(argv[i], 14, strlen(argv[i])));
                } else if (strstr(argv[i], "name_ends_with=") != NULL) {
                    name = substring(argv[i], 16, strlen(argv[i]));
                } else if (strstr(argv[i], "path=") != NULL) {
                    path = substring(argv[i], 6, strlen(argv[i]));
                }
            }
            DIR* dir = opendir(path);
            if (dir != NULL) {
                printf("SUCCESS\n");
                list_content(path, recursive, size, name);
            } else {
                printf("ERROR\ninvalid directory path\n");
            }
            free(path);
            free(dir);
            free(name);
        } else if (strcmp(argv[1], "parse") == 0) {
            char* path = (char *)calloc(512, sizeof(char));
            for (int i = 1; i < argc; ++i) {
                if (strstr(argv[i], "path=") != NULL) {
                    path = substring(argv[i], 6, strlen(argv[i]));
                }
            }
            if (-1 == open(path, O_RDONLY)) {
                printf("ERROR\ninvalid file path\n");
            } else {
                parseSF(path);
            }
            free(path);
        } else if (strcmp(argv[1], "extract") == 0) {
            char* path = (char *)calloc(512, sizeof(char));
            int line = 0;
            int section = 0;
            for (int i = 1; i < argc; ++i) {
                if (strstr(argv[i], "path=") != NULL) {
                    path = substring(argv[i], 6, strlen(argv[i]));
                } else if (strstr(argv[i], "line=") != NULL) {
                    line = atoi(substring(argv[i], 6, strlen(argv[i])));
                } else if (strstr(argv[i], "section=") != NULL) {
                    section = atoi(substring(argv[i], 9, strlen(argv[i])));
                }
            }
            if (-1 == open(path, O_RDONLY)) {
                printf("ERROR\ninvalid file\n");
            } else {
                extract(path, section, line);
            }
            free(path);
        } else if (strcmp(argv[1], "findall") == 0) {
            char* path = (char *)calloc(512, sizeof(char));
            if (argc >= 3) {
                path = substring(argv[2], 6, strlen(argv[2]));
                if (NULL == opendir(path)) {
                    printf("ERROR\ninvalid directory path\n");
                } else {
                    printf("SUCCESS\n");
                    findall(path);
                }
            } else {
                printf("Not enough arguments - usage: ./a1 findall path=<path>\n");
            }
        }
    }
    return 0;
}