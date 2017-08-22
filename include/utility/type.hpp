#ifndef GP_UTILITY_TYPE
#define GP_UTILITY_TYPE

#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace gp::utility {
    template <typename T>
    class TypeName {
        friend class TypeTranslator;
        static std::string name;
    public:
        std::string get()const {return name;}
    };

    template <typename T>
    std::string TypeName<T>::name = typeid(T).name();

    class TypeTranslator {
    private:
        using StringToType = std::unordered_map<std::string, const std::type_info*>;
        using TypeToString = std::unordered_map<std::type_index, std::string>;
    private:
        StringToType stringToType;
        TypeToString typeToString;
    public:
        template <typename TypeName>
        const std::type_info* getType(TypeName&& name) const {
            auto itr = stringToType.find(std::forward<TypeName>(name));
            if(itr == std::end(stringToType))return nullptr;
            else return itr->second;
        }
        std::string getTypeName(const std::type_info& type) const {
            auto itr = typeToString.find(type);
            if(itr == std::end(typeToString))return "";
            else return itr->second;
        }
        template <typename T>
        void setTypeNamePair(const std::string& name) {
            TypeName<T>::name = name;
            auto itr1 = typeToString.find(typeid(T));
            if (itr1 != std::end(typeToString)) { //update name
                stringToType.erase(itr1->second);
                stringToType.insert(std::make_pair(name, &typeid(T)));
                itr1->second = name;
            } else {
                typeToString[typeid(T)] = name;
                stringToType[name] = &typeid(T);
            }
        }
    };

    template <typename T>
    std::string typeName(){
        static TypeName<T> type;
        return type.get();
    }
}

#endif