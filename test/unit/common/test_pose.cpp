/**
 * @file test_pose.cpp
 * @author Kevin Orbie
 * 
 * @brief Unit tests for the pose functionality.
 */

/* ================== Include ================== */
/* Setup Google Testing Inferastructure */
#include <gtest/gtest.h>  // 

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "common/pose.h"


/* ============= Tests Declaration ============= */

TEST(TestPose, PoseInverse) {
    Pose pose_AB = {};
    pose_AB.translate({0.2, 0.3, 0.4});
    pose_AB.rotate(0.2, 0.3, 0.4);
    Pose pose_BA = pose_AB.inverse();

    /* Test for Null Operation Pose. */
    Pose nop_pose = pose_AB * pose_BA;
    EXPECT_NEAR(nop_pose.xrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.yrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.zrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.x(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.y(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.z(), 0.0, 1e-7);
}

TEST(TestPose, RotateInverse) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.rotate( 1.57079633, 0, 0);
    EXPECT_NEAR(pose.xrot(), 1.57079633, 1e-7);

    /* Test the inverse rotation. */
    pose.rotate(-1.57079633, 0, 0);
    EXPECT_NEAR(pose.xrot(), 0.0, 1e-7);
} 

TEST(TestPose, TranslateInverse) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.translate({1.0, 0, 0});
    EXPECT_NEAR(pose.x(), 1.0, 1e-7);

    /* Test the inverse rotation. */
    pose.translate({-1.0, 0, 0});
    EXPECT_NEAR(pose.x(), 0.0, 1e-7);
} 