#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace test {

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

class Registry {
public:
    // Register a test with a name and function.
    void Add(const std::string& name, std::function<void()> fn) {
        tests_.push_back({name, std::move(fn)});
    }

    // Execute all tests and return exit code.
    int RunAll() const {
        int failed = 0;
        for (const auto& test : tests_) {
            try {
                test.fn();
                std::cout << "[PASS] " << test.name << '\n';
            } catch (const std::exception& ex) {
                ++failed;
                std::cout << "[FAIL] " << test.name << " - " << ex.what() << '\n';
            } catch (...) {
                ++failed;
                std::cout << "[FAIL] " << test.name << " - unknown error" << '\n';
            }
        }
        if (failed > 0) {
            std::cout << failed << " test(s) failed." << '\n';
        }
        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

// Return the singleton test registry.
inline Registry& GetRegistry() {
    static Registry registry;
    return registry;
}

// Register a test case at static initialization time.
inline bool Register(const std::string& name, std::function<void()> fn) {
    GetRegistry().Add(name, std::move(fn));
    return true;
}

// Fail a test with a formatted message.
inline void Fail(const std::string& expr, const std::string& file, int line) {
    std::ostringstream oss;
    oss << file << ':' << line << " REQUIRE failed: " << expr;
    throw std::runtime_error(oss.str());
}

// Run all registered tests.
inline int RunAll() {
    return GetRegistry().RunAll();
}

}  // namespace test

#define TEST_CASE(name) TEST_CASE_IMPL(name, __COUNTER__)
#define TEST_CASE_IMPL(name, id) TEST_CASE_IMPL2(name, id)
#define TEST_CASE_IMPL2(name, id)     static void test_fn_##id();     static bool test_reg_##id = ::test::Register(name, test_fn_##id);     static void test_fn_##id()

#define REQUIRE(expr)     do {         if (!(expr)) {             ::test::Fail(#expr, __FILE__, __LINE__);         }     } while (0)

#define REQUIRE_NEAR(a, b, eps)     do {         auto va = (a);         auto vb = (b);         if (std::fabs(va - vb) > (eps)) {             ::test::Fail(#a " ~= " #b, __FILE__, __LINE__);         }     } while (0)
