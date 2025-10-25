#pragma once
#include "../Includes.h"

namespace Offsets
{
	class game
	{
	public:
		static constexpr uintptr_t CGame = 0x1440B7AE0 - 0x140000000;
		static constexpr uintptr_t CGraphics = 0x1440B7C50 - 0x140000000;
		static inline const std::vector<uintptr_t> viewMatrix = { 0xF0, 0xB0 };
	};

	class entity
	{
	public:
		static constexpr uintptr_t m_FirstObject = 0x1060;
		static constexpr uintptr_t m_NextObject = 0x5A8;
		static constexpr uintptr_t m_Team = 0x1E8;
		static constexpr uintptr_t m_Type = 0x5E0;
		static constexpr uintptr_t m_vecPos = 0x920;

		static constexpr uintptr_t m_PointerToHealthAndShield = 0x120;
		static constexpr uintptr_t m_CurHealth = 0x160;
		static constexpr uintptr_t m_MaxHealth = 0x164;
		static constexpr uintptr_t m_CurShield = 0x470;
		static constexpr uintptr_t m_MaxShield = 0x474;
	};
}