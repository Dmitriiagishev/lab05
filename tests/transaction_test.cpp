#include "Transaction.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class TransactionMock : public Transaction {
public:
    MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
};

TEST(Transaction, Mock) {
    TransactionMock tr;
    Account acc_1(1, 100);
    Account acc_2(2, 300);
    EXPECT_CALL(tr, Make(testing::_, testing::_, testing::_))
        .Times(5);
    tr.set_fee(200);
    tr.Make(acc_1, acc_2, 200);
    tr.Make(acc_2, acc_1, 300);
    tr.Make(acc_1, acc_1, 0);
    tr.Make(acc_1, acc_2, -5);
    tr.Make(acc_2, acc_1, 50);
}

TEST(Transaction, SimpleTest) {
    Transaction tr;
    Account acc_1(1, 100);
    Account acc_2(2, 300);
    tr.set_fee(10);
    EXPECT_EQ(tr.fee(), 10);
    EXPECT_THROW(tr.Make(acc_1, acc_2, 40), std::logic_error);
    EXPECT_THROW(tr.Make(acc_1, acc_2, -5), std::invalid_argument);
    EXPECT_THROW(tr.Make(acc_1, acc_1, 100), std::logic_error);
    EXPECT_FALSE(tr.Make(acc_1, acc_2, 400));
    EXPECT_FALSE(tr.Make(acc_2, acc_1, 300));
    EXPECT_FALSE(tr.Make(acc_2, acc_1, 290));
    EXPECT_TRUE(tr.Make(acc_2, acc_1, 150));
}
