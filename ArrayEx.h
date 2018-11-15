/************************************************************

  ArrayEx.h

  An improved array template class provides functionalities that are
  similar to the MFC "CArray" class except that:

  a) No need to link MFC libraries, can be used in any win32 applications.
  b) Implemented elements sorting and searching methods using the
     "qsort" and "bsearch" algorithms (requiring operators ">" and "=="
	 being defined for stored element data type.
  c) The constant operator[] now returns "const TYPE&" instead of
     a "TYPE" instance, which was the case in "CArray".

  Written by: Abin (abinn32@yahoo.com)

  May 08, 2004		Initial release.
*************************************************************/

#ifndef __ARRAYEX_H__
#define __ARRAYEX_H__

#pragma warning(disable:4311)

#define USE_MULTI_THREAD // uncomment this line if used in multi-thread application

#ifdef USE_MULTI_THREAD
#include "SyncObj.h"
#endif // USE_MULTI_THREAD

//////////////////////////////////////////////////////////////////////
// Necessary Definitions for Non-MFC applications
//////////////////////////////////////////////////////////////////////
#ifndef __AFXWIN_H__ // If non-MFC ...
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <search.h>
#include <assert.h>
#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif
#endif // __AFXWIN_H__


// Element sorting states
enum { AE_SORT_NONE = 0,		// Not sorted
	   AE_SORT_ASCENDING,		// Sorted in ascending order
	   AE_SORT_DESCENDING };	// Sorted in descending order

