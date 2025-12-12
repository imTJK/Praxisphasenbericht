/*
 * spi_task.h
 *
 *  Created on: 04.12.2019
 *      Author: tw
 */

#ifndef APP_INC_SPI_TASK_H_
#define APP_INC_SPI_TASK_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define SPI_TASK_STACK_SIZE   512 //configMINIMAL_STACK_SIZE
#define SPI_QUEUE_SIZE        60//50//30//16//8

// Queue Data
typedef void (*Fp_SPI_Queue_Request)(void * Handle);//Function Pointer used in SPI Task Queue as Request Message
//typedef void (*Fp_SPI_Queue_Callback)(void* Handle);//Function Pointer used in SPI Task Queue as Callback Message

typedef struct SPI_Queue_Data
{
  Fp_SPI_Queue_Request SPI_Request_Fp;
//  Fp_SPI_Queue_Callback SPI_Callback_Fp;
  void* Handle;
//  uint32_t data;
} SPI_Queue_Data_t;



// Functions
void SPI_Task_Init(void);

void SPI_Task_Send_Request(SPI_Queue_Data_t queue_data);
void SPI_QueueRequest(Fp_SPI_Queue_Request requestFunction, void *pData);
void SPI_Task_Send_Request_fromISR(SPI_Queue_Data_t queue_data);
void SPI_QueueRequest_fromISR(Fp_SPI_Queue_Request requestFunction, void *pData);

void SPI2_Task_Send_Request(SPI_Queue_Data_t queue_data);
void SPI2_QueueRequest(Fp_SPI_Queue_Request requestFunction, void *pData);
void SPI2_Task_Send_Request_fromISR(SPI_Queue_Data_t queue_data);
void SPI2_QueueRequest_fromISR(Fp_SPI_Queue_Request requestFunction, void *pData);



#endif /* APP_INC_SPI_TASK_H_ */
