#pragma once

#include "PythonWindow.h"
#include "../UserInterface/Locale_dev.h"

namespace UI 
{
	enum
	{
		ITEM_WIDTH = 32,
		ITEM_HEIGHT = 32,

		SLOT_NUMBER_NONE = 0xffffffff,
	};

	enum ESlotStyle
	{
		SLOT_STYLE_NONE,
		SLOT_STYLE_PICK_UP,
		SLOT_STYLE_SELECT,
	};

	enum ESlotState
	{
		SLOT_STATE_LOCK					= (1 << 0),
		SLOT_STATE_CANT_USE				= (1 << 1),
		SLOT_STATE_DISABLE				= (1 << 2),
		SLOT_STATE_ALWAYS_RENDER_COVER	= (1 << 3),			// ���� Cover ��ư�� ���Կ� ���� ���� ���� ������ ������ �ϴµ�, �� flag�� ������ �� �����̾ Ŀ�� ������
#ifdef WJ_ENABLE_TRADABLE_ICON
		SLOT_STATE_CANT_MOUSE_EVENT		= (1 << 4),
		SLOT_STATE_UNUSABLE				= (1 << 5),
#endif
		SLOT_STATE_HIGHLIGHT_GREEN 		= (1 << 6),
	};
	
	enum ESlotDiffuseColorType
	{
		SLOT_COLOR_TYPE_ORANGE,
		SLOT_COLOR_TYPE_WHITE,
		SLOT_COLOR_TYPE_RED,
		SLOT_COLOR_TYPE_GREEN,
		SLOT_COLOR_TYPE_YELLOW,
		SLOT_COLOR_TYPE_SKY,
		SLOT_COLOR_TYPE_PINK,
	};

	class CSlotWindow : public CWindow
	{
		public:
			static DWORD Type();

		public:
			class CSlotButton;
			class CCoverButton;
			class CCoolTimeFinishEffect;

			friend class CSlotButton;
			friend class CCoverButton;

			typedef struct SSlot
			{
				DWORD	dwState;
				DWORD	dwSlotNumber;
				DWORD	dwCenterSlotNumber;		// NOTE : ����� ū �������� ��� �������� ���� ��ġ ��ȣ
				DWORD	dwItemIndex;			// NOTE : ���⼭ ���Ǵ� Item�̶�� �ܾ�� ���� ������ ���� �ƴ�,
				BOOL	isItem;					//        "������ ���빰"�̶�� �������� �����. �� ���� ���� ������? - [levites]

				// CoolTime
				float	fCoolTime;
				float	fStartCoolTime;

				// Toggle
				BOOL	bActive;
#ifdef WJ_ACCE_SYSTEM
				BOOL	bAcceActive;
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
				BOOL	bChangeLookActive;
				BOOL	bSlotCoverImage;
#endif

				int		ixPosition;
				int		iyPosition;

				int		ixCellSize;
				int		iyCellSize;

				BYTE	byxPlacedItemSize;
				BYTE	byyPlacedItemSize;

				CGraphicImageInstance * pInstance;
				CNumberLine * pNumberLine;

				bool	bRenderBaseSlotImage;
				CCoverButton * pCoverButton;
				CSlotButton * pSlotButton;
				CImageBox * pSignImage;
				CAniImageBox * pFinishCoolTimeEffect;
				D3DXCOLOR	sDiffuseColor;
			} TSlot;
			typedef std::list<TSlot> TSlotList;
			typedef TSlotList::iterator TSlotListIterator;

		public:
			CSlotWindow(PyObject * ppyObject);
			virtual ~CSlotWindow();

			void Destroy();

			// Manage Slot
			void SetSlotType(DWORD dwType);
			void SetSlotStyle(DWORD dwStyle);

			void AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
			void SetCoverButton(DWORD dwIndex, const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName, const char * c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
			void SetSlotBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);
			void SetSlotBaseImageScale(const char * c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy);
			void SetSlotScale(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, float sx, float sy, D3DXCOLOR& diffuseColor);
			void AppendSlotButton(const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName);
			void AppendRequirementSignImage(const char * c_szImageName);

			void EnableCoverButton(DWORD dwIndex);
			void DisableCoverButton(DWORD dwIndex);
			void SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender = false);

			void ShowSlotBaseImage(DWORD dwIndex);
			void HideSlotBaseImage(DWORD dwIndex);
			BOOL IsDisableCoverButton(DWORD dwIndex);
			BOOL HasSlot(DWORD dwIndex);

			void ClearAllSlot();
			void ClearSlot(DWORD dwIndex);
			void SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor);
#ifdef WJ_OKEY_CARD_EVENT
			void SetCardSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, const char * c_szFileName, D3DXCOLOR& diffuseColor);
#endif
			void SetSlotCount(DWORD dwIndex, DWORD dwCount);
			void SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
			void SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
#ifdef WJ_CHANGELOOK_SYSTEM
			void EnableSlotCoverImage(DWORD dwIndex);
			void DisableSlotCoverImage(DWORD dwIndex);
#endif
			void ActivateSlot(DWORD dwIndex);
			void DeactivateSlot(DWORD dwIndex);
#ifdef WJ_ACCE_SYSTEM
			void ActivateAcceSlot(DWORD dwIndex);
			void DeactivateAcceSlot(DWORD dwIndex);
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
			void ActivateChangeLookSlot(DWORD dwIndex);
			void DeactivateChangeLookSlot(DWORD dwIndex);
