#include "ndarray.hpp"
#include <type_traits>

#include <gtest/gtest.h>

#include <array>

TEST(NDArrayTest, DefaultConstructor)
{
	NDArray< int, 1 > arr;
	EXPECT_EQ(arr.count(), 0);
	EXPECT_EQ(arr.total_count(), 0);
	EXPECT_EQ(arr.dim(), 1);
}

TEST(NDArrayTest, ShapeConstructor1D)
{
	NDArray< int, 1 > arr({ 5 }, 10);
	EXPECT_EQ(arr.count(), 5);
	EXPECT_EQ(arr.total_count(), 5);
	EXPECT_EQ(arr.dim(), 1);

	for (int i = 0; i < 5; ++i)
	{
		EXPECT_EQ(arr[i], 10);
	}
}

TEST(NDArrayTest, ShapeConstructor2D)
{
	NDArray< int, 2 > arr({ 2, 3 }, 5);
	EXPECT_EQ(arr.count(), 2);
	EXPECT_EQ(arr.total_count(), 6);
	EXPECT_EQ(arr.dim(), 2);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			EXPECT_EQ(arr.at({ static_cast< size_t >(i), static_cast< size_t >(j) }), 5);
		}
	}
}

TEST(NDArrayTest, InitializerList2D)
{
	NDArray< int, 2 > arr{ { 1, 2, 3 }, { 4, 5, 6 } };
	EXPECT_EQ(arr.count(), 2);
	EXPECT_EQ(arr.total_count(), 6);
	EXPECT_EQ(arr.dim(), 2);

	EXPECT_EQ(arr.at({ 0, 0 }), 1);
	EXPECT_EQ(arr.at({ 0, 1 }), 2);
	EXPECT_EQ(arr.at({ 0, 2 }), 3);
	EXPECT_EQ(arr.at({ 1, 0 }), 4);
	EXPECT_EQ(arr.at({ 1, 1 }), 5);
	EXPECT_EQ(arr.at({ 1, 2 }), 6);
}

TEST(NDArrayTest, CopyConstructor)
{
	NDArray< int, 2 > original{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > copy(original);

	EXPECT_EQ(copy.count(), 2);
	EXPECT_EQ(copy.total_count(), 4);
	EXPECT_EQ(copy.at({ 0, 0 }), 1);
	EXPECT_EQ(copy.at({ 1, 1 }), 4);
	EXPECT_TRUE(original.is_equal(copy));
}

TEST(NDArrayTest, MoveConstructor)
{
	NDArray< int, 2 > original{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > moved(std::move(original));

	EXPECT_EQ(moved.count(), 2);
	EXPECT_EQ(moved.total_count(), 4);
	EXPECT_EQ(moved.at({ 0, 0 }), 1);
	EXPECT_EQ(moved.at({ 1, 1 }), 4);
	EXPECT_EQ(original.count(), 0);
}

TEST(NDArrayTest, CopyAssignment)
{
	NDArray< int, 2 > original{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > copy;
	copy = original;

	EXPECT_EQ(copy.count(), 2);
	EXPECT_EQ(copy.total_count(), 4);
	EXPECT_EQ(copy.at({ 0, 0 }), 1);
	EXPECT_EQ(copy.at({ 1, 1 }), 4);
	EXPECT_TRUE(original.is_equal(copy));
}

TEST(NDArrayTest, MoveAssignment)
{
	NDArray< int, 2 > original{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > moved;
	moved = std::move(original);

	EXPECT_EQ(moved.count(), 2);
	EXPECT_EQ(moved.total_count(), 4);
	EXPECT_EQ(moved.at({ 0, 0 }), 1);
	EXPECT_EQ(moved.at({ 1, 1 }), 4);
	EXPECT_EQ(original.count(), 0);
}

TEST(NDArrayTest, OperatorBrackets2D)
{
	NDArray< int, 2 > arr({ 2, 3 });
	int counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			arr.at({ static_cast< size_t >(i), static_cast< size_t >(j) }) = counter++;
		}
	}

	counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			EXPECT_EQ(arr.at({ static_cast< size_t >(i), static_cast< size_t >(j) }), counter++);
		}
	}
}

