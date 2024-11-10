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

TEST(TestPose, BaseUnitVectors) {
    /* Setup */
    Pose pose = {};

    /* Execute */
    Eigen::Vector3d unit_x = pose.UnitX();
    Eigen::Vector3d unit_y = pose.UnitY();
    Eigen::Vector3d unit_z = pose.UnitZ();

    /* Validate */
    EXPECT_EQ(unit_x[0], 1.0);
    EXPECT_EQ(unit_x[1], 0.0);
    EXPECT_EQ(unit_x[2], 0.0);

    EXPECT_EQ(unit_y[0], 0.0);
    EXPECT_EQ(unit_y[1], 1.0);
    EXPECT_EQ(unit_y[2], 0.0);

    EXPECT_EQ(unit_z[0], 0.0);
    EXPECT_EQ(unit_z[1], 0.0);
    EXPECT_EQ(unit_z[2], 1.0);
}

TEST(TestPose, PoseInverse) {
    /* Setup */
    Pose pose_AB = {};
    pose_AB.translate({0.2, 0.3, 0.4});
    pose_AB.rotate(0.2, 0.3, 0.4);
    Pose pose_BA = pose_AB.inverse();

    /* Execute */
    Pose nop_pose = pose_AB * pose_BA;

    /* Validate */
    EXPECT_NEAR(nop_pose.xrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.yrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.zrot(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.x(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.y(), 0.0, 1e-7);
    EXPECT_NEAR(nop_pose.z(), 0.0, 1e-7);
}

TEST(TestPose, PositionMulBasic) {
    /* Setup */
    Pose pose_CA = {};
    position_t pos_AB_A = {1.0, 0.0, 0.0};

    /* Execute */
    position_t pos_CB_C = pose_CA * pos_AB_A;

    /* Validate */
    EXPECT_NEAR(pos_CB_C[0], pos_AB_A[0], 1e-7);
    EXPECT_NEAR(pos_CB_C[1], pos_AB_A[1], 1e-7);
    EXPECT_NEAR(pos_CB_C[2], pos_AB_A[2], 1e-7);
}

TEST(TestPose, PositionMulRotation) {
    /* Setup */
    Pose pose_CA = {};
    pose_CA.rotate({0.0, 3.14159265358979, 0.0});  // 180 Degree rotation round Y axis
    position_t pos_AB_A = {1.0, 0.0, 0.0};

    /* Execute */
    position_t pos_CB_C = pose_CA * pos_AB_A;

    /* Validate */
    EXPECT_NEAR(pos_CB_C[0], -pos_AB_A[0], 1e-7);
    EXPECT_NEAR(pos_CB_C[1],  pos_AB_A[1], 1e-7);
    EXPECT_NEAR(pos_CB_C[2], -pos_AB_A[2], 1e-7);
}

TEST(TestPose, PositionMulTranslation) {
    /* Setup */
    Pose pose_CA = {};
    pose_CA.translate({1.0, 1.0, 1.0});
    position_t pos_AB_A = {0.0, 0.0, 0.0};

    /* Execute */
    position_t pos_CB_C = pose_CA * pos_AB_A;

    /* Validate */
    EXPECT_NEAR(pos_CB_C[0], pos_AB_A[0] + 1.0, 1e-7);
    EXPECT_NEAR(pos_CB_C[1], pos_AB_A[1] + 1.0, 1e-7);
    EXPECT_NEAR(pos_CB_C[2], pos_AB_A[2] + 1.0, 1e-7);
}

TEST(TestPose, RotateAndInverseX) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.rotate( 1.57079633, 0, 0);
    EXPECT_NEAR(pose.xrot(), 1.57079633, 1e-7);

    /* Test the inverse rotation. */
    pose.rotate(-1.57079633, 0, 0);
    EXPECT_NEAR(pose.xrot(), 0.0, 1e-7);
}

TEST(TestPose, RotateAndInverseY) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.rotate( 0, 1.57079633, 0);
    EXPECT_NEAR(pose.yrot(), 1.57079633, 1e-7);

    /* Test the inverse rotation. */
    pose.rotate(0, -1.57079633, 0);
    EXPECT_NEAR(pose.yrot(), 0.0, 1e-7);
} 

TEST(TestPose, RotateAndInverseZ) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.rotate(0, 0, 1.57079633);
    EXPECT_NEAR(pose.zrot(), 1.57079633, 1e-7);

    /* Test the inverse rotation. */
    pose.rotate(0, 0, -1.57079633);
    EXPECT_NEAR(pose.zrot(), 0.0, 1e-7);
} 

TEST(TestPose, TranslateInverseX) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.translate({1.0, 0, 0});
    EXPECT_NEAR(pose.x(), 1.0, 1e-7);

    /* Test the inverse rotation. */
    pose.translate({-1.0, 0, 0});
    EXPECT_NEAR(pose.x(), 0.0, 1e-7);
}

TEST(TestPose, TranslateInverseY) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.translate({0, 1.0, 0});
    EXPECT_NEAR(pose.y(), 1.0, 1e-7);

    /* Test the inverse rotation. */
    pose.translate({0, -1.0, 0});
    EXPECT_NEAR(pose.y(), 0.0, 1e-7);
}

TEST(TestPose, TranslateInverseZ) {
    Pose pose = {};

    /* Test a simple rotation. */
    pose.translate({0, 0, 1.0});
    EXPECT_NEAR(pose.z(), 1.0, 1e-7);

    /* Test the inverse rotation. */
    pose.translate({0, 0, -1.0});
    EXPECT_NEAR(pose.z(), 0.0, 1e-7);
}
