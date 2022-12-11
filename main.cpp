#include "lib.h"
#include "file_funcs.h"
#include "print.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>



int main (int, char **) {
    std::cout << "Version: " << version() << std::endl;
    std::cout << "Hello, IP Filter!" << std::endl;

    try
    {
        // ¬ектор с векторами строк, полученный после чтени€ файла.
        ip_vector_strings ip_list_from_file;

        size_t ip_pool_size = 0; //  ол-во записей-адресов в файле.
        
        // „тение списка ip адресов из тестового файла.
        ip_pool_size = get_ip_list_from_file(ip_list_from_file, std::string{ "ip_filter.tsv" });
        if ( 0 == ip_pool_size) {
            return EXIT_FAILURE;
        }
 
        ip_vector_arrays ip_pool; // 
        ip_pool.reserve(ip_pool_size);

        // ѕреобразование вектора векторов строк в вектор массивов константного размера.
        if (!ip_list_transform(ip_list_from_file, ip_pool, ip_pool_size)) {
            return EXIT_FAILURE;
        }

        // ¬ывод в консоль оригинального списка.
        if (ask_to_print("\nWould you like to see original ip list?")) {
            print_ip_vector(ip_pool);
        }
        
        
        // TODO reverse lexicographically sort
        reverse_sort(ip_pool);
        
        if (ask_to_print("\nWould you like to write reversed ip list to file?")) {
            write_ip_list_to_file(ip_pool, std::string{ "reversed_list.txt" });
        }
        
        // ¬ывод в консоль списка, отсортированного в обратном пор€дке.
        if (ask_to_print("\nWould you like to see reversed ip list?")) {
            print_ip_vector(ip_pool);
        }

        
        // ¬ывод в консоль отсортированного списка, но только с 1 в первой октете.
        if (ask_to_print("\nWould you like to see filtered by first byte ip list?")) {
            filter_output(ip_pool, std::vector<int> {1});
        }

        // ¬ывод в консоль отсортированного списка, но только со значением 46 в первой октете и 70 во второй.
        if (ask_to_print("\nWould you like to see filtered by first and second bytes ip list?")) {
            filter_output(ip_pool, std::vector<int> {46, 70});
        }

        // ¬ывод в консоль отсортированного списка, но только со значением 46 в любой октете.
        if (ask_to_print("\nWould you like to see filtered by any byte ip list?")) {
            filter_output_by_any_octet(ip_pool, 46);
        }
        
        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
