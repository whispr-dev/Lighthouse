#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>

enum class TokenType {
    ObjectStart,
    ObjectEnd,
    String,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokenizer {
public:
    Tokenizer(const std::string& input);
    Token next();
    void skipWhitespace();
    Token parseString();

private:
    std::string src;
    size_t pos = 0;
};

#endif
