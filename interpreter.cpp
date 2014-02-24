#include "interpreter.hpp"
#include "utility.hpp"
#include "parsing.hpp"
#include "exceptions.hpp"
#include "scoring.hpp"
#include "minimax.hpp"

static ostream bitbucket(0);

Interpreter::Interpreter() : 
    _state(StandardGameState::new_game()),
    running(true),
    difficulty(2),
    max_nodes(1000),
    is_running_over_xboard(false),
    xboard_protocol_version(0)
    { }

ostream& Interpreter::human_cli() {
    return is_running_over_xboard
	? bitbucket
	: cout;
}

ostream& Interpreter::xboard_cli() {
    return cout;
}

void Interpreter::prompt() {
    human_cli() << "Welcome to Michael Burge's Ultimate Chinese Chess Happiness!" << endl;
    cout.setf(ios::unitbuf);
    while (this->running) {
	human_cli() << "> ";
        string input;
        getline(cin, input);
        dispatch_command(input);
    }
}

void Interpreter::dispatch_command(const string& command) {
    auto move = parse_move(command);
    if (!!move) {
	this->run_move((*move).first);
	return;
    }
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
    case str2int("run_computer"):
        this->cmd_run_computer();
        break;
    case str2int("difficulty"):
        this->cmd_set_difficulty(remaining_text);
        break;
    case str2int("max_nodes"):
        this->cmd_set_max_nodes(remaining_text);
        break;
    case str2int("move_scores"):
	this->cmd_show_move_scores();
	break;

    // XBoard commands
    case str2int("xboard"):
	cmd_xboard(); break;
    case str2int("protover"):
	cmd_protover(remaining_text); break;
    case str2int("accepted"):
	cmd_accepted(); break;
    case str2int("rejected"):
	cmd_rejected(); break;
    case str2int("variant"):
	cmd_variant(remaining_text); break;
    case str2int("quit"):
	cmd_exit(); break;
    case str2int("random"):

	cmd_random(); break;
    case str2int("force"):
	cmd_force(); break;
    case str2int("go"):
	cmd_go(); break;
    case str2int("playother"):
	cmd_playother(); break;
    case str2int("white"):
	cmd_white(); break;
    case str2int("black"):
	cmd_black(); break;
    case str2int("level"):
	cmd_level(remaining_text); break;
    case str2int("st"):
	cmd_st(remaining_text); break;
    case str2int("sd"):
	cmd_sd(remaining_text); break;
    case str2int("nps"):
	cmd_nps(remaining_text); break;
    case str2int("time"):
	cmd_time(remaining_text); break;
    case str2int("otim"):
	cmd_otim(remaining_text); break;
    case str2int("usermove"):
	cmd_usermove(remaining_text); break;
    case str2int("?"):
	cmd_movenow(remaining_text); break;
    case str2int("ping"):
	cmd_ping(remaining_text); break;
    case str2int("draw"):
	cmd_draw(); break;
    case str2int("result"):
	cmd_result(remaining_text); break;
    case str2int("setboard"):
	cmd_setboard(remaining_text); break;
    case str2int("edit"):
	cmd_edit(); break;
    case str2int("hint"):
	cmd_hint(); break;
    case str2int("bk"):
	cmd_bk(); break;
    case str2int("undo"):
	cmd_undo(); break;
    case str2int("remove"):
	cmd_remove(); break;
    case str2int("hard"):
	cmd_enable_pondering(); break;
    case str2int("easy"):
	cmd_disable_pondering(); break;
    case str2int("post"): break;
	cmd_post(); break;
    case str2int("nopost"):
	cmd_nopost(); break;
    case str2int("analyze"):
	cmd_analyze(); break;
    case str2int("name"):
	cmd_name(); break;
    case str2int("rating"):
	cmd_rating(); break;
    case str2int("ics"):
	cmd_ics(remaining_text); break;
    case str2int("computer"):
	cmd_set_is_computer_opponent(); break;
    case str2int("pause"):
	cmd_pause(); break;
    case str2int("resume"):
	cmd_resume(); break;
    case str2int("memory"):
	cmd_memory(remaining_text); break;
    case str2int("cores"):
	cmd_cores(remaining_text); break;
    case str2int("egtpath"):
	cmd_egtpath(remaining_text); break;
    case str2int("option"):
	cmd_option(remaining_text); break;
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
    this->_state = StandardGameState::new_game();
}

void Interpreter::cmd_exit() {
    this->running = false;
}

void Interpreter::cmd_unknown() {
    cout << "Unknown command" << endl;
}

