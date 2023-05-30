## Laboratory work V

[![Coverage Status](https://coveralls.io/repos/github/Dmitriiagishev/lab05/badge.svg?branch=main)](https://coveralls.io/github/Dmitriiagishev/lab05?branch=main)

Данная лабораторная работа посвещена изучению фреймворков для тестирования на примере **GTest**

```sh
$ open https://github.com/google/googletest
```

## Tasks

- [ ] 1. Создать публичный репозиторий с названием **lab05** на сервисе **GitHub**
- [ ] 2. Выполнить инструкцию учебного материала
- [ ] 3. Ознакомиться со ссылками учебного материала
- [ ] 4. Составить отчет и отправить ссылку личным сообщением в **Slack**

## Tutorial

```sh
$ export GITHUB_USERNAME=<имя_пользователя>
$ alias gsed=sed # for *-nix system
```

```sh
$ cd ${GITHUB_USERNAME}/workspace
$ pushd .
$ source scripts/activate
```

```sh
$ git clone https://github.com/${GITHUB_USERNAME}/lab04 projects/lab05
$ cd projects/lab05
$ git remote remove origin
$ git remote add origin https://github.com/${GITHUB_USERNAME}/lab05
```

```sh
$ mkdir third-party
$ git submodule add https://github.com/google/googletest third-party/gtest
$ cd third-party/gtest && git checkout release-1.8.1 && cd ../..
$ git add third-party/gtest
$ git commit -m"added gtest framework"
```

```sh
$ gsed -i '/option(BUILD_EXAMPLES "Build examples" OFF)/a\
option(BUILD_TESTS "Build tests" OFF)
' CMakeLists.txt
$ cat >> CMakeLists.txt <<EOF

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  file(GLOB \${PROJECT_NAME}_TEST_SOURCES tests/*.cpp)
  add_executable(check \${\${PROJECT_NAME}_TEST_SOURCES})
  target_link_libraries(check \${PROJECT_NAME} gtest_main)
  add_test(NAME check COMMAND check)
endif()
EOF
```

```sh
$ mkdir tests
$ cat > tests/test1.cpp <<EOF
#include <print.hpp>

#include <gtest/gtest.h>

TEST(Print, InFileStream)
{
  std::string filepath = "file.txt";
  std::string text = "hello";
  std::ofstream out{filepath};

  print(text, out);
  out.close();

  std::string result;
  std::ifstream in{filepath};
  in >> result;

  EXPECT_EQ(result, text);
}
EOF
```

```sh
$ cmake -H. -B_build -DBUILD_TESTS=ON
$ cmake --build _build
$ cmake --build _build --target test
```

```sh
$ _build/check
$ cmake --build _build --target test -- ARGS=--verbose
```

```sh
$ gsed -i 's/lab04/lab05/g' README.md
$ gsed -i 's/\(DCMAKE_INSTALL_PREFIX=_install\)/\1 -DBUILD_TESTS=ON/' .travis.yml
$ gsed -i '/cmake --build _build --target install/a\
- cmake --build _build --target test -- ARGS=--verbose
' .travis.yml
```

```sh
$ travis lint
```

```sh
$ git add .travis.yml
$ git add tests
$ git add -p
$ git commit -m"added tests"
$ git push origin master
```

```sh
$ travis login --auto
$ travis enable
```

```sh
$ mkdir artifacts
$ sleep 20s && gnome-screenshot --file artifacts/screenshot.png
# for macOS: $ screencapture -T 20 artifacts/screenshot.png
# open https://github.com/${GITHUB_USERNAME}/lab05
```

## Report

```sh
$ popd
$ export LAB_NUMBER=05
$ git clone https://github.com/tp-labs/lab${LAB_NUMBER} tasks/lab${LAB_NUMBER}
$ mkdir reports/lab${LAB_NUMBER}
$ cp tasks/lab${LAB_NUMBER}/README.md reports/lab${LAB_NUMBER}/REPORT.md
$ cd reports/lab${LAB_NUMBER}
$ edit REPORT.md
$ gist REPORT.md
```

## Homework

### Задание
1. Создайте `CMakeList.txt` для библиотеки *banking*.
```sh
cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  add_compile_options(--coverage)
endif()

project (banking)

add_library(banking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp)
target_include_directories(banking PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/banking )

target_link_libraries(banking gcov)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(googletest)
  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
  add_executable(check ${BANKING_TEST_SOURCES})
  target_link_libraries(check banking gtest_main gmock_main)
  add_test(NAME check COMMAND check)
endif()
```
2. Создайте модульные тесты на классы `Transaction` и `Account`.
# Account_test
```sh
#include "Account.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class AccountMock : public Account {
public:
    AccountMock(int id, int balance) : Account(id, balance) {}
    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int diff));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

TEST(Account, Mock) {
    AccountMock acc(1, 666);
    EXPECT_CALL(acc, GetBalance()).Times(1);
    EXPECT_CALL(acc, ChangeBalance(testing::_)).Times(2);
    EXPECT_CALL(acc, Lock()).Times(2);
    EXPECT_CALL(acc, Unlock()).Times(1);
    acc.GetBalance();
    acc.ChangeBalance(100);
    acc.Lock();
    acc.ChangeBalance(100);
    acc.Lock();
    acc.Unlock();
}

TEST(Account, SimpleTest) {
    Account acc(1, 666);
    EXPECT_EQ(acc.id(), 1);
    EXPECT_EQ(acc.GetBalance(), 666);
    EXPECT_THROW(acc.ChangeBalance(200), std::runtime_error);
    EXPECT_NO_THROW(acc.Lock());
    acc.ChangeBalance(200);
    EXPECT_EQ(acc.GetBalance(), 866);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
    EXPECT_NO_THROW(acc.Unlock());
}
```
# Transaction_test
```sh
#include "Transaction.h"
#include "Account.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class TransactionMock : public Transaction {
public:
    MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
};

TEST(Transaction, Mock) {
    TransactionMock tr;
    Account account_1(1, 100);
    Account account_2(2, 300);
    EXPECT_CALL(tr, Make(testing::_, testing::_, testing::_))
        .Times(5);
    tr.set_fee(200);
    tr.Make(account_1, account_2, 200);
    tr.Make(account_2, account_1, 300);
    tr.Make(account_1, account_1, 0);
    tr.Make(account_1, account_2, -5);
    tr.Make(account_2, account_1, 50);
}

TEST(Transaction, SimpleTest) {
    Transaction tr;
    Account account_1(1, 100);
    Account account_2(2, 300);
    tr.set_fee(10);
    EXPECT_EQ(tr.fee(), 10);
    EXPECT_THROW(tr.Make(account_1, account_2, 40), std::logic_error);
    EXPECT_THROW(tr.Make(account_1, account_2, -5), std::invalid_argument);
    EXPECT_THROW(tr.Make(account_1, account_1, 100), std::logic_error);
    EXPECT_FALSE(tr.Make(account_1, account_2, 400));
    EXPECT_FALSE(tr.Make(account_2, account_1, 300));
    EXPECT_FALSE(tr.Make(account_2, account_1, 290));
    EXPECT_TRUE(tr.Make(account_2, account_1, 150));
}
```
    * Используйте mock-объекты.
    * Покрытие кода должно составлять 100%.
3. Настройте сборочную процедуру на **TravisCI**.
4. Настройте [Coveralls.io](https://coveralls.io/).



## Links

- [C++ CI: Travis, CMake, GTest, Coveralls & Appveyor](http://david-grs.github.io/cpp-clang-travis-cmake-gtest-coveralls-appveyor/)
- [Boost.Tests](http://www.boost.org/doc/libs/1_63_0/libs/test/doc/html/)
- [Catch](https://github.com/catchorg/Catch2)

```
Copyright (c) 2015-2021 The ISC Authors
```
