#pragma once

#include <cstddef>
#include <cstdlib> 
#include <new>
#include <memory>
#include <utility>

#include <iostream>

const constexpr int default_limit = 10;

template<typename T>
struct Chunk {
    void *memory_ptr = nullptr; // ��������� �� ���������� ������� ������.
    void *end_ptr = nullptr; // ��������� �� ������ ����� ����� �������.
    
    std::size_t size = 0; // ������ � �������� ���� ����, ������� ������.
    std::size_t occupied = 0; // ������ ������.
    std::size_t released = 0; // ������ �����������.

    Chunk<T> *next = nullptr; // ��������� �� ��������� ��������� ������ ����.

    Chunk() : memory_ptr{ nullptr }, end_ptr{nullptr}, size { 0 }, occupied{ 0 }, released{ 0 } {}

    // �������������� ������ ��� ������.
    void* allocate_space(const std::size_t& items_number) {
        memory_ptr = std::malloc(items_number * sizeof(T));

        if (memory_ptr == nullptr) {
            return nullptr;
        }
        
        end_ptr = memory_ptr;
        size = items_number;

        return memory_ptr;
    }

    // ������������ ������, ���������� ��� ������.
    void free_memory() {
        std::free(memory_ptr);
        memory_ptr = nullptr;
        std::cout << " - Chunk FREE all memory" << std::endl;
    }

    // ������ ������ ��� ����������� ���-�� ��������� � ������ ������� ��������.
    void* occupy_memory(const std::size_t &n) {
        void* p = end_ptr;
        end_ptr = ((char*)end_ptr) + n * sizeof(T);
        occupied += n;
        std::cout << " - Chunk: " << memory_ptr << " - Occupied: " << occupied << std::endl;
        return p;
    }

    // ���������� ������ �� ����������� ���-�� ���������.
    bool deoccupy_memory(void* p, const std::size_t& n) {
        if (!contains_pointer(p)) {
            return false;
        }

        released += n;
        std::cout << " - Chunk: " << memory_ptr << " - Released: " << released << std::endl;
        if (released == occupied) { // �� ���� �� ����� ������ ������ �� ������������.
            free_memory();
        }

        return true;
    }

    bool is_free() {
        return (memory_ptr == nullptr);
    }

    // ������, ���� �� ��������� ����� ��� ���������� ���-�� ���������.
    bool enough_space(const std::size_t& n) {
        return ((size - occupied) >= n);
    }

    // ������, ��������� �� ���������� ��������� � ���������� ����� ������� ������.
    bool contains_pointer(void* p) {
        void *border_ptr = ((char*)memory_ptr) + size * sizeof(T);
        return ((p >= memory_ptr) && (p < border_ptr));
    }
};

