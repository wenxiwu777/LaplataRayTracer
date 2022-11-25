#pragma once

#ifdef _WIN32

namespace LaplataRayTracer
{
	class CMemBuffer
	{
	public:
		inline static void Load(HINSTANCE hAppInst, const TCHAR *pszFile, int w, int h)
		{
			m_shOrgBitmap = (HBITMAP)::LoadImage(hAppInst, (LPCTSTR)pszFile, IMAGE_BITMAP, w, h, LR_LOADFROMFILE);
		}
		inline static void Unload(void)
		{
			::DeleteObject(m_shOrgBitmap);
		}

	public:
		CMemBuffer(CDC *pDC, int w, int h) : m_pDC(pDC), m_pMemDC(NULL), m_nWidth(w), m_nHeight(h)
		{
			HDC hMemDC = ::CreateCompatibleDC(pDC->m_hDC);
			m_hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_shOrgBitmap);
			m_pMemDC = CDC::FromHandle(hMemDC);
		}
		~CMemBuffer()
		{
			::BitBlt(m_pDC->m_hDC, 0, 0, m_nWidth, m_nHeight, m_pMemDC->m_hDC, 0, 0, SRCCOPY);
			::SelectObject(m_pMemDC->m_hDC, m_hOldBitmap);
			::DeleteObject(m_pMemDC->m_hDC);
		}

		inline CDC *GetMemDC() { return m_pMemDC; }
		inline int GetWidth() const { return m_nWidth; }
		inline int GetHeight() const { return m_nHeight; }

		inline void Clear(COLORREF clBack)
		{
			for (int i = 0; i < m_nWidth; ++i)
				for (int j = 0; j < m_nHeight; ++j)
					m_pMemDC->SetPixel(i, j, clBack);
		}

	private:
		static HBITMAP m_shOrgBitmap;

	private:
		HBITMAP	m_hOldBitmap;
		CDC *	m_pDC;
		CDC *	m_pMemDC;
		int		m_nWidth;
		int		m_nHeight;

	};
}

#endif // _WIN32