TEST(NDArrayTest, OperatorBrackets3D)
{
	NDArray< int, 3 > arr({ 2, 2, 2 });
	int counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				arr.at({ static_cast< size_t >(i), static_cast< size_t >(j), static_cast< size_t >(k) }) = counter++;
			}
		}
	}

	counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				EXPECT_EQ(arr.at({ static_cast< size_t >(i), static_cast< size_t >(j), static_cast< size_t >(k) }), counter++);
			}
		}
	}
}

TEST(NDArrayTest, AtMethod2D)
{
	NDArray< int, 2 > arr({ 2, 3 });
	int counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			arr.at({ static_cast< size_t >(i), static_cast< size_t >(j) }) = counter++;
		}
	}

	counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			EXPECT_EQ(arr.at({ static_cast< size_t >(i), static_cast< size_t >(j) }), counter++);
		}
	}
}

TEST(NDArrayTest, AtMethod3D)
{
	NDArray< int, 3 > arr({ 2, 2, 2 });
	int counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				arr.at({ static_cast< size_t >(i), static_cast< size_t >(j), static_cast< size_t >(k) }) = counter++;
			}
		}
	}

	counter = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				EXPECT_EQ(arr.at({ static_cast< size_t >(i), static_cast< size_t >(j), static_cast< size_t >(k) }), counter++);
			}
		}
	}
}

TEST(NDArrayTest, AtMethodOutOfRange)
{
	NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };

	EXPECT_THROW(arr.at({ 2, 0 }), std::out_of_range);
	EXPECT_THROW(arr.at({ 0, 2 }), std::out_of_range);
	EXPECT_THROW(arr.at({ 0 }), std::out_of_range);
	EXPECT_THROW(arr.at({ 0, 0, 0 }), std::out_of_range);
}

TEST(NDArrayTest, Iterators)
{
	NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };

	int expected[] = { 1, 2, 3, 4 };
	int i = 0;

	for (auto it = arr.begin(); it != arr.end(); ++it)
	{
		EXPECT_EQ(*it, expected[i++]);
	}

	i = 0;
	for (auto val : arr)
	{
		EXPECT_EQ(val, expected[i++]);
	}
}

TEST(NDArrayTest, ConstIterators)
{
	const NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };

	int expected[] = { 1, 2, 3, 4 };
	int i = 0;

	for (auto it = arr.cbegin(); it != arr.cend(); ++it)
	{
		EXPECT_EQ(*it, expected[i++]);
	}
}

TEST(NDArrayTest, IsEqual)
{
	NDArray< int, 2 > arr1{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > arr2{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > arr3{ { 1, 2 }, { 3, 5 } };
	NDArray< int, 2 > arr4{ { 1, 2 } };

	EXPECT_TRUE(arr1.is_equal(arr2));
	EXPECT_FALSE(arr1.is_equal(arr3));
	EXPECT_FALSE(arr1.is_equal(arr4));
}

TEST(NDArrayTest, Reshape)
{
	NDArray< int, 2 > arr{ { 1, 2, 3 }, { 4, 5, 6 } };
	auto reshaped = arr.reshape< 3 >({ 3, 2, 1 });
	EXPECT_EQ(reshaped.count(), 3);
	EXPECT_EQ(reshaped.total_count(), 6);
	EXPECT_EQ(reshaped.at({ 0, 0, 0 }), 1);
	EXPECT_EQ(reshaped.at({ 2, 0, 0 }), 5);
	arr.at({ 0, 0 }) = 10;
	EXPECT_EQ(reshaped.at({ 0, 0, 0 }), 10);
}

TEST(NDArrayTest, ReshapeInvalid)
{
	NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };

	EXPECT_THROW(arr.reshape< 3 >({ 2, 2, 2 }), std::invalid_argument);
}

TEST(NDArrayTest, Swap)
{
	NDArray< int, 2 > arr1{ { 1, 2 }, { 3, 4 } };
	NDArray< int, 2 > arr2{ { 5, 6 }, { 7, 8 }, { 9, 10 } };

	arr1.swap(arr2);

	EXPECT_EQ(arr1.count(), 3);
	EXPECT_EQ(arr1.total_count(), 6);
	EXPECT_EQ(arr1.at({ 0, 0 }), 5);
	EXPECT_EQ(arr1.at({ 2, 1 }), 10);

	EXPECT_EQ(arr2.count(), 2);
	EXPECT_EQ(arr2.total_count(), 4);
	EXPECT_EQ(arr2.at({ 0, 0 }), 1);
	EXPECT_EQ(arr2.at({ 1, 1 }), 4);
}

TEST(NDArrayViewTest, ConstView)
{
	const NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };
	auto view = arr[0];

	EXPECT_EQ(view.count(), 2);
	EXPECT_EQ(view.total_count(), 2);
	EXPECT_EQ(view[0], 1);
	EXPECT_EQ(view[1], 2);
}

