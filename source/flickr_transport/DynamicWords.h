#ifndef TA2_SRI_DYNAMICWORDS_H
#define TA2_SRI_DYNAMICWORDS_H

class DynamicWords {

public:
  DynamicWords(const std::string& word_file, const std::string& static_word, int min_num_words, int max_num_words);
  static long REFRESH_INTERVAL;

  std::vector<std::string> words(long index);
  std::string tags(long index);

  std::vector<std::string> word_list;
  int l_word_list = 0;

};

DynamicWords makeDynamicWords(const std::string, long);
std::string  getCurrentTags(DynamicWords dw, long);
std::string  getCurrentTags2(long);


#endif  //  __TA2_SRI_DYNAMICWORDS_H__
