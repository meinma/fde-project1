#include "JoinQuery.hpp"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>


//---------------------------------------------------------------------------
JoinQuery::JoinQuery(std::string new_lineitem, std::string new_orders,
                     std::string new_customer) : lineitem(std::move(new_lineitem)), orders(std::move(new_orders)),
                                                 customer(std::move(new_customer)) {
}

//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam) {

    const std::unordered_set<int> customerKeys = getCustomerIds(std::move(segmentParam));
    const std::unordered_set<int> orderKeys = getOrderIds(customerKeys);
    return getLineitemQuantities(orderKeys);

}

//--------------------------------------------------------------------------
/*
    Compute the average quantity of all lineitems with an orderId, which is contained in orderKeys
 */
u_int64_t JoinQuery::getLineitemQuantities(const std::unordered_set<int> orderKeys) {
    std::ifstream stream;
    u_int64_t counter = 0;
    u_int64_t sum = 0;
    std::string line;
    stream.open(this->lineitem, std::ios::in);
    if (stream.is_open()) {
        int orderId;
        std::string quantityString = "";
        while (std::getline(stream, line)) {
            // Extract orderId
            const char *data = line.data(), *limit = data + line.length(), *last = data;
            unsigned field = 0;
            for (auto iter = data; iter != limit; ++iter) {
                if ((*iter) == '|') {
                    // OrderId is 1st field of the line
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

            // Extract quantity
            const char *data2 = line.data(), *limit2 = data + line.length(), *last2 = data;
            unsigned field2 = 0;
            quantityString.clear();
            for (auto iter3 = data2; iter3 != limit2; ++iter3) {
                if ((*iter3) == '|') {
                    // Quantity is 5th field of the linne
                    if (++field2 == 5) {
                        for (auto iter4 = last2; iter4 != iter3; ++iter4) {
                            quantityString.push_back(*iter4);
                        }
                        break;
                    } else
                        last2 = iter3 + 1;
                }
            }
            // If OrderId is in the given orderkeys, increase counter and add quantity to the sum of quanitites
            auto search = orderKeys.find(orderId);
            if (search != orderKeys.end()) {
                counter++;
                sum += std::stod(quantityString);
            }
        }
    }
    return (sum * 100) / counter;
}

//---------------------------------------------------------------------------
/*
    Returns unordered set of orderids, which belong to any customer of the given customerkeys in the argument

 */
std::unordered_set<int> JoinQuery::getOrderIds(const std::unordered_set<int> customerKeys) {
    std::ifstream stream;
    std::string line;
    std::unordered_set<int> orderKeys;
    stream.open(this->orders, std::ios::in);
    if (stream.is_open()) {
        int orderId;
        int customerId;
        while (std::getline(stream, line)) {
            // Extract the orderId per line 
            const char *data = line.data(), *limit = data + line.length(), *last = data;
            unsigned field = 0;
            for (auto iter = data; iter != limit; ++iter) {
                if ((*iter) == '|') {
                    // OrderId is the first field of the line
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
            // Extract the customerId per line
            const char *data2 = line.data(), *limit2 = data + line.length(), *last2 = data;
            unsigned field2 = 0;
            for (auto iter3 = data2; iter3 != limit2; ++iter3) {
                if ((*iter3) == '|') {
                    // customerId is the 2nd field of the line
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
            // If customerId is in the given customerkeys, add orderId to orderkeys,
            // which will be returned in the end
            auto search = customerKeys.find(customerId);
            if (search != customerKeys.end())
                orderKeys.insert(orderId);
        }
    }
    stream.close();
    return orderKeys;
}

//---------------------------------------------------------------------------
/*
    returns unordered_set with all customerKeys containing the given Marketsegment  
 */
std::unordered_set<int> JoinQuery::getCustomerIds(const std::string segmentParam) {
    std::ifstream stream;
    std::string line;
    std::unordered_set<int> customerKeys;
    stream.open(this->customer, std::ios::in);
    if (stream.is_open()) {
        int id;
        std::string segment;
        // Read file liny by line
        while (std::getline(stream, line)) {
            
            // extracts customerId,
            const char *data = line.data(), *limit = data + line.length(), *last = data;
            unsigned field = 0;
            for (auto iter = data; iter != limit; ++iter) {
                if ((*iter) == '|') {
                    // customerId is the first field of the line
                    if (++field == 1) {
                        union {
                            unsigned v;
                            char buffer[sizeof(unsigned)];
                        };
                        v = 0;
                        for (auto iter2 = last; iter2 != iter; ++iter2)
                            v = 10 * v + (*iter2) - '0';
                        id = v;
                        break;
                    } else
                        last = iter + 1;
                }
            }

            // extracts marketsegment from line
            const char *data2 = line.data(), *limit2 = data + line.length(), *last2 = data;
            unsigned field2 = 0;
            segment.clear();
            for (auto iter3 = data2; iter3 != limit2; ++iter3) {
                if ((*iter3) == '|') {
                    // marketsemgent is the 7th field of the line
                    if (++field2 == 7) {
                        for (auto iter4 = last2; iter4 != iter3; ++iter4) {
                            segment.push_back(*iter4);
                        }
                        break;
                    } else
                        last2 = iter3 + 1;
                }
            }
            // If the given marketsegment corresponds to the marketsegment of current line,
            // add customerId to unorderedset of customerIds
            if (segment == segmentParam)
                customerKeys.insert(id);

        }
    }
    stream.close();
    return customerKeys;
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
