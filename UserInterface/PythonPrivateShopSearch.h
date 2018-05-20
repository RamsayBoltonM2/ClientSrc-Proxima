#pragma once

class CPythonPrivateShopSearch : public CSingleton<CPythonPrivateShopSearch>
{
	public:
		struct TSearchItemData : TItemData
		{
			DWORD vid;
			DWORD price;
#ifdef WJ_CHEQUE_SYSTEM
			DWORD price2;
#endif
			BYTE Cell;
		};

		using TItemInstanceVector =  std::vector<TSearchItemData> ;

	public:
		CPythonPrivateShopSearch();
		virtual ~CPythonPrivateShopSearch();

		void AddItemData (DWORD vid, DWORD price, const TSearchItemData& rItemData);
		void ClearItemData();

		DWORD GetItemDataCount()
		{
			return m_ItemInstanceVector.size();
		}
		DWORD GetItemDataPtr (DWORD index, TSearchItemData** ppInstance);

	protected:
		TItemInstanceVector m_ItemInstanceVector;
};