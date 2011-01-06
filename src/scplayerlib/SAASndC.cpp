// Part of SAASound copyright 1998-2004 Dave Hooper <dave@rebuzz.org>
//
// Thanks to this file (and associated header file) you can now
// use CSAASound from within a standard 'C' program
//
// Version 3.1.3 (8th March 2004)
// (c) 1998-2004 dave @ spc       <dave@rebuzz.org>
//
//////////////////////////////////////////////////////////////////////

#include "SAASound.h"
#include "types.h"
#include "SAAEnv.h"
#include "SAANoise.h"
#include "SAAFreq.h"
#include "SAAAmp.h"
#include "SAASound.h"
#include "SAAImpl.h"

SAASND SAAAPI newSAASND(void)
{
	return (SAASND)(new CSAASoundInternal());
}

void SAAAPI deleteSAASND(SAASND object)
{
	delete (LPCSAASOUND)(object);
}

void SAAAPI SAASNDSetSoundParameters(SAASND object, SAAPARAM uParam)
{
	((LPCSAASOUND)(object))->SetSoundParameters(uParam);
}

void SAAAPI SAASNDWriteAddress(SAASND object, BYTE nReg)
{
	((LPCSAASOUND)(object))->WriteAddress(nReg);
}

void SAAAPI SAASNDWriteData(SAASND object, BYTE nData)
{
	((LPCSAASOUND)(object))->WriteData(nData);
}

void SAAAPI SAASNDWriteAddressData(SAASND object, BYTE nReg, BYTE nData)
{
	((LPCSAASOUND)(object))->WriteAddressData(nReg, nData);
}

void SAAAPI SAASNDClear(SAASND object)
{
	((LPCSAASOUND)(object))->Clear();
}

BYTE SAAAPI SAASNDReadAddress(SAASND object)
{
	return ((LPCSAASOUND)(object))->ReadAddress();
}

SAAPARAM SAAAPI SAASNDGetCurrentSoundParameters(SAASND object)
{
	return ((LPCSAASOUND)(object))->GetCurrentSoundParameters();
}

unsigned short SAAAPI SAASNDGetCurrentBytesPerSample(SAASND object)
{
	return ((LPCSAASOUND)(object))->GetCurrentBytesPerSample();
}

unsigned short SAAAPI SAASNDGetBytesPerSample(SAAPARAM uParam)
{
	return CSAASound::GetBytesPerSample(uParam);
}

unsigned long SAAAPI SAASNDGetCurrentSampleRate(SAASND object)
{
	return ((LPCSAASOUND)(object))->GetCurrentSampleRate();
}

unsigned long SAAAPI SAASNDGetSampleRate(SAAPARAM uParam)
{
	return CSAASound::GetSampleRate(uParam);
}


void SAAAPI SAASNDGenerateMany(SAASND object, BYTE * pBuffer, unsigned long nSamples)
{
	((LPCSAASOUND)(object))->GenerateMany(pBuffer, nSamples);
}


int SAAAPI SAASNDSendCommand(SAASND object, SAACMD nCommandID, long nData)
{
	return ((LPCSAASOUND)(object))->SendCommand(nCommandID, nData);
}
