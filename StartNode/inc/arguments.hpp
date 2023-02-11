#ifndef arguments_h
#define arguments_h

#include <string>
#include <vector>

class Arguments {
   public:
    typedef std::vector<std::string>::iterator Iterator;

    Arguments(int argc, const char* argv[]);

    bool HasArgument(const std::string& argument);

    std::vector<std::string> GetParameters(const std::string& argument,
                                           uint32_t count);

   private:
    std::vector<std::string> args;
};

#endif /* arguments_h */
