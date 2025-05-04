#include <gtest/gtest.h>

#include <quetzal/geography.hpp>

#include <filesystem>
#include <iostream>
#include <mp-units/ostream.h>

namespace utf = ::testing;

TEST(GeographyTest, gdalcppTest)
{
    auto file = std::filesystem::current_path() / "data/bio1.tif";

    quetzal::geography::gdalcpp::Dataset data(file.string());

    std::cout << "Name:\t" << data.dataset_name() << std::endl;
    std::cout << "Driver:\t " << data.driver_name() << std::endl;
    std::cout << "Width:\t" << data.width() << std::endl;
    std::cout << "Height:\t" << data.height() << std::endl;
    std::cout << "Depth:\t" << data.depth() << std::endl;
    std::cout << "Coefs:\t";

    auto v = data.affine_transformation_coefficients();
}

TEST(GeographyTest, resolution)
{
    using decimal_degree = double;
    decimal_degree lat_resolution = 2.0;
    decimal_degree lon_resolution = 1.0;
    quetzal::geography::resolution<decimal_degree> res(lat_resolution, lon_resolution);
    EXPECT_EQ(res.lat(), lat_resolution);
    EXPECT_EQ(res.lon(), lon_resolution);
    auto copy = res;
    copy.lon(2.0);
    EXPECT_NE(copy, res);
}

TEST(GeographyTest, coordinates)
{
    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;
    using coord_type = quetzal::geography::latlon;
    coord_type paris(48.856614, 2.3522219000000177);
    coord_type moscow(55.7522200, 37.6155600);
    bool b = paris < moscow;
    EXPECT_TRUE(b);
    auto computed = paris.great_circle_distance_to(moscow);
    auto expected = 2486.22 * km ;
    auto EPSILON = 1.0 * km; // a 1-km error is acceptable.
    auto diff = expected - computed;
    EXPECT_LT(diff, EPSILON);
    EXPECT_LT(-diff, EPSILON);
}

TEST(GeographyTest, extent)
{
    using decimal_degree = double;
    using extent_type = quetzal::geography::extent<decimal_degree>;
    decimal_degree paris_lat = 48.8566;
    decimal_degree paris_lon = 2.3522;
    decimal_degree moscow_lat = 55.7522;
    decimal_degree moscow_lon = 37.6155;
    extent_type extent(paris_lat, moscow_lat, paris_lon, moscow_lon);
    EXPECT_EQ(extent.lat_min(), paris_lat);
    EXPECT_EQ(extent.lat_max(), moscow_lat);
    EXPECT_EQ(extent.lon_min(), paris_lon);
    EXPECT_EQ(extent.lon_max(), moscow_lon);
    auto copy = extent;
    copy.lat_min(0.0);
    EXPECT_NE(copy, extent);
}

