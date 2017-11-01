#ifndef GP_UTILITY_RESULT
#define GP_UTILITY_RESULT

#include <variant>
#include <string>
#include <type_traits>
#include <optional>
#include <functional>
#include <boost/optional.hpp>
#include "is_detected.hpp"
#include "is_match_template.hpp"

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
        Result(const Ok<T>& ok)noexcept(std::is_nothrow_copy_constructible_v<Ok<T>>): data(ok){}
        Result(Ok<T>&& ok)noexcept(std::is_nothrow_move_constructible_v<Ok<T>>): data(std::move(ok)){}
        Result(const Err& err)noexcept(std::is_nothrow_copy_constructible_v<Err>): data(err){}
        Result(Err&& err)noexcept(std::is_nothrow_move_constructible_v<Err>): data(std::move(err)){}
    public:
        Result(const Result&) = default;
        Result(Result&&) = default;
        Result& operator=(const Result&) = default;
        Result& operator=(Result&&) = default;
        ~Result() = default;
    public:
        explicit operator bool()const noexcept {return std::get_if<Ok<T>>(&data) != nullptr;}
        T& unwrap()& {return std::get<Ok<T>>(data).data;}
        const T& unwrap()const &{return std::get<Ok<T>>(data).data;}
        T&& unwrap()&&{return std::move(std::get<Ok<T>>(data).data);}
        std::string& errMessage()& {return std::get<Err>(data).message;}
        const std::string& errMessage()const &{return std::get<Err>(data).message;}
        std::string&& errMessage()&& {return std::move(std::get<Err>(data).message);}
        template <typename Fn>
        T ok_or(Fn fn)const& noexcept(std::is_nothrow_invocable_r_v<T, Fn>
                                      && std::is_nothrow_copy_constructible_v<T>) {
            if(*this) return std::get<Ok<T>>(data).data;
            else return fn();
        }
        template <typename Fn>
        T ok_or(Fn fn)&& noexcept(std::is_nothrow_invocable_r_v<T, Fn>
                                  && std::is_nothrow_move_constructible_v<T>) {
            if(*this) return std::move(std::get<Ok<T>>(data).data);
            else return fn();
        }
        template <typename Fn, typename U = std::decay_t<std::invoke_result_t<Fn, const T&>>>
        Result<U> map(Fn fn)const & noexcept(std::is_nothrow_invocable_r_v<Ok<U>, Fn, const T&>
                                             && std::is_nothrow_constructible_v<Result<U>, Ok<U>&&>
                                             && std::is_nothrow_constructible_v<Result<U>, const Err&>){
            if(*this) return Result<U>{Ok<U>{fn(std::get<Ok<T>>(data).data)}};
            else return Result<U>{std::get<Err>(data)};
        };
        template <typename Fn, typename U = std::decay_t<std::invoke_result_t<Fn, T&&>>>
        Result<U> map(Fn fn)&& noexcept(std::is_nothrow_invocable_r_v<Ok<U>, Fn, T&&>
                                        && std::is_nothrow_constructible_v<Result<U>, Ok<U>>
                                        && std::is_nothrow_constructible_v<Result<U>, Err&&>){
            if(*this) return Result<U>{Ok<U>{fn(std::move(std::get<Ok<T>>(data).data))}};
            else return Result<U>{std::move(std::get<Err>(data))};
        };
        template <typename Fn, typename U = typename std::invoke_result_t<Fn, const T&>::wrap_type>
        Result<U> flatMap(Fn fn)const & noexcept(std::is_nothrow_invocable_r_v<Result<U>, Fn, const T&>
                                                 && std::is_nothrow_constructible_v<Result<U>, const Err&>) {
            if(*this) return fn(std::get<Ok<T>>(data).data);
            else return std::get<Err>(data);
        };
        template<typename Fn, typename U = typename std::invoke_result_t<Fn, T&&>::wrap_type>
        Result<U> flatMap(Fn fn)&& noexcept(std::is_nothrow_invocable_r_v<Result<U>, Fn, T&&>
                                            && std::is_nothrow_constructible_v<Result<U>, Err&&>) {
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
                    else return result ? ok(std::tuple_cat(std::tuple<T2>(result.unwrap()), std::move(nextResults).unwrap())) : err<AnsType>(result.errMessage());
                } else {
                    const auto& result = std::get<std::tuple_size_v<std::tuple<Ts1...>> - 1>(results);
                    return result ? ok(std::tuple<T2>(result.unwrap())) : err<std::tuple<T2>>(result.errMessage());
                }
            }

            template <typename... Ts1, typename T2, typename... Ts2>
            Result<std::tuple<T2, Ts2...>> sequenceHelper(std::tuple<Result<Ts1>...>&& results, TypeTuple<T2, Ts2...>, const char* msgSeparator) {
                using AnsType = std::tuple<T2, Ts2...>;
                if constexpr (sizeof...(Ts2) > 0) {
                    auto nextResults = sequenceHelper(std::move(results), TypeTuple<Ts2...>{}, msgSeparator);
                    auto&& result = std::get<sizeof...(Ts1) - sizeof...(Ts2) - 1>(std::move(results));
                    if(!nextResults) return result ? err<AnsType>(std::move(nextResults).errMessage()) : err<AnsType>(std::move(result).errMessage() + msgSeparator + std::move(nextResults).errMessage());
                    else return result ? ok(std::tuple_cat(std::tuple<T2>(std::move(result).unwrap()), std::move(nextResults).unwrap())) : err<AnsType>(std::move(result).errMessage());
                } else {
                    auto&& result = std::get<std::tuple_size_v<std::tuple<Ts1...>> - 1>(std::move(results));
                    return result ? ok(std::tuple<T2>(std::move(result).unwrap())) : err<std::tuple<T2>>(std::move(result).errMessage());
                }
            }

            template <typename Container, typename size_type>
            using support_reserve = decltype(std::declval<Container>().reserve(std::declval<size_type>()));
            template <typename Container, typename value_type>
            using support_push_back = decltype(std::declval<Container>().push_back(std::declval<value_type>()));
        }

        template <typename... Ts>
        Result<std::tuple<Ts...>> sequence(const std::tuple<Result<Ts>...>& results, const char* msgSeparator = "\n") {
            return detail::sequenceHelper(results, detail::TypeTuple<Ts...>{}, msgSeparator);
        }

        template <typename...Ts>
        Result<std::tuple<Ts...>> sequence(std::tuple<Result<Ts>...>&& results, const char* msgSeparator = "\n") {
            return detail::sequenceHelper(std::move(results), detail::TypeTuple<Ts...>{}, msgSeparator);
        }

        template <typename... Results, typename = std::enable_if_t<std::conjunction_v<is_match_template<Result, std::decay_t<Results>>...>>>
        auto sequence(Results&&... results) {
            return sequence(std::make_tuple(std::forward<Results>(results)...));
        }

        template <template <typename...> class Container,
                  typename... Ts,
                  typename U = std::enable_if_t<
                          is_match_template_v<Result, typename Container<Ts...>::value_type>,
                          typename Container<Ts...>::value_type::wrap_type
                  >,
                  typename = std::enable_if_t<
                          is_detected_v<detail::support_push_back, Container<U>, U>
                  >
        >
        Result<Container<U>> sequence(const Container<Ts...>& results, const char* msgSeparator = "\n") {
            using std::size;
            auto ans = ok(Container<U>());
            if(size(results) == 0)return ans;
            if constexpr (is_detected_v<detail::support_reserve, Container<U>, typename Container<U>::size_type>) {
                ans.unwrap().reserve(size(results));
            }
            for(const auto& res: results) {
                if(ans && res) {
                    ans.unwrap().push_back(res.unwrap());
                } else if(ans && !res) {
                    ans = err<Container<U>>(res.errMessage());
                } else if(!ans && !res) {
                    ans.errMessage() += (msgSeparator + res.errMessage());
                }
            }
            return ans;
        };

        template <template <typename...> class Container,
                typename... Ts,
                typename U = std::enable_if_t<
                        is_match_template_v<Result, typename Container<Ts...>::value_type>,
                        typename Container<Ts...>::value_type::wrap_type
                >,
                typename = std::enable_if_t<
                        is_detected_v<detail::support_push_back, Container<U>, U>
                >
        >
        Result<Container<U>> sequence(Container<Ts...>&& results, const char* msgSeparator = "\n") {
            using std::size;
            auto ans = ok(Container<U>());
            if(size(results) == 0)return ans;
            if constexpr (is_detected_v<detail::support_reserve, Container<U>, typename Container<U>::size_type>) {
                ans.unwrap().reserve(size(results));
            }
            for(auto& res: results) {
                if(ans && res) {
                    ans.unwrap().push_back(std::move(res).unwrap());
                } else if(ans && !res) {
                    ans = err<Container<U>>(std::move(res).errMessage());
                } else if(!ans && !res) {
                    ans.errMessage() += (msgSeparator + res.errMessage());
                }
            }
            return ans;
        };

        template <typename Fn, typename String, typename T = std::decay_t<std::invoke_result_t<Fn>>>
        Result<T> tryFunction(Fn fn, String errMessage) {
            try {
                return ok(fn());
            } catch (...){
                return err<T>(std::forward<String>(errMessage));
            }
        }

        template <typename Fn, typename String, typename Exception, typename T = std::decay_t<std::invoke_result_t<Fn>>>
        Result<T> tryFunction(Fn fn, String errMessage, Exception exception) {
            try {
                return ok(fn());
            } catch (const Exception& ex) {
                return err<T>(std::forward<String>(errMessage));
            }
        };
    }
}

#endif
