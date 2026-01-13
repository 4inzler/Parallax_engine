#include <gtest/gtest.h>
#include "Path.hpp"

class PathTestFixture : public ::testing::Test {
protected:
    void TearDown() override {
        parallax::Path::resetCache();
    }
};

TEST_F(PathTestFixture, ResolvePathRelativeToExe) {
    const std::filesystem::path exePath = parallax::Path::getExecutablePath();
    const std::filesystem::path resolvedPath = parallax::Path::resolvePathRelativeToExe("test.txt");

    EXPECT_EQ(resolvedPath, exePath.parent_path() / "test.txt");
}

TEST_F(PathTestFixture, ResolvePathRelativeToExeTwice) {
    const std::filesystem::path exePath = parallax::Path::getExecutablePath();
    const std::filesystem::path resolvedPath = parallax::Path::resolvePathRelativeToExe("test.txt");
    const std::filesystem::path resolvedPath2 = parallax::Path::resolvePathRelativeToExe("test2.txt");

    EXPECT_EQ(resolvedPath, exePath.parent_path() / "test.txt");
    EXPECT_EQ(resolvedPath2, exePath.parent_path() / "test2.txt");
}

TEST_F(PathTestFixture, ResolvePathRelativeToExeWithParent) {
    const std::filesystem::path exePath = parallax::Path::getExecutablePath();
    const std::filesystem::path resolvedPath = parallax::Path::resolvePathRelativeToExe("resources/test.txt");

    EXPECT_EQ(resolvedPath, exePath.parent_path() / "resources/test.txt");
}

/**
* @brief Test if the resolved paths are the same when called twice, check if cache works correctly
*/
TEST_F(PathTestFixture, ResolvePathRelativeToExeCache) {
    const std::filesystem::path resolvedPath = parallax::Path::resolvePathRelativeToExe("test.txt");
    const std::filesystem::path resolvedPath2 = parallax::Path::resolvePathRelativeToExe("test2.txt");

    EXPECT_EQ(resolvedPath.parent_path(), resolvedPath2.parent_path());
}

/**
* @brief Call twice the getExecutablePath method to check if the cache works correctly
*/
TEST_F(PathTestFixture, GetExecutablePathCache) {
    const std::filesystem::path exePath = parallax::Path::getExecutablePath();
    const std::filesystem::path exePath2 = parallax::Path::getExecutablePath();

    EXPECT_EQ(exePath, exePath2);
}

/**
 * @brief Test resetCache method
 */
TEST_F(PathTestFixture, ResetCache) {
    const std::filesystem::path exePath = parallax::Path::getExecutablePath();
    parallax::Path::resetCache();
    const std::filesystem::path exePath2 = parallax::Path::getExecutablePath();
    const std::filesystem::path resolvedPath = parallax::Path::resolvePathRelativeToExe("test.txt");

    EXPECT_EQ(exePath, exePath2);
    EXPECT_EQ(resolvedPath, exePath2.parent_path() / "test.txt");
}
