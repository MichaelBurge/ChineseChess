#include "rules.hpp"
#include "interpreter.hpp"
#include "utility.hpp"
#include "parsing.hpp"

Interpreter::Interpreter() {
    this->running = true;
    this->state = new_game();
}

void Interpreter::prompt() {
    cout << "Welcome to Super Happy Xiangqi funtime!" << endl;
    while (this->running) {
        cout << "> ";
        string input;
        cin >> input;
        dispatch_command(input);
    }
}

void Interpreter::dispatch_command(const string& command) {
    auto primary = parse_token(command, ' ');
    if (!primary) {
        this->cmd_empty();
        return;
    }
    auto command_token = (*primary).first;
    switch (str2int(command_token.c_str())) {
    case str2int("show"):
        this->cmd_show();
        break;
    case str2int("exit"):
        this->cmd_exit();
        break;
    case str2int("new"):
        this->cmd_new();
        break;
    default:
        this->cmd_unknown();
        break;
    }
}

void Interpreter::cmd_empty() {
    this->running = false;
    cout << "Empty command received - exiting" << endl;
}

void Interpreter::cmd_new() {
    this->state = new_game();
}

void Interpreter::cmd_exit() {
    this->running = false;
}

void Interpreter::cmd_unknown() {
    cout << "Unknown command" << endl;
}

void Interpreter::cmd_show() {
    print_board(this->state);
}
