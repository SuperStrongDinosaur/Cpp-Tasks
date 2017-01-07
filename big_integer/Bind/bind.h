#ifndef bind_h
#define bind_h

#include <functional>

template <int n>
struct place_holder{};

template <typename F, typename... Args>
struct bind_t {
private:
	F func;
	std::tuple<Args...> args;

	template <typename F_, typename... Args_>
	bind_t(F_&& func, Args_&&... args) :
		func(std::forward<F_>(func)),
		args(std::forward<Args_>(args)...){}

	template <typename old_Arg, typename... new_Args>
	decltype(auto) arg_get(old_Arg& old_arg, new_Args&...) {
		return old_arg;
	}

	template <typename F_, typename... old_Args, typename... new_Args>
	decltype(auto) arg_get(bind_t<F_, old_Args...>& b, new_Args&... new_args) {
		return b(new_args...);
	}

	template <int n, typename... new_Args>
	decltype(auto) arg_get(place_holder<n>&, new_Args&... new_args) {
		return std::get<n>(std::forward_as_tuple(new_args...));
	}

	template <int... n>
	struct sequence {};

	template <int s, int... n>
	struct seq_generator {
		typedef typename seq_generator<s - 1, s - 1, n...>::type type;
	};

	template <int... n>
	struct seq_generator<0, n...> {
		typedef sequence<n...> type;
	};

	template <typename... new_Args, int... s>
	auto call(const sequence<s...>&, new_Args&&... new_args) {
		return func(arg_get(std::get<s>(args), new_args...)...);
	}

	template <typename F_, typename... Args_>
	friend bind_t<F_, Args_...> bind(F_&& func, Args_&&... args);

public:
	template <typename... new_Args>
	auto operator()(new_Args&&... new_args) {
		return call(typename seq_generator<std::tuple_size<std::tuple<Args...>>::value>::type(), std::forward<new_Args>(new_args)...);
	}
};

static place_holder<0> _1;
static place_holder<1> _2;
static place_holder<2> _3;
static place_holder<3> _4;
static place_holder<4> _5;

template <typename F_, typename... Args_>
bind_t<F_, Args_...> bind(F_&& func, Args_&&... args) {
	return bind_t<F_, Args_...>(std::forward<F_>(func), std::forward<Args_>(args)...);
}

#endif

