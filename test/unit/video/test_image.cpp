/**
 * @file test_image.cpp
 * @author Kevin Orbie
 * 
 * @brief Unit tests for the image functionality.
 */

/* ================== Include ================== */
/* Setup Google Testing Inferastructure */
#include <gtest/gtest.h>  // 

/* Standard C++ Libraries */
// None

/* Custom C++ Libraries */
#include "video/image.h"


/* ============= Tests Declaration ============= */

TEST(TestImage, ImageEmptyInitially) {
    /* Setup */
    Image image = {};

    /* Execute */
    // None

    /* Validate */
    EXPECT_EQ(image.getWidth(), 0);
    EXPECT_EQ(image.getHeight(), 0);
    EXPECT_EQ(image.getFormat(), PixelFormat::EMPTY);
}

TEST(TestImage, ImageSetToZero) {
    /* Setup */
    Image image = {8,8,PixelFormat::YUV};
    
    /* Execute */
    image.zero();

    /* Validate */
    EXPECT_EQ(image.getWidth(), 8);
    EXPECT_EQ(image.getHeight(), 8);
    EXPECT_EQ(image.getFormat(), PixelFormat::YUV);

    uint8_t *data = image.getData();
    for (int index=0; index < 8*8; index++) {
        EXPECT_EQ(data[index], 0);
    }
}

TEST(TestImage, CopyImageViewSameFormat) {
    /* Setup */
    Image image_src = {8, 8, PixelFormat::YUV422P};
    Image image_dst = {8, 8, PixelFormat::YUV422P};
    ImageView view_src = image_src.view();
    ImageView view_dst = image_dst.view();

    // Fill Source Y-Pane with incrementing value
    uint8_t *src_data = image_src.getData();
    for (uint8_t i = 0; i < 8*8; i++) { src_data[i] = i; };

    // Fill Destination Y-Pane with 0s
    image_dst.zero();

    /* Execute */
    view_dst.copyFrom(view_src);

    /* Validate: Destination Y-Plane equals source Y-Plane */
    uint8_t *dst_data = image_dst.getData();
    for (uint8_t i = 0; i < 8*8; i++) { EXPECT_EQ(dst_data[i], src_data[i]); };
}

TEST(TestImage, CopyImageViewConvertFormat) {
    /* Setup */
    Image image_src = {8, 8, PixelFormat::YUV422P};
    Image image_dst = {8, 8, PixelFormat::YUV};
    ImageView view_src = image_src.view();
    ImageView view_dst = image_dst.view();

    // Fill Source Y-Pane with incrementing value
    uint8_t *src_data = image_src.getData();
    for (uint8_t i = 0; i < 8*8; i++) { src_data[i] = i; };

    // Fill Destination Y-Pane with 0s
    image_dst.zero();

    /* Execute */
    view_dst.copyFrom(view_src);

    /* Validate: Destination Y values equal source Y-Plane */
    uint8_t *dst_data = image_dst.getData();
    for (uint8_t i = 0; i < 8*8; i++) { EXPECT_EQ(dst_data[3*i], src_data[i]); };
}

TEST(TestImage, CopyConstructorSameFormat) {
    /* Setup */
    Image image_src = {8, 8, PixelFormat::YUV422P};
    ImageView view_src = image_src.view();

    // Fill Source Y-Pane with incrementing value
    uint8_t *src_data = image_src.getData();
    for (uint8_t i = 0; i < 8*8; i++) { src_data[i] = i; };

    /* Execute */
    Image image_dst = Image(view_src);

    /* Validate: Verify the new imageformat */
    EXPECT_EQ(image_dst.getFormat(), PixelFormat::YUV422P);
    /* Validate: Destination Y-Plane equals source Y-Plane */
    uint8_t *dst_data = image_dst.getData();
    for (uint8_t i = 0; i < 8*8; i++) { EXPECT_EQ(dst_data[i], src_data[i]); };
}

TEST(TestImage, CopyConstructorConvertFormat) {
    /* Setup */
    Image image_src = {8, 8, PixelFormat::YUV422P};
    ImageView view_src = image_src.view();

    // Fill Source Y-Pane with incrementing value
    uint8_t *src_data = image_src.getData();
    for (uint8_t i = 0; i < 8*8; i++) { src_data[i] = i; };

    /* Execute */
    Image image_dst = Image(view_src, PixelFormat::YUV);

    /* Validate: Verify the new imageformat */
    EXPECT_EQ(image_dst.getFormat(), PixelFormat::YUV);
    /* Validate: Destination Y-Plane equals source Y-Plane */
    uint8_t *dst_data = image_dst.getData();
    for (uint8_t i = 0; i < 8*8; i++) { EXPECT_EQ(dst_data[3*i], src_data[i]); };
}

TEST(TestImage, YUV422toYUV) {
    /* Setup */
    Image image = {8,8,PixelFormat::YUV422};

    /* Execute */
    image.to(PixelFormat::YUV);

    /* Validate */
    EXPECT_EQ(image.getFormat(), PixelFormat::YUV);
}

TEST(TestImage, YUV422PtoYUV) {
    /* Setup */
    Image image = {8,8,PixelFormat::YUV422P};

    /* Execute */
    image.to(PixelFormat::YUV);

    /* Validate */
    EXPECT_EQ(image.getFormat(), PixelFormat::YUV);
}

TEST(TestImage, YUV422toYUV422P) {
    /* Setup */
    Image image = {8,8,PixelFormat::YUV422};

    /* Execute */
    image.to(PixelFormat::YUV422P);

    /* Validate */
    EXPECT_EQ(image.getFormat(), PixelFormat::YUV422P);
}

