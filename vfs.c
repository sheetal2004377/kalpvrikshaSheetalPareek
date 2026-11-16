#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_BLOCKS 1024
#define BLOCK_SIZE 512
#define MAX_NAME 50
#define MAX_FILE_BLOCKS 100

typedef enum {
    NODE_DIR,
    NODE_FILE
} NodeType;

typedef struct FreeBlock {
    int blockIndex;
    struct FreeBlock *prev;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME];
    NodeType type;
    struct FileNode *parent;
    struct FileNode *childHead;
    struct FileNode *next;
    struct FileNode *prev;
    int contentSize;
    int blockPointers[MAX_FILE_BLOCKS];
    int numBlocksAllocated;
} FileNode;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
int freeBlockCount = 0;
FileNode *root = NULL;
FileNode *cwd = NULL;

void initFreeList() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        FreeBlock *fb = (FreeBlock *)malloc(sizeof(FreeBlock));
        fb->blockIndex = i;
        fb->next = NULL;
        fb->prev = NULL;
        
        if (freeHead == NULL) {
            freeHead = fb;
            freeTail = fb;
        } else {
            freeTail->next = fb;
            fb->prev = freeTail;
            freeTail = fb;
        }
        freeBlockCount++;
    }
}

int allocateBlock() {
    if (freeHead == NULL) return -1;
    FreeBlock *fb = freeHead;
    int idx = fb->blockIndex;
    
    freeHead = freeHead->next;
    if (freeHead != NULL) {
        freeHead->prev = NULL;
    } else {
        freeTail = NULL;
    }
    free(fb);
    freeBlockCount--;
    return idx;
}

void returnBlock(int idx) {
    FreeBlock *fb = (FreeBlock *)malloc(sizeof(FreeBlock));
    fb->blockIndex = idx;
    fb->next = NULL;
    fb->prev = NULL;
    
    if (freeTail == NULL) {
        freeHead = fb;
        freeTail = fb;
    } else {
        freeTail->next = fb;
        fb->prev = freeTail;
        freeTail = fb;
    }
    freeBlockCount++;
}

