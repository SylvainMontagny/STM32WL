/*!
 * \file      LmHandler.c
 *
 * \brief     Implements the LoRaMac layer handling.
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LmHandler.c
  * @author  MCD Application Team
  * @brief   LoRaMAC Layer handling definition
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utilities.h"
#include "timer.h"
#include "Commissioning.h"
#include "NvmDataMgmt.h"
#include "radio.h"
#include "Region.h"
#include "LoRaMacTest.h"
#include "LmHandler.h"
#include "LmhPackage.h"
#include "LmhpCompliance.h"
#include "secure-element.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */
#include "lorawan_version.h"
#include "lora_info.h"

//edit sylvain
#include "sys_app.h"
//#ifndef INC_LCD_PRINTF_H_
//	#include "lcd_printf.h"
//#endif
// fin edit

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#include "kms.h"
#include "kms_platf_objects_interface.h"
#endif /* LORAWAN_KMS */
#if (!defined (LORAWAN_DATA_DISTRIB_MGT) || (LORAWAN_DATA_DISTRIB_MGT == 0))
#else /* LORAWAN_DATA_DISTRIB_MGT == 1 */
#include "LmhpPackagesRegistration.h"
#endif /* LORAWAN_DATA_DISTRIB_MGT */



/* Private typedef -----------------------------------------------------------*/
/*!
 * MAC notification type
 */
typedef enum PackageNotifyTypes_e
{
    PACKAGE_MCPS_CONFIRM,
    PACKAGE_MCPS_INDICATION,
    PACKAGE_MLME_CONFIRM
} PackageNotifyTypes_t;

/* Private define ------------------------------------------------------------*/
/*!
 * Package application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/* Private macro -------------------------------------------------------------*/
/*!
 * Hex 8 split buffer
 */
#define HEX8(X)   X[0], X[1], X[2], X[3], X[4], X[5], X[6], X[7]

/*!
 * Hex 16 split buffer
 */
#define HEX16(X)  HEX8(X), X[8], X[9], X[10], X[11], X[12], X[13], X[14], X[15]

/* Private variables ---------------------------------------------------------*/
static CommissioningParams_t CommissioningParams =
{
    .DevEui = { 0 },  // Automatically filed from secure-element
    .JoinEui = { 0 }, // Automatically filed from secure-element
    .NetworkId = LORAWAN_NETWORK_ID,
    .DevAddr = LORAWAN_DEVICE_ADDRESS,
};

/*!
 * LoRaWAN compliance tests handler parameters
 */
static LmhpComplianceParams_t LmhpComplianceParams =
{
    .AdrEnabled =       LORAMAC_HANDLER_ADR_ON,
    .DutyCycleEnabled = false,
    .StopPeripherals =  NULL,
    .StartPeripherals = NULL,
};

static LmhPackage_t *LmHandlerPackages[PKG_MAX_NUMBER];

/*!
 * Upper layer LoRaMac parameters
 */
static LmHandlerParams_t LmHandlerParams;

/*!
 * Upper layer callbacks
 */
static LmHandlerCallbacks_t *LmHandlerCallbacks;

/*!
 * Used to notify LmHandler of LoRaMac events
 */
static LoRaMacPrimitives_t LoRaMacPrimitives;

/*!
 * LoRaMac callbacks
 */
static LoRaMacCallback_t LoRaMacCallbacks;

static LmHandlerJoinParams_t JoinParams =
{
    .Mode = ACTIVATION_TYPE_NONE,
    .Datarate = DR_0,
    .Status = LORAMAC_HANDLER_ERROR
};

static LmHandlerTxParams_t TxParams =
{
    .MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG,
    .AckReceived = 0,
    .Datarate = DR_0,
    .UplinkCounter = 0,
    .AppData =
    {
        .Port = 0,
        .BufferSize = 0,
        .Buffer = NULL
    },
    .TxPower = TX_POWER_0,
    .Channel = 0
};

static LmHandlerRxParams_t RxParams =
{
    .Rssi = 0,
    .Snr = 0,
    .DownlinkCounter = 0,
    .RxSlot = -1,
    .LinkCheck = false,
    .DemodMargin = 0,
    .NbGateways = 0
};

#if ( LORAMAC_CLASSB_ENABLED == 1 )
static LmHandlerBeaconParams_t BeaconParams =
{
    .State = LORAMAC_HANDLER_BEACON_ACQUIRING,
    .Info =
    {
        .Time = { .Seconds = 0, .SubSeconds = 0 },
        .Frequency = 0,
        .Datarate = 0,
        .Rssi = 0,
        .Snr = 0,
        .GwSpecific =
        {
            .InfoDesc = 0,
            .Info = { 0 }
        }
    }
};
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

/*!
 * Package Application buffer
 */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/*!
 * Package application data structure
 */
static LmHandlerAppData_t AppData = { 0, LORAWAN_APP_DATA_BUFFER_MAX_SIZE, AppDataBuffer };

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/*!
 * Indicates if a switch to Class B operation is pending or not.
 */
static bool IsClassBSwitchPending = false;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

static bool CtxRestoreDone = false;

/* Private function prototypes -----------------------------------------------*/
/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *                             containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm );

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication, LoRaMacRxStatus_t *RxStatus );

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication, LoRaMacRxStatus_t *RxStatus );

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/*!
 * Starts the beacon search
 *
 * \retval  status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                 processed else \ref LORAMAC_HANDLER_ERROR
 */
static LmHandlerErrorStatus_t LmHandlerBeaconReq( void );

