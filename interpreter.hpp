#pragma once

#include "configuration.hpp"

struct HumanCLI : public std::ostream {
    
};

struct Interpreter {
    StandardGameState _state;
    bool running;
    bool computer_enabled;
    bool are_extraneous_messages_disabled;
    int difficulty;
    int max_nodes;

    Interpreter();
    void prompt();
    void run_move(const Move& move);

    ostream& human_cli();
    ostream& xboard_cli();

    const StandardGameState& state();
    void dispatch_command(const string& command);
    void run_computer_move();
    void cmd_move(const string&);
    void cmd_show_moves(const string&);
    void cmd_new();
    void cmd_empty();
    void cmd_exit();
    void cmd_show();
    void cmd_show_score();
    void cmd_unknown();
    void cmd_run_computer();
    void cmd_set_max_nodes(const string&);
    void cmd_set_difficulty(const string&);
    void cmd_show_best_line();
    void cmd_show_move_scores();
    void cmd_show_move_scores_simple();

    // XBoard communication commands
    // http://www.gnu.org/software/xboard/engine-intf.html
    // http://home.hccnet.nl/h.g.muller/XQwinboard.html
    void cmd_xboard();
    void cmd_protover(const string&);
    void cmd_accepted();
    void cmd_rejected();
    void cmd_variant(const string&);
    void cmd_quit();
    void cmd_random();
    void cmd_force();
    void cmd_go();
    void cmd_playother();
    void cmd_white();
    void cmd_black();
    void cmd_level(const string&);
    void cmd_st(const string&);
    void cmd_sd(const string&);
    void cmd_nps(const string&);
    void cmd_time(const string&);
    void cmd_otim(const string&);
    void cmd_usermove(const string&);
    void cmd_movenow(const string&);
    void cmd_ping(const string&);
    void cmd_draw();
    void cmd_result(const string&);
    void cmd_setboard(const string&);
    void cmd_edit();
    void cmd_hint();
    void cmd_bk();
    void cmd_undo();
    void cmd_remove();
    void cmd_enable_pondering();
    void cmd_disable_pondering();
    void cmd_post();
    void cmd_nopost();
    void cmd_analyze();
    void cmd_name();
    void cmd_rating();
    void cmd_ics(const string&);
    void cmd_set_is_computer_opponent();
    void cmd_pause();
    void cmd_resume();
    void cmd_memory(const string&);
    void cmd_cores(const string&);
    void cmd_egtpath(const string&);
    void cmd_option(const string&);
    
    void out_feature();
    void out_illegal_move(const Move&);
    void out_move(const Move&);
    void out_result();
    void out_resign();
    void out_offer_draw();
    void out_tellopponent(const string&);
    void out_tellothers(const string&);
    void out_tellall(const string&);
    void out_telluser(const string&);
    void out_tellusererror(const string&);
    void out_askuser(const string&, const string&);
    void out_tellics(const string&);
    void out_tellicsnoalias(const string&);
    void out_comment(const string&);
    bool is_running_over_xboard;
    bool is_computer_opponent;
    int xboard_protocol_version;
};
