#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "markov.hpp"

void syntax(char *name) {
    std::cerr
        << "Usage:\n\t"
        << name << " -[igm] [-f FILE]"
        << "\n\nActions (one required):\n\t"
        << "-i: input sentences\n\t"
        << "-g: generate sentences\n"
        << "\nOptional parameters:\n\t"
        << "-f: specify a database filename (by default, ./inecpp.mkdb)\n";
    std::exit(1);
}
int main(int argc, char *argv[]) {
    std::cout << "inebriated, C++ edition v0.2.0\nan eta thing <http://theta.eu.org>\n\n";
    std::string filename {"./inecpp.mkdb"};
    int opt, mode = 0;
    while ((opt = getopt(argc, argv, "igf:")) != -1) {
        switch (opt) {
        case 'i':
            mode = 1;
            break;
        case 'g':
            mode = 2;
            break;
        case 'f':
            filename = std::string {optarg};
            break;
        default:
            syntax(argv[0]);
            break;
        }
    }
    if (mode == 0) {
        std::cerr << "ERROR: Please specify a mode of operation (for example, -i)\n";
        syntax(argv[0]);
    }
    Markov::DB mdb {};
    std::cout << "Reading from database...\n";
    if (!mdb.readCorpus(filename)) {
        if (mode == 2) {
            std::cerr << "\nERROR: Cannot generate sentences without a functioning database.\n";
            std::cerr << "hint: try `-f filename` to specify one, or `-i` to input data\n";
            throw std::runtime_error("Failed to open database file");
        }
    }
    if (mode == 1) {
        std::cout << "Input sentences, newline-separated (Ctrl-D to end):\n";
        std::string sentence;
        while (!std::cin.eof()){
            std::string sentence;
            std::getline(std::cin, sentence);
            std::istringstream ss;
            ss.str(sentence);
            std::pair<std::string,std::string> kv {"", ""};
            std::string tok {""}, ctok {""}, temp;
            int spaces = 0;
            while (std::getline(ss, temp, ' ')) {
                ctok = (ctok == "" ? temp : ctok + " " + temp);
                if (++spaces == 2) {
                    if (tok == "") {
                        mdb.insertSentenceStarter(ctok);
                    }
                    else {
                        mdb.insertHunk(tok, ctok);
                    }
                    tok = ctok;
                    spaces = 0;
                    ctok = "";
                }
            }
            if (ctok != "") mdb.insertHunk(tok, ctok);
        }
        std::cin.clear();
        std::cout << "\nSaving database...\n";
        if (!mdb.saveCorpus(filename)) {
            std::cerr << "\nERROR: Save failed! (Sorry for losing your data!)\n";
        }
        else std::cout << "Complete.\n";
    }
    else {
        std::cout << "Generating sentences (Enter for new, Control-D to stop)\n\n";
        mdb.recalcWeights();
        while (!std::cin.eof()) {
            std::cout << "inebriated says:\n\t" << mdb.build_sentence() << "\n";
            std::string str;
            std::getline(std::cin, str);
        };
    }
    std::cout << "\nThank you for using inebriated C++ edition.\n";
};
