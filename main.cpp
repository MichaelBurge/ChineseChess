#include "interpreter.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "This help message")
	("xboard", "Use XBoard mode?")
	;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
	cout << desc << "\n";
	return 1;
    }
    auto interpreter = Interpreter();

    if (vm.count("xboard")) {
	interpreter.is_running_over_xboard = true;
    }
    interpreter.prompt();
}
