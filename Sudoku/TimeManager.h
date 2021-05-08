#pragma once

#include <chrono>

using namespace std::chrono_literals;

double to_seconds(std::chrono::duration <double, std::ratio <1>> const duration) {
	return duration.count();
}

template <typename C = std::chrono::high_resolution_clock, typename ...A> auto
timed_run(std::invocable <A...> auto&& f, A && ...a) {
	auto const start{ C::now() };

	std::invoke(
		std::forward <std::remove_cvref_t <decltype (f)>>(f), std::forward <A>(a)...
	);
	return C::now() - start;
}

