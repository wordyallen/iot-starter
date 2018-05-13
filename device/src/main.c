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

}