/*!
 * \brief   Informs the server on the ping-slot periodicity to use
 *
 * \param   [IN] periodicity Is equal to 2^periodicity seconds.
 *                           Example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 *
 * \retval  status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                 processed else \ref LORAMAC_HANDLER_ERROR
 */
static LmHandlerErrorStatus_t LmHandlerPingSlotReq(uint8_t periodicity);
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

/*!
 * Notifies the package to process the LoRaMac callbacks.
 *
 * \param [IN] notifyType MAC notification type [PACKAGE_MCPS_CONFIRM,
 *                                               PACKAGE_MCPS_INDICATION,
 *                                               PACKAGE_MLME_CONFIRM,
 *                                               PACKAGE_MLME_INDICATION]
 * \param[IN] params      Notification parameters. The params type can be
 *                        [McpsConfirm_t, McpsIndication_t, MlmeConfirm_t, MlmeIndication_t]
 */
static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params );

static void LmHandlerPackagesProcess( void );

/*!
 * \brief   Check if the package ID is initialized
 *
 * \param   [IN] id package identifier
 *
 * \retval  status Returns true if initialized else false
 */
static bool LmHandlerPackageIsInitialized(uint8_t id);

/*!
 * \brief   Displays end-device class update
 *
 * \param   [IN] deviceClass Current end-device class
 */
static void DisplayClassUpdate(DeviceClass_t deviceClass);

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/*!
 * \brief   Displays beacon status update
 *
 * \param   [IN] params Beacon parameters
 */
static void DisplayBeaconUpdate(LmHandlerBeaconParams_t *params);
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

static LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs );

