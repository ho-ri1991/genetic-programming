#ifndef GP_UTILITY_RESULT
#define GP_UTILITY_RESULT

#include <variant>
#include <string>
#include <type_traits>
#include <optional>
#include <functional>
#include <boost/optional.hpp>

namespace gp::utility {
    template <typename T>
    struct Ok{
        using wrap_type = T;
        T data;
    };

    struct Err {
        using wrap_type = std::string;
        std::string message;
    };

    template <typename T>
    class Result {
    public:
        using wrap_type = T;
    private:
        std::variant<Ok<T>, Err> data;
    public:
        Result(const Ok<T>& ok): data(ok){}
        Result(Ok<T>&& ok): data(std::move(ok)){}
        Result(const Err& err): data(err){}
        Result(Err&& err): data(std::move(err)){}
    public:
        Result(const Result&) = default;
        Result(Result&&) = default;
        Result& operator=(const Result&) = default;
        Result& operator=(Result&&) = default;
        ~Result() = default;
    public:
        explicit operator bool()const {return std::get_if<Ok<T>>(&data) != nullptr;}
        T& unwrap()& {return std::get<Ok<T>>(data).data;}
        const T& unwrap()const &{return std::get<Ok<T>>(data).data;}
        T&& unwrap()&&{return std::move(std::get<Ok<T>>(data).data);}
        std::string& errMessage()& {return std::get<Err>(data).message;}
        const std::string& errMessage()const &{return std::get<Err>(data).message;}
        std::string&& errMessage()&& {return std::move(std::get<Err>(data).message);}
        template <typename Fn>
        T ok_or(Fn fn)const &{
            if(*this) return std::get<Ok<T>>(data).data;
            else return fn();
        }
        template <typename Fn>
        T ok_or(Fn fn)&& {
            if(*this) return std::move(std::get<Ok<T>>(data).data);
            else return fn();
        }
        template <typename Fn, typename U = std::decay_t<decltype(std::declval<Fn>()(std::declval<const T&>()))>>
        Result<U> map(Fn fn)const & {
            if(*this) return Result<U>{Ok<U>{fn(std::get<Ok<T>>(data).data)}};
            else return Result<U>{std::get<Err>(data)};
        };
        template <typename Fn, typename U = std::decay_t<decltype(std::declval<Fn>()(std::declval<T&&>()))>>
        Result<U> map(Fn fn)&& {
            if(*this) return Result<U>{Ok<U>{fn(std::move(std::get<Ok<T>>(data).data))}};
            else return Result<U>{std::move(std::get<Err>(data))};
        };
        template <typename Fn, typename U = typename decltype(std::declval<Fn>()(std::declval<const T&>()))::wrap_type>
        Result<U> flatMap(Fn fn)const & {
            if(*this) return fn(std::get<Ok<T>>(data).data);
            else return std::get<Err>(data);
        };
        template<typename Fn, typename U = typename decltype(std::declval<Fn>()(std::declval<T&&>()))::wrap_type>
        Result<U> flatMap(Fn fn)&& {
            if(*this) return fn(std::move(std::get<Ok<T>>(data).data));
            else return std::move(std::get<Err>(data));
        };
    public:
        template <typename Matcher>
        decltype(auto) match(Matcher&& matcher)const & {return std::visit(std::forward<Matcher>(matcher), data);}
        template <typename Matcher>
        decltype(auto) match(Matcher&& matcher)&& {return std::visit(std::forward<Matcher>(matcher), std::move(data));}
    };

    namespace result {
        template <typename T>
        auto ok(T&& data){return Result<std::decay_t<T>>{Ok<std::decay_t<T>>{std::forward<T>(data)}};}
        template <typename T, typename String>
        Result<T> err(String&& err){return Result<T>{Err{std::forward<String>(err)}};}
        template <typename T, typename String>
        Result<T> fromOptional(const std::optional<T>& option, String&& errMsg){
            if(option) return ok(*option);
            else return err<T>(std::forward<String>(errMsg));
        }
        template <typename T, typename String>
        Result<T> fromOptional(std::optional<T>&& option, String&& errMsg) {
            if(option) return ok(std::move(*option));
            else return err<T>(std::forward<String>(errMsg));
        }
        template <typename T, typename String>
        auto fromOptional(const boost::optional<T>& option, String&& errMsg) {
            if constexpr (std::is_reference_v<T>) {
                if(option) return ok(std::ref(*option));
                else return err<decltype(std::ref(*option))>(std::forward<String>(errMsg));
            } else {
                if(option) return ok(*option);
                else return err<T>(std::forward<String>(errMsg));
            }
        }
        template <typename T, typename String>
        auto fromOptional(boost::optional<T>&& option, String&& errMsg) {
            if constexpr (std::is_reference_v<T>) {
                if(option) return ok(std::ref(*option));
                else return err<decltype(std::ref(*option))>(std::forward<String>(errMsg));
            } else {
                if(option) return ok(std::move(*option));
                else return err<T>(std::forward<String>(errMsg));
            }
        }

        namespace detail {
            template <typename... Ts>
            struct TypeTuple{};

            template <typename... Ts1, typename T2, typename... Ts2>
            Result<std::tuple<T2, Ts2...>> sequenceHelper(const std::tuple<Result<Ts1>...>& results, TypeTuple<T2, Ts2...>, const char* msgSeparator) {
                using AnsType = std::tuple<T2, Ts2...>;
                if constexpr (sizeof...(Ts2) > 0) {
                    auto nextResults = sequenceHelper(results, TypeTuple<Ts2...>{}, msgSeparator);
                    const auto& result = std::get<sizeof...(Ts1) - sizeof...(Ts2) - 1>(results);
                    if(!nextResults) return result ? err<AnsType>(std::move(nextResults).errMessage()) : err<AnsType>(result.errMessage() + msgSeparator + std::move(nextResults).errMessage());
                    else return result ? ok(std::tuple_cat(std::make_tuple(result.unwrap()), std::move(nextResults).unwrap())) : err<AnsType>(result.errMessage());
                } else {
                    const auto& result = std::get<std::tuple_size_v<std::tuple<Ts1...>> - 1>(results);
                    return result ? ok(std::make_tuple(result.unwrap())) : err<std::tuple<T2>>(result.errMessage());
                }
            }
        }

        template <typename... Ts>
        Result<std::tuple<Ts...>> sequence(const std::tuple<Result<Ts>...>& results, const char* msgSeparator = "\n") {
            return detail::sequenceHelper(results, detail::TypeTuple<Ts...>{}, msgSeparator);
        }

        template <typename... Ts>
        Result<std::tuple<Ts...>> sequence(const Result<Ts>... results) {
            return sequence(std::make_tuple(results...));
        }

//        template <typename Ts...>
//        Result<std::tuple<Ts...>> sequence(std::tuple<Result<Ts>...>&& results) {
//            return detail::sequenceHelper<0>(std::move(results));
//        }
    }
}

#endif
