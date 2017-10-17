#ifndef GP_UTILITY_RESULT
#define GP_UTILITY_RESULT

#include <variant>
#include <string>
#include <type_traits>

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
    }
}

#endif
