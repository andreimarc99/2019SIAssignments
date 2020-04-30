#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>

#define RESP_PIPE "RESP_PIPE_21460"
#define REQ_PIPE "REQ_PIPE_21460"
#define SHM_KEY 15654

int fd1 = -1;
int fd2 = -1;
int shm_id, fd, file_size;
char* data;
char success_message[] = {0x07, 'S', 'U', 'C', 'C', 'E', 'S', 'S'};
char error_message[] = {0x05, 'E', 'R', 'R', 'O', 'R'};

int main(void)
{

    
    if (mkfifo(RESP_PIPE, 0600) != 0) {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe\n"); 
    }
    fd2 = open(REQ_PIPE, O_RDONLY);
    if (fd2 == -1) {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe\n");
    }

    fd1 = open(RESP_PIPE, O_WRONLY);
    if (fd1 == -1) {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe\n");
    }

    char s[] = {0x07, 0x43, 0x4F, 0x4E, 0x4E, 0x45, 0x43, 0x54};
    write(fd1, &s, sizeof(s));
    //printf("SUCCESS\n");

    for (;;) {
        char aux = '\0';
        read(fd2, &aux, 1);
        while ((int)aux > 31) {
            read(fd2, &aux, 1);
        }
        int auxInt = aux;
        char buf[auxInt];
        read(fd2, &buf, auxInt);
        buf[auxInt] = '\0';
        //printf("----- %s -----\n", buf);
        if (strcmp(buf, "PING") == 0) {
            char ping[] = {0x04, 0x50, 0x49, 0x4E, 0x47};
            char pong[] = {0x04, 0x50, 0x4F, 0x4E, 0x47};
            unsigned int variant = 21460;
            write(fd1, &ping, sizeof(ping));
            write(fd1, &pong, sizeof(pong));
            write(fd1, &variant, sizeof(unsigned int));
        }
        if (strcmp(buf, "CREATE_SHM") == 0) {
            char create_shm_message[] = {0x0A, 0x43, 0x52, 0x45, 0x41, 0x54, 0x45, 0x5f, 0x53, 0x48, 0x4d};
            char *ptr_shared = NULL;
            shm_id = shmget(SHM_KEY, 4733053, IPC_CREAT | 0644);
            if (shm_id < 0) {
                write(fd1, create_shm_message, sizeof(create_shm_message));
                write(fd1, error_message, sizeof(error_message));
            } else {

                ptr_shared = (char *)shmat(shm_id, NULL, 0);
                if (ptr_shared == (void *)-1) {
                    write(fd1, create_shm_message, sizeof(create_shm_message));
                    write(fd1, error_message, sizeof(error_message));
                } else {
                    write(fd1, create_shm_message, sizeof(create_shm_message));
                    write(fd1, success_message, sizeof(success_message));
                }
            }
        }
        if (strcmp(buf, "WRITE_TO_SHM") == 0) {
            char write_to_shm_message[] = {0x0C, 'W', 'R',
                'I', 'T', 'E', '_', 'T', 'O', '_', 'S', 'H', 'M'};
            unsigned int offset = 0;
            unsigned int value = 0;
            char *ptr_shared_aux = NULL;

            read(fd2, &offset, sizeof(unsigned int));
            read(fd2, &value, sizeof(unsigned int));

            //printf("----%d----%d----\n", offset, value);
            if (offset > 0 && offset < 4733053 - sizeof(value)) {
                ptr_shared_aux = (char *)shmat(shm_id, NULL, 0);
                if (ptr_shared_aux == (void *)-1) {
                    write(fd1, write_to_shm_message, sizeof(write_to_shm_message));
                    write(fd1, error_message, sizeof(error_message));
                } else {
                    memcpy(ptr_shared_aux + offset, &value, sizeof(value));
                    write(fd1, write_to_shm_message, sizeof(write_to_shm_message));
                    write(fd1, success_message, sizeof(success_message));
                }
            } else {
                write(fd1, write_to_shm_message, sizeof(write_to_shm_message));
                write(fd1, error_message, sizeof(error_message));
            }
        }
        if (strcmp(buf, "MAP_FILE") == 0) {
            char map_file_message[] = {0x08, 'M', 'A',
                'P', '_', 'F', 'I', 'L', 'E'};
            unsigned int size = 0;
            fd = 0; file_size=0;
            char *data = NULL;

            read(fd2, &size, sizeof(char));
            char file_name[(int)size];
            read(fd2, &file_name,(int)size);
            file_name[(int)size]='\0';

            fd = open(file_name, O_RDWR);
            if (fd == -1) {
                write(fd1, map_file_message, sizeof(map_file_message));
                write(fd1, error_message, sizeof(error_message));
            } else {
                file_size = lseek(fd, 0, SEEK_END);
                lseek(fd, 0, SEEK_SET);
                data = (char *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

                if (data == (void *) -1) {
                    write(fd1, map_file_message, sizeof(map_file_message));
                    write(fd1, error_message, sizeof(error_message));
                } else {
                    write(fd1, map_file_message, sizeof(map_file_message));
                    write(fd1, success_message, sizeof(success_message));
                }
            }
        }
        if (strcmp(buf, "READ_FROM_FILE_OFFSET") == 0) {
            char read_from_file_offset_message[] = {0x15, 'R', 'E', 'A', 'D', '_', 'F',
                'R', 'O', 'M', '_', 'F', 'I', 'L', 'E', '_', 'O', 'F', 'F', 'S',
                'E', 'T'};
            unsigned int offset = 0;
            unsigned int no_of_bytes = 0;
            char* ptr_shared = NULL;
            
            read(fd2, &offset, sizeof(unsigned int));
            read(fd2, &no_of_bytes, sizeof(unsigned int));
            if (data == (void *) -1) {
                write(fd1, read_from_file_offset_message, sizeof(read_from_file_offset_message));
                write(fd1, error_message, sizeof(error_message));
            }
            if (offset <= 0 || offset + no_of_bytes > file_size){
                write(fd1, read_from_file_offset_message, sizeof(read_from_file_offset_message));
                write(fd1, error_message, sizeof(error_message));
            }
            int j = 0;
            //memcpy(shared_char + offset, &value, no_of_bytes);
            for (int i = 0; i < no_of_bytes; ++i) {
                *(ptr_shared + (j++)) = *(data + offset + i);
            }
            write(fd1, read_from_file_offset_message, sizeof(read_from_file_offset_message));
            write(fd1, success_message, sizeof(success_message));
        }
        if (strcmp(buf, "READ_FROM_FILE_SECTION") == 0) {
            /*unsigned int section_no = 0;
            unsigned int offset = 0;
            unsigned int no_of_bytes = 0;
            char read_from_file_section_message[] = {0x15, 'R', 'E', 'A', 'D',
            'F', 'R', 'O', 'M', '_', 'F', 'I', 'L', 'E', '_',
            'S', 'E', 'C', 'T', 'I', 'O', 'N'};
            read(fd2, &section_no, sizeof(unsigned int));
            read(fd2, &offset, sizeof(unsigned int));
            read(fd2, &no_of_bytes, sizeof(unsigned int));

            if (section_no < 0 || offset < 0 || no_of_bytes < 0) {
                write(fd1, read_from_file_section_message, sizeof(read_from_file_section_message));
                write(fd1, error_message, sizeof(error_message));
            }*/
            return 0;
        }
        if (strcmp(buf, "READ_FROM_LOGICAL_SPACE_OFFSET") == 0) {
                return 0;
        }
        if (strcmp(buf, "EXIT") == 0) {
		free(data);
                close(fd1);
                close(fd2);
                unlink(REQ_PIPE);
                unlink(RESP_PIPE);
                return 0;
        }
    }
}
