#include <gtest/gtest.h>
#include <algorithm>

namespace
{
    // Replicating the logic from the refactored code for verification
    float calculateScaledDamage(float baseDamage, float hitchance, float intensity, float rngRoll)
    {
        float fHitChance = std::clamp(hitchance / 100.f, 0.0f, 1.0f);
        // scalingRoll range is [fHitChance, 1.0]
        float scalingRoll = fHitChance + (1.0f - fHitChance) * rngRoll;
        return baseDamage * ((1.0f - intensity) + (scalingRoll * intensity));
    }

    TEST(DamageScaling, PerfectHitChance)
    {
        float baseDamage = 100.0f;
        float hitchance = 100.0f;
        float intensity = 1.0f;
        
        // With 100% hit chance, any RNG roll should result in 100% damage
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.0f), 100.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.5f), 100.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 1.0f), 100.0f);
    }

    TEST(DamageScaling, ZeroHitChance)
    {
        float baseDamage = 100.0f;
        float hitchance = 0.0f;
        float intensity = 1.0f;
        
        // With 0% hit chance, damage should scale from 0% to 100% based on RNG
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.0f), 0.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.5f), 50.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 1.0f), 100.0f);
    }

    TEST(DamageScaling, HalfHitChance)
    {
        float baseDamage = 100.0f;
        float hitchance = 50.0f;
        float intensity = 1.0f;
        
        // With 50% hit chance, damage should scale from 50% to 100%
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.0f), 50.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.5f), 75.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 1.0f), 100.0f);
    }

    TEST(DamageScaling, ModifiedIntensity)
    {
        float baseDamage = 100.0f;
        float hitchance = 50.0f;
        float intensity = 0.5f;
        
        // intensity 0.5 means 50% of the damage is "fixed" and 50% is scaled by hit chance.
        // Scaling range [50%, 100%]
        // Min: 50 + (50 * 0.5) = 75
        // Max: 50 + (100 * 0.5) = 100
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.0f), 75.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 1.0f), 100.0f);
    }
    
    TEST(DamageScaling, ZeroIntensity)
    {
        float baseDamage = 100.0f;
        float hitchance = 10.0f;
        float intensity = 0.0f;
        
        // intensity 0 means no scaling is applied (classic-like but hits always land)
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 0.0f), 100.0f);
        EXPECT_FLOAT_EQ(calculateScaledDamage(baseDamage, hitchance, intensity, 1.0f), 100.0f);
    }
}