void Interpreter::cmd_show() {
    cout << this->_state;
}

void Interpreter::run_move(const Move& move) {
    if (StandardRulesEngine::is_legal_move(this->_state, move)) {
        this->_state.apply_move(move);
    } else {
	cout << "Illegal move: " << move << endl;
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
        moves = StandardRulesEngine::available_moves(this->state());
    } else {
        auto position = (*parsed_position).first;
        moves = StandardRulesEngine::available_moves_from(this->state(), position);
    }
    print_moves(moves);
}

void Interpreter::cmd_run_computer() {
    auto move = best_move(this->state(), this->difficulty, this->max_nodes, piece_score);
    cout << "The computer chooses " << move << endl;
    this->run_move(move);
}

void handle_integer(int& target, const string& name, const string& remaining_text) {
    auto parsed = parse_value<int>(remaining_text);
    if (!parsed) {
        cout << name << ": " << target << endl;
        return;
    }
    target = (*parsed).first;
}

void Interpreter::cmd_set_max_nodes(const string& remaining_text) {
    handle_integer(this->max_nodes, "Max Nodes", remaining_text);
}

void Interpreter::cmd_set_difficulty(const string& remaining_text) {
    handle_integer(this->difficulty, "Difficulty", remaining_text);
}

void Interpreter::cmd_show_move_scores() {
    print_move_scores(move_scores(this->state(), standard_score_function));
}

void Interpreter::cmd_xboard() {
    this->is_running_over_xboard = true;
}

void Interpreter::cmd_protover(const string& remaining_text) {
    handle_integer(this->xboard_protocol_version, "XBoard protocol version", remaining_text);
}

const StandardGameState& Interpreter::state() {
    return this->_state;
}

void Interpreter::cmd_accepted() {
}

void Interpreter::cmd_rejected() {
}

void Interpreter::cmd_variant(const string&) {
}

void Interpreter::cmd_quit() {
}

void Interpreter::cmd_random() {
}

void Interpreter::cmd_force() {
}

void Interpreter::cmd_go() {
}

void Interpreter::cmd_playother() {
}

void Interpreter::cmd_white() {
}

void Interpreter::cmd_black() {
}

void Interpreter::cmd_level(const string&) {
}

void Interpreter::cmd_st(const string&) {
}

void Interpreter::cmd_sd(const string&) {
}

void Interpreter::cmd_nps(const string&) {
}

void Interpreter::cmd_time(const string&) {
}

void Interpreter::cmd_otim(const string&) {
}

void Interpreter::cmd_usermove(const string&) {
}

void Interpreter::cmd_movenow(const string&) {
}

void Interpreter::cmd_ping(const string&) {
}

void Interpreter::cmd_draw() {
}

void Interpreter::cmd_result(const string&) {
}

void Interpreter::cmd_setboard(const string&) {
}

void Interpreter::cmd_edit() {
}

void Interpreter::cmd_hint() {
}

void Interpreter::cmd_bk() {
}

void Interpreter::cmd_undo() {
}

void Interpreter::cmd_remove() {
}

void Interpreter::cmd_enable_pondering() {
}

void Interpreter::cmd_disable_pondering() {
}

void Interpreter::cmd_post() {
}

void Interpreter::cmd_nopost() {
}

void Interpreter::cmd_analyze() {
}

void Interpreter::cmd_name() {
}

void Interpreter::cmd_rating() {
}

void Interpreter::cmd_ics(const string&) {
}

void Interpreter::cmd_set_is_computer_opponent() {
}

void Interpreter::cmd_pause() {
}

void Interpreter::cmd_resume() {
}

void Interpreter::cmd_memory(const string&) {
}

void Interpreter::cmd_cores(const string&) {
}

void Interpreter::cmd_egtpath(const string&) {
}

void Interpreter::cmd_option(const string&) {
}

    
void Interpreter::out_feature() {
}

void Interpreter::out_illegal_move(const Move&) {
}

void Interpreter::out_move(const Move&) {
}

void Interpreter::out_result() {
}

void Interpreter::out_resign() {
}

void Interpreter::out_offer_draw() {
}

void Interpreter::out_tellopponent(const string&) {
}

void Interpreter::out_tellothers(const string&) {
}

void Interpreter::out_tellall(const string&) {
}

void Interpreter::out_telluser(const string&) {
}

void Interpreter::out_tellusererror(const string&) {
}

void Interpreter::out_askuser(const string&, const string&) {
}

void Interpreter::out_tellics(const string&) {
}

void Interpreter::out_tellicsnoalias(const string&) {
}

void Interpreter::out_comment(const string&) {
}
