#include "JoinQuery.hpp"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <future>
#include <algorithm>

//---------------------------------------------------------------------------
JoinQuery::JoinQuery(std::string new_lineitem, std::string new_orders,
                     std::string new_customer):lineitem(std::move(new_lineitem)),orders(std::move(new_orders)),customer(std::move(new_customer))
{}
//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam)
{
    u_int64_t customerLength = lineCount(this->customer);
    u_int64_t orderLength = lineCount(this->orders);
    //u_int64_t lineitemLength = lineCount(this->lineitem);

    // Threads für CustomerTable
    /* Funktioniert ist aber langsam
    auto f1 = std::async(&JoinQuery::getCustomerIds,this,0,customerLength,segmentParam);
    auto f11 = std::async(&JoinQuery::getCustomerIds,this,customerLength/2,customerLength,segmentParam);
    const std::unordered_set<int> customers1 = f1.get();
    const std::unordered_set<int> customers2 = f11.get();
    std::unordered_set<int>finalCustomers;
    std::merge(customers1.begin(),customers1.end(),customers2.begin(),customers2.end(),std::inserter(finalCustomers,finalCustomers.begin()));

    auto f2 = std::async(&JoinQuery::getOrderIds,this,0,orderLength/2,finalCustomers);
    auto f21  = std::async(&JoinQuery::getOrderIds,this,orderLength/2,orderLength,finalCustomers);
    const std::unordered_set<int>orders1 = f2.get();
    const std::unordered_set<int>orders2 = f21.get();
    std::unordered_set<int>finalOrders;
    std::merge(orders1.begin(),orders1.end(),orders2.begin(),orders2.end(),std::inserter(finalOrders,finalOrders.begin()));
    auto f3 = std::async(&JoinQuery::getLineitemQuantities,this,finalOrders);
    const u_int64_t quantity = f3.get();
    return quantity;
    */

     // 2. schnellerer Versuch mit Threads
    auto f1 = std::async(&JoinQuery::getCustomerIds,this,0,customerLength/2,segmentParam);
    auto customerkeys1 = f1.get();
    auto f2 = std::async(&JoinQuery::getCustomerIds,this,customerLength/2,customerLength,segmentParam);
    auto customerkeys2 = f2.get();
    //Threads für OrderTable Ergebnisse sind noch korrekt
    auto f3 = std::async(&JoinQuery::getOrderIds,this,0,orderLength/2,customerkeys1);
    auto f4 = std::async(&JoinQuery::getOrderIds,this,orderLength/2,orderLength,customerkeys1);
    auto f5 = std::async(&JoinQuery::getOrderIds,this,0,orderLength/2,customerkeys2);
    auto f6 = std::async(&JoinQuery::getOrderIds,this,orderLength/2,orderLength,customerkeys2);

    //Threads für LineitemQuantities

    auto f7 = std::async(&JoinQuery::getLineitemQuantities,this,f3.get());
    auto f8 = std::async(&JoinQuery::getLineitemQuantities,this,f4.get());
    auto f9 = std::async(&JoinQuery::getLineitemQuantities,this,f5.get());
    auto f10 = std::async(&JoinQuery::getLineitemQuantities,this,f6.get());

    std::vector<u_int64_t> erg1 = f7.get();
    std::vector<u_int64_t> erg2 = f8.get();
    std::vector<u_int64_t> erg3 = f9.get();
    std::vector<u_int64_t> erg4 = f10.get();

    u_int64_t sum = erg1[0] + erg2[0] + erg3[0] + erg4[0];
    u_int64_t counter = erg1[1] + erg2[1] + erg3[1] + erg4[1];
    return sum/counter;


}


//Optimieren durch Weglassen des letzten Multisets und nur der Rückgabe der Summe und der Anzahl an Zeilen.
// Dadurch Einsparen einer for-Schleife
//--------------------------------------------------------------------------
std::vector<u_int64_t>JoinQuery::getLineitemQuantities(const std::unordered_set<int>orderKeys){
    std::ifstream stream;
    std::vector <u_int64_t>summary;
    u_int64_t counter = 0;
    u_int64_t sum = 0;
    assert(stream);
    std::string line;
    stream.open(this->lineitem,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string quantity;
        while (std::getline(stream,line)){
            std::stringstream linestream(line);
            std::getline(linestream, orderId, '|');
            auto search = orderKeys.find(std::stoi(orderId));
            if (search != orderKeys.end()) {
                counter++;
                for (int i = 0; i < 4; i++)
                    std::getline(linestream, quantity,'|');
                sum += std::stod(quantity);
            }
        }
    }
    summary.push_back(sum*100);
    summary.push_back(counter);
    return summary;
    //return (sum *100) /counter;
}

//---------------------------------------------------------------------------

std::unordered_set<int> JoinQuery::getOrderIds(u_int64_t start,u_int64_t end, const std::unordered_set<int> customerKeys){
    std::ifstream  stream;
    assert(stream);
    std::string line;
    std::unordered_set<int>orderKeys;
    stream.open(this->orders,std::ios::in);
    if (stream.is_open()){
        std::string orderId;
        std::string customerId;
        if (start != 0){
            for (u_int64_t jmp = 0; jmp < start; jmp++)
                std::getline(stream,line);
        }
        for (; start < end; start++){
            std::getline(stream,line);
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
std::unordered_set<int> JoinQuery::getCustomerIds(u_int64_t start, u_int64_t end, const std::string segmentParam){
    std::ifstream stream;
    assert(stream);
    std::string line;
    std::unordered_set<int>customerKeys;
    stream.open(this->customer,std::ios::in);
    if (stream.is_open()){
        std::string id;
        std::string segment;
        if (start != 0){
            for(u_int64_t jmp = 0; jmp < start; jmp++)
                std::getline(stream,line);
        }
        for(u_int64_t i = start; i < end; i++){
            std::getline(stream,line);
            std::stringstream linestream (line);
            std::getline(linestream,id,'|'); //save id
            for (int j  = 0; j < 5; ++j)
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
