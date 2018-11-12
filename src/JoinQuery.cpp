#include "JoinQuery.hpp"
#include <assert.h>
#include <fstream>
#include <thread>
#include <vector>
#include <sstream>


//---------------------------------------------------------------------------
JoinQuery::JoinQuery(std::string new_lineitem, std::string new_orders,
                     std::string new_customer):lineitem(std::move(new_lineitem)),orders(std::move(new_orders)),customer(std::move(new_customer))
{}
//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam)
{
    // Erst customer einlesen und mit segmentParam vergleichen
    // 1st try save customkeys of customers with segmentParam as segment
    const std::vector<int>customerKeys = getCustomerIds(std::move(segmentParam));
    const std::vector<int>orderKeys = getOrderIds(customerKeys);
    const std::vector<float>quantities = getLineitemQuantities(orderKeys);
    int size = quantities.size();
    float sum = 0;
    for (std::vector<float>::const_iterator it = quantities.begin(); it != quantities.end(); ++it){
       sum += *it;
    }
    return (sum / quantities.size()) * 100;
}

//--------------------------------------------------------------------------
std::vector<float>JoinQuery::getLineitemQuantities(const std::vector<int>orderKeys){
    std::ifstream stream;
    assert(stream);
    std::string line;
    std::vector<float>quantities;
    stream.open(this->lineitem,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string quantity;
        while (std::getline(stream,line)){
            std::stringstream linestream(line);
            std::getline(linestream,orderId,'|');
            // Ab hier Korrekturen
            for (int i = 0; i < 4; i++)
                std::getline(linestream,quantity,'|');
            for (std::vector<int>::const_iterator it = orderKeys.begin(); it != orderKeys.end(); ++it){
                if (std::stoi(orderId) == *it){
                    quantities.push_back(std::stof(quantity));
                    break;
                }
            }
        }
        return quantities;
    }

}

//contains Beziehung statt for-Schleife




//---------------------------------------------------------------------------

std::vector<int> JoinQuery::getOrderIds(const std::vector<int> customerKeys){
    std::ifstream  stream;
    assert(stream);
    std::string line;
    std::vector<int>orderkeys;
    stream.open(this->orders,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string customerId;
        while (std::getline(stream,line)){
            std::stringstream linestream(line);
            std::getline(linestream,orderId,'|');
            std::getline(linestream,customerId,'|');
            for (std::vector<int>::const_iterator it = customerKeys.begin(); it != customerKeys.end(); ++it){
                if (std::stoi(customerId) == *it) {
                    orderkeys.push_back(std::stoi(orderId));
                    break;
                }
            }
        }
        return orderkeys;
    }
    stream.close();

}

//---------------------------------------------------------------------------
std::vector<int> JoinQuery::getCustomerIds(const std::string segmentParam){
    std::ifstream stream;
    assert(stream);
    std::string line;
    std::vector<int>customerkeys;
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
                customerkeys.push_back(std::stoi(id));
            }
        }
    }
    stream.close();
    return customerkeys;
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
