/*
 * CheckPattrn.h
 *
 *  Created on: Aug 17, 2024
 *      Author: poorya
 */

#ifndef CHECKPATTRN_H_
#define CHECKPATTRN_H_
#define IN_RANGE(_var, _low, _high) (((_var) >= (_low))&&((_var) <= (_high)))
#include "circularbuf.h"
#include <opencv2/core/core.hpp>


class CheckPattern
{
public:
	struct SPat
	{
		int nLen;
		bool bWite;
		int nX;
		int nY;
	};
private:
	CCircularBuffer<SPat> *m_pbuf;
	SPat *m_pSpat;
	SPat *m_pSModifypat;
	int m_nPatLen;

	bool m_bWite;
	int m_nWite;
	int m_nBlack;
public:
	CheckPattern(SPat aS[], int nLen)
	{
		m_nPatLen = nLen;
		m_pSpat = new SPat[nLen];
		m_pSModifypat = new SPat[nLen];
		m_pbuf = new CCircularBuffer<SPat>(nLen);

		//! attention: pattern should be normalized pattern and have 1
		for (int nI = 0; nI < nLen; nI++)
			m_pSpat[nI] = aS[nI];

		m_bWite = false;
		m_nWite = 0;
		m_nBlack = 0;
	}


	bool CheckPatrn(unsigned char cVal, int nX, int nY)
	{
		bool bMatch = false;
		int nVal = 0;
		SPat sPat;
		sPat.nY = nY;
		sPat.nX = nX;
		if (m_bWite)
		{
			if (cVal < 128) //! black
			{
				sPat.bWite = true;
				sPat.nLen = m_nWite;
				bMatch = AddPattern(sPat);
				m_nWite = 0;
				m_bWite = false;
				m_nBlack++;
			}
			else //!white
			{
				m_nWite++;
				m_bWite = true;
			}
		}
		else
		{
			if (cVal > 128) //! white
			{
				sPat.bWite = false;
				sPat.nLen = m_nBlack;
				bMatch = AddPattern(sPat);
				m_nBlack = 0;
				m_bWite = true;
				m_nWite++;
			}
			else //!black
			{
				m_nBlack++;
				m_bWite = false;
			}
		}
		return bMatch;
	}

	bool CheckPatrnPres(unsigned char cVal, int nX, int nY, float fMinCof, float fMaxCof)
	{
		bool bMatch = false;

		int nVal = 0;
		SPat sPat;
		sPat.nY = nY;
		sPat.nX = nX;
		if (m_bWite)
		{
			if (cVal < 128) //! black
			{
				sPat.bWite = true;
				sPat.nLen = m_nWite;
				bMatch = AddPatternPres(sPat, fMinCof, fMaxCof);
				m_nWite = 0;
				m_bWite = false;
				m_nBlack++;
			}
			else //!white
			{
				m_nWite++;
				m_bWite = true;
			}
		}
		else
		{
			if (cVal > 128) //! white
			{
				sPat.bWite = false;
				sPat.nLen = m_nBlack;
				bMatch = AddPatternPres(sPat, fMinCof, fMaxCof);
				m_nBlack = 0;
				m_bWite = true;
				m_nWite++;
			}
			else //!black
			{
				m_nBlack++;
				m_bWite = false;
			}
		}
		return bMatch;
	}


	void Clear()
	{
		m_pbuf->Clear();
	}
	void GetBuf(SPat *spPat)
	{
		for(int nI = 0; nI < m_nPatLen; nI++)
		{
			spPat[nI] = *BufVal(nI);
		}
	}
private:
	bool AddPattern(SPat sPat)
	{
		m_pbuf->Enqueue(sPat);
		return CheckMatch();
	}
	bool AddPatternPres(SPat sPat, float fMinCof, float fMaxCof)
	{
		m_pbuf->Enqueue(sPat);
		return CheckMatchPrecision(fMinCof, fMaxCof);
	}
	SPat *BufVal(int nIndex)
	{
		return m_pbuf->Read(nIndex, m_pbuf->Head());
	}
	int FindMin()
	{
		int nMin = BufVal(0)->nLen;
		for(int nI = 0; nI < m_nPatLen; nI++)
		{
			if(nMin > BufVal(nI)->nLen)
				nMin = BufVal(nI)->nLen;
		}
		return nMin;
	}
	bool CheckMatchPrecision(float MinCof, float MaxCof)
	{
		if(m_pbuf->size() != m_nPatLen)
			return false;

		int dM = FindMin();
		if(dM == 0)
			return false;

		for(int nI = 0; nI < m_nPatLen; nI++)
		{
			float fLen = BufVal(nI)->nLen;
			float fMin = dM;

			int nMinVal = floorf((fLen/fMin) * MinCof);
			int nMaxVal = ceilf((fLen/fMin) * MaxCof);

			if((!IN_RANGE(m_pSpat[nI].nLen,nMinVal,nMaxVal)) || (BufVal(nI)->bWite != m_pSpat[nI].bWite))
				return false;
		}
		return true;
	}
	bool CheckMatch()
	{
		if(m_pbuf->size() != m_nPatLen)
			return false;

		int dM = FindMin();
		if(dM == 0)
			return false;

		for(int nI = 0; nI < m_nPatLen; nI++)
		{
			if(((BufVal(nI)->nLen/dM) != m_pSpat[nI].nLen) || (BufVal(nI)->bWite != m_pSpat[nI].bWite))
				return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------------
};



#endif /* CHECKPATTRN_H_ */
