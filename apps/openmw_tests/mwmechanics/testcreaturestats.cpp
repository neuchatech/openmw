#include <gtest/gtest.h>

#include "../../mwmechanics/creaturestats.hpp"
#include "../../mwmechanics/damagesourcetype.hpp"

namespace
{
    TEST(CreatureStats, TakeDamage_StaminaShield)
    {
        MWMechanics::CreatureStats stats;
        
        // Setup initial stats
        MWMechanics::DynamicStat<float> health;
        health.setBase(100);
        health.setCurrent(100);
        stats.setHealth(health);

        MWMechanics::DynamicStat<float> fatigue;
        fatigue.setBase(100);
        fatigue.setCurrent(100);
        stats.setFatigue(fatigue);

        // 1. Absorb full damage into fatigue
        // Damage 40. Fatigue -> 60. Health -> 100.
        stats.takeDamage(40, MWMechanics::DamageSourceType::Melee);
        EXPECT_FLOAT_EQ(stats.getFatigue().getCurrent(), 60.0f);
        EXPECT_FLOAT_EQ(stats.getHealth().getCurrent(), 100.0f);

        // 2. Absorb partial damage (Fatigue depletion)
        // Damage 80. Fatigue (60) absorbed. Remaining 20 to Health.
        // Fatigue -> 0. Health -> 80.
        stats.takeDamage(80, MWMechanics::DamageSourceType::Melee);
        EXPECT_FLOAT_EQ(stats.getFatigue().getCurrent(), 0.0f);
        EXPECT_FLOAT_EQ(stats.getHealth().getCurrent(), 80.0f);

        // 3. No Stamina left -> Full Health damage
        // Damage 10. Fatigue -> 0. Health -> 70.
        stats.takeDamage(10, MWMechanics::DamageSourceType::Melee);
        EXPECT_FLOAT_EQ(stats.getFatigue().getCurrent(), 0.0f);
        EXPECT_FLOAT_EQ(stats.getHealth().getCurrent(), 70.0f);
    }
    
    TEST(CreatureStats, TakeDamage_MagicBypassesShield)
    {
        MWMechanics::CreatureStats stats;
        
        MWMechanics::DynamicStat<float> health;
        health.setBase(100);
        health.setCurrent(100);
        stats.setHealth(health);

        MWMechanics::DynamicStat<float> fatigue;
        fatigue.setBase(100);
        fatigue.setCurrent(100);
        stats.setFatigue(fatigue);

        // Magic damage should BYPASS fatigue (unless we changed plan, but current code assumes Melee/Ranged check)
        // Wait, did I include Magic in the check?
        // Code: `if (source == DamageSourceType::Melee || source == DamageSourceType::Ranged)`
        
        stats.takeDamage(30, MWMechanics::DamageSourceType::Magic);
        EXPECT_FLOAT_EQ(stats.getFatigue().getCurrent(), 100.0f);
        EXPECT_FLOAT_EQ(stats.getHealth().getCurrent(), 70.0f);
    }
}
