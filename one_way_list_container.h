#pragma once

// Контейнер, реализующий однонаправленный список.
template <class T, class Allocator = std::allocator<T>>
class OneWayList {
private:
	struct OneWayNode {
		OneWayNode() : next_node{ nullptr } {}
		explicit OneWayNode(const T& value) : next_node{ nullptr }, data{ value } {}

		OneWayNode* next_node;
		T data;
	};
	
	using AllocatorTraits = std::allocator_traits<Allocator>;
	using node_alloc_t = typename std::allocator_traits<Allocator>::template rebind_alloc<OneWayNode>;
	
	//Allocator& custom_allocator;
	node_alloc_t node_alloc;

public:
	struct Iterator {
		Iterator() : one_way_node_ptr{ nullptr } {}

		explicit Iterator(OneWayNode* own_ptr) : one_way_node_ptr{ own_ptr } {}

		OneWayNode* operator++() {
			one_way_node_ptr = one_way_node_ptr->next_node;
			return one_way_node_ptr;
		}

		T get() {
			return one_way_node_ptr->data;
		}

		bool operator==(const Iterator& another_iter) {
			return (one_way_node_ptr == another_iter.one_way_node_ptr);
		}

		bool operator!=(const Iterator& another_iter) {
			return !(one_way_node_ptr == another_iter.one_way_node_ptr);
		}

	private:
		OneWayNode* one_way_node_ptr;
	};
	
	// Конструктор по умолчанию.
	OneWayList() : first_node_ptr{ nullptr }, last_node_ptr{ nullptr }, total_number{ 0 } {
		std::cout << "One Way List default constructor" << std::endl;
	}

	// Копирующий конструктор.
	OneWayList(const OneWayList& owl_cont) : OneWayList() {
		std::cout << "One Way List copy constructor" << std::endl;

		if (owl_cont.first_node_ptr != nullptr) {

			// В цикле создаем копии каждого узла другого объекта и строим цепочку.
			OneWayNode* own_ptr_to_copy = owl_cont.first_node_ptr;
			OneWayNode* own_ptr_new = nullptr;

			while (own_ptr_to_copy != nullptr) {
				own_ptr_new = node_alloc.allocate(1);
				node_alloc.construct(own_ptr_new, own_ptr_to_copy->data);

				if (first_node_ptr == nullptr) { // Только начали.
					first_node_ptr = own_ptr_new;
					last_node_ptr = own_ptr_new;
				}
				else { // Подключаем к уже созданным.
					last_node_ptr->next_node = own_ptr_new;
					last_node_ptr = own_ptr_new;
				}

				own_ptr_to_copy = own_ptr_to_copy->next_node;
			}

			total_number = owl_cont.total_number;
		}
	}

	// Перемещающий конструктор.
	OneWayList(OneWayList&& owl_cont) : OneWayList()
	{
		std::cout << "One Way List move constructor" << std::endl;

		std::swap(first_node_ptr, owl_cont.first_node_ptr);
		std::swap(last_node_ptr, owl_cont.last_node_ptr);
		std::swap(total_number, owl_cont.total_number);
	}

	// Оператор копирующего присваивания.
	OneWayList& operator=(const OneWayList& owl_cont)
	{
		std::cout << "One Way List copy operator" << std::endl;

		if ((this != &owl_cont) && (owl_cont.first_node_ptr != nullptr)) {
			clear(); // Полная очистка своего контейнера.

			// В цикле создаем копии каждого узла другого объекта.
			OneWayNode* own_ptr_to_copy = owl_cont.first_node_ptr;
			OneWayNode* own_ptr_new = nullptr;

			while (own_ptr_to_copy != nullptr) {
				own_ptr_new = node_alloc.allocate(1);
				node_alloc.construct(own_ptr_new, own_ptr_to_copy->data);

				if (first_node_ptr == nullptr) {
					first_node_ptr = own_ptr_new;
					last_node_ptr = own_ptr_new;
				}
				else {
					last_node_ptr->next_node = own_ptr_new;
					last_node_ptr = own_ptr_new;
				}

				own_ptr_to_copy = own_ptr_to_copy->next_node;
			}

			total_number = owl_cont.total_number;
		}

		return *this;
	}

	// Перемещающее присваивание.
	OneWayList& operator=(OneWayList&& owl_cont) noexcept
	{
		std::cout << "One Way List move operator" << std::endl;

		if ((this != &owl_cont) && (owl_cont.first_node_ptr != nullptr)) {
			clear(); // Полная очистка своего контейнера.

			OneWayList temp{ std::move(owl_cont) };
			std::swap(first_node_ptr, temp.first_node_ptr);
			std::swap(last_node_ptr, temp.last_node_ptr);
			std::swap(total_number, temp.total_number);
		}

		return *this;
	}

	// Получение доступа по индексу. Выход за пределы не проверяется.
	T& operator[](const size_t& place) {
		OneWayNode* current_node_ptr = get_node_by_place(place);

		return current_node_ptr->data;
	}

	// Константный метод. Получение доступа по индексу. Выход за пределы не проверяется.
	const T& operator[](const size_t& place) const {
		OneWayNode* current_node_ptr = get_node_by_place(place);

		return current_node_ptr->data;
	}

