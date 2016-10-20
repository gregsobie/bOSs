#include "filesystem.h"
#include "lib.h"
#include "types.h"
#include "idt.h"

int32_t file_read (struct file * fp, char * outbuff, uint32_t bytes){
	uint32_t read = read_data(fp->f_inode,fp->f_pos,(uint8_t *)outbuff,bytes);
	if(read > 0)
		fp->f_pos += read;
	return read;
}
int32_t file_write (struct file * fp, const char * inbuff, uint32_t bytes){
	//XI. Thou shalt not write to a read-only file system.
	return -1;
}
int32_t file_open (struct file * fp){
	//Nothing to see here. Move along.
	return 0;
}
int32_t file_close (struct file * fp){
	return 0;
}

int32_t dir_read (struct file * fp, char * outbuff, uint32_t bytes){
	if(bytes>32)
		bytes = 32;
	if(fp->f_pos/64 > fs_base->num_entries)
		return 0;
	strncpy(outbuff,((char *)fs_base) + fp->f_pos,bytes);
	fp->f_pos += 64;
	return bytes;
}
int32_t dir_write (struct file * fp, const char * inbuff, uint32_t bytes){
	return -1;
}
int32_t dir_open (struct file * fp){
	fp->f_pos = 64;
	return 0;
}
int32_t dir_close (struct file * fp){
	return 0;
}

boot_block_t * fs_base = 0x0;
boot_block_t * fs_end = 0x0;
int32_t test_read(const uint8_t* filename, const void* buf, int32_t nbytes){
	dentry_t dentry;
	int ret = 0;
	ret = read_dentry_by_name (filename, &dentry);
	struct file f = {
		.f_pos = 0,
		.flags = 0 // Unused for now
	};
	f.f_inode = dentry.inode;
	if(ret != 0)
		return ret;
	printf("File type: %d\n",dentry.type);
	if(dentry.type == FILE_RTC_TYPE){
		return -1;
	}else if(dentry.type == FILE_DIR_TYPE){
		f.f_op = &dir_ops;
	}else if(dentry.type == FILE_DAT_TYPE){
		f.f_op = &file_ops;
	}
	printf("File name: %s \n",(char *)filename);
	ret = f.f_op->open(&f);
	if(ret != 0) return ret;

	ret = f.f_op->read(&f,(char *)buf,nbytes);

	return ret;
}

void show_fs_info(){
	
	printf("%d File Entries\n",fs_base->num_entries);
	printf("%d inodes\n",fs_base->num_inodes);
	printf("%d data blocks\n", (int)(fs_end-fs_base) - fs_base->num_inodes -1);
	int i,i2;
	for(i = 0;i< fs_base->num_entries;i++){
		char name[33] = {0};
		strncpy(name, fs_base->dentries[i].name, 32);
		for(i2 = 0;i2<32;i2++){
			if(name[i2] == '\0')
				name[i2] = ' ';
		}
		uint32_t type = fs_base->dentries[i].type;
		uint32_t inode = fs_base->dentries[i].inode;
		printf("%s type: #%u inode: #%u\n",name,type,inode);

	}

}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
	int i;
	uint32_t fname_len = strlen((char *)fname);
	if(fname_len == FILE_NAME_LEN +1 && fname[FILE_NAME_LEN] == '\0')
		fname_len--;
	if(fname_len > FILE_NAME_LEN) return -1;
	for(i=0;i<fs_base->num_entries;i++){
		dentry_int_t * d = &(fs_base->dentries[i]);
		printf("%s\n",d->name);
		if(!strncmp((char *)fname,d->name,fname_len)){
			strncpy(dentry->name, d->name, FILE_NAME_LEN);
			dentry->name[FILE_NAME_LEN] = '\0';
			dentry->type = d->type;
			dentry->inode = d->inode;
			return 0;
		}
	}
	return -1;
}
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	if(index < 0 || index >= fs_base->num_entries) return -1;
	dentry_int_t * d = &(fs_base->dentries[index]);
	strncpy(dentry->name, d->name, FILE_NAME_LEN+1);
	dentry->name[FILE_NAME_LEN+1] = '\0';
	dentry->type = d->type;
	dentry->inode = d->inode;
	return 0;
}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	if(inode < 0 || inode >= fs_base->num_inodes) return -1;
	inode_t * node = (inode_t *)(&fs_base[inode+1]);
	uint32_t f_pos = offset;
	uint32_t b_pos = f_pos % BLOCK_BYTES;
	uint32_t b_num = f_pos / BLOCK_BYTES;
	char * addr = (char *)(&fs_base[1 + fs_base->num_inodes + node->data_blocks[b_num]]);
	addr += b_pos;
	int read;
	for(read = 0;read<length;read++){
		if(f_pos >= node->length_bytes)
			break;
		if(b_pos >= BLOCK_BYTES){
			b_pos = 0;
			b_num++;
			addr = (char *)(&fs_base[1 + fs_base->num_inodes + node->data_blocks[b_num]]);
			if(addr >= (char *)fs_end)
				return -1;
		}
		buf[read] = *addr;
		b_pos++;
		addr++;
		f_pos++;
	}
	return read;
}
