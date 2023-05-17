#include "sbgECom.h"
#include <streamBuffer/sbgStreamBuffer.h>
#include "commands/sbgEComCmdCommon.h"
#include "commands/sbgEComCmdGnss.h"
// sbgCommonLib headers
#include <sbgCommon.h>
#include <version/sbgVersion.h>

// sbgECom headers
#include <sbgEComLib.h>

void getAndPrintProductInfo(SbgEComHandle *pECom)
{
	printf("Getting product info... \n");
	SbgErrorCode					errorCode;
	SbgEComDeviceInfo				deviceInfo;

	assert(pECom);

	//
	// Get device inforamtions
	//
	errorCode = sbgEComCmdGetInfo(pECom, &deviceInfo);

	//
	// Display device information if no error
	//
	if (errorCode == SBG_NO_ERROR)
	{
		printf("Product info connection established. Fetching product info... \n");
		char	calibVersionStr[32];
		char	hwRevisionStr[32];
		char	fmwVersionStr[32];		

		sbgVersionToStringEncoded(deviceInfo.calibationRev, calibVersionStr, sizeof(calibVersionStr));
		sbgVersionToStringEncoded(deviceInfo.hardwareRev, hwRevisionStr, sizeof(hwRevisionStr));
		sbgVersionToStringEncoded(deviceInfo.firmwareRev, fmwVersionStr, sizeof(fmwVersionStr));

		printf("      Serial Number: %0.9"PRIu32"\n",	deviceInfo.serialNumber);
		printf("       Product Code: %s\n",				deviceInfo.productCode);
		printf("  Hardware Revision: %s\n",				hwRevisionStr);
		printf("   Firmware Version: %s\n",				fmwVersionStr);
		printf("     Calib. Version: %s\n",				calibVersionStr);
		printf("\n");
	}
	else
	{
		printf("Unable to connect to product info. \n");
	}
}

static SbgErrorCode gpsOnLogReceived(SbgEComHandle *pECom, SbgEComClass msgClass, SbgEComMsgId msg, const SbgBinaryLogData *pLogData, double *p)
{
	if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_0 && msg==SBG_ECOM_LOG_GPS1_POS)
	{
		*p = pLogData->gpsPosData.latitude;
		p++;
		*p = pLogData->gpsPosData.longitude;
		p++;
		*p = pLogData->gpsPosData.altitude;
	}
	return SBG_NO_ERROR;
}

void printGNSSConfig(SbgEComGnssInstallation sbgEComGnssInstallation)
{
	printf("Primary lever arm: [x,y,z] = [%.6f,%.6f,%.6f] \n", sbgEComGnssInstallation.leverArmPrimary[0], sbgEComGnssInstallation.leverArmPrimary[1], sbgEComGnssInstallation.leverArmPrimary[2]);
	printf("Primary level arm precise: X %s \n", sbgEComGnssInstallation.leverArmPrimaryPrecise ? "true" : "false");
	printf("Secondary lever arm: [x,y,z] = [%.6f,%.6f,%.6f] \n", sbgEComGnssInstallation.leverArmSecondary[0], sbgEComGnssInstallation.leverArmSecondary[1], sbgEComGnssInstallation.leverArmSecondary[2]);
}

