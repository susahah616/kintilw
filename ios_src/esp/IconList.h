#pragma once
#include <vector>
#include <string>
#include "ICON.h"
#include "MonsterIcon.h"

// Icon struct: texture = (__bridge_retained void*)id<MTLTexture>
// ImGui Metal backend: AddImage((void*)texture, ...)
struct Icon {
    void* texture = nullptr;  // MTLTexture via __bridge_retained
    int width = 0;
    int height = 0;
    bool IsValid = false;
};

std::vector<Icon> CountryIcon;
std::vector<Icon> HeroIcon;
std::vector<Icon> SpellIcon;
std::vector<Icon> RankIcon;


std::vector<Icon> StarIcon;
std::vector<Icon> MonsterIcon;
std::vector<Icon> MonsterAlert;

Icon MonsterTexture(int Id) {
    if (MonsterIcon.empty()) return Icon{};
    switch(Id) {
    case 2002: return (MonsterIcon.size()>1) ? MonsterIcon[1] : Icon{};
    case 2003: return (MonsterIcon.size()>2) ? MonsterIcon[2] : Icon{};
    case 2004: return (MonsterIcon.size()>3) ? MonsterIcon[3] : Icon{};
    case 2005: return (MonsterIcon.size()>4) ? MonsterIcon[4] : Icon{};
    case 2006: return (MonsterIcon.size()>5) ? MonsterIcon[5] : Icon{};
    case 2008: return (MonsterIcon.size()>6) ? MonsterIcon[6] : Icon{};
    case 2009: return (MonsterIcon.size()>7) ? MonsterIcon[7] : Icon{};
    case 2011: return (MonsterIcon.size()>8) ? MonsterIcon[8] : Icon{};
    case 2012: return (MonsterIcon.size()>9) ? MonsterIcon[9] : Icon{};
    case 2013: return (MonsterIcon.size()>10) ? MonsterIcon[10] : Icon{};
    case 2056: return (MonsterIcon.size()>11) ? MonsterIcon[11] : Icon{};
    case 2059: return (MonsterIcon.size()>12) ? MonsterIcon[12] : Icon{};
    default:
        if ((size_t)Id < MonsterIcon.size()) return MonsterIcon[Id];
        return Icon{};
    }
}

Icon MonsterAlertTexture(int Id) {
    switch(Id) {
	case 2002:
		return MonsterAlert[1];
        break;
	case 2003:
		return MonsterAlert[2];
        break;
	default:
	return MonsterAlert[Id];
	}
	return MonsterAlert[Id];
}



Icon ICTexture(int Id) {
    if (HeroIcon.empty()) return Icon{};
    switch(Id) {
    case 9995: return (HeroIcon.size()>18) ? HeroIcon[18] : Icon{};
    case 9996: return (HeroIcon.size()>16) ? HeroIcon[16] : Icon{};
    case 9997: return (HeroIcon.size()>6)  ? HeroIcon[6]  : Icon{};
    case 9998: return (HeroIcon.size()>16) ? HeroIcon[16] : Icon{};
    case 9999: return (HeroIcon.size()>18) ? HeroIcon[18] : Icon{};
    default:
        if ((size_t)Id < HeroIcon.size()) return HeroIcon[Id];
        return Icon{};
    }
}

Icon SpellTexture(int Id) {
    switch(Id) {
	case 20060:
		return SpellIcon[1];
        break;
	case 20160:
		return SpellIcon[2];
        break;
	case 20150:
		return SpellIcon[3];
        break;
	case 20140:
		return SpellIcon[4];
        break;
	case 20100:
		return SpellIcon[5];
        break;
	case 20030:
		return SpellIcon[6];
        break;
	case 20070:
		return SpellIcon[7];
        break;    
	case 20080:
		return SpellIcon[8];
        break;
	case 20020:
		return SpellIcon[9];
        break;    
	case 20050:
		return SpellIcon[10];
        break;
	case 20040:
		return SpellIcon[11];
        break;
	case 20190:
		return SpellIcon[12];
        break;
	default:
	return SpellIcon[Id];
	}
	return SpellIcon[Id];
}

Icon RankTexture(const int uiRankLevel) {
    if (uiRankLevel <= 136) {
        if (uiRankLevel <= 11) {
            return RankIcon[1];
        }
        if (uiRankLevel < 27) {
            return RankIcon[2];
        }
        if (uiRankLevel < 47) {
            return RankIcon[3];
        }
        if (uiRankLevel < 77) {
            return RankIcon[4];
        }
        if (uiRankLevel < 107) {
            return RankIcon[5];
        }
        return RankIcon[6];
    } else {
        int Star = uiRankLevel - 136;
        if (Star <= 24) {
            return RankIcon[7];
        }
        if (Star <= 49) {
            return RankIcon[8];
        }
        if (Star <= 99) {
            return RankIcon[9];
        }
        return RankIcon[10];
    }
    return RankIcon[0];
}






int MonsterPic(int m_id) {
	int monsterPick = 0;
	switch(m_id) {
		case 0:
			monsterPick = 0;
			break;
		case 2002:
			monsterPick = 1;
			break;
		case 2003:
			monsterPick = 2;
			break;
		case 2004:
			monsterPick = 3;
			break;
		case 2005:
			monsterPick = 4;
			break;
		case 2006:
			monsterPick = 5;
			break;
		case 2008:
			monsterPick = 6;
			break;
		case 2009:
			monsterPick = 7;
			break;
		case 2011:
			monsterPick = 8;
			break;
		case 2012:
			monsterPick = 9;
			break;
		case 2013:
			monsterPick = 10;
			break;
	    case 2056:
			monsterPick = 11;
			break;
		case 2059:
			monsterPick = 12;
			break;
		case 2079:
			monsterPick = 13;
			break;
			
	}
    return monsterPick;
}

