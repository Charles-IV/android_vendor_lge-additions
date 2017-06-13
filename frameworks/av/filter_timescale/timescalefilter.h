/**
 * Time Scale Filter Definition
 *
 * author : seunghyeon.kim@lge.com
 */
#ifndef __TIMESCALE_FILTER_H__
#define __TIMESCALE_FILTER_H__

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <map>

#include "ifilter.h"

#include "SoundTouch.h"
using namespace soundtouch;

class TimeScaleFilter : public IFilter {
public:
    TimeScaleFilter() : pST(NULL) { }
    virtual ~TimeScaleFilter() { }

    //virtual int        Open(FilterParamType param);
    virtual int        Open();
    virtual void    DoWork(unsigned char *in, unsigned long inSize, unsigned char *out, unsigned long *outSize);    // Size is in byte.
    //virtual int        GetConfig(LGF_IndexType index, void *p) ;
    //virtual int        SetConfig(LGF_IndexType index, void *p);
    virtual void    PutData(unsigned char *in, unsigned long inSize);
    virtual int     GetData(unsigned char *out, unsigned long bufSize, unsigned long *outSize);
    virtual int        GetConfig(void *p) ;
    virtual int        SetConfig(void *p);
    virtual int        Close();
    virtual int        Notify(unsigned short event, unsigned long param);
    virtual const char *getName() {return "TimeScaleFilter"; }

private:
    SoundTouch *pST;
    AudioParamTimeScaleType aParam;
};


extern "C" IFilter *createFilter() {
    return new TimeScaleFilter;
}
extern "C" void destroyFilter(IFilter* p) {
    delete p;
}

#endif // __TIMESCALE_FILTER_H__