static SbgErrorCode ChangeGNSSConfigRequest(SbgInterface *pInterface, float leverArmPrimary[3], bool leverArmPrimaryPrecise, float leverArmSecondary[3], int leverArmSecondaryMode)
{
	SbgErrorCode error_code = SBG_NO_ERROR;
	SbgErrorCode errorCode = SBG_NO_ERROR;
	SbgEComHandle comHandle;
		
	assert(pInterface);

	errorCode = sbgEComInit(&comHandle, pInterface);

	if (errorCode == SBG_NO_ERROR)
	{
		printf("sbgECom version %s\n\n", SBG_E_COM_VERSION_STR);

		getAndPrintProductInfo(&comHandle);
		
		SbgEComGnssInstallation sbgEComGnssInstallation;
		errorCode = sbgEComCmdGnss1InstallationGet(&comHandle,&sbgEComGnssInstallation);
		
		printf("Current GNSS configuration: \n");
		printGNSSConfig(sbgEComGnssInstallation);

		printf("Setting new GNSS configuration. \n");
		memcpy(sbgEComGnssInstallation.leverArmPrimary, leverArmPrimary, sizeof(leverArmPrimary));
		sbgEComGnssInstallation.leverArmPrimaryPrecise = leverArmPrimaryPrecise;
		memcpy(sbgEComGnssInstallation.leverArmSecondary, leverArmSecondary, sizeof(leverArmSecondary));
		sbgEComGnssInstallation.leverArmSecondaryMode = leverArmSecondaryMode;

		printf("New GNSS configuration: \n");
		printGNSSConfig(sbgEComGnssInstallation);
		printf("Secondary lever arm mode: %d \n", leverArmSecondaryMode);
		
		errorCode = sbgEComCmdGnss1InstallationSet(&comHandle,&sbgEComGnssInstallation);
		for (int i=0; i<=10; i++)
		{
			printf("saving_settings tryout: %d. \n", i);
			errorCode = sbgEComCmdSettingsAction(&comHandle, SBG_ECOM_SAVE_SETTINGS);

			if (errorCode == SBG_NO_ERROR)
			{
				printf("Successfully set new GNSS config.\n");
				break;
			}
			else
			{
				printf("Unable to configure SBG_ECOM_LOG_IMU_DATA log");
			}
			
		}
	}
	return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32

#    define MODULE_API __declspec(dllexport)

#else
#  define MODULE_API
#endif

	MODULE_API void ChangeGNSSConfig(char serialPortName[], int baudrate, float leverArmPrimary[3], bool leverArmPrimaryPrecise, float leverArmSecondary[3], int leverArmSecondaryMode)
	{
		printf("Starting config changing procedure... \n");
		SbgErrorCode		errorCode = SBG_NO_ERROR;
		SbgInterface		sbgInterface;

		//
		// Create a serial interface to communicate with the PULSE
		//
		errorCode = sbgInterfaceSerialCreate(&sbgInterface, serialPortName, baudrate);
    
		if (errorCode == SBG_NO_ERROR)
		{
			printf("Interface opened. \n");
			errorCode = ChangeGNSSConfigRequest(&sbgInterface, leverArmPrimary, leverArmPrimaryPrecise, leverArmSecondary, leverArmSecondaryMode);
			if (errorCode == SBG_NO_ERROR)
			{
				printf("Interface action successful. \n");
			}
			else
			{
				printf("Interface action failed. \n");
			}
		}
		else
		{
			printf("Failed to open interface. \n");
		}
    
	}
	MODULE_API bool GetGpsPos(double* latitude, double* longitude, double* altitude, char* serialPort, int baudrate)
	{
		SbgErrorCode errorCode = SBG_NO_ERROR;
		SbgInterface sbgInterface;
		errorCode = sbgInterfaceSerialCreate(&sbgInterface, serialPort, baudrate);
		if (errorCode != SBG_NO_ERROR)
		{
			printf("Failed to created serial interface in the task of getting GPS Position. \n");
			return false;
		}
		SbgEComHandle comHandle;
		
		errorCode = sbgEComInit(&comHandle, &sbgInterface);
		if (errorCode != SBG_NO_ERROR)
		{
			printf("Failed to initialize sbgECom in the task of getting GPS Position. \n");
			return false;
		}
		double gpsPos[3] = {0,0,0};
		double* pGpsPos = gpsPos;
		sbgEComSetReceiveLogCallback(&comHandle, gpsOnLogReceived, pGpsPos);
		int exitCounter = 0;
		while (exitCounter < 10)
		{
			errorCode = sbgEComHandle(&comHandle);
			if (errorCode!= SBG_ERROR)
			{
				if (gpsPos[0]!=0.0 && gpsPos[1]!=0.0 && gpsPos[2]!=0.0)
				{
					exitCounter = 100;
				}
				sbgSleep(1000);
			}
			exitCounter ++;
		}
		sbgEComClose(&comHandle);
		*latitude = gpsPos[0];
		*longitude = gpsPos[1];
		*altitude = gpsPos[2];
		printf("Value of my assigned pointer is: %f, %f, %f \n", *(latitude), *(longitude), *(altitude));
		if (exitCounter < 10)
		{
			return false;
		}
		return true;
	}
#ifdef __cplusplus
}
#endif



// // /////////////////////////////////////////////////////////////////////////////////////


