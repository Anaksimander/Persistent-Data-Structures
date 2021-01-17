#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PSDTest
{
	TEST_CLASS(PersistentMassTest)
	{
	public:
		TEST_METHOD(set_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			myMass.set(0, 10);
			Assert::AreEqual(10, myMass.get_size());
			Assert::AreEqual(10, myMass.get(0));

			myMass.undo();
			Assert::AreEqual(10, myMass.get_size());
			Assert::AreEqual(0, myMass.get(0));

			myMass.redo();
			Assert::AreEqual(10, myMass.get_size());
			Assert::AreEqual(10, myMass.get(0));
		}

		TEST_METHOD(pushback_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));
			Assert::AreEqual(10, myMass.get_size());

			for (int i = 0; i < 10; i++)
				myMass.undo();
			Assert::AreEqual(0, myMass.get_size());

			for (int i = 0; i < 10; i++)
				myMass.redo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));
			Assert::AreEqual(10, myMass.get_size());
		}


		TEST_METHOD(popback_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			for (int i = 0; i < 10; i++)
				myMass.pop_back();
			Assert::AreEqual(0, myMass.get_size());


			for (int i = 0; i < 10; i++)
				myMass.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));
			Assert::AreEqual(10, myMass.get_size());

			for (int i = 0; i < 10; i++)
				myMass.redo();
			Assert::AreEqual(0, myMass.get_size());
		}

		TEST_METHOD(insert_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			for (int i = 0; i < 10; i++)
				myMass.insert(i, 0);
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(0, myMass.get(i));

			for (int i = 0; i < 10; i++)
				myMass.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));

			for (int i = 0; i < 10; i++)
				myMass.redo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(0, myMass.get(i));
		}

		TEST_METHOD(erase_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			for (int i = 9; i >= 0; i--)
				myMass.erase(i);
			Assert::AreEqual(0, myMass.get_size());

			for (int i = 0; i < 10; i++)
				myMass.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));
			Assert::AreEqual(10, myMass.get_size());

			for (int i = 0; i < 10; i++)
				myMass.redo();
			Assert::AreEqual(0, myMass.get_size());
		}

		TEST_METHOD(clear_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			myMass.clear();
			Assert::AreEqual(0, myMass.get_size());

			myMass.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myMass.get(i));
			Assert::AreEqual(10, myMass.get_size());

			myMass.redo();
			Assert::AreEqual(0, myMass.get_size());
		}

		TEST_METHOD(get_size_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			Assert::AreEqual(10, myMass.get_size());
		}


		TEST_METHOD(get_test) {
			PersistentMass<int> myMass;

			for (int i = 0; i < 10; i++)
				myMass.push_back(i);

			Assert::AreEqual(9, myMass.get(9));
		}

		TEST_METHOD(into_push_back) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).push_back(3);
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.undo();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.redo();
			Assert::AreEqual(3, myList2.get(0).get(0));
		}

		TEST_METHOD(into_set) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).push_back(0);
			myList2.get(0).set(0, 3);
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.undo();
			Assert::AreEqual(0, myList2.get(0).get(0));
			myList2.redo();
			Assert::AreEqual(3, myList2.get(0).get(0));
		}

		TEST_METHOD(into_pop_back) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).push_back(3);
			myList2.get(0).pop_back();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.undo();
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.redo();
			Assert::AreEqual(0, myList2.get(0).get_size());
		}

		TEST_METHOD(into_insert) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).insert(0, 3);
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.undo();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.redo();
			Assert::AreEqual(3, myList2.get(0).get(0));
		}

		TEST_METHOD(into_erase) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).insert(0, 3);
			myList2.get(0).erase(0);
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.undo();
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.redo();
			Assert::AreEqual(0, myList2.get(0).get_size());
		}

		TEST_METHOD(into_clear) {
			PersistentMass<int> myList1;
			PersistentMass<PersistentMass<int>> myList2;

			myList2.push_back(myList1);
			myList2.get(0).insert(0, 3);
			myList2.get(0).clear();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.undo();
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.redo();
			Assert::AreEqual(0, myList2.get(0).get_size());
		}
	};

	TEST_CLASS(Persistentlisttest)
	{
	public:
		TEST_METHOD(set_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			myList.set(0, 10);
			Assert::AreEqual(10, myList.get_size());
			Assert::AreEqual(10, myList.get(0));

			myList.undo();
			Assert::AreEqual(10, myList.get_size());
			Assert::AreEqual(0, myList.get(0));

			myList.redo();
			Assert::AreEqual(10, myList.get_size());
			Assert::AreEqual(10, myList.get(0));
		}

		TEST_METHOD(pushfront_test) {
			PersistentList<int> myList;

			for (int i = 9; i >= 0; i--)
				myList.push_front(i);

			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.undo();
			Assert::AreEqual(0, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.redo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());
		}

		TEST_METHOD(pushback_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.undo();
			Assert::AreEqual(0, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.redo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());
		}

		TEST_METHOD(popfront_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			for (int i = 0; i < 10; i++)
				myList.pop_front();
			Assert::AreEqual(0, myList.get_size());


			for (int i = 0; i < 10; i++)
				myList.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.redo();
			Assert::AreEqual(0, myList.get_size());
		}

		TEST_METHOD(popback_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			for (int i = 0; i < 10; i++)
				myList.pop_back();
			Assert::AreEqual(0, myList.get_size());


			for (int i = 0; i < 10; i++)
				myList.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.redo();
			Assert::AreEqual(0, myList.get_size());
		}

		TEST_METHOD(insert_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			for (int i = 0; i < 10; i++)
				myList.insert(i, 0);
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(0, myList.get(i));

			for (int i = 0; i < 10; i++)
				myList.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));

			for (int i = 0; i < 10; i++)
				myList.redo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(0, myList.get(i));
		}

		TEST_METHOD(erase_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			for (int i = 9; i >= 0; i--)
				myList.erase(i);
			Assert::AreEqual(0, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			for (int i = 0; i < 10; i++)
				myList.redo();
			Assert::AreEqual(0, myList.get_size());
		}

		TEST_METHOD(clear_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			myList.clear();
			Assert::AreEqual(0, myList.get_size());

			myList.undo();
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(i, myList.get(i));
			Assert::AreEqual(10, myList.get_size());

			myList.redo();
			Assert::AreEqual(0, myList.get_size());
		}

		TEST_METHOD(get_size_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			Assert::AreEqual(10, myList.get_size());
		}


		TEST_METHOD(get_test) {
			PersistentList<int> myList;

			for (int i = 0; i < 10; i++)
				myList.push_back(i);

			Assert::AreEqual(9, myList.get(9));
		}

		TEST_METHOD(into_mass_push_back_) {
			PersistentMass<int> myMass1;
			PersistentList<PersistentMass<int>> myList2;

			myList2.push_back(myMass1);
			myList2.get(0).push_back(3);
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.undo();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.redo();
			Assert::AreEqual(3, myList2.get(0).get(0));
		}

		TEST_METHOD(into_mass_pop_back_) {
			PersistentMass<int> myMass1;
			PersistentList<PersistentMass<int>> myList2;

			myList2.push_back(myMass1);
			myList2.get(0).push_back(3);
			myList2.get(0).pop_back();
			Assert::AreEqual(0, myList2.get(0).get_size());
			myList2.undo();
			Assert::AreEqual(3, myList2.get(0).get(0));
			myList2.redo();
			Assert::AreEqual(0, myList2.get(0).get_size());
		}
	};
}