void addChild(FileNode *dir, FileNode *newNode) {
    if (dir->childHead == NULL) {
        dir->childHead = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else {
        FileNode *tail = dir->childHead->prev;
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = dir->childHead;
        dir->childHead->prev = newNode;
    }
}

void removeChild(FileNode *dir, FileNode *node) {
    if (dir->childHead == node && node->next == node) {
        dir->childHead = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (dir->childHead == node) {
            dir->childHead = node->next;
        }
    }
}

FileNode* findChild(FileNode *dir, const char *name) {
    if (dir->childHead == NULL) return NULL;
    FileNode *curr = dir->childHead;
    do {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    } while (curr != dir->childHead);
    return NULL;
}

void cmd_mkdir(const char *name) {
    if (findChild(cwd, name) != NULL) {
        printf("Error: Directory or file '%s' already exists.\n", name);
        return;
    }
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    strcpy(newNode->name, name);
    newNode->type = NODE_DIR;
    newNode->parent = cwd;
    newNode->childHead = NULL;
    newNode->contentSize = 0;
    newNode->numBlocksAllocated = 0;

    addChild(cwd, newNode);
    printf("Directory '%s' created successfully.\n", name);
}

void cmd_create(const char *name) {
    if (findChild(cwd, name) != NULL) {
        printf("Error: Directory or file '%s' already exists.\n", name);
        return;
    }
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    strcpy(newNode->name, name);
    newNode->type = NODE_FILE;
    newNode->parent = cwd;
    newNode->childHead = NULL;
    newNode->contentSize = 0;
    newNode->numBlocksAllocated = 0;

    addChild(cwd, newNode);
    printf("File '%s' created successfully.\n", name);
}

void cmd_write(const char *filename, char *content) {
    FileNode *file = findChild(cwd, filename);
    if (file == NULL || file->type != NODE_FILE) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    int len = strlen(content);
    if ((content[0] == '"' && content[len-1] == '"') || (content[0] == '\'' && content[len-1] == '\'')) {
        content[len-1] = '\0';
        content++;
        len -= 2;
    }

    int requiredBlocks = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (requiredBlocks == 0) requiredBlocks = 1;

    if (requiredBlocks > freeBlockCount) {
        printf("Error: Disk is full. Cannot write file.\n");
        return;
    }

    file->contentSize = len;
    file->numBlocksAllocated = 0;

    int totalWritten = 0;
    for (int i = 0; i < requiredBlocks; i++) {
        int blk = allocateBlock();
        if (blk == -1) break;
        file->blockPointers[file->numBlocksAllocated++] = blk;

        int chunk = (len - totalWritten > BLOCK_SIZE) ? BLOCK_SIZE : (len - totalWritten);
        memcpy(virtualDisk[blk], content + totalWritten, chunk);
        totalWritten += chunk;
    }

    printf("Data written successfully (size=%d bytes).\n", file->contentSize);
}

void cmd_read(const char *filename) {
    FileNode *file = findChild(cwd, filename);
    if (file == NULL || file->type != NODE_FILE) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    if (file->contentSize == 0) {
        printf("(empty file)\n");
        return;
    }

    int totalRead = 0;
    for (int i = 0; i < file->numBlocksAllocated; i++) {
        int blk = file->blockPointers[i];
        int chunk = (file->contentSize - totalRead > BLOCK_SIZE) ? BLOCK_SIZE : (file->contentSize - totalRead);
        
        for (int j = 0; j < chunk; j++) {
            putchar(virtualDisk[blk][j]);
        }
        totalRead += chunk;
    }
    putchar('\n');
}

void cmd_delete(const char *filename) {
    FileNode *file = findChild(cwd, filename);
    if (file == NULL || file->type != NODE_FILE) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }

    for (int i = 0; i < file->numBlocksAllocated; i++) {
        returnBlock(file->blockPointers[i]);
    }

    removeChild(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void cmd_rmdir(const char *dirname) {
    FileNode *dir = findChild(cwd, dirname);
    if (dir == NULL || dir->type != NODE_DIR) {
        printf("Error: Directory '%s' not found.\n", dirname);
        return;
    }

    if (dir->childHead != NULL) {
        printf("Error: Directory is not empty.\n");
        return;
    }

    removeChild(cwd, dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void cmd_ls() {
    if (cwd->childHead == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *curr = cwd->childHead;
    do {
        printf("%s\n", curr->name);
        curr = curr->next;
    } while (curr != cwd->childHead);
}

void cmd_cd(const char *path) {
    if (strcmp(path, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
            printf("Moved to /\n");
        }
        return;
    }

    FileNode *target = findChild(cwd, path);
    if (target != NULL && target->type == NODE_DIR) {
        cwd = target;
        if (cwd == root)
            printf("Moved to /\n");
        else
            printf("Moved to /%s\n", cwd->name);
    } else {
        printf("Error: Directory '%s' not found.\n", path);
    }
}

void printPath(FileNode *node) {
    if (node->parent == NULL) {
        printf("/");
        return;
    }
    printPath(node->parent);
    if (strcmp(node->parent->name, "/") != 0)
        printf("/");
    printf("%s", node->name);
}

void cmd_pwd() {
    printPath(cwd);
    printf("\n");
}

void cmd_df() {
    int usedBlocks = NUM_BLOCKS - freeBlockCount;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlockCount);
    printf("Disk Usage: %.2f%%\n", ((float)usedBlocks / NUM_BLOCKS) * 100.0);
}

int main() {
    memset(virtualDisk, 0, sizeof(virtualDisk));
    initFreeList();

    root = (FileNode *)malloc(sizeof(FileNode));
    strcpy(root->name, "/");
    root->type = NODE_DIR;
    root->parent = NULL;
    root->childHead = NULL;
    root->next = root;
    root->prev = root;

    cwd = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char line[256];
    while (1) {
        if (cwd == root)
            printf("/ > ");
        else
            printf("%s > ", cwd->name);

        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char cmd[50], arg1[100], arg2[256] = "";
        int args = sscanf(line, "%s %s %[^\n]", cmd, arg1, arg2);

        if (strcmp(cmd, "exit") == 0) {
            printf("Memory released. Exiting program...\n");
            break;
        } else if (strcmp(cmd, "mkdir") == 0 && args >= 2) {
            cmd_mkdir(arg1);
        } else if (strcmp(cmd, "create") == 0 && args >= 2) {
            cmd_create(arg1);
        } else if (strcmp(cmd, "write") == 0 && args >= 3) {
            cmd_write(arg1, arg2);
        } else if (strcmp(cmd, "read") == 0 && args >= 2) {
            cmd_read(arg1);
        } else if (strcmp(cmd, "delete") == 0 && args >= 2) {
            cmd_delete(arg1);
        } else if (strcmp(cmd, "rmdir") == 0 && args >= 2) {
            cmd_rmdir(arg1);
        } else if (strcmp(cmd, "ls") == 0) {
            cmd_ls();
        } else if (strcmp(cmd, "cd") == 0 && args >= 2) {
            cmd_cd(arg1);
        } else if (strcmp(cmd, "pwd") == 0) {
            cmd_pwd();
        } else if (strcmp(cmd, "df") == 0) {
            cmd_df();
        } else {
            printf("Unknown command or missing arguments.\n");
        }
    }

    return 0;
}
