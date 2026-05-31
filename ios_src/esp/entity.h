#pragma once
#include <vector>
#include <string>
#include <mutex>
#include "../Il2CppResolver.h"
#include "../il2cpp_types.h"
#include "../memory_internal.h"
#include "offsets.h"
#include "game_math.h"

// ============================================================
//  ENTITY DATA STRUCTURES
//  Mewakili data ShowPlayer / ShowEntity yang dibaca dari memori
// ============================================================

enum class EntityType {
    Unknown = 0,
    Hero,
    Monster,
    Tower,
    Soldier,
    Boss
};

struct EntityData {
    uintptr_t ptr       = 0;    // Pointer ke ShowEntity/ShowPlayer di memori

    // Identity
    uint32_t  guid      = 0;    // m_uGuid
    int32_t   entityId  = 0;    // m_ID
    int32_t   level     = 0;    // _level
    std::string name;           // m_HeroName (ShowPlayer only)
    EntityType  type    = EntityType::Unknown;

    // Camp
    int32_t   camp      = 0;    // m_EntityCampType (1=Blue, 2=Red)
    bool      isSelf    = false;
    bool      isPlayer  = false;
    bool      isMonster = false;
    bool      isTower   = false;
    bool      isSoldier = false;
    bool      isBoss    = false;
    bool      isDead    = false;
    bool      inScreen  = false;

    // Stats
    int32_t   hp        = 0;
    int32_t   hpMax     = 1;
    int32_t   hpPer     = 0;
    int32_t   mp        = 0;
    int32_t   mpMax     = 1;
    int32_t   energy    = 0;
    int32_t   energyMax = 1;
    float     atkRange  = 0.f;
    float     moveSpeed = 0.f;

    // Position (world)
    Vec3 pos;
    Vec3 smoothPos;  // Posisi yang di-smooth untuk menghilangkan jitter ESP

    // Position (screen) - filled by WorldToScreen
    Vec2 screenPos;
    bool onScreen = false;

    // Cooldowns
    int skill_cd[4] = {0};
    int skill_max_cd[4] = {1};
    int spell_cd = 0;
    int spell_max_cd = 1;
    int n_skills = 0;

    // Retri upgrade stats
    int killNum = 0;
    int assistNum = 0;
    int killWild = 0;

    // HP percent helper
    float HpPct() const {
        if (hpMax <= 0) return 0.f;
        return (float)hp / (float)hpMax;
    }
    float MpPct() const {
        if (mpMax <= 0) return 0.f;
        return (float)mp / (float)mpMax;
    }
};

// ============================================================
//  CODE BREAKER MONSTER FILTER
// ============================================================
inline std::string MonsterToString(int m_id) {
    std::string strMonster;
    switch(m_id) {
    case 2002: strMonster = "Lord"; break;
    case 2003: strMonster = "Turtle"; break;
    case 2004: strMonster = "Fiend"; break;
    case 2005: strMonster = "Serpent"; break;
    case 2006: strMonster = "Scaled Lizard"; break;
    case 2008: strMonster = "Crammer 1"; break;
    case 2009: strMonster = "Rockursa"; break;
    case 2011: strMonster = "Crab"; break;
    case 2012: strMonster = "Serpent kids"; break;
    case 2013: strMonster = "Crab"; break;
    case 2056: strMonster = "Lithowanderer 1"; break;
    case 2059: strMonster = "Crammer 2"; break;
    case 2072: strMonster = "Lithowanderer 2"; break;
    case 2220: strMonster = "Elemental Lord"; break;
    case 2221: strMonster = "Dragon Turtle"; break;
    case 2222: strMonster = "Molten Fiend"; break;
    case 2223: strMonster = "Thunder Fenrir"; break;
    case 2224: strMonster = "Horned Lizard"; break;
    case 2225: strMonster = "Fire Beetle"; break;
    case 2226: strMonster = "Lava Golem"; break;
    case 2227: strMonster = "Scavenger Crab"; break;
    case 2228: strMonster = "Little Thunder Fenrir"; break;
    case 2230: strMonster = "Lithowanderer"; break;
    default: return ""; // Jika tidak terdaftar, abaikan
    }
    return strMonster;
}