TEST(GeographyTest, raster)
{
    using time_type = unsigned int;
    using raster_type = quetzal::geography::raster<time_type>;
    using latlon = raster_type::latlon;

    auto file = std::filesystem::current_path() / "data/bio1.tif";

    auto bio1 = raster_type::from_file(file, {2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010});

    EXPECT_EQ(bio1.times().size(), 10);
    EXPECT_EQ(bio1.locations().size(), 9);
    EXPECT_EQ(bio1.origin(), raster_type::latlon(52., -5.));

    auto f = bio1.to_view();
    auto x = bio1.to_descriptor( bio1.origin() );
    auto t = bio1.times().front();
    // EXPECT_TRUE( f( x, t ).has_value() );

    auto space = bio1.locations() |
                 ranges::views::transform([&](auto i) { return bio1.to_latlon(i); }) |
                 ranges::to<std::vector>();

    auto c0 = space.at(0);
    auto c1 = space.at(1);
    auto c2 = space.at(2);
    auto c3 = space.at(3);
    auto c4 = space.at(4);
    auto c5 = space.at(5);
    auto c6 = space.at(6);
    auto c7 = space.at(7);
    auto c8 = space.at(8);

    /*
    //  Expected dataSet structure
    //
    //  * origin at Lon -5, Lat 52.
    //  * 9 cells
    //  * 10 layers
    //  * pixel size (-5, 5)
    //  * East and South limits ARE NOT in spatial extent
    //
    //
    // origin        -5        0       5      10
    //           \   /         /       /      /
    //            \ /         /       /      /
    //         52   * ------ * ----- * ---- *
    //              |    .   |   .   |   .
    //              |   c0   |  c1   |  c2
    //         47   * ------ * ----- * ---- *
    //              |    .   |   .   |   .
    //              |   c3   |  c4   |  c5
    //         42   * ------ * ----- * ---- *
    //              |   .    |   .   |   .
    //              |   c6   |  c7   |  c8
    //         37   *        *       *      *
    //
    //         0    -  -----> + 180    Xsize positive in decimal degre (east direction positive)
    //
    //         90
    //         +
    //              |
    //              |
    //         -    \/ Y size negative in decimal degree (south direction negative)
    //         0
    */

    // Corners
    latlon expected_top_left(52., -5.);
    latlon expected_bottom_right(37., 10.);
    EXPECT_TRUE(bio1.contains(expected_top_left));
    EXPECT_FALSE(bio1.contains(expected_bottom_right));

    // In boxes
    latlon in_box_0(51., -4.5);
    latlon in_box_1(48.32, 1.5);
    latlon in_box_6(37.21, -3.2);
    latlon in_box_8(38.21, 7.125);

    // Out of all boxes
    latlon out_north(55., 0.);

    // At boxes limits
    latlon W_c0_limit(48., -5.);
    latlon N_c0_limit(52., -2.);
    latlon c0_c3_limit(47., -2.);
    latlon W_c0_c3_limit(47., -5);
    latlon c0_c1_c3_c4_limit(47., 0.);
    latlon E_c2_c5_limit(47., 10.);

    // In boxes
    EXPECT_EQ(bio1.to_centroid(in_box_0), c0);
    EXPECT_EQ(bio1.to_centroid(in_box_1), c1);
    EXPECT_EQ(bio1.to_centroid(in_box_6), c6);
    EXPECT_EQ(bio1.to_centroid(in_box_8), c8);

    // At boxes limits
    EXPECT_EQ(bio1.to_centroid(W_c0_limit), c0);
    EXPECT_EQ(bio1.to_centroid(N_c0_limit), c0);
    EXPECT_EQ(bio1.to_centroid(c0_c3_limit), c3);
    EXPECT_EQ(bio1.to_centroid(W_c0_c3_limit), c3);
    EXPECT_EQ(bio1.to_centroid(c0_c1_c3_c4_limit), c4);

    // Out of all boxes
    // bio1.to_centroid(out_north); // assertion raised
    // bio1.to_centroid(E_c2_c5_limit); // assertion raised
}

TEST(GeographyTest, landscape)
{
    using time_type = int;
    using landscape_type = quetzal::geography::landscape<std::string, time_type>;

    auto file1 = std::filesystem::current_path() / "data/bio1.tif";
    auto file2 = std::filesystem::current_path() / "data/bio12.tif";

    try
    {

        auto env = landscape_type::from_file({{"bio1", file1}, {"bio12", file2}},
                                              {2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010});

        EXPECT_EQ(env.num_variables(), 2);

        landscape_type::latlon Bordeaux(44.5, 0.34);

        EXPECT_TRUE(env.contains(Bordeaux));
        EXPECT_TRUE(env.contains(env.to_centroid(Bordeaux)));

        const auto &f = env["bio1"].to_view();
        const auto &g = env["bio12"].to_view();

        EXPECT_EQ(env.times().size(), 10);
        EXPECT_LT(env.locations().size(), 100);

        for (auto t : env.times())
        {
            for (auto x : env.locations())
            {
                EXPECT_TRUE(f(x, t).has_value());

