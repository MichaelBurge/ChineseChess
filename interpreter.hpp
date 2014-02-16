#pragma once

#include "configuration.hpp"

struct Interpreter {
    StandardGameState _state;
    bool running;
    int difficulty;
    int max_nodes;
    Interpreter();
    void prompt();
    void run_move(const Move& move);
    const StandardGameState& state();
    void dispatch_command(const string& command);
    void cmd_move(const string&);
    void cmd_show_moves(const string&);
    void cmd_new();
    void cmd_empty();
    void cmd_exit();
    void cmd_show();
    void cmd_unknown();
    void cmd_run_computer();
    void cmd_set_max_nodes(const string& remaining_text);
    void cmd_set_difficulty(const string& remaining_text);
    void cmd_show_move_scores();
};
