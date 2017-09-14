#ifndef GP_GP_CONFIG
#define GP_GP_CONFIG

namespace gp {
    //settings for tree io
    namespace io {
        constexpr const char* NODE_HEADER = "+--";
        constexpr const char CONNECTION = '|';
        constexpr const char OFFSET_CHAR = ' ';
        constexpr std::size_t OFFSET_NUM = 4;

        static constexpr const char* ROOT_FIELD = "tree";
        static constexpr const char* NAME_FIELD = "name";
        static constexpr const char* RETURN_TYPE_FIELD = "return_type";
        static constexpr const char* ARGUMENT_TYPE_FIELD = "arguments";
        static constexpr const char* LOCAL_VARIABLE_FIELD = "local_variables";
        static constexpr const char* TREE_ENTITY_FIELD = "tree_entity";
        static constexpr const char* VARIABLE_TYPE_FIELD = "type";
    }
    namespace tree {
        constexpr utility::EvaluationContext::EvaluationCount defaultMaxEvaluationCount = 10000;
        constexpr utility::EvaluationContext::StackCount defaultMaxStackCount = 10000;
        constexpr std::size_t MAX_SUBROUTINE_ARGUMENT_NUM = 3;
    }
}

#endif
