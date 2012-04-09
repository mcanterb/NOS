#define null (void*)0

void itoa(unsigned int i, unsigned char* buffer);
void ltoa(unsigned long l, unsigned char* buffer);
void itoh(unsigned int i, unsigned char* buffer);
void memset(unsigned char* dest, unsigned char ch, unsigned int size);
void memcpy(void *s1, const void *s2, unsigned int n);