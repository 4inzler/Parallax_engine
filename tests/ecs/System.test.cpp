//// System.test.cpp ///////////////////////////////////////////////////////////////
//
// ⢀⢀⢀⣤⣤⣤⡀⢀⢀⢀⢀⢀⢀⢠⣤⡄⢀⢀⢀⢀⣠⣤⣤⣤⣤⣤⣤⣤⣤⣤⡀⢀⢀⢀⢠⣤⣄⢀⢀⢀⢀⢀⢀⢀⣤⣤⢀⢀⢀⢀⢀⢀⢀⢀⣀⣄⢀⢀⢠⣄⣀⢀⢀⢀⢀⢀⢀⢀
// ⢀⢀⢀⣿⣿⣿⣷⡀⢀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡟⡛⡛⡛⡛⡛⡛⡛⢁⢀⢀⢀⢀⢻⣿⣦⢀⢀⢀⢀⢠⣾⡿⢃⢀⢀⢀⢀⢀⣠⣾⣿⢿⡟⢀⢀⡙⢿⢿⣿⣦⡀⢀⢀⢀⢀
// ⢀⢀⢀⣿⣿⡛⣿⣷⡀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⡙⣿⡷⢀⢀⣰⣿⡟⢁⢀⢀⢀⢀⢀⣾⣿⡟⢁⢀⢀⢀⢀⢀⢀⢀⡙⢿⣿⡆⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⡈⢿⣷⡄⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⣇⣀⣀⣀⣀⣀⣀⣀⢀⢀⢀⢀⢀⢀⢀⡈⢀⢀⣼⣿⢏⢀⢀⢀⢀⢀⢀⣼⣿⡏⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⡘⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⡈⢿⣿⡄⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⣿⢿⢿⢿⢿⢿⢿⢿⢇⢀⢀⢀⢀⢀⢀⢀⢠⣾⣿⣧⡀⢀⢀⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⡈⢿⣿⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣰⣿⡟⡛⣿⣷⡄⢀⢀⢀⢀⢀⢿⣿⣇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⢀⡈⢿⢀⢀⢸⣿⡇⢀⢀⢀⢀⡛⡟⢁⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣼⣿⡟⢀⢀⡈⢿⣿⣄⢀⢀⢀⢀⡘⣿⣿⣄⢀⢀⢀⢀⢀⢀⢀⢀⢀⣼⣿⢏⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⢀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⢀⣀⣀⣀⣀⣀⣀⣀⣀⣀⡀⢀⢀⢀⣠⣾⡿⢃⢀⢀⢀⢀⢀⢻⣿⣧⡀⢀⢀⢀⡈⢻⣿⣷⣦⣄⢀⢀⣠⣤⣶⣿⡿⢋⢀⢀⢀⢀
// ⢀⢀⢀⢿⢿⢀⢀⢀⢀⢀⢀⢀⢀⢸⢿⢃⢀⢀⢀⢀⢻⢿⢿⢿⢿⢿⢿⢿⢿⢿⢃⢀⢀⢀⢿⡟⢁⢀⢀⢀⢀⢀⢀⢀⡙⢿⡗⢀⢀⢀⢀⢀⡈⡉⡛⡛⢀⢀⢹⡛⢋⢁⢀⢀⢀⢀⢀⢀
//
//  Author:      Mehdy MORVAN
//  Date:        09/04/2025
//  Description: Test file for the system manager
//
///////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>
#include "System.hpp"
#include "Coordinator.hpp"

namespace parallax::ecs {
    class MockCoordinator : public Coordinator {};

    // Mock systems for testing
    class MockQuerySystem : public AQuerySystem {
    public:
        const Signature& getSignature() const override {
            return signature;
        }

        Signature signature;
    };

    class MockGroupSystem : public AGroupSystem {};

    // Invalid system that doesn't inherit correctly
    class InvalidSystem {};

