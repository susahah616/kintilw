#pragma once
#include <cstdint>

// ============================================================
//  OFFSETS - Mobile Legends: Bang Bang
//  Sumber: dump.cs (Assembly-CSharp.dll + HybridCLR)
//  Arch: ARM64, IL2CPP
// ============================================================

// ---- EntityBase ----
#define OFF_ENTITY_ACID         0x38   // m_acId (UInt64)
#define OFF_ENTITY_ITYPE        0x48   // m_iType (Int32)
#define OFF_ENTITY_IS_PLAYER    0x5C   // IsPlayer (Boolean)
#define OFF_ENTITY_IS_SOLDIER   0x5F   // IsSoldier (Boolean)
#define OFF_ENTITY_IS_BUILDING  0x60   // IsBuilding (Boolean)
#define OFF_ENTITY_IS_TOWER     0x61   // IsTower (Boolean)
#define OFF_ENTITY_IS_MONSTER   0x63   // IsWildMonster (Boolean)
#define OFF_ENTITY_IS_BOSS      0x64   // IsBoss (Boolean)
#define OFF_ENTITY_IS_SUMMONS   0x67   // IsSummons (Boolean)
#define OFF_ENTITY_GUID         0xA8   // m_uGuid (UInt32)
#define OFF_ENTITY_ID           0xAC   // m_ID (Int32)
#define OFF_ENTITY_LEVEL        0xB0   // _level (Int32)
#define OFF_ENTITY_HP_LAST      0xB8   // m_HpLast (Int32)
#define OFF_ENTITY_HP           0xC8   // m_Hp (Int32) *** CURRENT HP ***
#define OFF_ENTITY_HP_MAX       0xCC   // m_HpMax (Int32) *** MAX HP ***
#define OFF_ENTITY_HP_PER       0xD0   // m_HpPer (Int32) - HP %
#define OFF_ENTITY_MP           0xF0   // m_Mp (Int32)
#define OFF_ENTITY_MP_MAX       0xF4   // _MpMax (Int32)
#define OFF_ENTITY_ENERGY       0x100  // m_EnergyPoint (Int32)
#define OFF_ENTITY_ENERGY_MAX   0x104  // m_EnergyPointMax (Int32)
#define OFF_ENTITY_COOL_PER     0x108  // m_CoolPer (Int32) - CDR
#define OFF_ENTITY_ATK_SPEED    0x110  // m_AtkSpeed (Double)
#define OFF_ENTITY_MOV_SPEED    0x118  // m_dMoveSpeed (Double)
#define OFF_ENTITY_IS_SELF      0x138  // m_bSelf (Boolean)
#define OFF_ENTITY_ATK_DIS      0x21C  // m_AtkDis (Single) - Attack Range
#define OFF_ENTITY_RADIUS       0x224  // m_fPlayerRadius (Single)

// ---- ShowEntity (extends EntityBase, visual layer) ----
#define OFF_SHOW_GROUP_ID       0x64   // m_groupID (Int32)
#define OFF_SHOW_IS_SOLDIER     0x68   // IsSoldier
#define OFF_SHOW_IS_BUILDING    0x69   // IsBuilding
#define OFF_SHOW_IS_TOWER       0x6A   // IsTower
#define OFF_SHOW_IS_MONSTER     0x6C   // IsWildMonster
#define OFF_SHOW_ACID           0x70   // m_acId (UInt64)
#define OFF_SHOW_ITYPE          0x80   // m_iType (Int32)
#define OFF_SHOW_IN_SCREEN      0x90   // m_bInScreen (Boolean)
#define OFF_SHOW_IS_PLAYER      0x93   // IsPlayer
#define OFF_SHOW_IS_BOSS        0x96   // IsBoss
#define OFF_SHOW_IS_SUMMONS     0x99   // IsSummons

inline size_t Get_SE_DEATH_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_bDeath"); if (off == 0) off = 0xCD; }
    return off;
}

#define OFF_SHOW_IS_DEATH       Get_SE_DEATH_Offset()

inline size_t Get_SE_CAMP_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_EntityCampType"); if (off == 0) off = 0xD8; }
    return off;
}

#define OFF_SHOW_CAMP           Get_SE_CAMP_Offset()
#define OFF_SHOW_EQUIP_COMP     0xE0   // m_EquipComp*
#define OFF_SHOW_BUFF_COMP      0xE8   // m_BuffComp*
#define OFF_SHOW_SKILL_COMP     0xF0   // m_SkillComp*
#define OFF_SHOW_ATTR_COMP      0x108  // m_ShowAttributeComp*

#include "../Il2CppResolver.h"

inline size_t Get_SE_GUID_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_uGuid"); if (off == 0) off = 0xA8; }
    return off;
}

inline size_t Get_SE_ID_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_ID"); if (off == 0) off = 0xAC; }
    return off;
}

inline size_t Get_SE_LEVEL_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "_level"); if (off == 0) off = 0xB0; }
    return off;
}