TEST(NDArrayTypeTest, DoubleType)
{
	NDArray< double, 2 > arr({ 2, 2 }, 1.5);

	EXPECT_DOUBLE_EQ(arr.at({ 0, 0 }), 1.5);
	EXPECT_DOUBLE_EQ(arr.at({ 1, 1 }), 1.5);
}

TEST(NDArrayTypeTest, StringType)
{
	NDArray< std::string, 2 > arr({ 2, 2 }, "test");

	EXPECT_EQ(arr.at({ 0, 0 }), "test");
	EXPECT_EQ(arr.at({ 1, 1 }), "test");
}

TEST(NDArrayEdgeTest, EmptyArray)
{
	NDArray< int, 2 > arr({ 0, 0 });

	EXPECT_EQ(arr.count(), 0);
	EXPECT_EQ(arr.total_count(), 0);
	EXPECT_EQ(arr.dim(), 2);
}

TEST(NDArrayEdgeTest, SingleElement)
{
	NDArray< int, 2 > arr({ 1, 1 }, 42);

	EXPECT_EQ(arr.count(), 1);
	EXPECT_EQ(arr.total_count(), 1);
	EXPECT_EQ(arr.at({ 0, 0 }), 42);
}

TEST(NDArrayExceptionTest, InvalidInitializerList)
{
	EXPECT_THROW((NDArray< int, 2 >{ { 1, 2 }, { 3 } }), std::invalid_argument);
}

TEST(NDArraySizeTest, Dimensions)
{
	NDArray< int, 3 > arr({ 2, 3, 4 }, 0);

	EXPECT_EQ(arr.count(), 2);
	EXPECT_EQ(arr.total_count(), 24);
	EXPECT_EQ(arr.dim(), 3);
}

TEST(NDArrayConstTest, ConstAccess)
{
	const NDArray< int, 2 > arr{ { 1, 2 }, { 3, 4 } };

	EXPECT_EQ(arr.at({ 0, 0 }), 1);
	EXPECT_EQ(arr.at({ 1, 1 }), 4);
}

TEST(NDArrayNestedTest, NestedOperations)
{
	NDArray< int, 3 > arr({ 2, 2, 2 });
	int counter = 0;

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				arr.at({ static_cast< size_t >(i), static_cast< size_t >(j), static_cast< size_t >(k) }) = counter++;
			}
		}
	}

	auto view1 = arr[0];
	auto view2 = view1[1];
	EXPECT_EQ(view2[0], 2);
	EXPECT_EQ(view2[1], 3);
}

TEST(NDArrayDimensionTest, DifferentDimensions)
{
	NDArray< int, 1 > arr1({ 5 }, 1);
	NDArray< int, 2 > arr2({ 3, 4 }, 2);
	NDArray< int, 3 > arr3({ 2, 3, 4 }, 3);
	NDArray< int, 4 > arr4({ 2, 2, 3, 4 }, 4);

	EXPECT_EQ(arr1.dim(), 1);
	EXPECT_EQ(arr2.dim(), 2);
	EXPECT_EQ(arr3.dim(), 3);
	EXPECT_EQ(arr4.dim(), 4);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
