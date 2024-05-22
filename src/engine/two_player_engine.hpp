#ifndef LIFEPVP_TWO_PLAYER_BASIC_ENGINE_H
#define LIFEPVP_TWO_PLAYER_BASIC_ENGINE_H

#include "basic_engine.hpp"

#include <cstdlib>

namespace lifepvp::engine {

struct TwoPlayerEngineRuleset {
	bool wrap_around = false;

	// Count sum, not difference, of neighboring cells of either color to determine a cell's next state
	bool cooperative_cells = false;
	// Use random coin flip instead of iteration# paritry to break tie for coloring a cell
	// bool random_break_tie = false;

	// bool surviving_cell_keep_color = true;

	constexpr BasicEngineRuleset base() const {
		return BasicEngineRuleset{ wrap_around };
	}
};

template <TwoPlayerEngineRuleset Ruleset, BasicEngineContainer T = std::vector<EngineBase::state_t>>
class TwoPlayerEngine : public BasicEngine<Ruleset.base(), T> {
public:
	using base = BasicEngine<Ruleset.base(), T>;
	using base::at;
	using base::H;
	using base::m_board;
	using base::m_board_pair;
	using base::m_update_cb;
	using base::W;

	using board_t = std::remove_reference_t<T>;

	TwoPlayerEngine(board_t &&board,
			size_t w,
			size_t h,
			EngineBase::update_cb_t update_cb) :
			base(std::move(board), w, h, update_cb) {}

	virtual ~TwoPlayerEngine() = default;

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
				int64_t counts[] = { 0, 0 };
				for (const auto [x, y] : neighbor_offsets) {
					const EngineBase::state_t neighbor = [&]() {
						if constexpr (Ruleset.wrap_around) {
							return at(curr_board, (x + W + i) % W, (y + H + j) % H);
						} else {
							const int64_t nx = x + i;
							const int64_t ny = y + j;
							return (0 <= nx && nx < W && 0 <= ny && ny < H) ? at(curr_board, nx, ny) : 0;
						}
					}();

					if (neighbor) {
						++counts[neighbor - 1];
					}
				}

				const size_t count = [&]() {
					if constexpr (Ruleset.cooperative_cells) {
						return counts[0] + counts[1];
					} else {
						return abs(counts[0] - counts[1]);
					}
				}();

				const auto curr_cell = at(curr_board, i, j);
				auto &next_cell = at(next_board, i, j);
				if (count < 2 || count > 3) {
					next_cell = 0; // die
				} else if (count == 3) {
					if (curr_cell) {
						// survive
						next_cell = curr_cell;
					} else {
						// create
						if (counts[0] == counts[1]) {
							next_cell = m_board + 1;
						} else if (counts[0] > counts[1]) {
							next_cell = 1;
						} else { // counts[0] < counts[1]
							next_cell = 2;
						}
					}
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
};

} //namespace lifepvp::engine

#endif