template <class TYPE, class ARG_TYPE>
class CArrayEx
#ifdef USE_MULTI_THREAD
: public CSyncObj
#endif // USE_MULTI_THREAD
{
public:

	// Constructions & Destructor
	CArrayEx();
	CArrayEx(const CArrayEx& src);
	virtual ~CArrayEx();

	// Sort & Search
	void Sort(BOOL bAscending = TRUE);
	int IsSorted() const { return m_nSort; }
	int Find(ARG_TYPE tData, int nStartAt = 0) const;
	int ReverseFind(ARG_TYPE tData) const;
	
	// General Access
	int GetSize() const	{ return m_nElementCount; }
	int GetUpperBound() const { return m_nElementCount - 1; }
	BOOL SetSize(int nNewSize);
	BOOL IsEmpty() const { return m_nElementCount == 0; }
	BOOL IsIndexValid(int nIndex) const { return nIndex >= 0 && nIndex < m_nElementCount; }
	BOOL FreeExtra();
	BOOL Copy(const CArrayEx& src);

	// Element Insertion & Removal
	int InsertAt(int nIndex, ARG_TYPE tData, int nCount = 1);
	int InsertAt(int nIndex, const CArrayEx* pNewArray);
	int Add(ARG_TYPE tData, int nCount = 1);
	int Append(const CArrayEx& src);
	int RemoveAt(int nIndex, int nCount = 1);
	BOOL RemoveLast() { return RemoveAt(m_nElementCount - 1, 1) > 0; }
	void RemoveAll() { m_nElementCount = 0; }
	
	// Element Access
	const TYPE& GetLast() const { return GetAt(m_nElementCount - 1); }
	TYPE& GetLast() { return ElementAt(m_nElementCount - 1); }
	const TYPE& GetAt(int nIndex) const;
	BOOL SetLast(ARG_TYPE tData) { return SetAt(m_nElementCount - 1, tData); }
	BOOL SetAt(int nIndex, ARG_TYPE tData);
	TYPE& ElementAt(int nIndex);
	const TYPE* GetData() const { return m_pData; }
	TYPE* GetData()	{ m_nSort = AE_SORT_NONE; return m_pData; }

	// Operators
	TYPE& operator[](int nIndex) { return ElementAt(nIndex); }
	const TYPE& operator[](int nIndex) const { return GetAt(nIndex); }
	const CArrayEx& operator=(const CArrayEx& src);

protected:
	
	BOOL _MakeSpaces(int nIndex, int nCount = 1); // Make spaces for new elements
	BOOL _GrowSize(int nAdd); // dynamically grow array size
	void _AdjustIdx(int& nIndex, BOOL bAllowGrow) const;

	// call-back compare functions
	static int _CompareAscending(const void* p1, const void* p2);
	static int _CompareDescending(const void* p1, const void* p2);

	// member data
	int m_nSort; // sort states
	int m_nElementCount; // element count
	int m_nAllocSize; // allocated size
	TYPE* m_pData; // data
};

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::_CompareAscending(const void* p1, const void* p2)
{
	TYPE* pElement1 = (TYPE*)p1;
	TYPE* pElement2 = (TYPE*)p2;
	ASSERT(pElement1 != NULL && pElement2 != NULL);

	if (*pElement1 == *pElement2)
		return 0;

	if (*pElement1 > *pElement2)
		return 1;

	return -1;
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::_CompareDescending(const void* p1, const void* p2)
{
	return _CompareAscending(p2, p1); // just reverse the order
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::Sort(BOOL bAscending)
{
	if (m_nElementCount < 2 
		|| (bAscending && (m_nSort == AE_SORT_ASCENDING))
		|| (!bAscending && (m_nSort == AE_SORT_DESCENDING)))
		return; // no need to resort

	// use qsort
	if (bAscending)
		::qsort(m_pData, m_nElementCount, sizeof(TYPE), _CompareAscending);
	else
		::qsort(m_pData, m_nElementCount, sizeof(TYPE), _CompareDescending);

	m_nSort = bAscending ? AE_SORT_ASCENDING : AE_SORT_DESCENDING;
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Find(ARG_TYPE tData, int nStartAt/*=0*/) const
{
	if (nStartAt < 0)
		nStartAt = 0;

	if (!IsIndexValid(nStartAt))
		return -1;

	if (m_nSort == AE_SORT_ASCENDING) // ascending
	{
		void* p = ::bsearch(&tData, &m_pData[nStartAt], m_nElementCount - nStartAt, sizeof(TYPE), _CompareAscending);
		int nIndex = (p == NULL) ? -1 : int(((long)p - (long)m_pData) / sizeof(TYPE));
		while (nIndex > 0 && m_pData[nIndex - 1] == tData)
			nIndex--;
		return nIndex;
	}
	else if (m_nSort == AE_SORT_DESCENDING) // descending
	{
		void* p = ::bsearch(&tData, &m_pData[nStartAt], m_nElementCount - nStartAt, sizeof(TYPE), _CompareDescending);
		int nIndex = (p == NULL) ? -1 : int(((long)p - (long)m_pData) / sizeof(TYPE));
		while (nIndex > 0 && m_pData[nIndex - 1] == tData)
			nIndex--;
		return nIndex;
	}
	else // not sorted
	{
		for (int i = nStartAt; i < m_nElementCount; i++)
		{
			if (m_pData[i] == tData) 
				return i;
		}
		return -1;
	}	
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::ReverseFind(ARG_TYPE tData) const
{
	if (m_nElementCount == 0)
		return -1;

	if (m_nSort == AE_SORT_ASCENDING) // ascending
	{
		void* p = ::bsearch(&tData, m_pData, m_nElementCount, sizeof(TYPE), _CompareAscending);
		int nIndex = (p == NULL) ? -1 : int(((long)p - (long)m_pData) / sizeof(TYPE));
		while (nIndex < m_nElementCount - 1 && m_pData[nIndex + 1] == tData)
			nIndex++;
		return nIndex;
	}
	else if (m_nSort == AE_SORT_DESCENDING) // descending
	{
		void* p = ::bsearch(&tData, m_pData, m_nElementCount, sizeof(TYPE), _CompareDescending);
		int nIndex = (p == NULL) ? -1 : int(((long)p - (long)m_pData) / sizeof(TYPE));
		while (nIndex < m_nElementCount - 1 && m_pData[nIndex + 1] == tData)
			nIndex++;
		return nIndex;
	}
	else // not sorted
	{
		for (int i = m_nElementCount - 1; i >= 0; i--)
		{
			if (m_pData[i] == tData) 
				return i;
		}
		return -1;
	}	
}

template <class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::CArrayEx()
{
	m_nSort = AE_SORT_NONE;
	m_nElementCount = 0;
	m_nAllocSize = 0;
	m_pData = NULL;	
};

template <class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::CArrayEx(const CArrayEx& src)
{
	m_nSort = AE_SORT_NONE;
	m_nElementCount = 0;
	m_nAllocSize = 0;
	m_pData = NULL;
	if (!Copy(src))
		ASSERT(FALSE);
}

template <class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::~CArrayEx()
{
	if (m_pData != 0)
		delete [] m_pData;
};

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::SetSize(int nNewSize)
{
	if (nNewSize < 0)
		return FALSE;

	if (!_GrowSize(nNewSize - m_nElementCount))
		return FALSE;

	m_nElementCount = nNewSize;
	return TRUE;
};

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Add(ARG_TYPE tData, int nCount)
{
	// append at the end
	return InsertAt(m_nElementCount, tData, nCount);
}

// insert single element
template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE tData, int nCount)
{
	if (nCount < 1)
		return -1;

	_AdjustIdx(nIndex, TRUE);
	if (!_MakeSpaces(nIndex, nCount))
		return -1;

	for (int i = 0; i < nCount; i++)
		m_pData[nIndex + i] = tData;	

	m_nSort = AE_SORT_NONE;
	return nIndex;
}

// insert a whole array
template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::InsertAt(int nIndex, const CArrayEx* pNewArray)
{
	if (pNewArray == NULL || pNewArray->GetSize() < 1)
		return -1;

	_AdjustIdx(nIndex, TRUE);
	if (!_MakeSpaces(nIndex, pNewArray->GetSize()))
		return -1;

	for (int i = 0; i < pNewArray->GetSize(); i++)
		m_pData[nIndex + i] = pNewArray->GetAt(i);

	m_nSort = AE_SORT_NONE;
	return nIndex;
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{	
	if (nCount > m_nElementCount - nIndex)
		nCount = m_nElementCount - nIndex;

	if (!IsIndexValid(nIndex) || nCount < 1)
		return 0;

	const int SEG = m_nElementCount - nIndex - nCount;

	for (int i = 0; i < SEG; i++)
		m_pData[nIndex + i] = m_pData[nIndex + i + nCount];

	m_nElementCount -= nCount;
	return nCount;
}

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::FreeExtra()
{
	if (m_pData == NULL || m_nAllocSize == m_nElementCount)
		return TRUE;

	// reallocate data array with optimized size
	m_nAllocSize = m_nElementCount;
	TYPE* p = new TYPE[m_nAllocSize];
	if (p == NULL)
		return FALSE;

	for (int i = 0; i < m_nElementCount; i++)
		p[i] = m_pData[i];

	delete [] m_pData;
	m_pData = p;
	return TRUE;
}

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::Copy(const CArrayEx& src)
{
	if (!SetSize(src.m_nElementCount))
		return FALSE;
	
	for (int i = 0; i < m_nElementCount; i++)
		m_pData[i] = src.m_pData[i];
	m_nSort = src.m_nSort;
	return TRUE;
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Append(const CArrayEx& src)
{
	int nAppenedeAt = src.m_nElementCount > 0 ? m_nElementCount : -1;
	if (!_GrowSize(src.m_nElementCount))
		return -1;

	// append at the end
	for (int i = 0; i < src.m_nElementCount; i++)
		m_pData[nAppenedeAt + i] = src.m_pData[i];
	
	m_nElementCount += src.m_nElementCount;
	m_nSort = AE_SORT_NONE;
	return nAppenedeAt;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::_AdjustIdx(int& nIndex, BOOL bAllowGrow) const
{
	if (nIndex < 0)
		nIndex = 0;

	if (nIndex >= m_nElementCount)
		nIndex = bAllowGrow ? m_nElementCount : (m_nElementCount - 1);
}

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::_MakeSpaces(int nIndex, int nCount /*= 1*/)
{
	if (nCount < 1)
		return TRUE;

	if (!_GrowSize(nCount))
		return FALSE;

	for (int i = m_nElementCount - 1; i >= nIndex; i--)
		 m_pData[i + nCount] = m_pData[i];	
	
	m_nElementCount += nCount;
	return TRUE;
}

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::_GrowSize(int nAdd)
{
	if (nAdd < 1)
		return TRUE;

	if (m_nAllocSize <= 0)
		m_nAllocSize = 32;

	while (m_nAllocSize < m_nElementCount + nAdd)
		m_nAllocSize *= 2;

	TYPE* pNew = new TYPE[m_nAllocSize];
	if (pNew == NULL)
		return FALSE; // out of memory

	if (m_pData != NULL)
	{
		for (int i = 0; i < m_nElementCount; i++)
			pNew[i] = m_pData[i];
		delete [] m_pData;		
	}
	m_pData = pNew;
	return TRUE;
}

template <class TYPE, class ARG_TYPE>
BOOL CArrayEx<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE tData)
{
	if (!IsIndexValid(nIndex))
		return FALSE;
	
	m_pData[nIndex] = tData;
	m_nSort = AE_SORT_NONE;
	return TRUE;
}

template <class TYPE, class ARG_TYPE>
TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{
	ASSERT(IsIndexValid(nIndex));
	m_nSort = AE_SORT_NONE;
	return m_pData[nIndex];
}

template <class TYPE, class ARG_TYPE>
const TYPE& CArrayEx<TYPE, ARG_TYPE>::GetAt(int nIndex) const
{
	ASSERT(IsIndexValid(nIndex));
	return m_pData[nIndex];
}

template <class TYPE, class ARG_TYPE>
const CArrayEx<TYPE, ARG_TYPE>& CArrayEx<TYPE, ARG_TYPE>::operator=(const CArrayEx& src)
{
	if (!Copy(src))
		ASSERT(FALSE);
	return *this;
}

#endif // __ARRAYEX_H__