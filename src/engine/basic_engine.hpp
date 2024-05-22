#ifndef LIFEPVP_BASIC_ENGINE_H
#define LIFEPVP_BASIC_ENGINE_H

#include <concepts>
#include <vector>

#include "engine_base.hpp"

namespace lifepvp::engine {

template <class T>
concept BoardConstructibleBySize = requires(T &&board) {
	T(board.size());
};

template <class T>
concept BoardResizable = std::is_default_constructible_v<T> &&
		requires(T board, size_t x) {
			board.resize(x);
		};

template <class T>
concept BasicEngineContainer =
		std::move_constructible<T> &&
		(BoardResizable<T> || BoardConstructibleBySize<T>) && requires(T &board, const T &cboard, size_t i) {
			{ board[i] } -> std::convertible_to<EngineBase::state_t &>;
			{ cboard[i] } -> std::convertible_to<EngineBase::state_t const &>;
		};

struct BasicEngineRuleset {
	bool wrap_around = false;
};

template <BasicEngineRuleset Ruleset, BasicEngineContainer T = std::vector<EngineBase::state_t>>
class BasicEngine : public EngineBase {
public:
	using board_t = std::remove_reference_t<T>;

	BasicEngine(board_t &&board,
			size_t w,
			size_t h,
			update_cb_t update_cb) :
			EngineBase(w, h, update_cb), m_board_pair([&]() {
				if constexpr (BoardConstructibleBySize<board_t>) {
					return decltype(m_board_pair){ std::move(board), board_t(board.size()) };
				} else {
					static_assert(BoardResizable<board_t>);
					decltype(m_board_pair) board_pair{ std::move(board), board_t() };
					board_pair[1].resize(w * h);
					return board_pair;
				}
			}()) {}

	virtual ~BasicEngine() = default;

	void next_iteration() override {
		const auto &curr_board = m_board_pair[m_board];
		auto &next_board = m_board_pair[m_board ^ 1];

		for (size_t i = 0; i < W; ++i) {
			for (size_t j = 0; j < H; ++j) {
				static const std::pair<int64_t, int64_t> neighbor_offsets[]{
					{ -1, -1 }, { -1, 0 }, { -1, 1 },
					{ 0, -1 }, { 0, 1 },
					{ 1, -1 }, { 1, 0 }, { 1, 1 }
				};
				size_t count = 0;
				for (const auto [x, y] : neighbor_offsets) {
					const bool is_neighbor_alive = [&]() {
						if constexpr (Ruleset.wrap_around) {
							return at(curr_board, (x + W + i) % W, (y + H + j) % H);
						} else {
							const int64_t nx = x + i;
							const int64_t ny = y + j;
							return 0 <= nx && nx < W && 0 <= ny && ny < H && at(curr_board, nx, ny);
						}
					}();

					if (is_neighbor_alive) {
						++count;
					}
				}

				const state_t curr_cell = at(curr_board, i, j);
				state_t &next_cell = at(next_board, i, j);
				if (count < 2 || count > 3) {
					next_cell = 0; // die
				} else if (count == 3) {
					next_cell = 1; // survive or create
				} else { // count == 2
					next_cell = curr_cell; // survive
				}

				if (next_cell != curr_cell) {
					m_update_cb(i, j, next_cell);
				}
			}
		}

		m_board ^= 1;
	}

	state_t get_state(size_t i, size_t j) override {
		return at(m_board_pair[m_board], i, j);
	};

protected:
	size_t m_board = 0;
	std::array<board_t, 2> m_board_pair;

	state_t &at(board_t &v, size_t i, size_t j) {
		return v[i * W + j];
	};

	const state_t &at(const board_t &v, size_t i, size_t j) const {
		return v[i * W + j];
	};
};

} //namespace lifepvp::engine

#endif