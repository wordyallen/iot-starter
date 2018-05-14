#include "main.h"
#include "stdint.h"
#include "stdarg.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

// lib
#include "aws_system_init.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"
#include "aws_clientcredential.h"

/* Some functions might require the OS to be running (wifi).
   This happens with the daemon task hook */
void vApplicationDaemonTaskStartupHook( void );

// Globals
RTC_HandleTypeDef xHrtc;
RNG_HandleTypeDef xHrng;

// Private Vars
static UART_HandleTypeDef xConsoleUart;

// Private function prototype (signature)
static void SystemClock_Config( void );
static void Console_UART_Init( void );
static void prvWifiConnect( void );

// Init clock LEDs, RNG (Random Number Generator) and Wifi Module
static void prvMiscInitialization( void );

// Heap_5 cause RAM is contioguous. See http://www.freertos.org/a00111.html
static void prvIntializeHeap( void );


int main( void ) {
  prvMiscInitialization();
  vTaskStartScheduler();
  return 0;
}


void vApplicationDaemonTaskStartupHook( void ){
  vDevModeKeyProvisioning();
  if( SYSTEM_Init() == pdPASS ){
      prvWifiConnect();
  }
}


static void prvWifiConnect( void ) {

  WIFINetworkParams_t xNetworkParams;
  WIFIReturnCode_t xWifiStatus;
  uint8_t ucIPAddr[ 4 ];

  // Setup WiFi Struct params to connect to access point
  xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
  xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
  xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
  xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
  xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;

  xWifiStatus = WIFI_On();

  if( xWifiStatus == eWiFiSuccess ) {
    configPRINTF( ( "WiFi module initialized.\r\n" ) );
    xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );     //Try connecting with credentials
    if( xWifiStatus = eWiFiSuccess ){
      configPRINTF( ( "WiFi connected to AP %s.\r\n", xNetworkParams.pcSSID ) );
      WIFI_GetIP( &ucIPAddr[ 0 ] );
      configPRINTF((
        "IP Address acquired %d.%d.%d.%d\r\n",
        ucIPAddr[ 0 ], ucIPAddr[ 1 ], ucIPAddr[ 2 ], ucIPAddr[ 3 ]
      ));
    } else {
      /* Connection failed. Configure softAP to allow user to set wifi credentials. */
      configPRINTF((
        "WiFi failed to connect to AP %s.\r\n",
        xNetworkParams.pcSSID
      ));

      xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
      xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
      xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
      xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

      configPRINTF((
        "Connect to softAP %s using password %s. \r\n",
        xNetworkParams.pcSSID, xNetworkParams.pcPassword
      ));

      while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess ){
        configPRINTF((
          "Connect to softAP %s using password %s and configure WiFi. \r\n",
           xNetworkParams.pcSSID,
           xNetworkParams.pcPassword
         ));
      }

      configPRINTF((
        "WiFi configuration to softAP %s successful. \r\n",
        xNetworkParams.pcSSID
      ));

    }
  } else {
     configPRINTF( ( "WiFi module failed to initialize.\r\n" ) );
  }
}


/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(
  StaticTask_t ** ppxIdleTaskTCBBuffer,
  StackType_t ** ppxIdleTaskStackBuffer,
  uint32_t * pulIdleTaskStackSize
){
  static StaticTask_t xIdleTaskTCB;
  static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory(
   StaticTask_t ** ppxTimerTaskTCBBuffer,
   StackType_t ** ppxTimerTaskStackBuffer,
   uint32_t * pulTimerTaskStackSize
){
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


void vSTM32L475putc( void * pv, char ch ){
  while( HAL_OK != HAL_UART_Transmit( &xConsoleUart, ( uint8_t * ) &ch, 1, 30000 ) ){
  }
}


static void prvMiscInitialization( void ) {
    HAL_Init();
    SystemClock_Config();
    prvInitializeHeap();
    BSP_LED_Init( LED_GREEN );
    BSP_PB_Init( BUTTON_USER, BUTTON_MODE_EXTI );
    xHrng.Instance = RNG;
    if( HAL_RNG_Init( &xHrng ) != HAL_OK ){
      Error_Handler();
    }
    RTC_Init();
    Console_UART_Init();
}


void Error_Handler( void ){
  while( 1 ){
    BSP_LED_Toggle( LED_GREEN );
    HAL_Delay( 200 );
  }
}


void vApplicationMallocFailedHook() {
  configPRINTF( ( "ERROR: Malloc failed to allocate memory\r\n" ) );
}


void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName ){
  portDISABLE_INTERRUPTS();
  for( ; ; );
}


void * malloc( size_t xSize ){
  configASSERT( xSize == ~0 );
  return NULL;
}


void vOutputChar( const char cChar, const TickType_t xTicksToWait ){
    ( void ) cChar;
    ( void ) xTicksToWait;
}


void vMainUARTPrintString( char * pcString ){
    const uint32_t ulTimeout = 3000UL;
    HAL_UART_Transmit(
      &xConsoleUart,
      ( uint8_t * ) pcString,
      strlen( pcString ),
      ulTimeout
    );
}


void prvGetRegistersFromStack( uint32_t * pulFaultStackAddress ){

    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  /* Link register. */
    volatile uint32_t pc;  /* Program counter. */
    volatile uint32_t psr; /* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* Remove compiler warnings about the variables not being used. */
    ( void ) r0;
    ( void ) r1;
    ( void ) r2;
    ( void ) r3;
    ( void ) r12;
    ( void ) lr;  /* Link register. */
    ( void ) pc;  /* Program counter. */
    ( void ) psr; /* Program status register. */

    for( ; ; );
}


void HardFault_Handler( void ){
  __asm volatile
  (
      " tst lr, #4                                                \n"
      " ite eq                                                    \n"
      " mrseq r0, msp                                             \n"
      " mrsne r0, psp                                             \n"
      " ldr r1, [r0, #24]                                         \n"
      " ldr r2, handler2_address_const                            \n"
      " bx r2                                                     \n"
      " handler2_address_const: .word prvGetRegistersFromStack    \n"
}


int iMainRand32( void ){
  static UBaseType_t uxlNextRand; /*_RB_ Not seeded. */
  const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;
  uxlNextRand = ( ulMultiplier * uxlNextRand ) + ulIncrement;
  return( ( int ) ( uxlNextRand >> 16UL ) & 0x7fffUL );
}


static void prvInitializeHeap( void ){
  static uint8_t ucHeap1[ configTOTAL_HEAP_SIZE ];
  static uint8_t ucHeap2[ 27 * 1024 ] __attribute__( ( section( ".freertos_heap2" ) ) );
  HeapRegion_t xHeapRegions[] = {
    { ( unsigned char * ) ucHeap2, sizeof( ucHeap2 ) },
    { ( unsigned char * ) ucHeap1, sizeof( ucHeap1 ) },
    { NULL,                                        0 }
  };
  vPortDefineHeapRegions( xHeapRegions );
}
