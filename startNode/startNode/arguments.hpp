#ifndef arguments_h
#define arguments_h

#include <map>
#include <vector>
#include <string>

class Arguments
{
public:
    explicit Arguments(int argc, const char * argv[]);
    std::vector<std::string> GetArgument(std::string identifier);
    ~Arguments();
    
private:
    std::map<std::string, std::vector<std::string>> args;
};

Arguments::Arguments(int argc, const char * argv[])
{
    for(int i = 1; i < argc; i = i + 2)
    {
        args[argv[i]].push_back(argv[i + 1]);
    }
}

Arguments::~Arguments()
{
    
}

std::vector<std::string> Arguments::GetArgument(std::string identifier)
{
    return args[identifier];
}


#endif /* arguments_h */
