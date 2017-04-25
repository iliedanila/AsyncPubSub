#include "arguments.hpp"

#include <algorithm>

Arguments::Arguments(int argc, const char * argv[])
{
    for (auto i = 0; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
}

bool Arguments::HasArgument(const std::string& argument)
{
    auto it = std::find(args.begin(), args.end(), argument);
    return it != args.end();
}

std::vector<std::string> Arguments::GetParameters(
    const std::string& argument, 
    std::size_t count)
{
    auto it = std::find(args.begin(), args.end(), argument);
    if (it == args.end())
        return std::vector<std::string>();

    std::vector<std::string> result(it + 1, it + count + 1);
    args.erase(it, it + count + 1);
    return result;
}

