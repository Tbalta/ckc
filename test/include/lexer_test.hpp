#pragma once
#define TEST 1
#include "lexer.hpp"
#include <gmock/gmock.h>

using namespace Lexer;
class MockTokenStream : public TokenStream
{
public:
    using TokenStream::TokenStream;

    MOCK_METHOD(void, unexpectedToken, (Token, std::optional<TokenType>), (override));
};