    class SystemTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Initialize the static coordinator
            System::coord = std::make_shared<MockCoordinator>();
        }

        void TearDown() override {
            // Clean up
            System::coord.reset();
        }

        SystemManager systemManager;
    };

    // System Base Class Tests
    TEST_F(SystemTest, CoordinatorInitialization) {
        ASSERT_NE(System::coord, nullptr);
    }

    // AQuerySystem Tests
    TEST_F(SystemTest, QuerySystemSignature) {
        auto mockSystem = std::make_shared<MockQuerySystem>();

        // Test initial signature
        Signature emptySignature;
        EXPECT_EQ(mockSystem->getSignature(), emptySignature);

        // Test modified signature
        Signature newSignature;
        newSignature.set(1, true); // Set some bit
        mockSystem->signature = newSignature;
        EXPECT_EQ(mockSystem->getSignature(), newSignature);
    }

    TEST_F(SystemTest, QuerySystemEntities) {
        auto mockSystem = std::make_shared<MockQuerySystem>();

        // Test initial state
        EXPECT_TRUE(mockSystem->entities.empty());

        // Test adding entities
        Entity entity1 = 1;
        mockSystem->entities.insert(entity1);
        EXPECT_EQ(mockSystem->entities.size(), 1);
        EXPECT_TRUE(mockSystem->entities.contains(entity1));

        // Test adding more entities
        Entity entity2 = 2;
        mockSystem->entities.insert(entity2);
        EXPECT_EQ(mockSystem->entities.size(), 2);
        EXPECT_TRUE(mockSystem->entities.contains(entity2));

        // Test removing entities
        mockSystem->entities.erase(entity1);
        EXPECT_EQ(mockSystem->entities.size(), 1);
        EXPECT_FALSE(mockSystem->entities.contains(entity1));
        EXPECT_TRUE(mockSystem->entities.contains(entity2));
    }

    // SystemManager Tests
    TEST_F(SystemTest, RegisterQuerySystem) {
        // Register a system
        auto system = systemManager.registerQuerySystem<MockQuerySystem>();
        ASSERT_NE(system, nullptr);

        // Try to register the same system again
        auto duplicateSystem = systemManager.registerQuerySystem<MockQuerySystem>();
        EXPECT_EQ(duplicateSystem, nullptr);
    }

    TEST_F(SystemTest, RegisterGroupSystem) {
        // Register a system
        auto system = systemManager.registerGroupSystem<MockGroupSystem>();
        ASSERT_NE(system, nullptr);

        // Try to register the same system again
        auto duplicateSystem = systemManager.registerGroupSystem<MockGroupSystem>();
        EXPECT_EQ(duplicateSystem, nullptr);
    }

    class SystemImplementationTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Setup code
            parallax::ecs::System::coord = std::make_shared<parallax::ecs::MockCoordinator>();

            // Register systems
            querySystem = systemManager.registerQuerySystem<parallax::ecs::MockQuerySystem>();
            groupSystem = systemManager.registerGroupSystem<parallax::ecs::MockGroupSystem>();

            // Set signatures
            querySignature.set(0, true); // System requires component 0
            querySystem->signature = querySignature;
            systemManager.setSignature<parallax::ecs::MockQuerySystem>(querySignature);
        }

        void TearDown() override {
            parallax::ecs::System::coord.reset();
        }

        parallax::ecs::SystemManager systemManager;
        std::shared_ptr<parallax::ecs::MockQuerySystem> querySystem;
        std::shared_ptr<parallax::ecs::MockGroupSystem> groupSystem;
        parallax::ecs::Signature querySignature;
    };

    TEST_F(SystemImplementationTest, EntityDestroyedRemovesFromAllSystems) {
        // Add entity to the system
        parallax::ecs::Entity entity = 1;
        querySystem->entities.insert(entity);

        // Destroy entity
        systemManager.entityDestroyed(entity, querySignature);

        // Verify entity was removed
        EXPECT_FALSE(querySystem->entities.contains(entity));
    }

    TEST_F(SystemImplementationTest, EntitySignatureChangedAddsToMatchingSystems) {
        parallax::ecs::Entity entity = 1;
        parallax::ecs::Signature oldSignature; // Empty
        parallax::ecs::Signature newSignature;
        newSignature.set(0, true); // Now matches querySystem

        // Initially not in system
        ASSERT_FALSE(querySystem->entities.contains(entity));

        // Change signature
        systemManager.entitySignatureChanged(entity, oldSignature, newSignature);

        // Should be added to system
        EXPECT_TRUE(querySystem->entities.contains(entity));
    }

    TEST_F(SystemImplementationTest, EntitySignatureChangedRemovesFromNonMatchingSystems) {
        parallax::ecs::Entity entity = 1;
        parallax::ecs::Signature oldSignature;
        oldSignature.set(0, true); // Initially matches querySystem
        parallax::ecs::Signature newSignature; // Empty, no longer matches

        // Add to system
        querySystem->entities.insert(entity);

        // Change signature
        systemManager.entitySignatureChanged(entity, oldSignature, newSignature);

        // Should be removed from system
        EXPECT_FALSE(querySystem->entities.contains(entity));
    }

    TEST_F(SystemImplementationTest, EntitySignatureChangedHandlesMultipleSystems) {
        // Register another system with different signature
        class AnotherMockQuerySystem : public AQuerySystem {
        public:
            const Signature& getSignature() const override {
                return signature;
            }

            Signature signature;
        };
        auto otherSystem = systemManager.registerQuerySystem<AnotherMockQuerySystem>();
        parallax::ecs::Signature otherSignature;
        otherSignature.set(1, true); // This system requires component 1
        otherSystem->signature = otherSignature;

        parallax::ecs::Entity entity = 1;
        parallax::ecs::Signature oldSignature;
        oldSignature.set(0, true); // Matches only querySystem

        parallax::ecs::Signature newSignature;
        newSignature.set(1, true); // Matches only otherSystem

        // Add to first system
        querySystem->entities.insert(entity);
        ASSERT_TRUE(querySystem->entities.contains(entity));
        ASSERT_FALSE(otherSystem->entities.contains(entity));

        // Change signature
        systemManager.entitySignatureChanged(entity, oldSignature, newSignature);

        // Should move from first to second system
        EXPECT_FALSE(querySystem->entities.contains(entity));
        EXPECT_TRUE(otherSystem->entities.contains(entity));
    }
}