template<typename T, size_t N = default_limit>
class CustomAllocator {
public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = CustomAllocator<U, N>;
    };

    //CustomAllocator() = default;
    //~CustomAllocator() = default;

    CustomAllocator() {
        std::cout << "Allocator default constructor. Allocate at least: " << N << " Item size: " << sizeof(T) << std::endl;
        items_per_chunk = N;
    }

    template<typename U, size_t S>
    CustomAllocator(const CustomAllocator<U, S>&)  { }

    ~CustomAllocator() {
        std::cout << "Allocator destructor" << std::endl;
        // ����������� ��� ������� ������.
        Chunk<T>* tmp_chunk = nullptr;
        last_allocator_chunk = first_allocator_chunk;
        
        while (last_allocator_chunk != nullptr) {
            tmp_chunk = last_allocator_chunk->next;
            std::cout << " - FREE mem: " << last_allocator_chunk->memory_ptr << std::endl;
            last_allocator_chunk->free_memory();
            delete last_allocator_chunk;
            last_allocator_chunk = tmp_chunk;
        }
    }
        

    // ��������� ������.
    pointer allocate(const std::size_t n) {
        // �������, ���� �� ����� ��������������� ������.
        if (first_allocator_chunk == nullptr) {
            std::size_t items_number = (n > items_per_chunk) ? n : items_per_chunk; // ���� ����������� ���-��, ���� ������.

            first_allocator_chunk = allocate_new_chunk(items_number);
            if (first_allocator_chunk == nullptr) {
                throw std::bad_alloc();
            }

            last_allocator_chunk = first_allocator_chunk;
            alloc_requests_number++; // ����� ���-�� �������� �� ���������.
            std::cout << "Allocate first chunk. Required: " << n << " Allocated: " << items_number << std::endl;
            return reinterpret_cast<T*>( last_allocator_chunk->occupy_memory(n) );
        }

        // �������, ���������� �� ���������� ����� ��� ����������� ������ � ������� ����������������� �������.
        if (last_allocator_chunk->enough_space(n)) {
            alloc_requests_number++; // ����� ���-�� �������� �� ���������.
            return reinterpret_cast<T*>(last_allocator_chunk->occupy_memory(n));
        }
        
        // ���� � ������� �� �������, ������� ���� �� ��������� ����� � ����������.
        Chunk<T>* tmp_chunk = first_allocator_chunk;
        while (tmp_chunk != last_allocator_chunk) {
            if (tmp_chunk->enough_space(n)) {
                alloc_requests_number++; // ����� ���-�� �������� �� ���������.
                return reinterpret_cast<T*>(tmp_chunk->occupy_memory(n));
            }
            tmp_chunk = tmp_chunk->next;
        }

        // ����� ������������� ��� ���� ������� ������.
        std::size_t items_number = (n > items_per_chunk) ? n : items_per_chunk; // ���� ����������� ���-��, ���� ������.
        last_allocator_chunk->next = allocate_new_chunk(items_number);
        if (last_allocator_chunk->next == nullptr) {
            throw std::bad_alloc();
        }

        last_allocator_chunk = last_allocator_chunk->next; // ������ ������� ������� ��������� � ������������.
        alloc_requests_number++; // ����� ���-�� �������� �� ���������.
        std::cout << "Allocate next chunk. Required: " << n << " Allocated: " << items_number << std::endl;
        return reinterpret_cast<T*>(last_allocator_chunk->occupy_memory(n));

        //std::cout << "Allocate space for: " << n << " items. Address: " << p << " size " << sizeof(T) << " " << typeid(T).name() << std::endl;
    }
    
    // ������������ ������.
    void deallocate(pointer p, const std::size_t& n ) {
        if (first_allocator_chunk == nullptr) {
            std::cout << "Unexpected deallocte error!" << std::endl;
            return;
        }

        // ���� Chunk, ������ �������� ����� ���������� ������.
        Chunk<T>* curr_chunk = first_allocator_chunk;
        Chunk<T>* next_chunk = nullptr;
        Chunk<T>* prev_chunk = nullptr;

        while (curr_chunk != nullptr) {
            next_chunk = curr_chunk->next;
            if (curr_chunk->deoccupy_memory(p ,n)) {
                break;
            }
            prev_chunk = curr_chunk;
            curr_chunk = curr_chunk->next;
        }

        if (curr_chunk->is_free() ) {
            if (prev_chunk == nullptr) {
                first_allocator_chunk = next_chunk;
            }
            else {
                prev_chunk->next = next_chunk;
            }

            if (last_allocator_chunk == curr_chunk) {
                last_allocator_chunk = prev_chunk;
            }

            if (first_allocator_chunk == curr_chunk) {
                first_allocator_chunk = next_chunk;
            }

            delete curr_chunk;
        }

        --alloc_requests_number;
        std::cout << "Deallocate space. Required: " << n << " Requests left: " << alloc_requests_number << " Address: " << p << std::endl;
    }
    
    // �������� ������� � ���������� ���������.
    template <typename U, typename... Args>
    void construct(U* p, Args &&...args) {
        std::cout << "Construct address: " << p << " size " << sizeof(U) << " " << typeid(U).name() << std::endl;
        
        new (p) U(std::forward<Args>(args)...);
    }

    // ����������� ������� � ���������� ���������.
    template <typename U>
    void destroy(pointer p) {
        std::cout << "Destroy: " << std::endl;
        p->~U();
    }

    std::size_t max_size() const throw() {
        return size_t(-1) / sizeof(T);
    }

    pointer address(reference __x) const { return &__x; }

    const_pointer address(const_reference __x) const { return &__x; }

private:
    std::size_t items_per_chunk; // ���-�� ���������� �� ��� ���������, ���� �� ������ ������.
    
    Chunk<T> *first_allocator_chunk = nullptr; // ������ ������� ������ ��� ���������.
    Chunk<T> *last_allocator_chunk = nullptr;  // ���������.

    std::size_t alloc_requests_number = 0; // ���-�� ���������� �������� �� ��������� ������.

    Chunk<T>* allocate_new_chunk(const size_t& items_number); // ����� ��������� ����� �������.
};

template <class T, size_t N, class U, size_t S>
constexpr bool operator== (const CustomAllocator<T, N>&, const CustomAllocator<U, S>&) noexcept {
    return true;
}

template <class T, size_t N, class U, size_t S>
constexpr bool operator!= (const CustomAllocator<T, N>& x, const CustomAllocator<U, S>& y) noexcept {
    return !(x == y);
}

// ��������� ������ ����� ������
template<typename T, size_t N>
Chunk<T>* CustomAllocator<T, N>::allocate_new_chunk(const size_t& items_number)
{
    //Chunk<T>* chunk_ptr = reinterpret_cast<Chunk<T>*>(std::malloc(sizeof(Chunk<T>)));
    Chunk<T>* chunk_ptr = reinterpret_cast<Chunk<T>*>(new Chunk<T>);
    if (!chunk_ptr) {
        return nullptr;
    }
    
    if ( nullptr == chunk_ptr->allocate_space(items_number)) {
        std::free( reinterpret_cast<void*>(chunk_ptr) );
        return nullptr;
    }

    return chunk_ptr;
}
