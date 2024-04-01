#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "fs.h"
#include <string.h>

void FileSysInit(void)
{
    DevCreateDisk();
    DevOpenDisk();

    char *pBuf = (char *)malloc(BLOCK_SIZE);
    memset(pBuf,0,BLOCK_SIZE);
    DevWriteBlock(FILESYS_INFO_BLOCK,pBuf);
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
    DevWriteBlock(INODELIST_BLOCK_FIRST,pBuf);
    DevWriteBlock(INODELIST_BLOCKS,pBuf);
    DevWriteBlock(5,pBuf);
    DevWriteBlock(6,pBuf);
    free(pBuf);
}

void SetInodeBytemap(int inodeno)
{
    char *pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    pBuf[inodeno]=1;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    free(pBuf);
}

void ResetInodeBytemap(int inodeno)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    pBuf[inodeno]=0;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    free(pBuf);
}

void SetBlockBytemap(int blkno)
{
   char* pBuf = (char*)malloc(BLOCK_SIZE);
   DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
   pBuf[blkno]=1;
   DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
   free(pBuf);
}

void ResetBlockBytemap(int blkno)
{
   char* pBuf = (char*)malloc(BLOCK_SIZE);
   DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
   pBuf[blkno]=0;
   DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
   free(pBuf);
}

void PutInode(int inodeno, Inode* pInode)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    int iBlock_index = inodeno/(BLOCK_SIZE/sizeof(Inode));
    int iBlock_num = inodeno%(BLOCK_SIZE/sizeof(Inode));

    switch(iBlock_index)
    {
        case 0:
            DevReadBlock(INODELIST_BLOCK_FIRST,pBuf);
            break;
        case 1:
            DevReadBlock(INODELIST_BLOCKS,pBuf);
            break;
        case 2:
            DevReadBlock(5,pBuf);
            break;
        case 3:
            DevReadBlock(6,pBuf);
            break;
    }
    memcpy((pBuf+iBlock_num*sizeof(Inode)),pInode,sizeof(Inode));
      switch(iBlock_index)
    {
        case 0:
            DevWriteBlock(INODELIST_BLOCK_FIRST,pBuf);
            break;
        case 1:
            DevWriteBlock(INODELIST_BLOCKS,pBuf);
            break;
        case 2:
            DevWriteBlock(5,pBuf);
            break;
        case 3:
            DevWriteBlock(6,pBuf);
            break;
    }
    free(pBuf);
}


void GetInode(int inodeno, Inode* pInode)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    int iBlock_index = inodeno/(BLOCK_SIZE/sizeof(Inode));
    int iBlock_num = inodeno%(BLOCK_SIZE/sizeof(Inode));

    switch(iBlock_index)
    {
        case 0:
            DevReadBlock(INODELIST_BLOCK_FIRST,pBuf);
            break;
        case 1:
            DevReadBlock(INODELIST_BLOCKS,pBuf);
            break;
        case 2:
            DevReadBlock(5,pBuf);
            break;
        case 3:
            DevReadBlock(6,pBuf);
            break;
    }
    memcpy(pInode,(pBuf+iBlock_num*sizeof(Inode)),sizeof(Inode));
    free(pBuf);
}

int GetFreeInodeNum(void)
{
    int freeInode_num=0;

    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM,pBuf);
    for(int i=0; i<BLOCK_SIZE; i++)
    {
        if(pBuf[i] == 0) {
            freeInode_num=i;
            break;
        }
    }
    free(pBuf);
    return freeInode_num;
}


int GetFreeBlockNum(void)
{
    int freeBlock_num=0;

    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM,pBuf);
    for(int i=0; i<BLOCK_SIZE; i++) {
        if(pBuf[i] == 0) {
            freeBlock_num=i;
            break;
        }
    }
    free(pBuf);
    return freeBlock_num;
}

void PutIndirectBlockEntry(int blkno, int index, int number)
{
    char *pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(blkno,pBuf);
    pBuf[index*sizeof(int)] = number;
    DevWriteBlock(blkno,pBuf);
    free(pBuf);
}

int GetIndirectBlockEntry(int blkno, int index)
{
   int index_num = 0;
   
   char *pBuf = (char*)malloc(BLOCK_SIZE);
   DevReadBlock(blkno,pBuf);
   index_num = pBuf[index*sizeof(int)];
   free(pBuf);
   return index_num;
}

void PutDirEntry(int blkno, int index, DirEntry* pEntry)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(blkno,pBuf);
    memcpy(pBuf+index*sizeof(DirEntry),pEntry,sizeof(DirEntry));
    DevWriteBlock(blkno,pBuf);
    free(pBuf);
}

int GetDirEntry(int blkno, int index, DirEntry* pEntry)
{
    int result=0;

    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(blkno,pBuf);
    memcpy(pEntry,pBuf+index*sizeof(DirEntry),sizeof(DirEntry));

    if(pBuf[index*sizeof(DirEntry)] != INVALID_ENTRY)
    {
        result = INVALID_ENTRY*-1;
    } 
    else result = INVALID_ENTRY;

    free(pBuf);
    return result;
}

void RemoveDirEntry(int blkno, int index)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(blkno,pBuf);
    memset(pBuf+(index*sizeof(DirEntry)),INVALID_ENTRY,sizeof(DirEntry));
    DevWriteBlock(blkno,pBuf);
    free(pBuf);
}

void RemoveIndirectBlockEntry(int blkno, int index)
{
    char* pBuf = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(blkno,pBuf);
    memset(pBuf+index*sizeof(int),INVALID_ENTRY,sizeof(int));
    DevWriteBlock(blkno,pBuf);
    free(pBuf);
}