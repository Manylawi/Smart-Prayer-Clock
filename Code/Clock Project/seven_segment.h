#ifndef SEVEN_SEGMENT_H_
#define SEVEN_SEGMENT_H_

void seven_seg_vinit(unsigned char port_name);
void seven_seg_write(unsigned char portname,unsigned char number);
void seven_seg_vinitT(void);
void seven_seg_writeT(unsigned char number);

#endif /* SEVEN SEGMENT_H_ */