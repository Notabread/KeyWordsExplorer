#include <sstream>
#include <iostream>

#include "explorer.h"
#include "log_duration.h"

using namespace std;

int main() {
    const KeyWords key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    for(int i = 0; i < 5001; ++i)
        ss << "rocks yangle rocks all\n"s;

    Stats stats;
    {
        LOG_DURATION("ExploreKeyWords"s);
        stats = ExploreKeyWords(key_words, ss);
    }


    for (const auto& [word, frequency] : stats.word_frequences) {
        cout << word << " " << frequency << endl;
    }

    return 0;
}
