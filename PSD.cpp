#include <iostream>
#include <vector>
#include <list>
using namespace std;

template<class T>
class PersistentMass {
public:
	friend class PersistentMass;

	//передача внутренней структуре указатель на внешнюю
	template<class J>
	friend void signal_inside(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* box_mass);
	template<class J>
	friend void signal_inside(PersistentMass<J>* point_mass, J* box_mass);

	//запись действия внутренней структуры во внешний вектор действий
	void signal_outside(PersistentMass<T>* point_mass);
	void signal_outside(PersistentMass<PersistentMass<T>>* point_mass);

	template<class J>
	friend void undo_out(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* elem, int vers);
	template<class J>
	friend void undo_out(PersistentMass<J>* point_mass, J* elem, int vers);

	template<class J>
	friend void redo_out(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* elem, int vers);
	template<class J>
	friend void redo_out(PersistentMass<J>* point_mass, J* elem, int vers);

	PersistentMass();
	T& set(const int i, const T& value);
	void push_back(const T& value);
	void pop_back();
	int insert(const int i, const T& value);
	int erase(const int i);
	void clear();
	int get_size();
	T& get(int i);
	void undo();
	void redo();
private:
	/*
	0-redo 1-set 2-push_back 4-pop_back 6-insert 7-erase 8-clear 9-undo_into
	*/
	class act {
	public:
		T past_elem;		//предыдущее значение элемента
		T present_elem;		//нынешнее значение
		T* elem;			//указатель на элемент для undo для вложенности
		int index_elem;		//индекс измененного элемента
		int kind_act;		//вид изменения 1-set 
		int past_version;
		int version;		//версия которую создают
		bool undo;
		vector<T> vector_data;
	};
	PersistentMass<PersistentMass<T>>* interior_mass;//укахатель на внешний контейнер
	int version;
	int use_version;
	int size;
	int kind;							//тип контейнера
	vector<act> vector_act;				//история действий
	vector<act> vector_undo;			//история откатов
	vector<T> vector_data;				//данные
	typename vector<T>::iterator iter_b;
	int iter;

	PersistentMass<T>::act swap_data_part_undu(act box, bool undo); //передача данных для сокращения кода undo redo
};

template<class J>// передает вложенным структурам указатль на внешнюю для вызова signal_outside() у нее
void signal_inside(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* box_mass)
{
	box_mass->interior_mass = point_mass;
	box_mass->kind = 3;
}

template<class J>
void signal_inside(PersistentMass<J>* point_mass, J* box_mass) {}

template<class T>//метод для наружной структуры который дергается влож структурами для записи изменения наруж структурой
void PersistentMass<T>::signal_outside(PersistentMass<T>* point_mass)
{
	typename PersistentMass<PersistentMass<T>>::act box;
	box.past_version = interior_mass->version;
	interior_mass->use_version = box.past_version;
	interior_mass->use_version = interior_mass->version;
	box.version = interior_mass->version;
	box.elem = this;
	box.kind_act = 9;
	box.undo = 0;
	interior_mass->vector_act.push_back(box);
}

template<class T>
void PersistentMass<T>::signal_outside(PersistentMass<PersistentMass<T>>* point_mass) {}

template<class J>
void undo_out(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* elem, int vers) {
	typename PersistentMass<PersistentMass<J>>::act box_undo;
	box_undo.past_version = point_mass->use_version;
	box_undo.version = vers;
	box_undo.kind_act = 9;
	box_undo.undo = 1;
	box_undo.elem = elem;
	box_undo.elem->undo(); 
	point_mass->vector_undo.push_back(box_undo);
	point_mass->use_version = vers;
}

template<class J>
void undo_out(PersistentMass<J>* point_mass, J* elem, int vers) {}

template<class J>
void redo_out(PersistentMass<PersistentMass<J>>* point_mass, PersistentMass<J>* elem, int vers) {
	typename PersistentMass<PersistentMass<J>>::act box_redo;
	box_redo.past_version = point_mass->use_version;
	box_redo.version = vers;
	box_redo.kind_act = 9;
	box_redo.undo = 0;
	box_redo.elem = elem;
	box_redo.elem->redo();
	point_mass->vector_act.push_back(box_redo);
	point_mass->use_version = vers;
}

template<class J>
void redo_out(PersistentMass<J>* point_mass, J* elem, int vers) {}