inline size_t Get_SE_HP_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_Hp"); if (off == 0) off = 0xC8; }
    return off;
}

inline size_t Get_SE_HPMAX_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_HpMax"); if (off == 0) off = 0xCC; }
    return off;
}

inline size_t Get_SE_POS_Offset() {
    static size_t off = 0;
    if (off == 0) {
        // Prioritas 1: field yang dipakai Code Breaker Android (referensi asli)
        off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "<LastPosition>k__BackingField");
    }
    if (off == 0) {
        // Prioritas 2: _Position
        off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "_Position");
    }
    if (off == 0) {
        // Prioritas 3: m_vCachePosition
        off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_vCachePosition");
    }
    if (off == 0) {
        // Hardcoded fallback
        off = 0x294;
    }
    return off;
}

inline void* Get_ShowEntity_get_position() {
    static void* method = nullptr;
    if (method) return method;
    // Coba semua varian nama (iOS IL2CPP case-sensitive)
    method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "ShowEntity", "get_Position", 0);
    if (!method) method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "ShowEntity", "get_position", 0);
    return method;
}

// ---- Unity Screen resolution (game render resolution, bukan device resolution) ----
inline int GetUnityScreenWidth() {
    static void* fn = nullptr;
    if (!fn) fn = Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_width", 0);
    if (!fn) fn = Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_width", 0);
    if (!fn) return 0;
    return reinterpret_cast<int(*)()>(fn)();
}

inline int GetUnityScreenHeight() {
    static void* fn = nullptr;
    if (!fn) fn = Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Screen", "get_height", 0);
    if (!fn) fn = Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_height", 0);
    if (!fn) return 0;
    return reinterpret_cast<int(*)()>(fn)();
}

inline size_t Get_SE_LogicFighter_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "_logicFighter"); }
    if (off == 0) { off = FindFieldContaining("Assembly-CSharp.dll", "", "ShowEntity", "fighter"); }
    return off;
}

inline size_t Get_LogicEntity_Position_Offset() {
    static size_t off = 0;
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "Battle", "LogicEntity", "m_Position"); }
    if (off == 0) { off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "LogicEntity", "m_Position"); }
    // Coba FindFieldContaining di kedua namespace
    if (off == 0) { off = FindFieldContaining("Assembly-CSharp.dll", "Battle", "LogicEntity", "position"); }
    if (off == 0) { off = FindFieldContaining("Assembly-CSharp.dll", "", "LogicEntity", "position"); }
    // Fallback ke LogicFighter
    if (off == 0) { off = FindFieldContaining("Assembly-CSharp.dll", "Battle", "LogicFighter", "position"); }
    if (off == 0) { off = FindFieldContaining("Assembly-CSharp.dll", "", "LogicFighter", "position"); }
    return off;
}

// ---- EntityBase HP fields (ShowEntity offset basis dynamically resolved) ----
#define OFF_SE_GUID             Get_SE_GUID_Offset()
#define OFF_SE_ID               Get_SE_ID_Offset()
#define OFF_SE_LEVEL            Get_SE_LEVEL_Offset()
#define OFF_SE_HP               Get_SE_HP_Offset()
#define OFF_SE_HP_MAX           Get_SE_HPMAX_Offset()
#define OFF_SE_HP_PER           (Get_SE_HPMAX_Offset() + 4)
#define OFF_SE_MP               (Get_SE_HPMAX_Offset() + 36)
#define OFF_SE_MP_MAX           (Get_SE_HPMAX_Offset() + 40)

// ---- Position (m_vCachePosition = Vector3) ----
#define OFF_POS_X               Get_SE_POS_Offset()
#define OFF_POS_Y               (Get_SE_POS_Offset() + 4)
#define OFF_POS_Z               (Get_SE_POS_Offset() + 8)

// ---- ShowPlayer (extends ShowEntity) ----
#define OFF_PLAYER_ORIGIN_HERO_ID 0x8F8  // m_iOriginHeroId (Int32)
#define OFF_PLAYER_HERO_NAME    0x918    // m_HeroName (IL2CPP String*)
#define OFF_PLAYER_IPOS         0x928    // m_iPos (Int32) lane position
#define OFF_PLAYER_TEAM_ID      0x930    // m_iTeamId (UInt64)
#define OFF_PLAYER_TEAM_NAME    0x940    // m_sTeamName (String*)
#define OFF_PLAYER_RANK_LEVEL   0x954    // m_uiRankLevel (UInt32)
#define OFF_PLAYER_ROLE_LEVEL   0x95C    // m_uiRoleLevel (UInt32)
#define OFF_PLAYER_IN_BATTLE    0x9C8    // m_bInBattle (Boolean)
#define OFF_PLAYER_SEX          0x9F8    // m_Sex (UInt32)

