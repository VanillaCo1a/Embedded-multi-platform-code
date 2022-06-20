#ifndef __UART_H
#define __UART_H
#include "head.h"

extern int8_t numTransmitBuf1, numReceiveBuf1;
extern Queue_structure *strBuf1_Receive, *strBuf1_Transmit;
extern uint8_t tail1_inside[], tail1_outside[];

void UART_Init(void);

int8_t writeBuf1_Transmit(uint8_t *, uint8_t *, uint8_t *);
uint8_t *readBuf1_Receive(uint8_t *, uint8_t *);
int8_t UART1_Transmit(void);
void UART1_Receive(void);

#endif



//---------------------backpack---------------------//
//将通信串更改后缀后写入缓冲区, 等待发信函数发信
//int8_t writeBuf2_Transmit(uint8_t *record, uint8_t *suffix_out) {
//	int16_t i = 0, j = 0;
//
//	if(record != NULL) {
//		for(i=0; !isQueueFull(strBuf2_Transmit); i++) {
//			if(record[i] == tail2_inside[j]) {
//				j++;
//				if(j == sizeof(tail2_inside)/sizeof(uint8_t)) {
//					if(suffix_out!=NULL && suffix_out[0]) {
//						for(j=0; j<suffix_out[0]; j++) {
//							QueueIn(strBuf2_Transmit, suffix_out[j+1]);
//						}
//					}
//					break;
//				}
//			}else {
//				if(j != 0) {
//					i = i - j;
//					j = 0;
//				}
//				QueueIn(strBuf2_Transmit, record[i]);
//			}
//		}
//		#if DEBUG_UART
//		printf("\r\nstrBuf2_Transmit=%d %d\r\n", strBuf2_Transmit->head, strBuf2_Transmit->tail);
//		for(i=0; i<strBuf2_Transmit->maxnum; i++) {
//			printf(" %X", strBuf2_Transmit->element[i]);
//		}printf("\r\n\r\n\r\n");
//		#endif
//		return 1;
//	}else {
//		return 0;
//	}
//}
////根据设置的外部后缀从缓存区读出1个通信串, 并添加所设置的内部后缀, 返回其地址, 若读取失败则返回NULL
//	//!!!注意此函数会申请内存空间, 使用后务必进行空间的释放!!!
//uint8_t *readBuf2_Receive(uint8_t *suffix_out) {
//	static int8_t flag_receive = 0;
//	static int8_t delay_result = 0;
//	int16_t i = 0, j = 0, k = 0;
//	uint8_t *record = malloc(strBuf2_Receive->maxnum*sizeof(uint8_t));	//在堆空间中申请有限大小内存
//
//	if(isQueueEmpty(strBuf2_Receive) == 0) {	//检测缓冲区不为空后等待一段时间再读入, 权宜之计
//		flag_receive = 1;
//		{	static uint16_t flag_comparetime = 0;	static int8_t flag_reset = 0;
//			delay_result = Receive_delay(10);	//这里不等待的话, 队列读入时就会在队尾多读一个乱码值. 推测不能一检测缓冲区有数据就读入, 否则会因读取太快导致错乱
//		}
//	}
//	if((flag_receive==1) && delay_result) {
//		flag_receive = 0;
//		for(i=0; isQueueEmpty(strBuf2_Receive)==0; i++) {
//			if(k == 0) {
//				record[i] = (uint8_t)QueueOut(strBuf2_Receive);
//			}else{
//				k--;
//			}
//			if(record[i] == suffix_out[j+1]) {
//				j++;
//				if(j == suffix_out[0]) {
//					for(k=0; k<suffix_out[0]; k++) {
//						record[i-j+(k+1)] = 0;
//					}
//					for(k=0; k<sizeof(tail2_inside)/sizeof(uint8_t); k++) {
//						record[i-(j-1)+k] = tail2_inside[k];
//					}
//					break;
//				}
//			}else {
//				if(j != 0) {
//					i = i - j;
//					k = j;
//					j = 0;
//				}
//			}
//		}
//		#if DEBUG_UART
//		printf("\r\n\r\n\r\nstrBuf2_Receive=%d %d\r\n", strBuf2_Receive->head, strBuf2_Receive->tail);
//		for(i=0; i<strBuf2_Receive->maxnum; i++) {
//			printf("%X ", strBuf2_Receive->element[i]);
//		}printf("\r\n");
//		#endif
//		#if DEBUG_UART
//		printf("\r\n");
//		for(i=0; i<strBuf2_Receive->maxnum; i++) {
//			printf("%X|", record[i]);
//		}printf("\r\n");
//		#endif
//		return record;
//	}else {
//		free(record);
//		return NULL;
//	}
//}
