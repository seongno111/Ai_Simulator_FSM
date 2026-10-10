#pragma once

#include "bt/Blackboard.h"

namespace simulation::keys
{
// Shared blackboard: written by the woodcutter, observed by both shopkeepers.
inline constexpr bt::Key<bool> SellingWood{"interaction.sellingWood"};
inline constexpr bt::Key<bool> VisitingTavern{"interaction.visitingTavern"};
inline constexpr bt::Key<bool> Drinking{"interaction.drinking"};
inline constexpr bt::Key<bool> SellerInToilet{"interaction.sellerInToilet"};

// Each model owns its own local blackboard, even for multiple actors of one type.
inline constexpr bt::Key<int> Wood{"woodcutter.wood"};
inline constexpr bt::Key<int> WorkLength{"woodcutter.workLength"};
inline constexpr bt::Key<int> Stamina{"woodcutter.stamina"};
inline constexpr bt::Key<int> Coin{"woodcutter.coin"};
inline constexpr bt::Key<int> Intoxication{"woodcutter.intoxication"};
inline constexpr bt::Key<int> SleepProgress{"woodcutter.sleepProgress"};
inline constexpr bt::Key<int> Sleep{"seller.sleep"};
inline constexpr bt::Key<int> Cigarette{"seller.cigarette"};
inline constexpr bt::Key<int> Toilet{"seller.toilet"};
inline constexpr bt::Key<int> ToiletProgress{"seller.toiletProgress"};
inline constexpr bt::Key<int> DishProgress{"tavern.dishProgress"};
inline constexpr bt::Key<int> CleaningProgress{"tavern.cleaningProgress"};
}