/* Exported functions ---------------------------------------------------------*/
LmHandlerErrorStatus_t LmHandlerInit( LmHandlerCallbacks_t *handlerCallbacks )
{
    LmHandlerCallbacks = handlerCallbacks;

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
    LoRaMacCallbacks.GetBatteryLevel = LmHandlerCallbacks->GetBatteryLevel;
    LoRaMacCallbacks.GetTemperatureLevel = LmHandlerCallbacks->GetTemperature;
    LoRaMacCallbacks.GetUniqueId = LmHandlerCallbacks->GetUniqueId;
    LoRaMacCallbacks.NvmDataChange  = NvmDataMgmtEvent;
    LoRaMacCallbacks.MacProcessNotify = LmHandlerCallbacks->OnMacProcess;

    /*The LoRa-Alliance Compliance protocol package should always be initialized and activated.*/
    if (LmHandlerPackageRegister(PACKAGE_ID_COMPLIANCE, &LmhpComplianceParams) != LORAMAC_HANDLER_SUCCESS)
    {
        return LORAMAC_HANDLER_ERROR;
    }

#if (!defined (LORAWAN_DATA_DISTRIB_MGT) || (LORAWAN_DATA_DISTRIB_MGT == 0))
#else /*LORAWAN_DATA_DISTRIB_MGT == 1*/
    if (LmhpPackagesRegistrationInit() != LORAMAC_HANDLER_SUCCESS)
    {
        return LORAMAC_HANDLER_ERROR;
    }
#endif /*LORAWAN_DATA_DISTRIB_MGT*/
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerConfigure( LmHandlerParams_t *handlerParams )
{
    uint16_t nbNvmData = 0;
    MibRequestConfirm_t mibReq;
    LoraInfo_t *loraInfo;

    UTIL_MEM_cpy_8((void *)&LmHandlerParams, (const void *)handlerParams, sizeof(LmHandlerParams_t));

#if ( LORAMAC_CLASSB_ENABLED == 1 )
    IsClassBSwitchPending = false;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

    loraInfo = LoraInfo_GetPtr();

    if (0U != ((1 << (LmHandlerParams.ActiveRegion)) & (loraInfo->Region)))
    {
        if (LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LmHandlerParams.ActiveRegion) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
    }
    else
    {
        MW_LOG(TS_ON, VLEVEL_ALWAYS, "error: Region is not defined in the MW: set lorawan_conf.h accordingly\r\n");
        while (1) {}  /* error: Region is not defined in the MW */
    }

    // Restore data if required
    nbNvmData = NvmDataMgmtRestore( );

    // Try to restore from NVM and query the mac if possible.
    if( nbNvmData > 0 )
    {
        CtxRestoreDone = true;
    }
    else
    {
        CtxRestoreDone = false;
    }

    // Read secure-element DEV_EUI and JOIN_EUI values.
    mibReq.Type = MIB_DEV_EUI;
    LoRaMacMibGetRequestConfirm( &mibReq );
    memcpy1( CommissioningParams.DevEui, mibReq.Param.DevEui, 8 );

    mibReq.Type = MIB_JOIN_EUI;
    LoRaMacMibGetRequestConfirm( &mibReq );
    memcpy1( CommissioningParams.JoinEui, mibReq.Param.JoinEui, 8 );

#if ( STATIC_DEVICE_ADDRESS != 1 )
    CommissioningParams.DevAddr = LmHandlerCallbacks->GetDevAddr();
#endif /* STATIC_DEVICE_ADDRESS != 1 */

    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = CommissioningParams.DevAddr;
    LoRaMacMibSetRequestConfirm(&mibReq);

    if(ADMIN_ACTIVATION_TYPE == 2){
	MW_LOG(TS_OFF, VLEVEL_L, "> AppEui-JoinEui          %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
		   HEX8(CommissioningParams.JoinEui));
    MW_LOG(TS_OFF, VLEVEL_L, "> DevEui                  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           HEX8(CommissioningParams.DevEui));

    }
    if(ADMIN_ACTIVATION_TYPE == 1){
	MW_LOG(TS_OFF, VLEVEL_L, "> DevEui                  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
		   HEX8(CommissioningParams.DevEui));
    MW_LOG(TS_OFF, VLEVEL_L, "> DevAddr                 %02X:%02X:%02X:%02X\r\n",
           (unsigned)((unsigned char *)(&CommissioningParams.DevAddr))[3],
           (unsigned)((unsigned char *)(&CommissioningParams.DevAddr))[2],
           (unsigned)((unsigned char *)(&CommissioningParams.DevAddr))[1],
           (unsigned)((unsigned char *)(&CommissioningParams.DevAddr))[0]);
    }
#if (defined (LORAWAN_KMS) && (LORAWAN_KMS == 1))
    MW_LOG(TS_OFF, VLEVEL_L, "###### KMS ENABLED \r\n");
#endif /* LORAWAN_KMS == 1 */

    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_REPEATER_SUPPORT;
    mibReq.Param.EnableRepeaterSupport = LORAWAN_REPEATER_SUPPORT;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LmHandlerParams.AdrEnable;
    LoRaMacMibSetRequestConfirm( &mibReq );

    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    getPhy.Attribute = PHY_DUTY_CYCLE;
    phyParam = RegionGetPhyParam( LmHandlerParams.ActiveRegion, &getPhy );
    LmHandlerParams.DutyCycleEnabled = (bool) phyParam.Value;

    LmHandlerSetSystemMaxRxError( 20 );

    /* override previous value if reconfigure new region */
    LoRaMacTestSetDutyCycleOn( LmHandlerParams.DutyCycleEnabled );

    return LORAMAC_HANDLER_SUCCESS;
}

bool LmHandlerIsBusy( void )
{
    if( LoRaMacIsBusy( ) == true )
    {
        return true;
    }
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        // The network isn't yet joined, try again later.
        LmHandlerJoin( JoinParams.Mode );
        return true;
    }

    if( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning( ) == true )
    {
        return true;
    }
    return false;
}

void LmHandlerProcess( void )
{
    /* Call at first the LoRaMAC process before to run all package process features */
    // Processes the LoRaMac events
    LoRaMacProcess( );

    // Call all packages process functions
    LmHandlerPackagesProcess( );

    // Store to NVM if required
    NvmDataMgmtStore( );
}

void LmHandlerJoin( ActivationType_t mode )
{
    MibRequestConfirm_t mibReq;

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#if (OVER_THE_AIR_ACTIVATION == 0)
    if ( mode == ACTIVATION_TYPE_OTAA )
    {
        MW_LOG(TS_OFF, VLEVEL_M, "ERROR: OTAA mode not implemented\r\n");
        while (1);
    }
#endif /* OVER_THE_AIR_ACTIVATION */
#if (ACTIVATION_BY_PERSONALIZATION == 0)
    if ( mode == ACTIVATION_TYPE_ABP )
    {
        MW_LOG(TS_OFF, VLEVEL_M, "ERROR: ABP mode not implemented\r\n");
        while (1);
    }
#endif /* ACTIVATION_BY_PERSONALIZATION */
#endif /* LORAWAN_KMS */

    if ( mode == ACTIVATION_TYPE_OTAA )
    {
        MlmeReq_t mlmeReq;
        JoinParams.Mode = ACTIVATION_TYPE_OTAA;

        LoRaMacStart();

        mlmeReq.Type = MLME_JOIN;
        mlmeReq.Req.Join.Datarate = LmHandlerParams.TxDatarate;

        // Starts the OTAA join procedure
        LoRaMacMlmeRequest( &mlmeReq );
    }
    else
    {
        JoinParams.Mode = ACTIVATION_TYPE_ABP;
        JoinParams.Datarate = LmHandlerParams.TxDatarate;
        JoinParams.Status = LORAMAC_HANDLER_SUCCESS;

        if (CtxRestoreDone == false)
        {
            /* Tell the MAC layer which network server version are we connecting too. */
            mibReq.Type = MIB_ABP_LORAWAN_VERSION;
            mibReq.Param.AbpLrWanVersion.Value = ABP_ACTIVATION_LRWAN_VERSION;
            LoRaMacMibSetRequestConfirm(&mibReq);

#if (defined (LORAWAN_KMS) && (LORAWAN_KMS == 1))
#if ( USE_LRWAN_1_1_X_CRYPTO == 1 )
            SecureElementSetObjHandler(F_NWK_S_INT_KEY, KMS_F_NWK_S_INT_KEY_OBJECT_HANDLE);
            SecureElementSetObjHandler(S_NWK_S_INT_KEY, KMS_S_NWK_S_INT_KEY_OBJECT_HANDLE);
            SecureElementSetObjHandler(NWK_S_ENC_KEY, KMS_NWK_S_ENC_KEY_OBJECT_HANDLE);
#else /* USE_LRWAN_1_1_X_CRYPTO == 0 */
            SecureElementSetObjHandler(NWK_S_KEY, KMS_NWK_S_KEY_OBJECT_HANDLE);
#endif /* USE_LRWAN_1_1_X_CRYPTO */
            SecureElementSetObjHandler(APP_S_KEY, KMS_APP_S_KEY_OBJECT_HANDLE);
#endif  /* LORAWAN_KMS == 1 */
        }

        LoRaMacStart();
        mibReq.Type = MIB_NETWORK_ACTIVATION;
        mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
        LoRaMacMibSetRequestConfirm( &mibReq );

        // Notify upper layer
        LmHandlerCallbacks->OnJoinRequest( &JoinParams );
        LmHandlerRequestClass(LmHandlerParams.DefaultClass);
    }
}

LmHandlerFlagStatus_t LmHandlerJoinStatus( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if( status == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
        {
            return LORAMAC_HANDLER_RESET;
        }
        else
        {
            return LORAMAC_HANDLER_SET;
        }
    }
    else
    {
        return LORAMAC_HANDLER_RESET;
    }
}

LmHandlerErrorStatus_t LmHandlerStop(void)
{
    if (LoRaMacDeInitialization() == LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerSend( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed,
                                      TimerTime_t *nextTxIn, bool allowDelayedTx )
{
    LoRaMacStatus_t status;
    LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_ERROR;
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (LoRaMacIsBusy() == true)
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }

    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        // The network isn't joined, try again.
        LmHandlerJoin(JoinParams.Mode);
        return LORAMAC_HANDLER_NO_NETWORK_JOINED;
    }

    if( ( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning( ) == true ) && ( appData->Port != LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->Port ) && ( appData->Port != 0 ) )
    {
        return LORAMAC_HANDLER_COMPLIANCE_RUNNING;
    }

    mcpsReq.Req.Unconfirmed.Datarate = LmHandlerParams.TxDatarate;
    if( LoRaMacQueryTxPossible( appData->BufferSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        TxParams.MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG;
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    }
    else
    {
        TxParams.MsgType = isTxConfirmed;
        mcpsReq.Req.Unconfirmed.fPort = appData->Port;
        mcpsReq.Req.Unconfirmed.fBufferSize = appData->BufferSize;
        mcpsReq.Req.Unconfirmed.fBuffer = appData->Buffer;
        if( isTxConfirmed == LORAMAC_HANDLER_UNCONFIRMED_MSG )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            // Edit sylvain & Sacha
            if (mcpsReq.Req.Unconfirmed.fPort == 0){
            	APP_LOG(TS_ON, VLEVEL_L, " Sending MAC Command.\r\n");
            	//lcd_printf(LCD_DEFAULT_FONT_COLOR, "Sending MAC Command.");
            }
            else{
            	APP_LOG(TS_ON, VLEVEL_L, " Sending Unconfirmed Data Up.\r\n");
            	//lcd_printf(LCD_DEFAULT_FONT_COLOR, "Sending Unconfirmed Data Up.");

            }
            // End Edit

        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.NbTrials = 8;
            // Edit sylvain & Sacha
            APP_LOG(TS_ON, VLEVEL_L, " Sending Confirmed Data Up - First try\r\n");
            //lcd_printf(LCD_DEFAULT_FONT_COLOR, "Sending Confirmed Data Up - 1");
            // End Edit
        }
    }

    TxParams.AppData = *appData;
    TxParams.Datarate = LmHandlerParams.TxDatarate;

    status = LoRaMacMcpsRequest(&mcpsReq, allowDelayedTx); // 1er Envoi du message mcpsReq
    if (nextTxIn != NULL)
    {
        *nextTxIn = mcpsReq.ReqReturn.DutyCycleWaitTime;
    }

    switch (status)
    {
        case LORAMAC_STATUS_OK:
            lmhStatus = LORAMAC_HANDLER_SUCCESS;
            break;
        case LORAMAC_STATUS_BUSY:
        case LORAMAC_STATUS_BUSY_UPLINK_COLLISION:
        case LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME:
        case LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME:
            lmhStatus = LORAMAC_HANDLER_BUSY_ERROR;
            break;
        case LORAMAC_STATUS_NO_NETWORK_JOINED:
            lmhStatus = LORAMAC_HANDLER_NO_NETWORK_JOINED;
            break;
        case LORAMAC_STATUS_CRYPTO_ERROR:
            lmhStatus = LORAMAC_HANDLER_CRYPTO_ERROR;
            break;
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
            lmhStatus = LORAMAC_HANDLER_DUTYCYCLE_RESTRICTED;
            break;
        case LORAMAC_STATUS_SERVICE_UNKNOWN:
        case LORAMAC_STATUS_PARAMETER_INVALID:
        case LORAMAC_STATUS_MAC_COMMAD_ERROR:
        case LORAMAC_STATUS_FCNT_HANDLER_ERROR:
        case LORAMAC_STATUS_REGION_NOT_SUPPORTED:
        case LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND:
        case LORAMAC_STATUS_NO_CHANNEL_FOUND:
        case LORAMAC_STATUS_LENGTH_ERROR:
        default:
            lmhStatus = LORAMAC_HANDLER_ERROR;
            break;
    }

    return lmhStatus;
}

LmHandlerErrorStatus_t LmHandlerLinkCheckReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_LINK_CHECK;

    status = LoRaMacMlmeRequest( &mlmeReq );

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerDeviceTimeReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_DEVICE_TIME;

    status = LoRaMacMlmeRequest( &mlmeReq );

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

#if ( LORAMAC_CLASSB_ENABLED == 1 )
static LmHandlerErrorStatus_t LmHandlerBeaconReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_BEACON_ACQUISITION;

    status = LoRaMacMlmeRequest( &mlmeReq );

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static LmHandlerErrorStatus_t LmHandlerPingSlotReq( uint8_t periodicity )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_PING_SLOT_INFO;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = periodicity;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

    status = LoRaMacMlmeRequest( &mlmeReq );

    if( status == LORAMAC_STATUS_OK )
    {
        LmHandlerParams.PingPeriodicity = periodicity;
        // Send an empty message
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0
        };
        return LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, false );
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