#endif
			void RefreshSlot();

			DWORD GetSlotCount();

			void LockSlot(DWORD dwIndex);
			void UnlockSlot(DWORD dwIndex);
			BOOL IsLockSlot(DWORD dwIndex);
			void SetCantUseSlot(DWORD dwIndex);
			void SetUseSlot(DWORD dwIndex);
			BOOL IsCantUseSlot(DWORD dwIndex);
			void EnableSlot(DWORD dwIndex);
			void DisableSlot(DWORD dwIndex);
			BOOL IsEnableSlot(DWORD dwIndex);

#ifdef WJ_ENABLE_TRADABLE_ICON
			void SetCanMouseEventSlot(DWORD dwIndex);
			void SetCantMouseEventSlot(DWORD dwIndex);
			void SetUsableSlotOnTopWnd(DWORD dwIndex);
			void SetUnusableSlotOnTopWnd(DWORD dwIndex);
#endif

			// Select
			void ClearSelected();
			void SelectSlot(DWORD dwSelectingIndex);
			BOOL isSelectedSlot(DWORD dwIndex);
			DWORD GetSelectedSlotCount();
			DWORD GetSelectedSlotNumber(DWORD dwIndex);

			// Slot Button
			void ShowSlotButton(DWORD dwSlotNumber);
			void HideAllSlotButton();
			void OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

			// Requirement Sign
			void ShowRequirementSign(DWORD dwSlotNumber);
			void HideRequirementSign(DWORD dwSlotNumber);

			// ToolTip
			BOOL OnOverInItem(DWORD dwSlotNumber);
			void OnOverOutItem();

			// For Usable Item
			void SetUseMode(BOOL bFlag);
			void SetUsableItem(BOOL bFlag);

			// CallBack
			void ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex);
			
			void SetDiffuseColor(DWORD dwIndex, BYTE colorType);
			void DisableSlotHighlightedGreen(DWORD dwIndex);
			void SetSlotHighlightedGreeen(DWORD dwIndex);

		protected:
			void __Initialize();
			void __CreateToggleSlotImage();
			void __CreateSlotEnableEffect();
			void __CreateFinishCoolTimeEffect(TSlot * pSlot);
			void __CreateBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);
			void __CreateBaseImageScale(const char * c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy);

			void __DestroyToggleSlotImage();
			void __DestroySlotEnableEffect();
#ifdef WJ_ACCE_SYSTEM
			void __CreateAcceSlotEnableEffect();
			void __DestroySlotAcceEnableEffect();
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
			void __CreateChangeLookSlotEnableEffect();
			void __DestroySlotChangeLookEnableEffect();
			void __CreateSlotCoverImage();
			void __DestroySlotCoverImage();
#endif
			void __DestroyFinishCoolTimeEffect(TSlot * pSlot);
			void __DestroyBaseImage();

			// Event
			void OnUpdate();
			void OnRender();
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			BOOL OnMouseRightButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			void OnMouseOverOut();
			void OnMouseOver();
			void RenderSlotBaseImage();
			void RenderLockedSlot();
			virtual void OnRenderPickingSlot();
			virtual void OnRenderSelectedSlot();

			// Select
			void OnSelectEmptySlot(int iSlotNumber);
			void OnSelectItemSlot(int iSlotNumber);
			void OnUnselectEmptySlot(int iSlotNumber);
			void OnUnselectItemSlot(int iSlotNumber);
			void OnUseSlot();

			// Manage Slot
			BOOL GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot);
			BOOL GetSelectedSlotPointer(TSlot ** ppSlot);
			virtual BOOL GetPickedSlotPointer(TSlot ** ppSlot);
			void ClearSlot(TSlot * pSlot);
			virtual void OnRefreshSlot();

			// ETC
			BOOL OnIsType(DWORD dwType);

		protected:
			DWORD m_dwSlotType;
			DWORD m_dwSlotStyle;
			std::list<DWORD> m_dwSelectedSlotIndexList;
			TSlotList m_SlotList;
			DWORD m_dwToolTipSlotNumber;

			BOOL m_isUseMode;
			BOOL m_isUsableItem;

			CGraphicImageInstance * m_pBaseImageInstance;
			CImageBox * m_pToggleSlotImage;
#ifdef WJ_HIGHLIGHT_SYSTEM
			CAniImageBox * m_apSlotActiveEffect[3];
#else
			CAniImageBox * m_pSlotActiveEffect;
#endif
#ifdef WJ_ACCE_SYSTEM
#ifdef WJ_HIGHLIGHT_SYSTEM
			CAniImageBox * m_apSlotAcceActiveEffect[3];
#else
			CAniImageBox * m_pSlotAcceActiveEffect;
#endif
#endif
#ifdef WJ_CHANGELOOK_SYSTEM
#ifdef WJ_HIGHLIGHT_SYSTEM
			CAniImageBox * m_apSlotChangeLookActiveEffect[3];
#else
			CAniImageBox * m_pSlotChangeLookActiveEffect;
#endif
			CAniImageBox * m_pSlotCoverImage;
#endif
			std::deque<DWORD> m_ReserveDestroyEffectDeque;
	};
};