// ============================================================
//  ENTITY READER
//  Membaca data dari pointer ShowEntity/ShowPlayer
// ============================================================
namespace EntityReader {

inline void ReadBase(EntityData& e) {
    using namespace InternalMemory;
    uintptr_t p = e.ptr;
    if (!p) return;

    // Flags
    e.isPlayer  = ReadBool(p + OFF_SHOW_IS_PLAYER);
    e.isSoldier = ReadBool(p + OFF_SHOW_IS_SOLDIER);
    e.isTower   = ReadBool(p + OFF_SHOW_IS_TOWER);
    e.isMonster = ReadBool(p + OFF_SHOW_IS_MONSTER);
    e.isBoss    = ReadBool(p + OFF_SHOW_IS_BOSS);
    e.isDead    = ReadBool(p + OFF_SHOW_IS_DEATH);
    e.inScreen  = ReadBool(p + OFF_SHOW_IN_SCREEN);
    e.camp      = ReadInt32(p + OFF_SHOW_CAMP);

    // Stats (EntityBase fields via ShowEntity)
    e.guid   = Read<uint32_t>(p + OFF_SE_GUID);
    e.entityId  = ReadInt32(p + OFF_SE_ID);
    e.level  = ReadInt32(p + OFF_SE_LEVEL);
    e.hp     = ReadInt32(p + OFF_SE_HP);
    e.hpMax  = ReadInt32(p + OFF_SE_HP_MAX);
    e.hpPer  = ReadInt32(p + OFF_SE_HP_PER);
    e.mp     = ReadInt32(p + OFF_SE_MP);
    e.mpMax  = ReadInt32(p + OFF_SE_MP_MAX);

    // Position — 3 metode, prioritas dari paling akurat:
    bool posFound = false;

    // ── Metode 1: ShowEntity.get_position() (Unity Transform, PALING AKURAT, real-time) ──
    {
        // Cache pointer method sekali — lazy init, aman karena dipanggil dari satu thread scanner
        static void* s_get_pos_fn = nullptr;
        if (!s_get_pos_fn) s_get_pos_fn = Get_ShowEntity_get_position();
        
        if (s_get_pos_fn) {
            auto fn = reinterpret_cast<UnityVector3(*)(void*)>(s_get_pos_fn);
            UnityVector3 v = fn(reinterpret_cast<void*>(p));
            // Abaikan jika nilai tampak nol/garbage (entity belum diinit)
            if (v.x != 0.0f || v.z != 0.0f) {
                e.pos.x = v.x;
                e.pos.y = v.y;
                e.pos.z = v.z;
                posFound = true;
            }
        }
    }

    // ── Metode 2: LogicFighter VInt3 (akurat untuk hero, bergerak dinamis) ──
    if (!posFound) {
        size_t offLogicFighter = Get_SE_LogicFighter_Offset();
        if (offLogicFighter != 0) {
            uintptr_t pLogicFighter = ReadPtr(p + offLogicFighter);
            if (pLogicFighter) {
                size_t offPos = Get_LogicEntity_Position_Offset();
                if (offPos != 0) {
                    int vx = ReadInt32(pLogicFighter + offPos);
                    int vy = ReadInt32(pLogicFighter + offPos + 4);
                    int vz = ReadInt32(pLogicFighter + offPos + 8);
                    // VInt3 scaled by 1000 in MLBB (not 10000 — coba dua skala)
                    float scale = 1000.0f;
                    e.pos.x = (float)vx / scale;
                    e.pos.y = (float)vy / scale;
                    e.pos.z = (float)vz / scale;
                    if (vx != 0 || vz != 0) posFound = true;
                }
            }
        }
    }

    // ── Metode 3: Field posisi langsung (fallback, bisa stale) ──
    if (!posFound) {
        e.pos.x = ReadFloat(p + OFF_POS_X);
        e.pos.y = ReadFloat(p + OFF_POS_Y);
        e.pos.z = ReadFloat(p + OFF_POS_Z);
    }

    // Type
    if (e.isBoss)    e.type = EntityType::Boss;
    else if (e.isPlayer)  e.type = EntityType::Hero;
    else if (e.isMonster) e.type = EntityType::Monster;
    else if (e.isTower)   e.type = EntityType::Tower;
    else if (e.isSoldier) e.type = EntityType::Soldier;
}

inline void ReadPlayerExtra(EntityData& e) {
    using namespace InternalMemory;
    uintptr_t p = e.ptr;
    if (!p || !e.isPlayer) return;

    // Hero name (IL2CPP String*)
    uintptr_t namePtr = ReadPtr(p + OFF_PLAYER_HERO_NAME);
    e.name = ReadIL2CppString(namePtr);

    // Fallback name jika kosong
    if (e.name.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Hero_%u", e.guid);
        e.name = buf;
    }
}

inline void ReadMonsterName(EntityData& e) {
    if (e.isBoss) {
        e.name = "Lord";
        return;
    }

    switch (e.entityId) {
        case 2002: e.name = "Lord"; break;
        case 2003: e.name = "Turtle"; break;
        case 2004: e.name = "Fiend"; break;        // Red Buff
        case 2005: e.name = "Serpent"; break;      // Blue Buff
        case 2006: e.name = "Scaled Lizard"; break;
        case 2008: e.name = "Crammer 1"; break;
        case 2009: e.name = "Rockursa"; break;
        case 2011: e.name = "Crab"; break;
        case 2012: e.name = "Serpent Kids"; break;
        case 2013: e.name = "Crab"; break;
        case 2056: e.name = "Lithowanderer 1"; break;
        case 2059: e.name = "Crammer 2"; break;
        case 2072: e.name = "Lithowanderer 2"; break;
        case 2220: e.name = "Elemental Lord"; break;
        case 2221: e.name = "Dragon Turtle"; break;
        case 2222: e.name = "Molten Fiend"; break;
        case 2223: e.name = "Thunder Fenrir"; break;
        case 2224: e.name = "Horned Lizard"; break;
        case 2225: e.name = "Fire Beetle"; break;
        case 2226: e.name = "Lava Golem"; break;
        case 2227: e.name = "Scavenger Crab"; break;
        case 2228: e.name = "Little Thunder Fenrir"; break;
        case 2230: e.name = "Lithowanderer"; break;
        case 2232: e.name = "Little Fire Beetle"; break;
        default:
            if (e.isTower) e.name = "Tower";
            else if (e.isSoldier) e.name = "Minion";
            else e.name = ""; // Empty string for unrecognized monsters (skips drawing in esp)
            break;
    }
}

} // namespace EntityReader

