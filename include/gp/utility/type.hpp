#ifndef GP_UTILITY_TYPE
#define GP_UTILITY_TYPE

#include <string>
#include <unordered_map>
#include <functional>
#include "reference.hpp"
#include "left_hand_value.hpp"

namespace gp::utility {
    //describes error type of the TypeInfo
    struct error{};
    //describes any type of TypeInfo (this is used in the PrognNode because PrognNode takes any types of chlid)
    struct any{};

    template <typename T>
    class TypeInfoImpl;

    class TypeInfo {
    private:
        friend class StringToType;
        virtual void setName(const std::string&) = 0;
    public:
        virtual std::string name()const = 0;
    public://expect singleton
        bool operator==(const TypeInfo& other) const {return this == &other;}
        bool operator!=(const TypeInfo& other) const {return !(*this == other);}
        virtual bool isAnyType()const noexcept {return false;}
        virtual bool isErrorType()const noexcept {return false;}
        virtual bool isReferenceType()const noexcept {return false;}
        virtual bool isLeftHandValueType()const noexcept {return false;}
        virtual const TypeInfo& removeReferenceType()const noexcept {return *this;}
        virtual const TypeInfo& removeLeftHandValueType()const noexcept {return *this;}
    private:
        template <typename T> friend class TypeInfoImpl;
        TypeInfo() = default;
    };

    template <typename T>
    class TypeInfoImpl: public TypeInfo {
    private:
        static std::string name_;
    private:
        void setName(const std::string& str)override {name_ = str;}
    public:
        std::string name()const override {return name_;}
        bool isAnyType()const noexcept override {return std::is_same_v<T, any>;}
        bool isErrorType()const noexcept override {return std::is_same_v<T, error>;}
    public:
        TypeInfoImpl(const TypeInfoImpl&) = delete;
        TypeInfoImpl(TypeInfoImpl&&) = delete;
        TypeInfoImpl& operator=(const TypeInfoImpl&) = delete;
        TypeInfoImpl& operator=(TypeInfoImpl&&) = delete;
    private:// singleton
        TypeInfoImpl() = default;
        ~TypeInfoImpl() = default;
    public:
        static TypeInfo& get(){
            static TypeInfoImpl type;
            return type;
        }
    };

    template <typename T>
    std::string TypeInfoImpl<T>::name_ = typeid(T).name();

    template <typename T>
    class TypeInfoImpl<Reference<T>>: public TypeInfo {
    private:
        static std::string name_;
    private:
        void setName(const std::string& str)override {name_ = str;}
    public:
        std::string name()const override {return name_;}
    public:
        TypeInfoImpl(const TypeInfoImpl&) = delete;
        TypeInfoImpl(TypeInfoImpl&&) = delete;
        TypeInfoImpl& operator=(const TypeInfoImpl&) = delete;
        TypeInfoImpl& operator=(TypeInfoImpl&&) = delete;
    private:// singleton
        TypeInfoImpl() = default;
        ~TypeInfoImpl() = default;
    public:
        bool isReferenceType()const noexcept override {return true;}
        const TypeInfo& removeReferenceType()const noexcept override {return TypeInfoImpl<T>::get();}
        static TypeInfo& get(){
            static TypeInfoImpl type;
            return type;
        }
    };

    template <typename T>
    std::string TypeInfoImpl<Reference<T>>::name_ = typeid(Reference<T>).name();

    template <typename T>
    class TypeInfoImpl<LeftHandValue<T>>: public TypeInfo {
    private:
        static std::string name_;
    private:
        void setName(const std::string& str)override {name_ = str;}
    public:
        std::string name()const override {return name_;}
    public:
        TypeInfoImpl(const TypeInfoImpl&) = delete;
        TypeInfoImpl(TypeInfoImpl&&) = delete;
        TypeInfoImpl& operator=(const TypeInfoImpl&) = delete;
        TypeInfoImpl& operator=(TypeInfoImpl&&) = delete;
    private:// singleton
        TypeInfoImpl() = default;
        ~TypeInfoImpl() = default;
    public:
        bool isLeftHandValueType()const noexcept override {return true;}
        const TypeInfo& removeLeftHandValueType()const noexcept override {return TypeInfoImpl<T>::get();}
        static TypeInfo& get(){
            static TypeInfoImpl type;
            return type;
        }
    };

    template <typename T>
    std::string TypeInfoImpl<LeftHandValue<T>>::name_ = typeid(Reference<T>).name();

    class TypeIndex {
    private:
        const TypeInfo& type;
    public:
        bool operator==(const TypeIndex& other)const {return type == other.type;}
        bool operator!=(const TypeIndex& other)const {return type != other.type;}
        TypeIndex(const TypeInfo& type_): type(type_){}
    public://hash function
        struct Hash {
            std::size_t operator()(const TypeIndex& key)const {return std::hash<const TypeInfo*>()(&key.type);}
        };
    };

    template <typename T>
    const TypeInfo& typeInfo(){
        return TypeInfoImpl<T>::get();
    }

    class StringToType {
    public:
        using Type = const TypeInfo&;
        using KeyType = std::string;
        using HoldingType = const TypeInfo*;
        using ContainerType = std::unordered_map<KeyType, HoldingType>;
    private:
        ContainerType container;
    public:
        template <typename String>
        Type operator()(String&& name)const {return *container.find(std::forward<String>(name))->second;}
        template <typename String>
        bool hasType(String&& name)const {return container.find(std::forward<String>(name)) != std::end(container);}
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