// ---- BattleManager (Singleton) ----
// BattleManager.Instance ditemukan via IL2CPP statics scan
#define OFF_BM_CUR_FRAME        0x30   // m_lCurFrame (UInt64)
#define OFF_BM_LOCAL_PLAYER     0x50   // m_LocalPlayerShow (ShowPlayer*)
#define OFF_BM_LOCAL_PLAYERS    0x58   // m_LocalShowPlayers (List*)
#define OFF_BM_SHOW_PLAYERS     0x78   // m_ShowPlayers (List*) *** ALL HEROES ***
#define OFF_BM_SHOW_MONSTERS    0x80   // m_ShowMonsters (List*) *** ALL MONSTERS ***
#define OFF_BM_MAIN_TOWER_DEAD  0x88   // m_MainTowerDead (Boolean)

// ---- IL2CPP List<T> ----
#define OFF_LIST_ITEMS          0x10   // _items (Array*)
#define OFF_LIST_SIZE           0x18   // _size (Int32)
#define OFF_ARRAY_FIRST_ITEM    0x20   // array[0] start

// ---- IL2CPP String ----
#define OFF_STRING_LENGTH       0x10   // String.Length (Int32)
#define OFF_STRING_CHARS        0x14   // String chars start (UTF-16)

// ---- LogicFighter (logic layer) ----
#define OFF_FIGHTER_STATUS      0x1BC  // m_iStatus (Int32)
#define OFF_FIGHTER_IN_BATTLE   0x1F4  // m_bInBattle
#define OFF_FIGHTER_REAL_POS_X  0x250  // m_dRealPosX (Double)
#define OFF_FIGHTER_REAL_POS_Y  0x258  // m_dRealPosY (Double)
#define OFF_FIGHTER_FACE_DIR_X  0x270  // m_v2FaceDir.x
#define OFF_FIGHTER_FACE_DIR_Y  0x278  // m_v2FaceDir.y
#define OFF_FIGHTER_GROUP_ID    0x498  // m_groupID (Int32)
#define OFF_FIGHTER_ATTR_COMP   0x4A0  // m_AttrComp (AttributeComp*)
#define OFF_FIGHTER_MOVE_SPEED  0x118  // m_dMoveSpeed (Double)

// ---- LogicPlayer (logic layer) ----
#define OFF_PLAYER_KILL_WILD    0x9BC  // _KillWildTimes (Int32)
#define OFF_PLAYER_DATA         0xB88  // m_PlayerData (PlayerData*)

// ---- PlayerData ----
#define OFF_PD_KILL_NUM         0x38   // _killNum (Int32)
#define OFF_PD_ASSIST_NUM       0x4C   // _assistNum (Int32)

// ---- ATTR_KIND enum values ----
#define ATTR_HP                 100
#define ATTR_MP                 101
#define ATTR_PHY_ATT            102
#define ATTR_MAG_ATT            103
#define ATTR_ATT_SPEED          104
#define ATTR_MOV_SPEED          105
#define ATTR_PHY_SHIELD         106
#define ATTR_MAG_SHIELD         107
#define ATTR_XP                 115
#define ATTR_ENERGYPOINT        117
#define ATTR_HURT_LESS          118
#define ATTR_EXTRA_HP           134
#define ATTR_HP_PER             201
#define ATTR_MP_PER             202
#define ATTR_MOV_SPEED_PER      204

// ---- Camp Types ----
#define CAMP_NEUTRAL            0
#define CAMP_BLUE               1   // Team 1 (Ally jika local = 1)
#define CAMP_RED                2   // Team 2 (Enemy jika local = 1)

// ---- LogicBattleManager ----
#define OFF_BM_CAMPA_PLAYERS    0x100  // List m_CampAPlayerList
#define OFF_BM_CAMPB_PLAYERS    0x108  // List m_CampBPlayerList

// ---- LogicSkillComp & Cooldowns ----
#define OFF_LF_SKILLCOMP     0x4a8   // Ptr - LogicSkillComp
#define OFF_SC_COOLDOWNCOMP  0xa8    // Ptr - CoolDownComp
#define OFF_CD_DICCOOLINFO   0x18    // Ptr - Dictionary<int,CoolDownData>

// ---- CoolDownData ----
#define OFF_CDD_SPELLID    0x10  // Int32 iSpellID
#define OFF_CDD_COOLTIME   0x14  // UInt32 uiCoolTime (total cd ms)
#define OFF_CDD_ORIGMAX    0x18  // UInt32 originalMaxCdTime
#define OFF_CDD_STARTTIME  0x1c  // UInt32 uiStartTime
#define OFF_CDD_ISCOOLDOWN 0x20  // Boolean m_isCoolDown

namespace InternalMemory {
    // Offset ini sekarang ditangani secara dinamis melalui Il2CppResolver
    // constexpr uintptr_t OFF_BATTLE_MANAGER = 0x0;       
    // constexpr uintptr_t OFF_LOGIC_BATTLE_MANAGER = 0x0; 
}