SbgErrorCode sbgEComInit(SbgEComHandle *pHandle, SbgInterface *pInterface)
{
	SbgErrorCode errorCode = SBG_NO_ERROR;
	
	assert(pHandle);
	assert(pInterface);
	
	//
	// Initialize the sbgECom handle
	//
	pHandle->pReceiveLogCallback	= NULL;
	pHandle->pUserArg				= NULL;

	//
	// Initialize the default number of trials and time out
	//
	pHandle->numTrials			= 3;
	pHandle->cmdDefaultTimeOut	= SBG_ECOM_DEFAULT_CMD_TIME_OUT;

	//
	// Initialize the protocol 
	//
	errorCode = sbgEComProtocolInit(&pHandle->protocolHandle, pInterface);
	
	return errorCode;
}

SbgErrorCode sbgEComClose(SbgEComHandle *pHandle)
{
	SbgErrorCode errorCode = SBG_NO_ERROR;

	assert(pHandle);

	//
	// Close the protocol
	//
	errorCode = sbgEComProtocolClose(&pHandle->protocolHandle);
	
	return errorCode;
}

SbgErrorCode sbgEComHandleOneLog(SbgEComHandle *pHandle)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	SbgBinaryLogData	logData;
	uint8_t				receivedMsg;
	uint8_t				receivedMsgClass;
	size_t				payloadSize;
	uint8_t				payloadData[SBG_ECOM_MAX_PAYLOAD_SIZE];

	assert(pHandle);

	//
	// Try to read a received frame
	//
	errorCode = sbgEComProtocolReceive(&pHandle->protocolHandle, &receivedMsgClass, &receivedMsg, payloadData, &payloadSize, sizeof(payloadData));

	//
	// Test if we have received a valid frame
	//
	if (errorCode == SBG_NO_ERROR)
	{
		//
		// Test if the received frame is a binary log
		//
		if (sbgEComMsgClassIsALog((SbgEComClass)receivedMsgClass))
		{
			//
			// The received frame is a binary log one
			//
			errorCode = sbgEComBinaryLogParse((SbgEComClass)receivedMsgClass, (SbgEComMsgId)receivedMsg, payloadData, payloadSize, &logData);

			//
			// Test if the incoming log has been parsed successfully
			//
			if (errorCode == SBG_NO_ERROR)
			{
				//
				// Test if we have a valid callback to handle received logs
				//
				if (pHandle->pReceiveLogCallback)
				{
					//
					// Call the binary log callback using the new method
					//
					errorCode = pHandle->pReceiveLogCallback(pHandle, (SbgEComClass)receivedMsgClass, receivedMsg, &logData, pHandle->pUserArg);
				}

				//
				// Clean up resources allocated during parsing, if any.
				//
				sbgEComBinaryLogCleanup(&logData, (SbgEComClass)receivedMsgClass, (SbgEComMsgId)receivedMsg);
			}
			else
			{
				//
				// Call the on error callback
				//
			}
		}
		else
		{
			//
			// We have received a command, it shouldn't happen
			//
		}
	}
	else if (errorCode != SBG_NOT_READY)
	{
		//
		// We have received an invalid frame
		//
		SBG_LOG_WARNING(errorCode, "Invalid frame received");
	}
	
	return errorCode;
}

SbgErrorCode sbgEComHandle(SbgEComHandle *pHandle)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;

	assert(pHandle);

	//
	// Try to read all received frames, we thus loop until we get an SBG_NOT_READY error
	//
	do
	{
		//
		// Try to read and parse one frame
		//
		errorCode = sbgEComHandleOneLog(pHandle);
	} while (errorCode != SBG_NOT_READY);
	
	return errorCode;
}

SbgErrorCode sbgEComPurgeIncoming(SbgEComHandle *pHandle)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;

	assert(pHandle);

	errorCode = sbgEComProtocolPurgeIncoming(&pHandle->protocolHandle);

	return errorCode;
}

void sbgEComSetReceiveLogCallback(SbgEComHandle *pHandle, SbgEComReceiveLogFunc pReceiveLogCallback, void *pUserArg)
{
	assert(pHandle);

	//
	// Define the callback and the user argument
	//
	pHandle->pReceiveLogCallback	= pReceiveLogCallback;
	pHandle->pUserArg				= pUserArg;
}

