#ifndef GP_GP_CONFIG
#define GP_GP_CONFIG

namespace gp {
    //settings for tree io
    namespace io {
        constexpr const char* NODE_HEADER = "+--";
        constexpr const char CONNECTION = '|';
        constexpr const char OFFSET_CHAR = ' ';
        constexpr std::size_t OFFSET_NUM = 4;
    }
}

#endif