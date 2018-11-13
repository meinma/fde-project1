#include "JoinQuery.hpp"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>


//---------------------------------------------------------------------------
JoinQuery::JoinQuery(std::string new_lineitem, std::string new_orders,
                     std::string new_customer):lineitem(std::move(new_lineitem)),orders(std::move(new_orders)),customer(std::move(new_customer))
{}
//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam)
{
    const std::unordered_set<int>customerKeys = getCustomerIds(std::move(segmentParam));
    const std::unordered_set<int>orderKeys = getOrderIds(customerKeys);
    const uint64_t quantity = getLineitemQuantities(orderKeys);
    return quantity;
}


//Optimieren durch Weglassen des letzten Multisets und nur der Rückgabe der Summe und der Anzahl an Zeilen.
// Dadurch Einsparen einer for-Schleife
//--------------------------------------------------------------------------
u_int64_t JoinQuery::getLineitemQuantities(const std::unordered_set<int>orderKeys){
    std::ifstream stream;
    u_int64_t counter = 0;
    u_int64_t sum = 0;
    assert(stream);
    std::string line;
    stream.open(this->lineitem,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string quantity;
        while (std::getline(stream,line)) {
            std::stringstream linestream(line);
            std::getline(linestream, orderId, '|');
            auto search = orderKeys.find(std::stoi(orderId));
            if (search != orderKeys.end()) {
                counter++;
                for (int i = 0; i < 4; i++)
                    std::getline(linestream, quantity,'|');
                sum += std::stod(quantity) *100;
            }
        }
    }
    return sum/counter;
}

//---------------------------------------------------------------------------

std::unordered_set<int> JoinQuery::getOrderIds(const std::unordered_set<int> customerKeys){
    std::ifstream  stream;
    assert(stream);
    std::string line;
    std::unordered_set<int>orderKeys;
    stream.open(this->orders,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string customerId;
        while (std::getline(stream,line)){
            std::stringstream linestream(line);
            std::getline(linestream,orderId,'|');
            std::getline(linestream,customerId,'|');
            auto search = customerKeys.find(std::stoi(customerId));
            if (search != customerKeys.end())
                orderKeys.insert(std::stoi(orderId));
        }
    }
    stream.close();
    return orderKeys;
}

//---------------------------------------------------------------------------
//This is working
std::unordered_set<int> JoinQuery::getCustomerIds(const std::string segmentParam){
    std::ifstream stream;
    assert(stream);
    std::string line;
    std::unordered_set<int>customerKeys;
    stream.open(this->customer,std::ios::in);
    if (stream.is_open()){
        std::string id;
        std::string segment;
        while(std::getline(stream,line)){
            std::stringstream linestream (line);
            std::getline(linestream,id,'|'); //save id
            for (int i  = 0; i < 5; ++i)
                std::getline(linestream,segment,'|'); //Skip the next five elements
            std::getline(linestream,segment,'|'); //save the segment
            if (segment == segmentParam){
                customerKeys.insert(std::stoi(id));
            }
        }
    }
    stream.close();
    return customerKeys;
}
//---------------------------------------------------------------------------
size_t JoinQuery::lineCount(std::string rel)
{
    std::ifstream relation(rel);
    assert(relation);  // make sure the provided string references a file
    size_t n = 0;
    for (std::string line; std::getline(relation, line);) n++;
    return n;
}
//---------------------------------------------------------------------------
