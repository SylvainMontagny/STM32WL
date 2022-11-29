
#include "LoRaMac.h"
#include "lora_info.h"
#include "sys_app.h" /* APP_PRINTF */


static LoraInfo_t loraInfo = {0, 0};


void LoraInfo_Init(void)
{
  loraInfo.ActivationMode = 0;
  loraInfo.Region = 0;
  loraInfo.ClassB = 0;
  loraInfo.Kms = 0;


#ifdef  REGION_AS923
  loraInfo.Region |= (1 << LORAMAC_REGION_AS923) ;
#endif /* REGION_AS923 */
#ifdef  REGION_AU915
  loraInfo.Region |= (1 << LORAMAC_REGION_AU915) ;
#endif /* REGION_AU915 */
#ifdef  REGION_CN470
  loraInfo.Region |= (1 << LORAMAC_REGION_CN470) ;
#endif /* REGION_CN470 */
#ifdef  REGION_CN779
  loraInfo.Region |= (1 << LORAMAC_REGION_CN779) ;
#endif /* REGION_CN779 */
#ifdef  REGION_EU433
  loraInfo.Region |= (1 << LORAMAC_REGION_EU433) ;
#endif /* REGION_EU433 */
#ifdef  REGION_EU868
  loraInfo.Region |= (1 << LORAMAC_REGION_EU868) ;
#endif /* REGION_EU868 */
#ifdef  REGION_KR920
  loraInfo.Region |= (1 << LORAMAC_REGION_KR920) ;
#endif /* REGION_KR920 */
#ifdef  REGION_IN865
  loraInfo.Region |= (1 << LORAMAC_REGION_IN865) ;
#endif /* REGION_IN865 */
#ifdef  REGION_US915
  loraInfo.Region |= (1 << LORAMAC_REGION_US915) ;
#endif /* REGION_US915 */
#ifdef  REGION_RU864
  loraInfo.Region |= (1 << LORAMAC_REGION_RU864) ;
#endif /* REGION_RU864 */

  if (loraInfo.Region == 0)
  {
    APP_PRINTF("error: At least one region shall be defined in the MW: check lorawan_conf.h \r\n");
    while (1 != UTIL_ADV_TRACE_IsBufferEmpty())
    {
      /* Wait that all printfs are completed*/
    }
    while (1) {} /* At least one region shall be defined */
  }

#if ( LORAMAC_CLASSB_ENABLED == 1 )
  loraInfo.ClassB = 1;
#elif !defined (LORAMAC_CLASSB_ENABLED)
#error LORAMAC_CLASSB_ENABLED not defined ( shall be <0 or 1> )
#endif /* LORAMAC_CLASSB_ENABLED */

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
  loraInfo.Kms = 0;
  loraInfo.ActivationMode = 3;
#else /* LORAWAN_KMS == 1 */
  loraInfo.Kms = 1;
  loraInfo.ActivationMode = ACTIVATION_BY_PERSONALIZATION + (OVER_THE_AIR_ACTIVATION << 1);
#endif /* LORAWAN_KMS */

}

LoraInfo_t *LoraInfo_GetPtr(void)
{

  return &loraInfo;
}

