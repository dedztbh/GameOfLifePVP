#ifndef LIFEPVP_BASIC_ENGINE_H
#define LIFEPVP_BASIC_ENGINE_H

#include <vector>

#include "engine_base.h"

class BasicEngine : public EngineBase {
public:
    using board_t = std::vector<state_t>;

    BasicEngine(board_t&& board,
                const size_t w,
                const size_t h,
                const update_cb_t update_cb,
                const done_cb_t done_cb) 
            : EngineBase(w, h, update_cb, done_cb), 
              m_board_pair{board, board_t(board.size())} {}
    
    virtual ~BasicEngine() = default;
    void next_iteration() override;
    state_t get_state(size_t i, size_t j) override;
    
protected:
    size_t m_board = 0;
    std::array<board_t, 2> m_board_pair;

    state_t& at(board_t& v, size_t i, size_t j);
    const state_t& at(const board_t& v, size_t i, size_t j) const;
};

#endif