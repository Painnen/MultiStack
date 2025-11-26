#include "Stack.h"
#include "MultiStack.h"

#include <gtest.h>
#include <fstream>
#include <sstream>

TEST(TMultiStackTest, DefaultConstructor) {
    TMultiStack<int> stack;
    EXPECT_EQ(stack.GetLen(), 0);
    EXPECT_EQ(stack.GetStackCount(), 0);
}

TEST(TMultiStackTest, ParameterizedConstructor) {
    TMultiStack<int> stack(100, 3);
    EXPECT_EQ(stack.GetLen(), 100);
    EXPECT_EQ(stack.GetStackCount(), 3);
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(stack.IsEmpty(i));
    }
}

TEST(TMultiStackTest, InvalidConstructorParameters) {
    EXPECT_THROW(TMultiStack<int>(-1, 3), std::invalid_argument);
    EXPECT_THROW(TMultiStack<int>(10, -1), std::invalid_argument);
}

TEST(TMultiStackTest, PushAndPop) {
    TMultiStack<int> stack(20, 2);
    stack.Push(0, 10);
    stack.Push(0, 20);
    stack.Push(0, 30);
    stack.Push(1, 40);
    stack.Push(1, 50);
    EXPECT_EQ(stack.Pop(0), 30);
    EXPECT_EQ(stack.Pop(0), 20);
    EXPECT_EQ(stack.Pop(1), 50);
    EXPECT_EQ(stack.Pop(0), 10);
    EXPECT_EQ(stack.Pop(1), 40);
}

TEST(TMultiStackTest, PushToInvalidStack) {
    TMultiStack<int> stack(20, 2);
    EXPECT_THROW(stack.Push(-1, 10), std::out_of_range);
    EXPECT_THROW(stack.Push(2, 10), std::out_of_range);
}

TEST(TMultiStackTest, PopFromEmptyStack) {
    TMultiStack<int> stack(20, 2);
    EXPECT_TRUE(stack.IsEmpty(0));
    EXPECT_THROW(stack.Pop(0), std::logic_error);
}

TEST(TMultiStackTest, CopyConstructor) {
    TMultiStack<int> original(20, 2);
    original.Push(0, 100);
    original.Push(1, 200);
    TMultiStack<int> copy(original);
    EXPECT_EQ(original.GetLen(), copy.GetLen());
    EXPECT_EQ(original.GetStackCount(), copy.GetStackCount());
    EXPECT_EQ(copy.Pop(0), 100);
    EXPECT_EQ(copy.Pop(1), 200);
    EXPECT_EQ(original.Pop(0), 100);
    EXPECT_EQ(original.Pop(1), 200);
}

TEST(TMultiStackTest, MoveConstructor) {
    TMultiStack<int> original(20, 2);
    original.Push(0, 100);
    original.Push(1, 200);
    TMultiStack<int> moved(std::move(original));
    EXPECT_EQ(moved.GetLen(), 20);
    EXPECT_EQ(moved.GetStackCount(), 2);
    EXPECT_EQ(moved.Pop(0), 100);
    EXPECT_EQ(moved.Pop(1), 200);
    EXPECT_EQ(original.GetLen(), 0);
    EXPECT_EQ(original.GetStackCount(), 0);
}

TEST(TMultiStackTest, MoveAssignment) {
    TMultiStack<int> original(20, 2);
    original.Push(0, 100);
    original.Push(1, 200);
    TMultiStack<int> moved;
    moved = std::move(original);
    EXPECT_EQ(moved.GetLen(), 20);
    EXPECT_EQ(moved.GetStackCount(), 2);
    EXPECT_EQ(moved.Pop(0), 100);
    EXPECT_EQ(moved.Pop(1), 200);
}

