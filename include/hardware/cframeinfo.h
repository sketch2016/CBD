#ifndef CFRAMEINFO_H
#define CFRAMEINFO_H

#include <math.h>
#include "GenICam/System.h"
#include "Media/ImageConvert.h"


typedef unsigned char BYTE;

class CFrameInfo : public Dahua::Memory::CBlock
{
public:
    CFrameInfo()
    {
        m_pImageBuf = NULL;
        m_nBufferSize = 0;
        m_pRGBbuffer = NULL;
        m_nRGBBufferSize = 0;
        m_nWidth = 0;
        m_nHeight = 0;
        m_PixelType = Dahua::GenICam::gvspPixelMono8;
        m_nPaddingX = 0;
        m_nPaddingY = 0;
        m_nTimeStamp = 0;
    }

    ~CFrameInfo()
    {
        if(m_pRGBbuffer != NULL) {
            free(m_pRGBbuffer);
        }

        if(m_pImageBuf != NULL) {
            free(m_pImageBuf);
        }
    }

public:
    BYTE		*m_pImageBuf;
    int			m_nBufferSize;
    BYTE        *m_pRGBbuffer;
    int         m_nRGBBufferSize;
    int			m_nWidth;
    int			m_nHeight;
    Dahua::GenICam::EPixelType	m_PixelType;
    int			m_nPaddingX;
    int			m_nPaddingY;
    uint64_t	m_nTimeStamp;
};

#endif // CFRAMEINFO_H
