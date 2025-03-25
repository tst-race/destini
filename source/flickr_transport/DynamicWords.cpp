//
// Code for generating tags as lists of words - translated from Python
// by Vinod/ChatGPT.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
#include <random>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <cstring>

#include <numeric>

// If true, generate word lists as hashtags:
#define ADD_HASHTAGS 1
#define ADD_INDEX 1
#define PRINT_LIST 0
#define STANDARD_INTERVAL 600
#define SHORT_INTERVAL 30



static std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    return std::accumulate(vec.begin() + 1, vec.end(), vec.front(),
                           [delimiter](const std::string& a, const std::string& b) {
                               return a + delimiter + b;
                           });
}


static std::vector<std::string> readWordList(const std::string& wordsFile) {
  std::vector<std::string> wlist;
  std::ifstream file(wordsFile);
  int k = 1;

  if (file.is_open()) {
    std::string rawline;
    while (std::getline(file, rawline)) {
      // Need to strip trailing newlines / cr's / whatever:

      unsigned long l = rawline.length();
      std::string word(rawline, 0, l-1);

      if (ADD_HASHTAGS) word = "#"+word;
      if (ADD_INDEX) word = word+std::to_string(k);

      wlist.push_back(word);
      k += 1;
    }

    file.close();
    std::cout << "In readWordList(" + wordsFile + "), list size = " + std::to_string(wlist.size()) << "\n";
  }

#if PRINT_LIST
  for (const auto& word : wlist) {
    std::cout << word << "\n";
  }
#endif

  std::cout << "In readWordList, returning.\n";

  return wlist;
}
  



class DynamicWords {

public:
  long REFRESH_INTERVAL;

  // The guts: compute a "curr_time" expressed in the units of the
  // REFRESH_INTERVAL; curr_time will advance every REFRESH_INTERVAL
  // seconds.
  std::vector<std::string> words(long index = 0) {
    std::time_t curr_time = (std::time(nullptr) + index * REFRESH_INTERVAL) / REFRESH_INTERVAL;

    // Create a vector to be used as a vector of words:
    std::vector<std::string> result;

    std::cout << "curr_time: " << curr_time << "\n";

    // Seed the PRNG with curr_time and the secret salt:
    random.seed( static_cast<unsigned long>(curr_time) + static_cast<unsigned long>(secret_salt) );

    // Choose a length for the vector of words:
    int num_words = static_cast<int>(random()) % (max_num_words - min_num_words + 1) + min_num_words;

    // Create a vector of randomly chosen words.  The vector length is random between min and max:
    for (int i = 0; i < num_words; ++i) {
      // Choose a new word from the list
      unsigned long idx = static_cast<unsigned long>(random()) % static_cast<unsigned long>(l_word_list);

      // add it to the vector:
      result.push_back(word_list[idx]);
    }
    return result;
  }


  std::string tags(long index = 0) {
    std::vector<std::string> r = words(index);
    // Join the random word vector into a comma-separated list:
    return join(r, ",");
  }

#define WORDLIST "./wordlist.txt"
  
  DynamicWords(const std::string& word_file, const std::string& static_word, int min_num_words, int max_num_words, long interval = 600) :
  //  secret_salt(std::abs(std::hash<std::string>{}(static_word)) % static_cast<int>(std::pow(10, 8))),
  REFRESH_INTERVAL(interval),
  secret_salt(std::hash<std::string>{}(static_word) % static_cast<unsigned long>(std::pow(10, 8))),
  random(std::random_device{}()),
  min_num_words(std::max(1, min_num_words)),
  max_num_words(std::max(min_num_words, max_num_words)),
  word_list(readWordList(word_file)) {
} unsigned long secret_salt;
  std::default_random_engine random;
  int min_num_words;
  int max_num_words;
  std::vector<std::string> word_list;
  int l_word_list = word_list.size();
  //  long REFRESH_INTERVAL = interval; // 60 * 10;
};

  std::vector<std::string> word_list;
  int l_word_list;





#if 0
std::vector<std::string> get_words(DynamicWords *dw, long index = 0) {
  std::time_t curr_time = (std::time(nullptr) + index * REFRESH_INTERVAL) / REFRESH_INTERVAL;

  // Create a vector to be used as a vector of words:
  std::vector<std::string> result;

  std::cout << "curr_time: " << curr_time << "\n";

  // Seed the PRNG with curr_time and the secret salt:
  random.seed( static_cast<unsigned long>(curr_time) + static_cast<unsigned long>(secret_salt) );

  // Choose a length for the vector of words:
  int num_words = static_cast<int>(random()) % (max_num_words - min_num_words + 1) + min_num_words;

  // Create a vector of randomly chosen words.  The vector length is random between min and max:
  for (int i = 0; i < num_words; ++i) {
    // Choose a new word from the list
    unsigned long idx = static_cast<unsigned long>(random()) % static_cast<unsigned long>(l_word_list);

    // add it to the vector:
    result.push_back(word_list[idx]);
  }
  return result;
}
  
#endif


#ifdef TEST
static DynamicWords __dw__ = DynamicWords("./wordlist.txt", "secret", 3, 5, 5);
#else
static DynamicWords __dw__ = DynamicWords("/etc/race/wordlist.txt", "secret", 3, 5, 600);
#endif

DynamicWords makeDynamicWords(std::string wfile, long interval) {
  // DynamicWords::Initialize(WORDLIST, interval);
  // DynamicWords dw = DynamicWords::dynamicWordsFor("secret", 3, 5, interval);
  DynamicWords dw = DynamicWords(wfile, "secret", 3, 5, interval);
  return dw;
}

std::string getCurrentTags(DynamicWords dw, long index) {
  return dw.tags(index);
}
    

extern std::string getCurrentTags2(long index) {
  return __dw__.tags(index);
}

#ifdef TEST
// #ifdef TEST
// Main program for testing:
//
// Compile this way to test this WITHIN sri-race-plugins/source/common:
//
//     g++ -std=c++11 -DTEST DynamicWords.cpp -o dw
//
// Then run dw.



#define WORDLIST "./wordlist.txt"
#define USE_JOIN 0

 int main(int argc, char* argv[]) {
   // Initialize the DynamicWords wordlist to be './wordlist.txt' and
   // refresh interval at 5 seconds:
   printf("About to call DynamicWords::Initialize...\n");
#if 0
   DynamicWords::Initialize(WORDLIST, 5);
   
   std::string static_word = (argc > 1) ? argv[1] : "secret";

   // Initialize a DynamicWords list generator that returns between 3
   // and 5 words at a time:
   DynamicWords& dynamic_words = DynamicWords::dynamicWordsFor(static_word, 3, 5);
#endif
   DynamicWords dynamic_words = __dw__;  // makeDynamicWords(WORDLIST, 5);
   
   for (int i = 0; i < 15; ++i) {
     /*
     std::string s1 = dynamic_words.tags(-2);
     std::string s2 = dynamic_words.tags(-1);
     std::string s3 = dynamic_words.tags(0);
     */
     
     std::string s1 = getCurrentTags(dynamic_words, -2);
     std::string s2 = getCurrentTags(dynamic_words, -1);
     std::string s3 = getCurrentTags(dynamic_words,  0);

     std::cout << "\nNext batch:\n";
     std::cout << "      " << s1 << "\n";
     std::cout << "      " << s2 << "\n";
     std::cout << "      " << s3 << "\n";
     std::cout << "\n";
     std::cout << "\n";
     
     std::this_thread::sleep_for(std::chrono::seconds(6));
   }
   
   return 0;
 }
#endif