TEST(TMultiStackTest, EqualityOperator) {
    TMultiStack<int> stack1(20, 2);
    stack1.Push(0, 10);
    stack1.Push(1, 20);
    TMultiStack<int> stack2(20, 2);
    stack2.Push(0, 10);
    stack2.Push(1, 20);
    TMultiStack<int> stack3(30, 2);
    stack3.Push(0, 10);
    stack3.Push(1, 20);
    TMultiStack<int> stack4(20, 3);
    EXPECT_TRUE(stack1 == stack2);
    EXPECT_FALSE(stack1 == stack3);
    EXPECT_FALSE(stack1 == stack4);
    EXPECT_TRUE(stack1 != stack3);
}

TEST(TMultiStackTest, StackFullAndRepack) {
    TMultiStack<int> stack(6, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    EXPECT_TRUE(stack.IsFull(0));
}

TEST(TMultiStackTest, OutputOperator) {
    TMultiStack<int> stack(10, 2);
    stack.Push(0, 100);
    stack.Push(1, 200);
    std::stringstream ss;
    ss << stack;
    std::string output = ss.str();
    EXPECT_TRUE(output.find("TMultiStack") != std::string::npos);
    EXPECT_TRUE(output.find("StackCount=2") != std::string::npos);
}

TEST(TMultiStackTest, EdgeCases) {
    TMultiStack<int> stack1(0, 0);
    EXPECT_EQ(stack1.GetLen(), 0);
    EXPECT_EQ(stack1.GetStackCount(), 0);
    TMultiStack<int> stack2(10, 1);
    stack2.Push(0, 42);
    EXPECT_EQ(stack2.Pop(0), 42);
}

TEST(TMultiStackRepackTest, RepackWhenOneStackOverflows) {
    TMultiStack<int> stack(10, 3);
    for (int i = 0; i < 3; i++) {
        stack.Push(0, i + 10);
    }
    stack.Push(2, 30);
    stack.Push(2, 31);
    stack.Push(0, 13);
    EXPECT_EQ(stack.Pop(0), 13);
    EXPECT_EQ(stack.Pop(0), 12);
    EXPECT_EQ(stack.Pop(0), 11);
    EXPECT_EQ(stack.Pop(0), 10);
    EXPECT_EQ(stack.Pop(2), 31);
    EXPECT_EQ(stack.Pop(2), 30);
    EXPECT_TRUE(stack.IsEmpty(1));
}

TEST(TMultiStackRepackTest, RepackWithMinimalSize) {
    TMultiStack<int> stack(6, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(1, 5);
    stack.Push(1, 6);
    EXPECT_EQ(stack.Pop(0), 3);
    EXPECT_EQ(stack.Pop(0), 2);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(1), 6);
    EXPECT_EQ(stack.Pop(1), 5);
    EXPECT_EQ(stack.Pop(1), 4);
}

TEST(TMultiStackRepackTest, RepackWithDifferentDataTypes) {
    TMultiStack<std::string> stack(8, 2);
    stack.Push(0, "hello");
    stack.Push(0, "world");
    stack.Push(0, "test");
    stack.Push(1, "foo");
    stack.Push(1, "bar");
    stack.Push(0, "new");
    EXPECT_EQ(stack.Pop(0), "new");
    EXPECT_EQ(stack.Pop(0), "test");
    EXPECT_EQ(stack.Pop(0), "world");
    EXPECT_EQ(stack.Pop(0), "hello");
    EXPECT_EQ(stack.Pop(1), "bar");
    EXPECT_EQ(stack.Pop(1), "foo");
}

TEST(TMultiStackRepackTest, NoRepackWhenSpaceAvailable) {
    TMultiStack<int> stack(10, 2);
    for (int i = 0; i < 3; i++) {
        stack.Push(0, i);
    }
    stack.Push(0, 3);
    EXPECT_EQ(stack.Pop(0), 3);
    EXPECT_EQ(stack.Pop(0), 2);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(0), 0);
    EXPECT_TRUE(stack.IsEmpty(1));
}

TEST(TMultiStackRepackTest, MinimalRepackTest) {
    TMultiStack<int> stack(4, 2);
    stack.Push(0, 1);
    stack.Push(1, 2);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(1), 2);
    SUCCEED();
}

