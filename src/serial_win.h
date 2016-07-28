


#ifndef SERIAL_WIN32_SVM
#define SERIAL_WIN32_SVM









int serial_begin(char *sComPort, unsigned int baudrate);

unsigned int serial_read(unsigned char *bufData, unsigned int len);

unsigned int serial_write(unsigned char *bufData, unsigned int len);

void serial_end(void);









#endif



