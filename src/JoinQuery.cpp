#include "JoinQuery.hpp"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>


//---------------------------------------------------------------------------
JoinQuery::JoinQuery(std::string new_lineitem, std::string new_orders,
                     std::string new_customer) : lineitem(std::move(new_lineitem)), orders(std::move(new_orders)),
                                                 customer(std::move(new_customer)),customerKeys(new std::unordered_set<int>()),orderKeys(new std::unordered_set<int>()) {

}

//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam) {
    /*
    const std::unordered_set<int> customerKeys = getCustomerIds(std::move(segmentParam));
    const std::unordered_set<int> orderKeys = getOrderIds(customerKeys);
    return getLineitemQuantities(orderKeys);
     */
    getCustomerIds(std::move(segmentParam));
    getOrderIds();
    return getLineitemQuantities();
}


//Optimieren durch Weglassen des letzten Multisets und nur der Rückgabe der Summe und der Anzahl an Zeilen.
// Dadurch Einsparen einer for-Schleife
//--------------------------------------------------------------------------
u_int64_t JoinQuery::getLineitemQuantities() {
    std::ifstream stream;
    u_int64_t counter = 0;
    u_int64_t sum = 0;
    std::string line;
    stream.open(this->lineitem, std::ios::in);
    if (stream.is_open()) {
        std::string orderId;
        std::string quantity;
        while (std::getline(stream, line)) {
            std::stringstream linestream(line);
            std::getline(linestream, orderId, '|');
            auto search = this->orderKeys->find(std::stoi(orderId));
            if (search != this->orderKeys->end()) {
                counter++;
                for (int i = 0; i < 4; i++)
                    std::getline(linestream, quantity, '|');
                sum += std::stod(quantity);
            }
        }
    }
    this->orderKeys->clear();
    this->customerKeys->clear();
    return (sum * 100) / counter;
}

//---------------------------------------------------------------------------
void JoinQuery::getOrderIds() {
    std::ifstream stream;
    std::string line;
    //std::unordered_set<int> orderKeys;
    stream.open(this->orders, std::ios::in);
    if (stream.is_open()) {
        int orderId;
        int customerId;
        while (std::getline(stream, line)) {
            const char *data = line.data(), *limit = data + line.length(), *last = data;
            unsigned field = 0;
            for (auto iter = data; iter != limit; ++iter) {
                if ((*iter) == '|') {
                    if (++field == 1) {
                        union {
                            unsigned v;
                            char buffer[sizeof(unsigned)];
                        };
                        v = 0;
                        for (auto iter2 = last; iter2 != iter; ++iter2)
                            v = 10 * v + (*iter2) - '0';
                        orderId = v;
                        break;
                    } else
                        last = iter + 1;
                }
            }
            const char *data2 = line.data(), *limit2 = data + line.length(), *last2 = data;
            unsigned field2 = 0;
            for (auto iter3 = data2; iter3 != limit2; ++iter3) {
                if ((*iter3) == '|') {
                    if (++field2 == 2) {
                        union {
                            unsigned w;
                            char buffer2[sizeof(unsigned)];
                        };
                        w = 0;
                        for (auto iter4 = last2; iter4 != iter3; ++iter4)
                            w = 10 * w + (*iter4) - '0';
                        customerId = w;
                        break;
                    } else
                        last2 = iter3 + 1;
                }
            }
            auto search = this->customerKeys->find(customerId);
            if (search != this->customerKeys->end())
                this->orderKeys->insert(orderId);
        }
    }
    stream.close();
    //return orderKeys;
}

//---------------------------------------------------------------------------
//This is working

//Replace std::getline
void JoinQuery::getCustomerIds(const std::string segmentParam) {
    std::ifstream stream;
    std::string line;
    //std::unordered_set<int> customerKeys;
    stream.open(this->customer, std::ios::in);
    if (stream.is_open()) {
        std::string  id;
        std::string segment;
        while (std::getline(stream, line)) {
                std::stringstream linestream(line);
                std::getline(linestream, id, '|'); //save id
                for (int i = 0; i < 5; ++i)
                    std::getline(linestream, segment, '|'); //Skip the next five elements
                std::getline(linestream, segment, '|'); //save the segment
                if (segment == segmentParam) {
                    this->customerKeys->insert(std::stoi(id));
                }
        }
    }
    stream.close();
    //return customerKeys;
}

//---------------------------------------------------------------------------
size_t JoinQuery::lineCount(std::string rel) {
    std::ifstream relation(rel);
    assert(relation);  // make sure the provided string references a file
    size_t n = 0;
    for (std::string line; std::getline(relation, line);) n++;
    return n;
}
//---------------------------------------------------------------------------
