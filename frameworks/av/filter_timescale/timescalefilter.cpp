/**
 * Time Scale Filter Class Implementation
 *
 * author : seunghyeon.kim@lge.com
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TimeScaleFilter"
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#include <utils/Log.h>

#include "timescalefilter.h"

static void setParam(SoundTouch *pST, AudioParamTimeScaleType *pParam)
{
    pST->clear();

    pST->setChannels(pParam->nChannels);
    pST->setSampleRate(pParam->nSamplingRate);
    if(pParam->bSpeech) {
        pST->setSetting(SETTING_SEQUENCE_MS,pParam->nSeqMsec);
        pST->setSetting(SETTING_SEEKWINDOW_MS, pParam->nSeekWinMsec);
        pST->setSetting(SETTING_OVERLAP_MS, pParam->nOverlapMsec);
    }
    pST->setTempo(pParam->fTempo);
    pST->setRate(pParam->fRate);
    pST->setPitchSemiTones(pParam->nPitch);
    pST->setSetting(SETTING_USE_QUICKSEEK, pParam->bQuickSeek);
    pST->setSetting(SETTING_USE_AA_FILTER, pParam->bAntiAlias);
}

int TimeScaleFilter::Open( )
{
    int ret = 0;

    pST = new SoundTouch();
    if(!pST)
    {
        ret = -1;
    }
    else
    {
        memset(&aParam, 0, sizeof(aParam));

        // default values setting
        aParam.nSamplingRate = 44100;
        aParam.nChannels = 2;    // ch support: 1 or 2
        aParam.fTempo = 1.0f;
        aParam.fRate = 1.0f;
        aParam.fPitch = 1.0f;
        aParam.nPitch = 0;
        aParam.bQuickSeek = (bool)0;
        aParam.bAntiAlias = (bool)1;
        aParam.bSpeech = (bool)0;

        setParam(pST, &aParam);
    }
    return ret;
}

void TimeScaleFilter::DoWork(unsigned char *in, unsigned long inSize, unsigned char *out, unsigned long *outSize)
{
    unsigned long channel = aParam.nChannels;
    unsigned long nSamples = inSize/2/channel;
    unsigned long nSamplesAccu = 0;
    float tempo = aParam.fTempo;

    pST->putSamples((SAMPLETYPE*)in, (uint)nSamples);
    SAMPLETYPE *obuf = (SAMPLETYPE *)out;

    uint reqSizeMax = nSamples*(1/tempo);

    if(reqSizeMax > nSamples)
        reqSizeMax = nSamples;

    do {
        nSamples = pST->receiveSamples(obuf+nSamplesAccu*channel, reqSizeMax);
        reqSizeMax -= nSamples;
        nSamplesAccu += nSamples;
    } while( nSamples);

    *outSize = (nSamplesAccu*2*channel);
}

void    TimeScaleFilter::PutData(unsigned char *in, unsigned long inSize)
{
    unsigned long channel = aParam.nChannels;
    unsigned long nSamples = inSize/2/channel;


    pST->putSamples((SAMPLETYPE*)in, (uint)nSamples);
}

int     TimeScaleFilter::GetData(unsigned char *out, unsigned long bufSize, unsigned long *outSize)
{
    unsigned long channel = aParam.nChannels;
    unsigned long nSamples = bufSize/2/channel;
    unsigned long nSamplesAccu = 0;
    float tempo = aParam.fTempo;

    SAMPLETYPE *obuf = (SAMPLETYPE *)out;

    uint reqSizeMax = nSamples;

    if(reqSizeMax > nSamples)
        reqSizeMax = nSamples;

    do {
        nSamples = pST->receiveSamples(obuf+nSamplesAccu*channel, reqSizeMax);
        reqSizeMax -= nSamples;
        nSamplesAccu += nSamples;
    } while( nSamples);

    *outSize = (nSamplesAccu*2*channel);

    return nSamplesAccu;
}

int TimeScaleFilter::Close()
{
    if(pST)
    {
        pST->clear();
        delete pST;
        pST = NULL;
    }
    return 0;
}

int TimeScaleFilter::GetConfig(void *p)
{
    AudioParamTimeScaleType *pParam = (AudioParamTimeScaleType *)p;
    memcpy(pParam, &aParam, sizeof(AudioParamTimeScaleType));
    return 0;
}

int TimeScaleFilter::SetConfig(void *p)
{
    AudioParamTimeScaleType *pParam = (AudioParamTimeScaleType *)p;
    memcpy(&aParam, pParam, sizeof(AudioParamTimeScaleType));
    setParam(pST, &aParam);
    return 0;
}

int TimeScaleFilter::Notify(unsigned short event, unsigned long param)
{
    return 0;
}
