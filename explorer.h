#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <future>
#include <utility>
#include <algorithm>
#include <deque>

using namespace std;

int64_t PrefixOffset(string_view text) {
    return text.find_first_not_of(' ');
}

vector<string_view> SplitIntoWords(string_view text) {
    vector<string_view> words;

    int64_t end = text.npos;
    while(true) {
        int64_t offset = PrefixOffset(text);
        if (offset == end) break;

        text.remove_prefix(offset);
        int64_t space = text.find(' ', 0);
        words.push_back(space == end ? text : text.substr(0, space));

        if (space == end) break;
        text.remove_prefix(space);
    }

    return words;
}

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (auto& [word, freq] : other.word_frequences) {
            word_frequences[word] += freq;
        }
    }
};

using KeyWords = set<string, less<>>;
using Chunk = vector<string>;
using ChunkSplitted = vector<vector<string_view>>;

Stats GetPublicationStat(const KeyWords& keys, const vector<string_view>& words) {
    map<string, int> stat;

    for (const string& key : keys) {
        stat[key] = count(words.begin(), words.end(), key);
    }

    Stats result;
    result.word_frequences = move(stat);
    return result;
}

Stats GetChunkStat(const Chunk& publications, const KeyWords& key_words) {
    Stats result;
    ChunkSplitted words;
    for (const string& publication : publications) {
        words.push_back(SplitIntoWords(publication));
    }

    for (const vector<string_view>& publication : words) {
        result += GetPublicationStat(key_words, publication);
    }
    return result;
}


Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    Stats result;
    vector<future<Stats>> thread_storage;

    deque<Chunk> chunks;
    Chunk current_chunk;

    while (input) {
        string str;
        getline(input, str);
        if (str != ""s) {
            current_chunk.push_back(move(str));
        }

        if (current_chunk.size() % 5000 == 0 || !input) {

            chunks.push_back(move(current_chunk));

            thread_storage.push_back(
                    async(GetChunkStat, ref(chunks[chunks.size() - 1]), ref(key_words))
            );

        }

    }

    for (future<Stats>& report : thread_storage) {
        result += report.get();
    }

    return result;
}
