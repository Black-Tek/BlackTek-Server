#include "otpch.h"
#include "groups.h"
#include "item.h"
#include "itemcontainer.h"
#include "player.h"

#include <gtest/gtest.h>

TEST(ContainerCapacity, NestedCarriedBagChecksPlayerCapacity)
{
    auto& type = Item::items.getItemType(0);
    struct RestoreType
    {
        ItemType& type;
        bool pickupable = type.pickupable;
        uint32_t weight = type.weight;
        ~RestoreType()
        {
            type.pickupable = pickupable;
            type.weight = weight;
        }
    } restore{type};
    type.pickupable = true;
    type.weight = 50000;

    Group group{};
    auto player = std::make_shared<Player>(nullptr);
    player->setGroup(&group);
    auto outer = std::make_shared<Item>(0);
    outer->attachContainer(10, true, false);
    outer->setInventoryOwner(player);
    auto inner = std::make_shared<Item>(0);
    inner->attachContainer(10, true, false);
    inner->setContainerParent(outer);
    auto heavyItem = std::make_shared<Item>(0);

    ASSERT_GT(heavyItem->getWeight(), player->getFreeCapacity());
    EXPECT_EQ(outer->getContainer()->canAddItem(INDEX_ANYWHERE, heavyItem, 1, 0), RETURNVALUE_NOTENOUGHCAPACITY);
    EXPECT_EQ(inner->getContainer()->canAddItem(INDEX_ANYWHERE, heavyItem, 1, 0), RETURNVALUE_NOTENOUGHCAPACITY);
    EXPECT_EQ(inner->getContainer()->canAddItem(INDEX_ANYWHERE, heavyItem, 1, FLAG_NOLIMIT), RETURNVALUE_NOERROR);
    type.weight = 100;
    EXPECT_EQ(inner->getContainer()->canAddItem(INDEX_ANYWHERE, heavyItem, 1, 0), RETURNVALUE_NOERROR);
}
