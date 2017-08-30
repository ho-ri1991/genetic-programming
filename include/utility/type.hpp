#ifndef GP_UTILITY_TYPE
#define GP_UTILITY_TYPE

#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace gp::utility {
    //describes error type of the TypeInfo
    struct error{};
    //describes any type of TypeInfo (this is used in the PrognNode because PrognNode takes any types of chlid)
    struct any{};

    class TypeInfo {
    private:
        friend class StringToType;
        virtual const std::type_info* getTypePtr()const noexcept = 0;
        virtual void setName(const std::string&) = 0;
    public:
        virtual std::string name()const = 0;
    public:
        operator std::type_index() {return *getTypePtr();}
        bool operator==(const TypeInfo& other) const {return getTypePtr() == other.getTypePtr();}
        bool operator!=(const TypeInfo& other) const {return !(*this == other);}
    };

    template <typename T>
    class TypeInfoImpl: public TypeInfo {
    private:
        template <typename U>
        friend const TypeInfo& typeInfo();
        friend class StringToType;
        static const std::type_info& type_;
        static std::string name_;
    private:
        const std::type_info* getTypePtr()const noexcept override {return &type_;}
        void setName(const std::string& str)override {name_ = str;}
    public:
        std::string name()const override {return name_;}
    public:
        TypeInfoImpl(const TypeInfoImpl&) = delete;
        TypeInfoImpl(TypeInfoImpl&&) = delete;
        TypeInfoImpl& operator=(const TypeInfoImpl&) = delete;
        TypeInfoImpl& operator=(TypeInfoImpl&&) = delete;
    private:
        TypeInfoImpl() = default;
        ~TypeInfoImpl() = default;
    private:
        static TypeInfo& get(){
            static TypeInfoImpl type;
            return type;
        }
    };

    template <typename T>
    const std::type_info& TypeInfoImpl<T>::type_ = typeid(T);
    template <typename T>
    std::string TypeInfoImpl<T>::name_ = typeid(T).name();

    template <typename T>
    const TypeInfo& typeInfo(){
        return TypeInfoImpl<T>::get();
    }

    class StringToType {
    public:
        using Type = const TypeInfo&;
    private:
        using KeyType = std::string;
        using HoldingType = const TypeInfo*;
        using ContainerType = std::unordered_map<KeyType, HoldingType>;
    private:
        ContainerType container;
    public:
        template <typename String>
        Type operator()(String&& name)const {return container[name];}
        template <typename String>
        bool hasType(String&& name)const {return container.find(name) != std::end(container);}
        template <typename T, typename String>
        void setTypeNamePair(String&& name) {
            auto& type = TypeInfoImpl<T>::get();
            for(auto itr = std::begin(container); itr != std::end(container); ++itr){
                if(*itr->second == type){
                    container.erase(itr);
                    break;
                }
            }
            type.setName(name);
            container[name] = &type;
        }
    };
}

#endif