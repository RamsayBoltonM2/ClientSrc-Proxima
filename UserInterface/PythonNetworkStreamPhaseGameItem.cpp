#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonItem.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonSafeBox.h"
#include "PythonCharacterManager.h"
#include "PythonSystem.h"
#include "PythonPlayer.h"
#include "AbstractPlayer.h"
#include "PythonPrivateShopSearch.h"

//////////////////////////////////////////////////////////////////////////
// SafeBox

bool CPythonNetworkStream::SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney)
{
	assert(!"CPythonNetworkStream::SendSafeBoxMoneyPacket - 사용하지 않는 함수");
	return false;

//	TPacketCGSafeboxMoney kSafeboxMoney;
//	kSafeboxMoney.bHeader = HEADER_CG_SAFEBOX_MONEY;
//	kSafeboxMoney.bState = byState;
//	kSafeboxMoney.dwMoney = dwMoney;
//	if (!Send(sizeof(kSafeboxMoney), &kSafeboxMoney))
//		return false;
//
//	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = bySafeBoxPos;
	if (!Send(sizeof(kSafeboxCheckin), &kSafeboxCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos)
{
	__PlaySafeBoxItemDropSound(bySafeBoxPos);

	TPacketCGSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = bySafeBoxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
	if (!Send(sizeof(kSafeboxCheckout), &kSafeboxCheckout))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount)
{
	__PlaySafeBoxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(INVENTORY, bySourcePos);
	kItemMove.num = byCount;
	kItemMove.change_pos = TItemPos(INVENTORY, byTargetPos);
	if (!Send(sizeof(kItemMove), &kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSafeBoxSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum	= kItemSet.vnum;
	kItemData.count = kItemSet.count;
#ifdef WJ_SOULBINDING_SYSTEM
	kItemData.bind = kItemSet.bind;
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
	kItemData.transmutation = kItemSet.transmutation;
#endif
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];
	
	CPythonSafeBox::Instance().SetItemData(kItemSet.Cell.cell, kItemData);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelItemData(kItemDel.pos);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxWrongPasswordPacket()
{
	TPacketGCSafeboxWrongPassword kSafeboxWrongPassword;

	if (!Recv(sizeof(kSafeboxWrongPassword), &kSafeboxWrongPassword))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnSafeBoxError", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxSizePacket()
{
	TPacketGCSafeboxSize kSafeBoxSize;
	if (!Recv(sizeof(kSafeBoxSize), &kSafeBoxSize))
		return false;

	CPythonSafeBox::Instance().OpenSafeBox(kSafeBoxSize.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSafeboxWindow", Py_BuildValue("(i)", kSafeBoxSize.bSize));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxMoneyChangePacket()
{
	TPacketGCSafeboxMoneyChange kMoneyChange;
	if (!Recv(sizeof(kMoneyChange), &kMoneyChange))
		return false;

	CPythonSafeBox::Instance().SetMoney(kMoneyChange.dwMoney);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafeboxMoney", Py_BuildValue("()"));

	return true;
}

// SafeBox
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mall
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos)
{
	__PlayMallItemDropSound(byMallPos);

	TPacketCGMallCheckout kMallCheckoutPacket;
	kMallCheckoutPacket.bHeader = HEADER_CG_MALL_CHECKOUT;
	kMallCheckoutPacket.bMallPos = byMallPos;
	kMallCheckoutPacket.ItemPos = InventoryPos;
	if (!Send(sizeof(kMallCheckoutPacket), &kMallCheckoutPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMallOpenPacket()
{
	TPacketGCMallOpen kMallOpen;
	if (!Recv(sizeof(kMallOpen), &kMallOpen))
		return false;

	CPythonSafeBox::Instance().OpenMall(kMallOpen.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMallWindow", Py_BuildValue("(i)", kMallOpen.bSize));

	return true;
}
bool CPythonNetworkStream::RecvMallItemSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum = kItemSet.vnum;
	kItemData.count = kItemSet.count;
#ifdef WJ_SOULBINDING_SYSTEM
	kItemData.bind = kItemSet.bind;
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
	kItemData.transmutation = kItemSet.transmutation;
#endif
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];
	
	CPythonSafeBox::Instance().SetMallItemData(kItemSet.Cell.cell, kItemData);

	__RefreshMallWindow();

	return true;
}
bool CPythonNetworkStream::RecvMallItemDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelMallItemData(kItemDel.pos);

	__RefreshMallWindow();
	Tracef(" >> CPythonNetworkStream::RecvMallItemDelPacket\n");

	return true;
}
// Mall
//////////////////////////////////////////////////////////////////////////

// Item
// Recieve
bool CPythonNetworkStream::RecvItemSetPacket()
{
	TPacketGCItemSet packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
#ifdef WJ_SOULBINDING_SYSTEM
	kItemData.bind = packet_item_set.bind;
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
	kItemData.transmutation = packet_item_set.transmutation;
#endif
	kItemData.flags = 0;
	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
	
	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemSetPacket2()
{
	TPacketGCItemSet2 packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet2), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
#ifdef WJ_SOULBINDING_SYSTEM
	kItemData.bind = packet_item_set.bind;
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
	kItemData.transmutation = packet_item_set.transmutation;
#endif
	kItemData.flags = packet_item_set.flags;
	kItemData.anti_flags = packet_item_set.anti_flags;

	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
	
	if (packet_item_set.highlight && CPythonSystem::Instance().IsShowHighlight())
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Highlight_Item", Py_BuildValue("(ii)", packet_item_set.Cell.window_type, packet_item_set.Cell.cell));
	
	__RefreshInventoryWindow();
	return true;
}


bool CPythonNetworkStream::RecvItemUsePacket()
{
	TPacketGCItemUse packet_item_use;

	if (!Recv(sizeof(TPacketGCItemUse), &packet_item_use))
		return false;

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUpdatePacket()
{
	TPacketGCItemUpdate packet_item_update;

	if (!Recv(sizeof(TPacketGCItemUpdate), &packet_item_update))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemCount(packet_item_update.Cell, packet_item_update.count);
#ifdef WJ_SOULBINDING_SYSTEM
	rkPlayer.SetItemBind(packet_item_update.Cell, packet_item_update.bind);
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
	rkPlayer.SetItemTransmutation(packet_item_update.Cell, packet_item_update.transmutation);
#endif
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		rkPlayer.SetItemMetinSocket(packet_item_update.Cell, i, packet_item_update.alSockets[i]);
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		rkPlayer.SetItemAttribute(packet_item_update.Cell, j, packet_item_update.aAttr[j].bType, packet_item_update.aAttr[j].sValue);

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemGroundAddPacket()
{
	TPacketGCItemGroundAdd packet_item_ground_add;

	if (!Recv(sizeof(TPacketGCItemGroundAdd), &packet_item_ground_add))
		return false;

	__GlobalPositionToLocalPosition(packet_item_ground_add.lX, packet_item_ground_add.lY);

#ifdef WJ_SKILLBOOK_SYSTEM
	CPythonItem::Instance().CreateItem(packet_item_ground_add.dwVID, 
									   packet_item_ground_add.dwVnum,
									   packet_item_ground_add.alSockets[0],
									   packet_item_ground_add.alSockets[1],
									   packet_item_ground_add.alSockets[2],
									   packet_item_ground_add.count,
									   packet_item_ground_add.lX,
									   packet_item_ground_add.lY,
									   packet_item_ground_add.lZ);
#else
	CPythonItem::Instance().CreateItem(packet_item_ground_add.dwVID, 
									   packet_item_ground_add.dwVnum,
									   packet_item_ground_add.lX,
									   packet_item_ground_add.lY,
									   packet_item_ground_add.lZ);
#endif
	return true;
}


bool CPythonNetworkStream::RecvItemOwnership()
{
	TPacketGCItemOwnership p;

	if (!Recv(sizeof(TPacketGCItemOwnership), &p))
		return false;

	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName);
	return true;
}

bool CPythonNetworkStream::RecvItemGroundDelPacket()
{
	TPacketGCItemGroundDel	packet_item_ground_del;

	if (!Recv(sizeof(TPacketGCItemGroundDel), &packet_item_ground_del))
		return false;

	CPythonItem::Instance().DeleteItem(packet_item_ground_del.vid);
	return true;
}

bool CPythonNetworkStream::RecvQuickSlotAddPacket()
{
	TPacketGCQuickSlotAdd packet_quick_slot_add;

	if (!Recv(sizeof(TPacketGCQuickSlotAdd), &packet_quick_slot_add))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.AddQuickSlot(packet_quick_slot_add.pos, packet_quick_slot_add.slot.Type, packet_quick_slot_add.slot.Position);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotDelPacket()
{
	TPacketGCQuickSlotDel packet_quick_slot_del;

	if (!Recv(sizeof(TPacketGCQuickSlotDel), &packet_quick_slot_del))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.DeleteQuickSlot(packet_quick_slot_del.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotMovePacket()
{
	TPacketGCQuickSlotSwap packet_quick_slot_swap;

	if (!Recv(sizeof(TPacketGCQuickSlotSwap), &packet_quick_slot_swap))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.MoveQuickSlot(packet_quick_slot_swap.pos, packet_quick_slot_swap.change_pos);

	__RefreshInventoryWindow();

	return true;
}



bool CPythonNetworkStream::SendShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount=1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSellPacket(BYTE bySlot)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef WJ_SELLING_DRAGONSOUL
bool CPythonNetworkStream::SendShopSellPacketNew(WORD wSlot, BYTE byCount, BYTE byType)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Errorn");
		return false;
	}
	if (!Send(sizeof(WORD), &wSlot))
	{
		Tracef("SendShopAddSellPacket Errorn");
		return false;
	}
	if (!Send(sizeof(BYTE), &byCount))
	{
		Tracef("SendShopAddSellPacket Errorn");
		return false;
	}
	if (!Send(sizeof(BYTE), &byType))
	{
		Tracef("SendShopAddSellPacket Errorn");
		return false;
	}

	Tracef(" SendShopSellPacketNew(wSlot=%d, byCount=%d, byType=%d)n", wSlot, byCount, byType);

	return SendSequence();
}
#else
bool CPythonNetworkStream::SendShopSellPacketNew(BYTE bySlot, BYTE byCount)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &byCount))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	Tracef(" SendShopSellPacketNew(bySlot=%d, byCount=%d)\n", bySlot, byCount);

	return SendSequence();
}
#endif

#ifdef WJ_OFFLINESHOP_SYSTEM
bool CPythonNetworkStream::SendOfflineShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_OFFLINE_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendOfflineShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount = 1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendOfflineShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddOfflineShopItem(TItemPos bDisplayPos, BYTE bPos, int lPrice, int iPrice2)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_ADD_ITEM;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendAddOfflineShopItem Error\n");
		return false;
	}

	TOfflineShopItemTable2 pTable;
	
	pTable.bDisplayPos = bDisplayPos;
	pTable.bPos = bPos;
	pTable.lPrice = lPrice;
	pTable.lPrice2 = iPrice2;

	if (!Send(sizeof(TOfflineShopItemTable2), &pTable))
	{
		Tracef("SendAddOfflineShopItem Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRemoveOfflineShopItem(BYTE bPos)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REMOVE_ITEM;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRemoveOfflineShopItem Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendRemoveOfflineShopItem Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDestroyOfflineShop()
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendDestroyOfflineShop Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshOfflineShop()
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshOfflineShop Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshUnsoldItems()
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH_UNSOLD_ITEMS;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshUnsoldItems Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRefreshOfflineShopMoney()
{
	static DWORD s_LastTime = timeGetTime() - 5001;

	if (timeGetTime() - s_LastTime < 5000)
		return true;

	s_LastTime = timeGetTime();

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_REFRESH_MONEY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshOfflineShopMoney Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopWithdrawMoney(DWORD llMoney)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_WITHDRAW_MONEY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopWithdrawMoney Packet Error\n");
		return false;
	}

	if (!Send(sizeof(llMoney), &llMoney))
	{
		Tracef("SendOfflineShopWithdrawMoney Packet Error \n");
		return false;
	}

	return SendSequence();
}

#ifdef WJ_CHEQUE_SYSTEM
bool CPythonNetworkStream::SendOfflineShopWithdrawCheque(DWORD dwCheque)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopWithdrawCheque Packet Error\n");
		return false;
	}

	if (!Send(sizeof(dwCheque), &dwCheque))
	{
		Tracef("SendOfflineShopWithdrawCheque Packet Error \n");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendOfflineShopTakeItem(BYTE bDisplayPos)
{
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_TAKE_ITEM;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopTakeItem Packet error\n");
		return false;
	}

	if (!Send(sizeof(bDisplayPos), &bDisplayPos))
	{
		Tracef("SendOfflineShopTakeItem Packet error\n");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopTeleport()
{	
	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_TELEPORT;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendRefreshUnsoldItems Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOfflineShopCheck()
{
	static DWORD s_LastTime = timeGetTime() - 5001;

	if (timeGetTime() - s_LastTime < 5000)
		return true;

	s_LastTime = timeGetTime();

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_OFFLINE_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_CHECK;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendOfflineShopCheck Packet error\n");
		return false;
	}

	return SendSequence();
}
#endif

// Send
bool CPythonNetworkStream::SendItemUsePacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGItemUse itemUsePacket;
	itemUsePacket.header = HEADER_CG_ITEM_USE;
	itemUsePacket.pos = pos;

	if (!Send(sizeof(TPacketCGItemUse), &itemUsePacket))
	{
		Tracen("SendItemUsePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
	if (!__CanActMainInstance())
		return true;	

	TPacketCGItemUseToItem itemUseToItemPacket;
	itemUseToItemPacket.header = HEADER_CG_ITEM_USE_TO_ITEM;
	itemUseToItemPacket.source_pos = source_pos;
	itemUseToItemPacket.target_pos = target_pos;

	if (!Send(sizeof(TPacketCGItemUseToItem), &itemUseToItemPacket))
	{
		Tracen("SendItemUseToItemPacket Error");
		return false;
	}

#ifdef _DEBUG
	Tracef(" << SendItemUseToItemPacket(src=%d, dst=%d)\n", source_pos, target_pos);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos, DWORD elk)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP;
	itemDropPacket.pos = pos;
	itemDropPacket.elk = elk;

	if (!Send(sizeof(TPacketCGItemDrop), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop2 itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP2;
	itemDropPacket.pos = pos;
	itemDropPacket.gold = elk;
	itemDropPacket.count = count;

	if (!Send(sizeof(itemDropPacket), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

#ifdef WJ_NEW_DROP_DIALOG
bool CPythonNetworkStream::SendItemDestroyPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGItemDestroy itemDestroyPacket;
	itemDestroyPacket.header = HEADER_CG_ITEM_DESTROY;
	itemDestroyPacket.pos = pos;
	
	if (!Send(sizeof(itemDestroyPacket), &itemDestroyPacket))
	{
		Tracen("SendItemDestroyPacket Error");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendItemSellPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGItemSell itemSellPacket;
	itemSellPacket.header = HEADER_CG_ITEM_SELL;
	itemSellPacket.pos = pos;
	
	if (!Send(sizeof(itemSellPacket), &itemSellPacket))
	{
		Tracen("SendItemSellPacket Error");
		return false;
	}
	
	return SendSequence();
}
#endif

bool CPythonNetworkStream::__IsEquipItemInSlot(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	return rkPlayer.IsEquipItemInSlot(uSlotPos);
}

void CPythonNetworkStream::__PlayInventoryItemUseSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayUseSound(dwItemID);
}

void CPythonNetworkStream::__PlayInventoryItemDropSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

//void CPythonNetworkStream::__PlayShopItemDropSound(UINT uSlotPos)
//{
//	DWORD dwItemID;
//	CPythonShop& rkShop=CPythonShop::Instance();
//	if (!rkShop.GetSlotItemID(uSlotPos, &dwItemID))
//		return;
//	
//	CPythonItem& rkItem=CPythonItem::Instance();
//	rkItem.PlayDropSound(dwItemID);
//}

void CPythonNetworkStream::__PlaySafeBoxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlayMallItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotMallItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, BYTE num)
{	
	if (!__CanActMainInstance())
		return true;
	
	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
				return true;
			}
		}

		if (CPythonShop::Instance().IsOpen())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
				return true;
			}
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemDropSound(pos);

	TPacketCGItemMove	itemMovePacket;
	itemMovePacket.header = HEADER_CG_ITEM_MOVE;
	itemMovePacket.pos = pos;
	itemMovePacket.change_pos = change_pos;
	itemMovePacket.num = num;

	if (!Send(sizeof(TPacketCGItemMove), &itemMovePacket))
	{
		Tracen("SendItemMovePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPickUpPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemPickUp	itemPickUpPacket;
	itemPickUpPacket.header = HEADER_CG_ITEM_PICKUP;
	itemPickUpPacket.vid = vid;

	if (!Send(sizeof(TPacketCGItemPickUp), &itemPickUpPacket))
	{
		Tracen("SendItemPickUpPacket Error");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendQuickSlotAddPacket(WORD wpos, BYTE type, WORD pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotAdd quickSlotAddPacket;

	quickSlotAddPacket.header		= HEADER_CG_QUICKSLOT_ADD;
	quickSlotAddPacket.pos			= wpos;
	quickSlotAddPacket.slot.Type	= type;
	quickSlotAddPacket.slot.Position = pos;

	if (!Send(sizeof(TPacketCGQuickSlotAdd), &quickSlotAddPacket))
	{
		Tracen("SendQuickSlotAddPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotDelPacket(WORD pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotDel quickSlotDelPacket;

	quickSlotDelPacket.header = HEADER_CG_QUICKSLOT_DEL;
	quickSlotDelPacket.pos = pos;

	if (!Send(sizeof(TPacketCGQuickSlotDel), &quickSlotDelPacket))
	{
		Tracen("SendQuickSlotDelPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotMovePacket(WORD pos, WORD change_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotSwap quickSlotSwapPacket;

	quickSlotSwapPacket.header = HEADER_CG_QUICKSLOT_SWAP;
	quickSlotSwapPacket.pos = pos;
	quickSlotSwapPacket.change_pos = change_pos;

	if (!Send(sizeof(TPacketCGQuickSlotSwap), &quickSlotSwapPacket))
	{
		Tracen("SendQuickSlotSwapPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvSpecialEffect()
{
	TPacketGCSpecialEffect kSpecialEffect;
	if (!Recv(sizeof(kSpecialEffect), &kSpecialEffect))
		return false;

	DWORD effect = -1;
	bool bPlayPotionSound = false;	//포션을 먹을 경우는 포션 사운드를 출력하자.!!
	bool bAttachEffect = true;		//캐리터에 붙는 어태치 이펙트와 일반 이펙트 구분.!!
	switch (kSpecialEffect.type)
	{
		case SE_HPUP_RED:
			effect = CInstanceBase::EFFECT_HPUP_RED;
			bPlayPotionSound = true;
			break;
		case SE_SPUP_BLUE:
			effect = CInstanceBase::EFFECT_SPUP_BLUE;
			bPlayPotionSound = true;
			break;
		case SE_SPEEDUP_GREEN:
			effect = CInstanceBase::EFFECT_SPEEDUP_GREEN;
			bPlayPotionSound = true;
			break;
		case SE_DXUP_PURPLE:
			effect = CInstanceBase::EFFECT_DXUP_PURPLE;
			bPlayPotionSound = true;
			break;
		case SE_CRITICAL:
			effect = CInstanceBase::EFFECT_CRITICAL;
			break;
		case SE_PENETRATE:
			effect = CInstanceBase::EFFECT_PENETRATE;
			break;
		case SE_BLOCK:
			effect = CInstanceBase::EFFECT_BLOCK;
			break;
		case SE_DODGE:
			effect = CInstanceBase::EFFECT_DODGE;
			break;
		case SE_CHINA_FIREWORK:
			effect = CInstanceBase::EFFECT_FIRECRACKER;
			bAttachEffect = false;
			break;
		case SE_SPIN_TOP:
			effect = CInstanceBase::EFFECT_SPIN_TOP;
			bAttachEffect = false;
			break;
		case SE_SUCCESS :
			effect = CInstanceBase::EFFECT_SUCCESS ;
			bAttachEffect = false ;
			break ;
		case SE_FAIL :
			effect = CInstanceBase::EFFECT_FAIL ;
			break ;
		case SE_FR_SUCCESS:
			effect = CInstanceBase::EFFECT_FR_SUCCESS;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_ON_14_FOR_GERMANY:	//레벨업 14일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_UNDER_15_FOR_GERMANY: //레벨업 15일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_PERCENT_DAMAGE1:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE1;
			break;
		case SE_PERCENT_DAMAGE2:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE2;
			break;
		case SE_PERCENT_DAMAGE3:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE3;
			break;
		case SE_AUTO_HPUP:
			effect = CInstanceBase::EFFECT_AUTO_HPUP;
			break;
		case SE_AUTO_SPUP:
			effect = CInstanceBase::EFFECT_AUTO_SPUP;
			break;
		case SE_EQUIP_RAMADAN_RING:
			effect = CInstanceBase::EFFECT_RAMADAN_RING_EQUIP;
			break;
		case SE_EQUIP_HALLOWEEN_CANDY:
			effect = CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP;
			break;
		case SE_EQUIP_HAPPINESS_RING:
 			effect = CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP;
			break;
		case SE_EQUIP_LOVE_PENDANT:
			effect = CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP;
			break;
#ifdef WJ_ACCE_SYSTEM
		case SE_ACCE_SUCESS_ABSORB:
			effect = CInstanceBase::EFFECT_ACCE_SUCESS_ABSORB;
			break;
		case SE_ACCE_EQUIP:
			effect = CInstanceBase::EFFECT_ACCE_EQUIP;
			break;
		case SE_ACCE_BACK:
			effect = CInstanceBase::EFFECT_ACCE_BACK;
			break;
#endif
#ifdef ENABLE_PVP_EFFECT
		case SE_PVP_WIN:
			effect = CInstanceBase::EFFECT_PVP_WIN;
			break;
		case SE_PVP_OPEN1:
			effect = CInstanceBase::EFFECT_PVP_OPEN1;
			break;
		case SE_PVP_OPEN2:
			effect = CInstanceBase::EFFECT_PVP_OPEN2;
			break;
		case SE_PVP_BEGIN1:
			effect = CInstanceBase::EFFECT_PVP_BEGIN1;
			break;
		case SE_PVP_BEGIN2:
			effect = CInstanceBase::EFFECT_PVP_BEGIN2;
			break;
#endif
#ifdef WJ_NATIONAL_POINT_SYSTEM
		case SE_NATIONAL_POINT:
			effect = CInstanceBase::EFFECT_NATIONAL_POINT;
			break;
#endif
		case SE_EQUIP_DEADLY_POWER_RING:
			effect = CInstanceBase::EFFECT_DEADLY_POWER_RING;
			break;
		case SE_EQUIP_WILL_POWER_RING:
			effect = CInstanceBase::EFFECT_WILL_POWER_RING;
			break;
		case SE_EQUIP_MEDAL_OF_HEROISM:
			effect = CInstanceBase::EFFECT_MEDAL_OF_HEROISM;
			break;
		case SE_EQUIP_MAGIC_CANDY:
			effect = CInstanceBase::EFFECT_MAGIC_CANDY;
			break;
		case SE_EQUIP_CHOCOLATE_CHARM:
			effect = CInstanceBase::EFEFCT_CHOCOLATE_CHARM;
			break;
		case SE_EQUIP_EVIL_EYE_CHARM:
			effect = CInstanceBase::EFFECT_EVIL_EYE_CHARM;
			break;
#ifdef WJ_TEMPLE_OCHAO
		case SE_EFFECT_HEALER:
			effect = CInstanceBase::EFFECT_HEALER;
			break;
#endif
		
		default:
			TraceError("%d 는 없는 스페셜 이펙트 번호입니다.TPacketGCSpecialEffect",kSpecialEffect.type);
			break;
	}

	if (bPlayPotionSound)
	{		
		IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
		if(rkPlayer.IsMainCharacterIndex(kSpecialEffect.vid))
		{
			CPythonItem& rkItem=CPythonItem::Instance();
			rkItem.PlayUsePotionSound();
		}
	}

	if (-1 != effect)
	{
		CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
		if (pInstance)
		{
			if(bAttachEffect)
				pInstance->AttachSpecialEffect(effect);
			else
				pInstance->CreateSpecialEffect(effect);
		}
	}

	return true;
}


bool CPythonNetworkStream::RecvSpecificEffect()
{
	TPacketGCSpecificEffect kSpecificEffect;
	if (!Recv(sizeof(kSpecificEffect), &kSpecificEffect))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecificEffect.vid);
	//EFFECT_TEMP
	if (pInstance)
	{
		CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_TEMP, "", kSpecificEffect.effect_file, false);
		pInstance->AttachSpecialEffect(CInstanceBase::EFFECT_TEMP);
	}

	return true;
}

bool CPythonNetworkStream::RecvDragonSoulRefine()
{
	TPacketGCDragonSoulRefine kDragonSoul;

	if (!Recv(sizeof(kDragonSoul), &kDragonSoul))
		return false;
	
	
	switch (kDragonSoul.bSubType)
	{
	case DS_SUB_HEADER_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("()"));
		break;
	case DS_SUB_HEADER_REFINE_FAIL:
	case DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE:
	case DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineFail", Py_BuildValue("(iii)", 
			kDragonSoul.bSubType, kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	case DS_SUB_HEADER_REFINE_SUCCEED:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineSucceed", 
				Py_BuildValue("(ii)", kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	}

	return true;
}

#ifdef WJ_SHOP_SEARCH_SYSTEM
#ifdef WJ_CHEQUE_SYSTEM
bool CPythonNetworkStream::SendPrivateShopSearchInfo (int32_t Race, int32_t ItemCat, int32_t SubCat, int32_t MinLevel, int32_t MaxLevel, int32_t MinRefine, int32_t MaxRefine, uint64_t MinGold, uint64_t MaxGold, uint64_t MinCheque, uint64_t MaxCheque)	
#else
bool CPythonNetworkStream::SendPrivateShopSearchInfo (int32_t Race, int32_t ItemCat, int32_t SubCat, int32_t MinLevel, int32_t MaxLevel, int32_t MinRefine, int32_t MaxRefine, uint64_t MinGold, uint64_t MaxGold)
#endif
{

	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGShopSearch ShopSearchPacket;
	ShopSearchPacket.header = HEADER_CG_SHOP_SEARCH;
	ShopSearchPacket.Race = Race;
	ShopSearchPacket.ItemCat = ItemCat;
	ShopSearchPacket.SubCat = SubCat;
	ShopSearchPacket.MinLevel = MinLevel;
	ShopSearchPacket.MaxLevel = MaxLevel;
	ShopSearchPacket.MinRefine = MinRefine;
	ShopSearchPacket.MaxRefine = MaxRefine;
	ShopSearchPacket.MinGold = MinGold;
	ShopSearchPacket.MaxGold = MaxGold;
#ifdef WJ_CHEQUE_SYSTEM
	ShopSearchPacket.MinCheque = MinCheque;
	ShopSearchPacket.MaxCheque = MaxCheque;
#endif

	if (!Send (sizeof (ShopSearchPacket), &ShopSearchPacket))
	{
		Tracen ("SendPrivateShopSearchInfo Error");
		return false;
	}

	return true;
}

#ifdef WJ_CHEQUE_SYSTEM
bool CPythonNetworkStream::SendPrivateShopSearchInfoSub (int32_t Race, int32_t ItemCat, int32_t SubCat, int32_t MinLevel, int32_t MaxLevel, int32_t MinRefine, int32_t MaxRefine, uint64_t MinGold, uint64_t MaxGold, uint64_t MinCheque, uint64_t MaxCheque, char* ItemName)
#else
bool CPythonNetworkStream::SendPrivateShopSearchInfoSub (int32_t Race, int32_t ItemCat, int32_t SubCat, int32_t MinLevel, int32_t MaxLevel, int32_t MinRefine, int32_t MaxRefine, uint64_t MinGold, uint64_t MaxGold, char* ItemName)
#endif
{

	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGShopSearch ShopSearchPacket;
	ShopSearchPacket.header = HEADER_CG_SHOP_SEARCH_SUB;
	ShopSearchPacket.Race = Race;
	ShopSearchPacket.ItemCat = ItemCat;
	ShopSearchPacket.SubCat = SubCat;
	ShopSearchPacket.MinLevel = MinLevel;
	ShopSearchPacket.MaxLevel = MaxLevel;
	ShopSearchPacket.MinRefine = MinRefine;
	ShopSearchPacket.MaxRefine = MaxRefine;
	ShopSearchPacket.MinGold = MinGold;
	ShopSearchPacket.MaxGold = MaxGold;
#ifdef WJ_CHEQUE_SYSTEM
	ShopSearchPacket.MinCheque = MinCheque;
	ShopSearchPacket.MaxCheque = MaxCheque;
#endif
	strncpy_s (ShopSearchPacket.ItemName, ItemName, sizeof (ShopSearchPacket.ItemName) - 1);

	if (!Send (sizeof (ShopSearchPacket), &ShopSearchPacket))
	{
		Tracen ("SendPrivateShopSearchInfoSub Error");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::SendPrivateShopSerchBuyItem (int32_t shopVid, BYTE shopItemPos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGShopSearchBuy PacketShopSearchBuy;
	PacketShopSearchBuy.header = HEADER_CG_SHOP_SEARCH_BUY;
	PacketShopSearchBuy.shopVid = shopVid;
	PacketShopSearchBuy.shopItemPos = shopItemPos;

	if (!Send (sizeof (TPacketCGShopSearchBuy), &PacketShopSearchBuy))
	{
		Tracef ("SendPrivateShopSerchBuyItem Error\n");
		return false;
	}

	Tracef ("SendPrivateShopSerchBuyItem: ShopVid: %d  Pos %d", shopVid, shopItemPos);

	return true;
}

bool CPythonNetworkStream::RecvShopSearchSet()
{
	TPacketGCShopSearchItemSet packet_item_set;

	if (!Recv (sizeof (TPacketGCShopSearchItemSet), &packet_item_set))
	{
		return false;
	}
	CPythonPrivateShopSearch::TSearchItemData searchItem;
	searchItem.vnum = packet_item_set.vnum;
	searchItem.count = packet_item_set.count;
	searchItem.flags = packet_item_set.flags;
	searchItem.anti_flags = packet_item_set.anti_flags;

	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
	{
		searchItem.alSockets[i] = packet_item_set.alSockets[i];
	}
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
	{
		searchItem.aAttr[j] = packet_item_set.aAttr[j];
	}

	searchItem.vid = packet_item_set.vid;
	searchItem.price = packet_item_set.price;
#ifdef WJ_CHEQUE_SYSTEM
	searchItem.price2 = packet_item_set.price2;
#endif
	searchItem.Cell = packet_item_set.Cell;

#ifdef WJ_CHANGELOOK_SYSTEM
	searchItem.transmutation = packet_item_set.transmutation;
#endif
	
	CPythonPrivateShopSearch::Instance().AddItemData (packet_item_set.vid, packet_item_set.price, searchItem);

	__RefreshShopSearchWindow();
	return true;
}
#endif

#ifdef WJ_ACCE_SYSTEM
bool CPythonNetworkStream::RecvAccePacket()
{


	TPacketGCAcce AccePacket;
	if (!Recv(sizeof(AccePacket), &AccePacket))
		return false;

	switch (AccePacket.subheader)
	{
	case ACCE_SUBHEADER_GC_SET_ITEM:
	{
		TPacketGCItemSet2 kItemSet;
		if (!Recv(sizeof(kItemSet), &kItemSet))
			return false;

		TItemData kItemData;
		kItemData.vnum = kItemSet.vnum;
		kItemData.count = kItemSet.count;
		kItemData.flags = kItemSet.flags;
		kItemData.anti_flags = kItemSet.anti_flags;

		for (int isocket = 0; isocket < ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
			kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
		for (int iattr = 0; iattr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
			kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];


		CPythonPlayer::instance().SetAcceItemData(kItemSet.Cell.cell, kItemData);

		break;
	}
	case ACCE_SUBHEADER_GC_CLEAR_SLOT:
	{
		WORD cell;
		if (!Recv(sizeof(WORD), &cell))
		{
			TraceError("Failed to receive Cell from ACCE_SUBHEADER_GC_CLEAR_SLOT");
			return false;

		}

		CPythonPlayer::instance().DelAcceItemData(cell);

		break;
	}
	case ACCE_SUBHEADER_GC_CLEAR_ALL:
	{

		CPythonPlayer::instance().DelAcceItemData(0);
		CPythonPlayer::instance().DelAcceItemData(1);
		CPythonPlayer::instance().DelAcceItemData(2);

		break;
	}
	case ACCE_SUBHEADER_GC_CLEAR_ONE:
	{

		CPythonPlayer::instance().DelAcceItemData(1);
		break;

	}
	}

	__RefreshInventoryWindow();
	__RefreshAcceWindow();

	return true;

}



bool CPythonNetworkStream::SendAcceRefineCheckinPacket(TItemPos InventoryPos, BYTE byAccePos)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGAcce kPackAcce;
	kPackAcce.header = HEADER_CG_ACCE;
	kPackAcce.subheader = ACCE_SUBHEADER_CG_REFINE_CHECKIN;


	TPacketCGAcceCheckin kAcceCheckin;
	kAcceCheckin.ItemPos = InventoryPos;
	kAcceCheckin.bAccePos = byAccePos;

	if (!Send(sizeof(kPackAcce), &kPackAcce))
		return false;

	if (!Send(sizeof(kAcceCheckin), &kAcceCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineCheckoutPacket(BYTE byAccePos)
{
	if (byAccePos > 0)
		__PlaySafeBoxItemDropSound(byAccePos);

	TPacketCGAcce kPackAcce;
	kPackAcce.header = HEADER_CG_ACCE;
	kPackAcce.subheader = ACCE_SUBHEADER_CG_REFINE_CHECKOUT;


	TPacketCGAcceCheckout kAcceCheckin;
	kAcceCheckin.bAccePos = byAccePos;

	if (!Send(sizeof(kPackAcce), &kPackAcce))
		return false;

	if (!Send(sizeof(kAcceCheckin), &kAcceCheckin))
		return false;


	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineAcceptPacket(BYTE windowType)
{
	TPacketCGAcce kPackAcce;
	kPackAcce.header = HEADER_CG_ACCE;
	kPackAcce.subheader = ACCE_SUBHEADER_CG_REFINE_ACCEPT;


	TPacketCGAcceRefineAccept kAcceCheckin;
	kAcceCheckin.windowType = windowType;

	if (!Send(sizeof(kPackAcce), &kPackAcce))
		return false;

	if (!Send(sizeof(kAcceCheckin), &kAcceCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineCancelPacket()
{
	TPacketCGAcce kPackAcce;
	kPackAcce.header = HEADER_CG_ACCE;
	kPackAcce.subheader = ACCE_SUBHEADER_CG_REFINE_CANCEL;


	if (!Send(sizeof(kPackAcce), &kPackAcce))
		return false;

	return SendSequence();
}
#endif