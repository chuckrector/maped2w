#ifndef BUFFER_INC
#define BUFFER_INC

class Buffer {
	unsigned char* _data;
	unsigned int _pos;
	unsigned int _length;
	unsigned int _capacity;
public:
	Buffer();
	Buffer(int size);
	~Buffer();

	void resize_uchar(unsigned int size);
	void resize_uchar_keep(unsigned int size);
	void resize_ushort(unsigned int size);
	void resize_ushort_keep(unsigned int size);
	void resize_uint(unsigned int size);
	void resize_uint_keep(unsigned int size);

	unsigned char* uchar_array();
	unsigned short* ushort_array();
	unsigned int* uint_array();
	
	unsigned int length();
	unsigned int available();

	unsigned char get_uchar();
	unsigned short get_ushort();
	unsigned int get_uint();
	unsigned char get_uchar(unsigned int offset);
	unsigned short get_ushort(unsigned int offset);
	unsigned int get_uint(unsigned int offset);

	void put_uchar(unsigned char value);
	void put_ushort(unsigned short value);
	void put_uint(unsigned int value);
	void put_uchar(unsigned char value, unsigned int offset);
	void put_ushort(unsigned short value, unsigned int offset);
	void put_uint(unsigned int value, unsigned int offset);

	void get(Buffer* buffer, unsigned int offset, unsigned int length);
	void get(Buffer* buffer);
	void put(Buffer* buffer, unsigned int offset, unsigned int length);
	void put(Buffer* buffer);

	int position();
	void position(unsigned int pos);

	Buffer* get_copy();
	static void copy_uchar(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length);
	static void copy_ushort(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length);
	static void copy_uint(Buffer* source, unsigned int source_offset, Buffer* dest, unsigned int dest_offset, unsigned int length);
	
	void fill_uchar(unsigned int offset, unsigned int length, unsigned char value);
	void fill_ushort(unsigned int offset, unsigned int length, unsigned short value);
	void fill_uint(unsigned int offset, unsigned int length, unsigned short value);

	void free();
};

#endif // BUFFER_INC