TEST(TMultiStackRepackTest, OperationsWithoutRepack) {
    TMultiStack<int> stack(20, 3);
    stack.Push(0, 1);
    stack.Push(1, 2);
    stack.Push(2, 3);
    stack.Push(0, 4);
    stack.Pop(1);
    stack.Push(2, 5);
    EXPECT_EQ(stack.Pop(0), 4);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_TRUE(stack.IsEmpty(1));
    EXPECT_EQ(stack.Pop(2), 5);
    EXPECT_EQ(stack.Pop(2), 3);
}

TEST(TMultiStackRepackTest, StateChecking) {
    TMultiStack<int> stack(5, 2);
    EXPECT_TRUE(stack.IsEmpty(0));
    EXPECT_TRUE(stack.IsEmpty(1));
    EXPECT_FALSE(stack.IsFull(0));
    EXPECT_FALSE(stack.IsFull(1));
    stack.Push(0, 1);
    EXPECT_FALSE(stack.IsEmpty(0));
    EXPECT_FALSE(stack.IsFull(0));
    stack.Push(0, 2);
    stack.Push(0, 3);
    if (stack.IsFull(0)) {
        std::cout << "Stack 0 is full" << std::endl;
    }
    SUCCEED();
}

TEST(RepackTest, RepackCalledOnOverflow) {
    TMultiStack<int> stack(6, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(0, 5);
    EXPECT_EQ(stack.Pop(0), 5);
    EXPECT_EQ(stack.Pop(0), 3);
    EXPECT_EQ(stack.Pop(0), 2);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(1), 4);
}

TEST(RepackTest, RepackRedistributesMemory) {
    TMultiStack<int> stack(8, 2);
    stack.Push(0, 10);
    stack.Push(0, 20);
    stack.Push(0, 30);
    stack.Push(1, 40);
    stack.Push(0, 40);
    EXPECT_FALSE(stack.IsEmpty(0));
    EXPECT_FALSE(stack.IsEmpty(1));
}

TEST(RepackTest, RepackWithEmptyStack) {
    TMultiStack<int> stack(10, 3);
    stack.Push(0, 100);
    stack.Push(0, 101);
    stack.Push(0, 102);
    EXPECT_TRUE(stack.IsEmpty(1));
    EXPECT_TRUE(stack.IsEmpty(2));
    stack.Push(0, 103);
    EXPECT_NO_THROW(stack.IsEmpty(1));
    EXPECT_NO_THROW(stack.IsEmpty(2));
}

TEST(RepackTest, RepackPreservesAllData) {
    TMultiStack<int> stack(12, 3);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(1, 3);
    stack.Push(1, 4);
    stack.Push(1, 5);
    stack.Push(2, 6);
    stack.Push(0, 7);
    int totalElements = 0;
    while (!stack.IsEmpty(0)) {
        stack.Pop(0);
        totalElements++;
    }
    while (!stack.IsEmpty(1)) {
        stack.Pop(1);
        totalElements++;
    }
    while (!stack.IsEmpty(2)) {
        stack.Pop(2);
        totalElements++;
    }
    EXPECT_EQ(totalElements, 7);
}

