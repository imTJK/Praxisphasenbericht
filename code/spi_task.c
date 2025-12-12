/*
 * spi_task.c
 *
 * File includes the SPI Task which depends on Hardware SPI Peripheral in Core Folder
 * The SPI Task and the Queue are managing the access to the SPI Hardware.
 * Putting SPI Data send and receive requests to a queue and a task enables the other tasks
 * to access the SPI Bus without to block on it and handle the hardware resource to different
 * modules.
 *
 *  Created on: 04.12.2019
 *      Author: tw
 */


#include "spi_task.h"

#include "spi.h"
#include "display.h"


static QueueHandle_t SPI_QueueHandle;
static TaskHandle_t SPI_TaskHandle;

static QueueHandle_t SPI2_QueueHandle;
static TaskHandle_t SPI2_TaskHandle;

void SPI_Task (void* pvParameters);
void SPI2_Task (void* pvParameters);


void SPI_Task_Init(void)
{
	BaseType_t taskCreateStatus;

	// SPI1 Task & Queue Init
	// Setup Queue, Timer and Task for event handling:
	SPI_QueueHandle = xQueueCreate(SPI_QUEUE_SIZE, sizeof(SPI_Queue_Data_t));
	if (SPI_QueueHandle == NULL) {
		// queue not created
		// todo: error!
		displayBlocking("ERROR 4301!", 5000);
		return;
	}
	vQueueAddToRegistry(SPI_QueueHandle, "SPI1_Queue");

	// Create task for handling SPI1 Bus Hardware Peripheral
	SPI_TaskHandle = NULL;

	taskCreateStatus = xTaskCreate(SPI_Task, "SPI1_Task",
			SPI_TASK_STACK_SIZE, (void *) 1,
			tskIDLE_PRIORITY, &SPI_TaskHandle);

	if (taskCreateStatus != pdPASS) {
		// something went wrong creating the task
		// todo: error!
		displayBlocking("ERROR 4301!", 5000);
		return;
	}

	// SPI2 Task & Queue Init
	// Setup Queue, Timer and Task for event handling:
	SPI2_QueueHandle = xQueueCreate(SPI_QUEUE_SIZE, sizeof(SPI_Queue_Data_t));
	if (SPI2_QueueHandle == NULL) {
		// queue not created
		// todo: error!
		displayBlocking("ERROR 4302!", 5000);
		return;
	}
	vQueueAddToRegistry(SPI2_QueueHandle, "SPI2_Queue");

	// Create task for handling SPI2 Bus Hardware Peripheral
	SPI2_TaskHandle = NULL;

	taskCreateStatus = xTaskCreate(SPI2_Task, "SPI2_Task",
			SPI_TASK_STACK_SIZE, (void *) 1,
			tskIDLE_PRIORITY, &SPI2_TaskHandle);

	if (taskCreateStatus != pdPASS) {
		// something went wrong creating the task
		// todo: error!
		displayBlocking("ERROR 4302!", 5000);
		return;
	}

}

void SPI_Task_Send_Request(SPI_Queue_Data_t queue_data)
{
	SPI_Queue_Data_t queuedata = queue_data;

	if(xQueueSendToBack(SPI_QueueHandle,&queuedata,( TickType_t )100) != pdPASS) {
		while(1) {
			// pass
		}
	}
}

void SPI2_Task_Send_Request(SPI_Queue_Data_t queue_data)
{
	SPI_Queue_Data_t queuedata = queue_data;

	if(xQueueSendToBack(SPI2_QueueHandle,&queuedata,( TickType_t )100) != pdPASS) {
		while(1) {
			// pass
		}
	}
}

void SPI_Task_Send_Request_fromISR(SPI_Queue_Data_t queue_data)
{
	SPI_Queue_Data_t queuedata = queue_data;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(xQueueSendToBackFromISR(SPI_QueueHandle, &queuedata, &xHigherPriorityTaskWoken) != pdPASS) {
		while(1) {
			// pass
		}
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void SPI2_Task_Send_Request_fromISR(SPI_Queue_Data_t queue_data)
{
	SPI_Queue_Data_t queuedata = queue_data;
	BaseType_t xHigherPriorityTaskWoken = pdTRUE; //pdFALSE;

	if(xQueueSendToBackFromISR(SPI2_QueueHandle, &queuedata, &xHigherPriorityTaskWoken) != pdPASS) {
		while(1) {
			// pass
		}
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void SPI_QueueRequest(Fp_SPI_Queue_Request requestFunction, void *pData)
{
	SPI_Queue_Data_t spi_request;

	if (requestFunction == NULL) return;

	spi_request.Handle = pData;
	spi_request.SPI_Request_Fp = requestFunction;
	SPI_Task_Send_Request(spi_request);
}

void SPI2_QueueRequest(Fp_SPI_Queue_Request requestFunction, void *pData)
{
	SPI_Queue_Data_t spi_request;

	if (requestFunction == NULL) return;

	spi_request.Handle = pData;
	spi_request.SPI_Request_Fp = requestFunction;
	SPI2_Task_Send_Request(spi_request);
}

void SPI_QueueRequest_fromISR(Fp_SPI_Queue_Request requestFunction, void *pData)
{
	SPI_Queue_Data_t spi_request;

	if (requestFunction == NULL) return;

	spi_request.Handle = pData;
	spi_request.SPI_Request_Fp = requestFunction;
	SPI_Task_Send_Request_fromISR(spi_request);
}

void SPI2_QueueRequest_fromISR(Fp_SPI_Queue_Request requestFunction, void *pData)
{
	SPI_Queue_Data_t spi_request;

	if (requestFunction == NULL) return;

	spi_request.Handle = pData;
	spi_request.SPI_Request_Fp = requestFunction;
	SPI2_Task_Send_Request_fromISR(spi_request);
}

void SPI_Task (void* pvParameters){
	UNUSED(pvParameters);
	SPI_Queue_Data_t queue_data = {0};

	for( ;; )
	{
		// Block on event queue
		if (xQueueReceive(SPI_QueueHandle, &queue_data, portMAX_DELAY))
		{
			queue_data.SPI_Request_Fp(queue_data.Handle);
		}
	}
	vTaskDelete( NULL);
}

void SPI2_Task (void* pvParameters){
	UNUSED(pvParameters);
	SPI_Queue_Data_t queue_data = {0};

	for( ;; )
	{
		// Block on event queue
		if (xQueueReceive(SPI2_QueueHandle, &queue_data, portMAX_DELAY))
		{
			queue_data.SPI_Request_Fp(queue_data.Handle);
		}
	}
	vTaskDelete( NULL);
}