// ============================================================
//  BATTLE STATE
//  Menyimpan semua entity yang dibaca dari BattleManager
// ============================================================
struct BattleState {
    uintptr_t battleMgrPtr = 0;
    uintptr_t localPlayerPtr = 0;
    int32_t   localCamp = 0;

    std::vector<EntityData> heroes;    // Buffer for read thread
    std::vector<EntityData> monsters;  // Buffer for read thread

    std::vector<EntityData> heroes_render;    // Buffer for render thread
    std::vector<EntityData> monsters_render;  // Buffer for render thread

    bool isValid = false;
    uint64_t curFrame = 0;
    
    // Debug Info
    uintptr_t dbg_bmInst = 0;
    uintptr_t dbg_logicBmInst = 0;
    int dbg_battleState = -1;
    size_t dbg_dicPlayerOff = 0;
    uintptr_t dbg_dicPlayerPtr = 0;
    int dbg_dicPlayerCount = 0;
    uintptr_t dbg_entriesPtr = 0;
    uintptr_t dbg_firstEntityPtr = 0;
    int dbg_entriesMaxLen = 0;
    size_t dbg_offLogicFighter = 0;
    size_t dbg_offLogicPos = 0;
    size_t dbg_offCachePos = 0;
    size_t dbg_offPos = 0;
    bool dbg_isLogicEntityFound = false;
    
    std::mutex m_mutex;

    // Fast copy for rendering to avoid locking during DrawList
    void SwapBuffers() {
        std::lock_guard<std::mutex> lock(m_mutex);
        heroes_render = heroes;
        monsters_render = monsters;
    }

    // Offset Cache
    size_t Get_dicPlayerShow_Offset() {
        static size_t off = 0;
        if (off == 0) off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_dicPlayerShow");
        return off;
    }
    
    size_t Get_dicMonsterShow_Offset() {
        static size_t off = 0;
        if (off == 0) off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_dicMonsterShow");
        return off;
    }
    
    size_t Get_LocalPlayerShow_Offset() {
        static size_t off = 0;
        if (off == 0) off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_LocalPlayerShow");
        return off;
    }
    
    size_t Get_ShowMonsters_Offset() {
        static size_t off = 0;
        if (off == 0) off = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_ShowMonsters");
        return off;
    }

