#include "Esp.h"

void Esp::drawEsp()
{
	// Dynamic screen size
	ImVec2 ioDisp = ImGui::GetIO().DisplaySize;
	Vec::Size screen{ ioDisp.x, ioDisp.y };
	if (screen.Width <= 0 || screen.Height <= 0) return;

	// Read view matrix once
	Vec::ViewMatrix currentViewMatrix = Vec::ReadViewMatrix();
	if (g_resolvedViewMatrixAddress == 0 ||
		(currentViewMatrix.M11 == 1 && currentViewMatrix.M12 == 0 &&
			currentViewMatrix.M13 == 0 && currentViewMatrix.M14 == 0))
	{
		ImGui::Text("[-] Failed to read viewMatrix");
		return;
	}

    // read Cgame
	uintptr_t Cgame;
	if (!read(hProc, gameModuleBase + Offsets::game::CGame, Cgame))
	{
		ImGui::Text("[-] Failed to read Cgame");
		return;
	}

    // read LocalPlayer
	uintptr_t localPlayer;
	if (!read(hProc, Cgame + Offsets::entity::m_FirstObject, localPlayer))
	{
		ImGui::Text("[-] Failed to read localplayer");
		return;
	}

    // read LocalPos
	Vec::Vector3 localPos{};
	if (!read(hProc, localPlayer + Offsets::entity::m_vecPos, localPos))
	{
		ImGui::Text("[-] Failed to read localPos");
		return;
	}

    // read LocalTeam
	int localTeam;
	if (!read(hProc, localPlayer + Offsets::entity::m_Team, localTeam))
	{
		ImGui::Text("[-] Failed to read localTeam");
		return;
	}


    // Loop through entities
	uintptr_t currentObject = localPlayer;
    for (int i = 0; i < 1000 && currentObject; i++)
    {
        uintptr_t nextObject;
        if (!read(hProc, currentObject + Offsets::entity::m_NextObject, nextObject) || nextObject == 0) {
            break;
        }

        currentObject = nextObject;

        // skip localPlayer
        if (currentObject == localPlayer)
            continue;

        // read Pos
        Vec::Vector3 currentObjectPos{};
        if (!read(hProc, currentObject + Offsets::entity::m_vecPos, currentObjectPos))
            continue;

        // read team
        int currentObjectTeam;
        if (!read(hProc, currentObject + Offsets::entity::m_Team, currentObjectTeam))
            continue;

        // read Type
        int currentObjectType;
        if (!read(hProc, currentObject + Offsets::entity::m_Type, currentObjectType))
            continue;

        // read health and shield
        uintptr_t hpStructPtr;
        int curHealth = 0, maxHealth = 0, curShield = 0, maxShield = 0;
        if (read(hProc, currentObject + Offsets::entity::m_PointerToHealthAndShield, hpStructPtr))
        {
            read(hProc, hpStructPtr + Offsets::entity::m_CurHealth, curHealth);
            read(hProc, hpStructPtr + Offsets::entity::m_MaxHealth, maxHealth);
            read(hProc, hpStructPtr + Offsets::entity::m_CurShield, curShield);
            read(hProc, hpStructPtr + Offsets::entity::m_MaxShield, maxShield);
        }

        // skip if dead
        if (curHealth <= 0)
            continue;

        // skip if same team as localplayer
        if (currentObjectTeam == localTeam)
            continue;

        // team color
        ImU32 teamColor;
        switch (currentObjectTeam)
        {
        case 1:
            teamColor = IM_COL32(200, 0, 255, 255);
            break;
        case 2:
            teamColor = IM_COL32(0, 153, 255, 255);
            break;
        case 3:
            teamColor = IM_COL32(255, 0, 0, 255);
            break;
        case 4:
            teamColor = IM_COL32(119, 139, 153, 255);
            break;
        default:
            teamColor = IM_COL32(255, 255, 255, 255);
            break;
        }

        // types
        static const std::unordered_map<int, const char*> types =
        {
            {2, "Inf"}, {4, "Light"}, {5, "Medic"}, {6, "Engi"}, {7, "Heavy"}, {8, "Max"},
            {10, "Inf"}, {12, "Light"}, {13, "Medic"}, {14, "Engi"}, {15, "Heavy"}, {16, "Max"},
            {17, "Inf"}, {19, "Light"}, {20, "Medic"}, {21, "Engi"}, {22, "Heavy"}, {23, "Max"},
            {190, "Inf"}, {191, "Light"}, {192, "Medic"}, {193, "Engi"}, {194, "Heavy"}, {252, "Max"},

            // vehicles
            //{24, "Flash"}, {26, "Lightning"}, {27, "Harasser"}, {28, "Valkyrie"}, {29, "Magrider"}, {30, "Prowler"},
            //{31, "Vanguard"}, {33, "Sunderar"}, {34, "Scythe"}, {35, "Mosquito"}, {36, "Reaver"}, {37, "Liberator"},
            //{95, "ANT"}, {104, "Turret"}, {100, "Colosus"}, {54, "Javelin"}, {42, "Chimra"},

            // Objects
            {61, "Mine"}
        };

        if (types.find(currentObjectType) == types.end())
            continue;

        float playerHeight = 1.82f;

        Vec::Vector3 headPos{ currentObjectPos.x, currentObjectPos.y + playerHeight, currentObjectPos.z };
        Vec::ScreenPoint spFeet, spHead;
        if (!Vec::WorldToScreen(currentObjectPos, currentViewMatrix, screen, spFeet) || !Vec::WorldToScreen(headPos, currentViewMatrix, screen, spHead))
            continue;

        // box
        float h = spFeet.y - spHead.y;
        if (h < 0.0f) h = -h;

        float w = h / 2.0f;

        float x = spFeet.x - w / 2.0f;
        float y = spHead.y;

        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), teamColor, 0.0f, 0, 1.0f);
    }

}
