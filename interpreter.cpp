#include "rules.hpp"
#include "interpreter.hpp"
#include "utility.hpp"
#include "parsing.hpp"
#include "exceptions.hpp"
#include "scoring.hpp"
#include "minimax.hpp"

Interpreter::Interpreter() {
    this->running = true;
    this->_state = new_game();
    this->difficulty = 1;
}

void Interpreter::prompt() {
    cout << "Welcome to Super Happy Xiangqi funtime!" << endl;
    while (this->running) {
        cout << "> ";
        string input;
        getline(cin, input);
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
    auto remaining_text = (*primary).second;

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
    case str2int("move"):
        this->cmd_move(remaining_text);
        break;
    case str2int("moves"):
        this->cmd_show_moves(remaining_text);
        break;
    case str2int("computer"):
        this->cmd_run_computer();
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
    this->_state = new_game();
}

void Interpreter::cmd_exit() {
    this->running = false;
}

void Interpreter::cmd_unknown() {
    cout << "Unknown command" << endl;
}

void Interpreter::cmd_show() {
    print_board(this->state());
}

void Interpreter::run_move(const Move& move) {
    try {
        apply_move(this->_state, move);
    } catch(const illegal_move& e) {
        cout << e.what() << endl;
    }
}

void Interpreter::cmd_move(const string& remaining_text) {
    auto parsed_move = parse_move(remaining_text);
    if (!parsed_move) {
        cout << "Unable to parse move: `" << remaining_text << "`" << endl;
        return;
    }
    this->run_move((*parsed_move).first);
}

void Interpreter::cmd_show_moves(const string& remaining_text) {
    auto moves = vector<Move>();
    auto parsed_position = parse_position(remaining_text);
    if (!parsed_position) {
        moves = available_moves(this->state());
    } else {
        auto position = (*parsed_position).first;
        moves = available_moves_from(this->state(), position);
    }
    print_moves(moves);
}

void Interpreter::cmd_run_computer() {
    auto move = best_move(this->state(), this->difficulty, piece_score);
    cout << "The computer chooses " << move << endl;
    this->run_move(move);
}

const GameState& Interpreter::state() {
    return this->_state;
}