    // Refresh dari memori
    void Update(uintptr_t bmPtr, uintptr_t logicBmPtr) {
        using namespace InternalMemory;
        if (!bmPtr) return;

        std::vector<EntityData> tempHeroes;
        std::vector<EntityData> tempMonsters;
        
        uint64_t t_curFrame = Read<uint64_t>(bmPtr + OFF_BM_CUR_FRAME);
        uintptr_t t_localPlayerPtr = ReadPtr(bmPtr + Get_LocalPlayerShow_Offset());
        int32_t t_localCamp = 0;
        
        if (t_localPlayerPtr) {
            t_localCamp = ReadInt32(t_localPlayerPtr + OFF_SHOW_CAMP);
        }
        
        dbg_offLogicFighter = Get_SE_LogicFighter_Offset();
        dbg_offLogicPos = Get_LogicEntity_Position_Offset();
        dbg_offCachePos = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_vCachePosition");
        dbg_offPos = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "_Position");
        dbg_isLogicEntityFound = (Il2CppGetClassType("Assembly-CSharp.dll", "Battle", "LogicEntity") != nullptr);

        // ============================================================
        // Read Players via m_dicPlayerShow - PERSIS CODE BREAKER
        // Menggunakan direct pointer casting (bukan manual offset)
        // ============================================================
        size_t offset_dicPlayer = Get_dicPlayerShow_Offset();
        auto* m_dicPlayerShow = *(Il2CppDictionary<int, uintptr_t>**)(bmPtr + offset_dicPlayer);
        uintptr_t dicPlayersPtr = (uintptr_t)m_dicPlayerShow;
        int parsedPlayersCount = 0;
        uintptr_t dbg_entries_local = 0;
        uintptr_t dbg_firstEntity_local = 0;
        int dbg_entriesMaxLen_local = 0;
        
        if (m_dicPlayerShow) {
            parsedPlayersCount = m_dicPlayerShow->count;
            
            // PERSIS Code Breaker: iterate entries->vector[i].value
            if (m_dicPlayerShow->entries) {
                dbg_entries_local = (uintptr_t)m_dicPlayerShow->entries;
                dbg_entriesMaxLen_local = m_dicPlayerShow->entries->max_length;
                
                int numEntries = m_dicPlayerShow->entries->max_length;
                if (numEntries > 50) numEntries = 50;
                
                for (int i = 0; i < numEntries; i++) {
                    auto& entry = m_dicPlayerShow->entries->vector[i];
                    if (entry.hashCode < 0) continue; // Empty slot
                    
                    uintptr_t entityPtr = entry.value;
                    if (i == 0) dbg_firstEntity_local = entityPtr;
                    if (!entityPtr || entityPtr < 0x100000000ULL) continue;
                    
                    EntityData e;
                    e.ptr = entityPtr;
                    EntityReader::ReadBase(e);
                    e.isPlayer = true;
                    e.type = EntityType::Hero;
                    EntityReader::ReadPlayerExtra(e);
                    e.isSelf = (entityPtr == t_localPlayerPtr);
                    tempHeroes.push_back(e);
                }
            }
        }

        // ============================================================
        // Read Monsters via m_ShowMonsters (List) - PERSIS CODE BREAKER
        // ============================================================
        size_t offset_showMonsters = Get_ShowMonsters_Offset();
        auto* m_ShowMonsters = *(Il2CppList<uintptr_t>**)(bmPtr + offset_showMonsters);
        if (m_ShowMonsters && m_ShowMonsters->items) {
            int size = m_ShowMonsters->size;
            if (size > 0 && size <= 500) {
                for (int i = 0; i < size; i++) {
                    uintptr_t entityPtr = m_ShowMonsters->items->vector[i];
                    if (!entityPtr) continue;
                    
                    int entityId = ReadInt32(entityPtr + OFF_SE_ID);
                    if (MonsterToString(entityId) == "") continue; // FILTER!
                    
                    EntityData e;
                    e.ptr = entityPtr;
                    EntityReader::ReadBase(e);
                    e.isMonster = true;
                    if (e.isBoss) e.type = EntityType::Boss;
                    else e.type = EntityType::Monster;
                    e.name = MonsterToString(e.entityId);
                    tempMonsters.push_back(e);
                }
            }
        }

