#include "buffer.h"

#include <cstring>
#include "log.h"

#define HUNK_SIZE (1024*8)

Buffer::Buffer(): _data(0),_pos(0),_length(0),_capacity(0) {
	// empty
}

Buffer::Buffer(int size): _data(0),_pos(0),_length(0),_capacity(0) {
	resize_uchar(size);
}

Buffer::~Buffer() {
	free();
}

void Buffer::resize_uchar(unsigned int size) {
	if (_capacity==0 || size>_capacity) {
		Log("resize for length %d", size);
		free();
		_capacity=(size+(HUNK_SIZE-1))/HUNK_SIZE*HUNK_SIZE;
		_data=new unsigned char[_capacity];
	}
	_length=size;
}

void Buffer::resize_uchar_keep(unsigned int size) {
	if (_capacity==0 || size>_capacity) {
		Log("---resize/keep for length %d", size);
		Buffer* copy=get_copy();
		resize_uchar(size);
		//copy_uchar(copy,0,this,0,size);
		Log("  copy.length=%d,copy.capacity=%d,ptr=%d",copy->_length,copy->_capacity,copy->_data);
		Log("  length=%d,capacity=%d,ptr=%d",_length,_capacity,_data);
		memcpy(_data,copy->_data,copy->_length);
		Log("---resize/keep done");
	}
	_length=size;
}

void Buffer::resize_ushort(unsigned int size) {
	resize_uchar(size*2);
}

void Buffer::resize_ushort_keep(unsigned int size) {
	resize_uchar_keep(size*2);
}

void Buffer::resize_uint(unsigned int size) {
	resize_uchar(size*4);
}

void Buffer::resize_uint_keep(unsigned int size) {
	resize_uchar_keep(size*4);
}

unsigned char* Buffer::uchar_array() {
	return (unsigned char*)_data;
}

unsigned short* Buffer::ushort_array() {
	return (unsigned short*)uchar_array();
}

unsigned int* Buffer::uint_array() {
	return (unsigned int*)uchar_array();
}

unsigned int Buffer::length() {
	return _length;
}

unsigned int Buffer::available() {
	return length()-position();
}

unsigned char Buffer::get_uchar() {
	return get_uchar(_pos++);
}
		
unsigned short Buffer::get_ushort() {
	unsigned short result=get_ushort(position());
	position(position()+2);
	return result;
}

unsigned int Buffer::get_uint() {
	unsigned int result=get_uint(position());
	position(position()+4);
	return result;
}

unsigned char Buffer::get_uchar(unsigned int offset) {
	return uchar_array()[offset];
}

unsigned short Buffer::get_ushort(unsigned int offset) {
	return *((unsigned short*)&uchar_array()[offset]);
}

unsigned int Buffer::get_uint(unsigned int offset) {
	return *((unsigned int*)&uchar_array()[offset]);
}

void Buffer::put_uchar(unsigned char value) {
	put_uchar(value,position());
	position(position()+1);
}

void Buffer::put_ushort(unsigned short value) {
	put_ushort(value,position());
	position(position()+2);
}

void Buffer::put_uint(unsigned int value) {
	put_uint(value,position());
	position(position()+4);
}

void Buffer::put_uchar(unsigned char value, unsigned int offset) {
	resize_uchar_keep(offset+1);
	uchar_array()[offset]=value;
}

void Buffer::put_ushort(unsigned short value, unsigned int offset) {
	resize_uchar_keep(offset+2);
	unsigned char* dest=&uchar_array()[offset];
	*((unsigned short*)dest)=value;
}

void Buffer::put_uint(unsigned int value, unsigned int offset) {
	resize_uchar_keep(offset+4);
	unsigned char* dest=&uchar_array()[offset];
	*((unsigned int*)dest)=value;
}

void Buffer::get(Buffer* buffer, unsigned int offset, unsigned int length) {
	Buffer::copy_uchar(buffer,offset,this,position(),length);
	position(position()+length);
}

void Buffer::get(Buffer* buffer) {
	get(buffer,0,buffer->length());
}

void Buffer::put(Buffer* buffer, unsigned int offset, unsigned int length) {
	Buffer::copy_uchar(this,position(),buffer,offset,length);
	buffer->position(buffer->position()+length);
}

void Buffer::put(Buffer* buffer) {
	put(buffer,0,buffer->length());
}

int Buffer::position() {
	return _pos;
}

void Buffer::position(unsigned int pos) {
	_pos=pos;
}

Buffer* Buffer::get_copy() {
	Log("Buffer:get_copy():start");
	Buffer* result=new Buffer(length());
	//Buffer::copy_uchar(this,0,result,0,length());
	memcpy(result->_data,_data,_length);
	Log("Buffer:get_copy():end");
	return result;
}

void Buffer::copy_uchar(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length) {
	source->resize_uchar_keep(source_offset+length);
	dest->resize_uchar_keep(dest_offset+length);
	// ... clipping ...
	Log("copy_uchar():before:source_offset=%d,dest_offset=%d,length=%d",source_offset,dest_offset,length);
	memcpy(dest->_data+dest_offset,source->_data+source_offset,length);
	Log("copy_uchar():after");
}

void Buffer::copy_ushort(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length) {
	copy_uchar(source,source_offset,dest,dest_offset,length*2);
}

void Buffer::copy_uint(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length) {
	copy_uchar(source,source_offset,dest,dest_offset,length*4);
}
	
void Buffer::fill_uchar(unsigned int offset, unsigned int length, unsigned char value) {
	Log("fill_uchar():before:offset=%d,length=%d",offset,length);
	memset(_data,value,length);
}

void Buffer::fill_ushort(unsigned int offset, unsigned int length, unsigned short value) {
	for (; length-->0; offset+=2)
		put_ushort(value,offset);
}

void Buffer::fill_uint(unsigned int offset, unsigned int length, unsigned short value) {
	for (; length-->0; offset+=4)
		put_uint(value,offset);
}

void Buffer::free() {
	if (_data) {
		delete[] _data;
		_data=0;
	}
	_pos=0;
	_length=0;
}