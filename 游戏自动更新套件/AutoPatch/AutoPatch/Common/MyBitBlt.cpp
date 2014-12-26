#include "mybitblt.h"
#include "FuncPerformanceLog.h"
void CommonClipBitBlt( 
					  BYTE*		pbyDst,			// 目标缓冲
					  int		nPixelStride,	// 像素跨距
					  int		nDstX,			
					  int		nDstY,			// 目标位置
					  int		nDstWidth,
					  int		nDstHeight,		// 目标大小
					  int		nDstPitch,		// 目标元素Pitch
					  BYTE*		pbySrc,			// 源缓冲
					  int		nBltLength,		// Blt长度
					  int		nBltLine,		// Blt高度
					  int		nSrcPitch		// 源元素Pitch
					  )
{
	guardfunc;
	if( !pbyDst || !pbySrc )
	{
		assert( false && "CommonClipBitBlt, dst or src is null" );
		return;
	}
	int nAdjDstX = nDstX;
	int nAdjDstY = nDstY;
	int nAdjBltLength = nBltLength;
	int nAdjBltLine = nBltLine;
	
	if( nDstX > nDstWidth )
		return;
	if( nDstX+nBltLength < 0 )
		return;
	if( nDstY > nDstHeight )
		return;
	if( nDstY+nBltLine < 0 )
		return;
	
	
	if( nDstX < 0 )
	{
		nAdjDstX = 0;
		nAdjBltLength = nDstX+nBltLength;
	}
	if( nAdjDstX+nAdjBltLength > nDstWidth )
	{
		nAdjBltLength = nDstWidth-nAdjDstX;
	}
	if( nDstY < 0 )
	{
		nAdjDstY = 0;
		nAdjBltLine = nDstY+nBltLine;
	}
	if( nAdjDstY+nAdjBltLine > nDstHeight )
	{
		nAdjBltLine = nDstHeight-nAdjDstY;
	}
	
	for( int i = 0; i < nAdjBltLine; i++ )
	{
		BYTE* dst = &pbyDst[((nAdjDstY+i)*nDstPitch+nAdjDstX)*nPixelStride];
		BYTE* src = &pbySrc[(i*nSrcPitch)*nPixelStride];
		memcpy( dst, src, nAdjBltLength*nPixelStride );
	}
	unguard;
}

void CCommonBitBlt::BitBlt( 
			   int x,
			   int y,
			   int nWidth,
			   int nHeight,
			   CCommonBitBlt* pSrc,
			   int nSrcX, 
			   int nSrcY,
			   DWORD dwOp )
{
	guardfunc;
	if( nSrcX+nWidth > pSrc->m_nWidth )
		nWidth = pSrc->m_nWidth-nSrcX;
	if( nSrcY+nHeight > pSrc->m_nHeight )
		nHeight = pSrc->m_nHeight-nSrcY;

	
	int nAdjDstX = x;
	int nAdjDstY = y;
	int nAdjBltLength = nWidth;
	int nAdjBltLine = nHeight;
	
	if( x > m_nWidth )
		return;
	if( x+nAdjBltLength < 0 )
		return;
	if( y > m_nHeight )
		return;
	if( y+nAdjBltLine < 0 )
		return;
	
	
	if( x < 0 )
	{
		nAdjDstX = 0;
		nAdjBltLength = x+nWidth;
	}
	if( nAdjDstX+nAdjBltLength > m_nWidth )
	{
		nAdjBltLength = m_nWidth-nAdjDstX;
	}
	if( y < 0 )
	{
		nAdjDstY = 0;
		nAdjBltLine = y+nHeight;
	}
	if( nAdjDstY+nAdjBltLine > m_nHeight )
	{
		nAdjBltLine = m_nHeight-nAdjDstY;
	}
	

	for( int i = 0; i < nAdjBltLine; i++ )
	{
		BYTE* dst = &m_pbyBuffer[((nAdjDstY+i)*m_nPitch+nAdjDstX)*m_nPixelStride];
		BYTE* src = &pSrc->m_pbyBuffer[((i+nSrcY)*pSrc->m_nPitch+nSrcX)*m_nPixelStride];
		switch( dwOp )
		{
		case eBltOp_Copy:
			memcpy( dst, src, nAdjBltLength*m_nPixelStride );
			break;
		case eBltOp_ColorKey:
			{
				DWORD* pdwDst = (DWORD*)dst;
				DWORD* pdwSrc = (DWORD*)src;
				for( int x = 0; x < nAdjBltLength; x++ )
				{
					DWORD color = *pdwSrc++;
					if( color & 0xff000000 )
						*pdwDst = color;
					pdwDst++;
				}
			}
			break;
		case eBltOp_AlphaBlend:
			{
				DWORD* pdwDst = (DWORD*)dst;
				DWORD* pdwSrc = (DWORD*)src;
				DWORD a0, r0, g0, b0;
				DWORD r1, g1, b1;
				for( int x = 0; x < nAdjBltLength; x++ )
				{
					DWORD dwSrcPixel = *pdwSrc++;
					DWORD dwDstPixel = *pdwDst;

					a0 = (dwSrcPixel&0xff000000)>>24;
					r0 = (dwSrcPixel&0x00ff0000)>>16;
					g0 = (dwSrcPixel&0x0000ff00)>>8;
					b0 = (dwSrcPixel&0x000000ff);

					r1 = (dwDstPixel&0x00ff0000)>>16;
					g1 = (dwDstPixel&0x0000ff00)>>8;
					b1 = (dwDstPixel&0x000000ff);

					float t = a0/255.0f;

					r0 = DWORD(t*r0+(1.0f-t)*r1);
					g0 = DWORD(t*g0+(1.0f-t)*g1);
					b0 = DWORD(t*b0+(1.0f-t)*b1);

					if( r0 > 255 )
						r0 = 255;
					if( g0 > 255 )
						g0 = 255;
					if( b0 > 255 )
						b0 = 255;
					//if( color & 0xff000000 )
					//	*pdwDst++ = color;
					//else
					//	pdwDst++;
					*pdwDst++ = 0xff000000|(r0<<16)|(g0<<8)|b0;
				}
			}
			break;
		}
		
	}
	unguard;	
}