        // ---- Read Monsters (Dictionary supplement) ----
        size_t offset_dicMonster = Get_dicMonsterShow_Offset();
        auto* m_dicMonsterShow = *(Il2CppDictionary<int, uintptr_t>**)(bmPtr + offset_dicMonster);
        if (m_dicMonsterShow && m_dicMonsterShow->entries) {
            int numEntries = m_dicMonsterShow->entries->max_length;
            if (numEntries > 500) numEntries = 500;
            for (int i = 0; i < numEntries; i++) {
                auto& entry = m_dicMonsterShow->entries->vector[i];
                if (entry.hashCode < 0) continue;
                uintptr_t entityPtr = entry.value;
                if (!entityPtr || entityPtr < 0x100000000ULL) continue;
                
                int entityId = ReadInt32(entityPtr + OFF_SE_ID);
                if (MonsterToString(entityId) == "") continue; // FILTER!
                
                // Dedup
                bool dup = false;
                for (auto& m : tempMonsters) {
                    if (m.ptr == entityPtr) { dup = true; break; }
                }
                if (!dup) {
                    EntityData e;
                    e.ptr = entityPtr;
                    EntityReader::ReadBase(e);
                    e.isMonster = true;
                    if (e.isBoss) e.type = EntityType::Boss;
                    else e.type = EntityType::Monster;
                    e.name = MonsterToString(e.entityId);
                    tempMonsters.push_back(e);
                }
            }
        }