template<class T>
PersistentMass<T>::PersistentMass()
{
	string name_type = typeid(T).name();
	if (name_type.find("class PersistentMass") != (-1)) {
		kind = 1;
	}
	else if (name_type.find("class PersistentList") != (-1)) {
		kind = 2;
	}
	else {
		kind = -1;
	}

	version = 0;
	use_version = 0;
	size = 0;
	iter_b = vector_data.begin();
	iter = 0;
}

template<class T>
T& PersistentMass<T>::set(const int i, const T& value)
{
	act box;
	box.past_version = version;
	box.past_elem = *iter_b;
	box.present_elem = value;
	box.index_elem = i;
	box.kind_act = 1;
	version++;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	vector_data[i] = value;

	if (kind == 1) {
		auto* box = &vector_data[i];
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
	return vector_data[i];
}

template<class T>
void PersistentMass<T>::push_back(const T& value)
{
	size++;
	act box;
	box.past_version = version;
	box.present_elem = value;
	box.index_elem = size - 1;
	box.kind_act = 2;
	version++;

	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	vector_data.push_back(value);
	iter_b = vector_data.begin();
	iter = 0;
	if (kind ==1) {
		T* box = &vector_data.back();
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
void PersistentMass<T>::pop_back()
{
	act box;
	box.past_version = version;
	box.past_elem = vector_data.back();
	box.index_elem = size - 1;
	box.kind_act = 4;
	version++;
	size--;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	vector_data.pop_back();

	if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
int PersistentMass<T>::insert(const int i, const T& value)
{
	iter_b = vector_data.begin();
	iter = 0;
	for (int j = 0; j < i; j++, iter_b++)
	{
	}

	act box;
	box.past_version = version;
	box.present_elem = value;
	box.index_elem = i;
	box.kind_act = 6;
	version++;
	size++;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	vector_data.insert(iter_b, value);

	if (kind == 1) {
		auto* box = &*iter_b;
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
	return i;
}

template<class T>
int PersistentMass<T>::erase(const int i)
{
	iter_b = vector_data.begin();
	for (int j = 0; j < i; j++, iter_b++)
	{
	}
	act box;
	box.past_version = version;
	box.past_elem = *iter_b;
	box.index_elem = i;
	box.kind_act = 7;
	version++;
	size--;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	vector_data.erase(iter_b);
	iter_b = vector_data.begin();
	iter = 0;
	if (kind == 3) {
		signal_outside(this);
	}
	return i;
}

template<class T>
void PersistentMass<T>::clear()
{
	act box;
	box.past_version = version;

	box.kind_act = 8;
	version++;
	size = 0;
	use_version = version;
	box.version = version;
	box.undo = 0;
	box.vector_data = vector_data;
	vector_act.push_back(box);
	vector_data.clear();
	if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
int PersistentMass<T>::get_size()
{
	return size;
}

template<class T>
T& PersistentMass<T>::get(int i) {
	return vector_data[i];
}

template<class T>
typename PersistentMass<T>::act PersistentMass<T>::swap_data_part_undu(act box, bool undo) {
	act box_undo;

	box_undo.past_version = use_version;
	use_version = box.past_version;
	box_undo.version = use_version;

	box_undo.past_elem = box.present_elem;
	box_undo.present_elem = box.past_elem;
	box_undo.index_elem = box.index_elem;
	box_undo.kind_act = box.kind_act;
	box_undo.undo = undo;
	return box_undo;
}

template<class T>
void PersistentMass<T>::undo()
{
	act box = vector_act.back();
	vector_act.pop_back();
	//определение вида действия
	switch (box.kind_act)
	{
	case 1: {//set

		vector_data[box.index_elem] = box.past_elem;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 2: {//push_back
		vector_data.pop_back();
		size--;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 4: {//pop_back
		vector_data.push_back(box.past_elem);
		size++;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 6: {//insert
		iter_b = vector_data.begin();
		iter = 0;
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		vector_data.erase(iter_b);
		size--;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 7: {//erase
		iter_b = vector_data.begin();
		iter = 0;
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		vector_data.insert(iter_b, box.past_elem);
		size++;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 8: {//clear
		act box_undo;

		box_undo.past_version = use_version;
		box_undo.version = use_version;
		box_undo.vector_data = box.vector_data;
		box_undo.kind_act = box.kind_act;
		box_undo.undo = 1;
		vector_undo.push_back(box_undo);

		use_version = box.past_version;
		vector_data = box.vector_data;
		size = vector_data.size();
		break;
	}
	case 9: {//undo_out
		undo_out(this, box.elem, box.past_version);
		break;
	}
	default:
		break;
	}
}

template<class T>
void PersistentMass<T>::redo()
{
	act box = vector_undo.back();
	vector_undo.pop_back();
	//определение вида действия
	switch (box.kind_act)
	{
	case 1: {//set
		iter_b = vector_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		*iter_b = box.past_elem;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 2: {//push_back
		vector_data.push_back(box.past_elem);
		size++;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 4: {//pop_back
		vector_data.pop_back();
		size--;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 6: {//insert
		iter_b = vector_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		vector_data.insert(iter_b, box.past_elem);
		size++;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 7: {//erase
		iter_b = vector_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		vector_data.erase(iter_b);
		size--;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 8: {//clear
		use_version = box.past_version;
		vector_data.clear();
		size = 0;

		act box_redo;
		box_redo.past_version = use_version;
		box_redo.version = use_version;
		box_redo.vector_data = box.vector_data;
		box_redo.undo = 0;
	}
	case 9: {//undo_out
		redo_out(this, box.elem, box.past_version);
		break;
	}
	default:
		break;
	}
}


template<class T>
class PersistentList {
public:
	friend class PersistentList;
	PersistentList();
	T& set(const int i, const T& value);
	void push_back(const T& value);
	void push_front(const T& value);
	void pop_back();
	void pop_front();
	int insert(const int i, const T& value);
	int erase(const int i);
	void clear();
	int get_version();
	int get_size();
	T& get(int i);
	void undo();
	void redo();
private:
	/*
	0-redo 1-set 2-push_back 3-push_front 4-pop_back, 5-pop_front 6-insert 7-erase 8-clear 9-undo_into
	*/
	class act {
	public:
		T past_elem;		//предыдущее значение элемента
		T present_elem;		//нынешнее значение
		T* elem;			//указатель на элемент для undo для вложенности
		int index_elem;		//индекс измененного элемента
		int kind_act;		//вид изменения 1-set 
		int past_version;
		int version;		//версия которую создают
		bool undo;
		list<T> list_data;
	};
	PersistentList<PersistentList<T>>* interior_list;//укахатель на внешний контейнер -- тут возможна ошибка
	int version;
	int use_version;
	int size;
	int kind;				//тип контейнера
	vector<act> vector_act;//история действий
	vector<act> vector_undo;//история откатов
	list<T> list_data;		//данные

	typename list<T>::iterator iter_b;
	int iter;

	void signal_inside(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* box_list);
	void signal_inside(PersistentList<T>* point_list, T* box_list);

	void signal_outside(PersistentList<T>* point_list);
	void signal_outside(PersistentList<PersistentList<T>>* point_list);

	void undo_out(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* elem, int vers);
	void undo_out(PersistentList<T>* point_list, T* elem, int vers);

	void redo_out(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* elem, int vers);
	void redo_out(PersistentList<T>* point_list, T* elem, int vers);

	PersistentList<T>::act swap_data_part_undu(act box, bool undo);
};

template<class T>// передает вложенным структурам указатль на внешнюю для вызова signal_outside() у нее
void PersistentList<T>::signal_inside(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* box_list)
{
	box_list->interior_list = point_list;
	box_list->nested = 3;
}

template<class T>
void PersistentList<T>::signal_inside(PersistentList<T>* point_list, T* box_list) {}

template<class T>//метод для наружной структуры который дергается влож структурами для записи изменения наруж структурой
void PersistentList<T>::signal_outside(PersistentList<T>* point_list)
{
	typename PersistentList<PersistentList<T>>::act box;
	box.past_version = interior_list->version;
	interior_list->use_version = box.past_version;
	interior_list->use_version = interior_list->version;
	box.version = interior_list->version;
	box.elem = this;
	box.kind_act = 9;
	box.undo = 0;
	interior_list->vector_act.push_back(box);
}

template<class T>
void PersistentList<T>::signal_outside(PersistentList<PersistentList<T>>* point_list) {}

template<class T>
void PersistentList<T>::undo_out(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* elem, int vers) {
	act box_undo;
	box_undo.past_version = point_list->use_version;
	box_undo.version = vers;
	box_undo.kind_act = 9;
	box_undo.undo = 1;
	box_undo.elem = elem;
	box_undo.elem.undo();
	point_list->vector_undo.push_back(box_undo);
	point_list->use_version = vers;
}

template<class T>
void PersistentList<T>::undo_out(PersistentList<T>* point_list, T* elem, int vers) {}


template<class T>
void PersistentList<T>::redo_out(PersistentList<PersistentList<T>>* point_list, PersistentList<T>* elem, int vers) {
	act box_undo;
	box_undo.past_version = point_list->use_version;
	box_undo.version = vers;
	box_undo.kind_act = 9;
	box_undo.undo = 0;
	box_undo.elem = elem;
	box_undo.elem.redo();
	point_list->vector_undo.push_back(box_undo);
	point_list->use_version = vers;
}

template<class T>
void PersistentList<T>::redo_out(PersistentList<T>* point_list, T* elem, int vers) {}



template<class T>
PersistentList<T>::PersistentList()
{
	string name_type = typeid(T).name();
	if (name_type.find("class PersistentMass") != (-1)) {
		kind = 1;
	}
	else if (name_type.find("class PersistentList") != (-1)) {
		kind = 2;
	}
	else {
		kind = -1;
	}

	version = 0;
	use_version = 0;
	size = 0;
	iter_b = list_data.begin();
	iter = 0;
}

template<class T>
T& PersistentList<T>::set(const int i, const T& value)
{
	while (iter < i) {
		iter++;
		iter_b++;
	}

	while (iter > i) {
		iter--;
		iter_b--;
	}
	act box;
	box.past_version = version;
	box.past_elem = *iter_b;
	box.present_elem = value;
	box.index_elem = i;
	box.kind_act = 1;
	version++;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	*iter_b = value;

	if (kind == 2) {
		auto* box = &*iter_b;
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}

	return *iter_b;
}

template<class T>
void PersistentList<T>::push_back(const T& value)
{
	size++;
	act box;
	box.past_version = version;
	box.present_elem = value;
	box.index_elem = size - 1;
	box.kind_act = 2;
	version++;

	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.push_back(value);
	iter_b = list_data.begin();
	iter = 0;

	if (kind == 2) {
		auto* box = &list_data.back();
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
void PersistentList<T>::push_front(const T& value)
{
	act box;
	box.past_version = version;
	box.present_elem = value;
	box.index_elem = 0;
	box.kind_act = 3;
	version++;
	size++;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.push_front(value);
	iter_b = list_data.begin();
	iter = 0;

	if (kind == 2) {
		auto* box = &list_data.front();
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
void PersistentList<T>::pop_back()
{
	act box;
	box.past_version = version;
	box.past_elem = list_data.back();
	box.index_elem = size - 1;
	box.kind_act = 4;
	version++;
	size--;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.pop_back();
	 if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
void PersistentList<T>::pop_front()
{
	act box;
	box.past_version = version;
	box.past_elem = list_data.front();
	box.kind_act = 5;
	version++;
	size--;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.pop_front();
	iter_b = list_data.begin();
	iter = 0;
	 if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
int PersistentList<T>::insert(const int i, const T& value)
{
	iter_b = list_data.begin();
	iter = 0;
	for (int j = 0; j < i; j++, iter_b++)
	{
	}

	act box;
	box.past_version = version;
	box.present_elem = value;
	box.index_elem = i;
	box.kind_act = 6;
	version++;
	size++;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.insert(iter_b, value);

	if (kind == 2) {
		auto* box = &*iter_b;
		signal_inside(this, box);
	}
	else if (kind == 3) {
		signal_outside(this);
	}
	return i;
}

template<class T>
int PersistentList<T>::erase(const int i)
{
	iter_b = list_data.begin();
	for (int j = 0; j < i; j++, iter_b++)
	{
	}
	act box;
	box.past_version = version;
	box.past_elem = *iter_b;
	box.index_elem = i;
	box.kind_act = 7;
	version++;
	size--;
	use_version = version;
	box.version = version;
	box.undo = 0;
	vector_act.push_back(box);
	list_data.erase(iter_b);
	iter_b = list_data.begin();
	iter = 0;

	 if (kind == 3) {
		signal_outside(this);
	}
	return i;
}

template<class T>
void PersistentList<T>::clear()
{
	act box;
	box.past_version = version;

	box.kind_act = 8;
	version++;
	size = 0;
	use_version = version;
	box.version = version;
	box.undo = 0;
	box.list_data = list_data;
	vector_act.push_back(box);
	list_data.clear();
	if (kind == 3) {
		signal_outside(this);
	}
}

template<class T>
int PersistentList<T>::get_version()
{
	return version;
}

template<class T>
int PersistentList<T>::get_size()
{
	return size;
}

template<class T>
T& PersistentList<T>::get(int i) {
	typename list<T>::iterator it;
	it = list_data.begin();
	iter = 0;
	for (int j = 0; j < i; j++, it++)
	{
	}
	return *it;
}

template<class T>
typename PersistentList<T>::act PersistentList<T>::swap_data_part_undu(act box, bool undo) {
	act box_undo;

	box_undo.past_version = use_version;
	use_version = box.past_version;
	box_undo.version = use_version;

	box_undo.past_elem = box.present_elem;
	box_undo.present_elem = box.past_elem;
	box_undo.index_elem = box.index_elem;
	box_undo.kind_act = box.kind_act;
	box_undo.undo = undo;
	return box_undo;
}

template<class T>
void PersistentList<T>::undo()
{
	act box = vector_act.back();
	vector_act.pop_back();
	//определение вида действия
	switch (box.kind_act)
	{
	case 1: {//set

		while (iter < box.index_elem) {
			iter++;
			iter_b++;
		}

		while (iter > box.index_elem) {
			iter--;
			iter_b--;
		}
		*iter_b = box.past_elem;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 2: {//push_back
		list_data.pop_back();
		size--;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 3: {//push_front
		list_data.pop_front();
		size--;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		iter_b = list_data.begin();
		iter = 0;
		break;
	}
	case 4: {//pop_back
		list_data.push_back(box.past_elem);
		size++;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 5: {//pop_front
		list_data.push_front(box.past_elem);
		size++;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		iter_b = list_data.begin();
		iter = 0;
		break;
	}
	case 6: {//insert
		iter_b = list_data.begin();
		iter = 0;
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		list_data.erase(iter_b);
		size--;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 7: {//erase
		iter_b = list_data.begin();
		iter = 0;
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		list_data.insert(iter_b, box.past_elem);
		size++;

		act box_undo = swap_data_part_undu(box, 1);
		vector_undo.push_back(box_undo);
		break;
	}
	case 8: {//clear
		act box_undo;

		box_undo.past_version = use_version;
		box_undo.version = use_version;
		box_undo.list_data = box.list_data;
		box_undo.kind_act = box.kind_act;
		box_undo.undo = 1;
		vector_undo.push_back(box_undo);

		use_version = box.past_version;
		list_data = box.list_data;
		size = list_data.size();
		break;
	}
	case 9: {//undo_out
		undo_out(this, box.elem, box.past_version);
		break;
	}
	default:
		break;
	}
}

template<class T>
void PersistentList<T>::redo()
{
	act box = vector_undo.back();
	vector_undo.pop_back();
	//определение вида действия
	switch (box.kind_act)
	{
	case 1: {//set
		iter_b = list_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		*iter_b = box.past_elem;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 2: {//push_back
		list_data.push_back(box.past_elem);
		size++;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 3: {//push_front
		list_data.push_front(box.past_elem);
		size++;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 4: {//pop_back
		list_data.pop_back();
		size--;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 5: {//pop_front
		list_data.pop_front();
		size--;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 6: {//insert
		iter_b = list_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		list_data.insert(iter_b, box.past_elem);
		size++;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 7: {//erase
		iter_b = list_data.begin();
		for (int j = 0; j < box.index_elem; j++, iter_b++)
		{
		}
		list_data.erase(iter_b);
		size--;

		act box_redo = swap_data_part_undu(box, 0);
		vector_act.push_back(box_redo);
		break;
	}
	case 8: {//clear
		use_version = box.past_version;
		list_data.clear();
		size = 0;

		act box_redo;
		box_redo.past_version = use_version;
		box_redo.version = use_version;
		box_redo.list_data = box.list_data;
		box_redo.undo = 0;
	}
	case 9: {//undo_out
		redo_out(this, box.elem, box.past_version);
		break;
	}
	default:
		break;
	}


}




int main()
{
	//int num = 5;
	//transactions<int> tr;

	//tr.run(&num);
	//cout << tr.get() << endl;
	//tr.get() = 15;
	//num = 10;
	//cout << tr.get() << endl;
	PersistentMass<int> myList1;
	PersistentMass<PersistentMass<int>> myList2;
	myList2.push_back(myList1);
	myList2.get(0).push_back(2);
	cout << myList2.get(0).get(0) << endl;
	myList2.undo();
	myList2.redo();
	return 0;
}

