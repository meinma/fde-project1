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
    //customers : 28,
    // orders: 268
    // lineitem_quantities: 50
    //Try of using unordered set instead of vectors
    const std::unordered_set<int>customerKeys = getCustomerIds(std::move(segmentParam));
    int customerSize = customerKeys.size();
    const std::unordered_set<int>orderKeys = getOrderIds(customerKeys);
    int orderSize = orderKeys.size();
    //Until here it's right
    const std::multiset<float>quantities = getLineitemQuantities(orderKeys);
    int quantitySize = quantities.size();
    u_int64_t sum = 0;
    for (std::multiset<float>::const_iterator it = quantities.begin(); it != quantities.end(); ++it){
        sum += *it;
    }
    return (sum *100) / quantities.size();
}

//--------------------------------------------------------------------------
std::multiset<float>JoinQuery::getLineitemQuantities(const std::unordered_set<int>orderKeys){
    std::ifstream stream;
    int counter = 0;
    assert(stream);
    std::string line;
    std::multiset<float>quantities;
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
                quantities.insert(std::stof(quantity));
            }
        }
    }
    return quantities;
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