        // ============================================================
        //  COOLDOWN PARSING (Logic Layer)
        // ============================================================
        if (logicBmPtr && logicBmPtr > 0x7000000000ULL) {
            uintptr_t logicEnts[50];
            int logicCount = 0;
            
            uintptr_t campA = ReadPtr(logicBmPtr + OFF_BM_CAMPA_PLAYERS);
            uintptr_t campB = ReadPtr(logicBmPtr + OFF_BM_CAMPB_PLAYERS);
            
            auto ReadLogicList = [&](uintptr_t listPtr) {
                if (!listPtr || listPtr < 0x7000000000ULL) return;
                int32_t size = ReadInt32(listPtr + OFF_LIST_SIZE);
                uintptr_t itemsArr = ReadPtr(listPtr + OFF_LIST_ITEMS);
                if (size > 0 && size <= 20 && itemsArr) {
                    for (int i=0; i<size; i++) {
                        uintptr_t lPtr = ReadPtr(itemsArr + OFF_ARRAY_FIRST_ITEM + i * 8);
                        if (lPtr > 0x7000000000ULL && logicCount < 50) {
                            logicEnts[logicCount++] = lPtr;
                        }
                    }
                }
            };
            ReadLogicList(campA);
            ReadLogicList(campB);
            
            // Match LogicFighter to ShowPlayer
            for (auto& h : tempHeroes) {
                uintptr_t matchedLogic = 0;
                for (int i=0; i<logicCount; i++) {
                    int32_t l_id = ReadInt32(logicEnts[i] + OFF_ENTITY_ID);
                    if (l_id == h.entityId) {
                        matchedLogic = logicEnts[i];
                        break;
                    }
                }
                
                if (matchedLogic) {
                    // Extract kill stats for self (needed for Retri upgrade calculation)
                    if (h.isSelf) {
                        h.killWild = ReadInt32(matchedLogic + OFF_PLAYER_KILL_WILD);
                        uintptr_t playerData = ReadPtr(matchedLogic + OFF_PLAYER_DATA);
                        if (playerData > 0x7000000000ULL) {
                            h.killNum = ReadInt32(playerData + OFF_PD_KILL_NUM);
                            h.assistNum = ReadInt32(playerData + OFF_PD_ASSIST_NUM);
                        }
                    }

                    uintptr_t skillComp = ReadPtr(matchedLogic + OFF_LF_SKILLCOMP);
                    if (skillComp > 0x7000000000ULL) {
                        uintptr_t cdComp = ReadPtr(skillComp + OFF_SC_COOLDOWNCOMP);
                        if (cdComp > 0x7000000000ULL) {
                            uintptr_t dic = ReadPtr(cdComp + OFF_CD_DICCOOLINFO);
                            if (dic > 0x7000000000ULL) {
                                uintptr_t entries = ReadPtr(dic + 0x18);
                                int32_t count = ReadInt32(dic + 0x20);
                                
                                struct CdEntry { int id; int remain_ms; int max_ms; };
                                std::vector<CdEntry> cds;
                                
                                if (entries > 0x7000000000ULL && count > 0 && count <= 50) {
    for (int k=0; k<count && k<30; k++) {
    uintptr_t entryBase = entries + 0x20 + k * 0x18;
    int32_t hash = ReadInt32(entryBase + 0x00);
    if (hash == -1) continue;

    int32_t key = ReadInt32(entryBase + 0x08);
    if (key <= 0) continue;

    uintptr_t cdData = ReadPtr(entryBase + 0x10);
    if (cdData > 0x7000000000ULL) {
        uint32_t cool_time = Read<uint32_t>(cdData + OFF_CDD_COOLTIME);
        uint32_t start_time = Read<uint32_t>(cdData + OFF_CDD_STARTTIME);
        uint32_t orig_max = Read<uint32_t>(cdData + OFF_CDD_ORIGMAX);
        uint32_t game_time = Read<uint32_t>(logicBmPtr + 0x19c);

        int max_ms = orig_max > 0 ? orig_max : cool_time;
        int remain_ms = 0;

        if (cool_time > 0) {
            if (start_time > 0) {
                uint32_t end_time = start_time + cool_time;
                if (game_time > 0 && game_time < end_time) {
                    remain_ms = end_time - game_time;
                } else {
                    remain_ms = cool_time;
                }
            } else {
                remain_ms = cool_time;
            }
        }

        cds.push_back({key, remain_ms, max_ms});
    }
}
}
                                // Map to Hero Skills
                                // Hero skills follow: heroId * 100 + slot (e.g. 1600=S1, 1610=S2)
                                int heroBase = h.entityId * 100;
                                int validSkillsFound = 0;
                                uint32_t game_time = Read<uint32_t>(logicBmPtr + 0x19c);
                                
                                // Sort or find matching skills
                                // Slot 1 = heroBase + 10
                                // Slot 2 = heroBase + 20
                                // Slot 3 = heroBase + 30
                                // Slot 4 = heroBase + 40
                                
                                // Pass 1: find spell
                                int bestSpellCd = 0;
                                int bestSpellMax = 0;
                                for (auto& c : cds) {
                                    if (c.id >= 20000 && c.max_ms > 10000) { // Battle Spell
                                        if (c.max_ms > bestSpellMax) {
                                            bestSpellCd = c.remain_ms;
                                            bestSpellMax = c.max_ms;
                                        }
                                    }
                                }
                                h.spell_cd = bestSpellCd;
                                h.spell_max_cd = bestSpellMax;
                                
                                // Pass 2: hero skills
                                for (int s_idx=1; s_idx<=4; s_idx++) {
                                    int targetId = heroBase + (s_idx * 10);
                                    for (auto& c : cds) {
                                        if (c.id == targetId && c.max_ms > 2000) {
                                            h.skill_cd[s_idx-1] = c.remain_ms;
                                            h.skill_max_cd[s_idx-1] = c.max_ms;
                                            if (s_idx > validSkillsFound) validSkillsFound = s_idx;
                                        }
                                    }
                                }
                                h.n_skills = validSkillsFound;
                                
                                // Fallback for some heroes where skill ID might not perfectly match * 100 + 10:
                                // Just grab the top N highest max_ms skills if validSkillsFound == 0
                                if (h.n_skills == 0) {
                                    std::vector<CdEntry> h_skills;
                                    for(auto& c : cds) {
                                        if (c.id > heroBase && c.id < heroBase + 100 && c.max_ms >= 2000) {
                                            h_skills.push_back(c);
                                        }
                                    }
                                    // Sort by ID ascending
                                    for(size_t i=0; i<h_skills.size(); i++) {
                                        for(size_t j=i+1; j<h_skills.size(); j++) {
                                            if (h_skills[i].id > h_skills[j].id) {
                                                auto tmp = h_skills[i]; h_skills[i] = h_skills[j]; h_skills[j] = tmp;
                                            }
                                        }
                                    }
                                    for(size_t i=0; i<h_skills.size() && i<4; i++) {
                                        h.skill_cd[i] = h_skills[i].remain_ms;
                                        h.skill_max_cd[i] = h_skills[i].max_ms;
                                    }
                                    h.n_skills = h_skills.size() > 4 ? 4 : h_skills.size();
                                }
                            }
                        }
                    }
                }
                // (Removed debug text logic since parsing is confirmed working)
            }
        }

        // Fast swap under lock
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            battleMgrPtr = bmPtr;
            curFrame = t_curFrame;
            localPlayerPtr = t_localPlayerPtr;
            localCamp = t_localCamp;
            heroes = std::move(tempHeroes);
            monsters = std::move(tempMonsters);
            
            dbg_dicPlayerOff = offset_dicPlayer;
            dbg_dicPlayerPtr = dicPlayersPtr;
            dbg_dicPlayerCount = parsedPlayersCount;
            dbg_entriesPtr = dbg_entries_local;
            dbg_firstEntityPtr = dbg_firstEntity_local;
            dbg_entriesMaxLen = dbg_entriesMaxLen_local;
            
            isValid = true;
        }
    }
};

// Global battle state
inline BattleState g_Battle;
