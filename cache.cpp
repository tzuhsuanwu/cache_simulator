#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <bitset>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <sstream>
#include <stdio.h>

using namespace std;

map<string, string> direct_cache;
map<string, deque<string> > fourway_FIFO_cache;
map<string, deque<string> > fourway_LRU_cache;
map<string, deque<vector<string> > > fourway_LFU_cache;
deque<string> fully_FIFO_cache;
deque<string> fully_LRU_cache;
deque<vector<string> > fully_LFU_cache;


string hex_to_binary(char);
int direct_cache_input(map<string, string> &, string , string);
int fourway_FIFO_cache_input(map<string, deque<string> > &, string, string);
int fourway_LRU_cache_input(map<string, deque<string> > &, string, string);
int fourway_LFU_cache_input(map<string, deque<vector<string> > > &, string, string);
int fully_FIFO_cache_input(deque<string> &, int, string);
int fully_LRU_cache_input(deque<string> &, int, string);
int fully_LFU_cache_input(deque<vector<string> > &, int, string);

int main(int argc, char *argv[])
{
    FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen(argv[2], "w");
	char buffer[100] = {0};

    int cache_size, block_size, associativity, replace_algo;
    int number_of_block, index_width, tag_width, word_offset_width, byte_offset_width;
    string str, address_hex;

    fgets(buffer, 50, input_file);
    cache_size = atoi(buffer);
    fgets(buffer, 50, input_file);
    block_size = atoi(buffer);
    fgets(buffer, 50, input_file);
    associativity = atoi(buffer);
    fgets(buffer, 50, input_file);
    replace_algo = atoi(buffer);
    index_width = log2(number_of_block);
    
    number_of_block = (cache_size * 1024) / block_size;
    
    if (associativity == 0){
        index_width = log2(number_of_block);
    }
    else if (associativity == 1)
    {
        index_width = log2(number_of_block / 4);
    }
    else
    {
        index_width = 0;
    }
    
    
    if (block_size / 4 < 2)
    {
        word_offset_width = 0;
    }
    else
    {
        word_offset_width = log2(block_size / 4);
    }
    if (log2(block_size) < 3)
    {
        byte_offset_width = log2(block_size);
    }
    else
    {
        byte_offset_width = 2;
    }

    tag_width = 32 - index_width - word_offset_width - byte_offset_width;

    /*
    cout << "cache_size: " << cache_size << endl;
    cout << "block_size: " << block_size << endl;
    cout << "associativity: " << associativity << endl;
    cout << "replace_algo: " << replace_algo << endl;
    cout << "number_of_block: " << number_of_block << endl;
    cout << "index_width: " << index_width << endl;
    cout << "tag_width: " << tag_width << endl;
    cout << "word_offset_width: " << word_offset_width << endl;
    cout << "byte_offset_width: " << byte_offset_width << endl;
    */
    

    while (fgets(buffer, 50, input_file))
    {
        string index, tag;
        string address_binary, address_hex = buffer;
        
        for (int i = 2; i < 10; i++)
            address_binary += hex_to_binary(buffer[i]);
        for (int i = 0; i < tag_width; i++)
            tag += address_binary[i];
        for (int i = tag_width; i < (tag_width + index_width); i++)
            index += address_binary[i];
        if (associativity == 0)
        {
            int temp_output = direct_cache_input(direct_cache, index, tag);
            fprintf(output_file, "%d\n", temp_output);
        }
        else if (associativity == 1)
        {
            if (replace_algo == 0)
            {
                int temp_output = fourway_FIFO_cache_input(fourway_FIFO_cache, index, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
            else if (replace_algo == 1)
            {
                int temp_output = fourway_LRU_cache_input(fourway_LRU_cache, index, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
            else
            {
                int temp_output = fourway_LFU_cache_input(fourway_LFU_cache, index, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
        }
        else if (associativity == 2)
        {
            if (replace_algo == 0)
            {
                int temp_output = fully_FIFO_cache_input(fully_FIFO_cache, number_of_block, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
            else if (replace_algo == 1)
            {
                int temp_output = fully_LRU_cache_input(fully_LRU_cache, number_of_block, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
            else
            {
                int temp_output = fully_LFU_cache_input(fully_LFU_cache, number_of_block, tag);
                fprintf(output_file, "%d\n", temp_output);
            }
        }
        
    }
    
    fclose(input_file);
    fclose(output_file);
}

int direct_cache_input(map<string, string> &direct_cache, string index, string tag)
{
    map<string, string>::iterator itr = direct_cache.find(index);
    if (itr == direct_cache.end())
    {
        direct_cache[index] = tag;
        return -1;
    }
    else
    {
        if (itr->second == tag)
        {
            return -1;
        }
        else
        {
            bitset<32> victim(itr->second);
            itr->second = tag;
            return victim.to_ulong();
        }
    }
}

int fourway_FIFO_cache_input(map<string, deque<string> > &fourway_FIFO_cache, string index, string tag)
{
    map<string, deque<string> >::iterator itr = fourway_FIFO_cache.find(index);
    int hit = 0;
    if (itr == fourway_FIFO_cache.end())
    {
        fourway_FIFO_cache[index].push_back(tag);
        return -1;
    }
    else if (fourway_FIFO_cache[index].size() < 4)
    {
        for (int i = 0; i < fourway_FIFO_cache[index].size(); i++)
        {
            if (fourway_FIFO_cache[index][i] == tag)
            {
                hit = 1;
            }
        }
        if (hit == 0)
        {
            fourway_FIFO_cache[index].push_back(tag);
        }
        return -1;        
    }
    else if (fourway_FIFO_cache[index].size() == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            if (fourway_FIFO_cache[index][i] == tag)
            {
                hit = 1;
            }
        }
        if (hit == 0)
        {
            bitset<32> victim(fourway_FIFO_cache[index].front());
            fourway_FIFO_cache[index].pop_front();
            fourway_FIFO_cache[index].push_back(tag);
            return victim.to_ulong();
        }
        else
        {
            return -1;
        }
    }
}

int fourway_LRU_cache_input(map<string, deque<string> > &fourway_LRU_cache, string index, string tag)
{
    map<string, deque<string> >::iterator itr = fourway_LRU_cache.find(index);
    int hit = 0;
    if (itr == fourway_LRU_cache.end())
    {
        fourway_LRU_cache[index].push_back(tag);
        return -1;
    }
    else if (fourway_LRU_cache[index].size() < 4)
    {
        for (int i = 0; i < fourway_LRU_cache[index].size(); i++)
        {
            if (fourway_LRU_cache[index][i] == tag)
            {
                hit = 1;
                fourway_LRU_cache[index].erase(fourway_LRU_cache[index].begin() + i);
                fourway_LRU_cache[index].push_back(tag);
                return -1;
            }
        }
        if (hit == 0)
        {
            fourway_LRU_cache[index].push_back(tag);
            return -1;
        }
    }
    else if (fourway_LRU_cache[index].size() == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            if (fourway_LRU_cache[index][i] == tag)
            {
                hit = 1;
                fourway_LRU_cache[index].erase(fourway_LRU_cache[index].begin() + i);
                fourway_LRU_cache[index].push_back(tag);
                return -1;
            }
        }
        if (hit == 0)
        {
            bitset<32> victim(fourway_LRU_cache[index].front());
            fourway_LRU_cache[index].pop_front();
            fourway_LRU_cache[index].push_back(tag);
            return victim.to_ulong();
        }
    }
}

int fourway_LFU_cache_input(map<string, deque<vector<string> > > &fourway_LFU_cache, string index, string tag)
{
    map<string, deque<vector<string> > >::iterator itr = fourway_LFU_cache.find(index);
    int hit = 0;
    if (itr == fourway_LFU_cache.end())
    {
        vector<string> temp;
        bitset<32> initial_counter(1);
        temp.push_back(tag);
        temp.push_back(initial_counter.to_string());
        fourway_LFU_cache[index].push_back(temp);
        return -1;
    }
    else if (fourway_LFU_cache[index].size() < 4)
    {
        for (int i = 0; i < fourway_LFU_cache[index].size(); i++)
        {
            if (fourway_LFU_cache[index][i][0] == tag)
            {
                hit = 1;
                bitset<32> old_count(fourway_LFU_cache[index][i][1]);
                bitset<32> new_count(old_count.to_ulong() + 1);
                fourway_LFU_cache[index][i][1] = new_count.to_string();
                return -1;
            }
        }
        if (hit == 0)
        {
            vector<string> temp;
            bitset<32> initial_counter(1);
            temp.push_back(tag);
            temp.push_back(initial_counter.to_string());
            fourway_LFU_cache[index].push_back(temp);
            return -1;
        }
    }
    else if (fourway_LFU_cache[index].size() == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            if (fourway_LFU_cache[index][i][0] == tag)
            {
                hit = 1;
                bitset<32> old_count(fourway_LFU_cache[index][i][1]);
                bitset<32> new_count(old_count.to_ulong() + 1);
                fourway_LFU_cache[index][i][1] = new_count.to_string();
                return -1;
            }
        }
        if (hit == 0)
        {
            int small = 0;
            int deque_index = 0;
            for (int i = 0; i < 4; i++)
            {
                bitset<32> temp(fourway_LFU_cache[index][i][1]);
                if (small == 0)
                {
                    small = temp.to_ulong();
                }
                else
                {
                    if (small > temp.to_ulong())
                    {
                        small = temp.to_ulong();
                        deque_index = i;
                    }
                }
            }
            deque<vector<string> >::iterator itr = fourway_LFU_cache[index].begin();
            bitset<32> victim(fourway_LFU_cache[index][deque_index][0]);
            fourway_LFU_cache[index].erase(itr + deque_index);
            vector<string> temp;
            bitset<32> initial_counter(1);
            temp.push_back(tag);
            temp.push_back(initial_counter.to_string());
            fourway_LFU_cache[index].push_back(temp);
            return victim.to_ulong();
        }
    }
}

int fully_FIFO_cache_input(deque<string> &fully_FIFO_cache, int number_of_block, string tag)
{
    int hit = 0;
    if (fully_FIFO_cache.size() < number_of_block)
    {
        for (int i = 0; i < fully_FIFO_cache.size(); i++)
        {
            if (fully_FIFO_cache[i] == tag)
            {
                hit = 1;
                return -1;
                break;
            }
        }
        if (hit == 0)
        {
            fully_FIFO_cache.push_back(tag);
            cout << -1 << endl;
        }
    }
    else if (fully_FIFO_cache.size() == number_of_block)
    {
        for (int i = 0; i < fully_FIFO_cache.size(); i++)
        {
            if (fully_FIFO_cache[i] == tag)
            {
                hit = 1;
                return -1;
                break;
            }
        }
        if (hit == 0)
        {
            bitset<32> victim(fully_FIFO_cache.front());
            return victim.to_ulong();
            fully_FIFO_cache.pop_front();
            fully_FIFO_cache.push_back(tag);
        }
    }
}

int fully_LRU_cache_input(deque<string> &fully_LRU_cache, int number_of_block, string tag)
{
    int hit = 0;
    if (fully_LRU_cache.size() < number_of_block)
    {
        for (int i = 0; i < fully_LRU_cache.size(); i++)
        {
            if (fully_LRU_cache[i] == tag)
            {
                hit = 1;
                fully_LRU_cache.erase(fully_LRU_cache.begin() + i);
                fully_LRU_cache.push_back(tag);
                return -1;
            }
        }
        if (hit == 0)
        {
            fully_LRU_cache.push_back(tag);
            return -1;
        }
    }
    else if (fully_LRU_cache.size() == number_of_block)
    {
        for (int i = 0; i < fully_LRU_cache.size(); i++)
        {
            if (fully_LRU_cache[i] == tag)
            {
                hit = 1;
                fully_LRU_cache.erase(fully_LRU_cache.begin() + i);
                fully_LRU_cache.push_back(tag);
                return -1;
            }
        }
        if (hit == 0)
        {
            bitset<32> victim(fully_LRU_cache.front());
            fully_LRU_cache.pop_front();
            fully_LRU_cache.push_back(tag);
            return victim.to_ulong();
        }
    }
}

int fully_LFU_cache_input(deque<vector<string> > &fully_LFU_cache, int number_of_block, string tag)
{
    int hit = 0;
    if (fully_LFU_cache.size() < number_of_block)
    {
        for (int i = 0; i < fully_LFU_cache.size(); i++)
        {
            if (fully_LFU_cache[i][0] == tag)
            {
                hit = 1;
                bitset<32> old_count(fully_LFU_cache[i][1]);
                bitset<32> new_count(old_count.to_ulong() + 1);
                fully_LFU_cache[i][1] = new_count.to_string();
                return -1;
            }
        }
        if (hit == 0)
        {
            vector<string> temp;
            bitset<32> initial_counter(1);
            temp.push_back(tag);
            temp.push_back(initial_counter.to_string());
            fully_LFU_cache.push_back(temp);
            return -1;
        }
    }
    else if (fully_LFU_cache.size() == number_of_block)
    {
        for (int i = 0; i < fully_LFU_cache.size(); i++)
        {
            if (fully_LFU_cache[i][0] == tag)
            {
                hit = 1;
                bitset<32> old_count(fully_LFU_cache[i][1]);
                bitset<32> new_count(old_count.to_ulong() + 1);
                fully_LFU_cache[i][1] = new_count.to_string();
                return -1;
            }
        }
        if (hit == 0)
        {
            int small = 0;
            int deque_index = 0;
            for (int i = 0; i < 4; i++)
            {
                bitset<32> temp(fully_LFU_cache[i][1]);
                if (small == 0)
                {
                    small = temp.to_ulong();
                }
                else
                {
                    if (small > temp.to_ulong())
                    {
                        small = temp.to_ulong();
                        deque_index = i;
                    }
                }
            }
            deque<vector<string> >::iterator itr = fully_LFU_cache.begin();
            bitset<32> victim(fully_LFU_cache[deque_index][0]);
            fully_LFU_cache.erase(itr + deque_index);
            vector<string> temp;
            bitset<32> initial_counter(1);
            temp.push_back(tag);
            temp.push_back(initial_counter.to_string());
            fully_LFU_cache.push_back(temp);
            return victim.to_ulong();
        }
    }
}
string hex_to_binary(char input)
{
    if (input >= 'A' && input <= 'Z')
        input = input + 32;

    if (input == '0')
        return "0000";
    if (input == '1')
        return "0001";
    if (input == '2')
        return "0010";
    if (input == '3')
        return "0011";
    if (input == '4')
        return "0100";
    if (input == '5')
        return "0101";
    if (input == '6')
        return "0110";
    if (input == '7')
        return "0111";
    if (input == '8')
        return "1000";
    if (input == '9')
        return "1001";
    if (input == 'a')
        return "1010";
    if (input == 'b')
        return "1011";
    if (input == 'c')
        return "1100";
    if (input == 'd')
        return "1101";
    if (input == 'e')
        return "1110";
    if (input == 'f')
        return "1111";
}