#ifndef GP_NODE_ARRAY_NODES
#define GP_NODE_ARRAY_NODES

#include "node_base.hpp"
#include <gp/utility/is_match_template.hpp>
#include <vector>
#include <algorithm>
#include <boost/lexical_cast.hpp>

namespace gp::node {
    //Array type for genetic_programming. this array does not support iterator
    template <typename T>
    class GpArray {
    private:
        using container_type = std::vector<T>;
        container_type container;
    public:
        using reference = container_type::reference;
        using const_reference = container_type::const_reference;
        using size_type = container_type::size_type;
        using difference_type = container_type::difference_type;
        using value_type = container_type::value_type;
        using pointer = container_type::pointer;
        using const_pointer = container_type::const_pointer;
    public:
        size_type size()const noexcept {return container.size();}
        size_type max_size()const noexcept {return container.max_size();}
        void resize(size_type sz){container.reserve(sz);}
        void resize(size_type sz, const T& c){container.resize(sz, c);}
        void resize(size_type sz, T c = T{}){container.resize(sz, c);}
        size_type capacity()const noexcept {return container.capacity();}
        bool empty()const noexcept {return container.empty();}
        void reserve(size_type sz){container.reserve(sz);}
        void shrink_to_fit(){container.shrink_to_fit();}
    public:
        reference operator[](size_type i){return container[i];}
        const_reference operator[](size_type i)const {return container[i];}
        reference at(size_type i){return container.at(i);}
        const_reference at(size_type i)const {return container.at(i);}
        T* data()noexcept {return container.data();}
        const T* data()const noexcept {return container.data();}
        reference front(){return container.front();}
        const_reference front()const {return container.front();}
        reference back(){return container.back();}
        const_reference back()const {return container.back();}
    public:
        template <typename InputIterator>
        void assign(InputIterator first, InputIterator last){container.assign(first, last);}
        void assign(size_type n, const T& c){container.assign(n, c);}
        void assign(std::initializer_list<T> l){container.assign(l);}
        void push_back(const T& c){container.push_back(c);}
        void push_back(T&& c){container.push_back(std::move(c));}
        template <typename ...Args>
        void emplace_back(Args&&... args){container.emplace_back(std::forward<Args>(args)...);}
        void pop_back(){container.pop_back();}
        void swap(GpArray& other)noexcept {container.swap(other.container);}
        void clear()noexcept {container.clear();}
    public:
        explicit GpArray(size_type n):container(n){}
        explicit GpArray(size_t n, const T& c):container(n, c){}
        template <typename InputIterator>
        explicit GpArray(InputIterator first, InputIterator last):container(first, last){}
        explicit GpArray(std::initializer_list<T> il):container(il){}
        GpArray(const GpArray&) = default;
        GpArray(GpArray&&) = default;
        GpArray& operator=(const GpArray&) = default;
        GpArray& operator=(GpArray&&) = default;
        ~GpArray() = default;

        friend bool operator==(const GpArray& lhs, const GpArray& rhs){return lhs.container == rhs.container;}
        friend std::istream& operator>>(std::istream& is, GpArray& arr) {
            using std::begin;
            using std::end;
            std::string line;
            is >> line;
            auto first = line.find('[');
            auto last = line.rfind(']');
            if(begin == std::string::npos || end == std::string::npos || last <= first)
                throw std::runtime_error("invalid GpArray string");

            GpArray org;
            org.swap(arr);
            try {
                if constexpr (utility::is_match_template_v<gp::node::GpArray, T>){
                    static_assert(!utility::is_match_template_v<gp::node::GpArray, T>, "not implemented");
                } else {
                    size_type num = std::count(begin(line) + first, last, ',') + 1;
                    arr.reserve(num);
                    decltype(first) l = first + 1, r = line.find(',', first);
                    for(size_type i = 0; i < num; ++i) {
                        arr.push_back(boost::lexical_cast<T>(line.substr(l, r - l)));
                        l = r;
                        r = line.find(',', l);
                    }
                }
            } catch (...) {
                arr.swap(org);
                throw;
            }

            return is;
        }
        friend std::ostream& operator<<(std::ostream& os, const GpArray& arr) {
            os << '[';
            for(size_type i = 0; i < arr.size() - 1; ++i)
                os << arr[i] << ',';
            os << arr.back() << ']';
            return os;
        }
    };

    template <typename T>
    inline void swap(GpArray<T>& a1, GpArray<T>& a2)noexcept {a1.swap(a2);}

    template <typename T, typename IndexType = int>
    class AtNode: public NodeBase<T(utility::Reference<GpArray<T>>, IndexType)> {
        using ThisType = AtNode;
        using ReturnType = utility::Reference<T>;
        using node_instance_type = NodeInterface::node_instance_type;
    public:

    };
}

#endif