TEST(RepackTest, MultipleRepackCalls) {
    TMultiStack<int> stack(10, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(0, 4);
    stack.Push(0, 5);
    stack.Push(1, 6);
    stack.Push(1, 7);
    stack.Push(1, 8);
    stack.Push(1, 9);
    stack.Push(1, 10);
    EXPECT_FALSE(stack.IsEmpty(0));
    EXPECT_FALSE(stack.IsEmpty(1));
}

TEST(RepackTest, RepackPreservesLIFO) {
    TMultiStack<int> stack(8, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(1, 5);
    stack.Push(0, 6);
    EXPECT_EQ(stack.Pop(0), 6);
    EXPECT_EQ(stack.Pop(0), 3);
    EXPECT_EQ(stack.Pop(0), 2);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(1), 5);
    EXPECT_EQ(stack.Pop(1), 4);
}

TEST(RepackTest, RepackWhenCompletelyFull) {
    TMultiStack<int> stack(6, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(1, 5);
    stack.Push(1, 6);
    EXPECT_THROW(stack.Push(0, 7), std::logic_error);
}

TEST(RepackTest, RepackManyStacks) {
    TMultiStack<int> stack(20, 5);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(2, 5);
    stack.Push(2, 6);
    stack.Push(2, 7);
    stack.Push(2, 8);
    stack.Push(3, 9);
    stack.Push(0, 10);
    for (int i = 0; i < 5; i++) {
        EXPECT_NO_THROW(stack.IsEmpty(i));
    }
}

TEST(RepackTest, RepackMinimalMultiStack) {
    TMultiStack<int> stack(3, 2);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(1, 3);
    EXPECT_THROW(stack.Push(0, 4), std::logic_error);
}

TEST(RepackTest, RepackAfterManyOperations) {
    TMultiStack<int> stack(15, 3);
    for (int i = 0; i < 5; i++) {
        stack.Push(0, i);
        stack.Push(1, i + 10);
        stack.Push(2, i + 20);
    }
    for (int i = 0; i < 3; i++) {
        stack.Pop(0);
        stack.Pop(1);
        stack.Pop(2);
    }
    for (int i = 0; i < 4; i++) {
        stack.Push(0, i + 100);
    }
    stack.Push(1, 50);
    EXPECT_NO_THROW(stack.Pop(0));
    EXPECT_NO_THROW(stack.Pop(1));
    EXPECT_NO_THROW(stack.Pop(2));
}

TEST(RepackTest, RepackDifferentDataTypes) {
    TMultiStack<std::string> stack(8, 2);
    stack.Push(0, "hello");
    stack.Push(0, "world");
    stack.Push(0, "test");
    stack.Push(1, "foo");
    stack.Push(1, "bar");
    stack.Push(0, "new");
    EXPECT_EQ(stack.Pop(0), "new");
    EXPECT_EQ(stack.Pop(0), "test");
    EXPECT_EQ(stack.Pop(0), "world");
    EXPECT_EQ(stack.Pop(0), "hello");
    EXPECT_EQ(stack.Pop(1), "bar");
    EXPECT_EQ(stack.Pop(1), "foo");
}

TEST(RepackTest, RepackDoesNotBreakPointers) {
    TMultiStack<int> stack(10, 2);
    stack.Push(0, 100);
    stack.Push(0, 200);
    stack.Push(1, 300);
    stack.Push(0, 400);
    stack.Push(1, 500);
    stack.Push(0, 600);
    EXPECT_EQ(stack.Pop(0), 600);
    EXPECT_EQ(stack.Pop(0), 400);
    EXPECT_EQ(stack.Pop(0), 200);
    EXPECT_EQ(stack.Pop(0), 100);
    EXPECT_EQ(stack.Pop(1), 500);
    EXPECT_EQ(stack.Pop(1), 300);
}

TEST(RepackTest, RepackSingleStack) {
    TMultiStack<int> stack(5, 1);
    stack.Push(0, 1);
    stack.Push(0, 2);
    stack.Push(0, 3);
    stack.Push(0, 4);
    stack.Push(0, 5);
    EXPECT_TRUE(stack.IsFull(0));
    EXPECT_THROW(stack.Push(0, 6), std::logic_error);
}

TEST(RepackTest, RepackLIFOOrder) {
    TMultiStack<int> stack(8, 2);
    stack.Push(0, 1);
    stack.Push(1, 2);
    stack.Push(0, 3);
    stack.Push(1, 4);
    stack.Push(0, 5);
    stack.Push(0, 6);
    EXPECT_EQ(stack.Pop(0), 6);
    EXPECT_EQ(stack.Pop(0), 5);
    EXPECT_EQ(stack.Pop(0), 3);
    EXPECT_EQ(stack.Pop(0), 1);
    EXPECT_EQ(stack.Pop(1), 4);
    EXPECT_EQ(stack.Pop(1), 2);
}