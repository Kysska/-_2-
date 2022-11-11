#include <iostream>
#include <vector>
#include<map>

// размер вектора пар
int size;

//Ãëîáàëüíûé âåêòîð ïàð äëÿ îòñëåæèâàíèÿ âñåõ ñâîáîäíûõ óçëîâ ðàçëè÷íûõ ðàçìåðîâ
std::vector<std::pair<int, int>> arr[100000];

// Map äëÿ õðàíåíèÿ íà÷àëüíîãî àäðåñà â êà÷åñòâå êëþ÷à è ðàçìåðà âûäåëåííîé ÿ÷åéêè â êà÷åñòâå çíà÷åíèÿ
std::map<int, int> mp;

void Buddy(int s)
{

    // ìàêñèìàëüíûé ðàçìåð ñòåïåíè äâîéêè ÿ÷åéêè
    int n = ceil(log(s) / log(2));

    size = n + 1;
    for (int i = 0; i <= n; i++)
        arr[i].clear();

    // Èçíà÷àëüíî äîñòóïåí âåñü áëîê óêàçàííîãî ðàçìåðà
    arr[n].push_back(std::make_pair(0, s - 1));
}

void allocate(int s)
{

    // Âû÷èñëåíèå èíäåêñà â ñâîáîäíîì ñïèñêå äëÿ ïîèñêà áëîêà, åñëè òàêîâîé èìååòñÿ
    int x = ceil(log(s) / log(2));
    // Äîñòóïíûé áëîê 
    if (arr[x].size() > 0)
    {
        std::pair<int, int> temp = arr[x][0];

        // Óäàëÿåì áëîê èç ñïèñêà ñâîáîäíûõ áëîêîâ
        arr[x].erase(arr[x].begin());

        std::cout << "Memory from " << temp.first
            << " to " << temp.second
            << " allocated" << "\n";

        // Ñîïîñòàâüòå íà÷àëüíûé àäðåñ ñ ðàçìåðîì, ÷òîáû óïðîñòèòü îñâîáîæäåíèå
        mp[temp.first] = temp.second -
            temp.first + 1;
    }
    else
    {
        int i;
        std::cout << size;

        // Åñëè íåò, íàéäèòå áëîê áîëüøåãî ðàçìåðà 
        for (i = x + 1; i < size; i++)
        {

            // Íàéòè ðàçìåð áëîêà, ïðåâûøàþùèé çàïðîñ 
            if (arr[i].size() != 0)
                break;
        }

        //Åñëè òàêîé áëîê íå íàéäåí, ò.å. íåò äîñòóïíîãî áëîêà ïàìÿòè
        if (i == size)
        {
            std::cout << "Sorry, failed to allocate memory\n";
        }

        // Åñëè íàøåë
        else
        {
            std::pair<int, int> temp;
            temp = arr[i][0];

            // Óäàëèòå ïåðâûé áëîê, ÷òîáû ðàçäåëèòü åãî íà ïîëîâèíêè
            arr[i].erase(arr[i].begin());
            i--;

            for (; i >= x; i--)
            {

                // Ðàçäåëÿåì áëîê íà äâå ïîëîêèíêè
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

                // Äîáàâëÿåì â ñïèñîê ñâîáîäíûõ áëîêîâ
                arr[i].push_back(pair2);
                temp = arr[i][0];

                //Óäàëèòå ïåðâûé ñâîáîäíûé áëîê äëÿ àëëîöèðîâàíèÿ   
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

    // Åñëè òàêîé íà÷àëüíûé àäðåñ íåäîñòóïåí
    if (mp.find(id) == mp.end())
    {
        std::cout << "invalid free request\n";
        return;
    }

    // Ðàçìåð áëîêà äëÿ ïîèñêà
    int n = ceil(log(mp[id]) / log(2));

    int i, buddyNumber, buddyAddress;

    // Äîáàâüòå áëîê â ñïèñîê ñâîáîäíûõ
    arr[n].push_back(std::make_pair(id,
        id + pow(2, n) - 1));
    std::cout << "Memory block from " << id
        << " to " << id + pow(2, n) - 1
        << " freed\n";

    // Âû÷èñëèòü íîìåð ïðèÿòåëÿ
    buddyNumber = id / mp[id];

    if (buddyNumber % 2 != 0)
        buddyAddress = id - pow(2, n);
    else
        buddyAddress = id + pow(2, n);

    // Íàéäèòå â ñâîáîäíîì ñïèñêå, ÷òîáû íàéòè åãî ïðèÿòåëÿ
    for (i = 0; i < arr[n].size(); i++)
    {

        // Åñëè ïðèÿòåëü íàéäåí è òîæå ñâîáîäåí
        if (arr[n][i].first == buddyAddress)
        {

            // îáúåäèíÿåì äðóçåé, ÷òîáû ñäåëàòü èõ îäíèì áîëüøèì áëîêîì ñâîáîäíîé ïàìÿòè
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

    // Óäàëèòü ñóùåñòâîâàíèå êëþ÷à
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
