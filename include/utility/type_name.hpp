#ifndef GP_UTILITY_TYPE_NAME
#define GP_UTILITY_TYPE_NAME

#include <string>

namespace gp::utility {
    class TypeNameInterface {
    public:
        virtual std::string get()const = 0;
        virtual void set(const char*) = 0;
    };

    template <typename T>
    class TypeName {
        static std::string name;
    public:
        std::string get()const {return name;}
        void set(const char* name_){name = name_;}
    };

    template <typename T>
    std::string TypeName<T>::name = typeid(T).name();

    template <typename T>
    std::string typeName(){
        static TypeName<T> type;
        return type.get();
    }
}

#endif