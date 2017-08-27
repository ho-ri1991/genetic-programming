#ifndef GP_UTILITY_STRING
#define GP_UTILITY_STRING

#include <tuple>

namespace gp::utility {
    template<std::size_t ...indexes>
    struct Indexes{};

    template <typename , typename >
    struct IndexCatHelper;

    template <std::size_t ...indexes1,
              std::size_t ...indexes2>
    struct IndexCatHelper<Indexes<indexes1...>, Indexes<indexes2...>> {
        static constexpr Indexes<indexes1..., indexes2...> cat(const Indexes<indexes1...>, const Indexes<indexes2...>) noexcept {
            return Indexes<indexes1..., indexes2...>{};
        }
    };

    template <typename Index, typename ...Indexes>
    constexpr auto index_cat(const Index index, const Indexes... indexes) noexcept {
        if constexpr (sizeof...(Indexes) == 0) {
            return index;
        } else {
            return IndexCatHelper<Index, decltype(index_cat(indexes...))>::cat(index, index_cat(indexes...));
        }
    }

    template <std::size_t N>
    constexpr auto make_indexes() noexcept {
        return index_cat(make_indexes<N - 1>(), Indexes<N>{});
    }

    template <>
    constexpr auto make_indexes<0>() noexcept {
        return Indexes<0>{};
    }

    template <std::size_t N>
    class string {
    private:
        char elems[N];
    public:
        template <std::size_t ...indexes>
        constexpr string(const char (&arr)[N + 1], const Indexes<indexes...>) noexcept
                : elems{arr[indexes]...}{}
        template <std::size_t N1, std::size_t N2, std::size_t ...indexes>
        constexpr string(const string<N1>& str1, const string<N2>& str2, const Indexes<indexes...>)
                : elems{(indexes < N1 ? str1[indexes] : str2[indexes - N1])...}{}
        constexpr string(const char (&arr)[N + 1]) noexcept
                : string(arr, make_indexes<N - 1>()){}
    public:
        constexpr char& operator[](std::size_t n) {return elems[n];}
        constexpr char operator[](std::size_t n) const {return elems[n];}
    public:
        template <std::size_t M>
        constexpr bool operator==(const string<M>& other) const noexcept {
            if(N != M) return false;
            else {
                for(std::size_t i = 0; i < N; ++i) {
                    if(elems[i] != other.elems[i])return false;
                }
                return true;
            }
        }
        template <std::size_t M>
        constexpr bool operator==(const char (&arr)[M]) const noexcept {
            if(N != M - 1) return false;
            else {
                for(std::size_t i = 0; i < N; ++i) {
                    if(elems[i] != arr[i])return false;
                }
                return true;
            }
        }
    };

    template <std::size_t N, std::size_t M>
    constexpr string<N + M> operator+(const string<N>& str1, const string<M>& str2) noexcept {
        return string<N + M>(str1, str2, make_indexes<N + M - 1>());
    };

    template <>
    class string<0>{};

    template <std::size_t N>
    constexpr string<N - 1> to_string(const char (&arr)[N]) noexcept {
        return string<N - 1>(arr, make_indexes<N - 2>());
    }
}

#endif