void sbgEComSetCmdTrialsAndTimeOut(SbgEComHandle *pHandle, uint32_t numTrials, uint32_t cmdDefaultTimeOut)
{
	assert(pHandle);
	assert(numTrials > 0);
	assert(cmdDefaultTimeOut > 0);

	//
	// Define the new settings
	//
	pHandle->numTrials			= numTrials;
	pHandle->cmdDefaultTimeOut	= cmdDefaultTimeOut;
}

void sbgEComErrorToString(SbgErrorCode errorCode, char errorMsg[256])
{
	if (errorMsg)
	{
		//
		// For each error code, copy the error msg
		//
		switch (errorCode)
		{
		case SBG_NO_ERROR:
			strcpy(errorMsg, "SBG_NO_ERROR: No error."); 
			break;
		case SBG_ERROR:
			strcpy(errorMsg, "SBG_ERROR: Generic error."); 
			break;
		case SBG_NULL_POINTER:
			strcpy(errorMsg, "SBG_NULL_POINTER: A pointer is null."); 
			break;
		case SBG_INVALID_CRC:
			strcpy(errorMsg, "SBG_INVALID_CRC: The received frame has an invalid CRC.");
			break;
		case SBG_INVALID_FRAME:
			strcpy(errorMsg, "SBG_INVALID_FRAME: The received frame is invalid.");
			break;
		case SBG_TIME_OUT:
			strcpy(errorMsg, "SBG_TIME_OUT: We have a time out during frame reception.");
			break;
		case SBG_WRITE_ERROR:
			strcpy(errorMsg, "SBG_WRITE_ERROR: All bytes hasn't been written.");
			break;
		case SBG_READ_ERROR:
			strcpy(errorMsg, "SBG_READ_ERROR: All bytes hasn't been read.");
			break;
		case SBG_BUFFER_OVERFLOW:
			strcpy(errorMsg, "SBG_BUFFER_OVERFLOW: A buffer is too small to contain so much data.");
			break;
		case SBG_INVALID_PARAMETER:
			strcpy(errorMsg, "SBG_INVALID_PARAMETER: An invalid parameter has been founded.");
			break;
		case SBG_NOT_READY:
			strcpy(errorMsg, "SBG_NOT_READY: A device isn't ready (Rx isn't ready for example).");
			break;
		case SBG_MALLOC_FAILED:
			strcpy(errorMsg, "SBG_MALLOC_FAILED: Failed to allocate a buffer.");
			break;
		case SGB_CALIB_MAG_NOT_ENOUGH_POINTS:
			strcpy(errorMsg, "SGB_CALIB_MAG_NOT_ENOUGH_POINTS: Not enough points were available to perform magnetometers calibration.");
			break;
		case SBG_CALIB_MAG_INVALID_TAKE:
			strcpy(errorMsg, "SBG_CALIB_MAG_INVALID_TAKE: The calibration procedure could not be properly executed due to insufficient precision.");
			break;
		case SBG_CALIB_MAG_SATURATION:
			strcpy(errorMsg, "SBG_CALIB_MAG_SATURATION: Saturation were detected when attempt to calibrate magnetos.");
			break;
		case SBG_CALIB_MAG_POINTS_NOT_IN_A_PLANE:
			strcpy(errorMsg, "SBG_CALIB_MAG_POINTS_NOT_IN_A_PLANE: 2D calibration procedure could not be performed.");
			break;
		case SBG_DEVICE_NOT_FOUND:
			strcpy(errorMsg, "SBG_DEVICE_NOT_FOUND: A device couldn't be founded or opened.");
			break;
		case SBG_OPERATION_CANCELLED:
			strcpy(errorMsg, "SBG_OPERATION_CANCELLED: An operation has been cancelled by a user.");
			break;
		case SBG_NOT_CONTINUOUS_FRAME:
			strcpy(errorMsg, "SBG_NOT_CONTINUOUS_FRAME: We have received a frame that isn't a continuous one.");
			break;
		case SBG_INCOMPATIBLE_HARDWARE:
			strcpy(errorMsg, "SBG_INCOMPATIBLE_HARDWARE: Hence valid, the configuration cannot be executed because of incompatible hardware.");
			break;
		default:
			sprintf(errorMsg, "Undefined error code: %u", errorCode);
			break;
		}
	}
}
