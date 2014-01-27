#pragma once

struct Interpreter {
    bool running;
    Interpreter();
    void prompt();
    void run_move(const Move& move);
    const GameState& state();
private:
    GameState _state;
    void dispatch_command(const string& command);
    void cmd_move(const string&);
    void cmd_new();
    void cmd_empty();
    void cmd_exit();
    void cmd_show();
    void cmd_unknown();
};
