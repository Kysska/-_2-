#include <iostream>
#include <vector>
#include<map>

// размер вектора пар
int size;

//Глобальный вектор пар для отслеживания всех свободных узлов различных размеров
std::vector<std::pair<int, int>> arr[100000];

// Map для хранения начального адреса в качестве ключа и размера выделенной ячейки в качестве значения
std::map<int, int> mp;

void Buddy(int s)
{

    // максимальный размер степени двойки ячейки
    int n = ceil(log(s) / log(2));

    size = n + 1;
    for (int i = 0; i <= n; i++)
        arr[i].clear();

    // Изначально доступен весь блок указанного размера
    arr[n].push_back(std::make_pair(0, s - 1));
}

void allocate(int s)
{

    // Вычисление индекса в свободном списке для поиска блока, если таковой имеется
    int x = ceil(log(s) / log(2));
    // Доступный блок 
    if (arr[x].size() > 0)
    {
        std::pair<int, int> temp = arr[x][0];

        // Удаляем блок из списка свободных блоков
        arr[x].erase(arr[x].begin());

        std::cout << "Memory from " << temp.first
            << " to " << temp.second
            << " allocated" << "\n";

        // Сопоставьте начальный адрес с размером, чтобы упростить освобождение
        mp[temp.first] = temp.second -
            temp.first + 1;
    }
    else
    {
        int i;
        std::cout << size;

        // Если нет, найдите блок большего размера 
        for (i = x + 1; i < size; i++)
        {

            // Найти размер блока, превышающий запрос 
            if (arr[i].size() != 0)
                break;
        }

        //Если такой блок не найден, т.е. нет доступного блока памяти
        if (i == size)
        {
            std::cout << "Sorry, failed to allocate memory\n";
        }

        // Если нашел
        else
        {
            std::pair<int, int> temp;
            temp = arr[i][0];

            // Удалите первый блок, чтобы разделить его на половинки
            arr[i].erase(arr[i].begin());
            i--;

            for (; i >= x; i--)
            {

                // Разделяем блок на две полокинки
                std::pair<int, int> pair1, pair2;
                pair1 = std::make_pair(temp.first,
                    temp.first +
                    (temp.second -
                        temp.first) / 2);
                pair2 = std::make_pair(temp.first +
                    (temp.second -
                        temp.first + 1) / 2,
                    temp.second);

                arr[i].push_back(pair1);

                // Добавляем в список свободных блоков
                arr[i].push_back(pair2);
                temp = arr[i][0];

                //Удалите первый свободный блок для аллоцирования   
                arr[i].erase(arr[i].begin());
            }

            std::cout << "Memory from " << temp.first
                << " to " << temp.second
                << " allocate" << "\n";

            mp[temp.first] = temp.second -
                temp.first + 1;
        }
    }
}

void deallocate(int id)
{

    // Если такой начальный адрес недоступен
    if (mp.find(id) == mp.end())
    {
        std::cout << "invalid free request\n";
        return;
    }

    // Размер блока для поиска
    int n = ceil(log(mp[id]) / log(2));

    int i, buddyNumber, buddyAddress;

    // Добавьте блок в список свободных
    arr[n].push_back(std::make_pair(id,
        id + pow(2, n) - 1));
    std::cout << "Memory block from " << id
        << " to " << id + pow(2, n) - 1
        << " freed\n";

    // Вычислить номер приятеля
    buddyNumber = id / mp[id];

    if (buddyNumber % 2 != 0)
        buddyAddress = id - pow(2, n);
    else
        buddyAddress = id + pow(2, n);

    // Найдите в свободном списке, чтобы найти его приятеля
    for (i = 0; i < arr[n].size(); i++)
    {

        // Если приятель найден и тоже свободен
        if (arr[n][i].first == buddyAddress)
        {

            // объединяем друзей, чтобы сделать их одним большим блоком свободной памяти
            if (buddyNumber % 2 == 0)
            {
                arr[n + 1].push_back(std::make_pair(id,
                    id + 2 * (pow(2, n) - 1)));

                std::cout << "Coalescing of blocks starting at "
                    << id << " and " << buddyAddress
                    << " was done" << "\n";
            }
            else
            {
                arr[n + 1].push_back(std::make_pair(
                    buddyAddress, buddyAddress +
                    2 * (pow(2, n))));

                std::cout << "Coalescing of blocks starting at "
                    << buddyAddress << " and "
                    << id << " was done" << "\n";
            }
            arr[n].erase(arr[n].begin() + i);
            arr[n].erase(arr[n].begin() +
                arr[n].size() - 1);
            break;
        }
    }

    // Удалить существование ключа
    mp.erase(id);
}

int main()
{
    Buddy(128);
    allocate(16);
    allocate(17);
    allocate(16);
    allocate(16);
    deallocate(0);
    deallocate(9);
    deallocate(32);
    deallocate(16);

    return 0;
}

/*
Output for the above code:
Memory from 0 to 15 allocated
Memory from 16 to 31 allocated
Memory from 32 to 47 allocated
Memory from 48 to 63 allocated
Memory block from 0 to 15 freed
Sorry, invalid free request
Memory block from 32 to 47 freed
Memory block from 16 to 31 freed
Coalescing of blocks starting at 0 and 16 was done
*/