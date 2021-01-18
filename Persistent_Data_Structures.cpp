#include <iostream>
#include <vector>
using namespace std;




//класс персистентный массив
template <class T>
class PersistMass
{
public:
	//структура хранение версий. т.е. тип данных узла
	class FatNodes_elem
	{
	public:
		T value;
		int version = 0;
	};

	PersistMass();
	PersistMass(int n);							//конструктор с одним параметром
	void set(int i, T value);					//ввод элемента
	T get(int i);								//вывод элемента последней версии 
	T get(int i, int version);					//вывод элемента выбранной версии 
	int getSize();								//получить размер массива
	void push_back(T value);					//добавить элемент в массив
	void pop_back();							//добавить элемент в массив
	void empty();								//опустошить массив
	T& operator[](const int i);					//перегрузка оператора[]
	
protected:
	int version;								//последняя версия структуры, счетчик
	int kind;
	int maxSize;								//максимальный размер массива (для реализации функций которые манипулируют размером массива)
	int SizePersistMass;							//размер массива этой версии 
	vector<int> vectroSizePersistMass;			//размер массива всех версий
	vector <vector <FatNodes_elem>> vectorFatNode;//хранение данных. внутренний вектр это толстый узел
};

//конструктор без параметров
template <class T>
PersistMass<T>::PersistMass()
{
	version = 0;
	SizePersistMass = 0;
	vectroSizePersistMass.push_back(SizePersistMass);
	maxSize = 0;
	vectorFatNode.resize(0);
	//vectorFatNode;
}

//конструктор с одним параметром
//template <class T>
//PersistMass<PersistMass<T>>::PersistMass(int n)
//{
//
//}
//конструктор с одним параметром
template <class T>
PersistMass<T>::PersistMass(int n)
{
	string name_type = typeid(T).name();
	if (name_type.find("class PersistentMass") != (-1)) {
		kind = 1;
	}
	else {
		kind = -1;
	}
	version = 0;
	SizePersistMass = n;
	vectroSizePersistMass.push_back(n);
	maxSize = n;
	vectorFatNode.resize(n);

	if (kind == 1) {
		FatNodes_elem box;
		for (int i = 0; i < n; i++) {
			vectorFatNode[i].push_back(box);
		}
	}
	else {
		FatNodes_elem box;
		for (int i = 0; i < n; i++) {
			box.version = 0;
			box.value = {};
			vectorFatNode[i].push_back(box);
		}
	}
}

//ввод элемента
template <class T>
void PersistMass<T>::set(int i, T value)
{

	if (i < 0 || i >= SizePersistMass)
		exit(1);
	version++;
	//расширяем узел новым значением
	FatNodes_elem box;
	box.value = value;
	box.version = version;

	vectorFatNode[i].push_back(box);
}

//вывод элемента последней версии 
template <class T>
T PersistMass<T>::get(int i)
{
	if (i < 0 || i >= SizePersistMass)
		exit(1);
	return vectorFatNode[i].back().value;

}

//вывод элемента выбранной версии 
template <class T>
T PersistMass<T>::get(int i, int inVersion)
{
	//запрашиваемая версия не может быть больше чем послед.верс. массива
	if (inVersion > version) {
		exit(1);
	}

	int size = vectorFatNode[i].size() - 1;
	//как только inVersion больше чем 
	while (inVersion < vectorFatNode[i][size].version) {
		size--;
	}
	return vectorFatNode[i][size].value;
}

//получить размер массива
template <class T>
int PersistMass<T>::getSize() {
	return SizePersistMass;
}

//перегрузка оператора[]
template <class T>
T& PersistMass<T>::operator[](const int i)
{
	if (i < 0 || i >= SizePersistMass)
		exit(1);

	return vectorFatNode[i].back().value;
}


template <class T>
void PersistMass<T>::push_back(T value) 
{
	if (SizePersistMass == maxSize) {//проверка на размер массива (было ли последним изменением - удаление)
		SizePersistMass++;
		vectroSizePersistMass.push_back(SizePersistMass);
		maxSize++;
		version++;
		FatNodes_elem box;
		vector <FatNodes_elem> vect_box;
		box.version = version;
		box.value = value;
		vect_box.push_back(box);
		vectorFatNode.push_back(vect_box);
	}
	else {
		version++;
		//расширяем узел новым значением
		FatNodes_elem box;
		box.value = value;
		box.version = version;
		vectorFatNode[SizePersistMass].push_back(box);
		SizePersistMass++;
		vectroSizePersistMass.push_back(SizePersistMass);
	}
	
}

template<class T>
void PersistMass<T>::pop_back()
{
	SizePersistMass--;
	version++;
	vectroSizePersistMass.push_back(SizePersistMass);
}

template<class T>
void PersistMass<T>::empty()
{
	SizePersistMass = 0;
	version++;
	vectroSizePersistMass.push_back(SizePersistMass);
}



int main()
{

	PersistMass<int> mass1;
	mass1.push_back(1);
	mass1.push_back(2);
	mass1.push_back(3);

	mass1.set(0, 10);
	mass1.set(1, 20);
	mass1.set(2, 30);

	cout << "version 0" << endl;
	cout <<"size: "<< mass1.getSize()<<"\n" << endl;

	cout << "version 1" << endl;
	cout << "size: " << mass1.getSize() << endl;
	cout <<"1 elem: "<< mass1.get(0, 1) << "\n" << endl;

	cout << "version 2" << endl;
	cout << "size: " << mass1.getSize() << endl;
	cout << "1 elem: " << mass1.get(0, 2) << endl;
	cout << "2 elem: " << mass1.get(1, 2) << "\n" << endl;
	
	cout << "version 3" << endl;
	cout << "size: " << mass1.getSize() << endl;
	cout << "1 elem: " << mass1.get(0, 3) << endl;
	cout << "2 elem: " << mass1.get(1, 3) << endl;
	cout << "3 elem: " << mass1.get(2, 3) << "\n" << endl;

	cout << "last version" << endl;
	cout << "1 elem: " << mass1.get(0) << endl;
	cout << "2 elem: " << mass1.get(1) << endl;
	cout << "3 elem: " << mass1.get(2) << "\n" << endl;


	typename PersistMass<PersistMass<int>> mass2(3);
	mass2.push_back(mass1);
	return 0;
}

