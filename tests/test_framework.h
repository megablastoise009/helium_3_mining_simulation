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
    void Add(const std::string& name, std::function<void()> fn) {
        tests_.push_back({name, std::move(fn)});
    }

    int RunAll() const {
        int failed = 0;
        for (const auto& test : tests_) {
            try {
                test.fn();
                std::cout << "[PASS] " << test.name << '
';
            } catch (const std::exception& ex) {
                ++failed;
                std::cout << "[FAIL] " << test.name << " - " << ex.what() << '
';
            } catch (...) {
                ++failed;
                std::cout << "[FAIL] " << test.name << " - unknown error" << '
';
            }
        }
        if (failed > 0) {
            std::cout << failed << " test(s) failed." << '
';
        }
        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

inline Registry& GetRegistry() {
    static Registry registry;
    return registry;
}

inline bool Register(const std::string& name, std::function<void()> fn) {
    GetRegistry().Add(name, std::move(fn));
    return true;
}

inline void Fail(const std::string& expr, const std::string& file, int line) {
    std::ostringstream oss;
    oss << file << ':' << line << " REQUIRE failed: " << expr;
    throw std::runtime_error(oss.str());
}

inline int RunAll() {
    return GetRegistry().RunAll();
}

}  // namespace test

#define TEST_CASE(name) TEST_CASE_IMPL(name, __COUNTER__)
#define TEST_CASE_IMPL(name, id)     static void test_fn_##id();     static bool test_reg_##id = ::test::Register(name, test_fn_##id);     static void test_fn_##id()

#define REQUIRE(expr)     do {         if (!(expr)) {             ::test::Fail(#expr, __FILE__, __LINE__);         }     } while (0)

#define REQUIRE_NEAR(a, b, eps)     do {         auto va = (a);         auto vb = (b);         if (std::fabs(va - vb) > (eps)) {             ::test::Fail(#a " ~= " #b, __FILE__, __LINE__);         }     } while (0)