	// Деструктор
	~OneWayList() {
		std::cout << "One Way List destructor" << std::endl;

		// Обход всех узлов от первого до последнего.
		clear();

	}

	// Очистка всего контейнера.
	void clear() {
		if (first_node_ptr == nullptr) {
			return;
		}

		OneWayNode* current_node_ptr = first_node_ptr;
		OneWayNode* next_to_delete_node_ptr = nullptr;

		// Удаляем все узлы в порядке от последнего к первому.
		while (current_node_ptr != nullptr) {
			next_to_delete_node_ptr = current_node_ptr->next_node;
			node_alloc.deallocate(current_node_ptr, 1);
			current_node_ptr = next_to_delete_node_ptr;
		}

		first_node_ptr = nullptr;
		last_node_ptr = nullptr;
		total_number = 0;
	}

	// Узнать количество элементов в контейнере.
	size_t get_number() const { return total_number; }

	// Добавить новый элемент в конец контейнера.
	void push_back(const T& value) {
		OneWayNode* own_ptr = node_alloc.allocate(1);
		node_alloc.construct(own_ptr, value);
		
		if (first_node_ptr == nullptr) {
			first_node_ptr = own_ptr;
			last_node_ptr = own_ptr;
		}
		else {
			last_node_ptr->next_node = own_ptr;
			last_node_ptr = own_ptr;
		}
		
		++total_number;
	}

	// Вставить новый элемент в контейнер в указанную позицию.
	bool insert(const size_t& place, const T& value) {
		if (place > total_number) return false;

		if (place == total_number) { // Добавление в конец.
			push_back(value);
			return true;
		}
	
		OneWayNode* own_ptr = node_alloc.allocate(1);
		node_alloc.construct(own_ptr, value);

		OneWayNode* current_node_ptr = first_node_ptr;
		OneWayNode* prev_node_ptr = nullptr;

		size_t i = 0;
		while (i != place) {
			prev_node_ptr = current_node_ptr;
			current_node_ptr = current_node_ptr->next_node;
			++i;
		}

		if (current_node_ptr != nullptr) {
			own_ptr->next_node = current_node_ptr;
		}
		else {
			return false;
		}

		if (prev_node_ptr == nullptr) {
			first_node_ptr = own_ptr;
		}
		else {
			prev_node_ptr->next_node = own_ptr;
		}

		++total_number;

		return true;
	}

	// Удалить элемент контейнера из указанной позиции.
	bool erase(const size_t& place) {
		if (place >= total_number) return false; // Выход за пределы.
		if (first_node_ptr == nullptr) return false;

		OneWayNode* current_node_ptr = first_node_ptr;
		OneWayNode* prev_node_ptr = nullptr;
		OneWayNode* next_node_ptr = current_node_ptr->next_node;

		size_t i = 0;
		while (i != place) {
			prev_node_ptr = current_node_ptr;
			current_node_ptr = current_node_ptr->next_node;

			if (current_node_ptr == nullptr) {
				return false; // Такой ситуации не должно быть.
			}

			next_node_ptr = current_node_ptr->next_node;
			++i;
		}

		if (current_node_ptr == first_node_ptr) {
			if (first_node_ptr == last_node_ptr) { // Удаляется единственный элемент.
				first_node_ptr = nullptr;
				last_node_ptr = nullptr;
			}
			else {
				first_node_ptr = next_node_ptr;
			}
		}
		else {
			if (current_node_ptr == last_node_ptr) {
				prev_node_ptr->next_node = nullptr;
				last_node_ptr = prev_node_ptr;
			}
			else {
				prev_node_ptr->next_node = next_node_ptr;
			}
		}

		node_alloc.deallocate(current_node_ptr, 1);

		--total_number;

		return true;
	}

	Iterator begin() const {
		return Iterator(first_node_ptr);
	}

	Iterator last_valid() const {
		if (last_node_ptr == nullptr) return Iterator(nullptr);
		return Iterator(last_node_ptr);
	}

	Iterator end() const {
		if (last_node_ptr == nullptr) return Iterator(nullptr);
		return Iterator(last_node_ptr->next_node);
	}

private: // Функции для внутреннего использования.

	// Получить указатель на узел по номеру места в контейнере.
	OneWayNode* get_node_by_place(const size_t& place) const {
		if (first_node_ptr == nullptr) {
			return nullptr;
		}

		OneWayNode* current_node_ptr = first_node_ptr;

		size_t i = 0;
		while (i != place) {
			current_node_ptr = current_node_ptr->next_node;

			if (current_node_ptr == nullptr) {
				return nullptr;
			}

			++i;
		}

		return current_node_ptr;
	}


private:
	OneWayNode* first_node_ptr; // Указатель на первый узел.
	OneWayNode* last_node_ptr;  // Указатель на последний узел.

	size_t total_number;

	Iterator owl_iterator;

};

// Вывод всех элементов контейнера в консоль.
template <class T, class Allocator>
std::ostream& operator<<(std::ostream& os, const OneWayList<T, Allocator>& lcont) {
	for (auto iter = lcont.begin(); iter != lcont.end(); ++iter) {
		os << iter.get();
		if (iter != lcont.last_valid()) { os << ", "; }
	}

	return os;
}

