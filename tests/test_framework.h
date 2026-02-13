#pragma once

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace test {

using TestFn = void (*)();

// Fail a test with a formatted message.
inline void Fail(const std::string& expr, const std::string& file, int line) {
    std::ostringstream oss;
    oss << file << ':' << line << " REQUIRE failed: " << expr;
    throw std::runtime_error(oss.str());
}

// Run a single test and print its result.
inline int RunTest(const std::string& name, TestFn fn) {
    try {
        fn();
        std::cout << "[PASS] " << name << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cout << "[FAIL] " << name << " - " << ex.what() << '\n';
        return 1;
    } catch (...) {
        std::cout << "[FAIL] " << name << " - unknown error" << '\n';
        return 1;
    }
}

// Run all tests in a list and return an exit code.
inline int RunAll(const std::vector<std::pair<std::string, TestFn>>& tests) {
    int failed = 0;
    for (const auto& test : tests) {
        failed += RunTest(test.first, test.second);
    }
    if (failed > 0) {
        std::cout << failed << " test(s) failed." << '\n';
    }
    return failed == 0 ? 0 : 1;
}

}  // namespace test

#define REQUIRE(expr)     do {         if (!(expr)) {             ::test::Fail(#expr, __FILE__, __LINE__);         }     } while (0)

#define REQUIRE_NEAR(a, b, eps)     do {         auto va = (a);         auto vb = (b);         if (std::fabs(va - vb) > (eps)) {             ::test::Fail(#a " ~= " #b, __FILE__, __LINE__);         }     } while (0)
