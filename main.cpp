#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "markov.hpp"

int main() {
    std::cout << "inebriated, C++ edition v0.2.0\nan eta thing <http://theta.eu.org>\n\n";
    Markov::DB mdb {};
    std::cout << "Reading from database...\n";
    mdb.doReadingThings();
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
    std::cout << "\nSentence input complete.\nGenerating sentences (Enter for new, Control-D to stop)\n\n";
    mdb.recalcWeights();
    while (!std::cin.eof()) {
        mdb.build_sentence();
        std::string str;
        std::getline(std::cin, str);
    };
    std::cout << "\nSaving database...\n";
    mdb.doSavingThings();
    std::cout << "Save complete.\n"
              << "\nThank you for using inebriated C++ edition.\n";
};
