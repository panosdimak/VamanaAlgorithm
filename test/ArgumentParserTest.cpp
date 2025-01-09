#include "ArgumentParser.h"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace std;

class ArgumentParserTest : public ::testing::Test
{
protected:
    ArgumentParser parser;

    void SetUp() override
    {
        optind = 1; // Reset getopt state
    }
};

TEST_F(ArgumentParserTest, ParsesArgumentsWithFlags)
{
    char *argv[] = {
        (char *)"program",
        (char *)"--k", (char *)"10",
        (char *)"--l", (char *)"20",
        (char *)"--r", (char *)"5",
        (char *)"--alpha", (char *)"0.5",
        (char *)"--base", (char *)"base_path",
        (char *)"--query", (char *)"query_path",
        (char *)"--groundtruth", (char *)"groundtruth_path",
        (char *)"--operation", (char *)"create-f",
        (char *)"--index", (char *)"index_path",
        (char *)"--search", (char *)"search_name",
        nullptr // Ensure null termination
    };
    int argc = 21;

    auto args = parser.ParseArguments(argc, argv);

    EXPECT_EQ(args.K, 10);
    EXPECT_EQ(args.L, 20);
    EXPECT_EQ(args.R, 5);
    EXPECT_DOUBLE_EQ(args.Alpha, 0.5);
    EXPECT_EQ(args.BaseDatasetPath, "base_path");
    EXPECT_EQ(args.QueryDatasetPath, "query_path");
    EXPECT_EQ(args.GroundTruthPath, "groundtruth_path");
    EXPECT_EQ(args.Operation, "create-f");
    EXPECT_EQ(args.IndexPath, "index_path");
    EXPECT_EQ(args.Experiment, "search_name");
}

TEST_F(ArgumentParserTest, ParsesArgumentsWithPositionals)
{
    char *argv[] = {
        (char *)"program",
        (char *)"50", (char *)"100",
        (char *)"60", (char *)"1.2",
        (char *)"data/dummy-data.bin",
        (char *)"data/dummy-queries.bin",
        (char *)"data/dummy-gt.bin",
        (char *)"create-f",
        (char *)"index_path",
        (char *)"search_name",
        nullptr // Ensure null termination
    };
    int argc = 11;

    auto args = parser.ParseArguments(argc, argv);

    EXPECT_EQ(args.K, 50);
    EXPECT_EQ(args.L, 100);
    EXPECT_EQ(args.R, 60);
    EXPECT_DOUBLE_EQ(args.Alpha, 1.2);
    EXPECT_EQ(args.BaseDatasetPath, "data/dummy-data.bin");
    EXPECT_EQ(args.QueryDatasetPath, "data/dummy-queries.bin");
    EXPECT_EQ(args.GroundTruthPath, "data/dummy-gt.bin");
}

TEST_F(ArgumentParserTest, ParsesArgumentsWithoutSearchName)
{
    char *argv[] = {
        (char *)"program",
        (char *)"--k", (char *)"10",
        (char *)"--l", (char *)"20",
        (char *)"--r", (char *)"5",
        (char *)"--alpha", (char *)"0.5",
        (char *)"--base", (char *)"base_path",
        (char *)"--query", (char *)"query_path",
        (char *)"--groundtruth", (char *)"groundtruth_path",
        (char *)"--operation", (char *)"create-f",
        (char *)"--index", (char *)"index_path",
        nullptr // Ensure null termination
    };
    int argc = 19;

    auto args = parser.ParseArguments(argc, argv);

    EXPECT_EQ(args.K, 10);
    EXPECT_EQ(args.L, 20);
    EXPECT_EQ(args.R, 5);
    EXPECT_DOUBLE_EQ(args.Alpha, 0.5);
    EXPECT_EQ(args.BaseDatasetPath, "base_path");
    EXPECT_EQ(args.QueryDatasetPath, "query_path");
    EXPECT_EQ(args.GroundTruthPath, "groundtruth_path");
    EXPECT_EQ(args.Operation, "create-f");
    EXPECT_EQ(args.IndexPath, "index_path");
    EXPECT_TRUE(args.Experiment.empty());
}

TEST_F(ArgumentParserTest, ThrowsOnMissingArguments)
{
    char *argv[] = {
        (char *)"program",
        (char *)"--k", (char *)"10",
        (char *)"--operation", (char *)"create-f"};
    int argc = 5;

    EXPECT_THROW(parser.ParseArguments(argc, argv), invalid_argument);
}

TEST_F(ArgumentParserTest, ThrowsOnInvalidOperation)
{
    char *argv[] = {
        (char *)"program",
        (char *)"--k", (char *)"10",
        (char *)"--l", (char *)"20",
        (char *)"--r", (char *)"5",
        (char *)"--alpha", (char *)"0.5",
        (char *)"--base", (char *)"base_path",
        (char *)"--query", (char *)"query_path",
        (char *)"--operation", (char *)"invalid-op",
        nullptr // Ensure null termination
    };
    int argc = 15;

    EXPECT_THROW(parser.ParseArguments(argc, argv), invalid_argument);
}

TEST_F(ArgumentParserTest, ThrowsOnMissingSearchNameInSearchOperation)
{
    char *argv[] = {
        (char *)"program",
        (char *)"--k", (char *)"10",
        (char *)"--l", (char *)"20",
        (char *)"--r", (char *)"5",
        (char *)"--alpha", (char *)"0.5",
        (char *)"--base", (char *)"base_path",
        (char *)"--query", (char *)"query_path",
        (char *)"--operation", (char *)"search",
        (char *)"--index", (char *)"index_path",
        nullptr // Ensure null termination
    };
    int argc = 17;

    EXPECT_THROW(parser.ParseArguments(argc, argv), invalid_argument);
}

TEST_F(ArgumentParserTest, ValidateThrowsOnInvalidData)
{
    ParsedArguments args;

    // Invalid K
    args.K = -1;
    args.L = 10;
    args.R = 5;
    args.Alpha = 0.5;
    args.BaseDatasetPath = "valid_path";
    args.QueryDatasetPath = "valid_query";
    args.Operation = "create-f";
    args.IndexPath = "valid_index";

    EXPECT_THROW(parser.Validate(args), invalid_argument);

    // Invalid Operation
    args.K = 10;
    args.Operation = "invalid";
    EXPECT_THROW(parser.Validate(args), invalid_argument);

    // Missing Experiment in Search Operation
    args.Operation = "search";
    args.Experiment = "";
    EXPECT_THROW(parser.Validate(args), invalid_argument);
}