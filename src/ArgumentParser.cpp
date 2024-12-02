#include "ArgumentParser.h"
#include <iostream>
#include <cstdlib>

ParsedArguments ArgumentParser::ParseArguments(int argc, char *argv[])
{
    ParsedArguments args;

    // Define long options for flag parsing
    static struct option longOptions[] = {
        {"k", required_argument, nullptr, 'k'},
        {"l", required_argument, nullptr, 'l'},
        {"r", required_argument, nullptr, 'r'},
        {"alpha", required_argument, nullptr, 'a'},
        {"base", required_argument, nullptr, 'b'},
        {"query", required_argument, nullptr, 'q'},
        {"groundtruth", required_argument, nullptr, 'g'},
        {nullptr, 0, nullptr, 0} // Termination of options
    };

    // Parse flags
    bool flagsUsed = false;
    int opt;
    while ((opt = getopt_long(argc, argv, "k:l:r:a:b:q:g:", longOptions, nullptr)) != -1)
    {
        flagsUsed = true; // At least one flag detected
        switch (opt)
        {
        case 'k':
            args.K = stoi(optarg);
            break;
        case 'l':
            args.L = stoi(optarg);
            break;
        case 'r':
            args.R = stoi(optarg);
            break;
        case 'a':
            args.Alpha = stod(optarg);
            break;
        case 'b':
            args.BaseDatasetPath = optarg;
            break;
        case 'q':
            args.QueryDatasetPath = optarg;
            break;
        case 'g':
            args.GroundTruthPath = optarg;
            break;
        default:
            throw invalid_argument("Invalid flag or missing argument.");
        }
    }

    // If no flags were used, fallback to positional arguments
    if (!flagsUsed)
    {
        int expectedArgs = 7; // Minimum required args for positional parsing
        if (argc < expectedArgs || argc > expectedArgs + 1)
        {
            throw invalid_argument(
                "Incorrect number of positional arguments.\n"
                "Usage: <k> <l> <r> <alpha> <base_dataset> <query_dataset> [groundtruth_dataset]");
        }

        // Parse positional arguments
        args.K = stoi(argv[optind++]);
        args.L = stoi(argv[optind++]);
        args.R = stoi(argv[optind++]);
        args.Alpha = stod(argv[optind++]);
        args.BaseDatasetPath = argv[optind++];
        args.QueryDatasetPath = argv[optind++];
        if (argc == expectedArgs + 1)
        {
            args.GroundTruthPath = argv[optind++];
        }
    }

    // Validate the parsed arguments
    Validate(args);

    return args;
}

void ArgumentParser::DisplayParsedArguments(const ParsedArguments &args)
{
    cout << "Parsed Arguments:" << endl;
    cout << " - K: " << args.K << endl;
    cout << " - L: " << args.L << endl;
    cout << " - R: " << args.R << endl;
    cout << " - Alpha: " << args.Alpha << endl;
    cout << " - Base Dataset Path: " << args.BaseDatasetPath << endl;
    cout << " - Query Dataset Path: " << args.QueryDatasetPath << endl;
    if (!args.GroundTruthPath.empty())
    {
        cout << " - Ground Truth Path: " << args.GroundTruthPath << endl;
    }
}

void ArgumentParser::Validate(const ParsedArguments &args)
{
    if (args.K <= 0 || args.L <= 0 || args.R <= 0 || args.Alpha <= 0.0 || args.BaseDatasetPath.empty() || args.QueryDatasetPath.empty())
    {
        throw invalid_argument(
            "All arguments must be positive, and dataset paths must be non-empty.\n"
            "Usage (flags): --k <value> --l <value> --r <value> --alpha <value> --base <path> --query <path> [--groundtruth <path>]\n"
            "Usage (positional): <k> <l> <r> <alpha> <base_dataset> <query_dataset> [groundtruth_dataset]");
    }
}
