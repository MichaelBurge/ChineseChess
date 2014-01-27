#pragma once

struct Interpreter {
    GameState state;
    bool running;
    Interpreter();
    void prompt();
private:
    void dispatch_command(const string& command);
    void cmd_new();
    void cmd_empty();
    void cmd_exit();
    void cmd_show();
    void cmd_unknown();
};
