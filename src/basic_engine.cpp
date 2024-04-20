#include <utility>

#include "basic_engine.h"

void BasicEngine::next_iteration() {
    const auto& curr_board = m_board_pair[m_board];
    auto& next_board = m_board_pair[m_board ^ 1];

    for (size_t i = 0; i < W; ++i) {
        for (size_t j = 0; j < H; ++j) {
            const std::pair<int64_t, int64_t> neighbor_offsets[]
            {{-1, -1}, {-1, 0}, {-1, 1},
             { 0, -1},          { 0, 1},
             { 1, -1}, { 1, 0}, { 1, 1}};
            size_t count = 0;
            for (const auto [x, y] : neighbor_offsets) {
                if (at(curr_board, (x+W+i)%W, (y+H+j)%H)) {
                    ++count;
                }
            }

            const state_t curr_cell = at(curr_board, i, j);
            state_t& next_cell = at(next_board, i, j);
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
    m_done_cb();
}

BasicEngine::state_t BasicEngine::get_state(size_t i, size_t j) {
    return at(m_board_pair[m_board], i, j);
}

BasicEngine::state_t& BasicEngine::at(board_t& v, size_t i, size_t j) {
    return v[i * W + j];
}

const BasicEngine::state_t& BasicEngine::at(const board_t& v, size_t i, size_t j) const {
    return v[i * W + j];
}
