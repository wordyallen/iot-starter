#include "main.h"
#include "stdint.h"
#include "stdarg.h"

#include "FreeRTOS.h"
#include "task.h"

#include "aws_ststem_init.h"
#include "aws_wifi.h"
#include "key_provisioning.h"
#include "aws_clientcredentials.h"


static void vApplicationDaemonTaskStartupHook( void ){
  vKeyProvisioning();
  if( SYSTEM_Init() == pdPASS ){
      prvWifiConnect();
  }
};


static void prvInitializeHeap( void ){
	static unin8_t ucHeap1
}
