#ifndef _mybitblt
#define _mybitblt
#include <windows.h>
#include <assert.h>
enum
{
	eBltOp_Copy,
	eBltOp_ColorKey,
	eBltOp_AlphaBlend,
};
class CCommonBitBlt
{
public:
	CCommonBitBlt( 
		void*	pBuffer,
		int		nPixelStride,
		int		nWidth,
		int		nHeight,
		int		nPitch = 0 )
	{
		m_pbyBuffer = (BYTE*)pBuffer;
		m_nPixelStride = nPixelStride;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		if( nPitch == 0 )
			m_nPitch = nWidth;
		else
			m_nPitch = nPitch;
	}

	void	BitBlt( 
		int x,
		int y,
		int nWidth,
		int nHeight,
		CCommonBitBlt* pSrc,
		int nSrcX, 
		int nSrcY,
		DWORD dwOp = eBltOp_Copy );
public:
	BYTE*	m_pbyBuffer;
	int		m_nPixelStride;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nPitch;
};

#endif