#pragma once

#include "Packet.h"

/*
 *	상점 처리
 *
 *	2003-01-16 anoa	일차 완료
 *	2003-12-26 levites 수정
 *
 *	2012-10-29 rtsummit 새로운 화폐 출현 및 tab 기능 추가로 인한 shop 확장.
 *
 */
typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#ifdef WJ_EXTENDED_SHOP_SYSTEM
typedef enum
{
	SHOP_TYPE_GOLD,	// DEFULT VALUE
	SHOP_TYPE_CASH,	// account.cash
	SHOP_TYPE_COINS,	// account.coins
	SHOP_TYPE_ALIGN,	// player.alignment
#ifdef WJ_NATIONAL_POINT_SYSTEM
	SHOP_TYPE_NATIONAL_POINT,
#endif
} EShopType;
#endif

class CPythonShop : public CSingleton<CPythonShop>
{
	public:
		CPythonShop(void);
		virtual ~CPythonShop(void);

		void Clear();
		
		void SetItemData(DWORD dwIndex, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(DWORD dwIndex, const TShopItemData ** c_ppItemData);

		void SetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData ** c_ppItemData);
		
#ifdef WJ_OFFLINESHOP_SYSTEM
		void SetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData & c_rShopItemData);
		BOOL GetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData ** c_ppItemData);

		void SetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData & c_rShopItemData);
		BOOL GetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData ** c_ppItemData);
#endif
		
		void SetTabCount(BYTE bTabCount) { m_bTabCount = bTabCount; }
		BYTE GetTabCount() { return m_bTabCount; }

		void SetTabCoinType(BYTE tabIdx, BYTE coinType);
		BYTE GetTabCoinType(BYTE tabIdx);

		void SetTabName(BYTE tabIdx, const char* name);
		const char* GetTabName(BYTE tabIdx);
		
#ifdef WJ_OFFLINESHOP_SYSTEM
		void SetShopDisplayedCount(DWORD dwDisplayedCount);
		DWORD	GetShopDisplayedCount();
#endif

		//BOOL GetSlotItemID(DWORD dwSlotPos, DWORD* pdwItemID);

#ifdef WJ_OFFLINESHOP_SYSTEM
		void Open(BOOL isPrivateShop, BOOL isMainPrivateShop, BOOL isOfflineShop);
#else
		void Open(BOOL isPrivateShop, BOOL isMainPrivateShop);	
#endif
		void Close();
		BOOL IsOpen();
		BOOL IsPrivateShop();
		BOOL IsMainPlayerPrivateShop();
#ifdef WJ_OFFLINESHOP_SYSTEM
		BOOL IsOfflineShop();
#endif

#ifdef WJ_EXTENDED_SHOP_SYSTEM
		void SetShopName(const char* name);
		void SetPriceType(short price_type);
		const char* GetShopName();
		short GetPriceType();
#endif

		void ClearPrivateShopStock();
#if defined(WJ_CHEQUE_SYSTEM)
		void AddPrivateShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, DWORD dwPrice, DWORD dwPrice2);
#else
		void AddPrivateShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, DWORD dwPrice);
#endif
		void DelPrivateShopItemStock(TItemPos ItemPos);
		int GetPrivateShopItemPrice(TItemPos ItemPos);
#if defined(WJ_CHEQUE_SYSTEM)
		int GetPrivateShopItemPrice2(TItemPos ItemPos);
#endif
#ifdef WJ_SHOPSTYLE_SYSTEM
		void BuildPrivateShop(const char * c_szName, BYTE bNpcType);
#else
		void BuildPrivateShop(const char * c_szName);
#endif
		
#ifdef WJ_OFFLINESHOP_SYSTEM
		void ClearOfflineShopStock();
#ifdef WJ_CHEQUE_SYSTEM
		void AddOfflineShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, DWORD dwPrice, DWORD dwPrice2);
#else
		void AddOfflineShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, DWORD dwPrice);
#endif
		void DelOfflineShopItemStock(TItemPos ItemPos);
		int	 GetOfflineShopItemPrice(TItemPos ItemPos);
#ifdef WJ_CHEQUE_SYSTEM
		int	 GetOfflineShopItemPrice2(TItemPos ItemPos);
#endif
		BYTE	GetOfflineShopItemStatus(TItemPos ItemPos);
		void BuildOfflineShop(const char * c_szName, BYTE bNpcVnum);
#endif

	protected:
		BOOL	CheckSlotIndex(DWORD dwIndex);

	protected:
		BOOL				m_isShoping;
		BOOL				m_isPrivateShop;
		BOOL				m_isMainPlayerPrivateShop;
		BOOL				m_isOfflineShop;

		struct ShopTab
		{
			ShopTab()
			{
				coinType = SHOP_COIN_TYPE_GOLD;
			}
			BYTE				coinType;
			std::string			name;
			TShopItemData		items[SHOP_HOST_ITEM_MAX_NUM];
		};
		
#ifdef WJ_OFFLINESHOP_SYSTEM
		struct OfflineShopTab
		{
			OfflineShopTab()
			{
				coinType = SHOP_COIN_TYPE_GOLD;
			}
			BYTE				coinType;
			std::string			name;
			TOfflineShopItemData items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
		};
		
		DWORD				m_dwDisplayedCount;
#endif
		
		BYTE m_bTabCount;
		ShopTab m_aShoptabs[SHOP_TAB_COUNT_MAX];
#ifdef WJ_OFFLINESHOP_SYSTEM
		OfflineShopTab m_aOfflineShoptabs[SHOP_TAB_COUNT_MAX];
#endif

#ifdef WJ_EXTENDED_SHOP_SYSTEM
		std::string			shop_name;
		short				sPrice_type;
#endif

		typedef std::map<TItemPos, TShopItemTable> TPrivateShopItemStock;
		TPrivateShopItemStock	m_PrivateShopItemStock;
		
#ifdef WJ_OFFLINESHOP_SYSTEM
		typedef std::map<TItemPos, TOfflineShopItemTable> TOfflineShopItemStock;
		TOfflineShopItemStock	m_OfflineShopItemStock;
#endif
};