LmHandlerErrorStatus_t LmHandlerRequestClass( DeviceClass_t newClass )
{
    MibRequestConfirm_t mibReq;
    DeviceClass_t currentClass;
    LmHandlerErrorStatus_t errorStatus = LORAMAC_HANDLER_SUCCESS;

    if (LoRaMacIsBusy() == true)
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }

    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        return LORAMAC_HANDLER_NO_NETWORK_JOINED;
    }

    mibReq.Type = MIB_DEVICE_CLASS;
    if ( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    currentClass = mibReq.Param.Class;

    // Attempt to switch only if class update
    if( currentClass != newClass )
    {
        switch( newClass )
        {
        case CLASS_A:
            {
                if( currentClass != CLASS_A )
                {
                    mibReq.Param.Class = newClass;
                    if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                    {
                        // Switch is instantaneous
                        DisplayClassUpdate(newClass);
                        if (LmHandlerCallbacks->OnClassChange != NULL)
                        {
                            LmHandlerCallbacks->OnClassChange( newClass );
                        }
                    }
                    else
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                }
            }
            break;
        case CLASS_B:
            {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                if( currentClass != CLASS_A )
                {
                    errorStatus = LORAMAC_HANDLER_ERROR;
                }
                else
                {
                    // Beacon must first be acquired
                    errorStatus = LmHandlerDeviceTimeReq( );
                    IsClassBSwitchPending = true;
                }
#else /* LORAMAC_CLASSB_ENABLED == 0 */
                errorStatus = LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
            }
            break;
        case CLASS_C:
            {
                if( currentClass != CLASS_A )
                {
                    errorStatus = LORAMAC_HANDLER_ERROR;
                }
                else
                {
                    // Switch is instantaneous
                    mibReq.Param.Class = newClass;
                    if (LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK)
                    {
                        DisplayClassUpdate(newClass);
                        if (LmHandlerCallbacks->OnClassChange != NULL)
                        {
                            LmHandlerCallbacks->OnClassChange( newClass );
                        }
                    }
                    else
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return errorStatus;
}

LmHandlerErrorStatus_t LmHandlerGetCurrentClass( DeviceClass_t *deviceClass )
{
    MibRequestConfirm_t mibReq;
    if (deviceClass == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEVICE_CLASS;
    if (LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *deviceClass = mibReq.Param.Class;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetTxDatarate( int8_t *txDatarate )
{
    MibRequestConfirm_t mibGet;
    if (txDatarate == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibGet.Type = MIB_CHANNELS_DATARATE;
    if (LoRaMacMibGetRequestConfirm( &mibGet ) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *txDatarate = mibGet.Param.ChannelsDatarate;
    LmHandlerParams.TxDatarate = *txDatarate;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetActiveRegion( LoRaMacRegion_t *region )
{
    if (region == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *region = LmHandlerParams.ActiveRegion;
    return LORAMAC_HANDLER_SUCCESS;
}

static LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
    mibReq.Param.SystemMaxRxError = maxErrorInMs;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

/*
 *=============================================================================
 * LORAMAC NOTIFICATIONS HANDLING
 *=============================================================================
 */

static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    TxParams.IsMcpsConfirm = 1;
    TxParams.Status = mcpsConfirm->Status;
    TxParams.Datarate = mcpsConfirm->Datarate;
    TxParams.UplinkCounter = mcpsConfirm->UpLinkCounter;
    TxParams.TxPower = mcpsConfirm->TxPower;
    TxParams.Channel = mcpsConfirm->Channel;
    TxParams.AckReceived = mcpsConfirm->AckReceived;

    LmHandlerCallbacks->OnTxData( &TxParams );

    LmHandlerPackagesNotify( PACKAGE_MCPS_CONFIRM, mcpsConfirm );
}

static void McpsIndication( McpsIndication_t *mcpsIndication, LoRaMacRxStatus_t *RxStatus )
{
    LmHandlerAppData_t appData;
    DeviceClass_t deviceClass = CLASS_A;
    RxParams.IsMcpsIndication = 1;
    RxParams.Status = mcpsIndication->Status;

    if( RxParams.Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    RxParams.Datarate = mcpsIndication->RxDatarate;
    RxParams.Rssi = RxStatus->Rssi;
    RxParams.Snr = RxStatus->Snr;
    RxParams.RxSlot = RxStatus->RxSlot;
    RxParams.DownlinkCounter = mcpsIndication->DownLinkCounter;

    appData.Port = mcpsIndication->Port;
    appData.BufferSize = mcpsIndication->BufferSize;
    appData.Buffer = mcpsIndication->Buffer;

    LmHandlerCallbacks->OnRxData(&appData, &RxParams);

    if ((LmHandlerCallbacks->OnSysTimeUpdate != NULL) && (mcpsIndication->DeviceTimeAnsReceived == true))
    {
        LmHandlerCallbacks->OnSysTimeUpdate( );
    }

    // Call packages RxProcess function
    LmHandlerPackagesNotify( PACKAGE_MCPS_INDICATION, mcpsIndication );

    LmHandlerGetCurrentClass(&deviceClass);
    if ((mcpsIndication->FramePending == true) && (deviceClass == CLASS_A))
    {
        // The server signals that it has pending data to be sent.
        // We schedule an uplink as soon as possible to flush the server.

        // Send an empty message
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0
        };
        LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, true);
    }
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    TxParams.IsMcpsConfirm = 0;
    TxParams.Status = mlmeConfirm->Status;
    LmHandlerCallbacks->OnTxData( &TxParams );

    LmHandlerPackagesNotify( PACKAGE_MLME_CONFIRM, mlmeConfirm );

    switch( mlmeConfirm->MlmeRequest )
    {
    case MLME_JOIN:
        {
            MibRequestConfirm_t mibReq;
            mibReq.Type = MIB_DEV_ADDR;
            LoRaMacMibGetRequestConfirm( &mibReq );
            CommissioningParams.DevAddr = mibReq.Param.DevAddr;
            LmHandlerGetTxDatarate( &JoinParams.Datarate );

            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Status is OK, node has joined the network
                JoinParams.Status = LORAMAC_HANDLER_SUCCESS;
                LmHandlerRequestClass(LmHandlerParams.DefaultClass);
            }
            else
            {
                // Join was not successful. Try to join again
                JoinParams.Status = LORAMAC_HANDLER_ERROR;
            }
            // Notify upper layer
            LmHandlerCallbacks->OnJoinRequest( &JoinParams );
        }
        break;
    case MLME_LINK_CHECK:
        {
            RxParams.LinkCheck = true;
            RxParams.DemodMargin = mlmeConfirm->DemodMargin;
            RxParams.NbGateways = mlmeConfirm->NbGateways;
        }
        break;
    case MLME_DEVICE_TIME:
        {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
            if( IsClassBSwitchPending == true )
            {
                LmHandlerBeaconReq( );
            }
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
        }
        break;
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    case MLME_BEACON_ACQUISITION:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Beacon has been acquired
                // Request server for ping slot
                LmHandlerPingSlotReq( LmHandlerParams.PingPeriodicity );
            }
            else
            {
                // Beacon not acquired
                // Request Device Time again.
                LmHandlerDeviceTimeReq( );
            }
        }
        break;
    case MLME_PING_SLOT_INFO:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                MibRequestConfirm_t mibReq;

                // Class B is now activated
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_B;
                LoRaMacMibSetRequestConfirm( &mibReq );
                // Notify upper layer
                DisplayClassUpdate( CLASS_B );
                if (LmHandlerCallbacks->OnClassChange != NULL)
                {
                    LmHandlerCallbacks->OnClassChange( CLASS_B );
                }
                IsClassBSwitchPending = false;
            }
            else
            {
                LmHandlerPingSlotReq( LmHandlerParams.PingPeriodicity );
            }
        }
        break;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
    default:
        break;
    }
}

static void MlmeIndication( MlmeIndication_t *mlmeIndication, LoRaMacRxStatus_t *RxStatus )
{
    RxParams.IsMcpsIndication = 0;
    RxParams.Status = mlmeIndication->Status;
    RxParams.Rssi = RxStatus->Rssi;
    RxParams.Snr = RxStatus->Snr;
    RxParams.RxSlot = RxStatus->RxSlot;
    if( RxParams.Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
    {
        LmHandlerCallbacks->OnRxData( NULL, &RxParams );
    }

    switch( mlmeIndication->MlmeIndication )
    {
    case MLME_SCHEDULE_UPLINK:
        {// The MAC signals that we shall provide an uplink as soon as possible
            // Send an empty message
            LmHandlerAppData_t appData =
            {
                .Buffer = NULL,
                .BufferSize = 0,
                .Port = 0
            };

            if( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning( ) == false )
            {
                LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, true );
            }
        }
        break;
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    case MLME_BEACON_LOST:
        {
            MibRequestConfirm_t mibReq;
            // Switch to class A again
            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = CLASS_A;
            LoRaMacMibSetRequestConfirm( &mibReq );

            BeaconParams.State = LORAMAC_HANDLER_BEACON_LOST;
            BeaconParams.Info.Time.Seconds = 0;
            BeaconParams.Info.GwSpecific.InfoDesc = 0;
            UTIL_MEM_set_8(BeaconParams.Info.GwSpecific.Info, 0, 6);

            DisplayClassUpdate( CLASS_A );
            if (LmHandlerCallbacks->OnClassChange != NULL)
            {
                LmHandlerCallbacks->OnClassChange( CLASS_A );
            }
            DisplayBeaconUpdate( &BeaconParams );

            LmHandlerDeviceTimeReq( );
        }
        break;
    case MLME_BEACON:
        {
            if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
            {
                BeaconParams.State = LORAMAC_HANDLER_BEACON_RX;
                BeaconParams.Info = mlmeIndication->BeaconInfo;

                DisplayBeaconUpdate( &BeaconParams );
            }
            else
            {
                BeaconParams.State = LORAMAC_HANDLER_BEACON_NRX;
                BeaconParams.Info = mlmeIndication->BeaconInfo;

                DisplayBeaconUpdate( &BeaconParams );
            }
        }
        break;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
    default:
        break;
    }
}

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */

LmHandlerErrorStatus_t LmHandlerPackageRegister( uint8_t id, void *params )
{
    LmhPackage_t *package = NULL;
    switch( id )
    {
        case PACKAGE_ID_COMPLIANCE:
        {
            package = LmhpCompliancePackageFactory( );
            break;
        }
        default:
        {
#if (!defined (LORAWAN_DATA_DISTRIB_MGT) || (LORAWAN_DATA_DISTRIB_MGT == 0))
#else /*LORAWAN_DATA_DISTRIB_MGT == 1*/
            LmhpPackagesRegister(id, &package);
#endif /*LORAWAN_DATA_DISTRIB_MGT*/
            break;
        }
    }
    if( package != NULL )
    {
        LmHandlerPackages[id] = package;
        LmHandlerPackages[id]->OnJoinRequest = LmHandlerJoin;
        LmHandlerPackages[id]->OnSendRequest = LmHandlerSend;
        LmHandlerPackages[id]->OnDeviceTimeRequest = LmHandlerDeviceTimeReq;
        LmHandlerPackages[id]->OnPackageProcessEvent = LmHandlerCallbacks->OnMacProcess;
        LmHandlerPackages[id]->Init( params, AppData.Buffer, AppData.BufferSize );

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static bool LmHandlerPackageIsInitialized( uint8_t id )
{
    if (( id < PKG_MAX_NUMBER ) && ( LmHandlerPackages[id]->IsInitialized != NULL ))
    {
        return LmHandlerPackages[id]->IsInitialized( );
    }
    else
    {
        return false;
    }
}

static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( LmHandlerPackages[i] != NULL )
        {
            switch( notifyType )
            {
                case PACKAGE_MCPS_CONFIRM:
                {
                    if( LmHandlerPackages[i]->OnMcpsConfirmProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMcpsConfirmProcess( ( McpsConfirm_t* ) params );
                    }
                    break;
                }
                case PACKAGE_MCPS_INDICATION:
                {
                    if( ( LmHandlerPackages[i]->OnMcpsIndicationProcess != NULL ) &&
                        ( ( LmHandlerPackages[i]->Port == ((McpsIndication_t* )params)->Port ) ||
                          ( ( i == PACKAGE_ID_COMPLIANCE ) && ( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning() ))))
                    {
                        LmHandlerPackages[i]->OnMcpsIndicationProcess( ( McpsIndication_t* )params );
                    }
                    break;
                }
                case PACKAGE_MLME_CONFIRM:
                {
                    if( LmHandlerPackages[i]->OnMlmeConfirmProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMlmeConfirmProcess( ( MlmeConfirm_t* )params );
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

static void LmHandlerPackagesProcess( void )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( ( LmHandlerPackages[i] != NULL ) &&
            ( LmHandlerPackages[i]->Process != NULL ) &&
            ( LmHandlerPackageIsInitialized( i ) != false ) )
        {
            LmHandlerPackages[i]->Process( );
        }
    }
}

LmHandlerErrorStatus_t LmHandlerGetDevEUI(uint8_t *devEUI)
{
    MibRequestConfirm_t mibReq;
    if (devEUI == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEV_EUI;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8(devEUI, mibReq.Param.DevEui, SE_EUI_SIZE);
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDevEUI(uint8_t *devEUI)
{
#if ( STATIC_DEVICE_EUI != 1 )
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_DEV_EUI;
        mibReq.Param.DevEui = devEUI;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
#else /* STATIC_DEVICE_EUI == 1 */
    return LORAMAC_HANDLER_ERROR;
#endif /* STATIC_DEVICE_EUI */
}

LmHandlerErrorStatus_t LmHandlerGetAppEUI(uint8_t *appEUI)
{
    MibRequestConfirm_t mibReq;

    if (appEUI == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_EUI;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8(appEUI, mibReq.Param.JoinEui, SE_EUI_SIZE);
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAppEUI(uint8_t *appEUI)
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_JOIN_EUI;
        mibReq.Param.JoinEui = appEUI;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetNetworkID(uint32_t *networkId)
{
    MibRequestConfirm_t mibReq;

    if (networkId == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_NET_ID;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *networkId = mibReq.Param.NetID;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetNetworkID(uint32_t networkId)
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_NET_ID;
        mibReq.Param.NetID = networkId;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change NetworkID in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetDevAddr(uint32_t *devAddr)
{
    MibRequestConfirm_t mibReq;

    if (devAddr == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEV_ADDR;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *devAddr = mibReq.Param.DevAddr;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDevAddr(uint32_t devAddr)
{
#if ( STATIC_DEVICE_ADDRESS != 1 )
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        mibReq.Type = MIB_DEV_ADDR;
        mibReq.Param.DevAddr = devAddr;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change DevAddr in running state */
        return LORAMAC_HANDLER_ERROR;
    }
#else /* STATIC_DEVICE_ADDRESS == 1 */
    return LORAMAC_HANDLER_ERROR;
#endif /* STATIC_DEVICE_ADDRESS */
}

LmHandlerErrorStatus_t LmHandlerSetActiveRegion(LoRaMacRegion_t region)
{
    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        LmHandlerParams.ActiveRegion = region;
        return LmHandlerConfigure( &LmHandlerParams );
    }
    else
    {
        /* Cannot change Region in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetAdrEnable(bool *adrEnable)
{
    if (adrEnable == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *adrEnable = LmHandlerParams.AdrEnable;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAdrEnable(bool adrEnable)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = adrEnable;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    LmHandlerParams.AdrEnable = adrEnable;

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetTxDatarate(int8_t txDatarate)
{
    if (LmHandlerParams.AdrEnable == true)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DATARATE;
    mibReq.Param.ChannelsDatarate = txDatarate;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    LmHandlerParams.TxDatarate = txDatarate;

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetDutyCycleEnable(bool *dutyCycleEnable)
{
    if (dutyCycleEnable == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *dutyCycleEnable = LmHandlerParams.DutyCycleEnabled;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDutyCycleEnable(bool dutyCycleEnable)
{
    LmHandlerParams.DutyCycleEnabled = dutyCycleEnable;
    LoRaMacTestSetDutyCycleOn(dutyCycleEnable);

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRX2Params(RxChannelParams_t *rxParams)
{
    if (rxParams == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_RX2_CHANNEL;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    rxParams->Frequency = mibReq.Param.Rx2Channel.Frequency;
    rxParams->Datarate = mibReq.Param.Rx2Channel.Datarate;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetTxPower(int8_t *txPower)
{
    MibRequestConfirm_t mibReq;
    if (txPower == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_CHANNELS_TX_POWER;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *txPower = mibReq.Param.ChannelsTxPower;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRx1Delay(uint32_t *rxDelay)
{
    MibRequestConfirm_t mibReq;
    if (rxDelay == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_RECEIVE_DELAY_1;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.ReceiveDelay1;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRx2Delay(uint32_t *rxDelay)
{
    MibRequestConfirm_t mibReq;
    if (rxDelay == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_RECEIVE_DELAY_2;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.ReceiveDelay2;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx1Delay(uint32_t *rxDelay)
{
    MibRequestConfirm_t mibReq;
    if (rxDelay == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_1;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.JoinAcceptDelay1;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx2Delay(uint32_t *rxDelay)
{
    MibRequestConfirm_t mibReq;
    if (rxDelay == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_2;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.JoinAcceptDelay2;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetTxPower(int8_t txPower)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_TX_POWER;
    mibReq.Param.ChannelsTxPower = txPower;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRX2Params(RxChannelParams_t *rxParams)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel.Frequency = rxParams->Frequency;
    mibReq.Param.Rx2Channel.Datarate = rxParams->Datarate;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRx1Delay(uint32_t rxDelay)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RECEIVE_DELAY_1;
    mibReq.Param.ReceiveDelay1 = rxDelay;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRx2Delay(uint32_t rxDelay)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RECEIVE_DELAY_2;
    mibReq.Param.ReceiveDelay2 = rxDelay;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx1Delay(uint32_t rxDelay)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_1;
    mibReq.Param.JoinAcceptDelay1 = rxDelay;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx2Delay(uint32_t rxDelay)
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_2;
    mibReq.Param.JoinAcceptDelay2 = rxDelay;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetPingPeriodicity(uint8_t *pingPeriodicity)
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    if (pingPeriodicity == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *pingPeriodicity = LmHandlerParams.PingPeriodicity;
    return LORAMAC_HANDLER_SUCCESS;
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerSetPingPeriodicity(uint8_t pingPeriodicity)
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    /* Not yet joined */
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        LmHandlerParams.PingPeriodicity = pingPeriodicity;
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Region in running state */
        return LmHandlerPingSlotReq(pingPeriodicity);
    }
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerGetBeaconState(BeaconState_t *beaconState)
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    MibRequestConfirm_t mibReq;

    if (beaconState == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_BEACON_STATE;
    if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *beaconState =  mibReq.Param.BeaconState;
    return LORAMAC_HANDLER_SUCCESS;
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerGetNwkKey( uint8_t *nwkKey )
{
    Key_t *keyItem;
    if (nwkKey == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID(NWK_KEY, &keyItem))
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8( nwkKey, keyItem->KeyValue, 16 );
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetNwkKey( uint8_t *nwkKey )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_NWK_KEY;
        mibReq.Param.NwkKey = nwkKey;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetAppKey( uint8_t *appKey )
{
    Key_t *keyItem;
    if (appKey == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID(APP_KEY, &keyItem))
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8( appKey, keyItem->KeyValue, 16 );
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAppKey( uint8_t *appKey )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_APP_KEY;
        mibReq.Param.AppKey = appKey;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetNwkSKey( uint8_t *nwkSKey )
{
    Key_t *keyItem;
    if (nwkSKey == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID(NWK_S_KEY, &keyItem))
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8( nwkSKey, keyItem->KeyValue, 16 );
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetNwkSKey( uint8_t *nwkSKey )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_NWK_S_KEY;
        mibReq.Param.NwkSKey = nwkSKey;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetAppSKey( uint8_t *appSKey )
{
    Key_t *keyItem;
    if (appSKey == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID(APP_S_KEY, &keyItem))
    {
        return LORAMAC_HANDLER_ERROR;
    }
    UTIL_MEM_cpy_8( appSKey, keyItem->KeyValue, 16 );
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAppSKey( uint8_t *appSKey )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_APP_S_KEY;
        mibReq.Param.AppSKey = appSKey;
        if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
        {
            return LORAMAC_HANDLER_ERROR;
        }

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

static void DisplayClassUpdate(DeviceClass_t deviceClass)
{
    MW_LOG(TS_OFF, VLEVEL_M, "Switch to Class %c done\r\n", "ABC"[deviceClass]);
}

#if ( LORAMAC_CLASSB_ENABLED == 1 )
static void DisplayBeaconUpdate(LmHandlerBeaconParams_t *params)
{
    static const char *EventBeaconStateStrings[] = { "BC_ACQUIRING", "BC_LOST", "BC_RECEIVED", "BC_NOT_RECEIVED" };

    MW_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== %s\r\n", EventBeaconStateStrings[params->State]);
    if (params->State == LORAMAC_HANDLER_BEACON_RX)
    {
        MW_LOG(TS_OFF, VLEVEL_H, "###### BTIME:%010d | GW DESC:%d | GW INFO:%02X %02X %02X %02X %02X %02X\r\n",
               params->Info.Time.Seconds, params->Info.GwSpecific.InfoDesc,
               params->Info.GwSpecific.Info[0], params->Info.GwSpecific.Info[1],
               params->Info.GwSpecific.Info[2], params->Info.GwSpecific.Info[3],
               params->Info.GwSpecific.Info[4], params->Info.GwSpecific.Info[5]);
        MW_LOG(TS_OFF, VLEVEL_H, "###### FREQ:%d | DR:%d | RSSI:%d | SNR:%d\r\n",
               params->Info.Frequency, params->Info.Datarate,
               params->Info.Rssi, params->Info.Snr);
    }
}
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
